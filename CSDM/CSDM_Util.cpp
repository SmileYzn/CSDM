#include "precompiled.h"

CCSDM_Util gCSDM_Util;

cvar_t* CCSDM_Util::CvarRegister(const char* Name, const char* Value)
{
	auto Pointer = g_engfuncs.pfnCVarGetPointer(Name);

	if (Pointer == nullptr)
	{
		static cvar_t Data;

		Data.name = Name;

		Data.string = (char*)(Value);

		Data.flags = (FCVAR_SERVER | FCVAR_SPONLY);

		g_engfuncs.pfnCVarRegister(&Data);

		Pointer = g_engfuncs.pfnCVarGetPointer(Name);

		if (Pointer)
		{
			g_engfuncs.pfnCvar_DirectSet(Pointer, Value);
		}
	}

	return Pointer;
}

void CCSDM_Util::ServerCommand(const char* Format, ...)
{
	char Command[255] = { 0 };

	va_list	argptr;

	va_start(argptr, Format);

	vsnprintf(Command, sizeof(Command), Format, argptr);

	va_end(argptr);

	Q_strncat(Command, "\n", 1);

	g_engfuncs.pfnServerCommand(Command);
}

void CCSDM_Util::TeamInfo(edict_t* pEntity, int playerIndex, const char* pszTeamName)
{
	static int iMsgTeamInfo;

	if (iMsgTeamInfo || (iMsgTeamInfo = gpMetaUtilFuncs->pfnGetUserMsgID(&Plugin_info, "TeamInfo", NULL)))
	{
		g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgTeamInfo, nullptr, pEntity);
		g_engfuncs.pfnWriteByte(playerIndex);
		g_engfuncs.pfnWriteString(pszTeamName);
		g_engfuncs.pfnMessageEnd();
	}
}

void CCSDM_Util::DeathMsg(edict_t* pEntity, int KillerId, int VictimId, int IsHeadshot, const char* TruncatedWeaponName)
{
	static int iDeathMsg;

	if (iDeathMsg || (iDeathMsg = gpMetaUtilFuncs->pfnGetUserMsgID(&Plugin_info, "DeathMsg", NULL)))
	{
		g_engfuncs.pfnMessageBegin(MSG_ONE, iDeathMsg, nullptr, pEntity);
		g_engfuncs.pfnWriteByte(KillerId);
		g_engfuncs.pfnWriteByte(VictimId);
		g_engfuncs.pfnWriteByte(IsHeadshot);
		g_engfuncs.pfnWriteString(TruncatedWeaponName);
		g_engfuncs.pfnMessageEnd();
	}
}

void CCSDM_Util::ScreenFade(edict_t* pEdict, int Duration, int HoldTime, int Flags, int Red, int Green, int Blue, int Alpha)
{
	if (!FNullEnt(pEdict))
	{
		static int iMsgScreenFade;

		if (iMsgScreenFade || (iMsgScreenFade = gpMetaUtilFuncs->pfnGetUserMsgID(&Plugin_info, "ScreenFade", NULL)))
		{
			g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgScreenFade, nullptr, pEdict);
			g_engfuncs.pfnWriteShort(BIT(10));
			g_engfuncs.pfnWriteShort(BIT(10));
			g_engfuncs.pfnWriteShort(0x0000);
			g_engfuncs.pfnWriteByte(0);
			g_engfuncs.pfnWriteByte(0);
			g_engfuncs.pfnWriteByte(200);
			g_engfuncs.pfnWriteByte(75);
			g_engfuncs.pfnMessageEnd();
		}
	}
}

int CCSDM_Util::ParseColors(char* Buffer)
{
	size_t len = strlen(Buffer);

	int offs = 0;

	if (len > 0)
	{
		int c;

		for (size_t i = 0; i < len; i++)
		{
			c = Buffer[i];

			if (c == '^' && (i != len - 1))
			{
				c = Buffer[++i];

				if (c == 'n' || c == 't' || (c >= '1' && c <= '4'))
				{
					switch (c)
					{
						case '1': c = '\x01'; break;
						case '2': c = '\x02'; break;
						case '3': c = '\x03'; break;
						case '4': c = '\x04'; break;
						case 'n': c = '\n'; break;
						case 't': c = '\t'; break;
					}

					offs++;
				}
			}

			Buffer[i - offs] = c;
		}

		Buffer[len - offs] = '\0';
	}

	return offs;
}

