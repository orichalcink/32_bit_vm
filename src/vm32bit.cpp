#include "executor.hpp"
#include "parser.hpp"
#include "translator.hpp"

// Project by chalcinxx
// https://www.youtube.com/playlist?list=PLAYMpoWModGOzP_LNhaJDvMbUxX_9OI90

// Resources:
// Command documentation for 16bit opcodes
// https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf
// Guide for creating your own LC-3 16bit virtual machine (lexer, parser not included!)
// https://www.jmeiners.com/lc3-vm/

// This virtual machine has memory and a CPU that executes some basic
// instructions. It first reads a file, turns the contents into tokens
// for parsing, translates the labels into memory addresses, parses
// the tokens into instructions and places them into memory and, finally,
// executes the instructions one by one.
// The commands can be found in opcodes.hpp file, where their bit size and
// functions are documented.

int main()
{
   while (true)
   {
      // Get file from the user
      std::cout << "Input the name of the file (Q to quit): ";
      std::string file;
      std::getline(std::cin, file);

      if (file == "q"s || file == "Q"s)
      {
         std::cout << "Quitting...\n"s;
         break;
      }

      Catcher catcher;

      if (!fs::is_regular_file(file))
      {
         catcher.insert("File '"s + file + "' could not be opened or found."s);
         catcher.display();
         continue;
      }

      // Tokenize the file contents
      Lexer lexer (catcher, file);
      auto& tokens = lexer.tokenize();

      if (catcher.display()) continue;

      // Replace labels with memory addresses
      Translator translator (catcher, tokens);
      translator.translate();

      if (catcher.display()) continue;

      // Parse tokens into instructions and place them in memory
      Parser parser (catcher, tokens);
      parser.parse();

      if (catcher.display()) continue;

      // Execute instructions one by one
      Executor executor;
      executor.execute();

      std::cout << reg[R_R0] << std::endl;
      std::cout << reg[R_R1] << std::endl;
      std::cout << reg[R_R2] << std::endl;
   }
   return 0;
}