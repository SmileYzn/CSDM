#include "precompiled.h"

CCSDM_Misc gCSDM_Misc;

void CCSDM_Misc::ServerActivate()
{
	this->m_hs_only_mode = gCSDM_Util.CvarRegister("csdm_hs_only_mode", "0");

	this->m_kill_fade = gCSDM_Util.CvarRegister("csdm_kill_fade", "2");

	this->m_kill_sound = gCSDM_Util.CvarRegister("csdm_kill_sound", "0");

	this->m_kill_hp = gCSDM_Util.CvarRegister("csdm_kill_hp", "15");

	this->m_kill_hp_hs = gCSDM_Util.CvarRegister("csdm_kill_hp_hs", "40");

	this->m_kill_repair_armor = gCSDM_Util.CvarRegister("csdm_kill_repair_armor", "1");

	this->m_kill_msg = gCSDM_Util.CvarRegister("csdm_kill_hp_msg", "1");

	this->m_hit_indicator = gCSDM_Util.CvarRegister("csdm_hit_indicator", "0");

	this->m_hide_kill_feed = gCSDM_Util.CvarRegister("csdm_hide_kill_feed", "0");

	this->m_money_as_frags = gCSDM_Util.CvarRegister("csdm_money_as_frags", "0");

	this->m_kd_ratio_hud = gCSDM_Util.CvarRegister("csdm_hud_kd_ratio", "1");

	this->m_reload_on_kill = gCSDM_Util.CvarRegister("csdm_reload_on_kill", "0");
}

void CCSDM_Misc::ServerDeactivate()
{
	//
}

void CCSDM_Misc::Enable()
{
	if (!this->m_Enabled)
	{
		if (gReGameDLL.m_Hookchains)
		{
			gReGameDLL.m_Hookchains->CBasePlayer_GetIntoGame()->registerHook(this->CBasePlayer_GetIntoGame);

			gReGameDLL.m_Hookchains->CBasePlayer_UpdateClientData()->registerHook(this->CBasePlayer_UpdateClientData);

			gReGameDLL.m_Hookchains->CBasePlayer_TakeDamage()->registerHook(this->CBasePlayer_TakeDamage);

			gReGameDLL.m_Hookchains->CSGameRules_PlayerKilled()->registerHook(this->CSGameRules_PlayerKilled);

			gReGameDLL.m_Hookchains->CBasePlayer_AddAccount()->registerHook(this->CBasePlayer_AddAccount);

			gReGameDLL.m_Hookchains->CSGameRules_SendDeathMessage()->registerHook(this->CSGameRules_SendDeathMessage);
		}

		this->m_Enabled = true;
	}
}

void CCSDM_Misc::Disable()
{
	if (this->m_Enabled)
	{
		if (gReGameDLL.m_Hookchains)
		{
			gReGameDLL.m_Hookchains->CBasePlayer_GetIntoGame()->unregisterHook(this->CBasePlayer_GetIntoGame);

			gReGameDLL.m_Hookchains->CBasePlayer_UpdateClientData()->unregisterHook(this->CBasePlayer_UpdateClientData);

			gReGameDLL.m_Hookchains->CBasePlayer_TakeDamage()->unregisterHook(this->CBasePlayer_TakeDamage);

			gReGameDLL.m_Hookchains->CSGameRules_PlayerKilled()->unregisterHook(this->CSGameRules_PlayerKilled);

			gReGameDLL.m_Hookchains->CBasePlayer_AddAccount()->unregisterHook(this->CBasePlayer_AddAccount);

			gReGameDLL.m_Hookchains->CSGameRules_SendDeathMessage()->unregisterHook(this->CSGameRules_SendDeathMessage);
		}

		this->m_Enabled = false;
	}
}

void CCSDM_Misc::TakeDamage(CBasePlayer* Victim, CBasePlayer* Attacker, float flDamage, int bitsDamageType)
{
	if (Victim->entindex() != Attacker->entindex())
	{
		if (Victim->IsPlayer() && Attacker->IsPlayer())
		{
			if (gReGameDLL.m_Rules)
			{
				if (gReGameDLL.m_Rules->FPlayerCanTakeDamage(Victim, Attacker))
				{
					if (this->m_hit_indicator->value > 0.0f)
					{
						switch (static_cast<int>(this->m_hit_indicator->value))
						{
							case 1:
							{
								gCSDM_Util.ShowHudMessage(Attacker->edict(), gCSDM_Util.SetHudParam(0, 255, 0, -1.0f, 0.55f, 0, 0.75f, 0.75f, 0.0f, 0.0f, 3), "*");
								break;
							}
							case 2:
							{
								gCSDM_Util.ShowHudMessage(Attacker->edict(), gCSDM_Util.SetHudParam(0, 255, 0, -1.0f, -1.0f, 0, 0.75f, 0.75f, 0.0f, 0.0f, 3), ">\x20\x20\x20\x20\x20\x20\x20\x20\x20<");
								break;
							}
							case 3:
							{
								gCSDM_Util.ShowHudMessage(Attacker->edict(), gCSDM_Util.SetHudParam(0, 255, 0, -1.0f, 0.55f, 0, 0.75f, 0.75f, 0.0f, 0.0f, 3), "%d", static_cast<int>(flDamage));
								break;
							}
							case 4:
							{
								gCSDM_Util.ShowHudMessage(Attacker->edict(), gCSDM_Util.SetHudParam(0, 255, 0, -1.0f, 0.55f, 0, 0.75f, 0.75f, 0.0f, 0.0f, 3), this->m_HitGroup[Victim->m_LastHitGroup]);
								break;
							}
						}
					}
				}
			}
		}
	}
}