void CCSDM_Util::SayText(edict_t* pEntity, int Sender, const char* Format, ...)
{
	static int iMsgSayText;

	if (iMsgSayText || (iMsgSayText = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "SayText", NULL)))
	{
		char Buffer[191] = { 0 };

		va_list ArgList;
		va_start(ArgList, Format);
		Q_vsnprintf(Buffer, sizeof(Buffer), Format, ArgList);
		va_end(ArgList);

		if (Plugin_info.logtag)
		{
			if (Plugin_info.logtag[0u] != '\0')
			{
				char Temp[191] = { 0 };

				Q_snprintf(Temp, sizeof(Temp), "^4[%s]^1 %s", Plugin_info.logtag, Buffer);

				Q_strcpy_s(Buffer, Temp);
			}
		}

		if (Sender < PRINT_TEAM_BLUE || Sender > gpGlobals->maxClients)
		{
			Sender = PRINT_TEAM_DEFAULT;
		}
		else if (Sender < PRINT_TEAM_DEFAULT)
		{
			Sender = abs(Sender) + MAX_CLIENTS;
		}

		this->ParseColors(Buffer);

		if (!FNullEnt(pEntity))
		{
			if (!(pEntity->v.flags & FL_FAKECLIENT))
			{
				g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgSayText, nullptr, pEntity);
				g_engfuncs.pfnWriteByte(Sender ? Sender : ENTINDEX(pEntity));
				g_engfuncs.pfnWriteString("%s");
				g_engfuncs.pfnWriteString(Buffer);
				g_engfuncs.pfnMessageEnd();
			}
		}
		else
		{
			for (int i = 1; i <= gpGlobals->maxClients; ++i)
			{
				edict_t* pTempEntity = INDEXENT(i);

				if (!FNullEnt(pTempEntity))
				{
					if (!(pTempEntity->v.flags & FL_FAKECLIENT))
					{
						g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgSayText, nullptr, pTempEntity);
						g_engfuncs.pfnWriteByte(Sender ? Sender : i);
						g_engfuncs.pfnWriteString("%s");
						g_engfuncs.pfnWriteString(Buffer);
						g_engfuncs.pfnMessageEnd();
					}
				}
			}
		}
	}
}

void CCSDM_Util::ReplaceAll(std::string& String, const std::string& From, const std::string& To)
{
	if (!From.empty())
	{
		size_t StartPos = 0;

		while ((StartPos = String.find(From, StartPos)) != std::string::npos)
		{
			String.replace(StartPos, From.length(), To);

			StartPos += To.length();
		}
	}
}

unsigned short CCSDM_Util::FixedUnsigned16(float value, float scale)
{
	int output = value * scale;

	if (output < 0)
	{
		output = 0;
	}

	if (output > USHRT_MAX)
	{
		output = USHRT_MAX;
	}

	return (unsigned short)output;
}

short CCSDM_Util::FixedSigned16(float value, float scale)
{
	int output = value * scale;

	if (output > SHRT_MAX)
	{
		output = SHRT_MAX;
	}

	if (output < SHRT_MIN)
	{
		output = SHRT_MIN;
	}

	return (short)output;
}

hudtextparms_t CCSDM_Util::SetHudParam(int red, int green, int blue, float x, float y, int effects, float fxtime, float holdtime, float fadeintime, float fadeouttime, int channel)
{
	hudtextparms_t hud;

	hud.r1 = red;
	hud.g1 = green;
	hud.b1 = blue;
	hud.a1 = 255;
	hud.r2 = 255;
	hud.g2 = 255;
	hud.b2 = 255;
	hud.a2 = 255;
	hud.x = x;
	hud.y = y;
	hud.effect = effects;
	hud.fxTime = fxtime;
	hud.holdTime = holdtime;
	hud.fadeinTime = fadeintime;
	hud.fadeoutTime = fadeouttime;
	hud.channel = channel;

	return hud;
}

void CCSDM_Util::ShowHudMessage(edict_t* pEntity, hudtextparms_t textparms, const char* Format, ...)
{
	va_list argList;

	va_start(argList, Format);

	char Buffer[511] = { 0 };

	vsnprintf(Buffer, sizeof(Buffer), Format, argList);

	va_end(argList);

	this->ParseColors(Buffer);

	if (pEntity)
	{
		g_engfuncs.pfnMessageBegin(MSG_ONE_UNRELIABLE, SVC_TEMPENTITY, NULL, pEntity);
	}
	else
	{
		g_engfuncs.pfnMessageBegin(MSG_BROADCAST, SVC_TEMPENTITY, NULL, NULL);
	}

	g_engfuncs.pfnWriteByte(TE_TEXTMESSAGE);
	g_engfuncs.pfnWriteByte(textparms.channel & 0xFF);

	g_engfuncs.pfnWriteShort(this->FixedSigned16(textparms.x, BIT(13)));
	g_engfuncs.pfnWriteShort(this->FixedSigned16(textparms.y, BIT(13)));

	g_engfuncs.pfnWriteByte(textparms.effect);

	g_engfuncs.pfnWriteByte(textparms.r1);
	g_engfuncs.pfnWriteByte(textparms.g1);
	g_engfuncs.pfnWriteByte(textparms.b1);
	g_engfuncs.pfnWriteByte(textparms.a1);

	g_engfuncs.pfnWriteByte(textparms.r2);
	g_engfuncs.pfnWriteByte(textparms.g2);
	g_engfuncs.pfnWriteByte(textparms.b2);
	g_engfuncs.pfnWriteByte(textparms.a2);

	g_engfuncs.pfnWriteShort(this->FixedUnsigned16(textparms.fadeinTime, BIT(8)));
	g_engfuncs.pfnWriteShort(this->FixedUnsigned16(textparms.fadeoutTime, BIT(8)));
	g_engfuncs.pfnWriteShort(this->FixedUnsigned16(textparms.holdTime, BIT(8)));

	if (textparms.effect == 2)
	{
		g_engfuncs.pfnWriteShort(this->FixedUnsigned16(textparms.fxTime, BIT(8)));
	}

	g_engfuncs.pfnWriteString(Buffer);
	g_engfuncs.pfnMessageEnd();
}