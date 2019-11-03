// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <memory>
#include <chrono>
#include <thread>
#include <iostream>

//-------------------------------------------------------------
// Our Export Function (Call This)
//-------------------------------------------------------------
extern "C" __declspec( dllexport ) int OurFunction( int code, WPARAM wParam, LPARAM lParam ) {

	/* Allocate Console To Show Outputs */
	AllocConsole( );
	freopen( "CONOUT$", "w", stdout );

	/* Print Parent Process Base Address (Passing nullptr Will Return Target/Parent Process' Base Address) */
	std::cout << "Base Address Of Target Process: " << GetModuleHandle( nullptr ) << std::endl;

	MessageBox(
		NULL,
		( LPCWSTR )L"Export Called Successfully",
		( LPCWSTR )L"Test",
		MB_OK
	);

	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		MessageBox(
			NULL,
			( LPCWSTR )L"Dll Loaded",
			( LPCWSTR )L"Test",
			MB_OK
		);

		return TRUE;
	}
	default:
        break;
    }
    return TRUE;
}

