# Custom GetProcAddress And GetModuleHandle

This project aims to provide a custom implementation of commonly used Windows API functions GetProcAddress() and GetModuleHandle() in malware development for Red Teaming/Offensive Security. 

## Files 
- Custom.h - this is a standalone file and can be included in projects by the user
- Custom.c - used to demonstrate the functionality provided in **Custom.h**
- Compile.bat - Compiles the project using the msvc compiler

## Details

The **GetModuleHandle()** API is used in windows to retrieve a handle/pointer to a modules loaded in memory of a process at runtime. The usage of the API combined with **GetProcAddress()** can **alert AV/EDR of the payload intentions** and may be categorized as malicious. 

Therefore, the custom implementation of **GetModuleHandle()** parses the different data structures like PEB, LDR_DATA_TABLE_ENTRY to find the base address of the module/DLL at runtime. This address is then passed to the **GetProcAddress()** which parses the PE structure and searches for the API address inside the **IMAGE_EXPORT_DIRECTORY**. 

## Note
The Dll whose address is to be found should already be loaded in memory or should be loaded using the **LoadLibrary()** API.

## Showcase

![GetModuleHandle](/pic_GetDllBase.png)
![GetProcAddress](/pic_GetProcAddress.png)
