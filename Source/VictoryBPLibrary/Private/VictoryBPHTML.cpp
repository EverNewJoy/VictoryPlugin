/*
	By Rama
*/
#include "VictoryBPLibraryPrivatePCH.h"
#include "VictoryBPHTML.h"

#if PLATFORM_HTML5_BROWSER
	#include "SDL_opengl.h"

	DEFINE_LOG_CATEGORY_STATIC(VictoryPluginHTML, Log, All);
	
	#include "emscripten.h"
	#include "html5.h"
#endif

bool UVictoryBPHTML::IsHTML()
{ 
	#if PLATFORM_HTML5_BROWSER
	return true;
	#else
	return false;
	#endif //HTML
}

void UVictoryBPHTML::VictoryHTML5_SetCursorVisible(bool MakeVisible)
{ 
	if(MakeVisible)
	{
		#if PLATFORM_HTML5_WIN32
		{
			SDL_SetRelativeMouseMode(SDL_FALSE);
			SDL_ShowCursor(SDL_ENABLE);
			SDL_SetWindowGrab(WindowHandle, SDL_FALSE); 
			UE_LOG(VictoryPluginHTML, Warning, TEXT("SDL Showing Mouse Cursor"));
		}
		#endif
		 
		#if PLATFORM_HTML5_BROWSER
		{
			emscripten_exit_pointerlock(); 
			UE_LOG(VictoryPluginHTML, Warning, TEXT("Exiting Pointer Lock"));
		} 
		#endif
	}
	else 
	{
		#if PLATFORM_HTML5_WIN32
		{
			SDL_SetWindowGrab(WindowHandle, SDL_TRUE);
			SDL_ShowCursor(SDL_DISABLE);
		    SDL_SetRelativeMouseMode(SDL_TRUE);
			UE_LOG(VictoryPluginHTML, Warning, TEXT("SDL Hiding Mouse Cursor"));
		} 
		#endif
		 
		#if PLATFORM_HTML5_BROWSER
		{
			emscripten_request_pointerlock ( "#canvas" , true);
			UE_LOG(VictoryPluginHTML, Warning, TEXT("Entering Pointer Lock"));
		} 
		#endif
	}
}