// TitleGameMode.cpp


#include "Gamemode/TitleGameMode.h"
#include "UI/MainHUD.h"

ATitleGameMode::ATitleGameMode()
{
	// HUD는 MainHUD 그대로 사용.
	HUDClass = AMainHUD::StaticClass();

	// 캐릭터 소환 금지
	DefaultPawnClass = nullptr;
}