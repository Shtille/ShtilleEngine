#include "../sht/include/sht.h"
#include "../sht/graphics/include/image/image.h"
#include "../sht/graphics/include/model/cube_model.h"
#include "../sht/graphics/include/model/tetrahedron_model.h"
#include "../sht/graphics/include/model/sphere_model.h"
#include "../sht/graphics/include/renderer/text.h"
#include "../sht/utility/include/console.h"
#include "../sht/utility/include/camera.h"
#include "../sht/geo/include/constants.h"
#include "../sht/geo/include/planet_zoom.h"
#include <cmath>

#ifdef TARGET_MAC // see common/platform.h
#include <mutex>
#endif

namespace {
    const float kCameraDistance = sht::geo::kEarthRadius * 5.0f;
    const sht::math::Vector3 kEarthPosition(0.0f, 0.0f, 0.0f);
    const sht::math::Vector3 kCameraPosition1(kCameraDistance, 0.0f, 0.0f);
    const sht::math::Vector3 kCameraPosition2(-kCameraDistance, 0.0f, 0.0f);
    const sht::math::Vector3 kCameraPosition3(-kCameraDistance, -kCameraDistance, kCameraDistance);
    const sht::math::Vector3 kSunPosition(kCameraDistance, 0.0f, 0.0f);
}

class ShtilleEarthApp : public sht::OpenGlApplication
{
public:
    ShtilleEarthApp()
    : cube_(nullptr)
    , shader_(nullptr)
	, shader2_(nullptr)
	, gui_shader_(nullptr)
	, text_shader_(nullptr)
    , earth_texture_(nullptr)
	, font_(nullptr)
	, fps_text_(nullptr)
	, console_(nullptr)
    , camera_manager_(nullptr)
    , planet_zoom_(nullptr)
    , angle_(0.0f)
    , camera_distance_(kCameraDistance)
    , need_update_projection_matrix_(true)
    , camera_animation_stopped_(false)
    {
    }
    const char* GetTitle(void) final
    {
        return "Shtille Earth";
    }
    bool Load() final
    {
        // First model
        cube_ = new sht::graphics::SphereModel(renderer_);
        cube_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
        cube_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
        cube_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTexcoord, 2));
        cube_->Create();
        if (!cube_->MakeRenderable())
            return false;
        
        const char *attribs[] = {"a_position", "a_normal", "a_texcoord"};
        if (!renderer_->AddShader(shader_, "data/shaders/shader", attribs, 3))
            return false;
        
        if (!renderer_->AddShader(shader2_, "data/shaders/shader2", attribs, 1))
            return false;
        
        if (!renderer_->AddShader(text_shader_, "data/shaders/text_shader", attribs, 1))
            return false;
        
        if (!renderer_->AddShader(gui_shader_, "data/shaders/gui_colored", attribs, 1))
            return false;
        
        if (!renderer_->AddTexture(earth_texture_, "data/textures/earth_texture.jpg"))
            return false;

        renderer_->AddFont(font_, "data/fonts/GoodDog.otf");
		if (font_ == nullptr)
			return false;

		fps_text_ = sht::graphics::DynamicText::Create(renderer_, 30);
        if (!fps_text_)
            return false;
        
        console_ = new sht::utility::Console(renderer_, font_, gui_shader_, text_shader_, 0.7f, 0.1f, 0.8f, aspect_ratio_);

        camera_manager_ = new sht::utility::CameraManager();
