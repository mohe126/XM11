



#include "XM11.h"





//[[ METHODS ]]


void XM11_DataWrite(XM11State* XM11, XM11RegSize index, XM11DataContainer data){
    uint8_t ret = 0;
    if(index >= XM11->DataSize){
        ret = 1;
        if(XM11->OnInvalidDataWrite != NULL) ret = XM11->OnInvalidDataWrite(XM11);
    }
    if(ret){
        XM11->Exception.code = XM11_EXCEPT_DATA_W_SEGFAULT;
        XM11->Exception.value = index;
        return;
    }


    XM11->DataPtr[index] = data;
}
XM11DataContainer XM11_DataRead(XM11State* XM11, XM11RegSize index){
    uint8_t ret = 0;
    if(index >= XM11->DataSize){
        ret = 1;
        if(XM11->OnInvalidDataRead != NULL) ret = XM11->OnInvalidDataRead(XM11);
    }
    if(ret){
        XM11->Exception.code = XM11_EXCEPT_DATA_R_SEGFAULT;
        XM11->Exception.value = index;
        XM11DataContainer tmp;
        tmp.uint32 = 0;
        return tmp;
    }


    return XM11->DataPtr[index];
}

void XM11_ExDataWrite(XM11State* XM11, XM11RegSize index, XM11ExtendedDataContainer data){
    uint8_t ret = 0;
    if(index >= XM11->ExDataSize){
        ret = 1;
        if(XM11->OnInvalidExDataWrite != NULL) ret = XM11->OnInvalidExDataWrite(XM11);
    }
    if(ret){
        XM11->Exception.code = XM11_EXCEPT_EXDATA_W_SEGFAULT;
        XM11->Exception.value = index;
        return;
    }


    XM11->ExDataPtr[index] = data;
}
XM11ExtendedDataContainer XM11_ExDataRead(XM11State* XM11, XM11RegSize index){
    uint8_t ret = 0;
    if(index >= XM11->ExDataSize){
        ret = 1;
        if(XM11->OnInvalidExDataRead != NULL) ret = XM11->OnInvalidExDataRead(XM11);
    }
    if(ret){
        XM11->Exception.code = XM11_EXCEPT_EXDATA_R_SEGFAULT;
        XM11->Exception.value = index;
        XM11ExtendedDataContainer tmp;
        tmp.uint64 = 0;
        return tmp;
    }


    return XM11->ExDataPtr[index];
}

void XM11_RawDataWrite(XM11State* XM11, XM11RegSize index, uint8_t data){
    uint8_t ret = 0;
    if(index >= XM11->RawDataSize){
        ret = 1;
        if(XM11->OnInvalidRawDataWrite != NULL) ret = XM11->OnInvalidRawDataWrite(XM11);
    }
    if(ret){
        XM11->Exception.code = XM11_EXCEPT_RDATA_W_SEGFAULT;
        XM11->Exception.value = index;
        return;
    }


    XM11->RawDataPtr[index] = data;
}
uint8_t XM11_RawDataRead(XM11State* XM11, XM11RegSize index){
    uint8_t ret = 0;
    if(index >= XM11->RawDataSize){
        ret = 1;
        if(XM11->OnInvalidRawDataRead != NULL) ret = XM11->OnInvalidRawDataRead(XM11);
    }
    if(ret){
        XM11->Exception.code = XM11_EXCEPT_RDATA_R_SEGFAULT;
        XM11->Exception.value = index;
        return 0;
    }


    return XM11->RawDataPtr[index];
}

