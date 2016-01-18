#pragma once
#ifndef __SHT_GRAPHICS_RENDERER_RENDERER_H__
#define __SHT_GRAPHICS_RENDERER_RENDERER_H__

#include "../../../common/types.h"
#include "../../../common/platform.h"
#include "../../../common/counting_pointer.h"
#include "../../../math/sht_math.h"

#include "vertex_format.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "texture.h"
#include "context.h"
#include "shader.h"
#include "font.h"

#include <list>
#include <stack>

namespace sht {
	namespace graphics {

		const int kMaxImageUnit = 16;
		const int kMaxMrt = 4;

		//! Base renderer class
		class Renderer {
		public:
			Renderer(int w, int h);
			virtual ~Renderer();
            
            Context * context();
            int width();
            int height();
            float aspect_ratio();

			void UpdateSizes(int w, int h);

			void Defaults(void);
			void CleanUp(void);
			void CheckForUsing(void); //!< user may do not detach texture or shader in the end of the frame, pure debug feature

			// Obtain memory consumption
			u32 GetUsedVideoMemorySize(void); // textures + vbo + ibo
			u32 GetUsedTexturesSize(void);
			u32 GetUsedVertexBuffersSize(void);
			u32 GetUsedIndexBuffersSize(void);

			void TakeScreenshot(int w, int h, const char* dir, bool is_fullscr);
			void Setup2DMatrix();

			// Texture functions
			bool AddTexture(Texture* &texture, const char* filename,
				Texture::Wrap wrap = Texture::Wrap::kRepeat,
				Texture::Filter filt = Texture::Filter::kTrilinear);
			bool AddTextureCubemap(Texture* &texture, const char* filename);
			bool CreateTextureNormalMapFromHeightMap(Texture* &texture, const char* filename,
				Texture::Wrap wrap = Texture::Wrap::kRepeat,
				Texture::Filter filt = Texture::Filter::kTrilinear);
			bool CreateTextureNormalHeightMapFromHeightMap(Texture* &texture, const char* filename,
				Texture::Wrap wrap = Texture::Wrap::kRepeat,
				Texture::Filter filt = Texture::Filter::kTrilinear);
			virtual void CreateTextureColor(Texture* &texture, float r, float g, float b, float a) = 0;
			virtual void CreateTextureCubemap(Texture* &texture, int w, int h, Image::Format fmt = Image::Format::kRGB8) = 0;
			virtual void CreateTextureDepth(Texture* &texture, int w, int h, u32 depthSize) = 0;
			virtual void CreateTexture(Texture* &texture, int w, int h, Image::Format fmt) = 0;
			virtual void CreateTextureFromData(Texture* &texture, int w, int h, Image::Format fmt, unsigned char *data) = 0;
			virtual void AddRenderTarget(Texture* &texture, int w, int h, Image::Format fmt, Texture::Filter filt = Texture::Filter::kLinear) = 0;
			virtual void AddRenderDepthStencil(Texture* &texture, int w, int h, u32 depthSize, u32 stencilSize) = 0;
			virtual void DeleteTexture(Texture* texture) = 0;
			virtual void ChangeTexture(Texture* texture, u32 layer = 0) = 0;
			virtual void ChangeRenderTargets(u8 nTargets, Texture* *colorRTs, Texture* depthRT) = 0;
			virtual void ChangeRenderTargetsToCube(u8 nTargets, Texture* *colorRTs, Texture* depthRT, int face) = 0;
			void ChangeRenderTarget(Texture* colorRT, Texture* depthRT);
			virtual void GenerateMipmap(Texture* texture) = 0;
			virtual void CopyToTexture(Texture* texture, u32 layer = 0) = 0;

			// Vertex format functions
			virtual void AddVertexFormat(VertexFormat* &vf, VertexAttribute *attribs, u32 nAttribs) = 0;
			virtual void ChangeVertexFormat(VertexFormat* vf) = 0;
			virtual void DeleteVertexFormat(VertexFormat* vf) = 0;

			// Vertex buffer functions
			virtual void AddVertexBuffer(VertexBuffer* &vb, u32 size, void *data, BufferUsage usage) = 0;
			virtual void DeleteVertexBuffer(VertexBuffer* vb) = 0;

