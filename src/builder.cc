#include <builder.hh>
#include <fstream>

#define BUILD_EXPORT_IDENTIFIER(ordinal) "__EXPORT_DUMMY" << ordinal
#define BUILD_EXPORT_DUMMY(name, ordinal) "void " << BUILD_EXPORT_IDENTIFIER(ordinal) << "()\n{\n" <<\
    "#pragma comment(linker, \"/EXPORT:" << name << "=\" __FUNCDNAME__ \",@" << ordinal << "\")\n"\
    "\t__CALL_DUMMY();\n"\
    "\tvolatileWord = " << ordinal << ";\n}\n\n"

#define BUILD_EXPORT_ENTRY(name, ordinal, rva) "{ " << BUILD_EXPORT_IDENTIFIER(ordinal) << ", \"" << name << "\", " << ordinal << ", 0x" << std::hex << rva << std::dec << " }, "

void buildResultHeader(const fs::path& dllPath, const fs::path& outFile, Architecture architecture, const std::vector<Export>& exports)
{
    std::ofstream file(outFile);
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    file << 
#include <headerboilerplate/top.inl>
        << '\n';

    file << "// Proxy header generated for " << dllPath.filename().string();
    
    if (architecture != Architecture::kUnknown)
    {
        file << " (" << (architecture == Architecture::kI386 ? "32" : "64") << " bit)" "\n";
        file << "static_assert(sizeof(void*) == " << (architecture == Architecture::kI386 ? 4 : 8) << ", \"The proxied DLL must match the architecture of the proxy DLL\");" "\n\n";
    }
    else
        file << "\n\n";

    for (const Export& exportEntry : exports)
    {
        file << "// #" << exportEntry.ordinal << ": " << exportEntry.name << " (" << parseMangled(exportEntry.name) << ")" "\n";
        file << BUILD_EXPORT_DUMMY(exportEntry.name, exportEntry.ordinal);
    }

    file << "#pragma optimize(\"\", on)" "\n\n";

    file << "constexpr char originalProxiedDll[]{ " << dllPath << " };" "\n";
    file << "constexpr Export exports[]{ ";
    for (const Export& exportEntry : exports)
        file << BUILD_EXPORT_ENTRY(exportEntry.name, exportEntry.ordinal, exportEntry.rva);
    
    file.seekp(-2, std::ios::cur);
    file << " };\n";

    file << "}\n"
#include <headerboilerplate/bottom.inl>
        ;

}

void buildResultDefinition(const fs::path& dllPath, const fs::path& outFile, const std::vector<Export>& exports)
{
    std::ofstream file(outFile);
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    file << "; DLLForward by itisluiz v" PROJECT_VERSION << '\n';

    file << "LIBRARY " << dllPath.filename().replace_extension().string() << '\n';
    file << "EXPORTS";

    for (const Export& exportEntry : exports)
    {
        file << "\n" ";\t#" << exportEntry.ordinal << ": " << parseMangled(exportEntry.name) << '\n';
        file << '\t' << exportEntry.name << '\n';
    }

}
