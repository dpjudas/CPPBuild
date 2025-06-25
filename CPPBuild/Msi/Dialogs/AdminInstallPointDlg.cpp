
#include "Precomp.h"
#include "AdminInstallPointDlg.h"

AdminInstallPointDlg::AdminInstallPointDlg()
{
	dialog =
	{
		.dialog = "AdminInstallPointDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 3,
		.title = "[ProductName] [Setup]",
		.control_First = "Text",
		.control_Default = "Next",
		.control_Cancel = "Cancel"
	};

	controls =
	{
		{.dialog = "AdminInstallPointDlg", .control = "Text", .type = "Text", .x = 25, .y = 80, .width = 320, .height = 10, .attributes = 3, .text = "&Enter a new network location or click Browse to browse to one.", .control_Next = "PathEdit" },
		{.dialog = "AdminInstallPointDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Text" },
		{.dialog = "AdminInstallPointDlg", .control = "PathEdit", .type = "PathEdit", .x = 25, .y = 93, .width = 320, .height = 18, .attributes = 3, .property = "TARGETDIR", .control_Next = "Browse" },
		{.dialog = "AdminInstallPointDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "AdminInstallPointDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "AdminInstallPointDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{.dialog = "AdminInstallPointDlg", .control = "Description", .type = "Text", .x = 25, .y = 20, .width = 280, .height = 20, .attributes = 196611, .text = "Please specify a network location for the server image of [ProductName] product" },
		{.dialog = "AdminInstallPointDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Network Location" },
		{.dialog = "AdminInstallPointDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{.dialog = "AdminInstallPointDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{.dialog = "AdminInstallPointDlg", .control = "Browse", .type = "PushButton", .x = 289, .y = 119, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Browse]", .control_Next = "Back" },
	};

	controlEvent =
	{
		{.dialog = "AdminInstallPointDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{.dialog = "AdminInstallPointDlg", .control = "Back", .event = "NewDialog", .argument = "AdminRegistrationDlg", .condition = "1" },
		{.dialog = "AdminInstallPointDlg", .control = "Next", .event = "SetTargetPath", .argument = "TARGETDIR", .condition = "1", .ordering = 1 },
		{.dialog = "AdminInstallPointDlg", .control = "Next", .event = "NewDialog", .argument = "VerifyReadyDlg", .condition = "1", .ordering = 2 },
		{.dialog = "AdminInstallPointDlg", .control = "Browse", .event = "SpawnDialog", .argument = "AdminBrowseDlg", .condition = "1" },
	};
}
