#include "../../../include/renderer/opengl/opengl_renderer.h"
#include "../../../include/renderer/opengl/opengl_texture.h"
#include "../../../include/renderer/opengl/opengl_context.h"
#include "opengl_include.h"
#include "../../../../system/include/stream/file_stream.h"
#include <string>
#include <algorithm>

#ifndef GL_CLAMP
#define GL_CLAMP GL_CLAMP_TO_BORDER
#endif

namespace sht {
	namespace graphics {

		OpenGlRenderer::OpenGlRenderer(int w, int h)
        : Renderer(w, h)
		{
            context_ = new OpenGlContext();
            
			framebuffer_ = 0;
			current_image_unit_ = 0;
			current_render_targets_ = 1;
            
			SetDefaultStates();
		}
		OpenGlRenderer::~OpenGlRenderer()
		{
            delete context_;
            
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

			context_->CheckForErrors();

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

			context_->CheckForErrors();

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

			context_->CheckForErrors();

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

			context_->CheckForErrors();

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

			context_->CheckForErrors();

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

			context_->CheckForErrors();

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
            
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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

			context_->CheckForErrors();

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

			context_->CheckForErrors();

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

			context_->CheckForErrors();

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
#if 0 // deprecated in core
					glDisable(curTex->target_);
#endif
                    glBindTexture(curTex->target_, 0);
				}
				else
				{
#if 0 // deprecated in core
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
#endif
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

			//CheckFrameBufferStatus();
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

			//CheckFrameBufferStatus();
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
            for (auto &p : vertex_formats_)
            {
                auto ptr = p.pointer();
                if (*ptr == *vf)
                {
                    // format exists and a new one is not need
                    delete vf;
                    p.IncreaseCount();
                    vf = ptr;
                    return;
                }
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
            auto it = std::find_if(vertex_formats_.begin(), vertex_formats_.end(), [vf]
                                   (const sht::CountingPointer<VertexFormat>& format)
            {
                return format.pointer() == vf;
            });
            if (it != vertex_formats_.end())
            {
                it->DecreaseCount();
				if (it->count() == 0)
				{
					delete it->pointer();
					vertex_formats_.erase(it);
				}
            }
		}
		void OpenGlRenderer::AddVertexBuffer(VertexBuffer* &vb, u32 size, void *data, BufferUsage usage)
		{
            // Create a vertex buffer and upload the provided data if any
			vb = new VertexBuffer(context_);
			vb->size_ = size;

            vb->Bind();
            vb->SetData(size, data, usage);

			context_->CheckForErrors();

			vertex_buffers_.push_back(vb);
		}
		void OpenGlRenderer::DeleteVertexBuffer(VertexBuffer* vb)
		{
			assert(vb);
			auto it = std::find(vertex_buffers_.begin(), vertex_buffers_.end(), vb);
			if (it != vertex_buffers_.end())
			{
				vertex_buffers_.erase(it);
				delete vb;
			}
		}
		void OpenGlRenderer::AddIndexBuffer(IndexBuffer* &ib, u32 nIndices, u32 indexSize, void *data, BufferUsage usage)
		{
            // Create an index buffer and upload the provided data if any
			ib = new IndexBuffer(context_);
			ib->index_count_ = nIndices;
			ib->index_size_ = indexSize;

			u32 size = nIndices * indexSize;
            ib->Bind();
            ib->SetData(size, data, usage);

			context_->CheckForErrors();

			index_buffers_.push_back(ib);
		}
		void OpenGlRenderer::DeleteIndexBuffer(IndexBuffer* ib)
		{
			assert(ib);
			auto it = std::find(index_buffers_.begin(), index_buffers_.end(), ib);
			if (it != index_buffers_.end())
			{
				index_buffers_.erase(it);
				delete ib;
			}
		}
		bool OpenGlRenderer::AddShader(Shader* &shader, const char* filename, const char **attribs, u32 n_attribs)
		{
            shader = Shader::Create(context_, filename, attribs, n_attribs);
            if (shader == nullptr)
                return false;

			// Done with shader loading
			shaders_.push_back(shader);

			return true;
		}
		void OpenGlRenderer::DeleteShader(Shader* shader)
		{
			assert(shader);
			auto it = std::find(shaders_.begin(), shaders_.end(), shader);
			if (it != shaders_.end())
			{
				shaders_.erase(it);
				delete shader;
			}
		}
		void OpenGlRenderer::AddFont(Font* &font, const char* fontname)
		{
			font = new Font();
            
            const int kFontHeight = 64;
            
            Image image;
            if (font->MakeAtlas(fontname, kFontHeight, &image))
            {
                CreateTextureFromData(font->texture_, image.width(), image.height(), image.format(), image.pixels());
                
                fonts_.push_back(font);
            }
            else
            {
                delete font;
                font = nullptr;
            }
		}
		void OpenGlRenderer::DeleteFont(Font* font)
		{
			assert(font);
			auto it = std::find(fonts_.begin(), fonts_.end(), font);
			if (it != fonts_.end())
			{
				fonts_.erase(it);
				delete font;
			}
		}
		void OpenGlRenderer::ReadPixels(int w, int h, u8 *data)
		{
			glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, data);
		}
        void OpenGlRenderer::ClearColor(f32 r, f32 g, f32 b, f32 a)
        {
            context_->ClearColor(r, g, b, a);
        }
		void OpenGlRenderer::ClearColorBuffer(void)
		{
            context_->ClearColorBuffer();
		}
		void OpenGlRenderer::ClearColorAndDepthBuffers(void)
		{
            context_->ClearColorAndDepthBuffers();
		}
		void OpenGlRenderer::ClearDepthBuffer(void)
		{
            context_->ClearDepthBuffer();
		}
        void OpenGlRenderer::ClearStencil(s32 value)
        {
            context_->ClearStencil(value);
        }
        void OpenGlRenderer::ClearStencilBuffer()
        {
            context_->ClearStencilBuffer();
        }
		void OpenGlRenderer::ChangeBlendFunc(u32 source, u32 dest)
		{
			glBlendFunc(source, dest);
		}
		void OpenGlRenderer::EnableBlend(void)
		{
            context_->EnableBlend();
		}
		void OpenGlRenderer::DisableBlend(void)
		{
            context_->DisableBlend();
		}
		void OpenGlRenderer::EnableDepthTest(void)
		{
            context_->EnableDepthTest();
		}
		void OpenGlRenderer::DisableDepthTest(void)
		{
            context_->DisableDepthTest();
		}
		void OpenGlRenderer::EnableDepthWrite(void)
		{
            context_->EnableDepthWrite();
		}
		void OpenGlRenderer::DisableDepthWrite(void)
		{
            context_->DisableDepthWrite();
		}
        void OpenGlRenderer::EnableStencilTest(void)
        {
            context_->EnableStencilTest();
        }
        void OpenGlRenderer::DisableStencilTest(void)
        {
            context_->DisableStencilTest();
        }
		void OpenGlRenderer::EnableWireframeMode(void)
		{
            context_->EnableWireframeMode();
		}
		void OpenGlRenderer::DisableWireframeMode(void)
		{
            context_->DisableWireframeMode();
		}
		void OpenGlRenderer::ApiViewport(int width, int height)
		{
            context_->Viewport(width, height);
		}

	} // namespace graphics
} // namespace sht