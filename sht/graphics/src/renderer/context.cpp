#include "../../include/renderer/context.h"

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
            fprintf(stdout, "%s\n", message);
        }
        
    }
}