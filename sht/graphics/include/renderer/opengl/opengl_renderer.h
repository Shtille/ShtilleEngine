#pragma once
#ifndef __SHT_GRAPHICS_RENDERER_OPENGL_RENDERER_H__
#define __SHT_GRAPHICS_RENDERER_OPENGL_RENDERER_H__

#include "../renderer.h"
#include <unordered_map>

struct EnumClassHash {
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

namespace sht {
	namespace graphics {

		class OpenGlRenderer final : public Renderer {
		public:
			OpenGlRenderer(int w, int h);
			virtual ~OpenGlRenderer();

			bool CheckForErrors();
			bool CheckFunctionalities();

			f32 GetStringHeight(f32 scale);
			f32 GetStringWidth(f32 scale, const char *str);

			void CreateTextureColor(Texture* &texture, float r, float g, float b, float a);
			void CreateTextureCubemap(Texture* &texture, int w, int h, Image::Format fmt = Image::Format::kRGB8);
			void CreateTextureDepth(Texture* &texture, int w, int h, u32 depthSize);
			void CreateTexture(Texture* &texture, int w, int h, Image::Format fmt);
			void CreateTextureFromData(Texture* &texture, int w, int h, Image::Format fmt, unsigned char *data);
			void AddRenderTarget(Texture* &texture, int w, int h, Image::Format fmt, Texture::Filter filt = Texture::Filter::kLinear);
			void AddRenderDepthStencil(Texture* &texture, int w, int h, u32 depthSize, u32 stencilSize);
			void DeleteTexture(Texture* texture);
			void ChangeTexture(Texture* texture, u32 layer = 0);
			void ChangeRenderTargets(u8 nTargets, Texture* *colorRTs, Texture* depthRT);
			void ChangeRenderTargetsToCube(u8 nTargets, Texture* *colorRTs, Texture* depthRT, int face);
			void GenerateMipmap(Texture* texture);
			void CopyToTexture(Texture* texture, u32 layer = 0);

			void AddVertexFormat(VertexFormat* &vf, VertexAttribute *attribs, u32 nAttribs);
			void ChangeVertexFormat(VertexFormat* vf);
			void DeleteVertexFormat(VertexFormat* vf);

			void AddVertexBuffer(VertexBuffer* &vb, u32 size, void *data, BufferUsage usage);
			void SetVertexBufferData(ptrdiff_t offset, u32 size, void *data);
			void* MapVertexBufferData(u32 size);
			void UnmapVertexBufferData(void);
			void ChangeVertexBuffer(VertexBuffer* vb);
			void DeleteVertexBuffer(VertexBuffer* vb);

			void AddIndexBuffer(IndexBuffer* &ib, u32 nIndices, u32 indexSize, void *data, BufferUsage usage);
			void SetIndexBufferData(ptrdiff_t offset, u32 size, void *data);
			void* MapIndexBufferData(u32 size);
			void UnmapIndexBufferData(void);
			void ChangeIndexBuffer(IndexBuffer* ib);
			void DeleteIndexBuffer(IndexBuffer* ib);

			bool AddShader(Shader* &shader, const char* filename, char **attribs = NULL, int nAttribs = 0);
			void DeleteShader(Shader* shader);
			void ChangeShader(Shader* shader);
			void ChangeShaderAttribBinding(const char *name);
			void ChangeShaderUniform1i(const char* name, int num);
			void ChangeShaderUniform1f(const char* name, float x);
			void ChangeShaderUniform2f(const char* name, float x, float y);
			void ChangeShaderUniform3f(const char* name, float x, float y, float z);
			void ChangeShaderUniform4f(const char* name, float x, float y, float z, float w);
			void ChangeShaderUniform1fv(const char* name, float *v, int n = 1);
			void ChangeShaderUniform2fv(const char* name, float *v, int n = 1);
			void ChangeShaderUniform3fv(const char* name, float *v, int n = 1);
			void ChangeShaderUniform4fv(const char* name, float *v, int n = 1);
			void ChangeShaderUniformMatrix2fv(const char* name, float *v, bool trans = false, int n = 1);
			void ChangeShaderUniformMatrix3fv(const char* name, float *v, bool trans = false, int n = 1);
			void ChangeShaderUniformMatrix4fv(const char* name, float *v, bool trans = false, int n = 1);

			void AddFont(Font* &font, const char* fontname, bool bold, bool italic, bool underline, bool strikeout, u32 family = 0);
			void DeleteFont(Font* font);
			void ChangeFont(Font* font);
			void Print(f32 x, f32 y, f32 s, f32 r, f32 g, f32 b, const char *string, ...);

			void ReadPixels(int w, int h, u8 *data);

            inline void ClearColor(f32 r, f32 g, f32 b, f32 a);
			inline void ClearColorBuffer(void);
			inline void ClearColorAndDepthBuffers(void);
			inline void ClearDepthBuffer(void);

			inline void ChangeBlendFunc(u32 source, u32 dest);
			inline void EnableBlend(void);
			inline void DisableBlend(void);

			inline void EnableDepthTest(void);
			inline void DisableDepthTest(void);
			inline void EnableDepthWrite(void);
			inline void DisableDepthWrite(void);

			inline void EnableWireframeMode(void);
			inline void DisableWireframeMode(void);

			inline void DrawElements(PrimitiveType mode);
			inline void DrawElements(u32 mode, u32 numindices);
			inline void Viewport(int w, int h);

		private:
			void SetDefaultStates();
			bool CheckFrameBufferStatus();
            void FillBufferUsage();
			void ApiAddTexture(Texture* &tex, Image &img, Texture::Wrap wrap, Texture::Filter filt);
			void ApiAddTextureCubemap(Texture* &tex, Image *imgs);
			void ApiDeleteTexture(Texture* tex);
			void ApiDeleteShader(Shader* shd);
			void ApiDeleteFont(Font* font);
			void ApiDeleteVertexFormat(VertexFormat* vf);
			void ApiDeleteVertexBuffer(VertexBuffer* vb);
			void ApiDeleteIndexBuffer(IndexBuffer* ib);
			void ChangeImageUnit(u32 unit);

			u32 framebuffer_;				//!< OpenGL framebuffer object
            u32 vertex_array_object_;       //!< OpenGL vertex array object
			u32 current_image_unit_;		//!< current image unit
			u32 current_render_targets_;	//!< current render targets count
            
            std::unordered_map<BufferUsage, u32, EnumClassHash> buffer_usage_map_; //!< buffer usage map
		};

	} // namespace graphics
} // namespace sht

#endif