void CCSDM_Misc::PlayerKilled(CBasePlayer* Victim, CBasePlayer* Killer)
{
	if (Victim->entindex() != Killer->entindex())
	{
		if (Victim->IsPlayer() && Killer->IsPlayer())
		{
			auto RestoreHealth = this->m_kill_hp->value;

			auto HeadshotCheck = 1.0f;

			if (Victim->m_bHeadshotKilled)
			{
				this->m_Headshots[Killer->entindex()]++;

				HeadshotCheck = 2.0f;

				if (this->m_kill_hp_hs->value > 0.0f)
				{
					RestoreHealth = this->m_kill_hp_hs->value;
				}
			}

			if (RestoreHealth > 0.0f)
			{
				Killer->edict()->v.health = clamp(Killer->edict()->v.health + RestoreHealth, 1.0f, 100.0f);
			}

			if (this->m_kill_repair_armor->value >= HeadshotCheck)
			{
				Killer->edict()->v.armorvalue = 100.0f;
			}

			if (this->m_kill_fade->value >= HeadshotCheck)
			{
				gCSDM_Util.ScreenFade(Killer->edict(), BIT(10), BIT(10), 0x0000, 0, 0, 200, 75);
			}

			if (this->m_kill_sound->value >= HeadshotCheck)
			{
				g_engfuncs.pfnClientCommand(Killer->edict(), "%s", "speak \"sound/fvox/blip.wav\"\n");
			}

			if (this->m_reload_on_kill->value >= HeadshotCheck)
			{
				if (Killer->m_pActiveItem)
				{
					if (Killer->m_pActiveItem->iItemSlot() == PRIMARY_WEAPON_SLOT || Killer->m_pActiveItem->iItemSlot() == PISTOL_SLOT)
					{
						Killer->CSPlayer()->ReloadWeapons(Killer->m_pActiveItem, true, true);
					}
				}
			}

			if (this->m_kill_msg->value)
			{
				auto Value = (int)(Killer->edict()->v.health - (float)(Killer->m_iLastClientHealth));

				if (Value > 0)
				{
					gCSDM_Util.ShowHudMessage(Killer->edict(), gCSDM_Util.SetHudParam(0, 255, 0, -1.0f, 0.8f, 0, 1.0f, 1.0f, 0.0f, 0.0f, 1), "+%d HP", Value);
				}
			}
		}
	}
}

void CCSDM_Misc::UpdateClientData(CBasePlayer* Player, bool Reset)
{
	if (Player->IsPlayer())
	{
		if (Reset || (Player->edict()->v.frags == 0.0f))
		{
			this->m_Headshots[Player->entindex()] = 0;
		}

		if (!Player->IsBot())
		{
			if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
			{
				if (this->m_kd_ratio_hud->value)
				{
					static float NextTime;

					if (gpGlobals->time > NextTime)
					{
						auto KD_Ratio = 0.0f;

						auto HS_Ratio = 0.0f;

						if (Player->edict()->v.frags > 0.0f)
						{
							KD_Ratio = (Player->m_iDeaths > 0) ? (1.0f * (Player->edict()->v.frags / static_cast<float>(Player->m_iDeaths))) : (1.0f * Player->edict()->v.frags);

							HS_Ratio = 100.0f * (static_cast<float>(this->m_Headshots[Player->entindex()]) / Player->edict()->v.frags);
						}

						gCSDM_Util.ShowHudMessage(Player->edict(), gCSDM_Util.SetHudParam(0, 255, 0, -1.0f, 0.01f, 0, 2.1f, 2.1f, 0.0f, 0.0f, 2), "K/D: %.2f - HS: %.2f%%", KD_Ratio, HS_Ratio);

						NextTime = (gpGlobals->time + this->m_kd_ratio_hud->value);
					}
				}
			}
		}
	}
}

