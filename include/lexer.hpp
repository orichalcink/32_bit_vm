#ifndef LEXER_HPP
#define LEXER_HPP

#include "catcher.hpp"
#include <unordered_set>
#include <cstdint>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using namespace std::string_literals;

// Keywords used in the language
inline const std::unordered_set<std::string> keywords
{
   "ADD"s, "SUB"s, "MUL"s, "DIV"s, "REM"s, "AND"s, "OR"s, "XOR"s, "NOT"s,
   "NEG"s, "BR"s, "BRn"s, "BRz"s, "BRp"s, "BRzp"s, "BRpz"s, "BRnp"s, "BRpn"s,
   "BRnz"s, "BRzn"s, "BRnzp"s, "BRnpz"s, "BRznp"s, "BRzpn"s, "BRpnz"s,
   "BRpzn"s, "JMP"s, "RET"s, "HALT"s
};

// Registers used in the language
inline const std::unordered_set<std::string> regis_words
{
   "R0"s, "R1"s, "R2"s, "R3"s, "R4"s, "R5"s, "R6"s, "R7"s, "R8"s, "R9"s,
   "R10"s, "R11"s, "R12"s, "R13"s, "R14"s, "R15"s
};

// Tokens used in the lexer
struct Token
{
   enum class Type : std::uint8_t
   {
      keyword, identifier, regis, number, label, comma, colon, eof
   };

   Type type;
   std::string lexeme;
};

// Lexer tokenizes a string into tokens used by the parser, but in our case
// the tokens are first put into a translator to translate labels into memory
// addresses and only parsed and placed into the VM's memory afterwards
class Lexer
{
public:
   // Constructors
   Lexer(Catcher& catcher, const fs::path& path)
      : catcher(catcher), path(path) {}
   ~Lexer() = default;

   // Tokenize the file into tokens
   std::vector<Token>& tokenize()
   {
      std::ifstream file(path);

      if (!file.is_open())
      {
         catcher.insert("Failed to open file '"s + path.string() + "'."s);
         return tokens;
      }

      std::string line;
      while (std::getline(file, line))
      {
         for (size_t index = 0; index < line.size(); ++index)
         {
            if (index >= line.size()) break;

            char ch = line.at(index);

            if (ch == ' ' || ch == '\t' || ch == '\r')
               continue;
            else if (ch == '\n' || ch == ';')
               break;
            else if (ch == ',')
               tokens.push_back({Token::Type::comma, ","s});
            else if (ch == ':')
               tokens.push_back({Token::Type::colon, ":"s});
            else if (std::isalpha(ch) || ch == '_')
            {
               std::string keyword;

               for (; index < line.size(); ++index)
               {
                  ch = line.at(index);
                  
                  if ((!std::isalnum(ch) && ch != '_') || std::isspace(ch))
                  {
                     --index;
                     break;
                  }
                  keyword.push_back(ch);
               }

               if (keywords.count(keyword))
                  tokens.push_back({Token::Type::keyword, keyword});
               else if (regis_words.count(keyword))
                  tokens.push_back({Token::Type::regis, keyword});
               else
                  tokens.push_back({Token::Type::identifier, keyword});
            }
            else if (std::isdigit(ch) || (ch == '-' && index + 1 < line.size() && std::isdigit(line.at(index + 1))))
            {
               std::string number;
               bool hex = false;
               bool bin = false;

               if (ch == '-')
               {
                  number = '-';
                  ++index;
                  ch = line.at(index);
               }

               // Binary
               if (ch == '0' && index + 1 < line.size() && std::tolower(line.at(index + 1)) == 'b')
               {
                  bin = true;
                  index += 2;
               }

               // Hexadecimal
               if (ch == '0' && index + 1 < line.size() && std::tolower(line.at(index + 1)) == 'x')
               {
                  hex = true;
                  index += 2;
               }

               for (; index < line.size(); ++index)
               {
                  ch = line.at(index);
                  
                  if (ch == '\'')
                     continue;

                  if (bin && ch != '0' && ch != '1' && std::isdigit(ch))
                  {
                     catcher.insert("Invalid binary format, expected '0' or '1', but got '"s + ch + "' instead."s);
                     return tokens;
                  }

                  if (hex && (!std::isdigit(ch) && (!std::isalpha(ch) || std::tolower(ch) > 'f' || std::tolower(ch) < 'a')))
                  {
                     catcher.insert("Invalid hex format, expected '0' to 'F', but got '"s + ch + "' instead."s);
                     return tokens;
                  }

                  if ((!hex && !std::isdigit(ch)) || (!hex && std::isalpha(ch)) || std::isspace(ch))
                  {
                     --index;
                     break;
                  }
                  number.push_back(ch);
               }

               if ((bin || hex) && number.size() == 0)
                  number += '0';
               
               tokens.push_back({Token::Type::number, (!hex && !bin ? number : 
                  std::to_string(std::stoi(number, nullptr, (bin ? 2 : 16))))});
            }
            else
               catcher.insert("Unexpected character '"s + ch + "' while tokenizing."s);
         }
      }
      file.close();

      tokens.push_back({Token::Type::eof, ""s});
      return tokens;
   }

private:
   Catcher& catcher;
   fs::path path;
   std::vector<Token> tokens;
};

#endif // LEXER_HPP
