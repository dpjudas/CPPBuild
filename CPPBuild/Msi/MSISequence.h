#pragma once

#include "MSISchema.h"

class MSISequences
{
public:
	MSISequences();
	void createTables(MSIDatabase* db);

	std::vector<MSIInstallUISequence> installUISequence;
	std::vector<MSIInstallExecuteSequence> installExecuteSequence;
	std::vector<MSIAdminUISequence> adminUISequence;
	std::vector<MSIAdminExecuteSequence> adminExecuteSequence;
	std::vector<MSIAdvtExecuteSequence> advtExecuteSequence;
	std::vector<MSIActionText> actionText;
};
