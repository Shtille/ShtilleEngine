#pragma once
#ifndef __SHT_GRAPHICS_RENDERER_OPENGL_RENDERER_H__
#define __SHT_GRAPHICS_RENDERER_OPENGL_RENDERER_H__

#include "../renderer.h"

namespace sht {
	namespace graphics {

		class OpenGlRenderer final : public Renderer {
		public:
			OpenGlRenderer(int w, int h);
			virtual ~OpenGlRenderer();

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
			void ChangeRenderTargetsToCube(u8 nTargets, Texture* *colorRTs, Texture* depthRT, int face, int level);
			void GenerateMipmap(Texture* texture);
			void CopyToTexture(Texture* texture, u32 layer = 0);

			void AddVertexFormat(VertexFormat* &vf, VertexAttribute *attribs, u32 nAttribs);
			void ChangeVertexFormat(VertexFormat* vf);
			void DeleteVertexFormat(VertexFormat* vf);

			void AddVertexBuffer(VertexBuffer* &vb, u32 size, void *data, BufferUsage usage);
			void DeleteVertexBuffer(VertexBuffer* vb);

			void AddIndexBuffer(IndexBuffer* &ib, u32 nIndices, u32 indexSize, void *data, BufferUsage usage);
			void DeleteIndexBuffer(IndexBuffer* ib);

			bool AddShader(Shader* &shader, const char* filename, const char **attribs = NULL, u32 n_attribs = 0);
			void DeleteShader(Shader* shader);

			void AddFont(Font* &font, const char* fontname);
			void DeleteFont(Font* font);

			void ReadPixels(int w, int h, u8 *data);

            void ClearColor(f32 r, f32 g, f32 b, f32 a);
			void ClearColorBuffer(void);
			void ClearColorAndDepthBuffers(void);
			void ClearDepthBuffer(void);
            void ClearStencil(s32 value);
            void ClearStencilBuffer();

			void ChangeBlendFunc(u32 source, u32 dest);
			void EnableBlend(void);
			void DisableBlend(void);

			void EnableDepthTest(void);
			void DisableDepthTest(void);
			void EnableDepthWrite(void);
			void DisableDepthWrite(void);
            
            void EnableStencilTest(void);
            void DisableStencilTest(void);

			void EnableWireframeMode(void);
			void DisableWireframeMode(void);
            
            void CullFace(CullFaceType mode);

		private:
			void SetDefaultStates();
			void ApiAddTexture(Texture* &tex, const Image &img, Texture::Wrap wrap, Texture::Filter filt);
			void ApiAddTextureCubemap(Texture* &tex, Image *imgs);
			void ApiDeleteTexture(Texture* tex);
            void ApiViewport(int width, int height);
			void ChangeImageUnit(u32 unit);

			u32 framebuffer_;				//!< OpenGL framebuffer object
			u32 current_image_unit_;		//!< current image unit
			u32 current_render_targets_;	//!< current render targets count
		};

	} // namespace graphics
} // namespace sht

#endif