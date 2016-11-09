#include<Windows.h>

void DebugLog(LPCWSTR string)
{
	MessageBox(NULL, string, TEXT("Debug"), MB_OK | MB_ICONSTOP);
}