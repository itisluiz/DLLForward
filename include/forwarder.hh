#pragma once
#include <filesystem>

namespace fs = std::filesystem;

bool makeProxy(const fs::path& dllPath, const fs::path& outFile);
