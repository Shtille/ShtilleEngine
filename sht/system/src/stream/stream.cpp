#include "../../include/stream/stream.h"
#include "../../../common/platform.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

namespace sht {
	namespace system {

		Stream::~Stream()
		{
		}
		bool Stream::WriteText(const char *text)
		{
			return Write(text, strlen(text));
		}
		bool Stream::WriteLine(const char *text)
		{
			return Write(text, strlen(text)) &&
				Write("\n", sizeof(char));
		}
		bool Stream::ReadLine(char string[256])
		{
			bool b;
			while ((b = ReadString(string, 255) && (string[0] == '\n')));
			char& c = string[strlen(string) - 1];
			if (c == '\n') c = '\0';
			return b;
		}
		bool Stream::ReadCodeLine(char string[256])
		{
			bool b;
			while ((b = ReadString(string, 255) && ((string[0] == '/') || (string[0] == '\n'))));
			char& c = string[strlen(string) - 1];
			if (c == '\n') c = '\0';
			return b;
		}
		bool Stream::PrintString(const char *string, ...)
		{
			char		text[256];		// Holds Our String
			va_list		ap;				// Pointer To List Of Arguments

			va_start(ap, string);				// Parses The String For Variables
            vsnprintf(text, 256, string, ap);		// And Converts Symbols To Actual Numbers
			va_end(ap);							// Results Are Stored In Text

			return WriteText(text);
		}
		bool Stream::ScanString(const char *string, ...)
		{
			char		text[256];		// Holds Our String
			va_list		ap;				// Pointer To List Of Arguments

			if (!ReadString(text, 255))
				return false;

			va_start(ap, string);				// Parses The String For Variables
			vsscanf(text, string, ap);			// And Converts Symbols To Actual Numbers
			va_end(ap);							// Results Are Stored In Text

			return true;
		}
		bool Stream::PrintLine(const char *string, ...)
		{
			char		text[256];		// Holds Our String
			va_list		ap;				// Pointer To List Of Arguments

			va_start(ap, string);				// Parses The String For Variables
			vsprintf(text, string, ap);		    // And Converts Symbols To Actual Numbers
			va_end(ap);							// Results Are Stored In Text

			return WriteLine(text);
		}

	} // namespace system
} // namespace sht