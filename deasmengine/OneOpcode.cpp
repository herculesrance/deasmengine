#include "OneOpcode.h"
#include <string.h>
#include <stdio.h>

#include <iostream>





OneOpcode::OneOpcode()  
{
	CodeToDeasm[15] = 0;
	insinfo= new INSINFO;
	insinfo->cindex = 0;
	insinfo->Mod = 32;
	insinfo->Eff_OpSize = 32;
	HasGetModrm = false;
	insinfo->REX.constant = 4;
};
OneOpcode::~OneOpcode()
{
	
	std::free(insinfo);
	
};

//十进制整型转成string串
std::string OneOpcode::IntToStr(long long int num)
{
	int i = 0, j = 0, isNeg = 0;
	char temp[255];
	memset(temp, 0, 255);
	std::string *str_of_int = new std::string;


	if (num < 0)
	{
		num *= -1;
		isNeg = 1;
	}
	do
	{
		temp[i++] = (num % 10) + '0';
		num /= 10;
	} while (num);

	if (isNeg)
		temp[i++] = '-';

	while (i >= j)
	*str_of_int += temp[j++];
	return *str_of_int;
	}

//十六进制整型转成string串
std::string OneOpcode::HexToStr(long long int num)
{
	int i = 0, j = 0, isNeg = 0;
	char temp[255];
	memset(temp, 0, 255);
	std::string* str_of_int = new std::string;
	
	if (num < 0)
	{
		num *= -1;
		isNeg = 1;
	}
	do
	{
		if (num % 0x10 >= 0 && num % 0x10 <= 9)
			temp[i++] = (num % 0x10) + 0x30;
		else
			temp[i++] = (num % 0x10) + 0x37;
		num /= 0x10;
	} while (num);
	if (isNeg)
		temp[i++] = '-';
	while (i > 0)
	{
		i--;
		*str_of_int += temp[i];
	}
	return "0x"+*str_of_int;
}
//char字符转string
std::string OneOpcode::CharToStr(unsigned char ch)
{	
	//int a = (unsigned char)ch / 0x10;
	//std::string b = TableOfChar[a];
	//int c = (unsigned char)ch % 0x10;
	//b += TableOfChar[c];
	//return b;
	return TableOfChar[ch / 0x10] + TableOfChar[ch % 0x10];
	//return TableOfChar[(unsigned char)ch / 0x10]+TableOfChar[(unsigned char)ch%0x10];
};
//char字符转byte
byte OneOpcode::CharToByte(char ch)
{
	if (0x30 <= ch && ch <= 0x39)
		return ch - 0x30;
	else if (0x41 <= ch && ch <= 0x46)
		return ch - 0x37;
	else if (0x61 <= ch && ch <= 0x66)
		return ch -0x57;
	else
		return 0;
}


