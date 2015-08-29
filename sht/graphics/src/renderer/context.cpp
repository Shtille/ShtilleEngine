#include "../../include/renderer/context.h"
#include <assert.h>

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
            assert(!message);
        }
        
    }
}