#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.
typedef bitset<32> bit32;
typedef bitset<16> bit16;
typedef bitset<5> bit5;
const bit32 defaultbs32=bit32(-1);
const int LW=0x23,SW=0x2b,BEQ=0x04,ADDU=0x21;
struct IDStruct;
struct EXStruct;
struct MEMStruct;
struct WBStruct;
struct stateStruct; 
class RF;
class INSMem;
class DataMem;
int stall=0;
struct IFStruct 
{
    bitset<32>  PC;
    bool        nop;  
    IFStruct()
    {
        PC=bit32(0);
        nop=0;
    }
    IFStruct getIns();
    IDStruct output(INSMem&);
    void halt();
    void set(bit32 PC,bool nop)
    {
        this->PC=PC;
        this->nop=nop;
    }
};
struct IDStruct 
{
    bitset<32>  Instr;
    bool        nop;  
    IDStruct()
    {
        nop=1;
    }
    EXStruct output(RF&);
    void set(bit32 PC,bool nop)
    {
        this->nop=nop;
        if (!nop)
            this->Instr=PC;
    }
};
struct EXStruct 
{
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;  
    EXStruct()
    {
        is_I_type=rd_mem=wrt_mem=alu_op=wrt_enable=0;
        alu_op=nop=1;
    }
    MEMStruct output();
    EXStruct set(bool nop)
    {
        this->nop=nop;
        return *this;
    }
};
struct MEMStruct 
{
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        wrt_enable;    
    bool        nop;    
    MEMStruct()
    {
        nop=1;
    }
    MEMStruct(bit5 Rs,bit5 Rt,bit5 Wrt_reg_addr,bool rd_mem,bool wrt_mem,bool wrt_enable):Rs(Rs),Rt(Rt),Wrt_reg_addr(Wrt_reg_addr),rd_mem(rd_mem),wrt_mem(wrt_mem),wrt_enable(wrt_enable)
    {
        nop=1;
    }
    WBStruct output(DataMem&);
    MEMStruct set(bool nop)
    {
        this->nop=nop;
        return *this;
    }
};
struct WBStruct 
{
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;     
    WBStruct()
    {
        nop=1;
    }
    WBStruct(bit5 Rs,bit5 Rt,bit5 Wrt_reg_addr,bool wrt_enable):Rs(Rs),Rt(Rt),Wrt_reg_addr(Wrt_reg_addr),wrt_enable(wrt_enable)
    {
        nop=1;
    }
    void output(RF&);
    WBStruct set(bool nop)
    {
        this->nop=nop;
        return *this;
    }
};
struct stateStruct 
{
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
    stateStruct()
    {
        IF=IFStruct();
        ID=IDStruct();
        EX=EXStruct();
        MEM=MEMStruct();
        WB=WBStruct();
    }
};
stateStruct state,newState;
class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr)
        {   
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
		 
		void outputRF()
		{
			ofstream rfout;
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
				for (int j = 0; j<32; j++)
				{        
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open file";
			rfout.close();               
		} 
			
	private:
		vector<bitset<32> >Registers;	
};
class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {       
			IMem.resize(MemSize); 
            ifstream imem;
			string line;
			int i=0;
			imem.open("imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{      
					IMem[i] = bitset<8>(line.substr(0,8));
					i++;
				}                    
			}
            else cout<<"Unable to open file";
			imem.close();                     
		}
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
};    
class DataMem    
{
    public:
        bitset<32> ReadData;  
        DataMem()
        {
            DMem.resize(MemSize); 
            ifstream dmem;
            string line;
            int i=0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {      
                    DMem[i] = bitset<8>(line.substr(0,8));
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();          
        }
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("dmemresult.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {     
                    dmemout << DMem[j]<<endl;
                }
                     
            }
            else cout<<"Unable to open file";
            dmemout.close();               
        }             
      
    private:
		vector<bitset<8> > DMem;      
};  
void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;        
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
    }
    else cout<<"Unable to open file";
    printstate.close();
}
int signExtend(bit16 &imm)
{
    if (imm[15]==1)
        return (int)(imm.to_ulong()^0xffff0000);
    return (int)imm.to_ulong();
}
IFStruct IFStruct::getIns()
{
    IFStruct res=state.IF;
    res.set(bit32((int)(PC.to_ulong())+4),0);
    return res;
}
IDStruct IFStruct::output(INSMem &myInsMem)
{
    IDStruct res=state.ID;
    res.set(myInsMem.readInstr(PC.to_ulong()),nop);
    if (!nop)
        res.nop=(res.Instr==defaultbs32);
    return res;
}
void IFStruct::halt()
{
    nop=1;
}
EXStruct IDStruct::output(RF &myRF)
{
    EXStruct res=EXStruct();
    if (nop)
    {
        res=state.EX;
        return res.set(nop);
    }
    unsigned int instr=Instr.to_ulong(),OpCode=instr>>26;
    res.nop=0;
    res.Rs=bit5((instr>>21)&0x1f);
    res.Rt=bit5((instr>>16)&0x1f);
    res.Read_data1=myRF.readRF(res.Rs);
    res.Read_data2=myRF.readRF(res.Rt);
    res.Imm=bit16(instr&0xffff);
    stall=0;
    if (state.EX.nop==0&&state.EX.rd_mem&&(res.Rs==state.EX.Wrt_reg_addr||(OpCode==0&&res.Rt==state.EX.Wrt_reg_addr)))
    {
        stall=1;
        return res.set(1);
    }
    if (OpCode==BEQ)
    {
        if (res.Read_data1!=res.Read_data2)
        {
            state.IF.PC=bit32((int)(state.IF.PC.to_ulong())+(signExtend(res.Imm)<<2)-4);
            state.IF.nop=1;
        }
        return res;
    }
    res.is_I_type=(OpCode!=0);
    if (OpCode==0)
    {
        res.Wrt_reg_addr=bit5((instr>>11)&0x1f);
        res.alu_op=((instr&0x3f)==ADDU);
        res.wrt_enable=1;
        return res;
    }
    res.Wrt_reg_addr=res.Rt;
    if (OpCode==LW||OpCode==SW)
    {
        res.alu_op=1;
        if (OpCode==LW)
            res.wrt_enable=res.rd_mem=1;
        else
            res.wrt_mem=1;
    }
    return res;
}
MEMStruct EXStruct::output()
{
    MEMStruct res=MEMStruct(Rs,Rt,Wrt_reg_addr,rd_mem,wrt_mem,wrt_enable);
    if (nop)
    {
        res=state.MEM;
        return res.set(nop);
    }
    bit32 op1=Read_data1,op2=Read_data2;
    if (state.WB.nop==0&&state.WB.wrt_enable)
    {
        if (state.WB.Wrt_reg_addr==Rs)
            op1=state.WB.Wrt_data;
        if (state.WB.Wrt_reg_addr==Rt)
            op2=state.WB.Wrt_data;
    }
    if (state.MEM.nop==0&&state.MEM.wrt_enable)
    {
        if (state.MEM.Wrt_reg_addr==Rs)
            op1=state.MEM.ALUresult;
        if (state.MEM.Wrt_reg_addr==Rt)
            op2=state.MEM.ALUresult;
    }
    res.nop=0;
    res.Store_data=op2;
    if (!is_I_type)
        res.ALUresult=bit32(alu_op==1?op1.to_ulong()+op2.to_ulong():op1.to_ulong()-op2.to_ulong());
    else
        res.ALUresult=bit32(op1.to_ulong()+signExtend(Imm));
    return res;
}
WBStruct MEMStruct::output(DataMem &myDataMem)
{
    WBStruct res=WBStruct(Rs,Rt,Wrt_reg_addr,wrt_enable);
    if (nop)
    {
        res=state.WB;
        return res.set(nop);
    }
    res.nop=0;
    res.Wrt_data=ALUresult;
    bit32 op=Store_data;
    if (state.WB.nop==0&&state.WB.wrt_enable&&state.WB.Wrt_reg_addr==Rt)
        op=state.WB.Wrt_data;
    if (rd_mem)
        res.Wrt_data=myDataMem.readDataMem(ALUresult);
    if (wrt_mem)
        myDataMem.writeDataMem(ALUresult,op);
    return res;
}
void WBStruct::output(RF &myRF)
{
    if (nop==0&&wrt_enable==1)
        myRF.writeRF(Wrt_reg_addr,Wrt_data);
}
int main()
{
    RF myRF=RF();
    INSMem myInsMem=INSMem();
    DataMem myDataMem=DataMem();
	state=stateStruct(),newState=stateStruct();
    int cycle=0;
    while (1) {
        /* --------------------- WB stage --------------------- */
        state.WB.output(myRF);
        /* --------------------- MEM stage --------------------- */
        newState.WB=state.MEM.output(myDataMem);
        /* --------------------- EX stage --------------------- */
        newState.MEM=state.EX.output();
        /* --------------------- ID stage --------------------- */
        newState.EX=state.ID.output(myRF);
        if (stall)
            newState.IF=state.IF,newState.ID=state.ID;
        /* --------------------- IF stage --------------------- */
        else
        {
            newState.ID=state.IF.output(myInsMem);
            if (newState.ID.Instr==defaultbs32)
            {
                state.IF.halt();
                newState.IF=state.IF;
            }
            else
                newState.IF=state.IF.getIns();
        }
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;
        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
        ++cycle;
        state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */   	
    }
    myRF.outputRF(); // dump RF;	
	myDataMem.outputDataMem(); // dump data mem 
	return 0;
}