//得到rex
void OneOpcode::GetRex()
{
		byte temp = CodeToDeasm[insinfo->cindex];
		insinfo->REX.constant =4;
		insinfo->REX.W = (temp << 4) >> 7;
		insinfo->REX.R = (temp << 5) >> 7;
		insinfo->REX.X= 0;
		insinfo->REX.B = (temp << 7) >> 7;
		insinfo->cindex++;
}
//得到modrm
void OneOpcode::GetModrm()
{
	byte temp = CodeToDeasm[insinfo->cindex];

	insinfo->Modrm.mod = temp >> 6;
	insinfo->Modrm.reg = (temp << 2) >> 5;
	insinfo->Modrm.rm = (temp << 5) >> 5;
	insinfo->cindex++;
}
//得到sib
void OneOpcode::GetSib() 
{
	byte temp =CodeToDeasm[insinfo->cindex];
	insinfo->Sib.scale = temp >> 6;
	insinfo->Sib.index = (temp << 2) >> 5;
	insinfo->Sib.base = (temp <<5)>>5;
	insinfo->cindex++;
}
//分析完一条语句之后，根据长度得到该语句的机器码。
void OneOpcode::GetStrToDeasm(outputinfo* Output, int length)
{
   	std::string* str = new std::string;
   	for (int i = 0; i < length; i++)
   		* str += CharToStr(CodeToDeasm[i]);
   	Output->CodeToDeasmmble = str->substr(0);
   	std::free(str);
//	 for (int i = 0; i < length; i++)
//	 Output->CodeToDeasmmble += CharToStr(CodeToDeasm[i]).substr(0);
};
//根据环境得到General_Reg
std::string  OneOpcode::GetGeneralReg(int index) 
{
	if (insinfo->Eff_OpSize == 16)
	{
		return General_Reg[index + 16];
	}
	else if (insinfo->Eff_OpSize == 32)
	{
		return General_Reg[index + 32];
	}
	else if (insinfo->Eff_OpSize == 64)
	{
		return General_Reg[index + 48];
	}
	return General_Reg[index];
};
//查表得到Oprandtype,
OPTYPE  OneOpcode::GetOp1Type() 
{
	return OneOpmap[insinfo->opcode[insinfo->cindex]].Op1Type;
};
OPTYPE  OneOpcode::GetOp2Type()
{
	return OneOpmap[insinfo->opcode[insinfo->cindex]].Op2Type;
};
OPTYPE  OneOpcode::GetOp3Type()
{
	return OneOpmap[insinfo->opcode[insinfo->cindex]].Op3Type;
};
//查表得到Opode
std::string  OneOpcode::GetOpCode()
{
	return OneOpmap[insinfo->opcode[insinfo->cindex]].OpCode;
};
//得到立即数
std::string OneOpcode::GetImmediate64()
{
	insinfo->Immediate.Immediate64 = CodeToDeasm[insinfo->cindex] + (CodeToDeasm[insinfo->cindex + 1] << 8) + (CodeToDeasm[insinfo->cindex + 2] << 16) + (CodeToDeasm[insinfo->cindex + 3] << 24)
		+ (long long)(CodeToDeasm[insinfo->cindex + 4] << 32) + (long long)(CodeToDeasm[insinfo->cindex + 5] << 40) + (long long)(CodeToDeasm[insinfo->cindex + 6] <<48) + (long long)(CodeToDeasm[insinfo->cindex + 7]<<56);
	insinfo->cindex += 8;
	return HexToStr(insinfo->Immediate.Immediate64);

};
std::string OneOpcode::GetImmediate32()
{
	insinfo->Immediate.Immediate32 = CodeToDeasm[insinfo->cindex] + (CodeToDeasm[insinfo->cindex + 1] << 8) + (CodeToDeasm[insinfo->cindex + 2] << 16) + (CodeToDeasm[insinfo->cindex + 3]<<24);
	insinfo->cindex += 4;
	return HexToStr(insinfo->Immediate.Immediate32);

};
std::string OneOpcode::GetImmediate16()
{
	insinfo->Immediate.Immediate16 = CodeToDeasm[insinfo->cindex] + (CodeToDeasm[insinfo->cindex + 1] << 8);
	insinfo->cindex += 2;
	return HexToStr(insinfo->Immediate.Immediate16);

};
std::string OneOpcode::GetImmediate8()
{
	insinfo->Immediate.Immediate8 = CodeToDeasm[insinfo->cindex++];
	return HexToStr(insinfo->Immediate.Immediate8);

};

/*Zero, One,Ap,Eb,Ev,Ew,Fv,Gb,Gv,Gw,Gz,Iz,Ib,Iv,Iw,Jz,Jb,M,Ma,Mp,Ob,Ov,Sw,
AL, CL, DL, BL, AH, CH, DH, BH, 
rAX, rCX, rDX, rBX, rSP, rBP, rSI, rDI,
 DX,  CS, DS, ES, SS, eAX, eBX, eCX, eDX, eSP, eBP, eSI, eDI,
Xb,Xz,Xv,Yb,Yz,Yv, Grp4, Grp5  */

