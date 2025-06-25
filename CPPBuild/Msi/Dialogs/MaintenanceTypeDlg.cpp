
#include "Precomp.h"
#include "MaintenanceTypeDlg.h"

MaintenanceTypeDlg::MaintenanceTypeDlg()
{
	dialog =
	{
		.dialog = "MaintenanceTypeDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 3,
		.title = "[ProductName] [Setup]",
		.control_First = "ChangeLabel",
		.control_Default = "ChangeButton",
		.control_Cancel = "Cancel"
	};

	controls =
	{
		{.dialog = "MaintenanceTypeDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "ChangeLabel" },
		{.dialog = "MaintenanceTypeDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "MaintenanceTypeDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "MaintenanceTypeDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{.dialog = "MaintenanceTypeDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 20, .attributes = 196611, .text = "Select the operation you wish to perform." },
		{.dialog = "MaintenanceTypeDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 240, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Modify, Repair or Remove installation" },
		{.dialog = "MaintenanceTypeDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{.dialog = "MaintenanceTypeDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{.dialog = "MaintenanceTypeDlg", .control = "ChangeLabel", .type = "Text", .x = 105, .y = 65, .width = 100, .height = 10, .attributes = 3, .text = "[DlgTitleFont]&Modify", .control_Next = "ChangeButton" },
		{.dialog = "MaintenanceTypeDlg", .control = "ChangeButton", .type = "PushButton", .x = 50, .y = 65, .width = 38, .height = 38, .attributes = 5767171, .text = "[CustomSetupIcon]", .control_Next = "RepairLabel", .help = "Modify Installation|" },
		{.dialog = "MaintenanceTypeDlg", .control = "RepairLabel", .type = "Text", .x = 105, .y = 114, .width = 100, .height = 10, .attributes = 3, .text = "[DlgTitleFont]Re&pair", .control_Next = "RepairButton" },
		{.dialog = "MaintenanceTypeDlg", .control = "ChangeText", .type = "Text", .x = 105, .y = 78, .width = 230, .height = 20, .attributes = 3, .text = "Allows users to change the way features are installed." },
		{.dialog = "MaintenanceTypeDlg", .control = "RemoveButton", .type = "PushButton", .x = 50, .y = 163, .width = 38, .height = 38, .attributes = 5767171, .text = "[RemoveIcon]", .control_Next = "Back", .help = "Remove Installation|" },
		{.dialog = "MaintenanceTypeDlg", .control = "RemoveLabel", .type = "Text", .x = 105, .y = 163, .width = 100, .height = 10, .attributes = 3, .text = "[DlgTitleFont]&Remove", .control_Next = "RemoveButton" },
		{.dialog = "MaintenanceTypeDlg", .control = "RemoveText", .type = "Text", .x = 105, .y = 176, .width = 230, .height = 20, .attributes = 3, .text = "Removes [ProductName] from your computer." },
		{.dialog = "MaintenanceTypeDlg", .control = "RepairButton", .type = "PushButton", .x = 50, .y = 114, .width = 38, .height = 38, .attributes = 5767171, .text = "[RepairIcon]", .control_Next = "RemoveLabel", .help = "Repair Installation|" },
		{.dialog = "MaintenanceTypeDlg", .control = "RepairText", .type = "Text", .x = 105, .y = 127, .width = 230, .height = 30, .attributes = 3, .text = "Repairs errors in the most recent installation state - fixes missing or corrupt files, shortcuts and registry entries." },
	};

	controlEvent =
	{
		{.dialog = "MaintenanceTypeDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{.dialog = "MaintenanceTypeDlg", .control = "Back", .event = "NewDialog", .argument = "MaintenanceWelcomeDlg", .condition = "1" },
		{.dialog = "MaintenanceTypeDlg", .control = "ChangeButton", .event = "NewDialog", .argument = "CustomizeDlg", .condition = "1", .ordering = 4 },
		{.dialog = "MaintenanceTypeDlg", .control = "ChangeButton", .event = "[InstallMode]", .argument = "Change", .condition = "1", .ordering = 1 },
		{.dialog = "MaintenanceTypeDlg", .control = "ChangeButton", .event = "[Progress1]", .argument = "Changing", .condition = "1", .ordering = 2 },
		{.dialog = "MaintenanceTypeDlg", .control = "ChangeButton", .event = "[Progress2]", .argument = "changes", .condition = "1", .ordering = 3 },
		{.dialog = "MaintenanceTypeDlg", .control = "RemoveButton", .event = "NewDialog", .argument = "VerifyRemoveDlg", .condition = "1", .ordering = 4 },
		{.dialog = "MaintenanceTypeDlg", .control = "RemoveButton", .event = "[InstallMode]", .argument = "Remove", .condition = "1", .ordering = 1 },
		{.dialog = "MaintenanceTypeDlg", .control = "RemoveButton", .event = "[Progress1]", .argument = "Removing", .condition = "1", .ordering = 2 },
		{.dialog = "MaintenanceTypeDlg", .control = "RemoveButton", .event = "[Progress2]", .argument = "removes", .condition = "1", .ordering = 3 },
		{.dialog = "MaintenanceTypeDlg", .control = "RepairButton", .event = "NewDialog", .argument = "VerifyRepairDlg", .condition = "1", .ordering = 4 },
		{.dialog = "MaintenanceTypeDlg", .control = "RepairButton", .event = "[InstallMode]", .argument = "Repair", .condition = "1", .ordering = 1 },
		{.dialog = "MaintenanceTypeDlg", .control = "RepairButton", .event = "[Progress1]", .argument = "Repairing", .condition = "1", .ordering = 2 },
		{.dialog = "MaintenanceTypeDlg", .control = "RepairButton", .event = "[Progress2]", .argument = "repairs", .condition = "1", .ordering = 3 },
	};
}