int CCSDM_Misc::AddAccount(CBasePlayer* Player, int Amount)
{
	if (this->m_money_as_frags->value > 0.0f)
	{
		Player->m_iAccount = 0;

		return static_cast<int>(Player->edict()->v.frags);
	}

	return Amount;
}

bool CCSDM_Misc::SendDeathMessage(CBaseEntity* Killer, CBasePlayer* Victim, CBasePlayer* Assister, entvars_t* pevInflictor, const char* killerWeaponName, int iDeathMessageFlags, int iRarityOfKill)
{
	if (this->m_hide_kill_feed)
	{
		if (this->m_hide_kill_feed->value > 0.0f)
		{
			if (Killer)
			{
				if (Killer->IsPlayer())
				{
					auto Player = static_cast<CBasePlayer*>(Killer);

					if (Player)
					{
						gCSDM_Util.DeathMsg(Player->edict(), Player->entindex(), Victim->entindex(), Victim->m_bHeadshotKilled ? 1 : 0, killerWeaponName);

						if (Player->entindex() != Victim->entindex())
						{
							gCSDM_Util.DeathMsg(Victim->edict(), Player->entindex(), Victim->entindex(), Victim->m_bHeadshotKilled ? 1 : 0, killerWeaponName);
						}

						return true;
					}
				}
			}
		}
	}

	return false;
}

bool CCSDM_Misc::CBasePlayer_GetIntoGame(IReGameHook_CBasePlayer_GetIntoGame* chain, CBasePlayer* Player)
{
	auto Result = chain->callNext(Player);

	gCSDM_Misc.UpdateClientData(Player, true);

	return Result;
}

void CCSDM_Misc::CBasePlayer_UpdateClientData(IReGameHook_CBasePlayer_UpdateClientData* chain, CBasePlayer* Player)
{
	chain->callNext(Player);

	gCSDM_Misc.UpdateClientData(Player, false);
}

BOOL CCSDM_Misc::CBasePlayer_TakeDamage(IReGameHook_CBasePlayer_TakeDamage* chain, CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType)
{
	auto Result = chain->callNext(Player, pevInflictor, pevAttacker, flDamage, bitsDamageType);

	auto Attacker = UTIL_PlayerByIndexSafe(ENTINDEX(pevAttacker));

	if (Attacker)
	{
		gCSDM_Misc.TakeDamage(Player, Attacker, flDamage, bitsDamageType);
	}

	return Result;
}

void CCSDM_Misc::CSGameRules_PlayerKilled(IReGameHook_CSGameRules_PlayerKilled* chain, CBasePlayer* Victim, entvars_t* pevKiller, entvars_t* pevInflictor)
{
	chain->callNext(Victim, pevKiller, pevInflictor);

	auto Killer = UTIL_PlayerByIndexSafe(ENTINDEX(pevKiller));

	if (Killer)
	{
		gCSDM_Misc.PlayerKilled(Victim, Killer);
	}
}

void CCSDM_Misc::CBasePlayer_AddAccount(IReGameHook_CBasePlayer_AddAccount* chain, CBasePlayer* Player, int amount, RewardType type, bool bTrackChange)
{
	amount = gCSDM_Misc.AddAccount(Player, amount);

	chain->callNext(Player, amount, type, bTrackChange);
}

void CCSDM_Misc::CSGameRules_SendDeathMessage(IReGameHook_CSGameRules_SendDeathMessage* chain, CBaseEntity* Killer, CBasePlayer* Victim, CBasePlayer* Assister, entvars_t* pevInflictor, const char* killerWeaponName, int iDeathMessageFlags, int iRarityOfKill)
{
	if (!gCSDM_Misc.SendDeathMessage(Killer, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill))
	{
		chain->callNext(Killer, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill);
	}
}

bool CCSDM_Misc::TraceLine(const float* start, const float* end, int fNoMonsters, edict_t* pentToSkip, TraceResult* ptr)
{
	if(this->m_hs_only_mode)
	{
		if (this->m_hs_only_mode->value)
		{
			g_engfuncs.pfnTraceLine(start, end, fNoMonsters, pentToSkip, ptr);

			if (pentToSkip)
			{
				if (ptr)
				{
					if (ptr->pHit && (ptr->pHit->v.flags & (FL_CLIENT | FL_FAKECLIENT)) && pentToSkip && (pentToSkip->v.flags & (FL_CLIENT | FL_FAKECLIENT)))
					{
						if (!(BIT(HITGROUP_HEAD) & (1 << ptr->iHitgroup)))
						{
							ptr->flFraction = 1.0;

							return true;
						}
					}
				}
			}
		}
	}

	return false;
}