/*
Yb,Yz,Yv, Grp4, Grp5  */
std::string OneOpcode::GetOprand(outputinfo* output, int mod,OPTYPE otype)
{	
	std::string* str = new std::string;
	
	switch (otype)
	{
		case Zero:
			break;
		case One:
			*str = "1";
		case Eb: 
			insinfo->Eff_OpSize = 8;
			*str = GetOprandE();
			break;
		case Ev: case Ew:
			*str = GetOprandE();
			break;
		case Gb: case Gw: case Gz: case Gv:
			if (!HasGetModrm)
			{
				GetModrm();
				HasGetModrm = true;
			}
			*str = GetGeneralReg(insinfo->Modrm.reg);
			break;
		case Ib:  case Jb:
			*str = GetImmediate8();
			break;
		case Iv:
			if(insinfo->Eff_OpSize==64)
				* str = GetImmediate64();
			else if(insinfo->Eff_OpSize==32)
				* str = GetImmediate32();
			else if(insinfo->Eff_OpSize==16)
				* str =  GetImmediate16();
			break;
		case Iw: 
			*str = GetImmediate16();
			break;
		case Iz: case Jz:
			if(insinfo->Eff_OpSize==64|| insinfo->Eff_OpSize==32)
				* str =  GetImmediate32();
			else
				*str = GetImmediate16();
			break;
		case Ap://64位无效
			//if (insinfo->Eff_OpSize == 64)
			//{
			//	*str = ":" + GetImmediate64();
			//	str->insert(0, GetImmediate16());
			//}
			//else 
			if (insinfo->Eff_OpSize == 32)
			{
				*str = ":" + GetImmediate32();
				str->insert(0, GetImmediate16());
			}
			else if (insinfo->Eff_OpSize == 16)
			{
				*str = ":" + GetImmediate16();
				str->insert(0, GetImmediate16());
			}
			break;
		case Fv:
			if (insinfo->Eff_OpSize == 64)
				* str = "Q ";
			else if (insinfo->Eff_OpSize == 32)
				* str = "D ";
			else if (insinfo->Eff_OpSize == 16)
				* str = "F ";
			break;	
		case M: case Ma: case Mp:
			* str += GetOprandE();
			break;
		case Ob: 
			if (insinfo->Eff_AddressSize == 32)
				* str ="byte ptr ["+ GetImmediate32() + "]";
			else if (insinfo->Eff_AddressSize == 16)
				* str = "byte ptr [" + GetImmediate16() + "]";
		case Ov:
			if (insinfo->Eff_OpSize == 64 && insinfo->Eff_AddressSize == 32)
				* str = "qword ptr ["+ GetImmediate32() + "]";
			if (insinfo->Eff_OpSize == 64 && insinfo->Eff_AddressSize == 64)
				* str = "qword ptr [" + GetImmediate64() + "]";
			if (insinfo->Eff_OpSize == 32 && insinfo->Eff_AddressSize == 32)
				* str = "dword ptr [" + GetImmediate32() + "]";
			if (insinfo->Eff_OpSize == 32 && insinfo->Eff_AddressSize == 16)
				* str = "dword ptr [" + GetImmediate16() + "]";
			if (insinfo->Eff_OpSize == 16 && insinfo->Eff_AddressSize == 16)
				* str = "word ptr [" + GetImmediate16() + "]";
			if (insinfo->Eff_OpSize == 16 && insinfo->Eff_AddressSize == 32)
				* str = "word ptr [" + GetImmediate32() + "]";
			break;
		case Sw://有点不好处理,在8c指令时，这个必定是oprand2，而oprand1是32位，但是已经生成了。
			insinfo->Eff_OpSize = 16;
			*str = GetGeneralReg(insinfo->Modrm.reg);
			break;
		case Xb: 
			if (insinfo->Eff_AddressSize == 32)
				* str = "byte ptr DS:[ESI]";
			else if (insinfo->Eff_AddressSize == 16)
				* str = "byte ptr DS:[SI]";
			break;
		case Xz:
			if(insinfo->Eff_OpSize == 64|| insinfo->Eff_OpSize == 32)
			{
				if (insinfo->Eff_AddressSize == 32)
					* str = "dword ptr DS:[ESI]";
				else if (insinfo->Eff_AddressSize == 16)
					* str = "dword ptr DS:[SI]";
			}
			else if(insinfo->Eff_OpSize ==16)
			{
				if (insinfo->Eff_AddressSize == 32)
					* str = "word ptr DS:[ESI]";
				else if (insinfo->Eff_AddressSize == 16)
					* str = "word ptr DS:[SI]";
			}
			break;
		case Xv:
			if (insinfo->Eff_OpSize == 64 && insinfo->Eff_AddressSize == 32)
				* str = "qword ptr DS:[ESI]";
			if (insinfo->Eff_OpSize == 64 && insinfo->Eff_AddressSize == 64)
				* str = "qword ptr DS:[RSI]";
			if (insinfo->Eff_OpSize == 32 && insinfo->Eff_AddressSize == 32)
				* str = "dword ptr DS:[ESI]";
			if (insinfo->Eff_OpSize == 32 && insinfo->Eff_AddressSize == 16)
				* str = "dword ptr DS:[SI]";
			if (insinfo->Eff_OpSize == 16 && insinfo->Eff_AddressSize == 16)
				* str = "word ptr DS:[SI]";
			if (insinfo->Eff_OpSize == 16 && insinfo->Eff_AddressSize == 32)
				* str = "word ptr DS:[ESI]";
			break;
		case Yb:
			if (insinfo->Eff_AddressSize == 32)
				* str = "byte ptr ES:[EDI]";
			else if (insinfo->Eff_AddressSize == 16)
				* str = "byte ptr ES:[DI]";
			break;
		case Yz:
			if (insinfo->Eff_OpSize == 64 || insinfo->Eff_OpSize == 32)
			{
				if (insinfo->Eff_AddressSize == 32)
					* str = "dword ptr ES:[EDI]";
				else if (insinfo->Eff_AddressSize == 16)
					* str = "dword ptr ES:[DI]";
			}
			else if (insinfo->Eff_OpSize == 16)
			{
				if (insinfo->Eff_AddressSize == 32)
					* str = "word ptr ES:[EDI]";
				else if (insinfo->Eff_AddressSize == 16)
					* str = "word ptr ES:[DI]";
			}
			break;
		case Yv:
			if (insinfo->Eff_OpSize == 64 && insinfo->Eff_AddressSize == 32)
				* str = "qword ptr ES:[EDI]";
			if (insinfo->Eff_OpSize == 64 && insinfo->Eff_AddressSize == 64)
				* str = "qword ptr ES:[RDI]";
			if (insinfo->Eff_OpSize == 32 && insinfo->Eff_AddressSize == 32)
				* str = "dword ptr ES:[EDI]";
			if (insinfo->Eff_OpSize == 32 && insinfo->Eff_AddressSize == 16)
				* str = "dword ptr ES:[DI]";
			if (insinfo->Eff_OpSize == 16 && insinfo->Eff_AddressSize == 16)
				* str = "word ptr ES:[DI]";
			if (insinfo->Eff_OpSize == 16 && insinfo->Eff_AddressSize == 32)
				* str = "word ptr ES:[EDI]";
			break;

		case AL:*str = "al";break;
		case CL:*str = "cl"; break;
		case DL:*str = "dl"; break;
		case BL:*str = "bl"; break;
		case AH:*str = "ah"; break;
		case CH:*str = "ch"; break;
		case DH:*str = "dh"; break;
		case BH:*str = "bh"; break;
		case DX:*str = "dx"; break;
		case CS:*str = "cs"; break;
		case DS:*str = "dh"; break;
		case ES:*str = "es"; break;
		case SS:*str = "ss"; break;

		//case eAX:
		//	if(insinfo->Eff_OpSize==32)
		//	*str = "eax";
		//	if(insinfo)
		//	break;
		//case eCX:*str = "eCX"; break;
		//case eDX:*str = "eDX"; break;
		//case eBX:*str = "eBX"; break;
		//case eSP:*str = "eSP"; break;
		//case eBP:*str = "eBP"; break;
		//case eSI:*str = "eSI"; break;
		//case eDI:*str = "eDI"; break;
		case eAX:case eCX:case eDX:case eBX:case eSP:case eBP:case eSI:case eDI:
			*str = GetGeneralReg(insinfo->opcode[insinfo->cindex-1] & 7);
			break;
		case rAX:case rCX:case rDX:case rBX:case rSP:case rBP:case rSI:case rDI:
			//int a = insinfo->opcode[insinfo->cindex-1] & 7 + (insinfo->REX.R << 4);
			* str = GetGeneralReg(insinfo->opcode[insinfo->cindex-1]&7+(insinfo->REX.R<<4));
			break;
					
	}
	return *str;
};

