#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <filesystem>

namespace fs = std::filesystem;

enum class Architecture : uint8_t
{
    kNotNT,
    kUnknown,
    kI386,
    kAMD64
};

struct Export
{
    const std::string name;
    const uint16_t ordinal;
    const uint32_t rva;

public:
    friend std::ostream& operator<<(std::ostream& ostream, const Export& exportEntry);

};

Architecture parseArchitecture(const fs::path& path);
std::vector<Export> parseExports(const fs::path& path);
std::string parseMangled(const std::string& mangledName);
