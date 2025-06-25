
#include "Precomp.h"
#include "MsiRMFilesInUse.h"

MsiRMFilesInUse::MsiRMFilesInUse()
{
	dialog =
	{
		.dialog = "MsiRMFilesInUse",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 19,
		.title = "[ProductName] [Setup]",
		.control_First = "OK",
		.control_Default = "OK",
		.control_Cancel = "Cancel"
	};

	controls =
	{
		{.dialog = "MsiRMFilesInUse", .control = "Text", .type = "Text", .x = 20, .y = 55, .width = 330, .height = 30, .attributes = 3, .text = "The following applications are using files that need to be updated by this setup." },
		{.dialog = "MsiRMFilesInUse", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Options" },
		{.dialog = "MsiRMFilesInUse", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "MsiRMFilesInUse", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "MsiRMFilesInUse", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{.dialog = "MsiRMFilesInUse", .control = "Description", .type = "Text", .x = 20, .y = 23, .width = 280, .height = 20, .attributes = 196611, .text = "Some files that need to be updated are currently in use." },
		{.dialog = "MsiRMFilesInUse", .control = "OK", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_OK]", .control_Next = "Cancel" },
		{.dialog = "MsiRMFilesInUse", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 25, .attributes = 196611, .text = "[DlgTitleFont]Files in Use" },
		{.dialog = "MsiRMFilesInUse", .control = "List", .type = "ListBox", .x = 20, .y = 85, .width = 330, .height = 89, .attributes = 7, .property = "FileInUseProcess" },
		{.dialog = "MsiRMFilesInUse", .control = "Options", .type = "RadioButtonGroup", .x = 20, .y = 172, .width = 330, .height = 54, .attributes = 3, .property = "AppsShutdownOption", .control_Next = "OK" },
	};

	controlEvent =
	{
		{.dialog = "MsiRMFilesInUse", .control = "Cancel", .event = "EndDialog", .argument = "Exit", .condition = "1" },
		{.dialog = "MsiRMFilesInUse", .control = "OK", .event = "EndDialog", .argument = "Return", .condition = "1", .ordering = 2 },
		{.dialog = "MsiRMFilesInUse", .control = "OK", .event = "RMShutdownAndRestart", .argument = "0", .condition = "AppsShutdownOption = \"All\"", .ordering = 1 },
	};
}