//几种寻址方式在这个函数里
std::string  OneOpcode::GetOprandE()
{
	std::string* oprE = new std::string;
	if (!HasGetModrm)
	{
		GetModrm();
		HasGetModrm = true;
	}
	switch (insinfo->Modrm.mod)
	{
	case 0:
	{
		if (insinfo->Modrm.rm == 4)
		{
			GetSib();
			switch (insinfo->Sib.scale)
			{
			case 0:
				*oprE = "[" + GetGeneralReg(insinfo->Sib.index) + "+" + GetGeneralReg(insinfo->Sib.base) + "]";
				break;
			case 1:
				*oprE = "[" + GetGeneralReg(insinfo->Sib.base) + "+" + GetGeneralReg(insinfo->Sib.index) + "*" + IntToStr(2) + "]";
				break;
			case 2:
				*oprE = "[" + GetGeneralReg(insinfo->Sib.base) + "+" + GetGeneralReg(insinfo->Sib.index) + "*" + IntToStr(4) + "]";
				break;
			case 3:
				*oprE = "[" + GetGeneralReg(insinfo->Sib.base) + "+" + GetGeneralReg(insinfo->Sib.index) + "*" + IntToStr(8) + "]";
				break;
			}
			//if (insinfo->Sib.scale == 0)
			//	*oprE = "[" + GetGeneralReg(insinfo->Sib.index) +"+"+ GetGeneralReg(insinfo->Sib.base) + "]";
			//else
			//	*oprE = "[" + GetGeneralReg(insinfo->Sib.base) + "+" + GetGeneralReg(insinfo->Sib.index) + "*" + IntToStr(insinfo->Sib.scale) + "]";
		}
		else if (insinfo->Modrm.rm == 5)
		{
			insinfo->DISPLACEMENT.displacement32 = (CodeToDeasm[insinfo->cindex] << 24) + (CodeToDeasm[insinfo->cindex + 1] << 16) + (CodeToDeasm[insinfo->cindex + 2] << 8) + CodeToDeasm[insinfo->cindex + 3];
			insinfo->cindex += 4;
			*oprE = "[" + HexToStr(insinfo->DISPLACEMENT.displacement32) + "+" + GetGeneralReg(insinfo->Sib.base) + "]";
		}
		else
			*oprE = "[" + GetGeneralReg(insinfo->Modrm.rm) + "]";
		break;
	}
	case 1:
	{
		if (insinfo->Modrm.rm == 4)
		{
			GetSib();
			insinfo->DISPLACEMENT.displacement8 = CodeToDeasm[insinfo->cindex++];
			if (insinfo->Sib.scale == 0)
				* oprE = "[" + GetGeneralReg(insinfo->Sib.index) + +"+" + GetGeneralReg(insinfo->Sib.base) + "+" + HexToStr(insinfo->DISPLACEMENT.displacement8) + "]";
			else
				*oprE = "[" + GetGeneralReg(insinfo->Sib.index) + "*" + IntToStr(insinfo->Sib.scale) + "+" + HexToStr(insinfo->DISPLACEMENT.displacement8) + "+" + GetGeneralReg(insinfo->Sib.base) + "]";
		}
		else
		{
			insinfo->DISPLACEMENT.displacement8 = CodeToDeasm[insinfo->cindex++];
			*oprE = "[" + GetGeneralReg(insinfo->Modrm.rm) +"+"+HexToStr(insinfo->DISPLACEMENT.displacement8) + "]";;
			break;
		}
	}
	case 2:
	{
		if (insinfo->Modrm.rm == 4)
		{
			GetSib();
			insinfo->DISPLACEMENT.displacement32 = (CodeToDeasm[insinfo->cindex] << 24) + (CodeToDeasm[insinfo->cindex + 1] << 16) + (CodeToDeasm[insinfo->cindex + 2] << 8) + CodeToDeasm[insinfo->cindex + 3];
			insinfo->cindex += 4;
			if (insinfo->Sib.scale == 0)
				* oprE = "[" + GetGeneralReg(insinfo->Sib.index) + +"+" + GetGeneralReg(insinfo->Sib.base) + "+" + HexToStr(insinfo->DISPLACEMENT.displacement32) + "]";
			else
				*oprE = "[" + GetGeneralReg(insinfo->Sib.index) + "*" + IntToStr(insinfo->Sib.scale) + "+" + HexToStr(insinfo->DISPLACEMENT.displacement32) + "+" + GetGeneralReg(insinfo->Sib.base) + "]";

		}
		else
		{
			insinfo->DISPLACEMENT.displacement32 = CodeToDeasm[insinfo->cindex];
			insinfo->cindex += 2;
			*oprE = "[" + GetGeneralReg(insinfo->Modrm.rm) + HexToStr(insinfo->DISPLACEMENT.displacement32) + "]";;
			break;
		}
	}
	case 3:
		*oprE = GetGeneralReg(insinfo->Modrm.rm);
		break;
	}
	return *oprE;
}


