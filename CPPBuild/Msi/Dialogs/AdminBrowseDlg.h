#pragma once

#include "Msi/MSISchema.h"

class AdminBrowseDlg
{
public:
	AdminBrowseDlg();

	MSIDialog dialog;
	std::vector<MSIControl> controls;
	std::vector<MSIControlCondition> controlCondition;
	std::vector<MSIControlEvent> controlEvent;
	std::vector<MSIEventMapping> eventMapping;
};
