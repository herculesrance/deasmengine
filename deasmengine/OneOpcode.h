#pragma   once

#include "deasmengine.h"
#include <string.h>

enum SUPERSCRIPTS
{  
	A1,		//Bits 5, 4, and 3 of ModR/M byte used as an opcode extension (refer to Section A.4, ¡°Opcode Extensions For One-ByteAnd Two - byte Opcodes¡±).
	B1,		//Use the 0F0B opcode (UD2 instruction), the 0FB9H opcode (UD1 instruction), or the 0FFFH opcode (UD0 instruction)when deliberately 
			//trying to generate an invalid opcode exception(#UD).
	C1,		//Some instructions use the same two-byte opcode. If the instruction has variations, or the opcode represents
			//different instructions, the ModR / M byte will be used to differentiate the instruction.For the value of the ModR / M
			//byte needed to decode the instruction, see Table A - 6.
	i64,	//The instruction is invalid or not encodable in 64-bit mode. 40 through 4F (single-byte INC and DEC) are REX prefix
			//combinations when in 64 - bit mode(use FE / FF Grp 4 and 5 for INC and DEC).
	o64,	//Instruction is only available when in 64-bit mode.
	d64,	//When in 64-bit mode, instruction defaults to 64-bit operand size and cannot encode 32-bit operand size.
	f64,	//The operand size is forced to a 64-bit operand size when in 64-bit mode (prefixes that change operand size are
			//ignored for this instruction in 64 - bit mode).
	v,		//VEX form only exists. There is no legacy SSE form of the instruction. For Integer GPR instructions it means VEX
			//prefix required.
	v1		//VEX128 & SSE forms only exist (no VEX256), when can¡¯t be inferred from the data size.
};

/*	A		Direct address: the instruction has no ModR/M byte; the address of the operand is encoded in the instruction.No base register, index register, or scaling factor can be applied(for example, far JMP(EA)).
	B		The VEX.vvvv field of the VEX prefix selects a general purpose register.
	C		The reg field of the ModR/M byte selects a control register (for example, MOV (0F20, 0F22)).
	D		The reg field of the ModR/M byte selects a debug register (for example,MOV (0F21,0F23)).
	E		 A ModR/M byte follows the opcode and specifies the operand. The operand is either a general-purposeregister or a memory address. If it is a memory address, the address is computed from a segment registerand any of the following values: a base register, an index register, a scaling factor, a displacement.
	F		 EFLAGS/RFLAGS Register.
	G		 The reg field of the ModR/M byte selects a general register (for example, AX (000)).
	H		The VEX.vvvv field of the VEX prefix selects a 128-bit XMM register or a 256-bit YMM register, determined by operand type. For legacy SSE encodings this operand does not exist, changing the instruction to destructive form.
	I		Immediate data: the operand value is encoded in subsequent bytes of the instruction.
	J		The instruction contains a relative offset to be added to the instruction pointer register (for example, JMP(0E9), LOOP).
	L		The upper 4 bits of the 8-bit immediate selects a 128-bit XMM register or a 256-bit YMM register, determined by operand type. (the MSB is ignored in 32-bit mode)
	M		The ModR/M byte may refer only to memory (for example, BOUND, LES, LDS, LSS, LFS, LGS,CMPXCHG8B).
	N		The R/M field of the ModR/M byte selects a packed-quadword, MMX technology register.
	O		The instruction has no ModR/M byte. The offset of the operand is coded as a word or double word(depending on address size attribute) in the instruction. No base register, index register, or scaling factorcan be applied (for example, MOV (A0¨CA3)).
	P		 The reg field of the ModR/M byte selects a packed quadword MMX technology register.
	Q		A ModR/M byte follows the opcode and specifies the operand. The operand is either an MMX technology register or a memory address. If it is a memory address, the address is computed from a segment register and any of the following values: a base register, an index register, a scaling factor, and a displacement.
	R		The R/M field of the ModR/M byte may refer only to a general register (for example, MOV (0F20-0F23)).
	S		 The reg field of the ModR/M byte selects a segment register (for example, MOV (8C,8E)).
	U		 The R/M field of the ModR/M byte selects a 128-bit XMM register or a 256-bit YMM register, determined by operand type.
	V		 The reg field of the ModR/M byte selects a 128-bit XMM register or a 256-bit YMM register, determined by operand type.
	W		 A ModR/M byte follows the opcode and specifies the operand. The operand is either a 128-bit XMM register,a 256-bit YMM register (determined by operand type), or a memory address. If it is a memory address, the address is computed from a segment register and any of the following values: a base register, an index register, a scaling factor, and a displacement.
	X		 Memory addressed by the DS:rSI register pair (for example, MOVS, CMPS, OUTS, or LODS).
	Y		 Memory addressed by the ES:rDI register pair (for example, MOVS, CMPS, INS, STOS, or SCAS).

	a		 Two one-word operands in memory or two double-word operands in memory, depending on operand-size attribute (used only by the BOUND instruction).
	b		 Byte, regardless of operand-size attribute.
	c		 Byte or word, depending on operand-size attribute.
	d		Doubleword, regardless of operand-size attribute.
	dq		 Double-quadword, regardless of operand-size attribute.
	p		32-bit, 48-bit, or 80-bit pointer, depending on operand-size attribute.
	pd		128-bit or 256-bit packed double-precision floating-point data.
	pi		 Quadword MMX technology register (for example: mm0).
	ps		 128-bit or 256-bit packed single-precision floating-point data.
	q		 Quadword, regardless of operand-size attribute.
	qq		 Quad-Quadword (256-bits), regardless of operand-size attribute.
	s		 6-byte or 10-byte pseudo-descriptor.
	sd		Scalar element of a 128-bit double-precision floating data.
	ss		Scalar element of a 128-bit single-precision floating data.
	si		Doubleword integer register (for example: eax).
	v		Word, doubleword or quadword (in 64-bit mode), depending on operand-size attribute.
	w		Word, regardless of operand-size attribute.
	x		 dq or qq based on the operand-size attribute.
	y		 Doubleword or quadword (in 64-bit mode), depending on operand-size attribute.
	z		Word for 16-bit operand-size or doubleword for 32 or 64-bit operand-size.

*/

