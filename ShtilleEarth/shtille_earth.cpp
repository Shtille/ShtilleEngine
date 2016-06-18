#include "../sht/include/sht.h"
#include "../sht/graphics/include/model/sphere_model.h"
#include "../sht/graphics/include/renderer/text.h"
#include "../sht/utility/include/console.h"
#include "../sht/utility/include/camera.h"
#include "../sht/geo/include/constants.h"
#include "../sht/geo/include/planet_navigation.h"
#include <cmath>

#ifdef TARGET_MAC // see common/platform.h
#include <mutex>
#endif

namespace {
    const float kCameraDistance = sht::geo::kEarthRadius * 5.0f;
	const float kInnerRadius = sht::geo::kEarthRadius;
	const float kOuterRadius = sht::geo::kEarthRadius + sht::geo::kEarthAtmosphereHeight;
    const sht::math::Vector3 kEarthPosition(0.0f, 0.0f, 0.0f);
    const sht::math::Vector3 kSunPosition(kCameraDistance, 0.0f, 0.0f);
}

class ShtilleEarthApp : public sht::OpenGlApplication
{
public:
    ShtilleEarthApp()
    : sphere_(nullptr)
	, font_(nullptr)
	, fps_text_(nullptr)
	, console_(nullptr)
    , camera_manager_(nullptr)
    , planet_navigation_(nullptr)
    , angle_(0.0f)
    , need_update_projection_matrix_(true)
    , camera_animation_stopped_(false)
    {
    }
    const char* GetTitle() final
    {
        return "Shtille Earth";
    }
	void BindShaderConstants()
	{
		float Kr = 0.0030f;
		float Km = 0.0015f;
		float ESun = 16.0f;
		float g = -0.75f;
		float Scale = 1.0f / (kOuterRadius - kInnerRadius);
		float ScaleDepth = 0.25f;
		float ScaleOverScaleDepth = Scale / ScaleDepth;

        ground_shader_->Bind();
        ground_shader_->Uniform3f("v3LightPos", 0.0f, 0.0f, 1.0f);
        ground_shader_->Uniform3f("v3InvWavelength", 1.0f / powf(0.650f, 4.0f), 1.0f / powf(0.570f, 4.0f), 1.0f / powf(0.475f, 4.0f));
        ground_shader_->Uniform1f("fInnerRadius", kInnerRadius);
        ground_shader_->Uniform1f("fOuterRadius", kOuterRadius);
        ground_shader_->Uniform1f("fOuterRadius2", kOuterRadius * kOuterRadius);
        ground_shader_->Uniform1f("fKrESun", Kr * ESun);
        ground_shader_->Uniform1f("fKmESun", Km * ESun);
        ground_shader_->Uniform1f("fKr4PI", Kr * 4.0f * sht::math::kPi);
        ground_shader_->Uniform1f("fKm4PI", Km * 4.0f * sht::math::kPi);
        ground_shader_->Uniform1f("fScale", Scale);
        ground_shader_->Uniform1f("fScaleDepth", ScaleDepth);
        ground_shader_->Uniform1f("fScaleOverScaleDepth", ScaleOverScaleDepth);
        ground_shader_->Uniform1i("Samples", 4);
        ground_shader_->Uniform1i("u_earth_texture", 0);
        ground_shader_->Uniform1i("u_clouds_texture", 1);
        ground_shader_->Unbind();
        
        sky_shader_->Bind();
        sky_shader_->Uniform3f("v3LightPos", 0.0f, 0.0f, 1.0f);
        sky_shader_->Uniform3f("v3InvWavelength", 1.0f / powf(0.650f, 4.0f), 1.0f / powf(0.570f, 4.0f), 1.0f / powf(0.475f, 4.0f));
        sky_shader_->Uniform1f("fInnerRadius", kInnerRadius);
        sky_shader_->Uniform1f("fOuterRadius", kOuterRadius);
        sky_shader_->Uniform1f("fOuterRadius2", kOuterRadius * kOuterRadius);
        sky_shader_->Uniform1f("fKrESun", Kr * ESun);
        sky_shader_->Uniform1f("fKmESun", Km * ESun);
        sky_shader_->Uniform1f("fKr4PI", Kr * 4.0f * sht::math::kPi);
        sky_shader_->Uniform1f("fKm4PI", Km * 4.0f * sht::math::kPi);
        sky_shader_->Uniform1f("fScale", Scale);
        sky_shader_->Uniform1f("fScaleDepth", ScaleDepth);
        sky_shader_->Uniform1f("fScaleOverScaleDepth", ScaleOverScaleDepth);
        sky_shader_->Uniform1i("Samples", 4);
        sky_shader_->Uniform1f("g", g);
        sky_shader_->Uniform1f("g2", g * g);
        sky_shader_->Unbind();
	}
	void BindShaderVariables()
	{
		float distance_to_earth = camera_manager_->position()->Distance(kEarthPosition);
        int from_space = (distance_to_earth > kOuterRadius) ? 1 : 0;

		ground_shader_->Bind();
		ground_shader_->Uniform3fv("v3CameraPos", *camera_manager_->position());
		ground_shader_->Uniform1f("fCameraHeight", distance_to_earth);
		ground_shader_->Uniform1f("fCameraHeight2", distance_to_earth * distance_to_earth);
        ground_shader_->Uniform1i("u_from_space", from_space);
		ground_shader_->Unbind();

		sky_shader_->Bind();
		sky_shader_->Uniform3fv("v3CameraPos", *camera_manager_->position());
		sky_shader_->Uniform1f("fCameraHeight", distance_to_earth);
		sky_shader_->Uniform1f("fCameraHeight2", distance_to_earth * distance_to_earth);
        sky_shader_->Uniform1i("u_from_space", from_space);
		sky_shader_->Unbind();
	}
    bool Load() final
    {
        // Sphere model
		sphere_ = new sht::graphics::SphereModel(renderer_, 128, 64);
		sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
		sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
		sphere_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTexcoord, 2));
		sphere_->Create();
        if (!sphere_->MakeRenderable())
            return false;
        
		// Load shaders
        const char *attribs[] = {"a_position", "a_normal", "a_texcoord"};
        if (!renderer_->AddShader(ground_shader_, "data/shaders/atmosphere/ground", attribs, _countof(attribs))) return false;
		if (!renderer_->AddShader(sky_shader_, "data/shaders/atmosphere/sky", attribs, _countof(attribs))) return false;
        if (!renderer_->AddShader(text_shader_, "data/shaders/text", attribs, 1)) return false;
        if (!renderer_->AddShader(gui_shader_, "data/shaders/gui_colored", attribs, 1)) return false;
        
		// Load textures
        if (!renderer_->AddTexture(earth_texture_, "data/textures/earth.jpg")) return false;
		if (!renderer_->AddTexture(clouds_texture_, "data/textures/clouds.jpg")) return false;
		if (!renderer_->AddTexture(lights_texture_, "data/textures/lights.jpg")) return false;

        renderer_->AddFont(font_, "data/fonts/GoodDog.otf");
		if (font_ == nullptr)
			return false;

		fps_text_ = sht::graphics::DynamicText::Create(renderer_, 30);
        if (!fps_text_)
            return false;
        
        console_ = new sht::utility::Console(renderer_, font_, gui_shader_, text_shader_, 0.7f, 0.1f, 0.8f, aspect_ratio_);

        camera_manager_ = new sht::utility::CameraManager();
        
        planet_navigation_ = new sht::geo::PlanetNavigation(camera_manager_, sht::geo::kEarthRadius, kCameraDistance, 100.0f);

		// Finally bind constants
		BindShaderConstants();
        
        return true;
    }
    void Unload() final
    {
        if (planet_navigation_)
            delete planet_navigation_;
        if (camera_manager_)
            delete camera_manager_;
		if (console_)
			delete console_;
		if (fps_text_)
			delete fps_text_;
		if (sphere_)
			delete sphere_;
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

		BindShaderVariables();
    }
    void RenderGround()
    {
        renderer_->PushMatrix();
        renderer_->Translate(kEarthPosition);
        renderer_->Scale(kInnerRadius);
        renderer_->MultMatrix(rotate_matrix);
        
        ground_shader_->Bind();
        ground_shader_->UniformMatrix4fv("u_projection", renderer_->projection_matrix());
        ground_shader_->UniformMatrix4fv("u_view", renderer_->view_matrix());
        ground_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
        
        renderer_->ChangeTexture(earth_texture_, 0);
        renderer_->ChangeTexture(clouds_texture_, 1);
        
        sphere_->Render();
        
        renderer_->ChangeTexture(nullptr, 1);
        renderer_->ChangeTexture(nullptr, 0);
        
        ground_shader_->Unbind();
        
        renderer_->PopMatrix();
    }
    void RenderSky()
    {
        renderer_->CullFace(sht::graphics::CullFaceType::kFront);
        
        renderer_->PushMatrix();
        renderer_->Translate(kEarthPosition);
        renderer_->Scale(kOuterRadius);
        renderer_->MultMatrix(rotate_matrix);
        
        sky_shader_->Bind();
        sky_shader_->UniformMatrix4fv("u_projection", renderer_->projection_matrix());
        sky_shader_->UniformMatrix4fv("u_view", renderer_->view_matrix());
        sky_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
        
        sphere_->Render();
        
        sky_shader_->Unbind();
        
        renderer_->PopMatrix();
        
        renderer_->CullFace(sht::graphics::CullFaceType::kBack);
    }
    void RenderInterface()
    {
        renderer_->DisableDepthTest();
        
        // Draw FPS
        text_shader_->Bind();
        text_shader_->Uniform1i("u_texture", 0);
        text_shader_->Uniform4f("u_color", 1.0f, 0.5f, 1.0f, 1.0f);
        fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"fps: %.2f", frame_rate_);
        fps_text_->Render();
        
        // Draw console
        console_->Render();
        
        renderer_->EnableDepthTest();
    }
    void Render() final
    {
//#ifdef TARGET_MAC
//        std::lock_guard<std::mutex> lock(mutex_);
//#endif
        //vec3 light_pos_eye = renderer_->view_matrix() * kSunPosition;
        
        renderer_->SetViewport(width_, height_);
        
        renderer_->ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        renderer_->ClearColorAndDepthBuffers();
        
        RenderGround();
        RenderSky();

        RenderInterface();
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
                planet_navigation_->SmoothZoomIn();
            }
            else if (key == sht::PublicKey::kMinus)
            {
                planet_navigation_->SmoothZoomOut();
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
                planet_navigation_->SmoothRotation(angle_x);
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
            planet_navigation_->PanBegin(mouse_.x(), mouse_.y(), viewport, proj, view);
        }
    }
    void OnMouseUp(sht::MouseButton button, int modifiers) final
    {
        if (mouse_.button_down(sht::MouseButton::kLeft))
        {
            planet_navigation_->PanEnd();
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
            planet_navigation_->PanMove(mouse_.x(), mouse_.y(), viewport, proj, view);
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
            planet_navigation_->ObtainZNearZFar(&znear, &zfar);
            renderer_->SetProjectionMatrix(sht::math::PerspectiveMatrix(45.0f, width(), height(), znear, zfar));
        }
    }
    
private:
#ifdef TARGET_MAC
    std::mutex mutex_;
#endif
    sht::graphics::Model * sphere_;
    sht::graphics::Shader * ground_shader_;
	sht::graphics::Shader * sky_shader_;
    sht::graphics::Shader * gui_shader_;
    sht::graphics::Shader * text_shader_;
    sht::graphics::Texture * earth_texture_;
	sht::graphics::Texture * clouds_texture_;
	sht::graphics::Texture * lights_texture_;
    sht::graphics::Font * font_;
    sht::graphics::DynamicText * fps_text_;
    sht::utility::Console * console_;
    sht::utility::CameraManager * camera_manager_;
    sht::geo::PlanetNavigation * planet_navigation_;
    
    sht::math::Matrix4 rotate_matrix;
    sht::math::Matrix3 normal_matrix;
    
    float angle_; //!< rotation angle of earth
    
    bool need_update_projection_matrix_;
    bool camera_animation_stopped_;
};

DECLARE_MAIN(ShtilleEarthApp);