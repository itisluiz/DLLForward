"// DLL Forward by itisluiz v" PROJECT_VERSION R"(
#pragma once
#include <cstdint>
#include <Windows.h>

namespace dllforward
{
namespace internal
{
struct Export
{ 
	void(*method)(); 
	const char* name; 
	const uint16_t ordinal;
	const uint32_t rva;
};

#pragma optimize("", off)

static volatile uint16_t volatileWord;
static __declspec(noinline) void __CALL_DUMMY() { volatileWord = 0; }
)"