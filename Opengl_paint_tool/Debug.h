#ifndef __DEBUG_H_
#define __DEBUG_H_

#include<Windows.h>

namespace Debug
{
	void Log(wchar_t* buffer, float value)
	{
		swprintf_s(buffer, 256, L"%f", value);

		MessageBox(NULL, buffer, TEXT("Debug"), MB_OK);
	}
	void Log(LPCWSTR str)
	{
		MessageBox(NULL, str, TEXT("Debug"), MB_OK);
	}
}

#endif