#ifndef OPCODES_HPP
#define OPCODES_HPP

#include "memory.hpp"
#include "register.hpp"
#include <cstdint>
#include <cstdio>

// ADD DR, SR1, SR2
// 0-5    6        7-10 11-14 15-18
// 000001 imm_flag DR   SR1   SR2
//
// ADD DR, SR1, imm17
// 0-5    6         7-10 11-14 15-31
// 000001 imm_flag, DR   SR1   imm17
//
// Both registers get added together and result is stored in DR, condition
// codes are set based on the result.
inline void opcode_add(std::uint32_t instr)
{
   bool imm_flag    = (instr >> 6)  & 0b1;
   std::uint8_t dr  = (instr >> 7)  & 0b1111;
   std::uint8_t sr1 = (instr >> 11) & 0b1111;

   if (imm_flag)
   {
      std::int32_t imm17 = sext((instr >> 15) & 0b11111111111111111, 17);
      reg.at(dr) = reg.at(sr1) + imm17;
   }
   else
   {
      std::uint8_t sr2 = (instr >> 15) & 0b1111;
      reg.at(dr) = reg.at(sr1) + reg.at(sr2);
   }
   update_flags(dr);
}

// SUB DR, SR1, SR2
// 0-5    6        7-10 11-14 15-18
// 000010 imm_flag DR   SR1   SR2
//
// SUB DR, SR1, imm17
// 0-5    6         7-10 11-14 15-31
// 000010 imm_flag, DR   SR1   imm17
//
// SR1 gets subtracted by SR2/imm17 and result is stored in DR, condition
// codes are set based on the result.
inline void opcode_sub(std::uint32_t instr)
{
   bool imm_flag    = (instr >> 6)  & 0b1;
   std::uint8_t dr  = (instr >> 7)  & 0b1111;
   std::uint8_t sr1 = (instr >> 11) & 0b1111;

   if (imm_flag)
   {
      std::int32_t imm17 = sext((instr >> 15) & 0b11111111111111111, 17);
      reg.at(dr) = reg.at(sr1) - imm17;
   }
   else
   {
      std::uint8_t sr2 = (instr >> 15) & 0b1111;
      reg.at(dr) = reg.at(sr1) - reg.at(sr2);
   }
   update_flags(dr);
}

// MUL DR, SR1, SR2
// 0-5    6        7-10 11-14 15-18
// 000011 imm_flag DR   SR1   SR2
//
// MUL DR, SR1, imm17
// 0-5    6         7-10 11-14 15-31
// 000011 imm_flag, DR   SR1   imm17
//
// Both registers get multiplied together and result is stored in DR, condition
// codes are set based on the result.
inline void opcode_mul(std::uint32_t instr)
{
   bool imm_flag    = (instr >> 6)  & 0b1;
   std::uint8_t dr  = (instr >> 7)  & 0b1111;
   std::uint8_t sr1 = (instr >> 11) & 0b1111;

   if (imm_flag)
   {
      std::int32_t imm17 = sext((instr >> 15) & 0b11111111111111111, 17);
      reg.at(dr) = reg.at(sr1) * imm17;
   }
   else
   {
      std::uint8_t sr2 = (instr >> 15) & 0b1111;
      reg.at(dr) = reg.at(sr1) * reg.at(sr2);
   }
   update_flags(dr);
}

// DIV DR, SR1, SR2
// 0-5    6        7-10 11-14 15-18
// 000100 imm_flag DR   SR1   SR2
//
// DIV DR, SR1, imm17
// 0-5    6         7-10 11-14 15-31
// 000100 imm_flag, DR   SR1   imm17
//
// SR1 gets divided by SR2/imm17 and result is stored in DR, condition
// codes are set based on the result.
inline void opcode_div(std::uint32_t instr)
{
   bool imm_flag    = (instr >> 6)  & 0b1;
   std::uint8_t dr  = (instr >> 7)  & 0b1111;
   std::uint8_t sr1 = (instr >> 11) & 0b1111;

   if (imm_flag)
   {
      std::int32_t imm17 = sext((instr >> 15) & 0b11111111111111111, 17);
      reg.at(dr) = (imm17 == 0 ? 0 : reg.at(sr1) / imm17);
   }
   else
   {
      std::uint8_t sr2 = (instr >> 15) & 0b1111;
      reg.at(dr) = (reg.at(sr2) == 0 ? 0 : reg.at(sr1) / reg.at(sr2));
   }
   update_flags(dr);
}

// REM DR, SR1, SR2
// 0-5    6        7-10 11-14 15-18
// 000101 imm_flag DR   SR1   SR2
//
// REM DR, SR1, imm17
// 0-5    6         7-10 11-14 15-31
// 000101 imm_flag, DR   SR1   imm17
//
// SR1 gets divided by SR2/imm17 and the remainder is stored in DR, condition
// codes are set based on the result.
inline void opcode_rem(std::uint32_t instr)
{
   bool imm_flag    = (instr >> 6)  & 0b1;
   std::uint8_t dr  = (instr >> 7)  & 0b1111;
   std::uint8_t sr1 = (instr >> 11) & 0b1111;

   if (imm_flag)
   {
      std::int32_t imm17 = sext((instr >> 15) & 0b11111111111111111, 17);
      reg.at(dr) = (imm17 == 0 ? 0 : reg.at(sr1) % imm17);
   }
   else
   {
      std::uint8_t sr2 = (instr >> 15) & 0b1111;
      reg.at(dr) = (reg.at(sr2) == 0 ? 0 : reg.at(sr1) % reg.at(sr2));
   }
   update_flags(dr);
}

