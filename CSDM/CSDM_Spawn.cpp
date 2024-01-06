#include "precompiled.h"

CCSDM_Spawn gCSDM_Spawn;

void CCSDM_Spawn::ServerActivate()
{
    this->m_Spawns.clear();

    char Path[MAX_PATH] = { 0 };

    Q_sprintf(Path, "%s/spawns/%s.txt", gCSDM_Config.GetPath().c_str(), STRING(gpGlobals->mapname));

    std::ifstream File(Path);

    if (File)
    {
        auto LineCount = 1;

        std::string Line = "";

        P_SPAWN_POINT Info;

        while (std::getline(File, Line))
        {
            std::istringstream Row(Line);

            if (!(Row >> Info.Vecs[0] >> Info.Vecs[1] >> Info.Vecs[2] >> Info.Angles[0] >> Info.Angles[1] >> Info.Angles[2] >> Info.Team >> Info.VAngles[0] >> Info.VAngles[1] >> Info.VAngles[2]))
            {
                gpMetaUtilFuncs->pfnLogConsole(&Plugin_info, "[%s] Line %d of the %s spawns file is incorrect.", &Plugin_info.logtag, LineCount, STRING(gpGlobals->mapname));
            }

            this->m_Spawns.insert(std::make_pair(this->m_Spawns.size(), Info));

            LineCount++;
        }

        File.close();
    }
    else
    {
        gpMetaUtilFuncs->pfnLogConsole(&Plugin_info, "[%s] Failed to read file: %s", __func__, Path);
    }
}

std::map<size_t, P_SPAWN_POINT> CCSDM_Spawn::GetSpawns()
{
    return this->m_Spawns;
}

void CCSDM_Spawn::Enable()
{
    if (!this->m_Enabled)
    {
        if (gReGameDLL.m_Hookchains)
        {
            gReGameDLL.m_Hookchains->CSGameRules_GetPlayerSpawnSpot()->registerHook(this->CSGameRules_GetPlayerSpawnSpot);
        }

        this->m_Enabled = true;
    }
}

void CCSDM_Spawn::Disable()
{
    if (this->m_Enabled)
    {
        if (gReGameDLL.m_Hookchains)
        {
            gReGameDLL.m_Hookchains->CSGameRules_GetPlayerSpawnSpot()->unregisterHook(this->CSGameRules_GetPlayerSpawnSpot);
        }

        this->m_Enabled = false;
    }
}

edict_t* CCSDM_Spawn::CSGameRules_GetPlayerSpawnSpot(IReGameHook_CSGameRules_GetPlayerSpawnSpot* chain, CBasePlayer* Player)
{
    auto Result = chain->callNext(Player);

    if (gCSDM_Spawn.SetPlayerPosition(Player))
    {
        return nullptr;
    }

    return Result;
}

bool CCSDM_Spawn::SetPlayerPosition(CBasePlayer* Player)
{
    if (this->m_Spawns.size())
    {
        do
        {
            auto Spawn = std::next(std::begin(this->m_Spawns), (std::rand() % this->m_Spawns.size()));

            if (!Spawn->second.Vecs.IsZero())
            {
                if (this->CheckDistance(Player, Spawn->second.Vecs, SPAWN_POINT_MIN_DISTANCE))
                {
                    Player->edict()->v.origin = Spawn->second.Vecs + Vector(0.0f, 0.0f, 1.0f);

                    Player->edict()->v.angles = Spawn->second.Angles;

                    Player->edict()->v.v_angle = Spawn->second.VAngles;

                    Player->edict()->v.v_angle.z = 0;

                    Player->edict()->v.angles = Player->edict()->v.v_angle;

                    Player->edict()->v.velocity = Vector(0.0f, 0.0f, 0.0f);

                    Player->edict()->v.punchangle = Vector(0.0f, 0.0f, 0.0f);

                    Player->edict()->v.fixangle = 1;

                    Player->m_bloodColor = BLOOD_COLOR_RED;

                    Player->m_modelIndexPlayer = Player->edict()->v.modelindex;

                    if (Player->edict()->v.flags & FL_DUCKING)
                    {
                        g_engfuncs.pfnSetSize(Player->edict(), VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
                    }
                    else
                    {
                        g_engfuncs.pfnSetSize(Player->edict(), VEC_HULL_MIN, VEC_HULL_MAX);
                    }

                    return true;
                }
            }
        }
        while (true);
    }

    return false;
}

bool CCSDM_Spawn::CheckDistance(CBasePlayer* Player, vec3_t Origin, float Distance)
{
    edict_t* pEntity = nullptr;

    while (!FNullEnt(pEntity = g_engfuncs.pfnFindEntityInSphere(pEntity, Origin, Distance)))
    {
        auto Target = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

        if (Target)
        {
            if (Target->IsPlayer())
            {
                if (Target->entindex() != Player->entindex())
                {
                    if (Target->IsAlive())
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}