/*
t t t n Mnemonic					 Condition
0 0 0 0		O						 Overflow
0 0 0 1	   NO						No overflow
0 0 1 0	    B, NAE			Below,Not above or equal
0 0 1 1	    NB, AE			 Not below,Above or equal
0 1 0 0	    E, Z					Equal,Zero
0 1 0 1	    NE, NZ				 Not equal, Not zero
0 1 1 0	    BE, NA				Below or equal,  Not above
0 1 1 1	    NBE, A				Not below or equal, Above
1 0 0 0	    S							Sign
1 0 0 1	    NS						 Not sign
1 0 1 0	    P, PE				 Parity, Parity Even
1 0 1 1	    NP, PO				 Not parity, Parity Odd
1 1 0 0	    L, NGE			 Less than, Not greater than or equal to
1 1 0 1	    NL, GE			 Not less than, Greater than or equal to
1 1 1 0	    LE, NG			 Less than or equal to, Not greater than
1 1 1 1	    NLE, G			 Not less than or equal to, Greater than
*/

const std::string TableOfChar[16] = { "0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F" };

const std::string General_Reg[255]{
	"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh","","","","","","","","",
   "ax", "cx", "dx", "bx", "sp", "bp", "si", "di","","","","","","","","",
	"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi","","","","","","","","",
	"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi","r8","r9","r10","r11","r12","r13","r14","r15"
};

typedef struct InsMap
{
	std::string OpCode;
	OPTYPE Op1Type;
	OPTYPE Op2Type;
	OPTYPE Op3Type;
	SUPERSCRIPTS ss;
}*INSMAP;

