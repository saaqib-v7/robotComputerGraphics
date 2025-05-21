// GLEW
#include <GL/glew.h>

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// ImGui
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

// standard
#include <iostream>
#include <sstream>

#include "MyApp.h"

int main( int argc, char* args[] )
{
	// Step 1: initialize SDL

	// configure SDL's logging
	SDL_LogSetPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR);

	// initialize only the graphical subsystem, on failure log it and exit
	if ( SDL_Init( SDL_INIT_VIDEO ) == -1 )
	{
		// log the error and terminate the program
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[SDL initialization] Error during the SDL initialization: %s", SDL_GetError());
		return 1;
	}

	// After SDL Init runs, the subsystems should be shut down on exit
	// This way, it will run even if we exit due to some error
	std::atexit(SDL_Quit);



			
	// Step 2: configure our OpenGL needs, create a window, and start OpenGL

	// 2a: configure OpenGL's start, this needs to be done before creating a window!

	// we can configure which specific OpenGL context we want to create - if this is not done,
	// then we get one with the highest available version number automatically
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef _DEBUG 
	// if the compiling is in debug mode, then set the OpenGL context to debug mode as well, this way the debug callback will work correctly
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif 

	// configure on how many bits we want to store the red, green, blue, and transparency information per pixel
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,         32);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);

	// double buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,	1);
	
	// how many bits for the depth buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,		24);

	// anti-aliasing - if needed
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);

	// create our window
	SDL_Window *win = nullptr;
	win = SDL_CreateWindow( "Hello SDL&OpenGL!",		// window title
							100,						// initial X coordinate of the top-left corner of the window
							100,						// initial Y coordinate of the top-left corner of the window
							800,						// window width
							600,						// window height
							SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);			// display properties


	// if the window creation failed, log the error and exit
	if (win == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Window creation] Error during the SDL initialization: %s", SDL_GetError());
		return 1;
	}




	// Step 3: create the OpenGL context - we will use this to draw

	SDL_GLContext	context = SDL_GL_CreateContext(win);
	if (context == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[OGL context creation] Error during the creation of the OGL context: %s", SDL_GetError());
		return 1;
	}	

	// display: await vsync
	SDL_GL_SetSwapInterval(1);

	// start GLEW
	GLenum error = glewInit();
	if ( error != GLEW_OK )
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[GLEW] Error during the initialization of glew.");
		return 1;
	}

	// get the OpenGL version
	int glVersion[2] = {-1, -1}; 
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]); 
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]); 

	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Running OpenGL %d.%d", glVersion[0], glVersion[1]);

	if ( glVersion[0] == -1 && glVersion[1] == -1 )
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow( win );

		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[OGL context creation] Error during the inialization of the OGL context! Maybe one of the SDL_GL_SetAttribute(...) calls is erroneous.");
		
		return 1;
	}

	std::stringstream window_title;
	window_title << "OpenGL " << glVersion[0] << "." << glVersion[1];
	SDL_SetWindowTitle(win, window_title.str().c_str());

	//Imgui init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(win, context);
	ImGui_ImplOpenGL3_Init();




	// step 4: start the main event handler cycle

	{
		// should the program exit?
		bool quit = false;
		// the event to be processed goes here
		SDL_Event ev;

		// application instance
		CMyApp app;
		if (!app.Init())
		{
			SDL_GL_DeleteContext(context);
			SDL_DestroyWindow(win);
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[app.Init] Error during the initialization of the application!");
			return 1;
		}

		// display ImGui window
		bool ShowImGui = true;

		while (!quit)
		{
			// while there are still events to be processed, keep processing them:
			while ( SDL_PollEvent(&ev) )
			{
				ImGui_ImplSDL2_ProcessEvent(&ev);
				bool is_mouse_captured    = ImGui::GetIO().WantCaptureMouse;    // does imgui need the mouse cursor
				bool is_keyboard_captured = ImGui::GetIO().WantCaptureKeyboard;	// does imgui need the keyboard

				switch (ev.type)
				{
					case SDL_QUIT:
						quit = true;
						break;
					case SDL_KEYDOWN:
						if ( ev.key.keysym.sym == SDLK_ESCAPE )
							quit = true;

						// ALT + ENTER toggles the full-screen mode
						if ( ( ev.key.keysym.sym == SDLK_RETURN )  // Enter was pressed, ...
							 && ( ev.key.keysym.mod & KMOD_ALT )   // together with the Alt key, ...
							 && !( ev.key.keysym.mod & ( KMOD_SHIFT | KMOD_CTRL | KMOD_GUI ) ) ) // but other modifier keys were not pressed.
						{
							Uint32 FullScreenSwitchFlag = ( SDL_GetWindowFlags( win ) & SDL_WINDOW_FULLSCREEN_DESKTOP ) ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP;
							SDL_SetWindowFullscreen( win, FullScreenSwitchFlag );
							is_keyboard_captured = true; // The ALT+ENTER should not be received by the application.
						}
						// CTRL + F1 to show/hide the ImGui
						if ( ( ev.key.keysym.sym == SDLK_F1 )  // F1 was pressed, ...
							 && ( ev.key.keysym.mod & KMOD_CTRL )   // together with the CTRL key, ...
							 && !( ev.key.keysym.mod & ( KMOD_SHIFT | KMOD_ALT | KMOD_GUI ) ) ) // but other modifier keys were not pressed.
						{
							ShowImGui = !ShowImGui;
							is_keyboard_captured = true; // The CTRL+F1 should not be received by the application.
						}
						if ( !is_keyboard_captured )
							app.KeyboardDown(ev.key);
						break;
					case SDL_KEYUP:
						if ( !is_keyboard_captured )
							app.KeyboardUp(ev.key);
						break;
					case SDL_MOUSEBUTTONDOWN:
						if ( !is_mouse_captured )
							app.MouseDown(ev.button);
						break;
					case SDL_MOUSEBUTTONUP:
						if ( !is_mouse_captured )
							app.MouseUp(ev.button);
						break;
					case SDL_MOUSEWHEEL:
						if ( !is_mouse_captured )
							app.MouseWheel(ev.wheel);
						break;
					case SDL_MOUSEMOTION:
						if ( !is_mouse_captured )
							app.MouseMove(ev.motion);
						break;
					case SDL_WINDOWEVENT:
						// On some platforms (e.g. Windows) the SIZE_CHANGED does not get called when displaying for the first time.
						// We think this is a bug in the SDL library.
						// Due to this, we handle this case separately,
						// because MyApp could include configuration dependent on the window's size, e.g. the camera aspect ratio when calling perspective().
						if ( ( ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ) || ( ev.window.event == SDL_WINDOWEVENT_SHOWN ) )
						{
							int w, h;
							SDL_GetWindowSize( win, &w, &h );
							app.Resize( w, h );
						}
						break;
					default:
						app.OtherEvent( ev );
				}
			}



			// Calculate the time duration needed for the update!
			static Uint32 LastTick = SDL_GetTicks(); // we store what the previous "tick" was in a static variable.
			Uint32 CurrentTick = SDL_GetTicks(); // What is the current "tick".
			SUpdateInfo updateInfo // Converting it to seconds!
			{ 
				static_cast<float>(CurrentTick) / 1000.0f, 
				static_cast<float>(CurrentTick - LastTick) / 1000.0f 
			};
			LastTick = CurrentTick; // Save the current "tick" as the last one!

			app.Update( updateInfo );
			app.Render();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(); // After this, any imgui function can be called, right up to ImGui::Render().

			ImGui::NewFrame();
			if ( ShowImGui) app.RenderGUI();
			ImGui::Render();

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			SDL_GL_SwapWindow(win);
		}

		// our object should clean up after itself
		app.Clean();
	} // this way, the app's desctructor will run while our context still exists => the destructors of the classes, that use GPU resources, will run here as well




	// Step 5: exit

	// ImGui de-init
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow( win );

	return 0;
}
