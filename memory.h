#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"

class Memory {
private:
    std::vector<uint8_t> data;
    const uint16_t startAddr = 0x200;
    const uint16_t endAddr = 0xFFF;
    const uint8_t fontsetSize = 80;
    const uint8_t fontset[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
public:
    Memory() {
        data = std::vector<uint8_t>(4096);
        // insert fontset from index 0
        for(int i=0; i<fontsetSize; ++i) {
            data[i] = fontset[i];
        }
    }

    bool WriteData(const uint16_t addr, const uint8_t value) {
        if(addr < startAddr || addr > endAddr) {
            std::cerr << "Invalid address" << std::endl;
            return false;
        }
        data[addr] = value;
        return true;
    }

    uint8_t ReadData(const uint16_t addr) {
        if(addr < startAddr || addr > endAddr) {
            std::cerr << "Invalid address" << std::endl;
            return 0;
        }
        return data[addr];
    }
};

#endif
