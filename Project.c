//Names: Jared Reich, Srignan Paruchuru, Gianmarco Folchi
//Professor Angell
//Date: 11/18/2021
//Project: Processor
#include "spimcore.h"

void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero)
{
	if (ALUControl == 0)
	{ // Adds
		*ALUresult = A + B;
	}
	if (ALUControl == 1)
	{ //subtracts
		*ALUresult = A - B;
	}

	if (ALUControl == 2)
	{ // set less than signed integer
		if ((int)A < (int)B)
			*ALUresult = 1;
		else
			*ALUresult = 0;
	}
	if (ALUControl == 3)
	{ //Set less than
		if (A < B)
			*ALUresult = 1;
		else
			*ALUresult = 0;
	}
	if (ALUControl == 4)
	{ //Bitwise and
		*ALUresult = A & B;
	}
	if (ALUControl == 5)
	{ // Bitwise Or
		*ALUresult = A | B;
	}
	if (ALUControl == 6)
	{ //left shift
		*ALUresult = B << 16;
	}
	if (ALUControl == 7)
	{ // bitwise not
		*ALUresult = ~A;
	}
	if (*ALUresult == 0)
	{ // check to see if answer is zero and set Zero to 1 if it is.
		*Zero = 1;
	}
	if (*ALUresult != 0)
	{
		*Zero = 0;
	}
}
/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction)
{
	// set halt to 1 if:
	//an illegal instruction is encountered
	// Check word alignment
	if ((PC % 4) != 0) //if word alignment is not divisible by 4 return halt 1.
		return 1;

	if (PC <= 0 || PC > 65535)
		return 1;

	*instruction = Mem[PC >> 2]; // Set instructions
	return 0;
}
/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1, unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	*op = (instruction & (unsigned)0xfc000000);
	*r1 = (instruction & (unsigned)0x03e00000) >> 21;
	*r2 = (instruction & (unsigned)0x001f0000) >> 16;
	*r3 = (instruction & (unsigned)0x0000f800) >> 11;
	*funct = (instruction & (unsigned)0x0000003f);
	*offset = (instruction & (unsigned)0x0000ffff);
	*jsec = (instruction & (unsigned)0x03ffffff);
}

int instruction_decode(unsigned op, struct_controls *controls)
{
	switch (op)
	{
	case 0x000000000: //r-type
	{
		controls->RegDst = 1;
		controls->Jump = 0;		//Not jumping
		controls->MemRead = 0;	//not reading memory
		controls->MemtoReg = 0; //alu output write data register
		controls->ALUOp = 7;
		controls->Branch = 0;	//not branching
		controls->MemWrite = 0; //not writing to memory
		controls->ALUSrc = 0;	//read data 2 to alu
		controls->RegWrite = 1; //write data to register
		break;
	}

	//addi I-type
	case 0x20000000:
	{
		controls->RegDst = 0;
		controls->Jump = 0; //not jumping
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 0;  //add for alu
		controls->Branch = 0; //not branching
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1; //write data to register
		break;
	}

	// load word, I-type
	case 0x8c000000:
	{
		controls->RegDst = 0; //want to write to register in bit 20-16
		controls->Jump = 0;	  //not jumping
		controls->MemRead = 1;
		controls->MemtoReg = 1;
		controls->ALUOp = 0;  //add for register address
		controls->Branch = 0; //not branching
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1; //write data to register
		break;
	}

	// store word I-type
	case 0xac000000:
	{
		controls->RegDst = 0;
		controls->Jump = 0; //not jumping
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 0;  //add register address
		controls->Branch = 0; //not branching
		controls->MemWrite = 1;
		controls->ALUSrc = 1;
		controls->RegWrite = 0; //don't write data to register
		break;
	}

	// slti I-type
	case 0x28000000:
	{
		controls->RegDst = 0;
		controls->Jump = 0; //not jumping
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 2;
		controls->Branch = 0; //not branching
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1; //write data to register
		break;
	}

	// sltiu I-type
	case 0x2c000000:
	{
		controls->RegDst = 0;
		controls->Jump = 0; //not jumping
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 3;
		controls->Branch = 0; //not branching
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1; //write data to register
		break;
	}

	// lui I-Type
	case 0x3c000000:
	{
		controls->RegDst = 0;
		controls->Jump = 0; //not jumping
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 6;
		controls->Branch = 0; //not branching
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1; //write data to register
		break;
	}

	// branch on equal I-type
	case 0x10000000:
	{
		controls->RegDst = 0;
		controls->Jump = 0; //not jumping
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 1;
		controls->Branch = 1; //branching
		controls->MemWrite = 0;
		controls->ALUSrc = 0;
		controls->RegWrite = 0; //don't write data to register
		break;
	}

	// jump J-type
	case 0x08000000:
	{
		controls->RegDst = 0;
		controls->Jump = 1; //jumping
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 0;  //add
		controls->Branch = 0; //not branching
		controls->MemWrite = 0;
		controls->ALUSrc = 0;
		controls->RegWrite = 0; //don't write data to register
		break;
	}

	// illegal operation
	default:
	{ //No other case must halt
		return 1;
	}
	}
	return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2)
{
	*data1 = Reg[r1];
	*data2 = Reg[r2];
}
/* Sign Extend */
/* 10 Points */

