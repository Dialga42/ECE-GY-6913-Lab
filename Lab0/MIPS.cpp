#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.
class RF
{
    public:
        bitset<32> ReadData1,ReadData2; 
     	RF()
    	{
    		Registers.resize(32);  
    		Registers[0]=bitset<32>(0);  
        }
        void ReadWrite(bitset<5> RdReg1,bitset<5> RdReg2,bitset<5> WrtReg,bitset<32> WrtData,bitset<1> WrtEnable)
        {   
            // implement the funciton by you. 
            if (WrtEnable==bitset<1>(1))
            {
                Registers[int(WrtReg.to_ulong())]=WrtData;
                cout<<"write "<<WrtReg.to_ulong()<<" "<<WrtData<<endl;
            } 
            ReadData1=Registers[int(RdReg1.to_ulong())];
            ReadData2=Registers[int(RdReg2.to_ulong())];
        }
		void OutputRF() // write RF results to file
		{
			ofstream rfout;
            rfout.open("RFresult.txt",std::ios_base::app);
            if (rfout.is_open())
            {
            	rfout<<"A state of RF:"<<endl;
                for (int j=0;j<32;j++)      
                    rfout<<Registers[j]<<endl;
            }
            else 
            	cout<<"Unable to open file RFresult.txt";
            rfout.close();     
        }     
	private:
        vector<bitset<32> >Registers;
	
};
class ALU
{
    public:
        bitset<32> ALUresult;
        bitset<32> ALUOperation(bitset<3> ALUOP,bitset<32> oprand1,bitset<32> oprand2)
        {
            // implement the ALU operations by you. 
            if (ALUOP==bitset<3>(ADDU))
        		ALUresult=bitset<32>(oprand1.to_ulong()+oprand2.to_ulong());
        	else if (ALUOP==bitset<3>(SUBU))
        		ALUresult=bitset<32>(oprand1.to_ulong()-oprand2.to_ulong());
        	else if (ALUOP==bitset<3>(AND))
        		ALUresult=(oprand1&oprand2);
        	else if (ALUOP==bitset<3>(OR))
        		ALUresult=(oprand1|oprand2);
        	else if (ALUOP==bitset<3>(NOR))
        		ALUresult=(~(oprand1|oprand2));
            return ALUresult;
        }            
};

class INSMem
{
	public:
     	bitset<32> Instruction;
        INSMem() // read instruction memory
        {
          	IMem.resize(MemSize); 
            ifstream imem;
            string line;
            int i=0;
            imem.open("imem.txt");
            if (imem.is_open())
            	while (getline(imem,line))
                {
                	IMem[i]=bitset<8>(line.substr(0,8));
                    i++;
                }
            else 
            	cout<<"Unable to open file imem.txt";
                imem.close();
        }
        bitset<32> ReadMemory(bitset<32> ReadAddress) 
        {    
            // implement by you. (Read the byte at the ReadAddress and the following three byte).
            unsigned int addr=ReadAddress.to_ulong();
            Instruction=bitset<32>((IMem[addr].to_ulong()<<24)|(IMem[addr+1].to_ulong()<<16)|(IMem[addr+2].to_ulong()<<8)|IMem[addr+3].to_ulong());
            return Instruction;     
        }     
    private:
        vector<bitset<8> > IMem;
      
};
      
