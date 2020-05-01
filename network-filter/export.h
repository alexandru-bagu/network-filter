#pragma once
#define DllExport   __declspec( dllexport )

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif
    DllExport void _cdecl entrypoint() { }
#ifdef __cplusplus
}
#endif