//        camera_manager_->MakeFree(vec3(camera_distance_, 0.0f, 0.0f), vec3(0.0f));
//        auto first_camera  = camera_manager_->Add(kCameraPosition1, kEarthPosition);
//        auto second_camera = camera_manager_->Add(kCameraPosition2, kEarthPosition);
//        auto third_camera  = camera_manager_->Add(kCameraPosition3, kEarthPosition);
//        camera_manager_->PathClear();
//        camera_manager_->PathSetStart(first_camera, 5.0f, false);
//        camera_manager_->PathAdd(second_camera, 5.0f, true);
//        camera_manager_->PathAdd(third_camera, 5.0f, false);
//        camera_manager_->PathSetCycling(true);
        
        planet_zoom_ = new sht::geo::PlanetZoom(camera_manager_, sht::geo::kEarthRadius, kCameraDistance, 100.0f);
        
        return true;
    }
    void Unload() final
    {
        if (planet_zoom_)
            delete planet_zoom_;
        if (camera_manager_)
            delete camera_manager_;
		if (console_)
			delete console_;
		if (fps_text_)
			delete fps_text_;
		if (cube_)
			delete cube_;
    }
    void Update() final
    {
//#ifdef TARGET_MAC
//        std::lock_guard<std::mutex> lock(mutex_);
//#endif
        //angle_ += 0.5f * frame_time_;
        rotate_matrix = sht::math::Rotate4(cos(angle_), sin(angle_), 0.0f, 1.0f, 0.0f);
        
        console_->Update(frame_time_);

        if (!camera_animation_stopped_)
            camera_manager_->Update(frame_time_);

        renderer_->SetViewMatrix(camera_manager_->view_matrix());
        
        UpdateProjectionMatrix();
    }
    void Render() final
    {
//#ifdef TARGET_MAC
//        std::lock_guard<std::mutex> lock(mutex_);
//#endif
        vec3 light_pos_eye = renderer_->view_matrix() * kSunPosition;
        
        renderer_->SetViewport(width_, height_);
        
        renderer_->ClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        renderer_->ClearColorAndDepthBuffers();
        
        shader_->Bind();
        shader_->UniformMatrix4fv("u_projection", renderer_->projection_matrix());
        shader_->UniformMatrix4fv("u_view", renderer_->view_matrix());
        shader_->Uniform3fv("u_light_pos", light_pos_eye);
        shader_->Uniform1i("u_texture", 0);
        renderer_->ChangeTexture(earth_texture_);
        
        // Draw first model
        renderer_->PushMatrix();
        renderer_->Translate(kEarthPosition);
        renderer_->Scale(sht::geo::kEarthRadius);
        renderer_->MultMatrix(rotate_matrix);
        shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
        normal_matrix = sht::math::NormalMatrix(renderer_->view_matrix() * renderer_->model_matrix());
        shader_->UniformMatrix3fv("u_normal_matrix", normal_matrix);
        cube_->Render();
        renderer_->PopMatrix();
        
        renderer_->ChangeTexture(nullptr);
        
        renderer_->DisableDepthTest();
        
        // Draw FPS
        text_shader_->Bind();
        text_shader_->Uniform1i("u_texture", 0);
        text_shader_->Uniform4f("u_color", 1.0f, 0.5f, 1.0f, 1.0f);
        //fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"fps: %.2f", frame_rate_);
        fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"CamX: %.7f", camera_manager_->position()->x);
        fps_text_->Render();
        
        // Draw console
        console_->Render();
        
        renderer_->EnableDepthTest();
        
        shader_->Unbind();
    }
    void OnChar(unsigned short code)
    {
//#ifdef TARGET_MAC
//        std::lock_guard<std::mutex> lock(mutex_);
//#endif
        if (console_->IsActive())
        {
            console_->ProcessCharInput(code);
        }
    }
    void OnKeyDown(sht::PublicKey key, int mods) final
    {
//#ifdef TARGET_MAC
//        mutex_.lock();
//#endif
        // Console blocks key input
        if (console_->IsActive())
        {
            console_->ProcessKeyInput(key, mods);
        }
        else // process another input
        {
            if (key == sht::PublicKey::kF)
            {
//#ifdef TARGET_MAC
//                //mutex_.unlock(); // to not get deadlock
//#endif
                ToggleFullscreen();
            }
            else if (key == sht::PublicKey::kEscape)
            {
//#ifdef TARGET_MAC
//                mutex_.unlock(); // to not get deadlock
//#endif
                Application::Terminate();
            }
            else if (key == sht::PublicKey::kEqual)
            {
                planet_zoom_->SmoothZoomIn();
            }
            else if (key == sht::PublicKey::kMinus)
            {
                planet_zoom_->SmoothZoomOut();
            }
            else if ((key == sht::PublicKey::kGraveAccent) && !(mods & sht::ModifierKey::kShift))
            {
                console_->Move();
            }
            else if (key == sht::PublicKey::kSpace)
            {
                camera_animation_stopped_ = !camera_animation_stopped_;
            }
            else if (key == sht::PublicKey::kR)
            {
                bool shift_presseed = (mods & sht::ModifierKey::kShift) == sht::ModifierKey::kShift;
                float angle_x = 0.25f * sht::math::kPi; // rotation by Pi/4
                if (shift_presseed)
                    angle_x = -angle_x; // opposite direction
                planet_zoom_->SmoothRotation(angle_x);
            }
        }
//#ifdef TARGET_MAC
//        mutex_.unlock();
//#endif
    }
    void OnMouseDown(sht::MouseButton button, int modifiers) final
    {
        if (mouse_.button_down(sht::MouseButton::kLeft))
        {
            const sht::math::Vector4& viewport = renderer_->viewport();
            const sht::math::Matrix4& proj = renderer_->projection_matrix();
            const sht::math::Matrix4& view = renderer_->view_matrix();
            planet_zoom_->PanBegin(mouse_.x(), mouse_.y(), viewport, proj, view);
        }
    }
    void OnMouseUp(sht::MouseButton button, int modifiers) final
    {
        if (mouse_.button_down(sht::MouseButton::kLeft))
        {
            planet_zoom_->PanEnd();
        }
    }
    void OnMouseMove() final
    {
//#ifdef TARGET_MAC
//        std::lock_guard<std::mutex> lock(mutex_);
//#endif
        if (mouse_.button_down(sht::MouseButton::kLeft))
        {
            const sht::math::Vector4& viewport = renderer_->viewport();
            const sht::math::Matrix4& proj = renderer_->projection_matrix();
            const sht::math::Matrix4& view = renderer_->view_matrix();
            planet_zoom_->PanMove(mouse_.x(), mouse_.y(), viewport, proj, view);
        }
    }
    void OnSize(int w, int h) final
    {
//#ifdef TARGET_MAC
//        std::lock_guard<std::mutex> lock(mutex_);
//#endif
        Application::OnSize(w, h);
        // To have correct perspective when resizing
        need_update_projection_matrix_ = true;
    }
    void UpdateProjectionMatrix()
    {
        if (need_update_projection_matrix_ || camera_manager_->animated())
        {
            need_update_projection_matrix_ = false;
            
            float znear, zfar;
            planet_zoom_->ObtainZNearZFar(&znear, &zfar);
            renderer_->SetProjectionMatrix(sht::math::PerspectiveMatrix(45.0f, width(), height(), znear, zfar));
        }
    }
    
private:
#ifdef TARGET_MAC
    std::mutex mutex_;
#endif
    sht::graphics::Model * cube_;
    sht::graphics::Shader * shader_;
    sht::graphics::Shader * shader2_;
    sht::graphics::Shader * gui_shader_;
    sht::graphics::Shader * text_shader_;
    sht::graphics::Texture * earth_texture_;
    sht::graphics::Font * font_;
    sht::graphics::DynamicText * fps_text_;
    sht::utility::Console * console_;
    sht::utility::CameraManager * camera_manager_;
    sht::geo::PlanetZoom * planet_zoom_;
    
    sht::math::Matrix4 rotate_matrix;
    sht::math::Matrix3 normal_matrix;
    
    float angle_; //!< rotation angle of earth
    float camera_distance_;
    
    bool need_update_projection_matrix_;
    bool camera_animation_stopped_;
};

DECLARE_MAIN(ShtilleEarthApp);