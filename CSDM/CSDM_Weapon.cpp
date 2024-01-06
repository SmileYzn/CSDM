#include "precompiled.h"

CCSDM_Weapon gCSDM_Weapon;

void CCSDM_Weapon::ServerActivate()
{
	this->m_Weapons.clear();

	char Path[MAX_PATH] = { 0 };

	Q_sprintf(Path, "%s/weapons.cfg", gCSDM_Config.GetPath().c_str());

	std::ifstream File(Path);

	if (File)
	{
		std::string Line;

		P_ITEM_INFO Info;

		while (std::getline(File, Line))
		{
			std::istringstream Row(Line);

			if (Row >> Info.Alias >> Info.Label >> Info.Enabled >> Info.Bot >> Info.Slot)
			{
				Info.Alias.erase(std::remove(Info.Alias.begin(), Info.Alias.end(), '\"'), Info.Alias.end());

				Info.Alias = "weapon_" + Info.Alias;

				Info.Label.erase(std::remove(Info.Label.begin(), Info.Label.end(), '\"'), Info.Label.end());

				this->m_Weapons.push_back(Info);
			}
		}

		File.close();
	}
	else
	{
		gpMetaUtilFuncs->pfnLogConsole(&Plugin_info, "[%s] Failed to read file: %s", __func__, Path);
	}
}

void CCSDM_Weapon::Enable()
{
	if (!this->m_Enabled)
	{
		if (gReGameDLL.m_Hookchains)
		{
			gReGameDLL.m_Hookchains->CBasePlayer_GetIntoGame()->registerHook(this->CBasePlayer_GetIntoGame);

			gReGameDLL.m_Hookchains->CSGameRules_PlayerSpawn()->registerHook(this->CSGameRules_PlayerSpawn);

			gReGameDLL.m_Hookchains->CBasePlayer_SetAnimation()->registerHook(this->CBasePlayer_SetAnimation);
		}

		this->m_Enabled = true;
	}
}

void CCSDM_Weapon::Disable()
{
	if (this->m_Enabled)
	{
		if (gReGameDLL.m_Hookchains)
		{
			gReGameDLL.m_Hookchains->CBasePlayer_GetIntoGame()->unregisterHook(this->CBasePlayer_GetIntoGame);

			gReGameDLL.m_Hookchains->CSGameRules_PlayerSpawn()->unregisterHook(this->CSGameRules_PlayerSpawn);

			gReGameDLL.m_Hookchains->CBasePlayer_SetAnimation()->unregisterHook(this->CBasePlayer_SetAnimation);
		}

		this->m_Enabled = false;
	}
}

bool CCSDM_Weapon::CBasePlayer_GetIntoGame(IReGameHook_CBasePlayer_GetIntoGame* chain, CBasePlayer* Player)
{
	auto Result = chain->callNext(Player);

	gCSDM_Weapon.ResetPlayer(Player);

	return Result;
}

void CCSDM_Weapon::CSGameRules_PlayerSpawn(IReGameHook_CSGameRules_PlayerSpawn* chain, CBasePlayer* Player)
{
	chain->callNext(Player);

	if (!Player->IsBot())
	{
		if (!gCSDM_Weapon.GetHideMenu(Player->entindex()))
		{
			gCSDM_Weapon.EquipMenu(Player->entindex());
		}
		else
		{
			gCSDM_Weapon.EquipLast(Player);
		}
	}
	else
	{
		gCSDM_Weapon.EquipRandom(Player, 2);

		gCSDM_Weapon.EquipRandom(Player, 1);
	}
}

