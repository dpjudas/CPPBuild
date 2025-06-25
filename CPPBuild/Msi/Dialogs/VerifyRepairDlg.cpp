
#include "Precomp.h"
#include "VerifyRepairDlg.h"

VerifyRepairDlg::VerifyRepairDlg()
{
	dialog =
	{
		.dialog = "VerifyRepairDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 35,
		.title = "[ProductName] [Setup]",
		.control_First = "Repair",
		.control_Default = "Repair",
		.control_Cancel = "Cancel"
	};

	controls =
	{
		{.dialog = "VerifyRepairDlg", .control = "Text", .type = "Text", .x = 25, .y = 70, .width = 320, .height = 30, .attributes = 3, .text = "Click Repair to repair the installation of [ProductName].  If you want to review or change any of your installation settings, click Back.  Click Cancel to exit the wizard." },
		{.dialog = "VerifyRepairDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Back" },
		{.dialog = "VerifyRepairDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "VerifyRepairDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "VerifyRepairDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{.dialog = "VerifyRepairDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "The [Wizard] is ready to begin the repair of [ProductName]." },
		{.dialog = "VerifyRepairDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Repair [ProductName]" },
		{.dialog = "VerifyRepairDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Repair" },
		{.dialog = "VerifyRepairDlg", .control = "Repair", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 8388611, .text = "[ButtonText_Repair]", .control_Next = "Cancel" },
	};

	controlEvent =
	{
		{.dialog = "VerifyRepairDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{.dialog = "VerifyRepairDlg", .control = "Back", .event = "NewDialog", .argument = "MaintenanceTypeDlg", .condition = "1" },
		{.dialog = "VerifyRepairDlg", .control = "Repair", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 5 },
		{.dialog = "VerifyRepairDlg", .control = "Repair", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace <> 1", .ordering = 3 },
		{.dialog = "VerifyRepairDlg", .control = "Repair", .event = "SpawnDialog", .argument = "OutOfDiskDlg", .condition = "(OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 1) OR (OutOfDiskSpace = 1 AND PROMPTROLLBACKCOST=\"F\")", .ordering = 7 },
		{.dialog = "VerifyRepairDlg", .control = "Repair", .event = "SpawnDialog", .argument = "OutOfRbDiskDlg", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND (PROMPTROLLBACKCOST=\"P\" OR NOT PROMPTROLLBACKCOST)", .ordering = 4 },
		{.dialog = "VerifyRepairDlg", .control = "Repair", .event = "EnableRollback", .argument = "False", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 6 },
		{.dialog = "VerifyRepairDlg", .control = "Repair", .event = "Reinstall", .argument = "All", .condition = "OutOfDiskSpace <> 1", .ordering = 2 },
		{.dialog = "VerifyRepairDlg", .control = "Repair", .event = "ReinstallMode", .argument = "ecmus", .condition = "OutOfDiskSpace <> 1", .ordering = 1 },
	};
}