const std::string Group1[8] = { "add","or","adc","sbb","and","sub","xor","cmp" };

 const InsMap  OneOpmap[256] =
{
{"add" ,Eb,Gb,Zero,},          {"add" ,Ev,Gv,Zero,},          {"add" ,Eb,Gb,Zero,},           {"add" ,Gv,Ev,Zero,},            {"add" ,AL,Ib,Zero,},        {"add" ,rAX,Iz,Zero,},       { "push" ,ES,Zero,Zero,i64} ,     {"pop" ,ES,Zero,Zero,i64},
{"or"  ,Eb,Gb,Zero,},          {"or"  ,Ev,Gv,Zero,},          {"or"  ,Gb,Eb,Zero,},           {"or"  ,Gv,Ev,Zero,},            {"or"  ,AL,Ib,Zero, },       {"or"  ,rAX,Iz,Zero, },      { "push",CS,Zero,Zero,i64 } ,     {"2byte_escape"},
{"adc",Eb,Gb,},                {"adc",Ev,Gv,},                {"adc",Eb,Gb,},                 {"adc",Gv,Ev,},                  {"adc",AL,Ib,},              {"adc",rAX,Iz,},             { "push",SS,Zero,Zero,i64}  ,     {"pop",SS,Zero,Zero,i64},
{"sbb",Eb,Gb,},                {"sbb",Ev,Gv,},                {"sbb",Gb,Eb,},                 {"sbb",Gv,Ev,},                  {"sbb",AL,Ib,},              {"sbb",rAX,Iz,},             { "push",DS,Zero,Zero,i64}  ,     {"pop",DS,Zero,Zero,i64},
{"and",Eb,Gb,Zero,},           {"and",Ev,Gv,Zero,},           {"and",Gb,Eb,Zero,},            {"and",Gv,Ev,Zero,},             {"and",AL,Ib,Zero,},         {"and",rAX,Iz,Zero,},        { "Prefix_ES"},                   {"daa",Zero,Zero,Zero,i64},
{"sub",Eb,Gb,Zero,},           {"sub",Ev,Gv,Zero,},           {"sub",Gb,Eb,Zero,},            {"sub",Gv,Ev,Zero,},             {"sub",AL,Ib,Zero,},         {"sub",rAX,Iz,Zero,},        { "Prefix_CS"},                   {"das",Zero,Zero,Zero,i64},
{"xor",Eb,Gb,Zero,},           {"xor",Ev,Gv,Zero,},           {"xor",Gb,Eb,Zero,},            {"xor",Gv,Ev,Zero,},             {"xor",AL,Ib,Zero,},         {"xor",rAX,Iz,Zero,},        { "Prefix_SS"},                   {"aaa",Zero,Zero,Zero,i64},
{"cmp",Eb,Gb,Zero,},           {"cmp",Ev,Gv,Zero,},           {"cmp",Gb,Eb,Zero,},            {"cmp",Gv,Ev,Zero,},             {"cmp",AL,Ib,Zero,},         {"cmp",rAX,Iz,Zero,},        { "Prefix_DS"},                   {"aas",Zero,Zero,Zero,i64},
{"inc",eAX,Zero,Zero,i64},     {"inc",eCX,Zero,Zero,i64},     {"inc",eDX,Zero,Zero,i64 },     {"inc",eBX,Zero,Zero,i64 },      {"inc",eSP,Zero,Zero,i64 },  {"inc",eBP,Zero,Zero,i64 },  {"inc",eSI,Zero,Zero,i64 },       {"inc",eDI,Zero,Zero,i64 },
{"dec",eAX,Zero,Zero,i64},     {"dec",eCX,Zero,Zero,i64},     {"dec",eDX,Zero,Zero,i64 },     {"dec",eBX,Zero,Zero,i64 },      {"dec",eSP,Zero,Zero,i64 },  {"dec",eBP,Zero,Zero,i64 },  {"dec",eSI,Zero,Zero,i64 },       {"dec",eDI,Zero,Zero,i64 },
{"push",rAX,Zero,Zero,d64},    {"push",rCX,Zero,Zero,d64 },   {"push",rDX,Zero,Zero,d64},     {"push",rBX,Zero,Zero,d64 },     {"push",rSP,Zero,Zero,d64},  {"push",rBP,Zero,Zero,d64 }, {"push",rSI,Zero,Zero,d64 },      { "push",rDI,Zero,Zero,d64 },
{"pop",rAX,Zero,Zero,d64},     {"pop",rCX,Zero,Zero,d64 },    {"pop",rDX,Zero,Zero,d64},      {"pop",rBX,Zero,Zero,d64 },      {"pop",rSP,Zero,Zero,d64 },  {"pop",rBP,Zero,Zero,d64 },  {"pop",rSI,Zero,Zero,d64 },       { "pop",rDI,Zero,Zero,d64 },
{"pushA",Zero,Zero,Zero,i64},  {"popA",Zero,Zero,Zero,i64},   {"bound",Gv,Ma,Zero,i64},       {"arpl",Ew,Gw,Zero,i64},         {"Prefix_Fs"},               {"Prefix_Gs"},               {"Prefix_OpSize"},                {"Prefix_AddSize"},
{"pushA",Iz,Zero,Zero,d64},    {"imul",Gv,Ev,Iz,},            {"push",Ib,Zero,Zero,d64},      {"imul",Gv,Ev,Ib,i64},           {"ins",Yb,DX,Zero,},         {"ins",Yz,DX,Zero,},         {"outs",DX,Xb,Zero,},              {"outs",DX,Xz,Zero,},
{"jo",},{"jno",},              {"jb",},{"jnb",},              {"jz",},{"jnz",},               {"jbe",},{"ja"}  ,               {"js",},{"jns",},            {"jp",},{"jnp",},            {"jl",},{"jnl",},                 {"jng",},{"jg"},
{"Imm_Grp1",Eb,Ib,Zero,A1},    {"Imm_Grp1",Ev,Iz,Zero,A1},    {"Imm_Grp1",Eb,Ib,Zero,A1},     {"Imm_Grp1",Ev,Ib,Zero,A1},      {"test",Eb,Gb,Zero,},        {"test",Ev,Gv,Zero,},        {"xchg",Eb,Gb,Zero,},             {"xchg",Ev,Gv,Zero,},
{"mov",Eb,Gb,Zero,},           {"mov",Ev,Gv,Zero,},           {"mov",Gb,Eb,Zero,},            {"mov",Gv,Ev,Zero,},             {"mov",Ev,Sw,Zero,},         {"lea",Gv,M,Zero,},          {"mov",Sw,Ew,Zero,},              {"pop",Ev,Zero,Zero,d64},
{"nop",},                      {"xchg",rAX,rCX,Zero,},        {"xchg",rAX,rDX,Zero,},         {"xchg",rAX,rBX,Zero,},          {"xchg",rSP,rCX,Zero,},      {"xchg",rAX,rBP,Zero,},      {"xchg",rSI,rCX,Zero,},           {"xchg",rDI,rCX,Zero,},
{"cbw",},                      {"cwd",},                      {"far_call",Ap,Zero,Zero,i64},  {"fwit",},                       {"push?",Fv,Zero,Zero,},     {"pop?",Fv,Zero,Zero,},      {"sahf",},                        {"lahf"},
{"mov",AL,Ob,Zero,},           {"mov",rAX,Ov,Zero,},          {"mov",Ob,AL,Zero,},            {"mov",Ov,rAX,Zero,},            {"movs",Yb,Xb,Zero,},        {"movs",Yv,Xv,Zero,},        {"CMPS",Xb,Yb,Zero,},             {"CMPS",Xv,Yv,Zero,},
{"test",AL,Ib,Zero,},          {"test",rAX,Iz,Zero,},         {"stos",Yb,AL,Zero,},           {"stos",Yv,rAX,Zero,},           {"lods",AL,Xb,Zero,},        {"lods",rAX,Xv,Zero,},       {"scas",AL,Yb,Zero,},             {"scas",rAX,Yv,Zero,},
{"mov",AL,Ib,Zero,},           {"mov",CL,Ib,Zero,},           {"mov",DL,Ib,Zero,},            {"mov",BL,Ib,Zero,},             {"mov",AH,Ib,Zero,},         {"mov",CH,Ib,Zero,},         {"mov",DH,Ib,Zero,},              {"mov",BH,Ib,Zero,},
{"mov",rAX,Iv,Zero,},          {"mov",rCX,Iv,Zero,},          {"mov",rDX,Iv,Zero,},           {"mov",rBX,Iv,Zero,},            {"mov",rSP,Iv,Zero,},        {"mov",rBP,Iv,Zero,},        {"mov",rSI,Iv,Zero,},             {"mov",rDI,Iv,Zero,},
{"Shift_Grp2",Eb,Ib,Zero,A1},  {"Shift_Grp2",Ev,Ib,Zero,A1},  {"near ret",Iw,Zero,Zero,f64},  {"near ret",Zero,Zero,Zero,f64}, {"les",Gz,Mp,Zero,i64},      {"lds",Gz,Mp,Zero,i64},      {"mov_Grp11",Eb,Ib,Zero,A1},      {"mov_Grp11",Ev,Ib,Zero,A1},
{"enter",Iw,Ib,Zero,},         {"leave",Zero,Zero,Zero,d64},  {"far ret",Iw,Zero,Zero,},      {"far ret",},                    {"int3",},                   {"int",Ib,Zero,Zero,},       {"into",Zero,Zero,Zero,i64},      {"iret",},
{"Shift_Grp2",Eb,One,Zero,A1}, {"Shift_Grp2",Ev,One,Zero,A1}, {"Shift_Grp2",Eb,CL,Zero,A1},   {"Shift_Grp2",Ev,CL,Zero,A1},    {"aam",Ib,Zero,Zero,i64},    {"aad",Ib,Zero,Zero,i64},    {},                               {"xlat",},
{"esc",},                      {"esc",},                      {"esc",},                       {"esc",},                        {"esc",},                    {"esc",},                    {"esc",},                         {"esc",},
{"loopne",Jb,Zero,Zero,f64},   {"loope",Jb,Zero,Zero,f64},    {"loop",Jb,Zero,Zero,f64},      {"JrCXZ",Jb,Zero,Zero,f64},      {"in",AL,Ib,Zero,},          {"in",eAX,Ib,Zero,},         {"out",Ib,AL,Zero,},              {"out",Ib,eAX,Zero,},
{"near call",Jz,Zero,Zero,f64},{"jmp_near",Jz,Zero,Zero,f64}, {"jmp_far",Ap,Zero,Zero,i64},   {"jmp_short",Jb,Zero,Zero,f64},  {"in",AL,DX,Zero,},          {"in",eAX,DX,Zero,},         {"out",DX,AL,Zero,},              {"out",DX,eAX,Zero,},
{"lock",},					   {},                            {"repne",},					  {"rep",},						   {"hlt",},				    {"cmc",},                    {"Unry_Grp3",Eb,Zero,Zero,A1},    {"Unry_Grp3",Ev,Zero,Zero,A1},
{"clc",},                      {"stc",},                      {"cli",},                       {"sti",},               {"cld",},                             {"std",},                    {"inc",},                         {"inc",}
};																																    




