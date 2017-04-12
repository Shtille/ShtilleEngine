#include "../include/ini_file.h"

#include <cstddef>
#include <cstring>
#include <assert.h>

namespace sht {
	namespace utility {

		IniFile::IniFile()
		: file_(nullptr)
		{

		}
		IniFile::~IniFile()
		{
			Close();
		}
		bool IniFile::OpenForWrite(const char* filename)
		{
			file_ = fopen(filename, "wt");
			return file_ != nullptr;
		}
		bool IniFile::OpenForRead(const char* filename)
		{
			file_ = fopen(filename, "rt");
			return file_ != nullptr;
		}
		void IniFile::Close()
		{
			if (file_)
			{
				fclose(file_);
				file_ = nullptr;
			}
		}
		bool IniFile::WriteSection(const char* section_name)
		{
			return fprintf(file_, "[%s]\n", section_name) == 1;
		}
		bool IniFile::WritePair(const char* key, const char* value)
		{
			return fprintf(file_, "%s=%s\n", key, value) == 2;
		}
		void IniFile::Read(IniFileReaderInterface * reader)
		{
			char buffer[256];
			while (!feof(file_) && ReadLine(buffer))
			{
				size_t len = strlen(buffer);
				if (len > 2 && buffer[0] == '[' && buffer[len-1] == ']')
				{
					// Section have been recognized
					buffer[len-1] = '\0';
					reader->OnSection(buffer + 1);
				}
				else
				{
					bool found_pair = false;
					for (char* p = buffer; *p != '\0'; ++p)
						if (*p == '=')
						{
							found_pair = true;
							*p = '\0';
							reader->OnPair(buffer, p + 1);
							break;
						}
					assert(found_pair);
				}
			}
		}
		bool IniFile::ReadLine(char text[256])
		{
			do
			{
				if (fgets(text, 255, file_) == nullptr)
					return false;
			}
			while (!feof(file_) && (text[0] == '\n' || text[0] == '/'));
			char& c = text[strlen(text) - 1];
			if (c == '\n') c = '\0';
			text[255] = '\0';
			return true;
		}

	} // namespace utility
} // namespace sht