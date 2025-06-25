
#include "Precomp.h"
#include "WelcomeDlg.h"

WelcomeDlg::WelcomeDlg()
{
	dialog =
	{
		.dialog = "WelcomeDlg",
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
		{.dialog = "WelcomeDlg", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "Back" },
		{.dialog = "WelcomeDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "WelcomeDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{.dialog = "WelcomeDlg", .control = "Description", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 30, .attributes = 196611, .text = "The [Wizard] will install [ProductName] on your computer.  Click Next to continue or Cancel to exit the [Wizard]." },
		{.dialog = "WelcomeDlg", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}Welcome to the [ProductName] [Wizard]" },
		{.dialog = "WelcomeDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{.dialog = "WelcomeDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
	};

	controlEvent =
	{
		{.dialog = "WelcomeDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{.dialog = "WelcomeDlg", .control = "Next", .event = "NewDialog", .argument = "LicenseAgreementDlg", .condition = "1" },
	};
}
