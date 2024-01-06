#include "precompiled.h"

CCSDM_Config gCSDM_Config;

void CCSDM_Config::ServerActivate()
{
	if (this->m_Path.empty())
	{
		std::string GameDir = gpMetaUtilFuncs->pfnGetGameInfo(&Plugin_info, GINFO_GAMEDIR);

		if (!GameDir.empty())
		{
			this->m_Path = gpMetaUtilFuncs->pfnGetPluginPath(&Plugin_info);

			if (!this->m_Path.empty())
			{
				this->m_Path.erase(0, GameDir.length() + 1);

				this->m_Path.erase(this->m_Path.find_last_of((char)(47)), this->m_Path.length());

				if (this->m_Path.find("/dlls") != std::string::npos)
				{
					this->m_Path.erase(this->m_Path.find_last_of((char)(47)), this->m_Path.length());
				}

				std::replace(this->m_Path.begin(), this->m_Path.end(), (char)(92), (char)(47));
			}
		}
	}
}

void CCSDM_Config::Execute(std::string File)
{
	if (!File.empty())
	{
		if (!this->m_Path.empty())
		{
			gCSDM_Util.ServerCommand("exec %s/%s", this->m_Path.c_str(), File.c_str());
		}
	}
}

std::string CCSDM_Config::GetPath()
{
	std::string Path = gpMetaUtilFuncs->pfnGetGameInfo(&Plugin_info, GINFO_GAMEDIR);

	if (!Path.empty())
	{
		Path = Path + (char)(47) + this->m_Path;

		std::replace(Path.begin(), Path.end(), (char)(92), (char)(47));
	}

	return Path;
}
