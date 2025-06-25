
#include "Precomp.h"
#include "PrepareDlg.h"

PrepareDlg::PrepareDlg()
{
	dialog =
	{
		.dialog = "PrepareDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 1,
		.title = "[ProductName] [Setup]",
		.control_First = "Cancel",
		.control_Default = "Cancel",
		.control_Cancel = "Cancel"
	};

	controls =
	{
		{.dialog = "PrepareDlg", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "Cancel" },
		{.dialog = "PrepareDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "PrepareDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{.dialog = "PrepareDlg", .control = "Description", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 20, .attributes = 196611, .text = "Please wait while the [Wizard] prepares to guide you through the installation." },
		{.dialog = "PrepareDlg", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}Welcome to the [ProductName] [Wizard]" },
		{.dialog = "PrepareDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]" },
		{.dialog = "PrepareDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Next]" },
		{.dialog = "PrepareDlg", .control = "ActionData", .type = "Text", .x = 135, .y = 125, .width = 220, .height = 30, .attributes = 196611 },
		{.dialog = "PrepareDlg", .control = "ActionText", .type = "Text", .x = 135, .y = 100, .width = 220, .height = 20, .attributes = 196611 },
	};

	controlEvent =
	{
		{.dialog = "PrepareDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
	};

	eventMapping =
	{
		{.dialog = "PrepareDlg", .control = "ActionData", .event = "ActionData", .attribute = "Text" },
		{.dialog = "PrepareDlg", .control = "ActionText", .event = "ActionText", .attribute = "Text" },
	};
}
