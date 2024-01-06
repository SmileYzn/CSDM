#include "precompiled.h"

CCSDM_SpawnEdit gCSDM_SpawnEdit;

void CCSDM_SpawnEdit::ServerActivate()
{
	this->m_Spawns.clear();

	this->m_Entities.clear();

	this->m_Marked.clear();

	this->m_Admin = gCSDM_Util.CvarRegister("csdm_spawn_edit_admin", "STEAM_0:1:20326844");

	this->m_InfoTarget = g_engfuncs.pfnAllocString("info_target");
}

void CCSDM_SpawnEdit::ServerDeactivate()
{
	/**/
}

void CCSDM_SpawnEdit::EditSpawns(CBasePlayer* Player)
{

	if (this->m_Spawns.empty())
	{
		this->m_Spawns = gCSDM_Spawn.GetSpawns();
	}
	
	this->MakeEntity(-1);

	this->Menu(Player);
}

void CCSDM_SpawnEdit::Menu(CBasePlayer* Player)
{
	if (!Q_strcmp(this->m_Admin->string, g_engfuncs.pfnGetPlayerAuthId(Player->edict())))
	{
		gCSDM_Menu[Player->entindex()].Create("CSDM: Spawn Manager", false, this->MenuHandle);

		gCSDM_Menu[Player->entindex()].AddItem("1", "Add current position to Spawn", false);

		if (this->m_Spawns.empty())
		{
			gCSDM_Menu[Player->entindex()].AddItem("2", "Edit Spawn - No spawns", true);
			gCSDM_Menu[Player->entindex()].AddItem("3", "Delete Spawn - No spawns", true);
		}
		else if (FNullEnt(this->m_Entities[this->m_Marked[Player->entindex()]]))
		{
			gCSDM_Menu[Player->entindex()].AddItem("2", "Edit Spawn - No spawn marked", true);
			gCSDM_Menu[Player->entindex()].AddItem("3", "Delete Spawn - No spawn marked", true);
		}
		else
		{
			gCSDM_Menu[Player->entindex()].AddItem("2", "Edit closest spawn (^yyellow^w) to Current Position", false);

			if ((Player->edict()->v.origin - this->m_Entities[this->m_Marked[Player->entindex()]]->v.origin).Length() > 200.0f)
			{
				gCSDM_Menu[Player->entindex()].AddItem("3", "Delete Spawn - Marked spawn far away", true);
			}
			else
			{
				gCSDM_Menu[Player->entindex()].AddItem("3", "Delete closest Spawn", false);
			}
		}

		gCSDM_Menu[Player->entindex()].AddItem("4", "Mark closest Spawn", false);

		gCSDM_Menu[Player->entindex()].AddItem("5", "Show Statistics", false);

		if (Player->edict()->v.movetype == MOVETYPE_WALK)
		{
			gCSDM_Menu[Player->entindex()].AddItem("6", "Enable No Clip", false);
		}
		else if(Player->edict()->v.movetype == MOVETYPE_NOCLIP)
		{
			gCSDM_Menu[Player->entindex()].AddItem("6", "Disable No Clip", false);
		}

		gCSDM_Menu[Player->entindex()].AddItem("7", "Discard all changes, and exit", false);

		gCSDM_Menu[Player->entindex()].AddItem("8", "Save all changes", false);

		gCSDM_Menu[Player->entindex()].Show(Player->entindex());
	}
}

