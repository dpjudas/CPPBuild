
#include "Precomp.h"
#include "WaitForCostingDlg.h"

WaitForCostingDlg::WaitForCostingDlg()
{
	dialog =
	{
		.dialog = "WaitForCostingDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 260,
		.height = 85,
		.attributes = 3,
		.title = "[ProductName] [Setup]",
		.control_First = "Return",
		.control_Default = "Return",
		.control_Cancel = "Return"
	};

	controls =
	{
		{.dialog = "WaitForCostingDlg", .control = "Text", .type = "Text", .x = 48, .y = 15, .width = 194, .height = 30, .attributes = 3, .text = "Please wait while the installer finishes determining your disk space requirements." },
		{.dialog = "WaitForCostingDlg", .control = "Icon", .type = "Icon", .x = 15, .y = 15, .width = 24, .height = 24, .attributes = 5242881, .text = "[ExclamationIcon]", .help = "Exclamation icon|" },
		{.dialog = "WaitForCostingDlg", .control = "Return", .type = "PushButton", .x = 102, .y = 57, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Return]" },
	};

	controlEvent =
	{
		{.dialog = "WaitForCostingDlg", .control = "Return", .event = "EndDialog", .argument = "Exit", .condition = "1" },
	};
}
