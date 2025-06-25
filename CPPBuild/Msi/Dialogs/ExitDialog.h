#pragma once

#include "Msi/MSISchema.h"

class ExitDialog
{
public:
	ExitDialog();

	MSIDialog dialog;
	std::vector<MSIControl> controls;
	std::vector<MSIControlCondition> controlCondition;
	std::vector<MSIControlEvent> controlEvent;
	std::vector<MSIEventMapping> eventMapping;
};
