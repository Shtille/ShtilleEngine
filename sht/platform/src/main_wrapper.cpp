#include "../include/main_wrapper.h"
#include "platform_inner.h"
#include "../../application/application.h"
#include "../../system/include/time/update_timer.h"

int MainWrapper(int argc, const char** argv)
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

			// Start time
            sht::system::UpdateTimer update_timer;
			update_timer.Start();

			while (!PlatformNeedQuit())
			{
				// Calculate dt
				app->SetFrameTime(update_timer.GetElapsedTime());

				app->Update();

				app->BeginFrame();
				app->Render();
				app->EndFrame();

				PlatformPollEvents();
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