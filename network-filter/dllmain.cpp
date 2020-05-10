// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "framework.h"
#include "export.h"
#include "path.h"

#define DBG_ATTACH 0

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	entrypoint();

	LONG error;
	(void)hModule;
	(void)lpReserved;

	if (DetourIsHelperProcess()) {
		return TRUE;
	}

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DetourRestoreAfterWith();


#if _DEBUG
		printf("network-filter" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:" " Starting.\n");
		fflush(stdout);
#endif

#if _DEBUG && DBG_ATTACH
		printf("Attach debugger then press any key...");
		fflush(stdout);
		{ char any; scanf_s("%c", &any, 1); }
#endif
		error = DetourAttach();

		if (error == NO_ERROR) {
#if _DEBUG
			printf("network-filter" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:" " Detoured accept, recv, send, closesocket.\n");
#endif
#if _LOG
			DetourStartBackground();
#endif
		}
		else {
#if _DEBUG
			printf("network-filter" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:" " Error detouring accept & recv & send: %d\n", error);
#endif
		}
	case DLL_PROCESS_DETACH:
		if (false) { //ignore exit. if process really exits then we don't really care then, do we?
			error = DetourDetach();
#if _LOG
			DetourStopBackground();
#endif 
#if _DEBUG

			printf("network-filter" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:" " Removed accept, recv, send, closesocket (result=%d).\n", error);
			fflush(stdout);
#endif
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}