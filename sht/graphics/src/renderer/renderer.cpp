#include "../../include/renderer/renderer.h"
#include <ctime>

namespace sht {
	namespace graphics {

		VertexBuffer::VertexBuffer()
		{
		}
		VertexBuffer::~VertexBuffer()
		{
		}
		u32 VertexBuffer::GetSize()
		{
			return size_;
		}
		IndexBuffer::IndexBuffer()
		{
		}
		IndexBuffer::~IndexBuffer()
		{
		}
		u32 IndexBuffer::GetSize()
		{
			return index_count_ * index_size_;
		}
		Shader::Shader()
		{
		}
		Shader::~Shader()
		{
		}
		Font::Font()
		{
		}
		Font::~Font()
		{
		}
		const int Font::GetBmpSize()
		{
			return 1024;
		}
		f32 Font::GetStrWidth(const char *str) const
		{
			int	shift = 0;
#if defined(TARGET_WINDOWS)
            size_t strl = strlen(str);
			for (size_t i = 0; i < strl; ++i)
			{
				shift += Abc[(u8)str[i]].abcA +
					Abc[(u8)str[i]].abcB +
					Abc[(u8)str[i]].abcC;
			}
			if (strl > 0)
			{
				shift -= Abc[(u8)str[0]].abcA;
				shift -= Abc[(u8)str[strl - 1]].abcC;
			}
#else
			//static_assert(false, "How to be with fonts on mac?");
#endif
			return (f32)shift / (f32)Font::GetBmpSize();
		}
		const f32 Font::GetBaseSize()
		{
			return 0.1f;
		}
		const f32 Font::GetBaseScale()
		{
			return 1.6f;
		}

