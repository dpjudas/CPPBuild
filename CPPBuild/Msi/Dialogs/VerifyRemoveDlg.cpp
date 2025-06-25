
#include "Precomp.h"
#include "VerifyRemoveDlg.h"

VerifyRemoveDlg::VerifyRemoveDlg()
{
	dialog =
	{
		.dialog = "VerifyRemoveDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 35,
		.title = "[ProductName] [Setup]",
		.control_First = "Back",
		.control_Default = "Back",
		.control_Cancel = "Cancel"
	};

	controls =
	{
		{.dialog = "VerifyRemoveDlg", .control = "Text", .type = "Text", .x = 25, .y = 70, .width = 320, .height = 30, .attributes = 3, .text = "Click Remove to remove [ProductName] from your computer.  If you want to review or change any of your installation settings, click Back.  Click Cancel to exit the wizard." },
		{.dialog = "VerifyRemoveDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Back" },
		{.dialog = "VerifyRemoveDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "VerifyRemoveDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "VerifyRemoveDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{.dialog = "VerifyRemoveDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "You have chosen to remove the program from your computer." },
		{.dialog = "VerifyRemoveDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Remove [ProductName]" },
		{.dialog = "VerifyRemoveDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Remove" },
		{.dialog = "VerifyRemoveDlg", .control = "Remove", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 8388611, .text = "[ButtonText_Remove]", .control_Next = "Cancel" },
	};

	controlEvent =
	{
		{.dialog = "VerifyRemoveDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{.dialog = "VerifyRemoveDlg", .control = "Back", .event = "NewDialog", .argument = "MaintenanceTypeDlg", .condition = "1" },
		{.dialog = "VerifyRemoveDlg", .control = "Remove", .event = "Remove", .argument = "All", .condition = "OutOfDiskSpace <> 1", .ordering = 1 },
		{.dialog = "VerifyRemoveDlg", .control = "Remove", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 4 },
		{.dialog = "VerifyRemoveDlg", .control = "Remove", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace <> 1", .ordering = 2 },
		{.dialog = "VerifyRemoveDlg", .control = "Remove", .event = "SpawnDialog", .argument = "OutOfDiskDlg", .condition = "(OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 1) OR (OutOfDiskSpace = 1 AND PROMPTROLLBACKCOST=\"F\")", .ordering = 6 },
		{.dialog = "VerifyRemoveDlg", .control = "Remove", .event = "SpawnDialog", .argument = "OutOfRbDiskDlg", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND (PROMPTROLLBACKCOST=\"P\" OR NOT PROMPTROLLBACKCOST)", .ordering = 3 },
		{.dialog = "VerifyRemoveDlg", .control = "Remove", .event = "EnableRollback", .argument = "False", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 5 },
	};
}