class OneOpcode {

public:
	OneOpcode();
	~OneOpcode();

	INSINFO *insinfo;
	
	std::string OprandG;
	std::string OprandE;

	int def_OpSize;
	int def_addressSize;
	void init_size(int mod);
	
	
	bool HasGetModrm ;

	unsigned char CodeToDeasm[15];
	unsigned int OnOneOpcode(outputinfo* output,int mod);

	void init_insinfo(int mod);
	
	void		 GetRex();
	void		 GetModrm();
	void		 GetSib();
	std::string  GetGeneralReg(int codeindex);
	std::string  GetOprandE();
	std::string  GetImmediate64();
	std::string  GetImmediate32();
	std::string  GetImmediate16();
	std::string  GetImmediate8();
	void		 GetStrToDeasm(outputinfo* Output, int length);
	OPTYPE		 GetOp1Type();
	OPTYPE	     GetOp2Type();
	OPTYPE		 GetOp3Type();
	std::string  GetOpCode();
	std::string  GetOprand(outputinfo* output, int mod,OPTYPE otype);

	int AddressSizeOverride(int mod);
	int OpSizeOverride(int mod);

	std::string CharToStr(unsigned char ch);
	std::string HexToStr(long long int num);
	std::string IntToStr(long long int num);
	byte CharToByte(char ch);

};
