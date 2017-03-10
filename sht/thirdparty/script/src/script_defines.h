#pragma once
#ifndef __CONSOLE_SCRIPT_DEFINES_H__
#define __CONSOLE_SCRIPT_DEFINES_H__

#include <string>

namespace console_script {

#ifndef PARSER_WIDE_STRING
#define CS_CHAR char
#define CS_TEXT(str) str
	typedef std::string String;
#else
#define CS_CHAR wchar_t
#define CS_TEXT(str) L##str
	typedef std::wstring String;
#endif

#ifndef PARSER_HIGHP_FLOAT
	typedef float Float;
#else
	typedef double Float;
#endif

} // namespace console_script

#endif