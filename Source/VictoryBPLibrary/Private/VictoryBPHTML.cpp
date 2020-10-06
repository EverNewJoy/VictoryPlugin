/*
	By Rama
*/
#include "VictoryBPLibraryPrivatePCH.h"
#include "VictoryBPHTML.h"


//! Someone should please integrate the HTML5 plugin with this code

/*
#if PLATFORM_HTML5
	#include "SDL_opengl.h"

	DEFINE_LOG_CATEGORY_STATIC(VictoryPluginHTML, Log, All);
	
	#include "emscripten.h"
	#include "html5.h"
#endif
*/

/*
bool UVictoryBPHTML::IsHTML()
{ 
	
	#if PLATFORM_HTML5
	return true;
	#else
	return false;
	#endif 
	
	return false;
}
*/

/*
void UVictoryBPHTML::VictoryHTML5_SetCursorVisible(bool MakeVisible)
{ 
	if(MakeVisible)
	{
		#if PLATFORM_HTML5
		{
			emscripten_exit_pointerlock(); 
			UE_LOG(VictoryPluginHTML, Warning, TEXT("Exiting Pointer Lock"));
		} 
		#endif
	}
	else 
	{	 
		#if PLATFORM_HTML5
		{
			emscripten_request_pointerlock ( "#canvas" , true);
			UE_LOG(VictoryPluginHTML, Warning, TEXT("Entering Pointer Lock"));
		} 
		#endif
	}

}
*/