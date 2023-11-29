#pragma once
#include <filesystem>

namespace fs = std::filesystem;

bool makeHeader(const fs::path& dllPath, const fs::path& outFile);
bool makeDefinition(const fs::path& dllPath, const fs::path& outFile);