void CCSDM_SpawnEdit::MenuHandle(int EntityIndex, P_MENU_ITEM Item)
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
					gCSDM_SpawnEdit.AddSpawnMenu(Player);
					break;
				}
				case 2:
				{
					gCSDM_SpawnEdit.EditSpawn(EntityIndex, Player->edict()->v, 15.0f);
					gCSDM_SpawnEdit.Menu(Player);
					break;
				}
				case 3:
				{
					gCSDM_SpawnEdit.DeleteSpawn(EntityIndex);
					gCSDM_SpawnEdit.Menu(Player);
					break;
				}
				case 4:
				{
					gCSDM_SpawnEdit.Refresh(Player);
					gCSDM_SpawnEdit.Menu(Player);
					break;
				}
				case 5:
				{
					gCSDM_SpawnEdit.ShowStats(Player);
					gCSDM_SpawnEdit.Menu(Player);
					break;
				}
				case 6:
				{
					if (Player->edict()->v.movetype == MOVETYPE_WALK)
					{
						Player->edict()->v.movetype = MOVETYPE_NOCLIP;

						gCSDM_Util.SayText(Player->edict(), PRINT_TEAM_BLUE, "^4No clip Enabled.");
					}
					else if (Player->edict()->v.movetype == MOVETYPE_NOCLIP)
					{
						Player->edict()->v.movetype = MOVETYPE_WALK;

						gCSDM_Util.SayText(Player->edict(), PRINT_TEAM_RED, "^4No clip Disabled.");
					}
					gCSDM_SpawnEdit.Menu(Player);
					break;
				}
				case 7:
				{
					gCSDM_SpawnEdit.DiscardChanges(Player);
					break;
				}
				case 8:
				{
					gCSDM_SpawnEdit.Save(Player);

					gCSDM_SpawnEdit.Menu(Player);
					break;
				}
			}
		}
	}
}

void CCSDM_SpawnEdit::AddSpawnMenu(CBasePlayer* Player)
{
	gCSDM_Menu[Player->entindex()].Create("CSDM: Add Spawns Menu", false, this->AddSpawnMenuHandle);

	gCSDM_Menu[Player->entindex()].AddItem("1", "Add Current Postion as a random spawn", false);
	gCSDM_Menu[Player->entindex()].AddItem("2", "Add Current Postion as a T spawn", false);
	gCSDM_Menu[Player->entindex()].AddItem("3", "Add Current Postion as a CT spawn", false);
	gCSDM_Menu[Player->entindex()].AddItem("4", "Back", false);

	gCSDM_Menu[Player->entindex()].Show(Player->entindex());
}

void CCSDM_SpawnEdit::AddSpawnMenuHandle(int EntityIndex, P_MENU_ITEM Item)
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
					gCSDM_SpawnEdit.AddSpawn(Player->edict()->v, UNASSIGNED, 15.0f);
					gCSDM_SpawnEdit.AddSpawnMenu(Player);
					break;
				}
				case 2:
				{
					gCSDM_SpawnEdit.AddSpawn(Player->edict()->v, TERRORIST, 15.0f);
					gCSDM_SpawnEdit.AddSpawnMenu(Player);
					break;
				}
				case 3:
				{
					gCSDM_SpawnEdit.AddSpawn(Player->edict()->v, CT, 15.0f);
					gCSDM_SpawnEdit.AddSpawnMenu(Player);
					break;
				}
				case 4:
				{
					gCSDM_SpawnEdit.Menu(Player);
					break;
				}
			}
		}
	}
}

void CCSDM_SpawnEdit::DiscardChanges(CBasePlayer* Player)
{
	gCSDM_Menu[Player->entindex()].Create("You want to discard all changes and exit editor?", false, this->DiscardChangesHandle);

	gCSDM_Menu[Player->entindex()].AddItem("1", "No, continue editing", false);

	gCSDM_Menu[Player->entindex()].AddItem("2", "Yes, discard all changes and close editor", false);

	gCSDM_Util.SayText(Player->edict(), PRINT_TEAM_RED, "^3You want to discard all changes and exit editor?");

	gCSDM_Menu[Player->entindex()].Show(Player->entindex());
}

void CCSDM_SpawnEdit::DiscardChangesHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		switch (std::stoi(Item.Info))
		{
			case 1:
			{
				gCSDM_SpawnEdit.Menu(Player);
				break;
			}
			case 2:
			{
				gCSDM_SpawnEdit.RemoveEntity(-1);

				gCSDM_Util.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "All changes have been discarded. Use ^3csdm_edit_spawns^1 to open editor again.");
				break;
			}
		}
	}
}

