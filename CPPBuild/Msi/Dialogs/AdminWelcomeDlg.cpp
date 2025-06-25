
#include "Precomp.h"
#include "AdminWelcomeDlg.h"

AdminWelcomeDlg::AdminWelcomeDlg()
{
	dialog =
	{
		.dialog = "AdminWelcomeDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 3,
		.title = "[ProductName] [Setup]",
		.control_First = "Next",
		.control_Default = "Next",
		.control_Cancel = "Cancel"
	};

	controls =
	{
		{.dialog = "AdminWelcomeDlg", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "Back" },
		{.dialog = "AdminWelcomeDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "AdminWelcomeDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{.dialog = "AdminWelcomeDlg", .control = "Description", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 30, .attributes = 196611, .text = "The [Wizard] will create a server image of [ProductName], at a specified network location.  Click Next to continue or Cancel to exit the [Wizard]." },
		{.dialog = "AdminWelcomeDlg", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}Welcome to the [ProductName] [Wizard]" },
		{.dialog = "AdminWelcomeDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{.dialog = "AdminWelcomeDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
	};

	controlEvent =
	{
		{.dialog = "AdminWelcomeDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{.dialog = "AdminWelcomeDlg", .control = "Next", .event = "NewDialog", .argument = "AdminRegistrationDlg", .condition = "1", .ordering = 2 },
		{.dialog = "AdminWelcomeDlg", .control = "Next", .event = "[InstallMode]", .argument = "Server Image", .condition = "1", .ordering = 1 },
	};
}
