#include "../../include/renderer/context.h"
#include "common\platform.h"

#include <cstdio>

namespace sht {
    namespace graphics {
        
        Context::Context()
        {
            
        }
        Context::~Context()
        {
            
        }
        void Context::ErrorHandler(const char *message)
        {
#if defined(TARGET_WINDOWS)
			OutputDebugStringA(message);
#else
            fprintf(stdout, "%s\n", message);
#endif
        }
        
    }
}