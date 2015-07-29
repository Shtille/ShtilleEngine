#pragma once
#ifndef __SHT_GRAPHICS_RENDERER_RENDERER_H__
#define __SHT_GRAPHICS_RENDERER_RENDERER_H__

#include "../../../common/types.h"
#include "../../../common/platform.h"
#include "../../../common/counting_pointer.h"
#include "../../../math/sht_math.h"

#include "vertex_format.h"
#include "texture.h"

#include <list>
#include <stack>

namespace sht {
	namespace graphics {

		const int kMaxImageUnit = 16;
		const int kMaxMrt = 4;
        
        enum class BufferUsage : int {
            kStaticDraw,
            kStaticRead,
            kStaticCopy,
            kStreamDraw,
            kStreamRead,
            kStreamCopy,
            kDynamicDraw,
            kDynamicRead,
            kDynamicCopy,
            kCount
        };

		//! Vertex buffer class
		class VertexBuffer {
			friend class Renderer;
			friend class OpenGlRenderer;

		protected:
			VertexBuffer();
			~VertexBuffer();
			VertexBuffer(const VertexBuffer&) = delete;
			void operator = (const VertexBuffer&) = delete;

			u32 GetSize();

		private:
			u32 id_;
			u32 size_;
		};

		//! Index buffer class
		class IndexBuffer {
			friend class Renderer;
			friend class OpenGlRenderer;

		protected:
			IndexBuffer();
			~IndexBuffer();
			IndexBuffer(const IndexBuffer&) = delete;
			void operator = (const IndexBuffer&) = delete;

			u32 GetSize();

		private:
			u32 id_;
			u32 index_count_;
			u32 index_size_;
		};

		//! Shader class
		class Shader {
			friend class Renderer;
			friend class OpenGlRenderer;

		protected:
			Shader();
			~Shader();
			Shader(const Shader&) = delete;
			void operator = (const Shader&) = delete;

		private:
			u32 vertex_;
			u32 fragment_;
			u32 program_;
		};

		//! Font class
		class Font {
			friend class Renderer;
			friend class OpenGlRenderer;

		public:
			f32 GetStrWidth(const char *str) const;

			static const f32 GetBaseSize(); // 0.1f
			static const f32 GetBaseScale(); // 1.6f

		protected:
			Font();
			~Font();
			Font(const Font&) = delete;
			void operator = (const Font&) = delete;

			static const int GetBmpSize();

		private:
#ifdef TARGET_WINDOWS
			ABC Abc[256];
#endif
			u32 base_;
			Texture* texture_;
		};
        
        enum class PrimitiveType {
            kTriangles,
            kTriangleStrip,
            kCount
        };

		//! Base renderer class
		class Renderer {

			friend class OpenGlRenderer;
			friend class DirectXRenderer;
            
            friend class VertexFormat;

		public:
			Renderer(int w, int h);
			virtual ~Renderer();

			void UpdateSizes(int w, int h);

			void Defaults(void);
			void CleanUp(void);
			virtual bool CheckForErrors(void) = 0;
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
			virtual void SetVertexBufferData(ptrdiff_t offset, u32 size, void *data) = 0;
			virtual void* MapVertexBufferData(u32 size) = 0;
			virtual void UnmapVertexBufferData(void) = 0;
			virtual void ChangeVertexBuffer(VertexBuffer* vb) = 0;
			virtual void DeleteVertexBuffer(VertexBuffer* vb) = 0;

			// Index buffer functions
			virtual void AddIndexBuffer(IndexBuffer* &ib, u32 nIndices, u32 indexSize, void *data, BufferUsage usage) = 0;
			virtual void SetIndexBufferData(ptrdiff_t offset, u32 size, void *data) = 0;
			virtual void* MapIndexBufferData(u32 size) = 0;
			virtual void UnmapIndexBufferData(void) = 0;
			virtual void ChangeIndexBuffer(IndexBuffer* ib) = 0;
			virtual void DeleteIndexBuffer(IndexBuffer* ib) = 0;

