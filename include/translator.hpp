#ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include "lexer.hpp"
#include "register.hpp"
#include <algorithm>
#include <unordered_map>

// Translator finds all labels in the code and replaces them with their
// memory address.
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
               definitions[token.lexeme] = std::to_string(memory_index - 1);
               token.lexeme = tokens.at(index + 1).lexeme = "FLAG_FOR_DEL"s;
            }
            else
            {
               labels[token.lexeme] = index;
            }
         }
         else if (is(Token::Type::keyword))
            ++memory_index;

         if (index < tokens.size())
            ++index;
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
   std::unordered_map<std::string, size_t> labels;
   std::unordered_map<std::string, std::string> definitions;
   size_t memory_index = pcStart;
   size_t index = 0;
};

#endif // TRANSLATOR_HPP
