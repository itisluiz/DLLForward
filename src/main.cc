#include <forwarder.hh>
#include <CLI/CLI.hpp>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

struct ExitCodes
{
	enum
	{
		kSuccess = 0,
		kFailed = 1,
		kBadArgs = 2
	};
};

int main(int argc, char* argv[])
{
	CLI::App app("Generate files for creating a DLL Proxy (Man in the middle) to any other DLL.", "DLLForward");
	
	fs::path dllPath, outFile;
	bool defMode{ false };

	app.add_option("input", dllPath, "The DLL that will be proxied, be mindful of the DLL's architecture/bitness.")->required()->check(CLI::ExistingFile);
	app.add_option("output", outFile, "Output path of the resulting file generated from the input DLL.");
	app.add_flag("-d,--def", defMode, "Generate a .def file instead of a .h file.");
	app.set_version_flag("-v,--version", "DLLForward v" PROJECT_VERSION);

	try
	{
		argv = app.ensure_utf8(argv);
		app.parse(argc, argv);
	}
	catch (const CLI::ParseError& e)
	{
		if (e.get_exit_code() == 0)
		{
			std::cout << e.what() << '\n';
			return ExitCodes::kSuccess;
		}

		std::cerr << "Bad argument provided: " << e.what() << '\n';
		return ExitCodes::kBadArgs;
	}
	
	if (outFile.empty() || fs::is_directory(outFile))
		outFile /= dllPath.filename().replace_extension();

	if (!outFile.has_extension())
		outFile = outFile.replace_extension(defMode ? ".def" : ".h");

	if (!fs::is_directory(fs::absolute(outFile).parent_path()))
	{
		std::cerr << "Output directory does not exist" "\n";
		return ExitCodes::kBadArgs;
	}

	if (defMode)
		return makeDefinition(dllPath, outFile) ? ExitCodes::kSuccess : ExitCodes::kFailed;

	return makeHeader(dllPath, outFile) ? ExitCodes::kSuccess : ExitCodes::kFailed;
}
