#ifndef CHIP_H
#define CHIP_H

#include "common.h"
#include "memory.h"
#include "window.h"
#include "register_manager.h"

#include<fstream>
#include<random>
#include<cmath>

class Chip8 {
private:
    WindowManager _window;
    RegisterManager registers;
    Memory memory;

    uint16_t programCounter;
    uint16_t instruction;
public:
    Chip8() {
        programCounter = 0x200;
        if(!_window.initialize()) {
            std::cerr << "Failed to initialize window" << std::endl;
        }
        std::cout << "Initialized window" << std::endl;
    }
    ~Chip8() {
        _window.destroy();
        std::cout <<"Destroyed window" << std::endl;
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

    bool render() {
        return _window.render();
    }

    void fetch() {
        instruction = ((uint16_t)memory.ReadData(programCounter) << 8) | memory.ReadData(programCounter+1);
        programCounter += 2;
    }

    void decodeAndExecute() {
        int instructionType = instruction >> 12;
        std::cout << std::hex << "Instruction: " << instruction << std::endl;
        switch(instructionType) {
            case 0x0:
            {
                int addr = (int)(instruction & 0x0FFF);
                if (addr == 0x0E0) {
                    // 00E0 clear display
                    _window.clear();
                } else if (addr == 0x0EE) {
                    // 00EE return from subroutine
                    std::cout << "Instruction not yet implemented!" << std::endl;
                } else {
                    // 0nnn jump to machine code routine
                    std::cout << "Instruction not yet implemented!" << std::endl;
                }
                break;
            }
            case 0x1:
            {
                // 1nnn jump to nnn
                int addr = (int)(instruction & 0x0FFF);
                programCounter = addr;
                break;
            }
            case 0x2:
            {
                // 2nnn call subroutine at nnn
                int addr = (int)(instruction & 0x0FFF);
                break;
            }
            case 0x3:
            {
                // 3xkk if Vx==kk skip next instruction
                int registerIndex = (int)(instruction & 0x0F00) >> 8;
                uint8_t byteValue = (uint8_t)(instruction & 0x00FF);
                if(registers.ReadGeneralRegister(registerIndex) == byteValue) {
                    programCounter += 2;
                }
                break;
            }
            case 0x4:
            {
                // 4xkk if Vx!=kk skip next instruction
                int registerIndex = (int)(instruction & 0x0F00) >> 8;
                uint8_t byteValue = (uint8_t)(instruction & 0x00FF);
                if(registers.ReadGeneralRegister(registerIndex) != byteValue) {
                    programCounter += 2;
                }
                break;
            }
            case 0x5:
            {
                // 5xy0 skip if vx==vy
                // not sure if the last nibble must be 0 or not
                // -> will consider as invalid for now
                if ((int)(instruction & 0x000F) != 0) {
                    std::cerr << "Unsupported instruction: " << std::hex << instruction << std::endl;
                }
                int registerIndex1 = (int)(instruction & 0x0F00) >> 8;
                int registerIndex2 = (int)(instruction & 0x00F0) >> 4;
                if(registers.ReadGeneralRegister(registerIndex1) == registers.ReadGeneralRegister(registerIndex2)) {
                    programCounter += 2;
                }
                break;
            }
            case 0x6:
            {
                // 6xkk set kk to Vx
                int registerIndex = (int)(instruction & 0x0F00) >> 8;
                int byteValue = (int)(instruction & 0x00FF);
                std::cout << "registerIndex: " << registerIndex << std::endl;
                std::cout << "byteValue: " << byteValue << std::endl;
                registers.WriteGeneralRegister(registerIndex, (uint8_t)byteValue);
                break;
            }
            case 0x7:
            {
                // 7xkk Vx += kk
                int registerIndex = (int)(instruction & 0x0F00) >> 8;
                int registerValue = (int)registers.ReadGeneralRegister(registerIndex);
                int byteValue = (int)(instruction & 0x00FF);
                std::cout << "registerValue: " << registerValue << std::endl;
                std::cout << "byteValue: " << byteValue << std::endl;
                registers.WriteGeneralRegister(registerIndex, (uint8_t)registerValue+byteValue);
                break;
            }
            case 0x8:
            {
                const int registerIndex1 = (int)(instruction & 0x0F00) >> 8;
                const int registerIndex2 = (int)(instruction & 0x00F0) >> 4;
                const int registerIndexF = 0xF;
                const int registerValue1 = registers.ReadGeneralRegister(registerIndex1);
                const int registerValue2 = registers.ReadGeneralRegister(registerIndex2);
                const int lastNibble = (int)(instruction & 0x000F);
                switch(lastNibble) {
                    case 0x0:
                        // 8xy0 Vx = Vy
                        registers.WriteGeneralRegister(registerIndex1, registerValue2);
                        break;
                    case 0x1:
                        // 8xy1 Vx = Vx | Vy
                        registers.WriteGeneralRegister(registerIndex1, registerValue1 | registerValue2);
                        break;
                    case 0x2:
                        // 8xy2 Vx = Vx & Vy
                        registers.WriteGeneralRegister(registerIndex1, registerValue1 & registerValue2);
                        break;
                    case 0x3:
                        // 8xy3 Vx = Vx ^ Vy
                        registers.WriteGeneralRegister(registerIndex1, registerValue1 ^ registerValue2);
                        break;
                    case 0x4:
                    {
                        // 8xy4 Vx = Vx + Vy, VF will have carry if exists
                        uint16_t result = (uint16_t)registerValue1 + registerValue2;
                        registers.WriteGeneralRegister(registerIndexF, result>>8 & 1);
                        registers.WriteGeneralRegister(registerIndex1, (uint8_t)result);
                        break;
                    }
                    case 0x5:
                    {
                        // 8xy5 Vx = Vx - Vy, VF=not borrow -> 1 if Vx>Vy, then run subtract
                        registers.WriteGeneralRegister(registerIndexF, registerValue1>registerValue2);
                        uint8_t result = registerValue1 - registerValue2;
                        registers.WriteGeneralRegister(registerIndex1, result);
                        break;
                    }
                    case 0x6:
                        // 8xy6 Vx >> 1, if Vx is odd set VF to 1
                        registers.WriteGeneralRegister(registerIndexF, registerValue1 & 1);
                        registers.WriteGeneralRegister(registerIndex1, registerValue1>>1);
                        break;
                    case 0x7:
                    {
                        // 8xy7 Vx = Vy - Vx, VF=not borrow -> 1 if Vy>Vx, then run subtract
                        registers.WriteGeneralRegister(registerIndexF, registerValue2>registerValue1);
                        uint8_t result = registerValue2 - registerValue1;
                        registers.WriteGeneralRegister(registerIndex1, result);
                        break;
                    }
                    case 0xE:
                        // 8xyE Vx << 1, if most significant bit is 1, set VF to 1 and shift
                        registers.WriteGeneralRegister(registerIndexF, registerValue1>>7 & 1);
                        registers.WriteGeneralRegister(registerIndex1, registerValue1<<1);
                        break;
                    default:
                        std::cerr << "Unsupported instruction: " << std::hex << instruction << std::endl;
                        break;
                }
                break;
            }
            case 0x9:
            {
                // 9xy0 if Vx != Vy skip next instruction
                const int registerIndex1 = (int)(instruction & 0x0F00) >> 8;
                const int registerIndex2 = (int)(instruction & 0x00F0) >> 4;
                const int registerValue1 = registers.ReadGeneralRegister(registerIndex1);
                const int registerValue2 = registers.ReadGeneralRegister(registerIndex2);
                if(registerValue1 != registerValue2) {
                    programCounter += 2;
                }
                break;
            }
            case 0xA:
            {
                // Annn I = nnn
                uint16_t last3Nibbles = instruction & 0x0FFF;
                registers.WriteIRegister(last3Nibbles);
                break;
            }
            case 0xB:
            {
                // Bnnn jump to V0 + nnn
                uint16_t last3Nibbles = instruction & 0x0FFF;
                const int registerValue = registers.ReadGeneralRegister(0);
                programCounter = last3Nibbles + registerValue;
                break;
            }
            case 0xC:
            {
                // Cxkk Vx = randomInt(0,255) & kk
                const int registerIndex1 = (int)(instruction & 0x0F00) >> 8;
                uint8_t last2Nibbles = (uint8_t)instruction;
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dist(0, 255);
                uint8_t randomValue = dist(gen);
                registers.WriteGeneralRegister(registerIndex1, randomValue & last2Nibbles);
                break;
            }
            case 0xD:
            {
                // Dxyn starting at (Vx, Vy) draws n bytes from address in I
                //     when draw, collided pixel will be erased (xor) and sets VF=1
                //     outside the display wraps around to the opposite side
                // wrapping (module calculation) will be handled at window class
                const int registerIndex1 = (int)(instruction & 0x0F00) >> 8;
                const int registerIndex2 = (int)(instruction & 0x00F0) >> 4;
                const int registerValue1 = registers.ReadGeneralRegister(registerIndex1);
                const int registerValue2 = registers.ReadGeneralRegister(registerIndex2);
                const int drawBytes = (int)(instruction & 0x000F);
                const uint16_t iAddr = registers.ReadIRegister();
                std::cout << "====draw====" << std::endl;
                std::cout << "registerIndex1: " << registerIndex1 << std::endl;
                std::cout << "registerIndex2: " << registerIndex2 << std::endl;
                std::cout << "registerValue1: " << registerValue1 << std::endl;
                std::cout << "registerValue2: " << registerValue2 << std::endl;
                std::cout << "drawbytes: " << drawBytes << std::endl;
                std::cout << "iAddr: " << iAddr << std::endl;
                for(int currentByte=0; currentByte<drawBytes; ++currentByte) {
                    uint8_t spriteData = memory.ReadData(iAddr+currentByte);
                    std::cout << "spriteData: " << std::hex << (int)memory.ReadData(iAddr+currentByte) << std::endl;
                    for(int currentBit=0; currentBit<8; ++currentBit) {
                        if(((spriteData >> (7-currentBit)) & 1) != 1){
                            continue;
                        }
                        _window.addSquare(registerValue1+currentBit, registerValue2+currentByte);
                    }
                }
                std::cout << "====end====" << std::endl;
                break;
            }
            case 0xE:
            {
                const uint8_t lastByte = (uint8_t)instruction;
                switch(lastByte){
                    case 0x9E:
                        // Ex9E Checks value in Vx and if the value is pressed in keyboard, skip next instruction
                        std::cout << "Instruction not yet implemented!" << std::endl;
                        break;
                    case 0xA1:
                        // ExA1 Skip next instruction if value in Vx is NOT pressed
                        std::cout << "Instruction not yet implemented!" << std::endl;
                        break;
                    default:
                        std::cerr << "Unsupported instruction: " << std::hex << instruction << std::endl;
                        break;
                }
                break;
            }
            case 0xF:
            {
                const uint8_t lastByte = (uint8_t)instruction;
                const int registerIndex1 = (int)(instruction & 0x0F00) >> 8;
                const int registerValue1 = registers.ReadGeneralRegister(registerIndex1);
                switch(lastByte){
                    case 0x07:
                        // Fx07 Vx = delay timer value
                        registers.WriteGeneralRegister(registerIndex1, registers.ReadDelayRegister());
                        break;
                    case 0x0A:
                        // Fx0A Wait for key input, pressed value will be stored in Vx 
                        break;
                    case 0x15:
                        // Fx15 delay timer = Vx
                        registers.WriteDelayRegister(registerValue1);
                        break;
                    case 0x18:
                        // Fx18 sound timer = Vx
                        registers.WriteSoundRegister(registerValue1);
                        break;
                    case 0x1E:
                        // Fx1E I += Vx
                        registers.WriteIRegister(registers.ReadIRegister() + registerValue1);
                        break;
                    case 0x29:
                        // Fx29 I = memory location for digit in Vx
                        // Get digit from Vx value
                        // each sprite is 5 byte so the location will be digit*5
                        registers.WriteIRegister((registerValue1&0x0F) * 5);
                        break;
                    case 0x33:
                    {
                        // Fx33 gets Vx value as decimal address I will have hundreds digit, I+1 gets tens, I+2 gets ones
                        const uint16_t iValue = registers.ReadIRegister();
                        for(int i=0; i<3; ++i){
                            memory.WriteData(iValue+i, (registerValue1/(int)std::pow(10,2-i))%10);
                        }
                        break;
                    }
                    case 0x55:
                    {
                        // Fx55 copy V0 to Vx values to address I
                        const uint16_t iValue = registers.ReadIRegister();
                        for(int i=0; i<=registerIndex1; ++i) {
                            memory.WriteData(iValue+i, registers.ReadGeneralRegister(i));
                        }
                        break;
                    }
                    case 0x65:
                    {
                        // Fx65 loads values to V0 to Vx from address I
                        const uint16_t iValue = registers.ReadIRegister();
                        for(int i=0; i<=registerIndex1; ++i) {
                            const uint8_t memoryValue = memory.ReadData(iValue+i);
                            registers.WriteGeneralRegister(i, memoryValue);
                        }
                        break;
                    }
                    default:
                        std::cerr << "Unsupported instruction: " << std::hex << instruction << std::endl;
                        break;
                }
                break;
            }
            default:
                std::cerr << "Unsupported instruction: " << std::hex << instruction << std::endl;
        }
    }
};

#endif
