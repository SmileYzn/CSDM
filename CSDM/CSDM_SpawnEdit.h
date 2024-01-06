#pragma once

class CCSDM_SpawnEdit
{
public:
	void ServerActivate();
	void ServerDeactivate();
	void EditSpawns(CBasePlayer* Player);
	void Menu(CBasePlayer* Player);
	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);
	void AddSpawnMenu(CBasePlayer* Player);
	static void AddSpawnMenuHandle(int EntityIndex, P_MENU_ITEM Item);
	void DiscardChanges(CBasePlayer* Player);
	static void DiscardChangesHandle(int EntityIndex, P_MENU_ITEM Item);
	edict_t* MakeEntity(int EntityIndex);
	void RemoveEntity(int EntityIndex);
	void GlowEnt(int EntityIndex, vec3_t Color);
	void UnGlowEnt(int EntityIndex);
	void SetPosition(int EntityIndex);
	void AddSpawn(entvars_t v, int Team, float FixOrigin);
	void EditSpawn(int EntityIndex, entvars_t v, float FixOrigin);
	void DeleteSpawn(int EntityIndex);
	int ClosestSpawn(edict_t* pEdict);
	void Refresh(CBasePlayer* Player);
	void ShowStats(CBasePlayer* Player);
	void Save(CBasePlayer* Player);

private:
	cvar_t* m_Admin;
	string_t m_InfoTarget;
	std::map<size_t, P_SPAWN_POINT> m_Spawns;
	std::map<int, edict_t*> m_Entities;
	std::map<int, int> m_Marked;
};

extern CCSDM_SpawnEdit gCSDM_SpawnEdit;