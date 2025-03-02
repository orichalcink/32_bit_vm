#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <array>

// Max memory of the virtual machine
inline constexpr std::size_t maxMemory = 1 << 16; // 65536
inline std::array<std::int32_t, maxMemory> memory;

// Write the value to the address
inline void writeMemory(std::uint16_t address, std::int32_t value)
{
   memory.at(address) = value;
}

// Read a value from the memory
inline std::int32_t readMemory(std::uint16_t address)
{
   return memory.at(address);
}

// Sign extend a number to 32 bits
inline std::int32_t sext(std::int32_t x, std::uint16_t bitCount)
{
   return ((x >> (bitCount - 1)) & 1 ? x | (0xffff << bitCount) : x);
}

#endif // MEMORY_HPP
