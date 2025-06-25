
#include "Precomp.h"
#include "OutOfRbDiskDlg.h"

OutOfRbDiskDlg::OutOfRbDiskDlg()
{
	dialog =
	{
		.dialog = "OutOfRbDiskDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 3,
		.title = "[ProductName] [Setup]",
		.control_First = "No",
		.control_Default = "No",
		.control_Cancel = "No"
	};

	controls =
	{
		{.dialog = "OutOfRbDiskDlg", .control = "Text", .type = "Text", .x = 20, .y = 53, .width = 330, .height = 40, .attributes = 3, .text = "The highlighted volumes do not have enough disk space available for the currently selected features.  You can either remove some files from the highlighted volumes, or choose to install less features onto local drive(s), or select different destination drive(s)." },
		{.dialog = "OutOfRbDiskDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "No" },
		{.dialog = "OutOfRbDiskDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "OutOfRbDiskDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "OutOfRbDiskDlg", .control = "Description", .type = "Text", .x = 20, .y = 20, .width = 280, .height = 20, .attributes = 196611, .text = "Disk space required for the installation exceeds available disk space." },
		{.dialog = "OutOfRbDiskDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Out of Disk Space" },
		{.dialog = "OutOfRbDiskDlg", .control = "No", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_No]", .control_Next = "Yes" },
		{.dialog = "OutOfRbDiskDlg", .control = "Yes", .type = "PushButton", .x = 240, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Yes]", .control_Next = "BannerBitmap" },
		{.dialog = "OutOfRbDiskDlg", .control = "VolumeList", .type = "VolumeCostList", .x = 20, .y = 140, .width = 330, .height = 80, .attributes = 4587527, .text = "{120}{70}{70}{70}{70}" },
		{.dialog = "OutOfRbDiskDlg", .control = "Text2", .type = "Text", .x = 20, .y = 94, .width = 330, .height = 40, .attributes = 3, .text = "Alternatively, you may choose to disable the installer's rollback functionality.  This allows the installer to restore your computer's original state should the installation be interrupted in any way.  Click Yes if you wish to take the risk to disable rollback." },
	};

	controlEvent =
	{
		{.dialog = "OutOfRbDiskDlg", .control = "No", .event = "EndDialog", .argument = "Return", .condition = "1" },
		{.dialog = "OutOfRbDiskDlg", .control = "Yes", .event = "EndDialog", .argument = "Return", .condition = "1", .ordering = 2 },
		{.dialog = "OutOfRbDiskDlg", .control = "Yes", .event = "EnableRollback", .argument = "False", .condition = "1", .ordering = 1 },
	};
}
