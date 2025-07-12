/******************************************************************************
PWChangeApp - Changes password at elevated level.

(c) 2025 Beem Media
******************************************************************************/

#pragma comment(lib, "netapi32.lib")

#include <conio.h>
#include <functional>
#include <stdio.h>
#include <string>
#include <Windows.h>
#include <LM.h>

enum class EChangeType
{
	None ,
	LanManager ,
	NetCommand ,
};

static const EChangeType PWChangeApp_ChangeType = EChangeType::LanManager;

template<class T> static void PWChangeApp_zero( T Obj )
{
	memset( Obj , 0 , sizeof(*Obj) );
}

template<typename ... Args> static void PWChangeApp_unused( Args ... ) { }

static bool PWChangeApp_UseLanManager(const wchar_t* DomainName, const wchar_t* Username, const wchar_t* OldPassword, const wchar_t* Password)
{
	const DWORD Result = NetUserChangePassword(DomainName, Username, OldPassword, Password);

	if (Result != NERR_Success)
	{
		const wchar_t* ErrorMessage = L"Unknown Error";
		switch (Result)
		{
		case ERROR_ACCESS_DENIED: ErrorMessage = L"Access denied."; break;
		case ERROR_INVALID_PASSWORD: ErrorMessage = L"Invalid password."; break;
		case NERR_InvalidComputer: ErrorMessage = L"Invalid computer."; break;
		case NERR_NotPrimary: ErrorMessage = L"Not primary domain."; break;
		case NERR_UserNotFound: ErrorMessage = L"User not found."; break;
		case NERR_PasswordTooShort: ErrorMessage = L"Password requirements not met."; break;
		}

		wprintf(L"%s (%d)\n", ErrorMessage, Result);
	}

	return Result == NERR_Success;
}

static bool PWChangeApp_UseNetCommand(const wchar_t* DomainName, const wchar_t* Username, const wchar_t* OldPassword, const wchar_t* Password)
{
	PWChangeApp_unused( DomainName , OldPassword );

	std::wstring ArgsLine = L"user " + std::wstring(Username) + L" " + std::wstring(Password);
	//wprintf( L"Command line: net %s\n" , ArgsLine.c_str() );

	int ResOut = -1;
	//We'll flush the console so that this output appears after any output
	//that has been put out by this compiler.
	fflush(stdout);
	fflush(stderr);

	BOOL bSucc = FALSE;
	/*
	SECURITY_ATTRIBUTES SA;
	PWChangeApp_zero( &SA );
	SA.nLength = sizeof(SA);
	SA.bInheritHandle = TRUE;
	SA.lpSecurityDescriptor = NULL;

	STARTUPINFO SI;
	PWChangeApp_zero(&SI);
	SI.cb = sizeof(SI);
	SI.dwFlags = STARTF_USESTDHANDLES;
	SI.hStdOutput = ::GetStdHandle( STD_OUTPUT_HANDLE );
	SI.hStdError  = ::GetStdHandle( STD_ERROR_HANDLE );
	SI.hStdInput  = ::GetStdHandle( STD_INPUT_HANDLE );


	PROCESS_INFORMATION PI;
	PWChangeApp_zero(&PI);

	bSucc = CreateProcess(NULL, const_cast<LPTSTR>(strCmd), &SA, &SA, TRUE, 0, NULL, NULL, &SI, &PI);
	*/
	SHELLEXECUTEINFO PI;
	PWChangeApp_zero( &PI );
	PI.cbSize = sizeof(PI);
	PI.fMask = SEE_MASK_NOCLOSEPROCESS;
	PI.hwnd = 0;
	PI.lpVerb = L"runas";                // Operation to perform
	PI.lpFile = L"net";       // Application to start    
	PI.lpParameters = ArgsLine.c_str();                  // Additional parameters
	PI.lpDirectory = 0;
	PI.nShow = SW_SHOW;
	PI.hInstApp = 0;  

	bSucc = ShellExecuteEx( &PI );

	if(bSucc)
	{
		DWORD Res = WaitForSingleObject(PI.hProcess, INFINITE);
		bSucc = SUCCEEDED(Res);
	}

	if( bSucc )
	{
		DWORD Res;
		bSucc = GetExitCodeProcess(PI.hProcess, &Res);
		if( bSucc )
		{
			ResOut = Res;
		}
		CloseHandle( PI.hProcess );
	}

	if(!bSucc)
	{
		wprintf( L"Failed to process the net command. The error code was %08X.\n" , ResOut );
	}

	return bSucc;
}

static bool PWChangeApp_ChangePassword( const wchar_t* DomainName , const wchar_t* Username , const wchar_t* OldPassword , const wchar_t* Password )
{
	std::function<bool(const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*)> Callback;

	switch (PWChangeApp_ChangeType)
	{
	case EChangeType::None:
		break;
	case EChangeType::LanManager:
		Callback = PWChangeApp_UseLanManager;
		break;
	case EChangeType::NetCommand:
		Callback = PWChangeApp_UseNetCommand;
		break;		
	}

	if (Callback)
	{
		return Callback(DomainName, Username, OldPassword, Password);
	}

	return false;
}

static bool PWChangeApp_ValidatePassword(const wchar_t* DomainName, const wchar_t* Username, const wchar_t* Password)
{
	HANDLE LoginToken = NULL;
	const BOOL bResult = LogonUserW( Username , DomainName , Password , LOGON32_LOGON_NETWORK_CLEARTEXT, LOGON32_PROVIDER_DEFAULT, &LoginToken);
	if (bResult)
	{
		CloseHandle(LoginToken);
		return true;
	}

	return false;
}


int wmain( int argc , wchar_t* argv[] )
{
	wprintf( L"PWChangeApp (c) 2025 Beem Media\n" );

	if (argc < 2)
	{
		wprintf(L"Not enough arguments provided...\n");
		wprintf(L"Usage: PWChangeApp.exe ChangePW|ValidatePW\n");
		return 1;
	}

	bool bOpSucceded = false;

	if (0 == _wcsicmp(argv[1] , L"ChangePW"))
	{
		if (argc < 6)
		{
			wprintf(L"Not enough arguments provided...\n");
			wprintf(L"Usage: PWChangeApp.exe ChangePW DomainName UserName OldPassword NewPassword\n");
			return 1;
		}

		bOpSucceded = PWChangeApp_ChangePassword(argv[2], argv[3], argv[4], argv[5]);

		if (bOpSucceded)
		{
			wprintf(L"Change succeeded.\n");
		}
		else
		{
			wprintf(L"Change failed.\n");
		}
	}
	else if (0 == _wcsicmp(argv[1] , L"ValidatePW"))
	{
		if (argc < 5)
		{
			wprintf(L"Not enough arguments provided...\n");
			wprintf(L"Usage: PWChangeApp.exe ValidatePW DomainName UserName Password\n");
			return 1;
		}

		bOpSucceded = PWChangeApp_ValidatePassword(argv[2], argv[3], argv[4]);
	}

	return bOpSucceded ? 0 : 2;
}
