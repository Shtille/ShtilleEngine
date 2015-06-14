#include "../../../include/renderer/opengl/opengl_renderer.h"
#include "../../../include/renderer/opengl/opengl_texture.h"
#ifdef TARGET_WINDOWS
#include "../../../../thirdparty/glew/include/GL/glew.h"
#elif defined(TARGET_MAC)
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#endif
#include "../../../../system/include/stream/file_stream.h"
#include <string>
#include <algorithm>

#ifndef GL_CLAMP
#define GL_CLAMP GL_CLAMP_TO_BORDER
#endif

namespace sht {
	namespace graphics {

		OpenGlRenderer::OpenGlRenderer(int w, int h) : Renderer(w, h)
		{
			framebuffer_ = 0;
			current_image_unit_ = 0;
			current_render_targets_ = 1;
			SetDefaultStates();
		}
		OpenGlRenderer::~OpenGlRenderer()
		{
			// delete our framebuffer, if it exists
			if (framebuffer_) glDeleteFramebuffers(1, &framebuffer_);
		}
		void OpenGlRenderer::SetDefaultStates()
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClearDepth(1.0f);

			glDepthFunc(GL_LEQUAL);
			glEnable(GL_DEPTH_TEST);

			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
			glEnable(GL_CULL_FACE);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);

			glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		}
		bool OpenGlRenderer::CheckForErrors()
		{
#ifdef _DEBUG
			int error = glGetError();
			if (error != GL_NO_ERROR){
				if (error == GL_INVALID_ENUM)
				{
					ErrorHandler("GL_INVALID_ENUM");
				}
				else if (error == GL_INVALID_VALUE){
					ErrorHandler("GL_INVALID_VALUE");
				}
				else if (error == GL_INVALID_OPERATION){
					ErrorHandler("GL_INVALID_OPERATION");
				}
				else if (error == GL_STACK_OVERFLOW){
					ErrorHandler("GL_STACK_OVERFLOW");
				}
				else if (error == GL_STACK_UNDERFLOW){
					ErrorHandler("GL_STACK_UNDERFLOW");
				}
				else if (error == GL_OUT_OF_MEMORY){
					ErrorHandler("GL_OUT_OF_MEMORY");
				}
				else if (error == GL_INVALID_FRAMEBUFFER_OPERATION_EXT){
					ErrorHandler("GL_INVALID_FRAMEBUFFER_OPERATION_EXT");
				}
				else {
					char str[32];
					sprintf_s(str, "Unknown OpenGL error: %d", error);
					ErrorHandler(str);
				}
				return true;
			}
#endif
			return false;
		}
		bool OpenGlRenderer::CheckFrameBufferStatus()
		{
#ifdef _DEBUG
			GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
			switch (status){
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				return true;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				ErrorHandler("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT");
				return false;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				ErrorHandler("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT");
				return false;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				ErrorHandler("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT");
				return false;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				ErrorHandler("GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT");
				return false;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				ErrorHandler("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT");
				return false;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				ErrorHandler("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT");
				return false;
			case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
				ErrorHandler("GL_FRAMEBUFFER_UNSUPPORTED_EXT");
				return false;
			default:
				ErrorHandler("Unknown FBO error");
				return false;
			}
#else
			return false;
#endif
		}
		bool OpenGlRenderer::CheckFunctionalities()
		{
			// Base engine functionality requirements:
			return true;
		}
		f32 OpenGlRenderer::GetStringHeight(f32 scale)
		{
			return 0.9375f * Font::GetBaseSize() * scale;
		}
		f32 OpenGlRenderer::GetStringWidth(f32 scale, const char *str)
		{
			return current_font_->GetStrWidth(str) * scale * Font::GetBaseScale() * 0.9375f;
		}
		void OpenGlRenderer::ApiAddTexture(Texture* &tex, Image &img, Texture::Wrap wrap, Texture::Filter filt)
		{
			tex = new OpenGlTexture();
			tex->width_ = img.width();
			tex->height_ = img.height();
			tex->format_ = img.format();
			tex->ChooseTarget();

			glGenTextures(1, &tex->texture_id_);
			glBindTexture(tex->target_, tex->texture_id_);

			switch (wrap)
			{
			case Texture::Wrap::kRepeat:
				glTexParameteri(tex->target_, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(tex->target_, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;
			case Texture::Wrap::kClamp:
				glTexParameteri(tex->target_, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(tex->target_, GL_TEXTURE_WRAP_T, GL_CLAMP);
				break;
			case Texture::Wrap::kClampToEdge:
				glTexParameteri(tex->target_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(tex->target_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			}
			float fAnisoMax;
			glTexParameterf(tex->target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			switch (filt)
			{
			case Texture::Filter::kPoint:
				glTexParameterf(tex->target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				break;
			case Texture::Filter::kLinear:
				glTexParameterf(tex->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				break;
			case Texture::Filter::kBilinear:
				glTexParameterf(tex->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
				break;
			case Texture::Filter::kTrilinear:
				glTexParameterf(tex->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				break;
			case Texture::Filter::kBilinearAniso:
				glTexParameterf(tex->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fAnisoMax);
				glTexParameterf(tex->target_, GL_TEXTURE_MAX_ANISOTROPY_EXT, fAnisoMax);
				break;
			case Texture::Filter::kTrilinearAniso:
				glTexParameterf(tex->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fAnisoMax);
				glTexParameterf(tex->target_, GL_TEXTURE_MAX_ANISOTROPY_EXT, fAnisoMax);
				break;
			}

			// create texture
			glTexImage2D(tex->target_, // target
                0, // mipmap
				tex->GetInternalFormat(), // the number of color components
				tex->width_, // texture width
				tex->height_, // texture height
                0,              // border
				tex->GetSrcFormat(), // the format of the pixel data
				tex->GetSrcType(), // the data type of the pixel data
				img.pixels());
            glGenerateMipmap(tex->target_);

			CheckForErrors();

			textures_.push_back(tex);
		}
		void OpenGlRenderer::ApiAddTextureCubemap(Texture* &tex, Image *imgs)
		{
			tex = new OpenGlTexture();
			tex->width_ = imgs[0].width();
			tex->height_ = imgs[0].height();
			tex->format_ = imgs[0].format();
			tex->target_ = GL_TEXTURE_CUBE_MAP;

			glGenTextures(1, &tex->texture_id_);
			glBindTexture(tex->target_, tex->texture_id_);

			glTexParameterf(tex->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(tex->target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(tex->target_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(tex->target_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(tex->target_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			for (u32 t = 0; t < 6; t++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + t, // target
					0, // mipmap level
					tex->GetInternalFormat(), // the number of color components
					tex->width_, // texture width
					tex->height_, // texture height
					0, // border
					tex->GetSrcFormat(), // the format of the pixel data
					tex->GetSrcType(), // the data type of the pixel data
					imgs[t].pixels());
			}

			CheckForErrors();

			textures_.push_back(tex);
		}
		void OpenGlRenderer::ApiDeleteTexture(Texture* tex)
		{
			if (tex->texture_id_)
			{
				glDeleteTextures(1, &tex->texture_id_);
				tex->texture_id_ = 0;
			}
			if (tex->depth_id_)
			{
				glDeleteRenderbuffers(1, &tex->depth_id_);
				tex->depth_id_ = 0;
			}
			if (tex->stencil_id_)
			{
				glDeleteRenderbuffers(1, &tex->stencil_id_);
				tex->stencil_id_ = 0;
			}
		}
		void OpenGlRenderer::ApiDeleteShader(Shader* shd)
		{
			if (shd->program_)
			{
				glDeleteProgram(shd->program_);
				shd->program_ = 0;
			}
		}
		void OpenGlRenderer::ApiDeleteFont(Font* font)
		{
			//glDeleteLists(font->base_, 256);
		}
		void OpenGlRenderer::ApiDeleteVertexFormat(VertexFormat* vf)
		{
			// there is nothing to free
		}
		void OpenGlRenderer::ApiDeleteVertexBuffer(VertexBuffer* vb)
		{
			if (vb->id_)
			{
				glDeleteBuffers(1, &vb->id_);
				vb->id_ = 0;
			}
		}
		void OpenGlRenderer::ApiDeleteIndexBuffer(IndexBuffer* ib)
		{
			if (ib->id_)
			{
				glDeleteBuffers(1, &ib->id_);
				ib->id_ = 0;
			}
		}
		void OpenGlRenderer::CreateTextureColor(Texture* &texture, float r, float g, float b, float a)
		{
			texture = new OpenGlTexture();
			texture->width_ = 1;
			texture->height_ = 1;
			texture->format_ = Image::Format::kRGBA8;
			texture->target_ = GL_TEXTURE_2D;

			glGenTextures(1, &texture->texture_id_);
			glBindTexture(texture->target_, texture->texture_id_);

			glTexParameteri(texture->target_, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(texture->target_, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(texture->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(texture->target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			u8 *data;
			data = new u8[texture->width_ * texture->height_ * 4];
			for (int i = 0; i < texture->width_ * texture->height_ * 4; i += 4)
			{
				data[i] = (u8)(r * 255.0f);
				data[i + 1] = (u8)(g * 255.0f);
				data[i + 2] = (u8)(b * 255.0f);
				data[i + 3] = (u8)(a * 255.0f);
			}

			// create texture
			glTexImage2D(texture->target_, // target
				0, // mipmap level
				texture->GetInternalFormat(), // the number of color components
				texture->width_, // texture width
				texture->height_, // texture height
				0, // border
				texture->GetSrcFormat(), // the format of the pixel data
				texture->GetSrcType(), // the data type of the pixel data
				data);

			delete[] data;

			CheckForErrors();

			textures_.push_back(texture);
		}
		void OpenGlRenderer::CreateTextureCubemap(Texture* &texture, int w, int h, Image::Format fmt)
		{
			texture = new OpenGlTexture();
			texture->width_ = w;
			texture->height_ = h;
			texture->format_ = fmt;
			texture->target_ = GL_TEXTURE_CUBE_MAP;

			glGenTextures(1, &texture->texture_id_);
			glBindTexture(texture->target_, texture->texture_id_);

			glTexParameterf(texture->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(texture->target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(texture->target_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(texture->target_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(texture->target_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			for (u32 t = 0; t < 6; t++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + t, // target
					0, // mipmap level
					texture->GetInternalFormat(), // the number of color components
					texture->width_, // texture width
					texture->height_, // texture height
					0, // border
					texture->GetSrcFormat(), // the format of the pixel data
					texture->GetSrcType(), // the data type of the pixel data
					NULL);
			}

			CheckForErrors();

			textures_.push_back(texture);
		}
		void OpenGlRenderer::CreateTextureDepth(Texture* &texture, int w, int h, u32 depthSize)
		{
			texture = new OpenGlTexture();
			texture->width_ = w;
			texture->height_ = h;
			switch (depthSize)
			{
			case 16: texture->format_ = Image::Format::kDepth16; break;
			case 24: texture->format_ = Image::Format::kDepth24; break;
			case 32: texture->format_ = Image::Format::kDepth32; break;
			default: delete texture; texture = nullptr; return; // unknown format
			}
			texture->target_ = GL_TEXTURE_2D;

			glGenTextures(1, &texture->texture_id_);
			glBindTexture(texture->target_, texture->texture_id_);

			glTexParameterf(texture->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(texture->target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(texture->target_, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(texture->target_, GL_TEXTURE_WRAP_T, GL_CLAMP);

			// create texture
			glTexImage2D(texture->target_, // target
				0, // mipmap level
				texture->GetInternalFormat(), // the number of color components
				texture->width_, // texture width
				texture->height_, // texture height
				0, // border
				texture->GetSrcFormat(), // the format of the pixel data
				texture->GetSrcType(), // the data type of the pixel data
				NULL);

			CheckForErrors();

			textures_.push_back(texture);
		}
		void OpenGlRenderer::CreateTexture(Texture* &texture, int w, int h, Image::Format fmt)
		{
			texture = new OpenGlTexture();
			texture->width_ = w;
			texture->height_ = h;
			texture->format_ = fmt;
			texture->target_ = GL_TEXTURE_2D;

			glGenTextures(1, &texture->texture_id_);
			glBindTexture(texture->target_, texture->texture_id_);

			glTexParameterf(texture->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(texture->target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(texture->target_, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(texture->target_, GL_TEXTURE_WRAP_T, GL_CLAMP);

			// create texture
			glTexImage2D(texture->target_, // target
				0, // mipmap level
				texture->GetInternalFormat(), // the number of color components
				texture->width_, // texture width
				texture->height_, // texture height
				0, // border
				texture->GetSrcFormat(), // the format of the pixel data
				texture->GetSrcType(), // the data type of the pixel data
				NULL);

			CheckForErrors();

			textures_.push_back(texture);
		}
		void OpenGlRenderer::CreateTextureFromData(Texture* &texture, int w, int h, Image::Format fmt, unsigned char *data)
		{
			texture = new OpenGlTexture();
			texture->width_ = w;
			texture->height_ = h;
			texture->format_ = fmt;
			texture->target_ = GL_TEXTURE_2D;

			glGenTextures(1, &texture->texture_id_);
			glBindTexture(texture->target_, texture->texture_id_);

			glTexParameterf(texture->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(texture->target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(texture->target_, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(texture->target_, GL_TEXTURE_WRAP_T, GL_CLAMP);

			// create texture
			glTexImage2D(texture->target_, // target
				0, // mipmap level
				texture->GetInternalFormat(), // the number of color components
				texture->width_, // texture width
				texture->height_, // texture height
				0, // border
				texture->GetSrcFormat(), // the format of the pixel data
				texture->GetSrcType(), // the data type of the pixel data
				data);

			CheckForErrors();

			textures_.push_back(texture);
		}
		void OpenGlRenderer::AddRenderTarget(Texture* &texture, int w, int h, Image::Format fmt, Texture::Filter filt)
		{
			assert(w > 0 && h > 0 && w <= GL_MAX_RENDERBUFFER_SIZE && h <= GL_MAX_RENDERBUFFER_SIZE);

			texture = new OpenGlTexture();
			texture->width_ = w;
			texture->height_ = h;
			texture->format_ = fmt;
			texture->target_ = GL_TEXTURE_2D;

			glGenTextures(1, &texture->texture_id_);
			glBindTexture(texture->target_, texture->texture_id_);

			switch (filt)
			{
			case Texture::Filter::kPoint:
				glTexParameterf(texture->target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameterf(texture->target_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			case Texture::Filter::kLinear:
			default:
				glTexParameterf(texture->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(texture->target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			}

			glTexParameteri(texture->target_, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(texture->target_, GL_TEXTURE_WRAP_T, GL_CLAMP);

			// create texture
			glTexImage2D(texture->target_, // target
				0, // mipmap level
				texture->GetInternalFormat(), // the number of color components
				texture->width_, // texture width
				texture->height_, // texture height
				0, // border
				texture->GetSrcFormat(), // the format of the pixel data
				texture->GetSrcType(), // the data type of the pixel data
				NULL);

			CheckForErrors();

			textures_.push_back(texture);
		}
		void OpenGlRenderer::AddRenderDepthStencil(Texture* &texture, int w, int h, u32 depthSize, u32 stencilSize)
		{
			assert(w > 0 && h > 0 && w <= GL_MAX_RENDERBUFFER_SIZE && h <= GL_MAX_RENDERBUFFER_SIZE && depthSize > 0 && stencilSize > 0);

			texture = new OpenGlTexture();
			texture->width_ = w;
			texture->height_ = h;
			texture->format_ = Image::Format::kDepth24; // by default
			texture->target_ = GL_TEXTURE_2D;

			if (depthSize > 0)
			{
				switch (depthSize)
				{
				case 16:
					texture->format_ = Image::Format::kDepth16;
					break;
				case 24:
					texture->format_ = Image::Format::kDepth24;
					break;
				case 32:
					texture->format_ = Image::Format::kDepth32;
					break;
				}
				// create depth renderbuffer
				glGenRenderbuffers(1, &texture->depth_id_);
				glBindRenderbuffer(GL_RENDERBUFFER, texture->depth_id_);
				glRenderbufferStorage(GL_RENDERBUFFER, texture->GetInternalFormat(), w, h);
			}
			if (stencilSize > 0)
			{
				// create stencil renderbuffer
				glGenRenderbuffers(1, &texture->stencil_id_);
				glBindRenderbuffer(GL_RENDERBUFFER, texture->stencil_id_);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, w, h);
			}
			// Restore current renderbuffer
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			CheckForErrors();

			textures_.push_back(texture);
		}
		void OpenGlRenderer::DeleteTexture(Texture* texture)
		{
			assert(texture);
			ApiDeleteTexture(texture);
			auto it = std::find(textures_.begin(), textures_.end(), texture);
			if (it != textures_.end())
			{
				textures_.erase(it);
				delete texture;
			}
		}
		void OpenGlRenderer::ChangeImageUnit(u32 unit)
		{
			if (unit != current_image_unit_)
			{
				glActiveTexture(GL_TEXTURE0 + unit);
				current_image_unit_ = unit;
			}
		}
		void OpenGlRenderer::ChangeTexture(Texture* texture, u32 layer)
		{
			Texture * curTex = current_textures_[layer];
			if (texture != curTex)
			{
				// change image unit
				ChangeImageUnit(layer);
				if (texture == nullptr)
				{
					// No texture wanted, so just disable the target
					glDisable(curTex->target_);
				}
				else
				{
					if (curTex == nullptr)
					{
						// No texture currently bound, so enable the target
						glEnable(texture->target_);
					}
					else if (texture->target_ != curTex->target_)
					{
						// Change enabled target
						glDisable(curTex->target_);
						glEnable(texture->target_);
					}
					// Bind the texture
					glBindTexture(texture->target_, texture->texture_id_);
				}
				current_textures_[layer] = texture;
			}
		}
		void OpenGlRenderer::ChangeRenderTargets(u8 nTargets, Texture* *colorRTs, Texture* depthRT)
		{
			if (nTargets == 1 && colorRTs[0] == nullptr && depthRT == nullptr)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(0, 0, width_, height_);
			}
			else
			{
				if (framebuffer_ == 0) glGenFramebuffers(1, &framebuffer_);
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

				if (depthRT != current_depth_rt_)
				{
					if (depthRT == nullptr)
					{
						// detach depth renderbuffer
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
					}
					else
					{
						if (depthRT->texture_id_) // is depth renderbuffer attached
							glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthRT->target_, depthRT->texture_id_, 0);
						else // attach depth renderbuffer
							glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRT->depth_id_);
					}
					current_depth_rt_ = depthRT;
				}

				bool colorLess = (depthRT != nullptr && depthRT->texture_id_);

				if (colorLess)
				{
					glDrawBuffer(GL_NONE);
					glReadBuffer(GL_NONE);
				}
				else
				{
					GLuint drawBuffers[kMaxMrt];
					for (u8 i = 0; i < nTargets; i++)
					{
						if (colorRTs[i] != current_color_rt_[i])
						{
							glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
								GL_TEXTURE_2D, colorRTs[i]->texture_id_, 0);

							current_color_rt_[i] = colorRTs[i];
						}

						drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
					}

					if (nTargets != current_render_targets_)
					{
						glDrawBuffers(current_render_targets_ = nTargets, drawBuffers);
					}
					//glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
				}

				Texture * tex = (colorRTs[0] != nullptr) ? colorRTs[0] : depthRT;
				glViewport(0, 0, tex->width_, tex->height_);
			}

			CheckFrameBufferStatus();
		}
		void OpenGlRenderer::ChangeRenderTargetsToCube(u8 nTargets, Texture* *colorRTs, Texture* depthRT, int face)
		{
			if (nTargets == 1 && colorRTs[0] == nullptr && depthRT == nullptr)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(0, 0, width_, height_);
			}
			else
			{
				if (framebuffer_ == 0) glGenFramebuffers(1, &framebuffer_);
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

				if (depthRT != current_depth_rt_)
				{
					if (depthRT == nullptr)
					{
						// detach depth renderbuffer
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
					}
					else
					{
						if (depthRT->texture_id_) // is depth renderbuffer attached
							glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthRT->target_, depthRT->texture_id_, 0);
						else // attach depth renderbuffer
							glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRT->depth_id_);
					}
					current_depth_rt_ = depthRT;
				}

				bool colorLess = (depthRT != nullptr && depthRT->texture_id_);

				if (colorLess)
				{
					glDrawBuffer(GL_NONE);
					glReadBuffer(GL_NONE);
				}
				else
				{
					GLuint drawBuffers[kMaxMrt];
					for (u8 i = 0; i < nTargets; i++)
					{
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
							GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, colorRTs[i]->texture_id_, 0);

						current_color_rt_[i] = colorRTs[i];

						drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
					}

					if (nTargets != current_render_targets_)
					{
						glDrawBuffers(current_render_targets_ = nTargets, drawBuffers);
					}
					//glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
				}

				Texture * tex = (colorRTs[0] != nullptr) ? colorRTs[0] : depthRT;
				glViewport(0, 0, tex->width_, tex->height_);
			}

			CheckFrameBufferStatus();
		}
		void OpenGlRenderer::GenerateMipmap(Texture* texture)
		{
			Texture * curtex = current_textures_[current_image_unit_];
			if (texture != curtex && curtex != nullptr)
			{
				glBindTexture(texture->target_, texture->texture_id_);
				glGenerateMipmap(texture->target_);
				glBindTexture(curtex->target_, curtex->texture_id_);
			}
			else
				glGenerateMipmap(texture->target_);
		}
		void OpenGlRenderer::CopyToTexture(Texture* texture, u32 layer)
		{
			ChangeTexture(texture, layer);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, texture->width_, texture->height_);
		}
		void OpenGlRenderer::AddVertexFormat(VertexFormat* &vf, VertexAttribute *attribs, u32 nAttribs)
		{
			vf = new VertexFormat();

			vf->Fill(attribs, nAttribs);

			// Try to find same format
			for (auto f : vertex_formats_)
				if (*f == *vf)
				{
					vf = f; // format exists and a new one is not need
					return;
				}

			// format doesn't exist
			vertex_formats_.push_back(vf);
		}
		void OpenGlRenderer::ChangeVertexFormat(VertexFormat* vf)
		{
			if (vf != current_vertex_format_)
			{
				static VertexFormat zero;
				VertexFormat *curr = &zero, *sel = &zero;

				if (current_vertex_format_ != nullptr) curr = current_vertex_format_;
				if (vf != nullptr) sel = vf;

				for (int i = 0; i < kMaxGeneric; ++i)
				{
					if ( sel->generic_[i].size && !curr->generic_[i].size) glEnableVertexAttribArray(i);
					if (!sel->generic_[i].size &&  curr->generic_[i].size) glDisableVertexAttribArray(i);
				}

				current_vertex_format_ = vf;
			}
		}
		void OpenGlRenderer::DeleteVertexFormat(VertexFormat* vf)
		{
			assert(vf);
			ApiDeleteVertexFormat(vf);
			auto it = std::find(vertex_formats_.begin(), vertex_formats_.end(), vf);
			if (it != vertex_formats_.end())
			{
				vertex_formats_.erase(it);
				delete vf;
			}
		}
		void OpenGlRenderer::AddVertexBuffer(VertexBuffer* &vb, u32 size, void *data, u32 flags)
		{
			vb = new VertexBuffer();
			vb->size_ = size;

			// Create a vertex buffer and upload the provided data if any
			glGenBuffers(1, &vb->id_);
			glBindBuffer(GL_ARRAY_BUFFER, vb->id_);
			glBufferData(GL_ARRAY_BUFFER, size, data, flags);

			CheckForErrors();

			vertex_buffers_.push_back(vb);
		}
		void OpenGlRenderer::SetVertexBufferData(ptrdiff_t offset, u32 size, void *data)
		{
			glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		}
		void* OpenGlRenderer::MapVertexBufferData(u32 size)
		{
			glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STREAM_DRAW);
			return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		}
		void OpenGlRenderer::UnmapVertexBufferData(void)
		{
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		void OpenGlRenderer::ChangeVertexBuffer(VertexBuffer* vb)
		{
			// Change bound VBO if neccesary
			if (vb != current_vertex_buffer_)
			{
				if (vb == nullptr)
					glBindBuffer(GL_ARRAY_BUFFER, 0);
				else
					glBindBuffer(GL_ARRAY_BUFFER, vb->id_);
			}

			/*
			If either current VBO, offset into VBO or the vertex format has changed we need to update all pointers.
			Since OpenGL doesn't have a concept of "streams" in the sense that Direct3D does, but instead has a
			set of independent array pointers, we only update the arrays that the current vertex format indicates
			belongs to this stream.
			*/
			if (vb != current_vertex_buffer_ || current_vertex_format_ != active_vertex_format_)
			{
				if (current_vertex_format_ != nullptr)
				{
					u8* base = (u8*)0;
					VertexFormat *cvf = current_vertex_format_;
					int vertexSize = cvf->vertex_size_;

					for (int i = 0; i < kMaxGeneric; ++i)
						if (cvf->generic_[i].size)
							glVertexAttribPointer(i, cvf->generic_[i].size, GL_FLOAT, GL_FALSE, vertexSize, base + cvf->generic_[i].offset);
				}

				current_vertex_buffer_ = vb;
				active_vertex_format_ = current_vertex_format_;
			}
		}
		void OpenGlRenderer::DeleteVertexBuffer(VertexBuffer* vb)
		{
			assert(vb);
			ApiDeleteVertexBuffer(vb);
			auto it = std::find(vertex_buffers_.begin(), vertex_buffers_.end(), vb);
			if (it != vertex_buffers_.end())
			{
				vertex_buffers_.erase(it);
				delete vb;
			}
		}
		void OpenGlRenderer::AddIndexBuffer(IndexBuffer* &ib, u32 nIndices, u32 indexSize, void *data, u32 flags)
		{
			ib = new IndexBuffer();
			ib->index_count_ = nIndices;
			ib->index_size_ = indexSize;

			GLsizeiptr size = nIndices * indexSize;

			// Create an index buffer and upload the provided data if any
			glGenBuffers(1, &ib->id_);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id_);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, flags);

			CheckForErrors();

			index_buffers_.push_back(ib);
		}
		void OpenGlRenderer::SetIndexBufferData(ptrdiff_t offset, u32 size, void *data)
		{
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
		}
		void* OpenGlRenderer::MapIndexBufferData(u32 size)
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, GL_STREAM_DRAW);
			return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
		}
		void OpenGlRenderer::UnmapIndexBufferData(void)
		{
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		}
		void OpenGlRenderer::ChangeIndexBuffer(IndexBuffer* ib)
		{
			if (ib != current_index_buffer_)
			{
				if (ib == nullptr)
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				else
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id_);

				current_index_buffer_ = ib;
			}
		}
		void OpenGlRenderer::DeleteIndexBuffer(IndexBuffer* ib)
		{
			assert(ib);
			ApiDeleteIndexBuffer(ib);
			auto it = std::find(index_buffers_.begin(), index_buffers_.end(), ib);
			if (it != index_buffers_.end())
			{
				index_buffers_.erase(it);
				delete ib;
			}
		}
		bool OpenGlRenderer::AddShader(Shader* &shader, const char* filename, char **attribs, int nAttribs)
		{
			shader = new Shader();

			GLint success;
			system::FileStream stream;
			std::string shader_source;
			std::string shader_filename;

			// Vertex program
			shader_filename = filename;
			shader_filename += ".vs";
			if (stream.Open(shader_filename.c_str(), system::StreamAccess::kReadText))
			{
				shader_source.resize(stream.Length());
				if (!stream.Read(&shader_source[0], shader_source.size()))
				{
					ErrorHandler("Failed to read from vertex shader file");
					delete shader;
					return false;
				}
				stream.Close();

				shader->vertex_ = glCreateShader(GL_VERTEX_SHADER);
				const char * source = shader_source.c_str();
				glShaderSource(shader->vertex_, 1, &source, NULL);
				glCompileShader(shader->vertex_);
				glGetShaderiv(shader->vertex_, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					char infoLog[2048];
					glGetShaderInfoLog(shader->vertex_, 2048, NULL, infoLog);
					char temp[100];
					sprintf(temp, "%s %s", "Error in vertex shader compilation in", filename);
					ErrorHandler(temp);
					ErrorHandler(infoLog);
					glDeleteShader(shader->vertex_);
					delete shader;
					return false;
				}
				CheckForErrors();
			}

			// Fragment program
			shader_filename = filename;
			shader_filename += ".fs";
			if (stream.Open(shader_filename.c_str(), system::StreamAccess::kReadText))
			{
				shader_source.resize(stream.Length());
				if (!stream.Read(&shader_source[0], shader_source.size()))
				{
					ErrorHandler("Failed to read from fragment shader file");
					delete shader;
					return false;
				}
				stream.Close();

				shader->fragment_ = glCreateShader(GL_FRAGMENT_SHADER);
				const char * source = shader_source.c_str();
				glShaderSource(shader->fragment_, 1, &source, NULL);
				glCompileShader(shader->fragment_);
				glGetShaderiv(shader->fragment_, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					char infoLog[2048];
					glGetShaderInfoLog(shader->fragment_, 2048, NULL, infoLog);
					char temp[100];
					sprintf(temp, "%s %s", "Error in fragment shader compilation in", filename);
					ErrorHandler(temp);
					ErrorHandler(infoLog);
					glDeleteShader(shader->fragment_);
					glDeleteShader(shader->vertex_);
					delete shader;
					return false;
				}
				CheckForErrors();
			}

			// Create program object, attach shader, then link
			shader->program_ = glCreateProgram();
			glAttachShader(shader->program_, shader->vertex_);
			glAttachShader(shader->program_, shader->fragment_);
			for (u32 i = 0; i < (u32)nAttribs; ++i)
			{
				if (attribs[i])
				{
					glBindAttribLocation(shader->program_, i, attribs[i]);
				}
			}
			glLinkProgram(shader->program_);
			glGetProgramiv(shader->program_, GL_LINK_STATUS, &success);
			if (!success)
			{
				char infoLog[2048];
				glGetProgramInfoLog(shader->program_, 2048, NULL, infoLog);
				char temp[100];
				sprintf(temp, "%s %s", "Error in shader linkage in", filename);
				ErrorHandler(temp);
				ErrorHandler(infoLog);
				glDeleteProgram(shader->program_);
				glDeleteShader(shader->vertex_);
				glDeleteShader(shader->fragment_);
				delete shader;
				return false;
			}
			CheckForErrors();

			// After linkage these objects may be deleted
			glDeleteShader(shader->vertex_);
			glDeleteShader(shader->fragment_);

			// Done with shader loading
			shaders_.push_back(shader);

			return true;
		}
		void OpenGlRenderer::DeleteShader(Shader* shader)
		{
			assert(shader);
			ApiDeleteShader(shader);
			auto it = std::find(shaders_.begin(), shaders_.end(), shader);
			if (it != shaders_.end())
			{
				shaders_.erase(it);
				delete shader;
			}
		}
		void OpenGlRenderer::ChangeShader(Shader* shader)
		{
			if (shader != nullptr)
				glUseProgram(shader->program_);
			else
				glUseProgram(0);
			current_shader_ = shader;
		}
		void OpenGlRenderer::ChangeShaderAttribBinding(const char *name)
		{
			int ind = glGetAttribLocation(current_shader_->program_, name);
			glBindAttribLocation(current_shader_->program_, ind, name);
		}
		void OpenGlRenderer::ChangeShaderUniform1i(const char* name, int num)
		{
			int location = glGetUniformLocation(current_shader_->program_, name);
			assert(location != -1);
			glUniform1i(location, num);
		}
		void OpenGlRenderer::ChangeShaderUniform1f(const char* name, float x)
		{
			int location = glGetUniformLocation(current_shader_->program_, name);
			assert(location != -1);
			glUniform1f(location, x);
		}
		void OpenGlRenderer::ChangeShaderUniform2f(const char* name, float x, float y)
		{
			int location = glGetUniformLocation(current_shader_->program_, name);
			assert(location != -1);
			glUniform2f(location, x, y);
		}
		void OpenGlRenderer::ChangeShaderUniform3f(const char* name, float x, float y, float z)
		{
			int location = glGetUniformLocation(current_shader_->program_, name);
			assert(location != -1);
			glUniform3f(location, x, y, z);
		}
		void OpenGlRenderer::ChangeShaderUniform4f(const char* name, float x, float y, float z, float w)
		{
			int location = glGetUniformLocation(current_shader_->program_, name);
			assert(location != -1);
			glUniform4f(location, x, y, z, w);
		}
		void OpenGlRenderer::ChangeShaderUniform1fv(const char* name, float *v, int n)
		{
			int location = glGetUniformLocation(current_shader_->program_, name);
			assert(location != -1);
			glUniform1fv(location, n, v);
		}
		void OpenGlRenderer::ChangeShaderUniform2fv(const char* name, float *v, int n)
		{
			int location = glGetUniformLocation(current_shader_->program_, name);
			assert(location != -1);
			glUniform2fv(location, n, v);
		}
		void OpenGlRenderer::ChangeShaderUniform3fv(const char* name, float *v, int n)
		{
			int location = glGetUniformLocation(current_shader_->program_, name);
			assert(location != -1);
			glUniform3fv(location, n, v);
		}
		void OpenGlRenderer::ChangeShaderUniform4fv(const char* name, float *v, int n)
		{
			int location = glGetUniformLocation(current_shader_->program_, name);
			assert(location != -1);
			glUniform4fv(location, n, v);
		}
		void OpenGlRenderer::ChangeShaderUniformMatrix2fv(const char* name, float *v, bool trans, int n)
		{
			int location = glGetUniformLocation(current_shader_->program_, name);
			assert(location != -1);
			glUniformMatrix2fv(location, n, trans, v);
		}
		void OpenGlRenderer::ChangeShaderUniformMatrix3fv(const char* name, float *v, bool trans, int n)
		{
			int location = glGetUniformLocation(current_shader_->program_, name);
			assert(location != -1);
			glUniformMatrix3fv(location, n, trans, v);
		}
		void OpenGlRenderer::ChangeShaderUniformMatrix4fv(const char* name, float *v, bool trans, int n)
		{
			int location = glGetUniformLocation(current_shader_->program_, name);
			assert(location != -1);
			glUniformMatrix4fv(location, n, trans, v);
		}
		void OpenGlRenderer::AddFont(Font* &font, const char* fontname, bool bold, bool italic, bool underline, bool strikeout, u32 family)
		{
			font = new Font();

#if defined(TARGET_WINDOWS)
			const int nBmpSize = Font::GetBmpSize();
			const int nCharSize = nBmpSize / 16;

			// Read preferences
			int iWidth = (bold) ? FW_BOLD : FW_DONTCARE;
			DWORD dwItalic = (italic) ? TRUE : FALSE;
			DWORD dwUnderline = (underline) ? TRUE : FALSE;
			DWORD dwStrikeOut = (strikeout) ? TRUE : FALSE;
			DWORD dwFamily = family;

			// Create desired font
			HFONT hFont;
			hFont = CreateFontA(nCharSize,					// The height, in logical units, of the font's character cell or character. 
				0,							// The average width, in logical units, of characters in the requested font.
				0,							// The angle, in tenths of degrees, between the escapement vector and the x-axis of the device.
				0,							// The angle, in tenths of degrees, between each character's base line and the x-axis of the device.
				iWidth,						// The weight of the font in the range 0 through 1000
				dwItalic,					// Specifies an italic font if set to TRUE.
				dwUnderline,					// Specifies an underlined font if set to TRUE.
				dwStrikeOut,					// A strikeout font if set to TRUE.
				RUSSIAN_CHARSET,				// The character set.
				OUT_OUTLINE_PRECIS,			// The output precision.
				CLIP_DEFAULT_PRECIS,			// The clipping precision.
				ANTIALIASED_QUALITY,			// The output quality. 
				VARIABLE_PITCH | dwFamily,	// The pitch and family of the font.
				fontname);					// A pointer to a null-terminated string that specifies the typeface name of the font.
			assert(hFont);
			if (!hFont)
			{
				delete font;
				return;
			}

			// Create bitmap
			HDC hdcWindow = GetDC(NULL);
			HDC hdcBitmap = CreateCompatibleDC(hdcWindow);
			HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, nBmpSize, nBmpSize);

			// Select objects
			SelectObject(hdcBitmap, hBitmap);
			SelectObject(hdcBitmap, hFont);

			// Select colors
			SetBkColor(hdcBitmap, 0);
			SetTextColor(hdcBitmap, ((COLORREF)(((BYTE)(255) | ((WORD)((BYTE)(255)) << 8)) | (((DWORD)(BYTE)(255)) << 16))));

			// Fill out our bitmap with symbols
			if (GetCharABCWidths(hdcBitmap, 0, 255, font->Abc))
			{
				int i, x, y;
				char temp[2] = {};
				// We now have an array of ABC structures
				for (i = 0; i < 256; ++i)
				{
					x = (i % 16) * nCharSize;
					y = (i / 16) * nCharSize;
					temp[0] = i;
					TextOutA(hdcBitmap, x, y, temp, 1);
				}
			}

			// Get the BITMAP from the HBITMAP
			BITMAP bitmap;
			GetObject(hBitmap, sizeof(BITMAP), &bitmap);

			BITMAPINFOHEADER   bi;
			bi.biSize = sizeof(BITMAPINFOHEADER);
			bi.biWidth = bitmap.bmWidth;
			bi.biHeight = bitmap.bmHeight;
			bi.biPlanes = 1;
			bi.biBitCount = 32;
			bi.biCompression = BI_RGB;
			bi.biSizeImage = 0;
			bi.biXPelsPerMeter = 0;
			bi.biYPelsPerMeter = 0;
			bi.biClrUsed = 0;
			bi.biClrImportant = 0;

			assert(bi.biBitCount == 32);

			u8 *lpbitmap = new u8[bi.biWidth * bi.biHeight * (bi.biBitCount / 8)];

			// Get bits from source image, but notice what all alpha bits are set to 0
			GetDIBits(hdcBitmap, hBitmap, 0, (UINT)bitmap.bmHeight, lpbitmap, (BITMAPINFO *)&bi, DIB_RGB_COLORS);

			// Make pixels with text data non-transparent
			for (int i = 0; i < bi.biWidth * bi.biHeight * 4; i += 4)
				if (lpbitmap[i] > 0)
					lpbitmap[i + 3] = 255;

			CreateTextureFromData(font->texture_, (int)bi.biWidth, (int)bi.biHeight, graphics::Image::Format::kRGBA8, lpbitmap);

			delete[] lpbitmap;

			assert(font->texture_);

			// Scale factor to scale new size to old size (supposed to be 1.6f)
			const f32 scale = 1.6f;

			// Finally create display lists
			font->base_ = glGenLists(256);
			ChangeTexture(font->texture_);
			for (s32 i = 0; i < 16; i++)
			{
				for (s32 j = 0; j < 16; j++)
				{
					// standart sizes and offsets in pixels
					s32 sym = i * 16 + j;
					f32 offA = (f32)font->Abc[sym].abcA / (f32)nBmpSize; // 0 to 1
					f32 offB = (f32)font->Abc[sym].abcB / (f32)nBmpSize; // 0 to 1
					f32 offC = (f32)font->Abc[sym].abcC / (f32)nBmpSize; // 0 to 1
					glTranslatef(offA * scale, 0.0f, 0.0f);
					glNewList(font->base_ + sym, GL_COMPILE);
					glBegin(GL_QUADS);
					glTexCoord2f(j*0.0625f + offA, 1.0f - i*0.0625f);
					glVertex2f(0.0f, 0.0625f * scale);
					glTexCoord2f(j*0.0625f + offA + offB, 1.0f - i*0.0625f);
					glVertex2f(offB * scale, 0.0625f * scale);
					glTexCoord2f(j*0.0625f + offA + offB, 1.0f - (i*0.0625f + 0.0625f));
					glVertex2f(offB * scale, 0.0f);
					glTexCoord2f(j*0.0625f + offA, 1.0f - (i*0.0625f + 0.0625f));
					glVertex2f(0.0f, 0.0f);
					glEnd();
					glTranslatef((offB + offC) * scale, 0.0f, 0.0f);
					glEndList();
				}
			}

			// Clean up all allocated data
			DeleteObject(hBitmap);
			DeleteObject(hdcBitmap);
			ReleaseDC(NULL, hdcWindow);

			DeleteObject(hFont);
#elif defined(TARGET_LINUX)
			static_assert(false, "Font creation has not been defined");
#elif defined(TARGET_MAC)
			//static_assert(false, "Font creation has not been defined");
#endif

			CheckForErrors();

			fonts_.push_back(font);
		}
		void OpenGlRenderer::DeleteFont(Font* font)
		{
			assert(font);
			ApiDeleteFont(font);
			auto it = std::find(fonts_.begin(), fonts_.end(), font);
			if (it != fonts_.end())
			{
				fonts_.erase(it);
				delete font;
			}
		}
		void OpenGlRenderer::ChangeFont(Font* font)
		{
			current_font_ = font;
		}
		void OpenGlRenderer::Print(f32 x, f32 y, f32 s, f32 r, f32 g, f32 b, const char *string, ...)
		{
			assert(current_font_);
			assert(string);

			char		text[256];		// Holds Our String
			va_list		ap;				// Pointer To List Of Arguments

			va_start(ap, string);				// Parses The String For Variables
            vsprintf(text, string, ap);		    // And Converts Symbols To Actual Numbers
			va_end(ap);							// Results Are Stored In Text

//			glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT);
//			ChangeTexture(current_font_->texture_);
//			glPushMatrix();
//			glColor3f(r, g, b);
//			glTranslatef(x, y, 0.0f);
//			glScalef(s, s, 1.0f);
//			glListBase(current_font_->base_);
//			glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
//			glPopMatrix();
//			glPopAttrib();
		}
		void OpenGlRenderer::ReadPixels(int w, int h, u8 *data)
		{
			glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, data);
		}
		inline void OpenGlRenderer::ClearColor(void)
		{
			glClear(GL_COLOR_BUFFER_BIT);
		}
		inline void OpenGlRenderer::ClearColorDepth(void)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		inline void OpenGlRenderer::ClearDepth(void)
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}
		inline void OpenGlRenderer::ChangeBlendFunc(u32 source, u32 dest)
		{
			glBlendFunc(source, dest);
		}
		inline void OpenGlRenderer::EnableBlend(void)
		{
			glEnable(GL_BLEND);
		}
		inline void OpenGlRenderer::DisableBlend(void)
		{
			glDisable(GL_BLEND);
		}
		inline void OpenGlRenderer::EnableDepthTest(void)
		{
			glEnable(GL_DEPTH_TEST);
		}
		inline void OpenGlRenderer::DisableDepthTest(void)
		{
			glDisable(GL_DEPTH_TEST);
		}
		inline void OpenGlRenderer::EnableDepthWrite(void)
		{
			glDepthMask(GL_TRUE);
		}
		inline void OpenGlRenderer::DisableDepthWrite(void)
		{
			glDepthMask(GL_FALSE);
		}
		inline void OpenGlRenderer::EnableWireframeMode(void)
		{
			glPolygonMode(GL_FRONT, GL_LINE);
		}
		inline void OpenGlRenderer::DisableWireframeMode(void)
		{
			glPolygonMode(GL_FRONT, GL_FILL);
		}
		inline void OpenGlRenderer::DrawElements(u32 mode)
		{
			GLenum index_type = (current_index_buffer_->index_size_ == 2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
			glDrawElements(mode, current_index_buffer_->index_count_, index_type, 0);
		}
		inline void OpenGlRenderer::DrawElements(u32 mode, u32 numindices)
		{
			GLenum index_type = (current_index_buffer_->index_size_ == 2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
			glDrawElements(mode, numindices, index_type, 0);
		}
		inline void OpenGlRenderer::Viewport(int w, int h)
		{
			glViewport(0, 0, w, h);
		}

	} // namespace graphics
} // namespace sht