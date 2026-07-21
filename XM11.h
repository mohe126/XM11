#pragma once
#include <stdint.h>
#include <string.h>
#include <stdio.h>


//[[ ENUM ]]


enum XM11ErrorCodes {
    XM11_EXCEPT_DATA_R_SEGFAULT,
    XM11_EXCEPT_DATA_W_SEGFAULT,
    XM11_EXCEPT_EXDATA_R_SEGFAULT,
    XM11_EXCEPT_EXDATA_W_SEGFAULT,
    XM11_EXCEPT_RDATA_R_SEGFAULT,
    XM11_EXCEPT_RDATA_W_SEGFAULT,

    XM11_EXCEPT_STACK_OVERFLOW,
    XM11_EXCEPT_STACK_UNDERFLOW,

    XM11_EXCEPT_PROGRAM_END,


    XM11_EXCEPT_INVALID_OP,
    XM11_EXCEPT_TIME_CALLBACK_FAIL,

    XM11_EXCEPT_CALL_HOST,

};
enum XM11InstructionOpcodes {
    _XM11_OPC_NOP,
    _XM11_OPC_HC,
    _XM11_OPC_MVI,
    _XM11_OPC_MVI2,
    _XM11_OPC_MV,
    
    


};


//[[ UNION ]]
typedef union {
    uint8_t char8;
    uint16_t char16;
    uint32_t char32;

    uint8_t uint8;
    uint16_t uint16;
    uint32_t uint32;
    
    int8_t int8;
    int16_t int16;
    int32_t int32;

    float f32;


    uint8_t boolean;
} XM11DataContainer;

typedef union {
    uint8_t char8;
    uint16_t char16;
    uint32_t char32;

    uint8_t uint8;
    uint16_t uint16;
    uint32_t uint32;
    uint64_t uint64;
    
    int8_t int8;
    int16_t int16;
    int32_t int32;
    int64_t int64;

    float f32;
    double f64;

    void* ptr;

    uint8_t boolean;

} XM11ExtendedDataContainer;


//[[ STRUCT ]]
typedef struct XM11Instruction {
    //Might make THUMB mode
    //Where operands are 16 bit
    //Though less memory
    //Doesn't sound too bad

    uint16_t opcode;
    uint32_t operand1;
    uint32_t operand2;
    uint32_t operand3;

} XM11Instruction;
typedef struct XM11Exception {
    uint32_t code;
    uint32_t value;
    

} XM11Exception;

typedef struct XM11State XM11State;

//[[ TYPEDEF ]]

typedef uint32_t XM11RegSize;
typedef uint8_t (*XM11fault_cb)(XM11State* state);
typedef float (*XM11time_cb)();
//[[ ENUM ]]





//[[ MAIN STRUCT ]]

typedef struct XM11State {
    //[[ PROGRAM ]]

    XM11Instruction* ProgramPtr;
    XM11RegSize      ProgramSize;
    XM11RegSize      ProgramReg;

    //[[ STACK ]]

    XM11DataContainer* StackPtr;
    XM11RegSize        StackSize;
    XM11RegSize        StackReg;

    //[[ DATA MEMORY ]]

    XM11DataContainer* DataPtr;
    XM11RegSize        DataSize;

    XM11ExtendedDataContainer* ExDataPtr;
    XM11RegSize                ExDataSize;

    uint8_t*         RawDataPtr;
    XM11RegSize      RawDataSize;

    XM11fault_cb OnInvalidDataWrite; //All set to NULL by default and ignored if NULL
    XM11fault_cb OnInvalidDataRead;

    XM11fault_cb OnInvalidExDataWrite;
    XM11fault_cb OnInvalidExDataRead;


    XM11fault_cb OnInvalidRawDataWrite;
    XM11fault_cb OnInvalidRawDataRead;

    XM11fault_cb OnStackOverflow; //If stack is empty and you try to pop a value then that's always gonna except

    XM11fault_cb OnProgramEnd;
    XM11fault_cb OnExceptionDuringExec; //Gets called in step function after an exception is met

    XM11time_cb  GetTime;


    //[[ EXCEPTION ]]
    XM11Exception Exception;
    

    //possible extend size? Return false if handled, true if except.
    //Essentially what this is for by the way.




} XM11State;





//[[ METHODS ]]

void XM11_DataWrite(XM11State* XM11, XM11RegSize index, XM11DataContainer data);
XM11DataContainer XM11_DataRead(XM11State* XM11, XM11RegSize index);

void XM11_ExDataWrite(XM11State* XM11, XM11RegSize index, XM11ExtendedDataContainer data);
XM11ExtendedDataContainer XM11_ExDataRead(XM11State* XM11, XM11RegSize index);

void XM11_RawDataWrite(XM11State* XM11, XM11RegSize index, uint8_t data);
uint8_t XM11_RawDataRead(XM11State* XM11, XM11RegSize index);

void XM11_PushStack(XM11State* XM11, XM11DataContainer data);
XM11DataContainer XM11_PopStack(XM11State* XM11);




//[[ PRIMARY FUNCTIONS ]]

void XM11_Constructor(XM11State* XM11);
void XM11_Step(XM11State* XM11);
void XM11_Run_Count(XM11State* XM11, XM11RegSize count);
void XM11_Run_Time(float ms);