edict_t* CCSDM_SpawnEdit::MakeEntity(int EntityIndex)
{
	edict_t* pEntity = nullptr;

	if (EntityIndex < 0)
	{
		for (auto const& Spawn : this->m_Spawns)
		{
			pEntity = g_engfuncs.pfnCreateNamedEntity(this->m_InfoTarget);

			if (!FNullEnt(pEntity))
			{
				pEntity->v.classname = g_engfuncs.pfnAllocString("view_spawn");

				switch (Spawn.second.Team)
				{
					case 0:
					{
						g_engfuncs.pfnSetModel(pEntity, "models/player/vip/vip.mdl");
						break;
					}
					case 1:
					{
						g_engfuncs.pfnSetModel(pEntity, "models/player/terror/terror.mdl");
						break;
					}
					case 2:
					{
						g_engfuncs.pfnSetModel(pEntity, "models/player/urban/urban.mdl");
						break;
					}
				}

				pEntity->v.solid = SOLID_SLIDEBOX;
				pEntity->v.movetype = MOVETYPE_NOCLIP;
				pEntity->v.flags = (pEntity->v.flags & FL_ONGROUND);
				pEntity->v.sequence = 1;

				if (this->m_Entities[Spawn.first])
				{
					g_engfuncs.pfnRemoveEntity(this->m_Entities[Spawn.first]);
				}

				this->m_Entities[Spawn.first] = pEntity;

				this->UnGlowEnt(Spawn.first);
			}
		}
	}
	else
	{
		pEntity = g_engfuncs.pfnCreateNamedEntity(this->m_InfoTarget);

		if (!FNullEnt(pEntity))
		{
			pEntity->v.classname = g_engfuncs.pfnAllocString("view_spawn");

			g_engfuncs.pfnSetModel(pEntity, "models/player/vip/vip.mdl");

			switch (this->m_Spawns[EntityIndex].Team)
			{
				case 0:
				{
					g_engfuncs.pfnSetModel(pEntity, "models/player/vip/vip.mdl");
					break;
				}
				case 1:
				{
					g_engfuncs.pfnSetModel(pEntity, "models/player/terror/terror.mdl");
					break;
				}
				case 2:
				{
					g_engfuncs.pfnSetModel(pEntity, "models/player/urban/urban.mdl");
					break;
				}
			}

			pEntity->v.solid = SOLID_SLIDEBOX;
			pEntity->v.movetype = MOVETYPE_NOCLIP;
			pEntity->v.flags = (pEntity->v.flags & FL_ONGROUND);
			pEntity->v.sequence = 1;

			if (this->m_Entities[EntityIndex])
			{
				g_engfuncs.pfnRemoveEntity(this->m_Entities[EntityIndex]);
			}

			this->m_Entities[EntityIndex] = pEntity;

			this->UnGlowEnt(EntityIndex);
		}
	}

	return pEntity;
}

void CCSDM_SpawnEdit::RemoveEntity(int EntityIndex)
{
	if (EntityIndex < 0)
	{
		for (auto const& Spawn : this->m_Spawns)
		{
			if (!FNullEnt(this->m_Entities[Spawn.first]))
			{
				g_engfuncs.pfnRemoveEntity(this->m_Entities[Spawn.first]);
			}
		}

		this->m_Spawns.clear();

		this->m_Entities.clear();

		this->m_Marked.clear();
	}
	else
	{
		if (!FNullEnt(this->m_Entities[EntityIndex]))
		{
			g_engfuncs.pfnRemoveEntity(this->m_Entities[EntityIndex]);
		}

		this->m_Spawns.erase(EntityIndex);

		this->m_Entities[EntityIndex] = nullptr;

		this->m_Marked[EntityIndex] = -1;
	}
}

void CCSDM_SpawnEdit::GlowEnt(int EntityIndex, vec3_t Color)
{
	auto pEntity = this->m_Entities[EntityIndex];

	if (!FNullEnt(pEntity))
	{
		this->SetPosition(EntityIndex);

		pEntity->v.renderfx = kRenderFxGlowShell;

		pEntity->v.renderamt = 127.0f;

		pEntity->v.rendermode = kRenderTransAlpha;

		pEntity->v.rendercolor = Color;
	}
}

