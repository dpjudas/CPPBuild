
#include "Precomp.h"
#include "CancelDlg.h"

CancelDlg::CancelDlg()
{
	dialog =
	{
		.dialog = "CancelDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 260,
		.height = 85,
		.attributes = 3,
		.title = "[ProductName] [Setup]",
		.control_First = "No",
		.control_Default = "No",
		.control_Cancel = "No"
	};

	controls =
	{
		{.dialog = "CancelDlg", .control = "Text", .type = "Text", .x = 48, .y = 15, .width = 194, .height = 30, .attributes = 3, .text = "Are you sure you want to cancel [ProductName] installation?" },
		{.dialog = "CancelDlg", .control = "Icon", .type = "Icon", .x = 15, .y = 15, .width = 24, .height = 24, .attributes = 5242881, .text = "[InfoIcon]", .help = "Information icon|" },
		{.dialog = "CancelDlg", .control = "No", .type = "PushButton", .x = 132, .y = 57, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_No]", .control_Next = "Yes" },
		{.dialog = "CancelDlg", .control = "Yes", .type = "PushButton", .x = 72, .y = 57, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Yes]", .control_Next = "No" },
	};

	controlEvent =
	{
		{.dialog = "CancelDlg", .control = "No", .event = "EndDialog", .argument = "Return", .condition = "1" },
		{.dialog = "CancelDlg", .control = "Yes", .event = "EndDialog", .argument = "Exit", .condition = "1" },
	};
}
