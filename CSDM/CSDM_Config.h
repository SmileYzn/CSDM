#pragma once

class CCSDM_Config
{
public:
	void ServerActivate();
	void Execute(std::string File);
	std::string GetPath();

private:
	std::string m_Path;
};

extern CCSDM_Config gCSDM_Config;