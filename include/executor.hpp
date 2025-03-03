#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include "opcodes.hpp"

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

         if ((instr & 0b111111) == 0b000001)
            opcode_add(instr);
         else if ((instr & 0b111111) == 0b000010)
            opcode_sub(instr);
         else if ((instr & 0b111111) == 0b000011)
            opcode_mul(instr);
         else if ((instr & 0b111111) == 0b000100)
            opcode_div(instr);
         else if ((instr & 0b111111) == 0b000101)
            opcode_rem(instr);
         else if ((instr & 0b111111) == 0b000110)
            opcode_and(instr);
         else if ((instr & 0b111111) == 0b000111)
            opcode_or(instr);
         else if ((instr & 0b111111) == 0b001000)
            opcode_xor(instr);
         else if ((instr & 0b111111) == 0b001001)
            opcode_not(instr);
         else if ((instr & 0b111111) == 0b001010)
            opcode_neg(instr);
         else if ((instr & 0b111111) == 0b001011)
            opcode_br(instr);
         else if ((instr & 0b111111) == 0b001100)
            opcode_jmp(instr);
         else if ((instr & 0b111111) == 0b001101)
            opcode_jsr(instr);
         else if ((instr & 0b111111) == 0b001110)
            opcode_ld(instr);
         else if ((instr & 0b111111) == 0b001111)
            opcode_ldi(instr);
         else if ((instr & 0b111111) == 0b010000)
            opcode_ldr(instr);
         else if ((instr & 0b111111) == 0b010001)
            opcode_lea(instr);
         else if ((instr & 0b111111) == 0b111111)
            break;

         ++reg.at(R_PC);
      }
      pcStart = 0x3000;
   }
};

#endif // EXECUTOR_HPP
