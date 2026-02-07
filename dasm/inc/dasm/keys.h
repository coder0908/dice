#ifndef	dasm_keys_h
#define	dasm_keys_h

#define DASM_OPCODE_CNT 		8	/* DASM_OPCODE_CNT < 0xFFFFFFFF */
#define DASM_INSTRUCTION_MAX_TOKEN_CNT 	12	
#define DASM_TOKEN_MAX_LEN 		32	/* single operand, opcode, label etc... but not string(literal)*/

#define DASM_SET "set"
#define DASM_NOP "nop"
#define DASM_EOP "eop"
#define DASM_IADD "iadd"
#define DASM_ISUB "isub"
#define DASM_PUTI "puti"
#define DASM_JMP "jmp"
#define DASM_JMPZ "jmpz"

#endif
