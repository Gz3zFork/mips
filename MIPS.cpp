#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <conio.h>
#include "MIPS.h"

template <class T>
std::string hex(T number)
{
    std::stringstream hexstr;
    hexstr << std::hex << (ui32) number;
    return hexstr.str();
}

ui32 sign_extend(ui32 x, ui32 bit_count)
{
    if (x >> (bit_count - 1)) {
        x |= (0xFFFFFFFF << bit_count);
    }
    return x;
}

void MIPS::fetch(ui32 offset=4)
{
    PC = nPC;
    nPC += offset;
}

void MIPS::executeSC()
{
    switch (reg[V0])
    {
        case PRINT_INT:
        {
            std::cout << (i32) reg[A0];
            break;
        }

        case PRINT_STR:
        {
            ui32 nullChar = reg[A0];
            while (memory[nullChar] != 0)
            {
                std::cout << memory[nullChar];
                nullChar++;
            }
            break;
        }

        case MALLOC:
        {
            if (reg[A0] + lastMemoryAddress > MAX_MEMORY)
            {
                running = false;
                error = "SYSCALL exception at 0x" + hex<ui32>(PC) + ": MALLOC not enough memory error";
                return;
            }
            reg[V0] = lastMemoryAddress;
            lastMemoryAddress += reg[A0];
            break;
        }

        case READ_INT:
        {
            std::cin >> reg[V0];
            std::cin.clear();
            std::cin.sync();
            break;
        }

        case READ_STR:
        {
            if (reg[A1] == 0) {}
            else if (reg[A1] == 1)
            {
                memory[reg[A0]] = 0;
            }
            else
            {
                char* input = new char[reg[A1]];
                std::cin.get(input, reg[A1]);
                for (ui32 i = 0; i < reg[A1]; i++)
                {
                    memory[i + reg[A0]] = input[i];
                }
                std::cin.clear();
                std::cin.sync();
                delete[] input;
            }
            break;
        }

        case EXIT:
        {
            running = false;
            error = "Terminated successfully";
            break;
        }

        case PRINT_CHAR:
        {
            std::cout << (ui8) (reg[A0] & 0xFF);
            break;
        }
        case READ_CHAR:
        {
            reg[V0] = getch();
            break;
        }

        case EXIT2:
        {
            running = false;
            error = "Terminated with error code " + std::to_string(reg[A0]);
            break;
        }
    }
}

void MIPS::decodeR()
{
    rs = ((memory[PC] & 0b11) << 3) | (memory[PC + 1] >> 5);

    rt = memory[PC + 1] & 0b11111;

    rd = memory[PC + 2] >> 3;

    shamt = ((memory[PC + 2] & 0b111) << 2) | (memory[PC + 3] >> 6);

    funct = memory[PC + 3] & 0b111111;
}

