#pragma once

// Hitbox
constexpr auto HITGROUP_GENERIC = 0;
constexpr auto HITGROUP_HEAD = 1;
constexpr auto HITGROUP_CHEST = 2;
constexpr auto HITGROUP_STOMACH = 3;
constexpr auto HITGROUP_LEFTARM = 4;
constexpr auto HITGROUP_RIGHTARM = 5;
constexpr auto HITGROUP_LEFTLEG = 6;
constexpr auto HITGROUP_RIGHTLEG = 7;
constexpr auto HITGROUP_SHIELD = 8;
constexpr auto NUM_HITGROUPS = 9;

class CCSDM_Misc
{
public:
	void ServerActivate();
	void ServerDeactivate();

	void Enable();
	void Disable();

	void TakeDamage(CBasePlayer* Victim, CBasePlayer* Attacker, float flDamage, int bitsDamageType);
	void PlayerKilled(CBasePlayer* Victim, CBasePlayer* Killer);
	void UpdateClientData(CBasePlayer* Player, bool Reset);
	int AddAccount(CBasePlayer* Player, int Amount);
	bool SendDeathMessage(CBaseEntity* Killer, CBasePlayer* Victim, CBasePlayer* Assister, entvars_t* pevInflictor, const char* killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);

	static bool CBasePlayer_GetIntoGame(IReGameHook_CBasePlayer_GetIntoGame* chain, CBasePlayer* Player);
	static void CBasePlayer_UpdateClientData(IReGameHook_CBasePlayer_UpdateClientData* chain, CBasePlayer* Player);
	static BOOL CBasePlayer_TakeDamage(IReGameHook_CBasePlayer_TakeDamage* chain, CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType);
	static void CSGameRules_PlayerKilled(IReGameHook_CSGameRules_PlayerKilled* chain, CBasePlayer* Victim, entvars_t* pevKiller, entvars_t* pevInflictor);
	static void CBasePlayer_AddAccount(IReGameHook_CBasePlayer_AddAccount* chain, CBasePlayer* Player, int amount, RewardType type, bool bTrackChange);
	static void CSGameRules_SendDeathMessage(IReGameHook_CSGameRules_SendDeathMessage* chain, CBaseEntity* Killer, CBasePlayer* Victim, CBasePlayer* Assister, entvars_t* pevInflictor, const char* killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);

	bool TraceLine(const float* start, const float* end, int fNoMonsters, edict_t* pentToSkip, TraceResult* ptr);

private:
	bool m_Enabled = 0;

	cvar_t* m_hs_only_mode;
	cvar_t* m_kill_fade;
	cvar_t* m_kill_sound;
	cvar_t* m_kill_hp;
	cvar_t* m_kill_hp_hs;
	cvar_t* m_kill_msg;
	cvar_t* m_hit_indicator;
	cvar_t* m_hide_kill_feed;
	cvar_t* m_money_as_frags;
	cvar_t* m_kd_ratio_hud;
	cvar_t* m_reload_on_kill;

	const char* m_HitGroup[9] ={ "Generic", "Head", "Chest", "Stomach", "Left Arm", "Right Arm", "Left Leg", "Right Leg", "Shield"};

	std::map<int, int> m_Headshots;
};

extern CCSDM_Misc gCSDM_Misc;

// Only HS (0, 1)

// Kill Screenfade (0, 1)
// Kill Sound (0, 1)

// Hits indicator
// Point Symbol   (1)
// >      <	      (2)
// Damage         (3)
// Place          (4)

// Hide other killfeed (0, 1)

// Show money as frags (0, 1)

// Show KD Ratio (0, 1)

// ??
// Say /menu or press 'G' key
