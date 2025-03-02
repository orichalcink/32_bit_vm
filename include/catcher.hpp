#ifndef CATCHER_HPP
#define CATCHER_HPP

#include <iostream>
#include <string>
#include <vector>

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
   
   // Display all errors if there are any
   bool display()
   {
      if (errors.size() == 0)
         return false;
      
      std::cout << std::endl << errors.size() << " error" << (errors.size() == 1 ? "" : "s") << " occurred: \n";

      for (const auto& error : errors)
         std::cout << error << std::endl;
      
      std::cout << std::endl;
      return true;
   }

private:
   std::vector<std::string> errors;
};

#endif // CATCHER_HPP
