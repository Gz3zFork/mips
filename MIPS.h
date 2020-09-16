#pragma once
#include <string>
typedef int64_t i64;
typedef int32_t i32;
typedef uint64_t ui64;
typedef uint32_t ui32;
typedef uint16_t ui16;
typedef uint8_t ui8;

// registers
enum
{
    ZERO, AT, V0, V1, A0, A1, A2, A3, T0, T1, T2, T3, T4, T5, T6, T7,
    S0, S1, S2, S3, S4, S5, S6, S7, T8, T9, K0, K1, GP, SP, FP, RA,
    R_COUNT
};

// r format instructions
enum
{
    SLL     = 0,
    SRL     = 2,
    SRA     = 3,
    SLLV    = 4,
    SRLV    = 6,
    SRAV    = 7,
    JR      = 8,
    JALR    = 9,
    SYSCALL = 12,
    MFHI    = 16,
    MTHI    = 17,
    MFLO    = 18,
    MTLO    = 19,
    MULT    = 24,
    MULTU   = 25,
    DIV     = 26,
    DIVU    = 27,
    ADD     = 32,
    ADDU    = 33,
    SUB     = 34,
    SUBU    = 35,
    AND     = 36,
    OR      = 37,
    XOR     = 38,
    NOR     = 39,
    SLT     = 42,
    SLTU    = 43
};

// j format instructions
enum
{
    J   = 2,
    JAL = 3
};

// i format instructions
enum
{
    BEQ   = 4,
    BNE   = 5,
    BLEZ  = 6,
    BGTZ  = 7,
    ADDI  = 8,
    ADDIU = 9,
    SLTI  = 10,
    SLTIU = 11,
    ANDI  = 12,
    ORI   = 13,
    XORI  = 14,
    LUI   = 15,
    LB    = 32,
    LH    = 33,
    LW    = 35,
    LBU   = 36,
    LHU   = 37,
    SB    = 40,
    SH    = 41,
    SW    = 43,
};

// syscall codes
enum
{
    PRINT_INT  = 0x01,
    PRINT_STR  = 0x04,
    READ_INT   = 0x05,
    READ_STR   = 0x08,
    MALLOC     = 0x09,
    EXIT       = 0x0A,
    PRINT_CHAR = 0x0B,
    READ_CHAR  = 0x0C,
    EXIT2      = 0x11
};

// 64 kb bytes of RAM
const ui32 MAX_MEMORY = 0xFFFF + 1;

class MIPS
{
private:
    ui8 memory[MAX_MEMORY];
    // used for malloc syscall
    ui32 lastMemoryAddress;

    // 32 32 bit int registers
    ui32 reg[R_COUNT];

    // HI and LO special registers
    ui32 HI = 0;
    ui32 LO = 0;

    // program counters
    ui32 PC = 0;
    ui32 nPC = 4;
    bool running = true;

    // execution variables
    ui8 opcode = 0;
    ui8 rs = 0;
    ui8 rt = 0;
    ui8 rd = 0;
    ui8 shamt = 0;
    ui8 funct = 0;
    ui16 imm = 0;

    // error message
    std::string error = "";

    void fetch(ui32 offset);

    void executeSC();

    void decodeR();
    void executeR();

    void decodeI();
    void executeI();

public:
    MIPS();

    std::string getError();

    bool loadProgram(std::string filename);

    void run();
};