// AND DR, SR1, SR2
// 0-5    6        7-10 11-14 15-18
// 000110 imm_flag DR   SR1   SR2
//
// AND DR, SR1, imm17
// 0-5    6        7-10 11-14 15-31
// 000110 imm_flag DR   SR1   imm17
//
// Perform a bitwise and operation on SR1 and SR2/imm17 and store it in DR,
// condition codes are set based on the result.
inline void opcode_and(std::uint32_t instr)
{
   bool imm_flag    = (instr >> 6)  & 0b1;
   std::uint8_t dr  = (instr >> 7)  & 0b1111;
   std::uint8_t sr1 = (instr >> 11) & 0b1111;

   if (imm_flag)
   {
      std::int32_t imm17 = sext((instr >> 15) & 0b11111111111111111, 17);
      reg.at(dr) = reg.at(sr1) & imm17;
   }
   else
   {
      std::uint8_t sr2 = (instr >> 15) & 0b1111;
      reg.at(dr) = reg.at(sr1) & reg.at(sr2);
   }
   update_flags(dr);
}

// OR DR, SR1, SR2
// 0-5    6        7-10 11-14 15-18
// 000111 imm_flag DR   SR1   SR2
//
// OR DR, SR1, imm17
// 0-5    6        7-10 11-14 15-31
// 000111 imm_flag DR   SR1   imm17
//
// Perform a bitwise or operation on SR1 and SR2/imm17 and store it in DR,
// condition codes are set based on the result.
inline void opcode_or(std::uint32_t instr)
{
   bool imm_flag    = (instr >> 6)  & 0b1;
   std::uint8_t dr  = (instr >> 7)  & 0b1111;
   std::uint8_t sr1 = (instr >> 11) & 0b1111;

   if (imm_flag)
   {
      std::int32_t imm17 = sext((instr >> 15) & 0b11111111111111111, 17);
      reg.at(dr) = reg.at(sr1) | imm17;
   }
   else
   {
      std::uint8_t sr2 = (instr >> 15) & 0b1111;
      reg.at(dr) = reg.at(sr1) | reg.at(sr2);
   }
   update_flags(dr);
}

// XOR DR, SR1, SR2
// 0-5    6        7-10 11-14 15-18
// 001000 imm_flag DR   SR1   SR2
//
// XOR DR, SR1, imm17
// 0-5    6        7-10 11-14 15-31
// 001000 imm_flag DR   SR1   imm17 
//
// Perform a bitwise xor operation on SR1 and SR2/imm17 and store it in DR,
// condition codes are set based on the result.
inline void opcode_xor(std::uint32_t instr)
{
   bool imm_flag    = (instr >> 6)  & 0b1;
   std::uint8_t dr  = (instr >> 7)  & 0b1111;
   std::uint8_t sr1 = (instr >> 11) & 0b1111;

   if (imm_flag)
   {
      std::int32_t imm17 = sext((instr >> 15) & 0b11111111111111111, 17);
      reg.at(dr) = reg.at(sr1) ^ imm17;
   }
   else
   {
      std::uint8_t sr2 = (instr >> 15) & 0b1111;
      reg.at(dr) = reg.at(sr1) ^ reg.at(sr2);
   }
   update_flags(dr);
}

// NOT DR, SR
// 0-5    6-9 10-13
// 001001 DR  SR
//
// Perform a bitwise not operation on SR and store the result in DR, condition
// codes are set based on result.
inline void opcode_not(std::uint32_t instr)
{
   std::uint8_t dr = (instr >> 6)  & 0b1111;
   std::uint8_t sr = (instr >> 10) & 0b1111;
   reg.at(dr) = ~reg.at(sr);
   update_flags(dr);
}

// NEG DR, SR
// 0-5    6-9 10-13
// 001010 DR  SR
//
// Negate the value in SR and store the result in DR, condition codes are set
// based on result.
inline void opcode_neg(std::uint32_t instr)
{
   std::uint8_t dr = (instr >> 6)  & 0b1111;
   std::uint8_t sr = (instr >> 10) & 0b1111;
   reg.at(dr) = -reg.at(sr);
   update_flags(dr);
}

// BR(nzp) LABEL
// 0-5    6 7 8 9-31
// 001011 n z p PCoffset23
//
// Branch relative to the current program counter or to the given label if the
// given condition code is true.
inline void opcode_br(std::uint32_t instr)
{
   std::int32_t pc_offset23 = sext((instr >> 9) & 0b11111111111111111111111, 23);
   std::uint8_t nzp         = (instr >> 6) & 0b111;
   reg.at(R_PC) += ((nzp & reg.at(R_COND)) ? pc_offset23 : 0);
}

