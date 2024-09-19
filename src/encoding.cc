#include <encoding.hh>
#include <Windows.h>
#include <sstream>

std::wstring escapedPathString(const fs::path& path)
{
	std::wstringstream wss;
	wss << path;
	return wss.str();
}

// Taken and modified from https://gist.github.com/pavel-a/090cbe4b2aea9a054c55974b7c7be634 (Pavel A.)
std::string ansiPathString(const fs::path& path)
{
	std::wstring widePath{ path.wstring() };

	std::string resultPath;
	resultPath.reserve(widePath.size());

	for (const WCHAR& w : widePath)
	{
		if (w == 0 || w > 0xFF)
			break;

		resultPath += static_cast<char>(w & 0xFF);
	}

	if (resultPath.size() == widePath.size())
		return resultPath;

	WCHAR shortPath[MAX_PATH]{ };
	DWORD r{ GetShortPathNameW(widePath.c_str(), shortPath, MAX_PATH) };

	if (r == 0 || r >= MAX_PATH)
		return { };

	std::wstring shortPathWstr{ shortPath };
	resultPath.clear();

	for (const WCHAR& w : shortPathWstr)
	{
		if (w == 0)
			break;

		if (w > 0xFF)
			return { };

		resultPath += static_cast<char>(w & 0xFF);
	}

	return resultPath;
}

std::string utf8String(const std::wstring& wstr)
{
	int size{ WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr) };
	std::string buffer(size - 1, '\0');
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buffer.data(), size - 1, nullptr, nullptr);
	return buffer;
}
