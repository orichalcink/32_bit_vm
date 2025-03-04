#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"
#include "memory.hpp"
#include "register.hpp"

// Parse the tokens and construct the instructions. Instructions get loaded
// into memory, which are then executed by the executor
class Parser
{
public:
   // Constructors
   Parser(Catcher& catcher, std::vector<Token>& tokens)
      : catcher(catcher), tokens(tokens) {}
   ~Parser() = default;

   // Parse the tokens
   void parse()
   {
      while (!is(Token::Type::eof))
      {
         // Handle directives as a unique case
         if (is(Token::Type::directive))
         {
            handle_directives();
            continue;
         }

         // Most valid lines start with a keyword. Labels are an exception
         // because they all get deleted by the translator.
         if (quit_flag || check(Token::Type::keyword))
            return;

         const std::string& lexeme = tokens.at(index).lexeme;

         // Match the command
         if (lexeme == "ADD"s)
            parse_imm17_opcode(0b000001);
         else if (lexeme == "SUB"s)
            parse_imm17_opcode(0b000010);
         else if (lexeme == "MUL"s)
            parse_imm17_opcode(0b000011);
         else if (lexeme == "DIV"s)
            parse_imm17_opcode(0b000100);
         else if (lexeme == "REM"s)
            parse_imm17_opcode(0b000101);
         else if (lexeme == "AND"s)
            parse_imm17_opcode(0b000110);
         else if (lexeme == "OR"s)
            parse_imm17_opcode(0b000111);
         else if (lexeme == "XOR"s)
            parse_imm17_opcode(0b001000);
         else if (lexeme == "NOT"s)
            parse_unary_opcode(0b001001);
         else if (lexeme == "NEG"s)
            parse_unary_opcode(0b001010);
         else if (lexeme.substr(0, 2) == "BR"s)
            parse_br_opcode(lexeme);
         else if (lexeme == "JMP"s)
            parse_jmp_opcode();
         else if (lexeme == "RET"s)
            parse_ret_opcode();
         else if (lexeme == "JSR"s)
            parse_jsr_opcode();
         else if (lexeme == "JSRR"s)
            parse_jsrr_opcode();
         else if (lexeme == "LD"s)
            parse_ld_opcode(0b001110);
         else if (lexeme == "LDI"s)
            parse_ld_opcode(0b001111);
         else if (lexeme == "LDR"s)
            parse_ldr_opcode(0b010000);
         else if (lexeme == "LEA"s)
            parse_ld_opcode(0b010001);
         else if (lexeme == "ST"s)
            parse_ld_opcode(0b010010);
         else if (lexeme == "STI"s)
            parse_ld_opcode(0b010011);
         else if (lexeme == "STR"s)
            parse_ldr_opcode(0b010100);
         else if (lexeme == "HALT"s)
            parse_halt_opcode();
         else check(Token::Type::eof);

         if (quit_flag) return;
      }

      // Always add a HALT command at the end
      memory.at(memory_index) = 0b111111;
   }

   void parse_imm17_opcode(std::uint32_t opcode)
   {
      std::uint32_t instr = opcode;
      
      advance();
      std::uint8_t dr = get_register();
      instr |= (dr & 0b1111) << 7;

      advance();
      if (check(Token::Type::comma)) return;

      advance();
      std::uint8_t sr1 = get_register();
      instr |= (sr1 & 0b1111) << 11;

      advance();
      if (check(Token::Type::comma)) return;
      advance();

      if (is(Token::Type::number, Token::Type::label))
      {
         std::int32_t number = std::stoi(tokens.at(index).lexeme);
         instr |= (number & 0b11111111111111111) << 15;
         instr |= 0b1 << 6;
      }
      else
      {
         std::uint8_t sr2 = get_register();
         instr |= (sr2 & 0b1111) << 15;
         instr |= 0b0 << 6;
      }
      advance();
      insert(instr);
   }

   void parse_unary_opcode(std::uint32_t opcode)
   {
      std::uint32_t instr = opcode;

      advance();
      std::uint8_t dr = get_register();
      instr |= (dr & 0b1111) << 6;

      advance();
      if (check(Token::Type::comma)) return;

      advance();
      std::uint8_t sr = get_register();
      instr |= (sr & 0b1111) << 10;

      advance();
      insert(instr);
   }

   void parse_br_opcode(const std::string& lexeme)
   {
      std::uint32_t instr = 0b001011;

      bool n = (lexeme.find("n") != lexeme.npos);
      bool z = (lexeme.find("z") != lexeme.npos);
      bool p = (lexeme.find("p") != lexeme.npos);

      if (!n && !z && !p) n = z = p = true;

      instr |= n << 6;
      instr |= z << 7;
      instr |= p << 8;

      advance();
      if (check(Token::Type::number, Token::Type::label)) return;

      std::int32_t pc_offset23 = std::stoi(tokens.at(index).lexeme);
      if (is(Token::Type::label))
         pc_offset23 -= memory_index + 1;

      instr |= (pc_offset23 & 0b11111111111111111111111) << 9;

      advance();
      insert(instr);
   }