			// Shader functions
			virtual bool AddShader(Shader* &shd, const char* filename, char **attribs = NULL, int nAttribs = 0) = 0;
			virtual void DeleteShader(Shader* shd) = 0;
			virtual void ChangeShader(Shader* shd) = 0;
			virtual void ChangeShaderAttribBinding(const char *name) = 0;
			virtual void ChangeShaderUniform1i(const char* name, int num) = 0;
			virtual void ChangeShaderUniform1f(const char* name, float x) = 0;
			virtual void ChangeShaderUniform2f(const char* name, float x, float y) = 0;
			virtual void ChangeShaderUniform3f(const char* name, float x, float y, float z) = 0;
			virtual void ChangeShaderUniform4f(const char* name, float x, float y, float z, float w) = 0;
			virtual void ChangeShaderUniform1fv(const char* name, float *v, int n = 1) = 0;
			virtual void ChangeShaderUniform2fv(const char* name, float *v, int n = 1) = 0;
			virtual void ChangeShaderUniform3fv(const char* name, float *v, int n = 1) = 0;
			virtual void ChangeShaderUniform4fv(const char* name, float *v, int n = 1) = 0;
			virtual void ChangeShaderUniformMatrix2fv(const char* name, float *v, bool trans = false, int n = 1) = 0;
			virtual void ChangeShaderUniformMatrix3fv(const char* name, float *v, bool trans = false, int n = 1) = 0;
			virtual void ChangeShaderUniformMatrix4fv(const char* name, float *v, bool trans = false, int n = 1) = 0;

			// Font functions
			virtual void AddFont(Font* &font, const char* fontname, bool bold, bool italic, bool underline, bool strikeout, u32 family = 0) = 0;
			virtual void DeleteFont(Font* font) = 0;
			virtual void ChangeFont(Font* font) = 0;
			virtual void Print(f32 x, f32 y, f32 s, f32 r, f32 g, f32 b, const char *string, ...) = 0;

			virtual f32 GetStringHeight(f32 scale) = 0;
			virtual f32 GetStringWidth(f32 scale, const char *str) = 0;

			virtual void ReadPixels(int w, int h, u8 *data) = 0; //!< reads pixels in R8G8B8 format

            virtual inline void ClearColor(f32 r, f32 g, f32 b, f32 a) = 0;
			virtual inline void ClearColorBuffer(void) = 0;
			virtual inline void ClearColorAndDepthBuffers(void) = 0;
			virtual inline void ClearDepthBuffer(void) = 0;
            
            void SetProjectionMatrix(const sht::math::Matrix4& mat);
            void SetViewMatrix(const sht::math::Matrix4& mat);
            void SetModelMatrix(const sht::math::Matrix4& mat);
            void PushMatrix();
            void PopMatrix();
            
            // Obsolete OpenGL analogs
            void LoadMatrix(const sht::math::Matrix4& matrix);
            void MultMatrix(const sht::math::Matrix4& matrix);
            void Translate(f32 x, f32 y, f32 z);
            void Translate(const sht::math::Vector3& v);
            void Scale(f32 x, f32 y, f32 z);
            void Scale(f32 s);

			virtual inline void ChangeBlendFunc(u32 source, u32 dest) = 0;
			virtual inline void EnableBlend(void) = 0;
			virtual inline void DisableBlend(void) = 0;

			virtual inline void EnableDepthTest(void) = 0;
			virtual inline void DisableDepthTest(void) = 0;
			virtual inline void EnableDepthWrite(void) = 0;
			virtual inline void DisableDepthWrite(void) = 0;

			virtual inline void EnableWireframeMode(void) = 0;
			virtual inline void DisableWireframeMode(void) = 0;

			virtual inline void DrawElements(PrimitiveType mode) = 0;
			virtual inline void DrawElements(u32 mode, u32 numindices) = 0;
			virtual inline void Viewport(int w, int h) = 0;

		protected:
			void ErrorHandler(const char *message);
			virtual void ApiAddTexture(Texture* &tex, Image &img, Texture::Wrap wrap, Texture::Filter filt) = 0;
			virtual void ApiAddTextureCubemap(Texture* &tex, Image *imgs) = 0;
			virtual void ApiDeleteTexture(Texture* tex) = 0;
			virtual void ApiDeleteShader(Shader* shd) = 0;
			virtual void ApiDeleteFont(Font* font) = 0;
			virtual void ApiDeleteVertexFormat(VertexFormat* vf) = 0;
			virtual void ApiDeleteVertexBuffer(VertexBuffer* vb) = 0;
			virtual void ApiDeleteIndexBuffer(IndexBuffer* ib) = 0;

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
			Shader* current_shader_;
			Font* current_font_;
			VertexFormat* current_vertex_format_;
			VertexFormat* active_vertex_format_;
			VertexBuffer* current_vertex_buffer_;
			IndexBuffer* current_index_buffer_;
			Texture* current_color_rt_[kMaxMrt];
			Texture* current_depth_rt_;
		};

	} // namespace graphics
} // namespace sht

#endif