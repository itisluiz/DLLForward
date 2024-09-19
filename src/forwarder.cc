#include <forwarder.hh>
#include <parser.hh>
#include <builder.hh>
#include <encoding.hh>
#include <iostream>
#include <fstream>

std::pair<Architecture, std::vector<Export>> acquireArchExports(const fs::path& dllPath)
{
	Architecture architecture{ parseArchitecture(dllPath) };

	if (architecture == Architecture::kNotNT)
		throw std::runtime_error("Input file is not an NT binary");

	std::cout << "Selected file is of ";

	switch (architecture)
	{
	case Architecture::kUnknown:
		std::cout << "unknown";
		break;
	case Architecture::kI386:
		std::cout << "x86";
		break;
	case Architecture::kAMD64:
		std::cout << "x64";
		break;
	}

	std::cout << " architecture" "\n";

	std::vector<Export> exports{ parseExports(dllPath) };

	std::cout << "There are " << exports.size() << " exports:" "\n";

	for (size_t hint{ 0 }; hint < exports.size(); ++hint)
	{
		const Export& exportEntry{ exports[hint] };
		std::cout << '\t' << exportEntry << '\n';
	}

	return std::make_pair(architecture, exports);
}

bool makeHeader(const fs::path& dllPath, const fs::path& outFile)
{
	try
	{
		std::pair<Architecture, std::vector<Export>> archExports{ acquireArchExports(dllPath) };

		buildResultHeader(dllPath, outFile, archExports.first, archExports.second);
		std::cout << "Generated output at " << utf8String(escapedPathString(fs::absolute(outFile))) << '\n';
	}
	catch (const std::system_error& e)
	{
		std::cerr << e.what() << " [" << e.code() << "]" "\n";
		return false;
	}
	catch(const std::runtime_error& e)
	{
		std::cerr << e.what() << '\n';
		return false;
	}

	return true;
}

bool makeDefinition(const fs::path& dllPath, const fs::path& outFile)
{
	try
	{
		std::pair<Architecture, std::vector<Export>> archExports{ acquireArchExports(dllPath) };

		buildResultDefinition(dllPath, outFile, archExports.second);
		std::cout << "Generated output at " << utf8String(escapedPathString(fs::absolute(outFile))) << '\n';
	}
	catch (const std::system_error& e)
	{
		std::cerr << e.what() << " [" << e.code() << "]" "\n";
		return false;
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << '\n';
		return false;
	}

	return true;
}
