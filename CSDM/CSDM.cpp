#include "precompiled.h"

CCSDM gCSDM;

void CCSDM::ServerActivate()
{
	std::srand(std::time(NULL));

	this->m_Active = gCSDM_Util.CvarRegister("csdm_active", "0");

	if (gReAPI.m_Hookchains)
	{
		gReAPI.m_Hookchains->Cvar_DirectSet()->registerHook(this->Cvar_DirectSet);
	}

	if (gReGameDLL.m_Hookchains)
	{
		gReGameDLL.m_Hookchains->CBasePlayer_JoiningThink()->registerHook(this->CBasePlayer_JoiningThink);
	}

	gCSDM_Config.Execute("csdm.cfg");
}

void CCSDM::ServerDeactivate()
{
	if (gReAPI.m_Hookchains)
	{
		gReAPI.m_Hookchains->Cvar_DirectSet()->unregisterHook(this->Cvar_DirectSet);
	}

	if (gReGameDLL.m_Hookchains)
	{
		gReGameDLL.m_Hookchains->CBasePlayer_JoiningThink()->unregisterHook(this->CBasePlayer_JoiningThink);
	}
}

bool CCSDM::IsActive()
{
	return (this->m_Active->value != 0.0f);
}

void CCSDM::StripEntities(bool StripEntities)
{
	std::vector<std::vector<std::string>> Entities =
	{
		{"func_bomb_target", "_func_bomb_target"},
		{"info_bomb_target","_info_bomb_target"},
		{"func_hostage_rescue","_func_hostage_rescue"},
		{"info_hostage_rescue","_info_hostage_rescue"},
		{"info_vip_start","_info_vip_start"},
		{"func_vip_safetyzone","_func_vip_safetyzone"},
		{"func_escapezone","_func_escapezone"},
		{"func_buyzone","_func_buyzone"}
	};

	edict_t* pEdict = nullptr;

	for (auto& Name : Entities)
	{
		while (!FNullEnt(pEdict = g_engfuncs.pfnFindEntityByString(pEdict, "classname", StripEntities ? Name[0].c_str() : Name[1].c_str())))
		{
			pEdict->v.classname = g_engfuncs.pfnAllocString(StripEntities ? Name[1].c_str() : Name[0].c_str());
		}
	}
}

void CCSDM::Cvar_DirectSet(IRehldsHook_Cvar_DirectSet* chain, struct cvar_s* pCvar, const char* pszValue)
{
	chain->callNext(pCvar, pszValue);

	if (pCvar)
	{
		if (Q_stricmp(pCvar->name, gCSDM.m_Active->name) == 0)
		{
			if (pCvar->value)
			{
				gCSDM.StripEntities(true);

				gCSDM_Util.SayText(nullptr, PRINT_TEAM_BLUE, "^3CS Deathmatch enabled.");

				gCSDM_Config.Execute("csdm_enable.cfg");

				gCSDM_Misc.Enable();

				gCSDM_Spawn.Enable();

				gCSDM_Weapon.Enable();
			}
			else
			{
				gCSDM.StripEntities(false);

				gCSDM_Util.SayText(nullptr, PRINT_TEAM_RED, "^3CS Deathmatch disabled.");

				gCSDM_Config.Execute("csdm_disable.cfg");

				gCSDM_Misc.Disable();

				gCSDM_Spawn.Disable();
				
				gCSDM_Weapon.Disable();
			}
		}
	}
}

void CCSDM::CBasePlayer_JoiningThink(IReGameHook_CBasePlayer_JoiningThink* chain, CBasePlayer* Player)
{
	chain->callNext(Player);

	if (!Player->IsBot())
	{
		gCSDM_Util.TeamInfo(Player->edict(), MAX_CLIENTS + TERRORIST + 1, "TERRORIST");
		gCSDM_Util.TeamInfo(Player->edict(), MAX_CLIENTS + CT + 1, "CT");
	}
}

