# DLL Forward
![my machine badge](https://forthebadge.com/images/badges/works-on-my-machine.svg)
![workflow badge](https://github.com/itisluiz/DLLForward/actions/workflows/build.yml/badge.svg)

DLL Forward is a tool that allows for creation of, x86 or x64, mangled or unmangled signature, DLL proxies, redirecting the exports of an arbitrary DLL through your DLL instead.

## Usage
After downloading the binaries or building the project for yourself, you may use the generated executable as follows:

```bash
DLLForward [--def] <input> <output (optional)>
```
Where the parameters represent:
- **input**: The input DLL file path which will be proxied, choose the desired architecture instance (x86 or x64) of the DLL to proxy.
- **output**: The output path, which is a file built from the the input DLL's data, by default a **header (.h)** with the same name as the proxied DLL.
-  **def**: Or `-d` for short, if set will generate a **module definition (.def)** instead of a proxy header from the input DLL.

### Creating a proxy example
```bash
DLLForward "C:/Windows/System32/msimg32.dll" "./msimg32.h"
```

Will generate a header file `msimg32.h` at the current directory, this header must be included in your project for proxying the DLL. It should be included in your project where the implementation of the `DllMain` entry point is as you must call `dllforward::setup()` in DllMain when `fdwReason` is `DLL_PROCESS_ATTACH`.

Example project using the generated header (The project must be configured to build for the same architecture as the proxied DLL)

`main.cpp`
```cpp	
#include "Windows.h"
#include "msimg32.h"

BOOL WINAPI DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// Initialize the proxy for the DLL
		dllforward::setup();

		// Your code here to be executed when the DLL is loaded
	}

	return TRUE;
}
```

**Note:** There might be some need to modify the generated header file to change where the proxy DLL will look for the original DLL in order to load it. By default, it will be set to look for it on the same directory as it was when the header was generated, as an absolute path. The variable containing the path is `dllforward::internal::proxiedDll`.

Having done this you may rename the DLL you built to the name of the original DLL and place it in the same directory as the original DLL, and it will be loaded instead of the original DLL, having it's exports automatically redirected to the loading binary while also having your code run.

### Creating a module definition example

This is an alternate way to use the program where you can build a module definition `.def` file from the input DLL, which can be used to generate a stub library with `lib.exe` or similar tools.

```bash
DLLForward -d "C:/Windows/System32/msimg32.dll" "./msimg32.def"
```

This will generate a module definition file `msimg32.def` at the current directory, it contains information about all the exports of the DLL.

By using a tool such as Microsoft's [lib.exe](https://learn.microsoft.com/en-us/cpp/build/reference/lib-reference?view=msvc-170) you can generate a stub library from the module definition file, which can be used to link against the original DLL, allowing you to create a project that consumes said DLL and properly resolves the imports without manually resolving the addresses with [GetProcAddress](https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress) for example.

Example usage of `lib.exe` to generate a stub library from the module definition file:
```bash
lib /def:msimg32.def /out:msimg32.lib /machine:x64
```

This will generate a stub library `msimg32.lib` at the current directory, which can be used to link against your projects on which you wish to use the DLL.

## Roadmap
- [x] Allow for x86 and x64 proxy DLLs, debug or release.
- [ ] Allow for control of the proxy DLL's header with macros defined before including the header.
- [ ] Optional smarter and more robust ways of locating the original DLL.
- [ ] Create a parent project for generically generating and compiling a proxy DLL, with options to load other DLLs on a text file.

## Building (With Visual Studio)
1. Clone the repository recursively to get the submodules: `git clone --recurse-submodules https://github.com/itisluiz/DLLForward`\
(If you didn't clone the repository recursively, you can initialize the submodules with `git submodule update --init --recursive`)
2. Launch Visual Studio -> Select "Continue without code" -> File -> Open -> CMake -> Open the `CMakeLists.txt` file in the repository root.
3. Select your architecture at the top and build the project with `Ctrl + Shift + B`.

## Contributing
Pull requests are welcome, so are issues with suggestions or bug reports. For major changes, please open an issue first to discuss what you would like to change.
