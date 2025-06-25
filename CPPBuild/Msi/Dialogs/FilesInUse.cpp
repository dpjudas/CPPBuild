
#include "Precomp.h"
#include "FilesInUse.h"

FilesInUse::FilesInUse()
{
	dialog =
	{
		.dialog = "FilesInUse",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 19,
		.title = "[ProductName] [Setup]",
		.control_First = "Retry",
		.control_Default = "Retry",
		.control_Cancel = "Retry"
	};

	controls =
	{
		{.dialog = "FilesInUse", .control = "Text", .type = "Text", .x = 20, .y = 55, .width = 330, .height = 30, .attributes = 3, .text = "The following applications are using files that need to be updated by this setup. Close these applications and then click Retry to continue the installation or Cancel to exit it." },
		{.dialog = "FilesInUse", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Retry" },
		{.dialog = "FilesInUse", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "FilesInUse", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "FilesInUse", .control = "Description", .type = "Text", .x = 20, .y = 23, .width = 280, .height = 20, .attributes = 196611, .text = "Some files that need to be updated are currently in use." },
		{.dialog = "FilesInUse", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Files in Use" },
		{.dialog = "FilesInUse", .control = "Retry", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Retry]", .control_Next = "Ignore" },
		{.dialog = "FilesInUse", .control = "Exit", .type = "PushButton", .x = 166, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Exit]", .control_Next = "BannerBitmap" },
		{.dialog = "FilesInUse", .control = "Ignore", .type = "PushButton", .x = 235, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Ignore]", .control_Next = "Exit" },
		{.dialog = "FilesInUse", .control = "List", .type = "ListBox", .x = 20, .y = 87, .width = 330, .height = 130, .attributes = 7, .property = "FileInUseProcess" },
	};

	controlEvent =
	{
		{.dialog = "FilesInUse", .control = "Retry", .event = "EndDialog", .argument = "Retry", .condition = "1" },
		{.dialog = "FilesInUse", .control = "Exit", .event = "EndDialog", .argument = "Exit", .condition = "1" },
		{.dialog = "FilesInUse", .control = "Ignore", .event = "EndDialog", .argument = "Ignore", .condition = "1" },
	};
}