void MIPS::executeR()
{
    bool callFetch = true;
    switch (funct)
    {
        case SLL:
        {
            reg[rd] = reg[rt] << shamt;
            break;
        }

        case SRL:
        {
            reg[rd] = reg[rt] >> shamt;
            break;
        }

        case SRA:
        {
            // to do an arithmetic (signed) shift, convert to a signed integer, shift, and convert back to an unsigned integer
            reg[rd] = (ui32) ((i32) reg[rt] >> shamt);
            break;
        }

        case SLLV:
        {
            reg[rd] = reg[rt] << reg[rs];
            break;
        }

        case SRLV:
        {
            reg[rd] = reg[rt] >> reg[rs];
            break;
        }

        case SRAV:
        {
            // see SRA
            reg[rd] = (ui32) ((i32) reg[rt] >> reg[rs]);
            break;
        }

        case JR:
        {
            if (reg[rs] % 4)
            {
                running = false;
                error = "Trap exception at 0x" + hex<ui32>(PC) + ": JR address error";
                return;
            }
            PC = nPC;
            nPC = reg[rs];
            callFetch = false;
            break;
        }

        case JALR:
        {
            if (reg[rs] % 4)
            {
                running = false;
                error = "Trap exception at 0x" + hex<ui32>(PC) + ": JALR address error";
                return;
            }
            reg[rd] = PC + 8;
            PC = nPC;
            nPC = reg[rs];
            callFetch = false;
            break;
        }

        case SYSCALL:
        {
            executeSC();
            break;
        }

        case MFHI:
        {
            reg[rd] = HI;
            break;
        }

        case MTHI:
        {
            HI = reg[rs];
            break;
        }

        case MFLO:
        {
            reg[rd] = LO;
            break;
        }

        case MTLO:
        {
            LO = reg[rs];
            break;
        }

        case MULT:
        {
            // convert arguments to an integer first because (i64) ui32 doesn't account for signs, and will pad with 0's
            i64 mult = (i64) (i32) reg[rs] * (i64) (i32) reg[rt]; HI = mult >> 32;
            LO = mult & 0xFFFFFFFF;
            break;
        }

        case MULTU:
        {
            ui64 mult = (ui64) reg[rs] * (ui64) reg[rt];
            HI = mult >> 32;
            LO = mult & 0xFFFFFFFF;
            break;
        }

        case DIV:
        {
            LO = (i32) reg[rs] / (i32) reg[rt];
            HI = (i32) reg[rs] % (i32) reg[rt];
            break;
        }

        case DIVU:
        {
            LO = reg[rs] / reg[rt];
            HI = reg[rs] % reg[rt];
            break;
        }

        case ADD:
        {
            reg[rd] = reg[rs] + reg[rt];
            if (reg[rd] < reg[rs] || reg[rd] < reg[rt])
            {
                running = false;
                error = "Trap exception at 0x" + hex<ui32>(PC) + ": ADD integer overflow";
            }
            break;
        }

        case ADDU:
        {
            reg[rd] = reg[rs] + reg[rt];
            break;
        }

        case SUB:
        {
            reg[rd] = reg[rs] - reg[rt];
            if (reg[rd] > reg[rs])
            {
                running = false;
                error = "Trap exception at 0x" + hex<ui32>(PC) + ": SUB integer overflow";
            }
            break;
        }

        case SUBU:
        {
            reg[rd] = reg[rs] - reg[rt];
            break;
        }

        case AND:
        {
            reg[rd] = reg[rs] & reg[rt];
            break;
        }

        case OR:
        {
            reg[rd] = reg[rs] | reg[rt];
            break;
        }

        case XOR:
        {
            reg[rd] = reg[rs] ^ reg[rt];
            break;
        }

        case NOR:
        {
            reg[rd] = !(reg[rs] | reg[rt]);
            break;
        }

        case SLT:
        {
            reg[rd] = (i32) reg[rs] < (i32) reg[rt];
            break;
        }

        case SLTU:
        {
            reg[rd] = reg[rs] < reg[rt];
            break;
        }
    }

    if (callFetch)
    {
        fetch();
    }
}

void MIPS::decodeI()
{
    rs = ((memory[PC] & 0b11) << 3) | (memory[PC + 1] >> 5);

    rt = memory[PC + 1] & 0b11111;

    imm = memory[PC + 2] << 8 | memory[PC + 3];
}