class DataMem    
{
	public:
		bitset<32> readdata;  
        DataMem() // read data memory
        {
        	DMem.resize(MemSize); 
            ifstream dmem;
            string line;
            int i=0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            	while (getline(dmem,line))
                {
                	DMem[i]=bitset<8>(line.substr(0,8));
                    i++;
                }
            else 
            	cout<<"Unable to open file dmem.txt";
            dmem.close();
        }  
        bitset<32> MemoryAccess(bitset<32> Address,bitset<32> WriteData,bitset<1> readmem,bitset<1> writemem) 
        {    
        	int addr=int(Address.to_ulong());
            if (readmem==bitset<1>(1))
            	readdata=bitset<32>((DMem[addr].to_ulong()<<24)|(DMem[addr+1].to_ulong()<<16)|(DMem[addr+2].to_ulong()<<8)|DMem[addr+3].to_ulong());
            else if (writemem==bitset<1>(1))
            {
            	unsigned long data=WriteData.to_ulong();
            	for (int i=3;i>=0;i--)
            	{
            		DMem[addr+i]=bitset<8>(data&0xff);
            		data>>=8;
            	}
            }
            // implement by you.
            return readdata;     
        }             
        void OutputDataMem()  // write dmem results to file
        {
            ofstream dmemout;
            dmemout.open("dmemresult.txt");
            if (dmemout.is_open())
            	for (int j=0;j<1000;j++)     
                    dmemout<<DMem[j]<<endl;         
            else 
            	cout<<"Unable to open file dmemresult.txt";
            dmemout.close();
               
        }             
    private:
        vector<bitset<8> > DMem;
      
};  
int main()
{
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;
    int PC=0;
    while (1)
	{
        // Fetch
        if (PC>60)
        	break;
        cout<<"PC: "<<PC<<endl;
        unsigned int instruction=0;
        instruction=myInsMem.ReadMemory(bitset<32>(PC)).to_ulong();
		// If current instruciton is "11111111111111111111111111111111", then break;
        cout<<bitset<32>(instruction)<<endl;
        if (instruction==0xffffffff)
        	break;
        if ((instruction>>26)==2)
        {
        	PC=(((PC+4)&0xf0000000)|((instruction&0x03ffffff)<<2));
        	myRF.OutputRF();
        	continue;
        }
		// decode(Read RF)
		bitset<5> RdReg1=bitset<5>((instruction>>21)&0x1f),
			RdReg2=bitset<5>((instruction>>16)&0x1f),
			WrtReg;
		bitset<1> RfWrtEnable(1);
		cout<<RdReg1<<endl;
		cout<<RdReg2<<endl;
		myRF.ReadWrite(RdReg1,RdReg2,0,0,bitset<1>(0));
		// Execute
		bitset<32> WrtData;
		if ((instruction>>26)==0)
		{
			WrtReg=bitset<5>((instruction>>11)&0x1f);
			WrtData=myALU.ALUOperation(bitset<3>(instruction&0x0f),myRF.ReadData1,myRF.ReadData2);
		}
		else
		{
			WrtReg=RdReg2;
			if ((instruction>>26)==0x9)//addiu
				WrtData=myALU.ALUOperation(bitset<3>(ADDU),myRF.ReadData1,bitset<32>(instruction&0xffff));
			else if ((instruction>>26)==0x4)//beq
			{
				if (myRF.ReadData1==myRF.ReadData2)
					PC+=((instruction&0xffff)<<2);
				RfWrtEnable=bitset<1>(0);
			}
			else if ((instruction>>26)==0x23)//lw
			{
				bitset<32> ReadAddress=myALU.ALUOperation(bitset<3>(ADDU),bitset<32>(instruction&0xffff),myRF.ReadData1);
				WrtData=myDataMem.MemoryAccess(ReadAddress,0,bitset<1>(1),0); 
				cout<<ReadAddress<<endl;
				cout<<WrtData<<endl;
			}
			else if ((instruction>>26)==0x2B)//sw
			{
				bitset<32> WriteAddress=myALU.ALUOperation(bitset<3>(ADDU),bitset<32>(instruction&0xffff),myRF.ReadData1);
				myDataMem.MemoryAccess(WriteAddress,myRF.ReadData2,0,bitset<1>(1)); 
				RfWrtEnable=bitset<1>(0);
			}
		}
		// Read/Write Mem
		
		// Write back to RF
		cout<<WrtReg<<" "<<WrtData<<endl;
		myRF.ReadWrite(RdReg1,RdReg2,WrtReg,WrtData,RfWrtEnable);
        myRF.OutputRF(); // dump RF; 
        PC+=4;   
    }
    myDataMem.OutputDataMem(); // dump data mem
    return 0; 
}