//根据设定的mod值,来初始化默认opsize和addresssize.构建insinfo结构后,只需要执行一次.
void OneOpcode::init_size(int mod)
{
	if (mod == 16)
	{
		def_OpSize = 16;
		def_addressSize = 16;
	}
	else if (mod == 32)
	{
		def_OpSize = 32;
		def_addressSize = 32;
	}

	else if (mod == 64)
	{
		def_OpSize = 32;
		def_addressSize = 64;
	}
};
//初始化iniinfo数据结构,每次解析一条指令前都需要初始化.
void OneOpcode::init_insinfo(int mod)
{
		memset(insinfo, 0, sizeof(INSINFO));
		insinfo->Mod = mod;
		insinfo->Eff_OpSize = def_OpSize;
		insinfo->Eff_AddressSize = def_addressSize;
		
};
//oprand大小重写
int OneOpcode::OpSizeOverride(int mod)
{
	if (mod == 16)
	{
		if (insinfo->Eff_OpSize == 32)
			insinfo->Eff_OpSize = 16;
		else if (insinfo->Eff_OpSize == 16)
			insinfo->Eff_OpSize = 32;
	}
	else if(mod==32)
	{
		if (insinfo->Eff_OpSize == 32)
			insinfo->Eff_OpSize = 16;
		else if (insinfo->Eff_OpSize == 16)
			insinfo->Eff_OpSize = 32;
	}
	else if (mod == 64)
	{
		if (insinfo->Eff_OpSize == 32)
		{
			if(insinfo->REX.W)
				insinfo->Eff_OpSize = 64;
			else 
				insinfo->Eff_OpSize = 16;
		}
		
		else if (insinfo->Eff_OpSize == 64)
			insinfo->Eff_OpSize = 16;

	}

	return insinfo->Eff_OpSize;
};
//address大小重写
int OneOpcode::AddressSizeOverride(int mod)
{
	if (mod == 16)
	{
		if (insinfo->Eff_AddressSize == 32)
			insinfo->Eff_AddressSize = 16;
		else if (insinfo->Eff_AddressSize == 16)
			insinfo->Eff_AddressSize = 32;
	}
	else if (mod == 32)
	{
		if (insinfo->Eff_AddressSize == 32)
			insinfo->Eff_AddressSize = 16;
		else if (insinfo->Eff_AddressSize == 16)
			insinfo->Eff_AddressSize = 32;
	}
	else if (mod == 64)
	{
		if (insinfo->Eff_AddressSize == 32)
			insinfo->Eff_AddressSize = 64;
		else if (insinfo->Eff_AddressSize == 64)
			insinfo->Eff_AddressSize = 32;
	}

	return insinfo->Eff_AddressSize;
};

