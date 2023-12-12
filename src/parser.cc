#include <parser.hh>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <imagehlp.h>

std::ostream& operator<<(std::ostream& ostream, const Export& exportEntry)
{
    std::ios_base::fmtflags flags{ ostream.flags() };
    ostream << std::dec << exportEntry.ordinal << " -> " << exportEntry.name << " (RVA 0x" << std::hex << exportEntry.rva << ')';
    ostream.flags(flags);

    return ostream;
}

Architecture parseArchitecture(const fs::path& path)
{
    IMAGE_DOS_HEADER dosHeader;
    IMAGE_NT_HEADERS32 ntHeader32;

#ifndef _WIN64
    PVOID oldRedirection{ NULL };
    if (!Wow64DisableWow64FsRedirection(&oldRedirection))
        throw std::system_error(std::error_code(GetLastError(), std::system_category()), "Failed to disable Wow64Fs redirection");
#endif

    std::ifstream file(path, std::ios::binary);

#ifndef _WIN64
    if (!Wow64RevertWow64FsRedirection(oldRedirection))
        throw std::system_error(std::error_code(GetLastError(), std::system_category()), "Failed to revert Wow64Fs redirection");
#endif

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    file.read(reinterpret_cast<char*>(&dosHeader), sizeof(dosHeader));

    if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) return Architecture::kNotNT;

    file.seekg(dosHeader.e_lfanew);
    file.read(reinterpret_cast<char*>(&ntHeader32), sizeof(ntHeader32));

    if (ntHeader32.Signature != IMAGE_NT_SIGNATURE) return Architecture::kNotNT;
    if (ntHeader32.FileHeader.Machine == IMAGE_FILE_MACHINE_I386) return Architecture::kI386;
    if (ntHeader32.FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64) return Architecture::kAMD64;

    return Architecture::kUnknown;
}

std::vector<Export> parseExports(const fs::path& path)
{
    _LOADED_IMAGE LoadedImage;

#ifndef _WIN64
    PVOID oldRedirection{ NULL };
    if (!Wow64DisableWow64FsRedirection(&oldRedirection))
        throw std::system_error(std::error_code(GetLastError(), std::system_category()), "Failed to disable Wow64Fs redirection");
#endif

    if (!MapAndLoad(path.u8string().c_str(), nullptr, &LoadedImage, TRUE, TRUE))
        throw std::system_error(std::error_code(GetLastError(), std::system_category()), "MapAndLoad failed to load DLL");

#ifndef _WIN64
    if (!Wow64RevertWow64FsRedirection(oldRedirection))
        throw std::system_error(std::error_code(GetLastError(), std::system_category()), "Failed to revert Wow64Fs redirection");
#endif

    std::vector<Export> exportVector;

    ULONG cDirSize;
    _IMAGE_EXPORT_DIRECTORY* imageExportDirectory = reinterpret_cast<_IMAGE_EXPORT_DIRECTORY*>(
        ImageDirectoryEntryToData(LoadedImage.MappedAddress, FALSE, IMAGE_DIRECTORY_ENTRY_EXPORT, &cDirSize));

    if (imageExportDirectory) {
        DWORD* dNameRVAs = reinterpret_cast<DWORD*>(
            ImageRvaToVa(LoadedImage.FileHeader, LoadedImage.MappedAddress, imageExportDirectory->AddressOfNames, nullptr));
        WORD* dOrdinals = reinterpret_cast<WORD*>(
            ImageRvaToVa(LoadedImage.FileHeader, LoadedImage.MappedAddress, imageExportDirectory->AddressOfNameOrdinals, nullptr));
        DWORD* dFunctions = reinterpret_cast<DWORD*>(
            ImageRvaToVa(LoadedImage.FileHeader, LoadedImage.MappedAddress, imageExportDirectory->AddressOfFunctions, nullptr));

        for (size_t i{ 0 }; i < imageExportDirectory->NumberOfNames; ++i)
        {
            char* exportName = reinterpret_cast<char*>(
                ImageRvaToVa(LoadedImage.FileHeader, LoadedImage.MappedAddress, dNameRVAs[i], nullptr));

            Export exportEntry{ exportName, static_cast<uint16_t>(dOrdinals[i] + 1), static_cast<uint32_t>(dFunctions[dOrdinals[i]]) };
            exportVector.push_back(exportEntry);
        }
    }
    UnMapAndLoad(&LoadedImage);

    return exportVector;
}

std::string parseMangled(const std::string& mangledName)
{
    std::string unmangledName;
	unmangledName.resize(512);

    DWORD writtenChars{ UnDecorateSymbolName(mangledName.c_str(), unmangledName.data(), static_cast<DWORD>(unmangledName.size()), UNDNAME_COMPLETE) };
    if (writtenChars)
    {   
        unmangledName.resize(writtenChars);
        unmangledName.shrink_to_fit();
		return unmangledName;
    }
	else
		return mangledName;
}
