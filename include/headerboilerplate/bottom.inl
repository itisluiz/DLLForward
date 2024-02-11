R"(
static HMODULE setup()
{
	HMODULE hProxiedDLL{ LoadLibraryA(internal::proxiedDll) };

	if (!hProxiedDLL)
		return NULL;

	for (const internal::Export& exportEntry : internal::exports)
	{
		// uintptr_t pProxiedMethod{ reinterpret_cast<uintptr_t>(hProxiedDLL) + exportEntry.rva };
		// uintptr_t pProxiedMethod{ reinterpret_cast<uintptr_t>(GetProcAddress(hProxiedDLL, MAKEINTRESOURCEA(exportEntry.ordinal))) };
		uintptr_t pProxiedMethod{ reinterpret_cast<uintptr_t>(GetProcAddress(hProxiedDLL, exportEntry.name)) };

		uintptr_t pProxyMethod{ reinterpret_cast<uintptr_t>(exportEntry.method) };

#ifdef _DEBUG
		if (*reinterpret_cast<uint8_t*>(pProxyMethod) == 0xE9)
		{
			int32_t offset{ *reinterpret_cast<int32_t*>(pProxyMethod + 1) };
			pProxyMethod += offset + 5;
		}
#endif

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
)"