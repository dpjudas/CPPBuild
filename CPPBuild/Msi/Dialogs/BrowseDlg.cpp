
#include "Precomp.h"
#include "BrowseDlg.h"

BrowseDlg::BrowseDlg()
{
	dialog =
	{
		.dialog = "BrowseDlg",
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
		{.dialog = "BrowseDlg", .control = "Up", .type = "PushButton", .x = 298, .y = 55, .width = 19, .height = 19, .attributes = 3670019, .text = "Up", .control_Next = "NewFolder", .help = "Up One Level|" },
		{.dialog = "BrowseDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "PathEdit" },
		{.dialog = "BrowseDlg", .control = "PathEdit", .type = "PathEdit", .x = 84, .y = 202, .width = 261, .height = 18, .attributes = 11, .property = "_BrowseProperty", .control_Next = "OK" },
		{.dialog = "BrowseDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "BrowseDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "BrowseDlg", .control = "Cancel", .type = "PushButton", .x = 240, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "ComboLabel" },
		{.dialog = "BrowseDlg", .control = "ComboLabel", .type = "Text", .x = 25, .y = 58, .width = 44, .height = 10, .attributes = 3, .text = "&Look in:", .control_Next = "DirectoryCombo" },
		{.dialog = "BrowseDlg", .control = "DirectoryCombo", .type = "DirectoryCombo", .x = 70, .y = 55, .width = 220, .height = 80, .attributes = 393227, .property = "_BrowseProperty", .control_Next = "Up" },
		{.dialog = "BrowseDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "Browse to the destination folder" },
		{.dialog = "BrowseDlg", .control = "DirectoryList", .type = "DirectoryList", .x = 25, .y = 83, .width = 320, .height = 110, .attributes = 15, .property = "_BrowseProperty", .control_Next = "PathLabel" },
		{.dialog = "BrowseDlg", .control = "PathLabel", .type = "Text", .x = 25, .y = 205, .width = 59, .height = 10, .attributes = 3, .text = "&Folder name:", .control_Next = "BannerBitmap" },
		{.dialog = "BrowseDlg", .control = "NewFolder", .type = "PushButton", .x = 325, .y = 55, .width = 19, .height = 19, .attributes = 3670019, .text = "New", .control_Next = "DirectoryList", .help = "Create A New Folder|" },
		{.dialog = "BrowseDlg", .control = "OK", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_OK]", .control_Next = "Cancel" },
		{.dialog = "BrowseDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Change current destination folder" },
	};

	controlEvent =
	{
		{.dialog = "BrowseDlg", .control = "Up", .event = "DirectoryListUp", .argument = "0", .condition = "1" },
		{.dialog = "BrowseDlg", .control = "Cancel", .event = "Reset", .argument = "0", .condition = "1", .ordering = 1 },
		{.dialog = "BrowseDlg", .control = "Cancel", .event = "EndDialog", .argument = "Return", .condition = "1", .ordering = 2 },
		{.dialog = "BrowseDlg", .control = "NewFolder", .event = "DirectoryListNew", .argument = "0", .condition = "1" },
		{.dialog = "BrowseDlg", .control = "OK", .event = "EndDialog", .argument = "Return", .condition = "1", .ordering = 2 },
		{.dialog = "BrowseDlg", .control = "OK", .event = "SetTargetPath", .argument = "[_BrowseProperty]", .condition = "1", .ordering = 1 },
	};

	eventMapping =
	{
		{.dialog = "BrowseDlg", .control = "DirectoryCombo", .event = "IgnoreChange", .attribute = "IgnoreChange" },
	};
}
