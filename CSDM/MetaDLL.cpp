#include "precompiled.h"

DLL_FUNCTIONS gDLL_FunctionTable_Pre;
DLL_FUNCTIONS gDLL_FunctionTable_Post;

#pragma region DLL_PRE
C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS* pFunctionTable, int* interfaceVersion)
{
	memset(&gDLL_FunctionTable_Pre, 0, sizeof(DLL_FUNCTIONS));

	gDLL_FunctionTable_Pre.pfnClientCommand = DLL_PRE_ClientCommand;

	memcpy(pFunctionTable, &gDLL_FunctionTable_Pre, sizeof(DLL_FUNCTIONS));

	return 1;
}

void DLL_PRE_ClientCommand(edict_t* pEntity)
{
	if (gCSDM_Command.ClientCommand(pEntity))
	{
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}
#pragma endregion

#pragma region DLL_POST
C_DLLEXPORT int GetEntityAPI2_Post(DLL_FUNCTIONS* pFunctionTable, int* interfaceVersion)
{
	memset(&gDLL_FunctionTable_Post, 0, sizeof(DLL_FUNCTIONS));

	gDLL_FunctionTable_Post.pfnServerActivate = DLL_POST_ServerActivate;

	gDLL_FunctionTable_Post.pfnServerDeactivate = DLL_POST_ServerDeactivate;

	memcpy(pFunctionTable, &gDLL_FunctionTable_Post, sizeof(DLL_FUNCTIONS));

	return 1;
}

void DLL_POST_ServerActivate(edict_t* pEdictList, int edictCount, int clientMax)
{
	gCSDM_Config.ServerActivate();

	gCSDM.ServerActivate();

	gCSDM_Misc.ServerActivate();

	gCSDM_Spawn.ServerActivate();

	gCSDM_SpawnEdit.ServerActivate();

	gCSDM_Weapon.ServerActivate();

	RETURN_META(MRES_IGNORED);
}

void DLL_POST_ServerDeactivate()
{
	gCSDM.ServerDeactivate();

	gCSDM_Misc.ServerDeactivate();

	gCSDM_SpawnEdit.ServerDeactivate();

	RETURN_META(MRES_IGNORED);
}
#pragma endregion