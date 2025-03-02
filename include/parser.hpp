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
         // All valid lines start with a keyword. Labels are an exception
         // because the all are deleted by the translator.
         check(Token::Type::keyword);

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
         else if (lexeme == "HALT"s)
            parse_halt_opcode();
         else check(Token::Type::eof);
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
      check(Token::Type::comma);

      advance();
      std::uint8_t sr1 = get_register();
      instr |= (sr1 & 0b1111) << 11;

      advance();
      check(Token::Type::comma);
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
      check(Token::Type::comma);

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
      check(Token::Type::number, Token::Type::label);

      std::int32_t pc_offset23 = std::stoi(tokens.at(index).lexeme);
      if (is(Token::Type::label))
         pc_offset23 -= memory_index;

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
      insert(0b0011001111);
   }

   void parse_halt_opcode()
   {
      advance();
      insert(0b111111);
   }

   void advance()
   {
      if (index < tokens.size()) ++index;
   }

   void check(Token::Type type)
   {
      if (tokens.at(index).type != type)
         catcher.insert("Unexpected token while parsing: '"s + tokens.at(index).lexeme + "'."s);
   }

   void check(Token::Type type1, Token::Type type2)
   {
      if (tokens.at(index).type != type1 && tokens.at(index).type != type2)
         catcher.insert("Unexpected token while parsing: '"s + tokens.at(index).lexeme + "'."s);
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
};

#endif // PARSER_HPP
