#pragma once

#pragma region DLL_PRE
void DLL_PRE_ClientCommand(edict_t* pEntity);
#pragma endregion

#pragma region DLL_POST
void DLL_POST_ServerActivate(edict_t* pEdictList, int edictCount, int clientMax);
void DLL_POST_ServerDeactivate();
#pragma endregion