void XM11_PushStack(XM11State* XM11, XM11DataContainer data){
    uint8_t ret = 0;

    if(XM11->StackReg >= XM11->StackSize){
        ret = 1;
        if(XM11->OnStackOverflow != NULL) ret = XM11->OnStackOverflow(XM11);
    }
    if(ret){
        XM11->Exception.code = XM11_EXCEPT_STACK_OVERFLOW;
        XM11->Exception.value = XM11->StackReg;
        return;
    }
    XM11->StackPtr[XM11->StackReg] = data;
    XM11->StackReg++;

}
XM11DataContainer XM11_PopStack(XM11State* XM11){
    if(XM11->StackReg == 0){

        XM11->Exception.code = XM11_EXCEPT_STACK_UNDERFLOW;
        XM11->Exception.value = XM11->StackSize;

        XM11DataContainer tmp;
        tmp.uint32 = 0;
        return tmp;
    }

    XM11->StackReg--;

    XM11DataContainer res;
    res = XM11->StackPtr[XM11->StackReg];
    return res;
}
//[[ ISA ]]
static void XM11_ISA_NOP(XM11State* XM11, XM11Instruction ins){
    //literally a NOP what did you expect there buddy
}
static void XM11_ISA_HOSTCALL(XM11State* XM11, XM11Instruction ins){
    XM11->Exception.code = XM11_EXCEPT_CALL_HOST;
    XM11->Exception.value = ins.operand1;
}


static void XM11_ISA_MVI(XM11State* XM11, XM11Instruction ins){
    uint32_t target = XM11_DataRead(XM11, ins.operand1).uint32;
    XM11DataContainer value = {.uint32 = ins.operand2};
    XM11_DataWrite(XM11, target, value);
}
static void XM11_ISA_MVI2(XM11State* XM11, XM11Instruction ins){
    uint32_t target = ins.operand1;
    XM11DataContainer value = {.uint32 = ins.operand2};
    XM11_DataWrite(XM11, target, value);
}

static void XM11_ISA_MV(XM11State* XM11, XM11Instruction ins){
    uint32_t target = XM11_DataRead(XM11, ins.operand1).uint32;
    uint32_t source = XM11_DataRead(XM11, ins.operand2).uint32;

    XM11_DataWrite(XM11, target, XM11_DataRead(XM11, source));
}



//[[ LOAD FROM RAW ]]

//[[ EXTENDED 64 BIT ISA ]]




//[[ PRIMARY FUNCTIONS ]]

void XM11_Constructor(XM11State* XM11){
    //[[ SET POINTER DEFAULTS ]]
    XM11->OnExceptionDuringExec = NULL;

    XM11->OnInvalidDataRead = NULL;
    XM11->OnInvalidDataWrite = NULL;

    XM11->OnInvalidExDataRead = NULL;
    XM11->OnInvalidExDataWrite = NULL;

    XM11->OnInvalidRawDataRead = NULL;
    XM11->OnInvalidRawDataWrite = NULL;

    XM11->OnStackOverflow = NULL;
    XM11->OnProgramEnd = NULL;

    XM11->ProgramPtr = NULL;
    XM11->DataPtr = NULL;
    XM11->ExDataPtr = NULL;
    XM11->StackPtr = NULL;
    XM11->RawDataPtr = NULL;
    //[[ REST OF THINGS IDK ]]
    XM11->Exception.code = 0;
    XM11->Exception.value = 0;

    XM11->ProgramReg = 0;
    XM11->StackReg = 0;
}

void XM11_Step(XM11State* XM11){
    uint8_t ret = 0;
    if(XM11->ProgramReg >= XM11->ProgramSize){
        ret = 1;
        if(XM11->OnProgramEnd != NULL) ret = XM11->OnProgramEnd(XM11);
    }
    if(ret){
        XM11->Exception.code = XM11_EXCEPT_PROGRAM_END;
        XM11->Exception.value = XM11->ProgramReg;
        return;

    }


    XM11Instruction ins = XM11->ProgramPtr[XM11->ProgramReg++];
    switch(ins.opcode){
        default: {
            XM11->Exception.code = XM11_EXCEPT_INVALID_OP;
            XM11->Exception.value = ins.opcode;
        } break;
        case _XM11_OPC_NOP: XM11_ISA_NOP(XM11, ins); break;
        case _XM11_OPC_HC: XM11_ISA_HOSTCALL(XM11, ins); break;
        case _XM11_OPC_MVI: XM11_ISA_MVI(XM11, ins); break;
        case _XM11_OPC_MVI2: XM11_ISA_MVI2(XM11, ins); break;
        case _XM11_OPC_MV: XM11_ISA_MV(XM11, ins); break;

    }

    uint8_t unret;
    if(XM11->Exception.code != 0 && XM11->OnExceptionDuringExec != NULL) unret = XM11->OnExceptionDuringExec(XM11);


}
