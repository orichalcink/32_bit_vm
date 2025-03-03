#ifndef CATCHER_HPP
#define CATCHER_HPP

#include <iostream>
#include <string>
#include <vector>

using namespace std::string_literals;

// Catcher holds all the errors inserted and displays them at will.
class Catcher
{
public:
   // Constructors
   Catcher() = default;
   ~Catcher() = default;

   // Insert a new error
   void insert(const std::string& error)
   {
      errors.push_back(error);
   }

   // Specify the file
   void specify(const std::string& file)
   {
      file_name = file;
   }

   std::string get_file() const
   {
      return file_name;
   }

   // Check if there are any errors.
   bool any_errors() const
   {
      return errors.size() > 0;
   }
   
   // Display all errors if there are any
   bool display()
   {
      if (errors.size() == 0)
         return false;
      
      std::cout << std::endl << errors.size() << " error" << (errors.size() == 1 ? "" : "s")
         << " occurred" << (file_name.empty() ? ":\n"s : " in file '"s + file_name + "': \n"s);

      for (const auto& error : errors)
         std::cout << error << std::endl;
      
      std::cout << std::endl;
      errors.clear();
      return true;
   }

private:
   std::string file_name;
   std::vector<std::string> errors;
};

#endif // CATCHER_HPP
