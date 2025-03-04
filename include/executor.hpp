#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include "opcodes.hpp"
#include <unordered_map>
#include <functional>

// List of all opcodes to easily execute them 
inline std::unordered_map<std::int16_t, std::function<void(std::uint32_t)>> opcode_list
{
   {1, opcode_add}, {2, opcode_sub}, {3, opcode_mul}, {4, opcode_div}, {5, opcode_rem},
   {6, opcode_and}, {7, opcode_or}, {8, opcode_xor}, {9, opcode_not}, {10, opcode_neg},
   {11, opcode_br}, {12, opcode_jmp}, {13, opcode_jsr}, {14, opcode_ld},
   {15, opcode_ldi}, {16, opcode_ldr}, {17, opcode_lea}, {18, opcode_st},
   {19, opcode_sti}, {20, opcode_str}
};


// Goes through the memory and executes all of the instructions until comes
// across the HALT command.
class Executor
{
public:
   // Constructors
   Executor() = default;
   ~Executor() = default;

   // Execute all of the instructions found in memory.
   void execute()
   {
      clear_registers();
      reg.at(R_PC) = pcStart;

      while (reg.at(R_PC) < maxMemory)
      {
         std::uint32_t instr = memory.at(reg.at(R_PC));

         // Halt command
         if (instr == 63)
            break;

         if (opcode_list.count(instr & 0b111111))
            opcode_list.at(instr & 0b111111)(instr);

         ++reg.at(R_PC);
      }
      pcStart = 0x3000;
   }
};

#endif // EXECUTOR_HPP
