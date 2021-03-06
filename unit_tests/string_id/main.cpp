#include "sht/utility/include/string_id.h"

#include <stdio.h>

#define TEST_STRING "Quick brown fox jumps over the lazy dog"

enum {
	kCompileTimeValue = SID(TEST_STRING)
};

int main()
{
    const char* kString = TEST_STRING;
    const sht::utility::StringId run_time_id = MakeStringId(kString);
    const sht::utility::StringId compile_time_id = kCompileTimeValue;
    printf("%s\n", kString);
    if (run_time_id == compile_time_id)
        printf("Good, strings are the same (%x)\n", compile_time_id);
    else
        printf("Bad, strings differ from each other (rt: %x, ct: %x)\n", run_time_id, compile_time_id);

    return 0;
}