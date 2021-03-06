//=================================================================================================
// Util.cpp - Utility functions
//=================================================================================================

// Global includes
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <dxerr.h>

// Project includes
#include <client/graphics/Util.h>

#pragma comment(lib, "dxerr.lib")

//=================================================================================================

std::wstring Util::Win32ErrorToString(DWORD dwErrorCode)
{
	wchar_t* szMsgBuffer;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwErrorCode, 0, (LPTSTR)&szMsgBuffer, 0, NULL);
	std::wstring strRet(szMsgBuffer);
	LocalFree(szMsgBuffer);
	return strRet;
}

//=================================================================================================

std::wstring Util::DXErrorToString(HRESULT hResult)
{
	return DXGetErrorString(hResult);
}

//=================================================================================================
