#pragma once
#include<string>
using namespace std;

class CMd5Encode {
public:
	CMd5Encode();
	~CMd5Encode();
	unsigned int* add(string& str);
	string changeHex(int a);
	string getMD5(string source);
protected:
	void mainLoop(unsigned int M[]);
};

