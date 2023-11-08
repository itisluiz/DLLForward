#pragma once
#include <parser.hh>
#include <filesystem>

namespace fs = std::filesystem;

void buildResult(const fs::path& dllPath, const fs::path& outFile, Architecture architecture, const std::vector<Export>& exports);
