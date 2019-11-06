#include <cstdio>
#include <iostream>
#include <string>
#include <bitset>
using namespace std;
int signExtend(int imm)
{
    if ((imm>>15)&1)
        return imm^0xffff0000;
    return imm;
}
int main()
{
	string now;
	while (1)
	{
		string s;
		now="";
		//for (int i=1;i<=4;i++)
		//	cin>>s,now+=s;
		cin>>now;
		if (now==(bitset<32>(-1)).to_string())
			break;
		//cout<<now<<endl;
		unsigned int instr=bitset<32>(now).to_ulong(),opcode=(instr>>26),funct=(instr&0x3f),rs=(instr>>21)&0x1f,rt=(instr>>16)&0x1f,rd=(instr>>11)&0x1f;
		int imm=signExtend(instr&0xffff);
		if (opcode==0x23)
			cout<<"lw\t\t";
		else if (opcode==0x2b)
			cout<<"sw\t\t";
		else if (opcode==0x04)
			cout<<"beq\t\t";
		else if (funct==0x21)
			cout<<"addu\t";
		else
			cout<<"subu\t";
		cout<<"rs="<<rs<<"\t";
		cout<<"rt="<<rt<<"\t";
		if (opcode)
			cout<<"imm="<<imm<<"\t";
		else
			cout<<"rd="<<rd<<"\t";
		cout<<endl;
	}
	return 0;
}