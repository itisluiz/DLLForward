R"(
static HMODULE setup()
{
	HMODULE hProxiedDLL{ LoadLibraryA(internal::proxiedDll) };

	for (const internal::Export& exportEntry : internal::exports)
	{
		uintptr_t pProxiedMethod{ reinterpret_cast<uintptr_t>(hProxiedDLL) + exportEntry.rva };
		uintptr_t pProxyMethod{ reinterpret_cast<uintptr_t>(exportEntry.method) };

		DWORD originalProtect, newProtect;
		VirtualProtect(reinterpret_cast<void*>(pProxyMethod), 5, PAGE_EXECUTE_READWRITE, &originalProtect);
		*reinterpret_cast<uint8_t*>(pProxyMethod) = 0xE9;
		*reinterpret_cast<int32_t*>(pProxyMethod + 1) = static_cast<int32_t>(pProxiedMethod - pProxyMethod - 5);
		VirtualProtect(reinterpret_cast<void*>(pProxyMethod), 5, originalProtect, &newProtect);
	}

	return hProxiedDLL;
}
}
)"