// ExternalHack.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <stdio.h>
#include <windows.h>	//Windows內建API

using namespace std;
int main()
{
	printf("Ready to Hack!\n");

	/*HWND FindWindowA(
	LPCSTR lpClassName,
	LPCSTR lpWindowName		//Game Windows Title
	);
	*/
	// 名稱可能會有.或空格導致抓不到，因此建議使用Spy++工具來複製名稱

	HWND hWnd = FindWindowA(NULL, ("Plants vs. Zombies"));	//獲得Windows的handle
	if (hWnd) {
		printf("Find Game Windows Hwnd!\n");
		DWORD game_pid = NULL;
		GetWindowThreadProcessId(hWnd, &game_pid);	//hwnd to PID
		printf("game_pid:%d\n", game_pid);

		//找出遊戲的基礎記憶體位置
		DWORD game_image_base = GetWindowLongPtr(hWnd, GWLP_HINSTANCE);	// Hwnd to game image address
		printf("game_image_base:0x%x\n", game_image_base);


		/*
		HANDLE OpenProcess(
		DOWRD dwDesiredAccess,	//要求Process Handle權限
		BOOL binheritHandle,	//是否要被 child process 繼承
		DWORD dwProcessId		//欲請求 process Handle 的目標PID
		);
		*/

		// (PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION)
		HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, game_pid);	//取得process的handle

		//判斷是否有抓到handle
		if (pHandle != NULL) {
			//準備讀取陽光位置 [[[popcapgame1.exe + 0x002a9f38] + 0x768] + 0x5560]
			DWORD dwReaded = 0;	//tmp使用
			DWORD target_addr = game_image_base + 0x002a9f38;	//欲讀取記憶體位置(遊戲記憶體位置 + 第0層 offset)
			DWORD dwSunny = 0;	//陽光記憶體位置
			DWORD dwOffset1 = 0x00000768;	//指向第一級指標 offset
			DWORD dwOffset2 = 0x00005560;	//指向第二級指標 offset

			/*
			BOOL WINAPI ReadProcessMemory(
			_In_ HANDLE hProcess,				//可存取目標的process handle
			_In_ LPVOID IpBaseAddress,			//欲讀取的記憶體位址
			_Out_ LPVOID IpBuffer,				//讀取後的結果
			_In_ SIZE_T nSize,					//欲讀取的記憶體大小
			_Out_ SIZE_T *IpNumberOfBytesRead	//真正讀取到的記憶體大小(錯誤判斷)
			)
			回傳1代表成功 0代表失敗
			*/
			if (ReadProcessMemory(pHandle, (LPVOID)target_addr, &dwSunny, 4, &dwReaded)) {	//[popcapgame1.exe + 0x002a9f38]
				target_addr = dwSunny + dwOffset1;	//加入第一級指標
				if (ReadProcessMemory(pHandle, (LPVOID)target_addr, &dwSunny, 4, &dwReaded)) {	//[[popcapgame1.exe + 0x002a9f38] + 0x768]
					target_addr = dwSunny + dwOffset2;	//加入第二級指標
					if (ReadProcessMemory(pHandle, (LPVOID)target_addr, &dwSunny, 4, &dwReaded)) {	//[[[popcapgame1.exe + 0x002a9f38] + 0x768] + 0x5560]
						printf("Get Sunny Memory Address:0x%x Count:%d\n", target_addr, dwSunny);

						//竄改內容
						DWORD dwWrited = 0;
						dwSunny = 9999;

						/*
						BOOL WINAPI WriteProcessMemory(
						_In_ HANDLE hProcess,				//可存取目標的process handle
						_In_ LPVOID IpBaseAddress,			//欲寫入的記憶體位址
						_Out_ LPVOID IpBuffer,				//欲寫入的資料
						_In_ SIZE_T nSize,					//欲寫入的記憶體大小
						_Out_ SIZE_T *IpNumberOfBytesRead	//真正讀取到的記憶體大小(錯誤判斷)
						)
						*/

						while (TRUE) {
							if (WriteProcessMemory(pHandle, (LPVOID)target_addr, &dwSunny, 4, &dwReaded)) {
								Sleep(20);

							}
						}
					}
				}
			}
			else {
				printf("Read Process Error!\n");
			}
			CloseHandle(pHandle);
		}
		else {
			DWORD error = GetLastError();
			printf("OpenProcess failed with error code %lu\n", error);
		}
	}
	else {
		printf("Please Open Game!\n");
	}






	printf("Waiting For End...\n");
	getchar();

	return 0;
}