void CCSDM_SpawnEdit::UnGlowEnt(int EntityIndex)
{
	auto pEntity = this->m_Entities[EntityIndex];

	if (!FNullEnt(pEntity))
	{
		this->SetPosition(EntityIndex);

		pEntity->v.renderfx = kRenderFxNone;

		pEntity->v.renderamt = 255.0f;

		pEntity->v.rendermode = kRenderTransAlpha;
	}
}

void CCSDM_SpawnEdit::SetPosition(int EntityIndex)
{
	auto pEntity = this->m_Entities[EntityIndex];

	if (!FNullEnt(pEntity))
	{
		pEntity->v.origin = this->m_Spawns[EntityIndex].Vecs;

		pEntity->v.angles = this->m_Spawns[EntityIndex].Angles;

		pEntity->v.v_angle = this->m_Spawns[EntityIndex].VAngles;

		pEntity->v.fixangle = 1;
	}
}

void CCSDM_SpawnEdit::AddSpawn(entvars_t v, int Team, float FixOrigin)
{
	P_SPAWN_POINT Info;

	Q_memset(&Info, 0, sizeof(Info));

	if (FixOrigin != 0.0f)
	{
		v.origin[2] += FixOrigin;
	}

	Info.Vecs = v.origin;

	Info.Angles = v.angles;

	Info.Team = Team;

	Info.VAngles = v.v_angle;

	auto Index = this->m_Spawns.size();

	this->m_Spawns[Index] = Info;

	this->MakeEntity(Index);
}

void CCSDM_SpawnEdit::EditSpawn(int EntityIndex, entvars_t v, float FixOrigin)
{
	auto pEntity = this->m_Marked[EntityIndex];

	if (!FNullEnt(pEntity))
	{
		for (auto const& Spawn : this->m_Spawns)
		{
			if ((this->m_Spawns[EntityIndex].Vecs[0] == Spawn.second.Vecs[0]) && (this->m_Spawns[EntityIndex].Vecs[1] == Spawn.second.Vecs[1]) && ((this->m_Spawns[EntityIndex].Vecs[2] - Spawn.second.Vecs[2]) <= 15.0f))
			{
				this->RemoveEntity(this->m_Marked[EntityIndex]);

				if (FixOrigin != 0.0f)
				{
					v.origin[2] += FixOrigin;
				}

				this->m_Spawns[this->m_Marked[EntityIndex]].Vecs = v.origin;

				this->m_Spawns[this->m_Marked[EntityIndex]].Angles = v.angles;

				this->m_Spawns[this->m_Marked[EntityIndex]].Team = 0;

				this->m_Spawns[this->m_Marked[EntityIndex]].VAngles = v.v_angle;

				this->MakeEntity(this->m_Marked[EntityIndex]);

				this->GlowEnt(this->m_Marked[EntityIndex], Vector(255.0f, 0.0f, 0.0f));

				break;
			}
		}
	}
}

void CCSDM_SpawnEdit::DeleteSpawn(int EntityIndex)
{
	this->UnGlowEnt(this->m_Marked[EntityIndex]);

	this->RemoveEntity(this->m_Marked[EntityIndex]);

	auto it = this->m_Spawns.find(this->m_Marked[EntityIndex]);

	if (it != this->m_Spawns.end())
	{
		this->m_Spawns.erase(it);
	}

	this->MakeEntity(-1);

	this->m_Marked[EntityIndex] = -1;
}

int CCSDM_SpawnEdit::ClosestSpawn(edict_t* pEdict)
{
	auto Result = 0;

	if (!FNullEnt(pEdict))
	{
		auto LastDistance = 9999.0f;

		for (auto const& Spawn : this->m_Spawns)
		{
			auto Distance = (pEdict->v.origin - Spawn.second.Vecs).Length();

			if (Distance < LastDistance)
			{
				LastDistance = Distance;

				Result = Spawn.first;
			}
		}
	}

	return Result;
}

