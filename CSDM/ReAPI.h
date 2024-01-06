#pragma once

class CReAPI
{
public:
	bool Init();
	bool Stop();

	IRehldsApi* m_API;
	const RehldsFuncs_t* m_Funcs;
	IRehldsServerData* m_Data;
	IRehldsHookchains* m_Hookchains;
	IRehldsServerStatic* m_Server_Static;
};

extern CReAPI gReAPI;