#pragma once
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

std::wstring escapedPathString(const fs::path& path);
std::string ansiPathString(const fs::path& path);
std::string utf8String(const std::wstring& wstr);
