#pragma once

constexpr auto PRINT_TEAM_DEFAULT = 0;
constexpr auto PRINT_TEAM_GREY = -1;
constexpr auto PRINT_TEAM_RED = -2;
constexpr auto PRINT_TEAM_BLUE = -3;

class CCSDM_Util
{
public:
	cvar_t* CvarRegister(const char* Name, const char* Value);
	void ServerCommand(const char* Format, ...);
	void TeamInfo(edict_t* pEntity, int playerIndex, const char* pszTeamName);
	void DeathMsg(edict_t* pEntity, int KillerId, int VictimId, int IsHeadshot, const char* TruncatedWeaponName);
	void ScreenFade(edict_t* pEdict, int Duration, int HoldTime, int Flags, int Red, int Green, int Blue, int Alpha);
	int ParseColors(char* Buffer);
	void SayText(edict_t* pEntity, int Sender, const char* Format, ...);
	void ReplaceAll(std::string& String, const std::string& From, const std::string& To);
	unsigned short FixedUnsigned16(float value, float scale);
	short FixedSigned16(float value, float scale);
	hudtextparms_t SetHudParam(int red, int green, int blue, float x, float y, int effects, float fxtime, float holdtime, float fadeintime, float fadeouttime, int channel);
	void ShowHudMessage(edict_t* pEntity, hudtextparms_t textparms, const char* Format, ...);
};

extern CCSDM_Util gCSDM_Util;