		Renderer::Renderer(int w, int h)
		{
			UpdateSizes(w, h);
			Setup2DMatrix();
			Defaults();
		}
		Renderer::~Renderer()
		{
			CleanUp();
		}
		void Renderer::UpdateSizes(int w, int h)
		{
			width_ = w;
			height_ = h;
			aspect_ratio_ = (float)w / (float)h;
		}
		void Renderer::Defaults(void)
		{
			for (int i = 0; i < kMaxImageUnit; ++i)
				current_textures_[i] = nullptr;
			current_shader_ = nullptr;
			current_font_ = nullptr;
			current_vertex_format_ = nullptr;
			active_vertex_format_ = nullptr;
			current_vertex_buffer_ = nullptr;
			current_index_buffer_ = nullptr;

			for (int i = 0; i < kMaxMrt; ++i)
				current_color_rt_[i] = nullptr;
			current_depth_rt_ = nullptr;
		}
		void Renderer::CleanUp(void)
		{
			// Clean up textures
			for (auto &obj : textures_)
			{
				ApiDeleteTexture(obj);
				delete obj;
			}
			textures_.clear();

			// Clean up shaders
			for (auto &obj : shaders_)
			{
				ApiDeleteShader(obj);
				delete obj;
			}
			shaders_.clear();

			// Clean up fonts
			for (auto &obj : fonts_)
			{
				ApiDeleteFont(obj);
				delete obj;
			}
			fonts_.clear();

			// Clean up vertex formats
			for (auto &obj : vertex_formats_)
			{
				ApiDeleteVertexFormat(obj);
				delete obj;
			}
			vertex_formats_.clear();

			// Clean up vertex buffers
			for (auto &obj : vertex_buffers_)
			{
				ApiDeleteVertexBuffer(obj);
				delete obj;
			}
			vertex_buffers_.clear();

			// Clean up index buffers
			for (auto &obj : index_buffers_)
			{
				ApiDeleteIndexBuffer(obj);
				delete obj;
			}
			index_buffers_.clear();
		}
		void Renderer::CheckForUsing(void)
		{
			assert(current_shader_ == nullptr);
			for (int i = 0; i < kMaxImageUnit; ++i)
				assert(current_textures_[i] == nullptr);
		}
		u32 Renderer::GetUsedVideoMemorySize(void)
		{
			u32 s = GetUsedTexturesSize();
			s += GetUsedVertexBuffersSize();
			s += GetUsedIndexBuffersSize();
			return s;
		}
		u32 Renderer::GetUsedTexturesSize(void)
		{
			u32 s = 0;
			for (auto &t : textures_)
				s += t->GetSize();
			return s;
		}
		u32 Renderer::GetUsedVertexBuffersSize(void)
		{
			u32 s = 0;
			for (auto &vb : vertex_buffers_)
				s += vb->GetSize();
			return s;
		}
		u32 Renderer::GetUsedIndexBuffersSize(void)
		{
			u32 s = 0;
			for (auto &ib : index_buffers_)
				s += ib->GetSize();
			return s;
		}
		void Renderer::TakeScreenshot(int w, int h, const char* dir, bool is_fullscr)
		{
			// fill our path string
			char duration[50];
			time_t now = time(nullptr);
			strftime(duration, _countof(duration), "SS.%Y.%m.%d.%H.%M.%S.jpg", localtime(&now));
			size_t size = strlen(dir) + strlen(duration) + 1;
			char *filename = new char[size];
#ifdef TARGET_WINDOWS
#ifdef _MSC_VER
			strcpy_s(filename, size, dir);
			strcat_s(filename, size, duration);
#else
			strcpy(filename, dir);
			strcat(filename, duration);
#endif

			// TODO: exchange CreateDirectory
			CreateDirectoryA(dir, NULL); // create directory if it doesn't exist
#else
            assert(!"Takescreenshot not implemented for this OS");
#endif

			Image image;
			// allocate memory and read pixels
			u8 *data = image.Allocate(w, h, Image::Format::kRGB8);
			ReadPixels(w, h, data);
			image.Save(filename);

			delete[] filename;
		}
		void Renderer::Setup2DMatrix()
		{
			standart_2d_matrix_ = sht::math::OrthoMatrix(0.0f, aspect_ratio_, 0.0f, 1.0f, -1.0f, 1.0f);
		}
		bool Renderer::AddTexture(Texture* &texture, const char* filename, Texture::Wrap wrap, Texture::Filter filt)
		{
			Image image;
			if (image.LoadFromFile(filename))
				ApiAddTexture(texture, image, wrap, filt);
			return texture != nullptr;
		}
		bool Renderer::AddTextureCubemap(Texture* &texture, const char* filename)
		{
			texture = nullptr;
			bool succeed = true;
			Image *images = new Image[6];
			for (int i = 0; i<6; i++)
				if (!images[i].LoadCubemapFromFile(filename, i))
				{
					succeed = false;
					break;
				}
			if (succeed)
				ApiAddTextureCubemap(texture, images);
			delete[] images;
			return succeed;
		}
		bool Renderer::CreateTextureNormalMapFromHeightMap(Texture* &texture, const char* filename, Texture::Wrap wrap, Texture::Filter filt)
		{
			Image image;
			if (image.LoadNMapFromHMap(filename))
				ApiAddTexture(texture, image, wrap, filt);
			return texture != nullptr;
		}
		bool Renderer::CreateTextureNormalHeightMapFromHeightMap(Texture* &texture, const char* filename, Texture::Wrap wrap, Texture::Filter filt)
		{
			Image image;
			if (image.LoadNHMapFromHMap(filename))
				ApiAddTexture(texture, image, wrap, filt);
			return texture != nullptr;
		}
		void Renderer::ChangeRenderTarget(Texture* colorRT, Texture* depthRT)
		{
			ChangeRenderTargets(1, &colorRT, depthRT);
		}
		void Renderer::ErrorHandler(const char *message)
		{
			assert(!message);
		}
        void Renderer::SetProjectionMatrix(const sht::math::Matrix4& mat)
        {
            projection_matrix_ = mat;
        }
        void Renderer::SetViewMatrix(const sht::math::Matrix4& mat)
        {
            view_matrix_ = mat;
        }
        void Renderer::SetModelMatrix(const sht::math::Matrix4& mat)
        {
            model_matrix_ = mat;
        }
        void Renderer::PushMatrix()
        {
            matrices_stack_.push(model_matrix_);
        }
        void Renderer::PopMatrix()
        {
            assert(!matrices_stack_.empty());
            model_matrix_ = matrices_stack_.top();
            matrices_stack_.pop();
        }
        void Renderer::LoadMatrix(const sht::math::Matrix4& matrix)
        {
            model_matrix_ = matrix;
        }
        void Renderer::MultMatrix(const sht::math::Matrix4& matrix)
        {
            model_matrix_ *= matrix;
        }
        void Renderer::Translate(f32 x, f32 y, f32 z)
        {
            model_matrix_ *= sht::math::Translate(x, y, z);
        }
        void Renderer::Translate(const sht::math::Vector3& v)
        {
            model_matrix_ *= sht::math::Translate(v);
        }
        void Renderer::Scale(f32 x, f32 y, f32 z)
        {
            model_matrix_ *= sht::math::Scale4(x, y, z);
        }
        void Renderer::Scale(f32 s)
        {
            model_matrix_ *= sht::math::Scale4(s);
        }

	} // namespace graphics
} // namespace sht