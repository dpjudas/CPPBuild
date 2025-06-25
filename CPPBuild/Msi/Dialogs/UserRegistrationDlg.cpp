
#include "Precomp.h"
#include "UserRegistrationDlg.h"

UserRegistrationDlg::UserRegistrationDlg()
{
	dialog =
	{
		.dialog = "UserRegistrationDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 3,
		.title = "[ProductName] [Setup]",
		.control_First = "NameLabel",
		.control_Default = "Next",
		.control_Cancel = "Cancel"
	};

	controls =
	{
		{.dialog = "UserRegistrationDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "NameLabel" },
		{.dialog = "UserRegistrationDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "UserRegistrationDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "UserRegistrationDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{.dialog = "UserRegistrationDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "Please enter your customer information" },
		{.dialog = "UserRegistrationDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Customer Information" },
		{.dialog = "UserRegistrationDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{.dialog = "UserRegistrationDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{.dialog = "UserRegistrationDlg", .control = "OrganizationLabel", .type = "Text", .x = 45, .y = 110, .width = 100, .height = 15, .attributes = 3, .text = "&Organization:", .control_Next = "OrganizationEdit" },
		{.dialog = "UserRegistrationDlg", .control = "CDKeyEdit", .type = "MaskedEdit", .x = 45, .y = 159, .width = 250, .height = 16, .attributes = 3, .property = "PIDKEY", .text = "[PIDTemplate]", .control_Next = "Back" },
		{.dialog = "UserRegistrationDlg", .control = "CDKeyLabel", .type = "Text", .x = 45, .y = 147, .width = 50, .height = 10, .attributes = 3, .text = "CD &Key:", .control_Next = "CDKeyEdit" },
		{.dialog = "UserRegistrationDlg", .control = "OrganizationEdit", .type = "Edit", .x = 45, .y = 122, .width = 220, .height = 18, .attributes = 3, .property = "COMPANYNAME", .text = "{80}", .control_Next = "CDKeyLabel" },
		{.dialog = "UserRegistrationDlg", .control = "NameLabel", .type = "Text", .x = 45, .y = 73, .width = 100, .height = 15, .attributes = 3, .text = "&User Name:", .control_Next = "NameEdit" },
		{.dialog = "UserRegistrationDlg", .control = "NameEdit", .type = "Edit", .x = 45, .y = 85, .width = 220, .height = 18, .attributes = 3, .property = "USERNAME", .text = "{80}", .control_Next = "OrganizationLabel" },
	};

	controlEvent =
	{
		{.dialog = "UserRegistrationDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{.dialog = "UserRegistrationDlg", .control = "Back", .event = "NewDialog", .argument = "LicenseAgreementDlg", .condition = "1" },
		{.dialog = "UserRegistrationDlg", .control = "Next", .event = "NewDialog", .argument = "SetupTypeDlg", .condition = "ProductID", .ordering = 3 },
		{.dialog = "UserRegistrationDlg", .control = "Next", .event = "ValidateProductID", .argument = "0", .condition = "0", .ordering = 1 },
		{.dialog = "UserRegistrationDlg", .control = "Next", .event = "SpawnWaitDialog", .argument = "WaitForCostingDlg", .condition = "CostingComplete = 1", .ordering = 2 },
	};
}
