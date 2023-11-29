#pragma once
#include <parser.hh>
#include <filesystem>

namespace fs = std::filesystem;

void buildResultHeader(const fs::path& dllPath, const fs::path& outFile, Architecture architecture, const std::vector<Export>& exports);
void buildResultDefinition(const fs::path& dllPath, const fs::path& outFile, const std::vector<Export>& exports);