// JMP BaseR
// 0-5    6-9
// 001100 BaseR
//
// RET
// 0-5    6-9
// 001100 1111
//
// Unconditionally jump to the value stored in BaseR register.
inline void opcode_jmp(std::uint32_t instr)
{
   std::uint8_t base_r = (instr >> 6) & 0b1111;
   reg.at(R_PC) = (base_r == 15 ? reg.at(base_r) : reg.at(base_r) - 1);
}

// JSR LABEL
// 0-5    6         7-31
// 001101 jsrr_flag PCoffset25
//
// JSRR BaseR
// 0-5    6         7-10
// 001101 jsrr_flag BaseR
//
// Firstly save program counter in register 15 and then unconditionally jump
// to the memory address of the label or address contained in the register.
inline void opcode_jsr(std::uint32_t instr)
{
   bool jsrr_flag = (instr >> 6) & 0b1;
   reg.at(R_R15) = reg.at(R_PC);

   if (jsrr_flag)
   {
      std::uint8_t base_r = (instr >> 7) & 0b1111;
      reg.at(R_PC) = reg.at(base_r) - 1;
   }
   else
   {
      std::int32_t pc_offset25 = sext((instr >> 7) & 0b1111111111111111111111111, 25);
      reg.at(R_PC) += pc_offset25;
   }
}

// LD DR, LABEL
// 0-5    6-9 10-31
// 001110 DR  PCoffset22
//
// Load contents of memory at the address into DR and set condition codes based
// on the loaded value.
inline void opcode_ld(std::uint32_t instr)
{
   std::uint8_t dr          = (instr >> 6) & 0b1111;
   std::int32_t pc_offset22 = sext((instr >> 10) & 0b1111111111111111111111, 22);
   reg.at(dr) = readMemory(reg.at(R_PC) + pc_offset22);
   update_flags(dr);
}

// LDI DR, LABEL
// 0-5    6-9 10-31
// 001111 DR  PCoffset22
//
// Load the address inside memory, place contents into DR and set and set
// condition codes based on loaded value.
inline void opcode_ldi(std::uint32_t instr)
{
   std::uint8_t dr          = (instr >> 6) & 0b1111;
   std::int32_t pc_offset22 = sext((instr >> 10) & 0b1111111111111111111111, 22);
   reg.at(dr) = readMemory(readMemory(reg.at(R_PC) + pc_offset22));
   update_flags(dr);
}

// LDR DR, BaseR, offset18
// 0-5    6-9 10-13 14-31
// 010000 DR  BaseR PCoffset18
//
// Load the contents of memory at the address into DR and set condition codes
// based on the loaded value.
inline void opcode_ldr(std::uint32_t instr)
{
   std::uint8_t dr          = (instr >> 6)  & 0b1111;
   std::uint8_t base_r      = (instr >> 10) & 0b1111;
   std::int32_t pc_offset18 = sext((instr >> 14) & 0b11111111111111, 14);

   reg.at(dr) = readMemory(reg.at(base_r) + pc_offset18);
   update_flags(dr);
}

// LEA DR, LABEL
// 0-5    6-9 10-31
// 010001 DR  PCoffset22
//
// Load the address of the label into DR and set condition codes based on the
// loaded value.
inline void opcode_lea(std::uint32_t instr)
{
   std::uint8_t dr          = (instr >> 6) & 0b1111;
   std::int32_t pc_offset22 = sext((instr >> 10) & 0b1111111111111111111111, 22);
   reg.at(dr) = reg.at(R_PC) + pc_offset22;
   update_flags(dr);
}

// ST SR, LABEL
// 0-5    6-9 10-31
// 010010 SR  PCoffset22
//
// Store the value in the register in the memory address of the label.
inline void opcode_st(std::uint32_t instr)
{
   std::uint8_t sr          = (instr >> 6) & 0b1111;
   std::int32_t pc_offset22 = sext((instr >> 10) & 0b1111111111111111111111, 22);
   writeMemory(reg.at(R_PC) + pc_offset22, reg.at(sr));
}

// STI SR, LABEL
// 0-5    6-9 10-31
// 010011 SR  PCoffset22
//
// Store the value in the register in the memory addres specified in the label.
inline void opcode_sti(std::uint32_t instr)
{
   std::uint8_t sr          = (instr >> 6) & 0b1111;
   std::int32_t pc_offset22 = sext((instr >> 10) & 0b1111111111111111111111, 22);
   writeMemory(readMemory(reg.at(R_PC) + pc_offset22), reg.at(sr));
}

// STR SR, BaseR, offset18
// 0-5    6-9 10-13 14-31
// 010100 SR  BaseR offset18
//
// Store the value in the SR register in the memory address found in the BaseR
// register plus the offset.
inline void opcode_str(std::uint32_t instr)
{
   std::uint8_t sr       = (instr >> 6)  & 0b1111;
   std::uint8_t base_r   = (instr >> 10) & 0b1111;
   std::int32_t offset18 = sext((instr >> 14) & 0b111111111111111111, 18);
   writeMemory(reg.at(base_r) + offset18, reg.at(sr));
}

#endif // OPCODES_HPP
