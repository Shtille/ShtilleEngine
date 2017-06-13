#include "../../sht/include/sht.h"

#include "../../sht/system/include/time/time_manager.h"
#include "../../sht/utility/include/resource_manager.h"

#include "game_scene_manager.h"

class BilliardGameApp
	: public sht::OpenGlApplication
{
public:
	BilliardGameApp()
	: scene_manager_(nullptr)
	, need_update_projection_matrix_(true)
	{
	}
	const char* GetTitle() final
	{
		return "Billiard";
	}
	const bool IsMultisample() final
	{
		return false;
	}
	bool Load() final
	{
		sht::system::TimeManager::CreateInstance();
		sht::utility::ResourceManager::CreateInstance();

		// Create scene manager
		scene_manager_ = new GameSceneManager(renderer_);
		
		return true;
	}
	void Unload() final
	{
		if (scene_manager_)
			delete scene_manager_;

		sht::utility::ResourceManager::DestroyInstance();
		sht::system::TimeManager::DestroyInstance();
	}
	void Update() final
	{
		// Update time manager
		sht::system::TimeManager::GetInstance()->Update();

		// Update scene manager
		scene_manager_->Update();

		// Update matrices
		UpdateProjectionMatrix();
	}
	void Render() final
	{
		renderer_->SetViewport(width_, height_);
		
		renderer_->ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		renderer_->ClearColorAndDepthBuffers();

		scene_manager_->Render();
	}
	void OnChar(unsigned short code)
	{
		scene_manager_->OnChar(code);
	}
	void OnKeyDown(sht::PublicKey key, int mods) final
	{
		if (key == sht::PublicKey::kF)
		{
			ToggleFullscreen();
		}
		else if (key == sht::PublicKey::kEscape)
		{
			Application::Terminate();
		}
		else
		{
			scene_manager_->OnKeyDown(key, mods);
		}
	}
	void OnMouseDown(sht::MouseButton button, int modifiers) final
	{
		scene_manager_->OnMouseDown(button, modifiers);
	}
	void OnMouseUp(sht::MouseButton button, int modifiers) final
	{
		scene_manager_->OnMouseUp(button, modifiers);
	}
	void OnMouseMove() final
	{
		scene_manager_->OnMouseMove();
	}
	void OnSize(int w, int h) final
	{
		Application::OnSize(w, h);
		// To have correct perspective when resizing
		need_update_projection_matrix_ = true;
	}
	void UpdateProjectionMatrix()
	{
		// We dont have an animated camera her, thus just use a simple flag
		if (need_update_projection_matrix_)
		{
			need_update_projection_matrix_ = false;
			
			const float znear = 10.0f;
			const float zfar = 10000.0f;
			renderer_->SetProjectionMatrix(sht::math::PerspectiveMatrix(45.0f, width(), height(), znear, zfar));
		}
	}
	
private:
	GameSceneManager * scene_manager_;
	
	bool need_update_projection_matrix_;
};

DECLARE_MAIN(BilliardGameApp);