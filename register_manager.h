#ifndef REGISTER_MANAGER_H
#define REGISTER_MANAGER_H

#include "common.h"

/*
 * 16 general purpose 8-bit registers
 * one 16-bit register "I" which stores memory addresses
 * two 8-bit registers for delay and sound timers
 * "pseudo-registers" will be implemented in chip
 */
class RegisterManager {
private:
    std::vector<uint8_t>generalRegisters;
    uint16_t iRegister;
    uint8_t delayRegister;
    uint8_t soundRegister;
public:
    RegisterManager() {
        generalRegisters = std::vector<uint8_t>(16);
        // beginning address of chip8 program
        iRegister = 0x200;
        delayRegister = 0;
        soundRegister = 0;
    }
    void WriteGeneralRegister(const int index, const uint8_t value) {
        if(index<0 || index>=16) {
            std::cerr << "Register Index out of range" << std::endl;
            return;
        }
        if(index == 0xF) {
            std::cerr << "This register should not be used" << std::endl;
            return;
        }
        generalRegisters[index] = value;
        return;
    }

    uint8_t ReadGeneralRegister(const int index) {
        if(index<0 || index>=16) {
            std::cerr << "Register Index out of range" << std::endl;
            return 0;
        }
        return generalRegisters[index];
    }

    void WriteIRegister(const uint16_t addr) {
        if(0x1000 <= addr) {
            std::cerr << "Invalid memory address" << std::endl;
            return;
        }
        iRegister = addr;
    }

    uint16_t ReadIRegister() {
        return iRegister;
    }
};

#endif