void CCSDM_Weapon::CBasePlayer_SetAnimation(IReGameHook_CBasePlayer_SetAnimation* chain, CBasePlayer* Player, PLAYER_ANIM playerAnimation)
{
	chain->callNext(Player, playerAnimation);

	if (playerAnimation == PLAYER_ATTACK1 || playerAnimation == PLAYER_ATTACK2 || playerAnimation == PLAYER_RELOAD)
	{
		if (Player->m_pActiveItem)
		{
			if (Player->m_pActiveItem->m_iId == WEAPON_USP || Player->m_pActiveItem->m_iId == WEAPON_GLOCK18 || Player->m_pActiveItem->m_iId == WEAPON_FAMAS || Player->m_pActiveItem->m_iId == WEAPON_M4A1)
			{
				CBasePlayerWeapon* Weapon = static_cast<CBasePlayerWeapon*>(Player->m_pActiveItem);

				if (Weapon)
				{
					gCSDM_Weapon.SetWeaponState(Player->entindex(), Weapon);
				}
			}
		}
	}
}

void CCSDM_Weapon::ResetPlayer(CBasePlayer* Player)
{
	this->m_Info[Player->entindex()].Reset();
}

void CCSDM_Weapon::EquipItem(CBasePlayer* Player, InventorySlotType SlotType, std::string Item)
{
	if (Player)
	{
		auto AutoWepSwitch = Player->m_iAutoWepSwitch;

		Player->m_iAutoWepSwitch = 1;

		auto PlayerItem = static_cast<CBasePlayerItem*>(Player->CSPlayer()->GiveNamedItemEx(Item.c_str()));

		if (PlayerItem)
		{
			Player->GiveAmmo(PlayerItem->CSPlayerItem()->m_ItemInfo.iMaxAmmo1, const_cast<char*>(PlayerItem->CSPlayerItem()->m_ItemInfo.pszAmmo1), -1);

			if (SlotType == InventorySlotType::PISTOL_SLOT)
			{
				this->m_Info[Player->entindex()].LastSecondary = Item;
			}
			else if (SlotType == InventorySlotType::PRIMARY_WEAPON_SLOT)
			{
				this->m_Info[Player->entindex()].LastPrimary = Item;
			}
		}

		if (Player->m_pActiveItem)
		{
			if (Player->m_pActiveItem->m_iId == WEAPON_USP || Player->m_pActiveItem->m_iId == WEAPON_GLOCK18 || Player->m_pActiveItem->m_iId == WEAPON_FAMAS || Player->m_pActiveItem->m_iId == WEAPON_M4A1)
			{
				auto Weapon = static_cast<CBasePlayerWeapon*>(Player->m_pActiveItem);

				if (Weapon)
				{
					Weapon->m_iWeaponState = this->m_Info[Player->entindex()].WeaponState[Weapon->m_iId];
				}
			}
		}

		Player->m_iAutoWepSwitch = AutoWepSwitch;
	}
}

void CCSDM_Weapon::EquipRandom(CBasePlayer* Player, int Slot)
{
	if (Player)
	{
		if (this->m_Weapons.size())
		{
			do
			{
				auto Item = std::next(std::begin(this->m_Weapons), (std::rand() % this->m_Weapons.size()));

				if (Item->Enabled)
				{
					if (Item->Slot == Slot)
					{
						if (!Item->Bot && Player->IsBot())
						{
							continue;
						}

						this->EquipItem(Player, InventorySlotType::PISTOL_SLOT, Item->Alias);

						break;
					}
				}
			} while (true);
		}
	}
}

bool CCSDM_Weapon::EquipLast(CBasePlayer* Player)
{
	auto Result = false;

	if (!this->m_Info[Player->entindex()].LastSecondary.empty())
	{
		Result = true;

		this->EquipItem(Player, InventorySlotType::PISTOL_SLOT, this->m_Info[Player->entindex()].LastSecondary);
	}

	if (!this->m_Info[Player->entindex()].LastPrimary.empty())
	{
		Result = true;

		this->EquipItem(Player, InventorySlotType::PRIMARY_WEAPON_SLOT, this->m_Info[Player->entindex()].LastPrimary);
	}

	return Result;
}

void CCSDM_Weapon::SetHideMenu(CBasePlayer* Player, bool HideMenu)
{
	if (Player)
	{
		if (HideMenu)
		{
			gCSDM_Util.SayText(Player->edict(), PRINT_TEAM_RED, "Type ^4'/guns'^1 in chat to re-enable your equip menu.");
		}
		else
		{
			gCSDM_Util.SayText(Player->edict(), PRINT_TEAM_BLUE, "Your equip menu has been re-enabled.");
		}

		this->m_Info[Player->entindex()].HideMenu = HideMenu;
	}
}

