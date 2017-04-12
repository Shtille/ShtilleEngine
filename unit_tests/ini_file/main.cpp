#include "sht/utility/include/ini_file.h"

#include <stdio.h>

class IniFileReader final : public sht::utility::IniFileReaderInterface {
public:
    void OnSection(const char* section_name) final
    {
        printf("%s\n", section_name);
    }
    void OnPair(const char* key, const char* value) final
    {
        printf("%s=%s\n", key, value);
    }
};

int main()
{
    const char* ini_filename = "test.ini";
    sht::utility::IniFile file;
    printf("Do you wanna test INI saving? Press 'y' in that case.\n");
    char c = fgetc(stdin);
    if (c == 'y')
    {
        if (file.OpenForWrite(ini_filename))
        {
            file.WriteSection("models");
            file.WritePair("sphere", "sphere.obj");
            file.WritePair("box", "box.obj");
            file.WriteSection("textures");
            file.WritePair("diff", "diff.jpg");
            file.WritePair("spec", "spec.jpg");
            file.Close();
        }
        else
            printf("File open failed\n");
    }
    else
    {
        if (file.OpenForRead(ini_filename))
        {
            IniFileReader reader;
            file.Read(&reader);
            file.Close();
        }
        else
            printf("File open failed\n");
    }

    return 0;
}