#pragma once

#include<Windows.h>
#include<string>
#include<map>

enum OPTYPE {
	Zero, One, Ap, Eb, Ev, Ew, Fv, Gb, Gv, Gw, Gz, Iz, Ib, Iv, Iw, Jz, Jb, M, Ma, Mp, Ob, Ov, Sw,
	AL, CL, DL, BL, AH, CH, DH, BH,
	rAX, rCX, rDX, rBX, rSP, rBP, rSI, rDI,
	DX, CS, DS, ES, SS, eAX, eBX, eCX, eDX, eSP, eBP, eSI, eDI,
	Xb, Xz, Xv, Yb, Yz, Yv, Grp4, Grp5
};


typedef struct Rex
{
	byte constant : 4;
	byte W : 1;
	byte R : 1;
	byte X : 1;
	byte B : 1;
}REX;

typedef struct Vex2
{
	byte Opcode = 0xC5;
	byte R : 1;
	byte vvvv : 4;
	byte L : 1;
	byte pp : 2;
}VEX2;

typedef struct Vex3
{
	byte Opcode = 0xC4;
	byte R : 1;
	byte X : 1;
	byte B : 1;
	byte m_mmmm : 5;
	byte W : 1;
	byte vvvv : 4;
	byte L : 1;
	byte pp : 2;
}VEX3;


typedef struct Ins_info
{
	int Mod;
	Rex REX;
	VEX2 vex2;
	VEX3 vex3;
	byte opcode[4];
	OPTYPE Op1Type;
	OPTYPE Op2Type;
	OPTYPE Op3Type;
	struct Imme
	{
		union
		{
			byte Immediate8;
			WORD Immediate16;
			DWORD Immediate32;
			long long Immediate64;
		};
	} Immediate;

	struct modrm
	{
	byte mod : 2;
	byte reg : 3;
	byte rm : 3;
	}Modrm;

	struct sib
	{
		byte scale : 2;
		byte index : 3;
		byte base : 3;
	}Sib;

	int cindex;

	struct Displacement
	{
		union
		{
			byte displacement8;
			WORD displacement16;
			DWORD displacement32;
			};
	}DISPLACEMENT;
	
	int Eff_OpSize;
	int Eff_AddressSize;

	} INSINFO;

//typedef struct Imme
//{
//	union
//	{
//		byte Immediate8;
//		WORD Immediate16;
//		DWORD Immediate32;
//		long long Immediate64;
//	};
//}IMMEDIATE;
//
//typedef struct modrm {
//	byte mod : 2;
//	byte reg : 3;
//	byte rm : 3;
//}MODRM;
//
//typedef struct sib
//{
//	byte scale : 2;
//	byte index : 3;
//	byte base : 3;
//}SIB;

typedef struct outputinfo
{
	std::string CodeToDeasmmble;
	std::string deasmed;
	int length;
	unsigned long address;

}OUTPUT;


