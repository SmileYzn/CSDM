#pragma once

class CCSDM_Command
{
public:
	bool ClientCommand(edict_t* pEntity);
};

extern CCSDM_Command gCSDM_Command;