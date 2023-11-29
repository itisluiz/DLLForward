#include <forwarder.hh>
#include <iostream>
#include <filesystem>
#include <cxxopts.hpp>

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
	cxxopts::Options options(fs::path(argv[0]).filename().string(), "Generate files for creating a DLL Proxy (Man in the middle) to any other DLL.");

	options.add_options()
		("i,input", "Input DLL path", cxxopts::value<std::string>(), "The DLL that will be proxied, be mindful of the DLL's architecture/bitness.")
		("o,output", "Output header file path", cxxopts::value<std::string>()->default_value("./"), "Output path of the resulting file generated from the input DLL.")
		("d,def", "Create a module definition (.def file) instead of a header for proxying.")
		("h,help", "Print usage");

	options.parse_positional({ "input", "output" });
	options.positional_help("input output");
	options.show_positional_help();

	bool argDef;
	fs::path argInput,argOutput;
	try
	{
		cxxopts::ParseResult result{ options.parse(argc, argv) };

		if (result["help"].count())
		{
			std::cout << options.help();
			return ExitCodes::kSuccess;
		}

		argInput = result["input"].as<std::string>();
		argOutput = result["output"].as<std::string>();
		argDef = result["def"].count();
	}
	catch (const cxxopts::exceptions::exception& e)
	{
		std::cerr << options.help() << '\n' << e.what();
		return ExitCodes::kBadArgs;
	}

	if (!fs::is_regular_file(argInput))
	{
		std::cerr << "Invalid input DLL path " << argInput << '\n';
		return ExitCodes::kBadArgs;
	}

	if (!fs::is_directory(argOutput.parent_path()))
	{
		std::cerr << "Invalid output path " << argOutput << '\n';
		return ExitCodes::kBadArgs;
	}

	if (fs::is_directory(argOutput))
		argOutput /= argInput.filename().replace_extension();

	if (!argOutput.has_extension())
		argOutput.replace_extension(argDef ? ".def" : ".h");

	if (argDef)
		return makeDefinition(argInput, argOutput) ? ExitCodes::kSuccess : ExitCodes::kFailed;
	else
		return makeHeader(argInput, argOutput) ? ExitCodes::kSuccess : ExitCodes::kFailed;
}
