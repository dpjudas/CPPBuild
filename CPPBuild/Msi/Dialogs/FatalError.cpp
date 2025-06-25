
#include "Precomp.h"
#include "FatalError.h"

FatalError::FatalError()
{
	dialog =
	{
		.dialog = "FatalError",
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
		{.dialog = "FatalError", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "Back" },
		{.dialog = "FatalError", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "FatalError", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{.dialog = "FatalError", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}[ProductName] [Wizard] ended prematurely" },
		{.dialog = "FatalError", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Finish" },
		{.dialog = "FatalError", .control = "Finish", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Finish]", .control_Next = "Cancel" },
		{.dialog = "FatalError", .control = "Description1", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 40, .attributes = 196611, .text = "[ProductName] setup ended prematurely because of an error.  Your system has not been modified.  To install this program at a later time, please run the installation again." },
		{.dialog = "FatalError", .control = "Description2", .type = "Text", .x = 135, .y = 115, .width = 220, .height = 20, .attributes = 196611, .text = "Click the Finish button to exit the [Wizard]." },
	};

	controlEvent =
	{
		{.dialog = "FatalError", .control = "Finish", .event = "EndDialog", .argument = "Exit", .condition = "1" },
	};
}
