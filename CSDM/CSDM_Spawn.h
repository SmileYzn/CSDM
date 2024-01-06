#pragma once

constexpr auto SPAWN_POINT_MIN_DISTANCE = 250.0f;

typedef struct S_SPAWN_POINT
{
	vec3_t Vecs;
	vec3_t Angles;
	vec3_t VAngles;
	int Team;
} P_SPAWN_POINT, *LP_SPAWN_POINT;

class CCSDM_Spawn
{
public:
	void ServerActivate();

	void Enable();
	void Disable();

	std::map<size_t, P_SPAWN_POINT> GetSpawns();

	static edict_t* CSGameRules_GetPlayerSpawnSpot(IReGameHook_CSGameRules_GetPlayerSpawnSpot* chain, CBasePlayer* Player);

	bool SetPlayerPosition(CBasePlayer* Player);
	bool CheckDistance(CBasePlayer* Player, vec3_t Origin, float Distance);

private:
	bool m_Enabled;
	std::map<size_t, P_SPAWN_POINT> m_Spawns;
};

extern CCSDM_Spawn gCSDM_Spawn;