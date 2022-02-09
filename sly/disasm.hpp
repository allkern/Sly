/* 65816.c
 * 65816/6502 module for DisPel
 * James Churchill
 * Created 230900
 * Last Modified 240900
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

//#include "dispel.h"

/* disasm() - disassembles a single instruction
 * Pre:  mem   - pointer to memory for disassembly
 *       pos   - "address" of the instruction
 *       inst  - pointer to string buffer
 *       flag  - current processor state
 *       tsrc  - 1 if addresses/hex dump is to be suppressed.
 * Post: inst  - disassembled instruction
 *       returns number of bytes to advance, or 0 for error.
 */

int disasm(unsigned char *mem, unsigned long pos, unsigned char *flag, char *inst, unsigned char tsrc)
{
	// temp buffers to hold instruction,parameters and hex
	char ibuf[5],pbuf[20],hbuf[9];
	// variables to hold the instruction increment and signed params
	int offset,sval,i;

	// Parse out instruction mnemonic

	switch (mem[0])
	{
		// ADC
	case 0x69:
	case 0x6D:
	case 0x6F:
	case 0x65:
	case 0x72:
	case 0x67:
	case 0x7D:
	case 0x7F:
	case 0x79:
	case 0x75:
	case 0x61:
	case 0x71:
	case 0x77:
	case 0x63:
	case 0x73:
		std::strcpy(ibuf,"adc");
		break;
		// AND
	case 0x29:
	case 0x2D:
	case 0x2F:
	case 0x25:
	case 0x32:
	case 0x27:
	case 0x3D:
	case 0x3F:
	case 0x39:
	case 0x35:
	case 0x21:
	case 0x31:
	case 0x37:
	case 0x23:
	case 0x33:
		std::strcpy(ibuf,"and");
		break;
		// ASL
	case 0x0A:
	case 0x0E:
	case 0x06:
	case 0x1E:
	case 0x16:
		std::strcpy(ibuf,"asl");
		break;
		// BCC
	case 0x90:
		std::strcpy(ibuf,"bcc");
		break;
		// BCS
	case 0xB0:
		std::strcpy(ibuf,"bcs");
		break;
		// BEQ
	case 0xF0:
		std::strcpy(ibuf,"beq");
		break;
		// BNE
	case 0xD0:
		std::strcpy(ibuf,"bne");
		break;
		// BMI
	case 0x30:
		std::strcpy(ibuf,"bmi");
		break;
		// BPL
	case 0x10:
		std::strcpy(ibuf,"bpl");
		break;
		// BVC
	case 0x50:
		std::strcpy(ibuf,"bvc");
		break;
		// BVS
	case 0x70:
		std::strcpy(ibuf,"bvs");
		break;
		// BRA
	case 0x80:
		std::strcpy(ibuf,"bra");
		break;
		// BRL
	case 0x82:
		std::strcpy(ibuf,"brl");
		break;
		// BIT
	case 0x89:
	case 0x2C:
	case 0x24:
	case 0x3C:
	case 0x34:
		std::strcpy(ibuf,"bit");
		break;
		// BRK
	case 0x00:
		std::strcpy(ibuf,"brk");
		break;
		// CLC
	case 0x18:
		std::strcpy(ibuf,"clc");
		break;
		// CLD
	case 0xD8:
		std::strcpy(ibuf,"cld");
		break;
		// CLI
	case 0x58:
		std::strcpy(ibuf,"cli");
		break;
		// CLV
	case 0xB8:
		std::strcpy(ibuf,"clv");
		break;
		// SEC
	case 0x38:
		std::strcpy(ibuf,"sec");
		break;
		// SED
	case 0xF8:
		std::strcpy(ibuf,"sed");
		break;
		// SEI
	case 0x78:
		std::strcpy(ibuf,"sei");
		break;
		// CMP
	case 0xC9:
	case 0xCD:
	case 0xCF:
	case 0xC5:
	case 0xD2:
	case 0xC7:
	case 0xDD:
	case 0xDF:
	case 0xD9:
	case 0xD5:
	case 0xC1:
	case 0xD1:
	case 0xD7:
	case 0xC3:
	case 0xD3:
		std::strcpy(ibuf,"cmp");
		break;
		// COP
	case 0x02:
		std::strcpy(ibuf,"cop");
		break;
		// CPX
	case 0xE0:
	case 0xEC:
	case 0xE4:
		std::strcpy(ibuf,"cpx");
		break;
		// CPY
	case 0xC0:
	case 0xCC:
	case 0xC4:
		std::strcpy(ibuf,"cpy");
		break;
		// DEC
	case 0x3A:
	case 0xCE:
	case 0xC6:
	case 0xDE:
	case 0xD6:
		std::strcpy(ibuf,"dec");
		break;
		// DEX
	case 0xCA:
		std::strcpy(ibuf,"dex");
		break;
		// DEY
	case 0x88:
		std::strcpy(ibuf,"dey");
		break;
		// EOR
	case 0x49:
	case 0x4D:
	case 0x4F:
	case 0x45:
	case 0x52:
	case 0x47:
	case 0x5D:
	case 0x5F:
	case 0x59:
	case 0x55:
	case 0x41:
	case 0x51:
	case 0x57:
	case 0x43:
	case 0x53:
		std::strcpy(ibuf,"eor");
		break;
		// INC
	case 0x1A:
	case 0xEE:
	case 0xE6:
	case 0xFE:
	case 0xF6:
		std::strcpy(ibuf,"inc");
		break;
		// INX
	case 0xE8:
		std::strcpy(ibuf,"inx");
		break;
		// INY
	case 0xC8:
		std::strcpy(ibuf,"iny");
		break;
		// JMP
	case 0x4C:
	case 0x6C:
	case 0x7C:
	case 0x5C:
	case 0xDC:
		std::strcpy(ibuf,"jmp");
		break;
		// JSR
	case 0x22:
	case 0x20:
	case 0xFC:
		std::strcpy(ibuf,"jsr");
		break;
		// LDA
	case 0xA9:
	case 0xAD:
	case 0xAF:
	case 0xA5:
	case 0xB2:
	case 0xA7:
	case 0xBD:
	case 0xBF:
	case 0xB9:
	case 0xB5:
	case 0xA1:
	case 0xB1:
	case 0xB7:
	case 0xA3:
	case 0xB3:
		std::strcpy(ibuf,"lda");
		break;
		// LDX
	case 0xA2:
	case 0xAE:
	case 0xA6:
	case 0xBE:
	case 0xB6:
		std::strcpy(ibuf,"ldx");
		break;
		// LDY
	case 0xA0:
	case 0xAC:
	case 0xA4:
	case 0xBC:
	case 0xB4:
		std::strcpy(ibuf,"ldy");
		break;
		// LSR
	case 0x4A:
	case 0x4E:
	case 0x46:
	case 0x5E:
	case 0x56:
		std::strcpy(ibuf,"lsr");
		break;
		// MVN
	case 0x54:
		std::strcpy(ibuf,"mvn");
		break;
		// MVP
	case 0x44:
		std::strcpy(ibuf,"mvp");
		break;
		// NOP
	case 0xEA:
		std::strcpy(ibuf,"nop");
		break;
		// ORA
	case 0x09:
	case 0x0D:
	case 0x0F:
	case 0x05:
	case 0x12:
	case 0x07:
	case 0x1D:
	case 0x1F:
	case 0x19:
	case 0x15:
	case 0x01:
	case 0x11:
	case 0x17:
	case 0x03:
	case 0x13:
		std::strcpy(ibuf,"ora");
		break;
		// PEA
	case 0xF4:
		std::strcpy(ibuf,"pea");
		break;
		// PEI
	case 0xD4:
		std::strcpy(ibuf,"pei");
		break;
		// PER
	case 0x62:
		std::strcpy(ibuf,"per");
		break;
		// PHA
	case 0x48:
		std::strcpy(ibuf,"pha");
		break;
		// PHP
	case 0x08:
		std::strcpy(ibuf,"php");
		break;
		// PHX
	case 0xDA:
		std::strcpy(ibuf,"phx");
		break;
		// PHY
	case 0x5A:
		std::strcpy(ibuf,"phy");
		break;
		// PLA
	case 0x68:
		std::strcpy(ibuf,"pla");
		break;
		// PLP
	case 0x28:
		std::strcpy(ibuf,"plp");
		break;
		// PLX
	case 0xFA:
		std::strcpy(ibuf,"plx");
		break;
		// PLY
	case 0x7A:
		std::strcpy(ibuf,"ply");
		break;
		// PHB
	case 0x8B:
		std::strcpy(ibuf,"phb");
		break;
		// PHD
	case 0x0B:
		std::strcpy(ibuf,"phd");
		break;
		// PHK
	case 0x4B:
		std::strcpy(ibuf,"phk");
		break;
		// PLB
	case 0xAB:
		std::strcpy(ibuf,"plb");
		break;
		// PLD
	case 0x2B:
		std::strcpy(ibuf,"pld");
		break;
		// REP
	case 0xC2:
		std::strcpy(ibuf,"rep");
		break;
		// ROL
	case 0x2A:
	case 0x2E:
	case 0x26:
	case 0x3E:
	case 0x36:
		std::strcpy(ibuf,"rol");
		break;
		// ROR
	case 0x6A:
	case 0x6E:
	case 0x66:
	case 0x7E:
	case 0x76:
		std::strcpy(ibuf,"ror");
		break;
		// RTI
	case 0x40:
		std::strcpy(ibuf,"rti");
		if(tsrc&0x2)
			strcat(ibuf,"\n");
		break;
		// RTL
	case 0x6B:
		std::strcpy(ibuf,"rtl");
		if(tsrc&0x2)
			strcat(ibuf,"\n");
		break;
		// RTS
	case 0x60:
		std::strcpy(ibuf,"rts");
		if(tsrc&0x2)
			strcat(ibuf,"\n");
		break;
		// SBC
	case 0xE9:
	case 0xED:
	case 0xEF:
	case 0xE5:
	case 0xF2:
	case 0xE7:
	case 0xFD:
	case 0xFF:
	case 0xF9:
	case 0xF5:
	case 0xE1:
	case 0xF1:
	case 0xF7:
	case 0xE3:
	case 0xF3:
		std::strcpy(ibuf,"sbc");
		break;
		// SEP
	case 0xE2:
		std::strcpy(ibuf,"sep");
		break;
		// STA
	case 0x8D:
	case 0x8F:
	case 0x85:
	case 0x92:
	case 0x87:
	case 0x9D:
	case 0x9F:
	case 0x99:
	case 0x95:
	case 0x81:
	case 0x91:
	case 0x97:
	case 0x83:
	case 0x93:
		std::strcpy(ibuf,"sta");
		break;
		// STP
	case 0xDB:
		std::strcpy(ibuf,"stp");
		break;
		// STX
	case 0x8E:
	case 0x86:
	case 0x96:
		std::strcpy(ibuf,"stx");
		break;
		// STY
	case 0x8C:
	case 0x84:
	case 0x94:
		std::strcpy(ibuf,"sty");
		break;
		// STZ
	case 0x9C:
	case 0x64:
	case 0x9E:
	case 0x74:
		std::strcpy(ibuf,"stz");
		break;
		// TAX
	case 0xAA:
		std::strcpy(ibuf,"tax");
		break;
		// TAY
	case 0xA8:
		std::strcpy(ibuf,"tay");
		break;
		// TXA
	case 0x8A:
		std::strcpy(ibuf,"txa");
		break;
		// TYA
	case 0x98:
		std::strcpy(ibuf,"tya");
		break;
		// TSX
	case 0xBA:
		std::strcpy(ibuf,"tsx");
		break;
		// TXS
	case 0x9A:
		std::strcpy(ibuf,"txs");
		break;
		// TXY
	case 0x9B:
		std::strcpy(ibuf,"txy");
		break;
		// TYX
	case 0xBB:
		std::strcpy(ibuf,"tyx");
		break;
		// TCD
	case 0x5B:
		std::strcpy(ibuf,"tcd");
		break;
		// TDC
	case 0x7B:
		std::strcpy(ibuf,"tdc");
		break;
		// TCS
	case 0x1B:
		std::strcpy(ibuf,"tcs");
		break;
		// TSC
	case 0x3B:
		std::strcpy(ibuf,"tsc");
		break;
		// TRB
	case 0x1C:
	case 0x14:
		std::strcpy(ibuf,"trb");
		break;
		// TSB
	case 0x0C:
	case 0x04:
		std::strcpy(ibuf,"tsb");
		break;
		// WAI
	case 0xCB:
		std::strcpy(ibuf,"wai");
		break;
		// WDM
	case 0x42:
		std::strcpy(ibuf,"wdm");
		break;
		// XBA
	case 0xEB:
		std::strcpy(ibuf,"xba");
		break;
		// XCE
	case 0xFB:
		std::strcpy(ibuf,"xce");
		break;
	default:
		// Illegal
		printf("Unhandled instruction: %02x\n",mem[0]);
		exit(1);
	};

	// Parse out parameter list
	switch(mem[0]){
		// Absolute
	case 0x0C:
	case 0x0D:
	case 0x0E:
	case 0x1C:
	case 0x20:
	case 0x2C:
	case 0x2D:
	case 0x2E:
	case 0x4C:
	case 0x4D:
	case 0x4E:
	case 0x6D:
	case 0x6E:
	case 0x8C:
	case 0x8D:
	case 0x8E:
	case 0x9C:
	case 0xAC:
	case 0xAD:
	case 0xAE:
	case 0xCC:
	case 0xCD:
	case 0xCE:
	case 0xEC:
	case 0xED:
	case 0xEE:
		std::sprintf(pbuf,"$%04x",mem[1]+mem[2]*256);
		offset=3;
		break;
		// Absolute Indexed Indirect
	case 0x7C:
	case 0xFC:
		std::sprintf(pbuf,"($%04x,X)",mem[1]+mem[2]*256);
		offset=3;
		break;
		// Absolute Indexed, X
	case 0x1D:
	case 0x1E:
	case 0x3C:
	case 0x3D:
	case 0x3E:
	case 0x5D:
	case 0x5E:
	case 0x7D:
	case 0x7E:
	case 0x9D:
	case 0x9E:
	case 0xBC:
	case 0xBD:
	case 0xDD:
	case 0xDE:
	case 0xFD:
	case 0xFE:
		std::sprintf(pbuf,"$%04x,X",mem[1]+mem[2]*256);
		offset=3;
		break;
		// Absolute Indexed, Y
	case 0x19:
	case 0x39:
	case 0x59:
	case 0x79:
	case 0x99:
	case 0xB9:
	case 0xBE:
	case 0xD9:
	case 0xF9:
		std::sprintf(pbuf,"$%04x,Y",mem[1]+mem[2]*256);
		offset=3;
		break;
		// Absolute Indirect
	case 0x6C:
		std::sprintf(pbuf,"($%04x)",mem[1]+mem[2]*256);
		offset=3;
		break;
		// Absolute Indirect Long
	case 0xDC:
		std::sprintf(pbuf,"[$%04x]",mem[1]+mem[2]*256);
		offset=3;
		break;
		// Absolute Long
	case 0x0F:
	case 0x22:
	case 0x2F:
	case 0x4F:
	case 0x5C:
	case 0x6F:
	case 0x8F:
	case 0xAF:
	case 0xCF:
	case 0xEF:
		std::sprintf(pbuf,"$%06x",mem[1]+mem[2]*256+mem[3]*65536);
		offset=4;
		break;
		// Absolute Long Indexed, X
	case 0x1F:
	case 0x3F:
	case 0x5F:
	case 0x7F:
	case 0x9F:
	case 0xBF:
	case 0xDF:
	case 0xFF:
		std::sprintf(pbuf,"$%06x,X",mem[1]+mem[2]*256+mem[3]*65536);
		offset=4;
		break;
		// Accumulator
	case 0x0A:
	case 0x1A:
	case 0x2A:
	case 0x3A:
	case 0x4A:
	case 0x6A:
		std::sprintf(pbuf,"\0");
		offset=1;
		break;
		// Block Move
	case 0x44:
	case 0x54:
		std::sprintf(pbuf,"$%02x,$%02x",mem[1],mem[2]);
		offset=3;
		break;
		// Direct Page
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x14:
	case 0x24:
	case 0x25:
	case 0x26:
	case 0x45:
	case 0x46:
	case 0x64:
	case 0x65:
	case 0x66:
	case 0x84:
	case 0x85:
	case 0x86:
	case 0xA4:
	case 0xA5:
	case 0xA6:
	case 0xC4:
	case 0xC5:
	case 0xC6:
	case 0xE4:
	case 0xE5:
	case 0xE6:
		std::sprintf(pbuf,"$%02x",mem[1]);
		offset=2;
		break;
		// Direct Page Indexed, X
	case 0x15:
	case 0x16:
	case 0x34:
	case 0x35:
	case 0x36:
	case 0x55:
	case 0x56:
	case 0x74:
	case 0x75:
	case 0x76:
	case 0x94:
	case 0x95:
	case 0xB4:
	case 0xB5:
	case 0xD5:
	case 0xD6:
	case 0xF5:
	case 0xF6:
		std::sprintf(pbuf,"$%02x,X",mem[1]);
		offset=2;
		break;
		// Direct Page Indexed, Y
	case 0x96:
	case 0xB6:
		std::sprintf(pbuf,"$%02x,Y",mem[1]);
		offset=2;
		break;
		// Direct Page Indirect
	case 0x12:
	case 0x32:
	case 0x52:
	case 0x72:
	case 0x92:
	case 0xB2:
	case 0xD2:
	case 0xF2:
		std::sprintf(pbuf,"($%02x)",mem[1]);
		offset=2;
		break;
		// Direct Page Indirect Long
	case 0x07:
	case 0x27:
	case 0x47:
	case 0x67:
	case 0x87:
	case 0xA7:
	case 0xC7:
	case 0xE7:
		std::sprintf(pbuf,"[$%02x]",mem[1]);
		offset=2;
		break;
		// Direct Page Indexed Indirect, X
	case 0x01:
	case 0x21:
	case 0x41:
	case 0x61:
	case 0x81:
	case 0xA1:
	case 0xC1:
	case 0xE1:
		std::sprintf(pbuf,"($%02x,X)",mem[1]);
		offset=2;
		break;
		// Direct Page Indirect Indexed, Y
	case 0x11:
	case 0x31:
	case 0x51:
	case 0x71:
	case 0x91:
	case 0xB1:
	case 0xD1:
	case 0xF1:
		std::sprintf(pbuf,"($%02x),Y",mem[1]);
		offset=2;
		break;
		// Direct Page Indirect Long Indexed, Y
	case 0x17:
	case 0x37:
	case 0x57:
	case 0x77:
	case 0x97:
	case 0xB7:
	case 0xD7:
	case 0xF7:
		std::sprintf(pbuf,"[$%02x],Y",mem[1]);
		offset=2;
		break;
		// Stack (Pull)
	case 0x28:
	case 0x2B:
	case 0x68:
	case 0x7A:
	case 0xAB:
	case 0xFA:
		// Stack (Push)
	case 0x08:
	case 0x0B:
	case 0x48:
	case 0x4B:
	case 0x5A:
	case 0x8B:
	case 0xDA:
		// Stack (RTL)
	case 0x6B:
		// Stack (RTS)
	case 0x60:
		// Stack/RTI
	case 0x40:
		// Implied
	case 0x18:
	case 0x1B:
	case 0x38:
	case 0x3B:
	case 0x58:
	case 0x5B:
	case 0x78:
	case 0x7B:
	case 0x88:
	case 0x8A:
	case 0x98:
	case 0x9A:
	case 0x9B:
	case 0xA8:
	case 0xAA:
	case 0xB8:
	case 0xBA:
	case 0xBB:
	case 0xC8:
	case 0xCA:
	case 0xCB:
	case 0xD8:
	case 0xDB:
	case 0xE8:
	case 0xEA:
	case 0xEB:
	case 0xF8:
	case 0xFB:
		pbuf[0] = 0;
		offset = 1;
		break;
		// Program Counter Relative
	case 0x10:
	case 0x30:
	case 0x50:
	case 0x70:
	case 0x80:
	case 0x90:
	case 0xB0:
	case 0xD0:
	case 0xF0:
		// Calculate the signed value of the param
		sval = (mem[1]>127) ? (mem[1]-256) : mem[1];
		std::sprintf(pbuf, "$%04lx", (pos+sval+2) & 0xFFFF);
		offset = 2;
		break;
		// Stack (Program Counter Relative Long)
	case 0x62:
		// Program Counter Relative Long
	case 0x82:
		// Calculate the signed value of the param
		sval = mem[1] + mem[2]*256;
		sval = (sval>32767) ? (sval-65536) : sval;
		std::sprintf(pbuf, "$%04lx", (pos+sval+3) & 0xFFFF);
		offset = 3;
		break;
		// Stack Relative Indirect Indexed, Y
	case 0x13:
	case 0x33:
	case 0x53:
	case 0x73:
	case 0x93:
	case 0xB3:
	case 0xD3:
	case 0xF3:
		std::sprintf(pbuf, "($%02x,S),Y", mem[1]);
		offset = 2;
		break;
		// Stack (Absolute)
	case 0xF4:
		std::sprintf(pbuf, "$%04x", mem[1] + mem[2]*256);
		offset = 3;
		break;
		// Stack (Direct Page Indirect)
	case 0xD4:
		std::sprintf(pbuf,"($%02x)",mem[1]);
		offset = 2;
		break;
		offset = 3;
		break;
		// Stack Relative
	case 0x03:
	case 0x23:
	case 0x43:
	case 0x63:
	case 0x83:
	case 0xA3:
	case 0xC3:
	case 0xE3:
		std::sprintf(pbuf,"$%02x,S",mem[1]);
		offset = 2;
		break;
		// WDM mode
	case 0x42:
		// Stack/Interrupt
	case 0x00:
	case 0x02:
		std::sprintf(pbuf,"$%02x",mem[1]);
		offset = 2;
		break;
		// Immediate (Invariant)
	case 0xC2:
		// REP following
		*flag=*flag&~mem[1];
		std::sprintf(pbuf,"#$%02x",mem[1]);
		offset = 2;
		break;
	case 0xE2:
		// SEP following
		*flag = *flag|mem[1];
		std::sprintf(pbuf, "#$%02x", mem[1]);
		offset = 2;
		break;
		// Immediate (A size dependent)
	case 0x09:
	case 0x29:
	case 0x49:
	case 0x69:
	case 0x89:
	case 0xA9:
	case 0xC9:
	case 0xE9:
		if (*flag & 0x20)
		{
			std::sprintf(pbuf, "#$%02x", mem[1]);
			offset = 2;
		}
		else
		{
			std::sprintf(pbuf,"#$%04x",mem[1]+mem[2]*256);
			offset = 3;
		}
		break;
		// Immediate (X/Y size dependent)
	case 0xA0:
	case 0xA2:
	case 0xC0:
	case 0xE0:
		if ((*flag) & 0x10)
		{
			std::sprintf(pbuf,"#$%02x",mem[1]);
			offset = 2;
		}
		else
		{
			std::sprintf(pbuf,"#$%04x",mem[1]+mem[2]*256);
			offset = 3;
		}
		break;
		//printf("Unhandled Addressing Mode: %02x\n",mem[0]);
		//exit(1);
	};

	// Generate hex output
	for (i=0; i<offset; i++)
	{
		std::sprintf(hbuf+i*2,"%02x",mem[i]);
	}
	for (i=offset*2; i<8; i++)
	{
		hbuf[i]=0x20;
	}
	hbuf[8]=0;

	// Generate whole disassembly line
	if(!(tsrc & 1))
	{
		std::sprintf(inst, "%02lX/%04lX:\t%s\t%s %s", (pos >> 16) & 0xFF, pos&0xFFFF, hbuf, ibuf, pbuf);
	}
	else
	{
		std::sprintf(inst, "%s %s", ibuf, pbuf);
	}

	return offset;
}