#pragma once

class CCSDM
{
public:
	void ServerActivate();
	void ServerDeactivate();
	bool IsActive();
	void StripEntities(bool Remove);
	static void Cvar_DirectSet(IRehldsHook_Cvar_DirectSet* chain, cvar_s* pCvar, const char* pszValue);
	static void CBasePlayer_JoiningThink(IReGameHook_CBasePlayer_JoiningThink* chain, CBasePlayer* Player);

private:
	cvar_t* m_Active;
};

extern CCSDM gCSDM;