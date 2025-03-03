#ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include "lexer.hpp"
#include "register.hpp"
#include <algorithm>
#include <unordered_map>

// Store translated files to avoid infinite include loops
inline std::unordered_set<std::string> translated_files;

// Translator finds all labels in the code and replaces them with their
// memory address and handles includes
class Translator
{
public:
   // Constructors
   Translator(Catcher& catcher, std::vector<Token>& tokens)
      : catcher(catcher), tokens(tokens) {}
   ~Translator() = default;

   // Translate the tokens
   void translate()
   {
      // Find all definitions and labels
      while (!is(Token::Type::eof))
      {
         auto& token = tokens.at(index);

         if (is(Token::Type::identifier))
         {
            if (peek(Token::Type::colon) && definitions.count(token.lexeme))
               catcher.insert("Label '"s + token.lexeme + "' is already defined."s);
            else if (peek(Token::Type::colon))
            {
               definitions[token.lexeme] = std::to_string(memory_index);
               token.lexeme = tokens.at(index + 1).lexeme = "FLAG_FOR_DEL"s;
            }
            else
               labels.push_back({token.lexeme, index});
         }
         else if (is(Token::Type::directive) || is(Token::Type::keyword))
         {
            if (token.lexeme == ".ORG"s)
            {
               advance();
               
               if (is(Token::Type::number))
               {
                  memory_index = std::stoi(tokens.at(index).lexeme);
                  if (memory_index < pcStart)
                     pcStart = memory_index;
               }
               else
                  tokens.at(index).type = Token::Type::label;
               advance();
            }
            else if (token.lexeme == ".INCLUDE"s)
            {
               token.lexeme = "FLAG_FOR_DEL"s;
               advance();

               if (!is(Token::Type::string))
               {
                  catcher.insert("Expected string after '.INCLUDE' directive, got '"s + tokens.at(index).lexeme + "' instead."s);
                  return;
               }

               if (!fs::is_regular_file(tokens.at(index).lexeme))
               {
                  catcher.insert("File '"s + tokens.at(index).lexeme + "' could not be included as it cannot be opened or found."s);
                  return;
               }

               if (translated_files.count(tokens.at(index).lexeme))
               {
                  tokens.at(index).lexeme = "FLAG_FOR_DEL"s;
                  advance();
                  continue;
               }
               translated_files.insert(tokens.at(index).lexeme);

               std::string original = catcher.get_file();
               catcher.specify(tokens.at(index).lexeme);

               Lexer lexer (catcher, tokens.at(index).lexeme);
               auto& tokens2 = lexer.tokenize();

               if (catcher.any_errors())
                  return;

               Translator translator (catcher, tokens2);
               translator.translate();

               if (catcher.any_errors())
                  return;

               // pop off the EOF token to avoid duplicates
               if (!tokens2.empty())
                  tokens2.pop_back();

               catcher.specify(original);
               tokens.at(index).lexeme = "FLAG_FOR_DEL"s;
               tokens.insert(tokens.begin() + index + 1, tokens2.begin(), tokens2.end());
            }
            else
               ++memory_index;
         }

         advance();
      }

      // Replace all labels with their memory addresses
      for (auto& [label, ind] : labels)
      {
         if (!definitions.count(label))
            catcher.insert("Undefined label '"s + tokens.at(ind).lexeme + "' while translating."s);
         else
            tokens.at(ind) = {Token::Type::label, definitions.at(label)};
      }

      // Erase label definitions
      tokens.erase(std::remove_if(tokens.begin(), tokens.end(),
      [](const auto& t) -> bool
      {
         return t.lexeme == "FLAG_FOR_DEL"s;
      }), tokens.end());
   }

   void advance()
   {
      if (index < tokens.size()) ++index;
   }

   bool is(Token::Type type)
   {
      return tokens.at(index).type == type;
   }

   bool peek(Token::Type type)
   {
      return index + 1 < tokens.size() && tokens.at(index + 1).type == type;
   }

private:
   Catcher& catcher;
   std::vector<Token>& tokens;
   std::vector<std::pair<std::string, size_t>> labels;
   std::unordered_map<std::string, std::string> definitions;
   size_t memory_index = pcStart;
   size_t index = 0;
};

#endif // TRANSLATOR_HPP
