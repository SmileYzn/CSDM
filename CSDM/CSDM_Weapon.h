#pragma once

typedef struct S_ITEM_INFO
{
	std::string Alias;
	std::string Label;
	int Enabled;
	int Bot;
	int Slot;
} P_ITEM_INFO, * LP_ITEM_INFO;

typedef struct S_PLAYER_INFO
{
	bool HideMenu;
	std::map<int, int> WeaponState;
	std::string LastSecondary;
	std::string LastPrimary;

	void Reset()
	{
		this->HideMenu = false;
		this->WeaponState.clear();
		this->LastSecondary.clear();
		this->LastPrimary.clear();
	}
} P_PLAYER_INFO, *LP_PLAYER_INFO;

class CCSDM_Weapon
{
public:
	void ServerActivate();

	void Enable();
	void Disable();

	static bool CBasePlayer_GetIntoGame(IReGameHook_CBasePlayer_GetIntoGame* chain, CBasePlayer* Player);
	static void CSGameRules_PlayerSpawn(IReGameHook_CSGameRules_PlayerSpawn* chain, CBasePlayer* Player);
	static void CBasePlayer_SetAnimation(IReGameHook_CBasePlayer_SetAnimation* chain, CBasePlayer* Player, PLAYER_ANIM playerAnimation);

	void ResetPlayer(CBasePlayer* Player);

	void EquipItem(CBasePlayer* Player, InventorySlotType SlotType, std::string Item);
	void EquipRandom(CBasePlayer* Player, int Slot);
	bool EquipLast(CBasePlayer* Player);

	bool SetHideMenu(CBasePlayer* Player, bool HideMenu);
	bool GetHideMenu(int EntityIndex);

	void SetWeaponState(int EntityIndex, CBasePlayerWeapon* Weapon);
	int GetWeaponState(int EntityIndex, int WeaponIndex);

	void EquipMenu(int EntityIndex);
	static void EquipMenuHandle(int EntityIndex, P_MENU_ITEM Item);

	void SecondaryMenu(int EntityIndex);
	static void SecondaryMenuHandle(int EntityIndex, P_MENU_ITEM Item);

	void PrimaryMenu(int EntityIndex);
	static void PrimaryMenuHandle(int EntityIndex, P_MENU_ITEM Item);

private:
	bool m_Enabled;
	std::vector<P_ITEM_INFO> m_Weapons;
	std::map<int, P_PLAYER_INFO> m_Info;
};

extern CCSDM_Weapon gCSDM_Weapon;