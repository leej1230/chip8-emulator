#ifndef CHIP_H
#define CHIP_H

#include "common.h"
#include "memory.h"
#include "window.h"
#include "register_manager.h"

#include<fstream>

class Chip8 {
private:
    WindowManager window;
    RegisterManager registers;
    Memory memory;

    uint16_t programCounter;
public:
    Chip8() {
        programCounter = 0x200;
    }
    bool loadROM(const std::string fileName) {
        std::ifstream data(fileName, std::ios::binary);
        if(!data.is_open()) {
            std::cerr << "Failed to open file" << std::endl;
            return false;
        }

        char currentByte;
        uint16_t memoryAddr = 0x200;
        while(data.get(currentByte)) {
            if(!memory.WriteData(memoryAddr, (uint8_t)currentByte)) {
                std::cerr << "Failed to write instruction to memory" << std::endl;
                return false;
            }
            ++memoryAddr;
        }
        data.close();

        return true;
    }
    void fetch();
    void decode();
    void execute();
};

#endif