void MIPS::executeI()
{
    bool callFetch = true;
    switch (opcode)
    {
        case BEQ:
        {
            if (reg[rs] == reg[rt])
            {
                fetch(sign_extend(imm << 2, 18));
                callFetch = false;
            }
            break;
        }

        case BNE:
        {
            if (reg[rs] != reg[rt])
            {
                fetch(sign_extend(imm << 2, 18));
                callFetch = false;
            }
            break;
        }

        case BLEZ:
        {
            if (reg[rs] <= 0)
            {
                fetch(sign_extend(imm << 2, 18));
                callFetch = false;
            }
            break;
        }

        case BGTZ:
        {
            if (reg[rs] > 0)
            {
                fetch(sign_extend(imm << 2, 18));
                callFetch = false;
            }
            break;
        }

        case ADDI:
        {
            ui32 imm32 = sign_extend(imm, 16);
            reg[rt] = reg[rs] + imm32;
            if (reg[rt] < reg[rs] || reg[rt] < imm32)
            {
                running = false;
                error = "Exception at 0x" + hex<ui32>(PC) + ": ADDI integer overflow, $rs = 0x" + hex<ui32>(reg[rs]) + ", $rt = 0x" + hex<ui32>(reg[rt]) + ", imm = 0x" + hex<ui32>(imm32);
            }
            break;
        }

        case ADDIU:
        {
            reg[rt] = reg[rs] + sign_extend(imm, 16);
            break;
        }

        case SLTI:
        {
            reg[rt] = (i32) reg[rs] < (i32) sign_extend(imm, 16);
            break;
        }

        case SLTIU:
        {
            reg[rt] = reg[rs] < sign_extend(imm, 16);
            break;
        }

        case ANDI:
        {
            reg[rt] = reg[rs] & imm;
            break;
        }

        case ORI:
        {
            reg[rt] = reg[rs] | imm;
            break;
        }

        case XORI:
        {
            reg[rt] = reg[rs] ^ imm;
            break;
        }

        case LUI:
        {
            reg[rt] = imm << 16;
            break;
        }

        case LB:
        {
            ui32 address = reg[rs] + sign_extend(imm, 16);
            reg[rt] = sign_extend(memory[address], 8);
            break;
        }

        case LH:
        {
            ui32 address = reg[rs] + sign_extend(imm, 16);
            reg[rt] = sign_extend((memory[address] << 8) | memory[address + 1], 16);
            break;
        }

        case LW:
        {
            ui32 address = reg[rs] + sign_extend(imm, 16);
            reg[rt] = (memory[address] << 24) | (memory[address + 1] << 16) | (memory[address + 2] << 8) | memory[address + 3];
            break;
        }

        case LBU:
        {
            ui32 address = reg[rs] + sign_extend(imm, 16);
            reg[rt] = memory[address];
            break;
        }

        case LHU:
        {
            ui32 address = reg[rs] + sign_extend(imm, 16);
            reg[rt] = (memory[address] << 8) | memory[address + 1];
            break;
        }

        case SB:
        {
            ui32 address = reg[rs] + sign_extend(imm, 16);
            memory[address] = reg[rt] & 0xFF;
            break;
        }

        case SH:
        {
            ui32 address = reg[rs] + sign_extend(imm, 16);
            memory[address] = (reg[rt] >> 8) & 0xFF;
            memory[address + 1] = reg[rt] & 0xFF;
            break;
        }

        case SW:
        {
            ui32 address = reg[rs] + sign_extend(imm, 16);
            memory[address] = (reg[rt] >> 24) & 0xFF;
            memory[address + 1] = (reg[rt] >> 16) & 0xFF;
            memory[address + 2] = (reg[rt] >> 8) & 0xFF;
            memory[address + 3] = reg[rt] & 0xFF;
            break;
        }
    }

    if (callFetch)
    {
        fetch();
    }
}

MIPS::MIPS()
{
    for (int i = 0; i < R_COUNT; i++)
    {
        reg[i] = 0;
    }

    reg[SP] = MAX_MEMORY;

    for (int i = 0; i < MAX_MEMORY; i++)
    {
        memory[i] = 0;
    }
}

std::string MIPS::getError() { return error; }

// loads a program. returns true if an error occured
bool MIPS::loadProgram(std::string filename)
{
    std::ifstream f(filename, std::ios::binary);
    if (!f) {
        error = "Could not read file";
        return true;
    }

    f.seekg(0, std::ios::end);
    ui32 progLength = f.tellg();
    f.seekg(0, std::ios::beg);

    if (progLength > MAX_MEMORY)
    {
        error = "Program length is too long";
        return true;
    }

    lastMemoryAddress = progLength;

    f.read((char*) &memory[0], progLength);
    return false;
}

void MIPS::run()
{
    while (running)
    {
        opcode = memory[PC] >> 2;
        switch (opcode)
        {
            case 0:
            {
                decodeR();
                executeR();
                break;
            }

            case J:
            {
                ui32 target = ((memory[PC] & 0b11) << 26) | (memory[PC + 1] << 18) | (memory[PC + 2] << 10) | (memory[PC + 3] << 2);
                PC = nPC;
                nPC = (PC & 0xF0000000) | target;
                break;
            }

            case JAL:
            {
                ui32 target = ((memory[PC] & 0b11) << 26) | (memory[PC + 1] << 18) | (memory[PC + 2] << 10) | (memory[PC + 3] << 2);
                reg[RA] = PC + 8;
                PC = nPC;
                nPC = (PC & 0xF0000000) | target;
                break;
            }

            default:
            {
                decodeI();
                executeI();
                break;
            }
        }
    }
}
