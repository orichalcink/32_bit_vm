#ifndef REGISTER_HPP
#define REGISTER_HPP

#include <array>
#include <cstdint>

// Start of the program counter
inline std::uint16_t pcStart = 0x3000; // ~12000 in hexadecimal

// Registers - 16 usable registers, program counter and condition register
enum Register : std::uint8_t
{
   R_R0,
   R_R1,
   R_R2,
   R_R3,
   R_R4,
   R_R5,
   R_R6,
   R_R7,
   R_R8,
   R_R9,
   R_R10,
   R_R11,
   R_R12,
   R_R13,
   R_R14,
   R_R15,
   R_PC,
   R_COND,
   R_COUNT // Register count
};

// Register storage
inline std::array<std::int32_t, R_COUNT> reg;

// Condition flags
enum class Flag : std::int8_t
{
   FL_P = 0b100, // Positive flag
   FL_Z = 0b010, // Zero flag
   FL_N = 0b001, // Negative flag
};

// Update condition flags
inline void update_flags(std::uint8_t r)
{
   reg.at(R_COND) = static_cast<std::int32_t>(
      reg.at(r) == 0 ? Flag::FL_Z : (reg.at(r) >> 31 ? Flag::FL_N : Flag::FL_P)
   );
}

// Clear all registers
inline void clear_registers()
{
   reg.fill(0);
}

#endif // REGISTER_HPP