   void parse_jmp_opcode()
   {
      std::uint32_t instr = 0b001100;
      advance();

      std::uint8_t base_r = get_register();
      instr |= (base_r & 0b1111) << 6;

      advance();
      insert(instr);
   }

   void parse_ret_opcode()
   {
      advance();
      std::uint32_t instr = 0b001100;
      instr |= 0b1111 << 6;
      insert(instr);
   }

   void parse_jsr_opcode()
   {
      std::uint32_t instr = 0b001101;
      advance();

      if (check(Token::Type::number, Token::Type::label)) return;

      std::int32_t pc_offset25 = std::stoi(tokens.at(index).lexeme);
      if (is(Token::Type::label))
         pc_offset25 -= memory_index + 1;
      
      instr |= (pc_offset25 & 0b1111111111111111111111111) << 7;

      advance();
      insert(instr);
   }

   void parse_jsrr_opcode()
   {
      std::uint32_t instr = 0b001101;
      advance();

      instr |= 0b1 << 6;

      std::uint8_t base_r = get_register();
      instr |= (base_r & 0b1111) << 7;

      advance();
      insert(instr);
   }

   void parse_ld_opcode(std::uint32_t opcode)
   {
      std::uint32_t instr = opcode;
      advance();

      std::uint8_t dr = get_register();
      instr |= (dr & 0b1111) << 6;

      advance();
      if (check(Token::Type::comma)) return;

      advance();
      if (check(Token::Type::number, Token::Type::label)) return;

      std::int32_t pc_offset22 = std::stoi(tokens.at(index).lexeme);
      if (is(Token::Type::label))
         pc_offset22 -= memory_index;

      instr |= (pc_offset22 & 0b1111111111111111111111) << 10;
      
      advance();
      insert(instr);
   }

   void parse_ldr_opcode(std::uint32_t opcode)
   {
      std::uint32_t instr = opcode;
      advance();

      std::uint8_t dr = get_register();
      instr |= (dr & 0b1111) << 6;

      advance();
      if (check(Token::Type::comma)) return;

      advance();
      std::uint8_t base_r = get_register();
      instr |= (base_r & 0b1111) << 10;

      advance();
      if (check(Token::Type::comma)) return;

      advance();
      if (check(Token::Type::number, Token::Type::label)) return;

      std::int32_t pc_offset18 = std::stoi(tokens.at(index).lexeme);
      if (is(Token::Type::label))
         pc_offset18 -= memory_index;

      instr |= (pc_offset18 & 0b111111111111111111) << 14;

      advance();
      insert(instr);
   }

   void parse_halt_opcode()
   {
      advance();
      insert(0b111111);
   }

   void handle_directives()
   {
      const std::string& lexeme = tokens.at(index).lexeme;

      if (lexeme == ".ORG"s)
      {
         advance();
         if (check(Token::Type::number)) return;

         std::uint16_t destination = std::stoi(tokens.at(index).lexeme);
         reg.at(R_PC) = destination;
         memory_index = destination;

         advance();
      }
      else if (lexeme == ".WORD"s)
      {
         advance();
         if (check(Token::Type::number, Token::Type::label)) return;

         std::int32_t value = std::stoi(tokens.at(index).lexeme);
         writeMemory(memory_index, value);
         ++memory_index;

         advance();
      }
      else if (lexeme == ".END"s)
      {
         memory.at(memory_index) = 0b111111;
         quit_flag = true;
         return;
      }
   }

   void advance()
   {
      if (index < tokens.size()) ++index;
   }

   bool check(Token::Type type)
   {
      if (tokens.at(index).type != type)
      {
         catcher.insert("Unexpected token while parsing: '"s + tokens.at(index).lexeme + "'."s);
         quit_flag = true;
      }
      return quit_flag;
   }

   bool check(Token::Type type1, Token::Type type2)
   {
      if (tokens.at(index).type != type1 && tokens.at(index).type != type2)
      {
         catcher.insert("Unexpected token while parsing: '"s + tokens.at(index).lexeme + "'."s);
         quit_flag = true;
      }
      return quit_flag;
   }

   bool is(Token::Type type)
   {
      return tokens.at(index).type == type;
   }

   bool is(Token::Type type1, Token::Type type2)
   {
      return tokens.at(index).type == type1 || tokens.at(index).type == type2;
   }

   void insert(std::uint32_t instr)
   {
      if (memory_index < memory.size())
      {
         memory.at(memory_index) = instr;
         ++memory_index;
      }
   }

   std::uint8_t get_register()
   {
      if (!is(Token::Type::regis))
         catcher.insert("Unexpected token while parsing: '"s + tokens.at(index).lexeme + "'. Expected register."s);
      return std::stoi(tokens.at(index).lexeme.substr(1));
   }

private:
   Catcher& catcher;
   std::vector<Token>& tokens;
   size_t memory_index = pcStart;
   size_t index = 0;
   bool quit_flag = false;
};

#endif // PARSER_HPP
