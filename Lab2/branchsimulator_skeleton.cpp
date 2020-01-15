#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

using namespace std;

int main (int argc, char** argv) {
	ifstream config;
	config.open(argv[1]);

	int m, k;
	config >> m >> k;
 
	config.close();

	ofstream out;
	string out_file_name = string(argv[2]) + ".out";
	out.open(out_file_name.c_str());
	
	ifstream trace;
	trace.open(argv[2]);
	unsigned long pc; bool taken;
	trace >> std::hex >> pc >> taken;

	while (!trace.eof()) {
		bool prediction;
		prediction = true;
		out << prediction<<endl;
		trace >> std::hex >> pc >> taken;
	}
	 
	trace.close();	
	out.close();
}