void CCSDM_SpawnEdit::Refresh(CBasePlayer* Player)
{
	this->UnGlowEnt(this->m_Marked[Player->entindex()]);

	this->m_Marked[Player->entindex()] = this->ClosestSpawn(Player->edict());

	this->GlowEnt(this->m_Marked[Player->entindex()], Vector(255.0f, 200.0f, 20.0f));

	auto Spawn = this->m_Spawns.find(this->m_Marked[Player->entindex()]);

	if (Spawn != this->m_Spawns.end())
	{
		gCSDM_Util.SayText
		(
			Player->edict(), PRINT_TEAM_DEFAULT, "The closest spawn: number ^4%d^1, Team = ^4%s",
			Spawn->first,
			(Spawn->second.Team == UNASSIGNED) ? "Random" : (Spawn->second.Team == CT ? "CT" : "T")
		);

		gCSDM_Util.SayText
		(
			Player->edict(), PRINT_TEAM_DEFAULT, "Origin [%.0f %.0f %.0f] Angle [%.0f %.0f %.0f] VAngle [%.0f %.0f %.0f]",
			Spawn->second.Vecs[0],
			Spawn->second.Vecs[1],
			Spawn->second.Vecs[2],
			Spawn->second.Angles[0],
			Spawn->second.Angles[1],
			Spawn->second.Angles[2],
			Spawn->second.VAngles[0],
			Spawn->second.VAngles[1],
			Spawn->second.VAngles[2]
		);
	}
}

void CCSDM_SpawnEdit::ShowStats(CBasePlayer* Player)
{
	auto TotalSpawns = 0;
	auto TotalRandom = 0;
	auto TotalTR = 0;
	auto TotalCT = 0;

	for (auto const& Spawn : this->m_Spawns)
	{
		TotalSpawns++;

		switch (Spawn.second.Team)
		{
			case UNASSIGNED:
			{
				TotalRandom++;
				break;
			}
			case TERRORIST:
			{
				TotalTR++;
				break;
			}
			case CT:
			{
				TotalCT++;
				break;
			}
		}
	}

	gCSDM_Util.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "Spawns: %d; Random: %d; T: %d; CT: %d", TotalSpawns, TotalRandom, TotalTR, TotalCT);
	gCSDM_Util.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "Current Origin: X: %.3f Y: %.3f Z: %.3f", Player->edict()->v.origin.x, Player->edict()->v.origin.y, Player->edict()->v.origin.z);
}

void CCSDM_SpawnEdit::Save(CBasePlayer* Player)
{
	if (!this->m_Spawns.empty())
	{
		char Path[MAX_PATH] = { 0 };

		Q_sprintf(Path, "%s/spawns/%s.txt", gCSDM_Config.GetPath().c_str(), STRING(gpGlobals->mapname));

		std::ofstream File(Path);

		if (File.is_open())
		{
			for (auto const& Spawn : this->m_Spawns)
			{
				File << static_cast<int>(Spawn.second.Vecs[0]) << "\t" << static_cast<int>(Spawn.second.Vecs[1]) << "\t" << static_cast<int>(Spawn.second.Vecs[2]) << "\t";
				File << static_cast<int>(Spawn.second.Angles[0]) << "\t" << static_cast<int>(Spawn.second.Angles[1]) << "\t" << static_cast<int>(Spawn.second.Angles[2]) << "\t";
				File << Spawn.second.Team << "\t";
				File << static_cast<int>(Spawn.second.VAngles[0]) << "\t" << static_cast<int>(Spawn.second.VAngles[1]) << "\t" << static_cast<int>(Spawn.second.VAngles[2]);
				File << "\n";
			}

			File.close();
			
			gCSDM_Util.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^3%u^1 Spawns saved to file: ^3%s.txt", this->m_Spawns.size(), STRING(gpGlobals->mapname));
		}
		else
		{
			gCSDM_Util.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "Failed to open: ^3%s.txt", STRING(gpGlobals->mapname));
		}
	}
	else
	{
		gCSDM_Util.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "Cannot save to file, spawns is empty..");
	}	
}
