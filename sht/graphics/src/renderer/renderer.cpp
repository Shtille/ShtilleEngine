#include "../../include/renderer/renderer.h"

#include "../../../system/include/filesystem/directory.h"

#include <ctime>
#include <algorithm>

namespace sht {
	namespace graphics {

		Renderer::Renderer(int w, int h)
		{
			UpdateSizes(w, h);
			Setup2DMatrix();
            
            projection_matrix_ = sht::math::Identity4();
            view_matrix_ = sht::math::Identity4();
            model_matrix_ = sht::math::Identity4();
            
			Defaults();
		}
		Renderer::~Renderer()
		{
			// Don't call any functions with virtual table here!
		}
        Context * Renderer::context()
        {
            return context_;
        }
        int Renderer::width()
        {
            return width_;
        }
        int Renderer::height()
        {
            return height_;
        }
        float Renderer::aspect_ratio()
        {
            return aspect_ratio_;
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
				delete obj;
			}
			shaders_.clear();

			// Clean up fonts
			for (auto &obj : fonts_)
			{
				delete obj;
			}
			fonts_.clear();

			// Clean up vertex formats
            for (auto &obj : vertex_formats_)
            {
                delete obj.pointer();
            }
			vertex_formats_.clear();

			// Clean up vertex buffers
			for (auto &obj : vertex_buffers_)
			{
				delete obj;
			}
			vertex_buffers_.clear();

			// Clean up index buffers
			for (auto &obj : index_buffers_)
			{
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
		bool Renderer::TakeScreenshot(const char* directory_name)
		{
			// Fill our path string
			char filename[50];
			time_t now = time(nullptr);
			strftime(filename, _countof(filename), "SS.%Y.%m.%d.%H.%M.%S.jpg", localtime(&now));
			char delimeter[2] = { system::GetPathDelimeter(), '\0' };
			size_t size = strlen(directory_name) + strlen(filename) + 2; // 1 is for delimeter, another 1 is for \0
			char *full_filename = new char[size];
			strcpy(full_filename, directory_name);
			strcat(full_filename, delimeter);
			strcat(full_filename, filename);

			system::CreateDirectory(directory_name); // create directory if it doesn't exist

			Image image;
			// allocate memory and read pixels
			u8 *data = image.Allocate(width_, height_, Image::Format::kRGB8);
			ReadPixels(width_, height_, data);
			if (!image.Save(full_filename))
			{
				delete[] full_filename;
				return false;
			}

			delete[] full_filename;
			return true;
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
		void Renderer::AddTextureFromImage(Texture* &texture, const Image& image, Texture::Wrap wrap, Texture::Filter filt)
		{
			ApiAddTexture(texture, image, wrap, filt);
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
        void Renderer::SetViewport(int w, int h)
        {
            viewport_.Set(0.0f, 0.0f, (float)w, (float)h);
            ApiViewport(w, h);
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
        const sht::math::Vector4& Renderer::viewport() const
        {
            return viewport_;
        }
        const sht::math::Matrix4& Renderer::projection_matrix() const
        {
            return projection_matrix_;
        }
        const sht::math::Matrix4& Renderer::view_matrix() const
        {
            return view_matrix_;
        }
        const sht::math::Matrix4& Renderer::model_matrix() const
        {
            return model_matrix_;
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