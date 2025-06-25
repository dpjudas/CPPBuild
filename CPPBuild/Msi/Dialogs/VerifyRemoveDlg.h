#pragma once

#include "Msi/MSISchema.h"

class VerifyRemoveDlg
{
public:
	VerifyRemoveDlg();

	MSIDialog dialog;
	std::vector<MSIControl> controls;
	std::vector<MSIControlCondition> controlCondition;
	std::vector<MSIControlEvent> controlEvent;
	std::vector<MSIEventMapping> eventMapping;
};
