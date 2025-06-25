
#include "Precomp.h"
#include "VerifyReadyDlg.h"

VerifyReadyDlg::VerifyReadyDlg()
{
	dialog =
	{
		.dialog = "VerifyReadyDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 35,
		.title = "[ProductName] [Setup]",
		.control_First = "Install",
		.control_Default = "Install",
		.control_Cancel = "Cancel"
	};

	controls =
	{
		{.dialog = "VerifyReadyDlg", .control = "Text", .type = "Text", .x = 25, .y = 70, .width = 320, .height = 20, .attributes = 3, .text = "Click Install to begin the installation.  If you want to review or change any of your installation settings, click Back.  Click Cancel to exit the wizard." },
		{.dialog = "VerifyReadyDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Back" },
		{.dialog = "VerifyReadyDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "VerifyReadyDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "VerifyReadyDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{.dialog = "VerifyReadyDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "The [Wizard] is ready to begin the [InstallMode] installation" },
		{.dialog = "VerifyReadyDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Ready to Install" },
		{.dialog = "VerifyReadyDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Install" },
		{.dialog = "VerifyReadyDlg", .control = "Install", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 8388611, .text = "[ButtonText_Install]", .control_Next = "Cancel" },
	};

	controlEvent =
	{
		{.dialog = "VerifyReadyDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{.dialog = "VerifyReadyDlg", .control = "Back", .event = "NewDialog", .argument = "AdminInstallPointDlg", .condition = "InstallMode = \"Server Image\"" },
		{.dialog = "VerifyReadyDlg", .control = "Back", .event = "NewDialog", .argument = "CustomizeDlg", .condition = "InstallMode = \"Custom\" OR InstallMode = \"Change\"" },
		{.dialog = "VerifyReadyDlg", .control = "Back", .event = "NewDialog", .argument = "MaintenanceTypeDlg", .condition = "InstallMode = \"Repair\"" },
		{.dialog = "VerifyReadyDlg", .control = "Back", .event = "NewDialog", .argument = "SetupTypeDlg", .condition = "InstallMode = \"Typical\" OR InstallMode = \"Complete\"" },
		{.dialog = "VerifyReadyDlg", .control = "Install", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 3 },
		{.dialog = "VerifyReadyDlg", .control = "Install", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace <> 1", .ordering = 1 },
		{.dialog = "VerifyReadyDlg", .control = "Install", .event = "SpawnDialog", .argument = "OutOfDiskDlg", .condition = "(OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 1) OR (OutOfDiskSpace = 1 AND PROMPTROLLBACKCOST=\"F\")", .ordering = 5 },
		{.dialog = "VerifyReadyDlg", .control = "Install", .event = "SpawnDialog", .argument = "OutOfRbDiskDlg", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND (PROMPTROLLBACKCOST=\"P\" OR NOT PROMPTROLLBACKCOST)", .ordering = 2 },
		{.dialog = "VerifyReadyDlg", .control = "Install", .event = "EnableRollback", .argument = "False", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 4 },
	};
}
