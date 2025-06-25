
#include "Precomp.h"
#include "DiskCostDlg.h"

DiskCostDlg::DiskCostDlg()
{
	dialog =
	{
		.dialog = "DiskCostDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 3,
		.title = "[ProductName] [Setup]",
		.control_First = "OK",
		.control_Default = "OK",
		.control_Cancel = "OK"
	};

	controls =
	{
		{.dialog = "DiskCostDlg", .control = "Text", .type = "Text", .x = 20, .y = 53, .width = 330, .height = 40, .attributes = 3, .text = "The highlighted volumes (if any) do not have enough disk space available for the currently selected features.  You can either remove some files from the highlighted volumes, or choose to install less features onto local drive(s), or select different destination drive(s)." },
		{.dialog = "DiskCostDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "OK" },
		{.dialog = "DiskCostDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "DiskCostDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "DiskCostDlg", .control = "Description", .type = "Text", .x = 20, .y = 20, .width = 280, .height = 20, .attributes = 196611, .text = "The disk space required for the installation of the selected features." },
		{.dialog = "DiskCostDlg", .control = "OK", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_OK]", .control_Next = "BannerBitmap" },
		{.dialog = "DiskCostDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Disk Space Requirements" },
		{.dialog = "DiskCostDlg", .control = "VolumeList", .type = "VolumeCostList", .x = 20, .y = 100, .width = 330, .height = 120, .attributes = 393223, .text = "{120}{70}{70}{70}{70}" },
	};

	controlEvent =
	{
		{.dialog = "DiskCostDlg", .control = "OK", .event = "EndDialog", .argument = "Return", .condition = "1" },
	};
}
