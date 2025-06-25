
#include "Precomp.h"
#include "ErrorDlg.h"

ErrorDlg::ErrorDlg()
{
	dialog =
	{
		.dialog = "ErrorDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 270,
		.height = 105,
		.attributes = 65539,
		.title = "Installer Information",
		.control_First = "ErrorText"
	};

	controls =
	{
		{.dialog = "ErrorDlg", .control = "Y", .type = "PushButton", .x = 100, .y = 80, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Yes]" },
		{.dialog = "ErrorDlg", .control = "A", .type = "PushButton", .x = 100, .y = 80, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]" },
		{.dialog = "ErrorDlg", .control = "C", .type = "PushButton", .x = 100, .y = 80, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]" },
		{.dialog = "ErrorDlg", .control = "ErrorIcon", .type = "Icon", .x = 15, .y = 15, .width = 24, .height = 24, .attributes = 5242881, .text = "[InfoIcon]", .help = "Information icon|" },
		{.dialog = "ErrorDlg", .control = "ErrorText", .type = "Text", .x = 48, .y = 15, .width = 205, .height = 60, .attributes = 3, .text = "Information text" },
		{.dialog = "ErrorDlg", .control = "I", .type = "PushButton", .x = 100, .y = 80, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Ignore]" },
		{.dialog = "ErrorDlg", .control = "N", .type = "PushButton", .x = 100, .y = 80, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_No]" },
		{.dialog = "ErrorDlg", .control = "O", .type = "PushButton", .x = 100, .y = 80, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_OK]" },
		{.dialog = "ErrorDlg", .control = "R", .type = "PushButton", .x = 100, .y = 80, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Retry]" },
	};

	controlEvent =
	{
		{.dialog = "ErrorDlg", .control = "Y", .event = "EndDialog", .argument = "ErrorYes", .condition = "1" },
		{.dialog = "ErrorDlg", .control = "A", .event = "EndDialog", .argument = "ErrorAbort", .condition = "1" },
		{.dialog = "ErrorDlg", .control = "C", .event = "EndDialog", .argument = "ErrorCancel", .condition = "1" },
		{.dialog = "ErrorDlg", .control = "I", .event = "EndDialog", .argument = "ErrorIgnore", .condition = "1" },
		{.dialog = "ErrorDlg", .control = "N", .event = "EndDialog", .argument = "ErrorNo", .condition = "1" },
		{.dialog = "ErrorDlg", .control = "O", .event = "EndDialog", .argument = "ErrorOk", .condition = "1" },
		{.dialog = "ErrorDlg", .control = "R", .event = "EndDialog", .argument = "ErrorRetry", .condition = "1" },
	};
}
