
#include "Precomp.h"
#include "ResumeDlg.h"

ResumeDlg::ResumeDlg()
{
	dialog =
	{
		.dialog = "ResumeDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 3,
		.title = "[ProductName] [Setup]",
		.control_First = "Install",
		.control_Default = "Install",
		.control_Cancel = "Cancel"
	};

	controls =
	{
		{.dialog = "ResumeDlg", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "Back" },
		{.dialog = "ResumeDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "ResumeDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{.dialog = "ResumeDlg", .control = "Description", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 30, .attributes = 196611, .text = "The [Wizard] will complete the installation of [ProductName] on your computer.  Click Install to continue or Cancel to exit the [Wizard]." },
		{.dialog = "ResumeDlg", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}Resuming the [ProductName] [Wizard]" },
		{.dialog = "ResumeDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Install" },
		{.dialog = "ResumeDlg", .control = "Install", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Install]", .control_Next = "Cancel" },
	};

	controlEvent =
	{
		{.dialog = "ResumeDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{.dialog = "ResumeDlg", .control = "Install", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 4 },
		{.dialog = "ResumeDlg", .control = "Install", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace <> 1", .ordering = 2 },
		{.dialog = "ResumeDlg", .control = "Install", .event = "SpawnDialog", .argument = "OutOfDiskDlg", .condition = "(OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 1) OR (OutOfDiskSpace = 1 AND PROMPTROLLBACKCOST=\"F\")", .ordering = 6 },
		{.dialog = "ResumeDlg", .control = "Install", .event = "SpawnDialog", .argument = "OutOfRbDiskDlg", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND (PROMPTROLLBACKCOST=\"P\" OR NOT PROMPTROLLBACKCOST)", .ordering = 3 },
		{.dialog = "ResumeDlg", .control = "Install", .event = "SpawnWaitDialog", .argument = "WaitForCostingDlg", .condition = "CostingComplete = 1", .ordering = 1 },
		{.dialog = "ResumeDlg", .control = "Install", .event = "EnableRollback", .argument = "False", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 5 },
	};
}
