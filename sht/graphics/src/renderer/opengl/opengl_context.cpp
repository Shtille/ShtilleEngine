# include "../../../include/renderer/opengl/opengl_context.h"
#include "opengl_include.h"

namespace sht {
    namespace graphics {
        
        OpenGlContext::OpenGlContext()
        {
            
        }
        OpenGlContext::~OpenGlContext()
        {
            
        }
        void OpenGlContext::ClearColor(f32 r, f32 g, f32 b, f32 a)
        {
            glClearColor(r, g, b, a);
        }
        void OpenGlContext::ClearColorBuffer()
        {
            glClear(GL_COLOR_BUFFER_BIT);
        }
        void OpenGlContext::ClearDepthBuffer()
        {
            glClear(GL_DEPTH_BUFFER_BIT);
        }
        void OpenGlContext::ClearColorAndDepthBuffers()
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        void OpenGlContext::Viewport(int w, int h)
        {
            glViewport(0, 0, w, h);
        }
        void OpenGlContext::EnableBlend()
        {
            glEnable(GL_BLEND);
        }
        void OpenGlContext::DisableBlend()
        {
            glDisable(GL_BLEND);
        }
        void OpenGlContext::EnableDepthTest()
        {
            glEnable(GL_DEPTH_TEST);
        }
        void OpenGlContext::DisableDepthTest()
        {
            glDisable(GL_DEPTH_TEST);
        }
        void OpenGlContext::EnableDepthWrite()
        {
            glDepthMask(GL_TRUE);
        }
        void OpenGlContext::DisableDepthWrite()
        {
            glDepthMask(GL_FALSE);
        }
        void OpenGlContext::EnableWireframeMode()
        {
            glPolygonMode(GL_FRONT, GL_LINE);
        }
        void OpenGlContext::DisableWireframeMode()
        {
            glPolygonMode(GL_FRONT, GL_FILL);
        }
        void OpenGlContext::DrawArrays(u32 mode, s32 first, u32 count)
        {
            glDrawArrays(mode, first, count);
        }
        void OpenGlContext::DrawElements(u32 mode, u32 num_indices, u32 index_type)
        {
            glDrawElements(mode, num_indices, index_type, 0);
        }
        
    } // namespace graphics
} // namespace sht