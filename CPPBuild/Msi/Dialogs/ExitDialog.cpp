
#include "Precomp.h"
#include "ExitDialog.h"

ExitDialog::ExitDialog()
{
	dialog =
	{
		.dialog = "ExitDialog",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 3,
		.title = "[ProductName] [Setup]",
		.control_First = "Finish",
		.control_Default = "Finish",
		.control_Cancel = "Finish"
	};

	controls =
	{
		{.dialog = "ExitDialog", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "LaunchAppBox" },
		{.dialog = "ExitDialog", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "ExitDialog", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{.dialog = "ExitDialog", .control = "Description", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 20, .attributes = 196611, .text = "Click the Finish button to exit the [Wizard]." },
		{.dialog = "ExitDialog", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}Completing the [ProductName] [Wizard]" },
		{.dialog = "ExitDialog", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Finish" },
		{.dialog = "ExitDialog", .control = "Finish", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Finish]", .control_Next = "Cancel" },
		{.dialog = "ExitDialog", .control = "LaunchAppBox", .type = "CheckBox", .x = 135, .y = 120, .width = 140, .height = 20, .attributes = 3, .property = "LaunchApp", .text = "Launch Application When finished", .control_Next = "Back" },
	};

	controlCondition =
	{
		{.dialog = "ExitDialog", .control = "LaunchAppBox", .action = "Hide", .condition = "Installed" },
		{.dialog = "ExitDialog", .control = "LaunchAppBox", .action = "Hide", .condition = "VersionHandler < \"5.00\"" },
	};

	controlEvent =
	{
		{.dialog = "ExitDialog", .control = "Finish", .event = "EndDialog", .argument = "Return", .condition = "1" },
	};
}
