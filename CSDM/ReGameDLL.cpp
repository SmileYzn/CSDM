#include "precompiled.h"

CReGameDLL gReGameDLL;

bool CReGameDLL::Init()
{
	auto szGameDLLModule = gpMetaUtilFuncs->pfnGetGameInfo(&Plugin_info, GINFO_DLL_FULLPATH);

	if (szGameDLLModule)
	{
		auto GameModule = Sys_LoadModule(szGameDLLModule);

		if (GameModule)
		{
			auto ifaceFactory = Sys_GetFactory(GameModule);

			if (ifaceFactory)
			{
				int retCode = 0;

				this->m_API = (IReGameApi*)ifaceFactory(VRE_GAMEDLL_API_VERSION, &retCode);

				if (this->m_API)
				{
					if (this->m_API->GetMajorVersion() == REGAMEDLL_API_VERSION_MAJOR)
					{
						if (this->m_API->GetMinorVersion() >= REGAMEDLL_API_VERSION_MINOR)
						{
							if (this->m_API->BGetICSEntity(CSENTITY_API_INTERFACE_VERSION))
							{
								if (gReGameDLL.m_API->BGetIGameRules(GAMERULES_API_INTERFACE_VERSION))
								{
									this->m_Funcs = this->m_API->GetFuncs();

									this->m_Hookchains = this->m_API->GetHookchains();

									this->m_Hookchains->InstallGameRules()->registerHook(this->InstallGameRules);
								}

								return true;
							}
						}
					}
				}
			}
		}
	}

	gpMetaUtilFuncs->pfnLogConsole(&Plugin_info, "[%s] ReGameDLL_CS API failed to load.", Plugin_info.logtag);

	return false;
}

bool CReGameDLL::Stop()
{
	this->m_Hookchains->InstallGameRules()->unregisterHook(this->InstallGameRules);

	return false;
}

CGameRules* CReGameDLL::InstallGameRules(IReGameHook_InstallGameRules* chain)
{
	gReGameDLL.m_Rules = chain->callNext();

	return gReGameDLL.m_Rules;
}
