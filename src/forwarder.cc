#include <forwarder.hh>
#include <parser.hh>
#include <builder.hh>
#include <iostream>
#include <fstream>

bool makeHeader(const fs::path& dllPath, const fs::path& outFile)
{
	try
	{
		Architecture architecture{ parseArchitecture(dllPath) };
		
		if (architecture != Architecture::kI386 && architecture != Architecture::kAMD64)
			throw std::runtime_error("Bad executable and/or architecture");

		std::cout << "Selected file is a " << (architecture == Architecture::kI386 ? "x86" : "x64") << " binary" "\n";

		std::vector<Export> exports{ parseExports(dllPath) };

		std::cout << "There are " << exports.size() << " exports:" "\n";

		for (size_t hint{ 0 }; hint < exports.size(); ++hint)
		{
			const Export& exportEntry{ exports[hint] };
			std::cout << '\t' << exportEntry << '\n';
		}

		buildResultHeader(dllPath, outFile, architecture, exports);
		std::cout << "Generated output at \"" << fs::absolute(outFile).string() << "\"" "\n";
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
		std::vector<Export> exports{ parseExports(dllPath) };

		std::cout << "There are " << exports.size() << " exports:" "\n";

		for (size_t hint{ 0 }; hint < exports.size(); ++hint)
		{
			const Export& exportEntry{ exports[hint] };
			std::cout << '\t' << exportEntry << '\n';
		}

		buildResultDefinition(dllPath, outFile, exports);
		std::cout << "Generated output at \"" << fs::absolute(outFile).string() << "\"" "\n";
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
