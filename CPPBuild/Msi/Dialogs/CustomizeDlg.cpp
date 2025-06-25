
#include "Precomp.h"
#include "CustomizeDlg.h"

CustomizeDlg::CustomizeDlg()
{
	dialog =
	{
		.dialog = "CustomizeDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 35,
		.title = "[ProductName] [Setup]",
		.control_First = "Tree",
		.control_Default = "Next",
		.control_Cancel = "Cancel"
	};

	controls =
	{
		{.dialog = "CustomizeDlg", .control = "Text", .type = "Text", .x = 25, .y = 55, .width = 320, .height = 20, .attributes = 3, .text = "Click on the icons in the tree below to change the way features will be installed." },
		{.dialog = "CustomizeDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Tree" },
		{.dialog = "CustomizeDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "CustomizeDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "CustomizeDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{.dialog = "CustomizeDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "Select the way you want features to be installed." },
		{.dialog = "CustomizeDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Custom Setup" },
		{.dialog = "CustomizeDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{.dialog = "CustomizeDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{.dialog = "CustomizeDlg", .control = "Browse", .type = "PushButton", .x = 304, .y = 200, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Browse]", .control_Next = "Reset" },
		{.dialog = "CustomizeDlg", .control = "Tree", .type = "SelectionTree", .x = 25, .y = 85, .width = 175, .height = 95, .attributes = 7, .property = "_BrowseProperty", .text = "Tree of selections", .control_Next = "Browse" },
		{.dialog = "CustomizeDlg", .control = "Box", .type = "GroupBox", .x = 210, .y = 81, .width = 140, .height = 98, .attributes = 1 },
		{.dialog = "CustomizeDlg", .control = "Reset", .type = "PushButton", .x = 42, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Reset]", .control_Next = "DiskCost" },
		{.dialog = "CustomizeDlg", .control = "DiskCost", .type = "PushButton", .x = 111, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "Disk &Usage", .control_Next = "Back" },
		{.dialog = "CustomizeDlg", .control = "ItemDescription", .type = "Text", .x = 215, .y = 90, .width = 131, .height = 30, .attributes = 3, .text = "Multiline description of the currently selected item." },
		{.dialog = "CustomizeDlg", .control = "ItemSize", .type = "Text", .x = 215, .y = 130, .width = 131, .height = 45, .attributes = 3, .text = "The size of the currently selected item." },
		{.dialog = "CustomizeDlg", .control = "Location", .type = "Text", .x = 75, .y = 200, .width = 215, .height = 20, .attributes = 3, .text = "<The selection's path>" },
		{.dialog = "CustomizeDlg", .control = "LocationLabel", .type = "Text", .x = 25, .y = 200, .width = 50, .height = 10, .attributes = 3, .text = "Location:" },
	};

	controlCondition =
	{
		{.dialog = "CustomizeDlg", .control = "Browse", .action = "Hide", .condition = "Installed" },
		{.dialog = "CustomizeDlg", .control = "Location", .action = "Hide", .condition = "Installed" },
		{.dialog = "CustomizeDlg", .control = "LocationLabel", .action = "Hide", .condition = "Installed" },
	};

	controlEvent =
	{
		{.dialog = "CustomizeDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{.dialog = "CustomizeDlg", .control = "Back", .event = "NewDialog", .argument = "MaintenanceTypeDlg", .condition = "InstallMode = \"Change\"" },
		{.dialog = "CustomizeDlg", .control = "Back", .event = "NewDialog", .argument = "SetupTypeDlg", .condition = "InstallMode = \"Custom\"" },
		{.dialog = "CustomizeDlg", .control = "Next", .event = "NewDialog", .argument = "VerifyReadyDlg", .condition = "1" },
		{.dialog = "CustomizeDlg", .control = "Browse", .event = "SelectionBrowse", .argument = "BrowseDlg", .condition = "1" },
		{.dialog = "CustomizeDlg", .control = "Reset", .event = "Reset", .argument = "0", .condition = "1" },
		{.dialog = "CustomizeDlg", .control = "DiskCost", .event = "SpawnDialog", .argument = "DiskCostDlg", .condition = "1", .ordering = 2 },
	};

	eventMapping =
	{
		{.dialog = "CustomizeDlg", .control = "Next", .event = "SelectionNoItems", .attribute = "Enabled" },
		{.dialog = "CustomizeDlg", .control = "Reset", .event = "SelectionNoItems", .attribute = "Enabled" },
		{.dialog = "CustomizeDlg", .control = "DiskCost", .event = "SelectionNoItems", .attribute = "Enabled" },
		{.dialog = "CustomizeDlg", .control = "ItemDescription", .event = "SelectionDescription", .attribute = "Text" },
		{.dialog = "CustomizeDlg", .control = "ItemSize", .event = "SelectionSize", .attribute = "Text" },
		{.dialog = "CustomizeDlg", .control = "Location", .event = "SelectionPath", .attribute = "Text" },
		{.dialog = "CustomizeDlg", .control = "Location", .event = "SelectionPathOn", .attribute = "Visible" },
		{.dialog = "CustomizeDlg", .control = "LocationLabel", .event = "SelectionPathOn", .attribute = "Visible" },
	};
}