void Process2ByteOpcode() 
{

};


//解析的主逻辑函数
unsigned int OneOpcode::OnOneOpcode(outputinfo *output,int mod)
{
	init_insinfo(mod);
	int op_prefix;
	op_prefix=
	insinfo->opcode[insinfo->cindex] =CodeToDeasm[insinfo->cindex];
	
	switch (op_prefix)//前缀的处理
	{
	
	case 0xF2:	//repne前缀
		insinfo->opcode[insinfo->cindex] = CodeToDeasm[insinfo->cindex+1];
		if (insinfo->opcode[insinfo->cindex] == 0xF0 || insinfo->opcode[insinfo->cindex] == 0x88 || insinfo->opcode[insinfo->cindex] == 0x89 ||
			insinfo->opcode[insinfo->cindex] == 0xC6 || insinfo->opcode[insinfo->cindex] == 0xC7 ||
			(insinfo->opcode[insinfo->cindex] >= 0x90 && insinfo->opcode[insinfo->cindex] <= 0x97))
			output->deasmed = "XACQUIRE" + GetOpCode() + ":";
		else
			output->deasmed = GetOpCode();
		insinfo->cindex++;
		insinfo->opcode[insinfo->cindex] = CodeToDeasm[insinfo->cindex];
		break;
	case 0xF3: //repe前缀
		insinfo->opcode[insinfo->cindex] = CodeToDeasm[insinfo->cindex];
		if (insinfo->opcode[insinfo->cindex] == 0xF0 || insinfo->opcode[insinfo->cindex] == 0x88 || insinfo->opcode[insinfo->cindex] == 0x89 ||
		   insinfo->opcode[insinfo->cindex] == 0xC6 || insinfo->opcode[insinfo->cindex] == 0xC7 ||
		   (insinfo->opcode[insinfo->cindex] >= 0x90 && insinfo->opcode[insinfo->cindex] <= 0x97))
			output->deasmed = "XRELEASE" + GetOpCode() + ":";
		insinfo->cindex++;
		break;
	case 0xF0:	 //lock前缀
		output->deasmed = GetOpCode() + ":";
		insinfo->cindex++;
		insinfo->opcode[insinfo->cindex] = CodeToDeasm[insinfo->cindex];
		output->deasmed += GetOpCode() + " ";
		break;
	case 0x26:case 0x36:case 0x64:case 0x65:case 0x2e:case 0x3e:  //段前缀
		insinfo->opcode[insinfo->cindex] = CodeToDeasm[insinfo->cindex++];
		break;
	case 0x66: //oprand大小重写前缀
		OpSizeOverride(mod);
		insinfo->cindex++;
		insinfo->opcode[insinfo->cindex] = CodeToDeasm[insinfo->cindex];
		output->deasmed = output->deasmed+GetOpCode()+" ";
		break;
	case 0x67://address大小重写前缀
		AddressSizeOverride(mod);
		insinfo->cindex++;
		insinfo->opcode[insinfo->cindex] = CodeToDeasm[insinfo->cindex];
		output->deasmed = output->deasmed + GetOpCode() + " ";
		break;
	case 0x40:case 0x41:case 0x42:case 0x43:case 0x44:case 0x45:case 0x46:case 0x47:	//32位时为inc.dec指令;64位时为rex前缀
	case 0x48:case 0x49:case 0x4A:case 0x4B:case 0x4C:case 0x4D:case 0x4E:case 0x4F:
		if (mod == 64)
			GetRex();
		break;
	default:
		break;
	};

	switch (insinfo->opcode[insinfo->cindex])
	{
	case 0x80: case 0x81: case 0x82: case 0x83: //处理group1
		insinfo->cindex++;
		if (!HasGetModrm)
		{
			GetModrm();
			HasGetModrm = true;
		}
		output->deasmed = output->deasmed + Group1[insinfo->Modrm.reg] + " ";
		insinfo->cindex -= 2;
		insinfo->Op1Type = GetOp1Type();
		insinfo->Op2Type = GetOp2Type();
		insinfo->Op3Type = GetOp3Type();
		insinfo->cindex += 2;
		if (insinfo->Op1Type)
			output->deasmed += GetOprand(output, mod, insinfo->Op1Type) + " ";
		if (insinfo->Op2Type)
			output->deasmed += GetOprand(output, mod, insinfo->Op2Type) + " ";
		if (insinfo->Op3Type)
			output->deasmed += GetOprand(output, mod, insinfo->Op3Type) + " ";
		output->length = insinfo->cindex;
		GetStrToDeasm(output, insinfo->cindex);
		return output->length;
	case 0x50:case 0x51:case 0x52:case 0x53:case 0x54:case 0x55:case 0x56:case 0x57:
	case 0x58:case 0x59:case 0x5A:case 0x5B:case 0x5C:case 0x5D:case 0x5E:case 0x5F:
		if (mod == 64)
			insinfo->Eff_OpSize = 64;
			output->deasmed = GetOpCode() + " ";
			
		break;
	case 0x0F:
		Process2ByteOpcode();
		break;
	default:
		//insinfo->opcode[insinfo->cindex] = CodeToDeasm[insinfo->cindex];
		output->deasmed = GetOpCode() + " ";
	}
	
	insinfo->Op1Type = GetOp1Type();
	insinfo->Op2Type = GetOp2Type();
	insinfo->Op3Type = GetOp3Type();
	insinfo->cindex++;
	if(insinfo->Op1Type)
		output->deasmed += GetOprand(output, mod, insinfo->Op1Type)+" ";
	if(insinfo->Op2Type)
		output->deasmed += GetOprand(output, mod, insinfo->Op2Type) + " ";
	if (insinfo->Op3Type)
		output->deasmed += GetOprand(output, mod, insinfo->Op3Type) + " ";
	
	
	output->length = insinfo->cindex;
	GetStrToDeasm(output, insinfo->cindex);
	return output->length;
};

