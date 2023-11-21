
# DLL Forward
![](https://i.imgur.com/slrvrXM.png)

DLL Forward is a tool that allows for creation of, x86 or x64, mangled or unmangled signature, DLL proxies, redirecting the exports of an arbitrary DLL through your DLL instead.

## Usage

After downloading the binaries or building the project for yourself, you may use the generated executable as follows:

```bash
DLLForward <input> <output (optional)>
```
Where the parameters represent:
- **input**: The input DLL file path which will be proxied, choose the desired architecture instance
- **output**: The output path, which is a **header (*.h)** file containing the necessary information for proxying the input DLL.

## Example
```bash
DLLForward "C:/Windows/System32/msimg32.dll" "./outheader.h"
```
Should generate the following header file at **outheader.h**:


```cpp
// DLL Forward by itisluiz v1.0
#pragma once
#include <cstdint>
#include <Windows.h>

namespace dllforward
{
namespace internal
{
struct Export
{ 
	void(*method)(); 
	const char* name; 
	const uint16_t ordinal;
	const uint32_t rva;
};

#pragma optimize("", off)

static volatile uint16_t volatileWord;
static __declspec(noinline) void __CALL_DUMMY() { volatileWord = 0; }

// Proxy header generated for msimg32.dll (64 bit)
static_assert(sizeof(void*) == 8, "The proxied DLL must match the architecture of the proxy DLL");

// 2 -> AlphaBlend (RVA 0x1010)
void __EXPORT_DUMMY2()
{
#pragma comment(linker, "/EXPORT:AlphaBlend=" __FUNCDNAME__ ",@2")
	__CALL_DUMMY();
	volatileWord = 2;
}

// 3 -> DllInitialize (RVA 0x1180)
void __EXPORT_DUMMY3()
{
#pragma comment(linker, "/EXPORT:DllInitialize=" __FUNCDNAME__ ",@3")
	__CALL_DUMMY();
	volatileWord = 3;
}

// 4 -> GradientFill (RVA 0x1620)
void __EXPORT_DUMMY4()
{
#pragma comment(linker, "/EXPORT:GradientFill=" __FUNCDNAME__ ",@4")
	__CALL_DUMMY();
	volatileWord = 4;
}

// 5 -> TransparentBlt (RVA 0x1570)
void __EXPORT_DUMMY5()
{
#pragma comment(linker, "/EXPORT:TransparentBlt=" __FUNCDNAME__ ",@5")
	__CALL_DUMMY();
	volatileWord = 5;
}

// 1 -> vSetDdrawflag (RVA 0x1560)
void __EXPORT_DUMMY1()
{
#pragma comment(linker, "/EXPORT:vSetDdrawflag=" __FUNCDNAME__ ",@1")
	__CALL_DUMMY();
	volatileWord = 1;
}

#pragma optimize("", on)

constexpr char proxiedDll[]{ "C:/Windows/System32/msimg32.dll" };
constexpr Export exports[]{ { __EXPORT_DUMMY2, "AlphaBlend", 2, 0x1010 }, { __EXPORT_DUMMY3, "DllInitialize", 3, 0x1180 }, { __EXPORT_DUMMY4, "GradientFill", 4, 0x1620 }, { __EXPORT_DUMMY5, "TransparentBlt", 5, 0x1570 }, { __EXPORT_DUMMY1, "vSetDdrawflag", 1, 0x1560 } };
}

static HMODULE setup()
{
	HMODULE hProxiedDLL{ LoadLibraryA(internal::proxiedDll) };

	for (const internal::Export& exportEntry : internal::exports)
	{
		uintptr_t pProxiedMethod{ reinterpret_cast<uintptr_t>(hProxiedDLL) + exportEntry.rva };
		uintptr_t pProxyMethod{ reinterpret_cast<uintptr_t>(exportEntry.method) };

#ifdef _WIN64
		uint8_t opcodes[14]{ 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
		*reinterpret_cast<uintptr_t*>(opcodes + 6) = pProxiedMethod;
#else
		uint8_t opcodes[5]{ 0xE9 };
		*reinterpret_cast<uintptr_t*>(opcodes + 1) = pProxiedMethod - pProxyMethod - sizeof(opcodes);
#endif

		DWORD originalProtect, newProtect;
		uint8_t* pProxyMethodBytes{ reinterpret_cast<uint8_t*>(pProxyMethod) };
		VirtualProtect(pProxyMethodBytes, sizeof(opcodes), PAGE_EXECUTE_READWRITE, &originalProtect);
		memcpy_s(pProxyMethodBytes, sizeof(opcodes), opcodes, sizeof(opcodes));
		VirtualProtect(pProxyMethodBytes, sizeof(opcodes), originalProtect, &newProtect);
	}

	return hProxiedDLL;
}
}

```

### Setup and initialization notes
- This header file should be included in your project where the implementation of the `DllMain` entry point is.
- You must call `dllforward::setup()` in DllMain when `fdwReason` is `DLL_PROCESS_ATTACH`

The header is mostly composed of boilerplate in `dllforward::internal` for exporting the functions with the same signature and ordinal as the original DLL, and `setup()` implements the logic to redirect (patch) the dummy exports to real the real exports.

**Note:** `proxiedDll` is the path the process will look for the original, proxied DLL. If you wish to work with relative paths, keep in mind the path will be relative to the cwd of the process that loads the DLL forwarder. 
