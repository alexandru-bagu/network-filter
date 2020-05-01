# network-filter 

## Description
Small attachable DLL that hooks basic winsock2 functions (connect, accept, recv, send, closesocket) to monitor active connections of a program.
The DLL can be injected or it can be "fed" into the program by altering the imports. If the integrity of the program does not matter (code-sign validation or any other checks like that) adding an import is the easier solution long term (avoids antivirus issues) - check out [NTCore Explorer Suite](https://ntcore.com/?page_id=388).

The hooking is done via Detours lib which is added as a submodule of this repository for reference. The built libs have been included only for x86 & x64.

## License

Copyright (c) Alexandru Bagu. All rights reserved.

Licensed under the [MIT](LICENSE.md) License.
