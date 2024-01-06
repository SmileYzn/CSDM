#pragma once

class CReGameDLL
{
public:
	bool Init();
	bool Stop();

	IReGameApi* m_API;
	const ReGameFuncs_t* m_Funcs;
	IReGameHookchains* m_Hookchains;
	CGameRules* m_Rules;

	static CGameRules* InstallGameRules(IReGameHook_InstallGameRules* chain);
};

extern CReGameDLL gReGameDLL;