			// Index buffer functions
			virtual void AddIndexBuffer(IndexBuffer* &ib, u32 nIndices, u32 indexSize, void *data, BufferUsage usage) = 0;
			virtual void DeleteIndexBuffer(IndexBuffer* ib) = 0;

			// Shader functions
			virtual bool AddShader(Shader* &shd, const char* filename, const char **attribs = NULL, u32 n_attribs = 0) = 0;
			virtual void DeleteShader(Shader* shd) = 0;

			// Font functions
			virtual void AddFont(Font* &font, const char* fontname) = 0;
			virtual void DeleteFont(Font* font) = 0;

			virtual void ReadPixels(int w, int h, u8 *data) = 0; //!< reads pixels in R8G8B8 format

            virtual void ClearColor(f32 r, f32 g, f32 b, f32 a) = 0;
			virtual void ClearColorBuffer(void) = 0;
			virtual void ClearColorAndDepthBuffers(void) = 0;
			virtual void ClearDepthBuffer(void) = 0;
            
            // Matrices functions
            void SetProjectionMatrix(const sht::math::Matrix4& mat);
            void SetViewMatrix(const sht::math::Matrix4& mat);
            void SetModelMatrix(const sht::math::Matrix4& mat);
            void PushMatrix();
            void PopMatrix();
            
            const sht::math::Matrix4& projection_matrix() const;
            const sht::math::Matrix4& view_matrix() const;
            const sht::math::Matrix4& model_matrix() const;
            
            // Obsolete OpenGL analogs
            void LoadMatrix(const sht::math::Matrix4& matrix);
            void MultMatrix(const sht::math::Matrix4& matrix);
            void Translate(f32 x, f32 y, f32 z);
            void Translate(const sht::math::Vector3& v);
            void Scale(f32 x, f32 y, f32 z);
            void Scale(f32 s);

			virtual void ChangeBlendFunc(u32 source, u32 dest) = 0;
			virtual void EnableBlend(void) = 0;
			virtual void DisableBlend(void) = 0;

			virtual void EnableDepthTest(void) = 0;
			virtual void DisableDepthTest(void) = 0;
			virtual void EnableDepthWrite(void) = 0;
			virtual void DisableDepthWrite(void) = 0;

			virtual void EnableWireframeMode(void) = 0;
			virtual void DisableWireframeMode(void) = 0;

			virtual void Viewport(int w, int h) = 0;

		protected:
			virtual void ApiAddTexture(Texture* &tex, Image &img, Texture::Wrap wrap, Texture::Filter filt) = 0;
			virtual void ApiAddTextureCubemap(Texture* &tex, Image *imgs) = 0;
			virtual void ApiDeleteTexture(Texture* tex) = 0;
            
            Context * context_;

			int width_;										//!< owner app's window width
			int height_;									//!< owner app's window height
			float aspect_ratio_;							//!< w/h
			sht::math::Matrix4 standart_2d_matrix_;			//!< matrix for 2d projection
            
            sht::math::Matrix4 projection_matrix_;          //!< projection matrix
            sht::math::Matrix4 view_matrix_;                //!< view matrix
            sht::math::Matrix4 model_matrix_;               //!< model matrix
            std::stack<sht::math::Matrix4> matrices_stack_; //!< matrices stack

			std::list<Texture*> textures_;
			std::list<Shader*> shaders_;
			std::list<Font*> fonts_;
            std::list<sht::CountingPointer<VertexFormat>> vertex_formats_;
			std::list<VertexBuffer*> vertex_buffers_;
			std::list<IndexBuffer*> index_buffers_;

			// Current identifiers
			Texture* current_textures_[kMaxImageUnit];
			Shader* current_shader_;                //!< obsolete
			VertexFormat* current_vertex_format_;   //!< obsolete
			VertexFormat* active_vertex_format_;    //!< obsolete
			VertexBuffer* current_vertex_buffer_;   //!< obsolete
			IndexBuffer* current_index_buffer_;     //!< obsolete
			Texture* current_color_rt_[kMaxMrt];
			Texture* current_depth_rt_;
		};

	} // namespace graphics
} // namespace sht

#endif