bool CCSDM_Weapon::GetHideMenu(int EntityIndex)
{
	return this->m_Info[EntityIndex].HideMenu;
}

void CCSDM_Weapon::SetWeaponState(int EntityIndex, CBasePlayerWeapon* Weapon)
{
	if (Weapon)
	{
		this->m_Info[EntityIndex].WeaponState[Weapon->m_iId] = Weapon->m_iWeaponState;
	}
}

int CCSDM_Weapon::GetWeaponState(int EntityIndex, int WeaponIndex)
{
	return this->m_Info[EntityIndex].WeaponState[WeaponIndex];
}

void CCSDM_Weapon::EquipMenu(int EntityIndex)
{
	gCSDM_Menu[EntityIndex].Create("CSDM: Equip", false, (void*)this->EquipMenuHandle);

	gCSDM_Menu[EntityIndex].AddItem("1", "New Weapons", false);

	if (!this->m_Info[EntityIndex].LastSecondary.empty() || !this->m_Info[EntityIndex].LastPrimary.empty())
	{
		gCSDM_Menu[EntityIndex].AddItem("2", "Previous Setup", false);
		gCSDM_Menu[EntityIndex].AddItem("3", "2 + Don't show menu again", false);
	}

	gCSDM_Menu[EntityIndex].Show(EntityIndex);
}

void CCSDM_Weapon::EquipMenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		if (!Item.Disabled)
		{
			switch (std::stoi(Item.Info))
			{
				case 1:
				{
					gCSDM_Weapon.SecondaryMenu(EntityIndex);
					return;
				}
				case 2:
				{
					if (gCSDM_Weapon.EquipLast(Player))
					{
						return;
					}
					break;
				}
				case 3:
				{
					gCSDM_Weapon.SetHideMenu(Player, true);

					if (gCSDM_Weapon.EquipLast(Player))
					{
						return;
					}
					break;
				}
			}
		}

		gCSDM_Weapon.EquipMenu(Player->entindex());
	}
}

void CCSDM_Weapon::SecondaryMenu(int EntityIndex)
{
	gCSDM_Menu[EntityIndex].Create("CSDM: Secondary Weapons", false, (void*)this->SecondaryMenuHandle);

	for (auto const& Weapon : this->m_Weapons)
	{
		if (Weapon.Slot == 2)
		{
			gCSDM_Menu[EntityIndex].AddItem(Weapon.Alias, Weapon.Label, Weapon.Enabled ? false : true);
		}
	}

	gCSDM_Menu[EntityIndex].Show(EntityIndex);
}

void CCSDM_Weapon::SecondaryMenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		if (!Item.Disabled)
		{
			gCSDM_Weapon.EquipItem(Player, InventorySlotType::PISTOL_SLOT, Item.Info);

			gCSDM_Weapon.PrimaryMenu(EntityIndex);
		}
	}
}

void CCSDM_Weapon::PrimaryMenu(int EntityIndex)
{
	gCSDM_Menu[EntityIndex].Create("CSDM: Primary Weapons", false, (void*)this->PrimaryMenuHandle);

	for (auto const& Weapon : this->m_Weapons)
	{
		if (Weapon.Slot == 1)
		{
			gCSDM_Menu[EntityIndex].AddItem(Weapon.Alias, Weapon.Label, Weapon.Enabled ? false : true);
		}
	}

	gCSDM_Menu[EntityIndex].Show(EntityIndex);
}

void CCSDM_Weapon::PrimaryMenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		if (!Item.Disabled)
		{
			gCSDM_Weapon.EquipItem(Player, InventorySlotType::PRIMARY_WEAPON_SLOT, Item.Info);
		}
		else
		{
			gCSDM_Weapon.PrimaryMenu(EntityIndex);
		}
	}
}