void sign_extend(unsigned offset, unsigned *extended_value)
{
	//extend offset from 16 bit value to a 32 bit value
	//DONE needs testing
	if (offset & 0x00008000)
	{
		*extended_value = offset | 0xffff0000;
	}
	else
	{
		*extended_value = offset;
	}
}

int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero)
{

	if (ALUOp == 7 && ALUSrc == 0)
	{ // R-Type instructions
		switch (funct)
		{
		case 32: // addition
			ALU(data1, data2, 0, ALUresult, Zero);
			return 0;
		case 34: //subtraction
			ALU(data1, data2, 1, ALUresult, Zero);
			return 0;
		case 36: // bitwise and
			ALU(data1, data2, 4, ALUresult, Zero);
			return 0;
		case 37: //bitwise or
			ALU(data1, data2, 5, ALUresult, Zero);
			return 0;
		case 42: //set on less than ?????????
			ALU(data1, data2, 2, ALUresult, Zero);
			return 0;
		case 43: // set on less than unsigned ????????
			ALU(data1, data2, 3, ALUresult, Zero);
			return 0;
		default: // If none match set as a halt.
			return 1;
		}
	}

	if (ALUOp == 6 && ALUSrc == 1)
	{ //if the alu op code is 6 do a shift left 16 bits on extended value.
		ALU(data1, extended_value, 6, ALUresult, Zero);
		return 0;
	}
	if (ALUOp == 0 && ALUSrc == 1)
	{ // add immediate I-Type
		ALU(data1, extended_value, 0, ALUresult, Zero);
		return 0;
	}
	if (ALUOp == 2 && ALUSrc == 1)
	{ // set less than immediate I-Type
		ALU(data1, extended_value, 2, ALUresult, Zero);
		return 0;
	}
	if (ALUOp == 3 && ALUSrc == 1)
	{ // set less than immediate unsigned I-type
		ALU(data1, extended_value, 3, ALUresult, Zero);
		return 0;
	}
	if (ALUOp == 0 && ALUSrc == 0)
	{ // jump instruction
		ALU(data1, data2, 0, ALUresult, Zero);
		return 0;
	}
	if (ALUOp == 1 && ALUSrc == 0)
	{
		ALU(data1, data2, 1, ALUresult, Zero);
		return 0;
	}
	else
		return 1; // if none instructions match return 1 for halt means illegal instruction
}

int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem)
{
	//check to see if aluresult is a valid address (only when it is a address)

	//load word into memory
	if (MemRead == 1 && MemWrite == 0)
	{
		//if ALUresult is word aligned or it is out of bounds of memory halt
		if (((ALUresult % 4) != 0) || ALUresult < 0 || ALUresult > 65535)
			return 1; //halt

		*memdata = Mem[ALUresult >> 2];
	}
	//store word into memory
	if (MemRead == 0 && MemWrite == 1)
	{
		//if ALUresult is word aligned or it is out of bounds of memory halt
		if (((ALUresult % 4) != 0) || ALUresult < 0 || ALUresult > 65535)
			return 1; //halt

		Mem[ALUresult >> 2] = data2;
	}
	return 0;
}
/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg)
{
	// Write the data (ALUresult or memdata) to a register (Reg) addressed by r2 or r3.

	if (RegWrite == 1 && MemtoReg == 1)
	{
		//data is coming from memory
		//r2 or r3 = memdata
		if (RegDst == 1)
		{
			Reg[r3] = memdata;
		}
		else
		{
			Reg[r2] = memdata;
		}
	}
	else if (RegWrite == 1 && MemtoReg == 0)
	{
		//data is coming from ALU_Result
		if (RegDst == 1)
		{
			Reg[r3] = ALUresult;
		}
		else
		{
			Reg[r2] = ALUresult;
		}
	}
}
/* PC update */
/* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC)
{
	if (Branch == 0 && Jump == 0)
	{
		*PC += 4; //After every instruction, we add 4
	}
	if (Branch == 1 && Zero == 1 && Jump == 0)
	{
		*PC += 4 + (extended_value << 2);
	}

	if (Jump == 1)
	{
		*PC = (jsec << 2) | ((*PC + 4) & 0xf0000000); //if jump is one, pc = jump target shifted left two bits
	}
}