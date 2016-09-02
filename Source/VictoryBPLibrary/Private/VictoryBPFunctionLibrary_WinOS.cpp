/*
	By Rama
*/
    
#include "VictoryBPLibraryPrivatePCH.h"
#include "VictoryBPFunctionLibrary.h"

#if PLATFORM_WINDOWS
#include "AllowWindowsPlatformTypes.h"
//#include "AdditionalWindowsHeaders.h"
#endif

void UVictoryBPFunctionLibrary::FlashGameOnTaskBar(APlayerController* PC, bool FlashContinuous, int32 MaxFlashCount, int32 FlashFrequencyMilliseconds)
{
	#if PLATFORM_WINDOWS  
	if(!PC) return;
	
	//Local Player
	ULocalPlayer* VictoryPlayer = Cast<ULocalPlayer>(PC->Player);
	if(!VictoryPlayer) return;
	  
	//Game Viewport Client
	UGameViewportClient* GameViewport = Cast<UGameViewportClient>(VictoryPlayer->ViewportClient);
	if(!GameViewport) return;
	
	//Slate Game Window
	TSharedPtr< SWindow > GWSlate = GameViewport->GetWindow();
	if(!GWSlate.IsValid()) return;
	
	//Native OS Window
	TSharedPtr<FGenericWindow> GW = GWSlate->GetNativeWindow(); 
	if(!GW.IsValid()) return;
	   
	//Windows
	FLASHWINFO fi;
	fi.cbSize = sizeof(FLASHWINFO);
	fi.hwnd = (HWND)GW->GetOSWindowHandle(); 
	fi.dwFlags = FLASHW_ALL;
	
	//Continuous? <3 Rama
	if(FlashContinuous)
	{
		fi.dwFlags |= FLASHW_TIMERNOFG;
		fi.uCount = 0;
		fi.dwTimeout = 0;
	}
	else
	{
		fi.uCount = MaxFlashCount;
		fi.dwTimeout = FlashFrequencyMilliseconds;
	}
	 
	FlashWindowEx(&fi);
	#endif
	
	//<3 Rama
}





#if PLATFORM_WINDOWS
#include "HideWindowsPlatformTypes.h"
#endif
