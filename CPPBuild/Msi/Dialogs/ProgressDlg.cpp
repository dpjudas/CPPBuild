
#include "Precomp.h"
#include "ProgressDlg.h"

ProgressDlg::ProgressDlg()
{
	dialog =
	{
		.dialog = "ProgressDlg",
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
		{.dialog = "ProgressDlg", .control = "Text", .type = "Text", .x = 35, .y = 65, .width = 300, .height = 20, .attributes = 3, .text = "Please wait while the [Wizard] [Progress2] [ProductName].  This may take several minutes." },
		{.dialog = "ProgressDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Back" },
		{.dialog = "ProgressDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "ProgressDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "ProgressDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{.dialog = "ProgressDlg", .control = "Title", .type = "Text", .x = 20, .y = 15, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont][Progress1] [ProductName]" },
		{.dialog = "ProgressDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{.dialog = "ProgressDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{.dialog = "ProgressDlg", .control = "ActionText", .type = "Text", .x = 70, .y = 100, .width = 265, .height = 10, .attributes = 3 },
		{.dialog = "ProgressDlg", .control = "ProgressBar", .type = "ProgressBar", .x = 35, .y = 115, .width = 300, .height = 10, .attributes = 65537, .text = "Progress done" },
		{.dialog = "ProgressDlg", .control = "StatusLabel", .type = "Text", .x = 35, .y = 100, .width = 35, .height = 10, .attributes = 3, .text = "Status:" },
	};

	controlEvent =
	{
		{.dialog = "ProgressDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
	};

	eventMapping =
	{
		{.dialog = "ProgressDlg", .control = "ActionText", .event = "ActionText", .attribute = "Text" },
		{.dialog = "ProgressDlg", .control = "ProgressBar", .event = "SetProgress", .attribute = "Progress" },
	};
}
