#include "../include/main_wrapper.h"
#include "platform_inner.h"
#include "../../application/application.h"
#include "../../system/include/time/clock.h"

int MainWrapper(int, const char**)
{
	if (!PlatformInit())
		return 1;

	if (!PlatformWindowCreate())
	{
		PlatformTerminate();
		return 2;
	}

	sht::Application * app = sht::Application::GetInstance();
	app->Center();

	if (app->InitApi())
	{
		if (app->Load())
		{
			app->Show();
			app->set_visible(true);

			sht::system::Clock clock;
			float time_gameclock = clock.GetTime();
			float time_physics_prev, time_physics_curr;
			time_physics_prev = time_physics_curr = time_gameclock;
			const float kTickTime = 1.0f / app->GetDesiredFrameRate();

			while (!PlatformNeedQuit())
			{
				// Render a frame
				app->BeginFrame();
				app->Render();
				app->EndFrame();

				// Update physics
				time_physics_curr = clock.GetTime();
				app->UpdatePhysics(time_physics_curr - time_physics_prev);
				time_physics_prev = time_physics_curr;

				// Game clock part of the loop. Ticks for every tick_time at average.
				float dt = clock.GetTime() - time_gameclock;

				while (dt >= kTickTime)
				{
					dt -= kTickTime;
					time_gameclock += kTickTime;

					PlatformPollEvents();

					app->SetFrameTime(kTickTime);
					app->Update();
				}
			}
		}
		app->Unload(); // delete allocated objects (may be allocated partially)
        app->DeinitApi();
	}
	else
	{
		PlatformWindowDestroy();
	}

	PlatformTerminate();

	return 0;
}