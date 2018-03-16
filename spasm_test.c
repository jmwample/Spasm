

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/syscall.h>

#include "spasm.h"



// Test the Syscall Function with the Register Pointer (sinulated with an array)
// TODO
int test_doSyscall_mmap(){   
                         //R ax bx cx   dx si di r8 r9  STACK
    uint_reg R_mock[16] = {0, 1, 0, 0, 0x8, 1, 1, 0, 0, 
            0x0a41424344454648,0,0,0,0,0,0};

    R_mock[5] = (uint_reg)(R_mock + STK_OFFSET);
    
    printf("R(array): %016lX, R(addr):%016lX, RSI:%016lX\n",(uint_reg)R_mock, (uint_reg)&R_mock, R_mock[5]);

    doSyscall(R_mock);

    printRegs((R_mock), 0);
    return 1;
}


// Test the Syscall Function with the Register Pointer (sinulated with an array)
void test_doSyscall_write(){   
    printf("--------[ Test doSyscall write ]--------\n");
    
    uint_reg R_mock[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    R_mock[SRAX_OFFSET] = 0x1;
    R_mock[SRDI_OFFSET] = 0x1;
    R_mock[SRSI_OFFSET] = (uint_reg)(R_mock + STK_OFFSET);
    R_mock[SRDX_OFFSET] = 0xC;
    R_mock[STK_OFFSET] = 0x6F57206F6C6C6548;
    R_mock[STK_OFFSET+1]  = 0x0A646C72;

    doSyscall(R_mock);

    printf("Bytes Written 0x%lX\n", R_mock[SRAX_OFFSET]);
}



void test_changeRegs(){
    printf("--------[ Test Update SHL Swap ]--------\n");
    uint_reg R_mock[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    uint8_t step = 0x0;
    
    for (step=0; step < 32; step++){
        doSHLReg( &(R_mock[VAL_OFFSET]) );
        doUpdateReg( &(R_mock[VAL_OFFSET]), step&0xF );
        printf("step: %02X -- PTR:%016lX,  VAL: 0x%016lX\n", step&0xF, R_mock[PTR_OFFSET], R_mock[VAL_OFFSET]);
        doSwap(R_mock);
    }
}

void test_Pointers(){
    printf("-----[ Test Base Add Deref Assign ]-----\n");
    uint_reg R_mock[16] = {0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0};
    
    R_mock[SRAX_OFFSET] = 0x1;
    R_mock[SRDI_OFFSET] = 0x1;
    R_mock[SRSI_OFFSET] = (uint_reg)(R_mock + STK_OFFSET);
    R_mock[SRDX_OFFSET] = 0xC;

    printRegs(R_mock, 0);

    update(R_mock, 0x13);   // PTR = BASE_ADDR 
    update(R_mock, 0x02);   // CLR VAL
    update(R_mock, 0x32);   // UVAL [RAX_OFFSET_BYTES] 
    update(R_mock, 0x1D);   // SHVAL 
    update(R_mock, 0x0F);   // PTR += VAL
    update(R_mock, 0x11);   // *PTR = VAL
    
    printRegs(R_mock, 0);

    update(R_mock, 0x13);   // PTR = BASE_ADDR 
    update(R_mock, 0x02);   // CLR VAL
    update(R_mock, 0x33);
    update(R_mock, 0x1D);   // SHVAL 
    update(R_mock, 0x38);
    update(R_mock, 0x0F);   // PTR += VAL
    update(R_mock, 0x12);   // VAL = *PTR
    update(R_mock, 0x10);   // SWAP 
    
    printRegs(R_mock, 0);
}

void test_PushPop(){
    printf("------[ Test Push Pop PrintRegs ]-------\n");
    uint_reg R_mock[16] = {9,9,0,0,0,0,0,0,0,0,0, 0,0,0,0,0};
    uint_reg *R_val = &(R_mock[VAL_OFFSET]);
    uint8_t step = 0x0;
    
    for (step=0; step < 8; step++){
        doUpdateReg(R_val, step);
        doSHLReg(R_val);
    }
    printf("Push R: 0x%lX\n", *R_val);
    doPushReg( R_val );
    printRegs(R_mock, 4);
    doPushReg( R_val );
    doPushReg( R_val );
    
    for (step=0xf; step >8 ; step--){
        doUpdateReg(R_val, step);
        doSHLReg(R_val);
    }
    printf("Push R: 0x%lX\n", *R_val);
    doPushReg( R_val );
    printRegs(R_mock, 6);
    doPopReg(R_val);
    printRegs(R_mock, 6);
    doPopReg(R_val);
    printRegs(R_mock, 6);
     
    for (step=0; step < 8; step ++){doSHLReg(R_val);}
    doPushReg(R_val);
    doPushReg(R_val);
    doPushReg(R_val);
    printRegs(R_mock, 8);
    doPopReg(R_val);
    doPopReg(R_val);
    doPopReg(R_val);
    doPopReg(R_val);
    doPopReg(R_val);
    doPopReg(R_val);
    printRegs(R_mock, 4);
}

int test_asmSyscall(){
    printf("------------[ Test syscall ]------------\n");
    int ret; 
    asm volatile(
        "mov  $0x0A646C72, %%rax\n"
        "push %%rax\n"
        "mov  $0x6F57206F6C6C6548, %%rax\n"
        "push %%rax\n"
        "mov $0x1, %%rax\n"
        "mov $0x1, %%rdi\n"
        "mov %%rsp, %%rsi\n"
        "mov $0x0C, %%rdx\n"
        "syscall\n"
        :"=a"(ret):: "rdi", "rsi", "rdx");

    printf("Bytes Written: 0x%X\n", ret);
    return ret;
}

int test_generic(){
    bool b[5] = {1,1,1,0,1};
    char s[] = "TEST";
    uint_reg i =1;

    printf("%d, %d\n", (5 & 4), SYS_write);
    printf("%016lX\n",(i<<63));

    return 1;
}

int main(){
    printISA_short();
    //1 printISA();
    //1 test_asmSyscall();
    //1 test_doSyscall_write();
    //1 test_changeRegs();
    test_Pointers();
    // test_PushPop();
    // test_generic();
    // test_doChangeDerefR();
}
    
