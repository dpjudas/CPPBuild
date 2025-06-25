
#include "Precomp.h"
#include "AdminBrowseDlg.h"

AdminBrowseDlg::AdminBrowseDlg()
{
	dialog =
	{
		.dialog = "AdminBrowseDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 3,
		.title = "[ProductName] [Setup]",
		.control_First = "PathEdit",
		.control_Default = "OK",
		.control_Cancel = "Cancel"
	};

	controls =
	{
		{.dialog = "AdminBrowseDlg", .control = "Up", .type = "PushButton", .x = 298, .y = 55, .width = 19, .height = 19, .attributes = 3670019, .text = "Up", .control_Next = "NewFolder", .help = "Up One Level|" },
		{.dialog = "AdminBrowseDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "PathEdit" },
		{.dialog = "AdminBrowseDlg", .control = "PathEdit", .type = "PathEdit", .x = 84, .y = 202, .width = 261, .height = 17, .attributes = 3, .property = "TARGETDIR", .control_Next = "OK" },
		{.dialog = "AdminBrowseDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "AdminBrowseDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "AdminBrowseDlg", .control = "Cancel", .type = "PushButton", .x = 240, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "ComboLabel" },
		{.dialog = "AdminBrowseDlg", .control = "ComboLabel", .type = "Text", .x = 25, .y = 58, .width = 44, .height = 10, .attributes = 3, .text = "&Look in:", .control_Next = "DirectoryCombo" },
		{.dialog = "AdminBrowseDlg", .control = "DirectoryCombo", .type = "DirectoryCombo", .x = 70, .y = 55, .width = 220, .height = 80, .attributes = 458755, .property = "TARGETDIR", .control_Next = "Up" },
		{.dialog = "AdminBrowseDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "Browse to the destination folder" },
		{.dialog = "AdminBrowseDlg", .control = "DirectoryList", .type = "DirectoryList", .x = 25, .y = 83, .width = 320, .height = 110, .attributes = 7, .property = "TARGETDIR", .control_Next = "PathLabel" },
		{.dialog = "AdminBrowseDlg", .control = "PathLabel", .type = "Text", .x = 25, .y = 205, .width = 59, .height = 10, .attributes = 3, .text = "&Folder name:", .control_Next = "BannerBitmap" },
		{.dialog = "AdminBrowseDlg", .control = "NewFolder", .type = "PushButton", .x = 325, .y = 55, .width = 19, .height = 19, .attributes = 3670019, .text = "New", .control_Next = "DirectoryList", .help = "Create A New Folder|" },
		{.dialog = "AdminBrowseDlg", .control = "OK", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_OK]", .control_Next = "Cancel" },
		{.dialog = "AdminBrowseDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Change current destination folder" },
	};

	controlEvent =
	{
		{.dialog = "AdminBrowseDlg", .control = "Up", .event = "DirectoryListUp", .argument = "0", .condition = "1" },
		{.dialog = "AdminBrowseDlg", .control = "Cancel", .event = "Reset", .argument = "0", .condition = "1", .ordering = 1 },
		{.dialog = "AdminBrowseDlg", .control = "Cancel", .event = "EndDialog", .argument = "Return", .condition = "1", .ordering = 2 },
		{.dialog = "AdminBrowseDlg", .control = "NewFolder", .event = "DirectoryListNew", .argument = "0", .condition = "1" },
		{.dialog = "AdminBrowseDlg", .control = "OK", .event = "EndDialog", .argument = "Return", .condition = "1", .ordering = 2 },
		{.dialog = "AdminBrowseDlg", .control = "OK", .event = "SetTargetPath", .argument = "TARGETDIR", .condition = "1", .ordering = 1 },
	};

	eventMapping =
	{
		{.dialog = "AdminBrowseDlg", .control = "DirectoryCombo", .event = "IgnoreChange", .attribute = "IgnoreChange" },
	};
}
