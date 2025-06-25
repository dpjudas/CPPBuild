
#include "Precomp.h"
#include "AdminRegistrationDlg.h"

AdminRegistrationDlg::AdminRegistrationDlg()
{
	dialog = { .dialog = "AdminRegistrationDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "OrganizationLabel", .control_Default = "Next", .control_Cancel = "Cancel" };

	controls =
	{
		{.dialog = "AdminRegistrationDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "OrganizationLabel" },
		{.dialog = "AdminRegistrationDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "AdminRegistrationDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "AdminRegistrationDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{.dialog = "AdminRegistrationDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "Please enter your company information" },
		{.dialog = "AdminRegistrationDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Company Information" },
		{.dialog = "AdminRegistrationDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 65539, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{.dialog = "AdminRegistrationDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{.dialog = "AdminRegistrationDlg", .control = "OrganizationLabel", .type = "Text", .x = 45, .y = 71, .width = 285, .height = 30, .attributes = 3, .text = "&Please enter the name of your organization in the box below.  This will be used as default company name for subsequent installations of [ProductName]:", .control_Next = "OrganizationEdit" },
		{.dialog = "AdminRegistrationDlg", .control = "CDKeyEdit", .type = "MaskedEdit", .x = 45, .y = 143, .width = 250, .height = 16, .attributes = 3, .property = "PIDKEY", .text = "[PIDTemplate]", .control_Next = "Back" },
		{.dialog = "AdminRegistrationDlg", .control = "CDKeyLabel", .type = "Text", .x = 45, .y = 130, .width = 50, .height = 10, .attributes = 3, .text = "CD &Key:", .control_Next = "CDKeyEdit" },
		{.dialog = "AdminRegistrationDlg", .control = "OrganizationEdit", .type = "Edit", .x = 45, .y = 105, .width = 220, .height = 18, .attributes = 3, .property = "COMPANYNAME", .text = "{80}", .control_Next = "CDKeyLabel" },
	};

	controlEvent =
	{
		{.dialog = "AdminRegistrationDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{.dialog = "AdminRegistrationDlg", .control = "Back", .event = "NewDialog", .argument = "AdminWelcomeDlg", .condition = "1" },
		{.dialog = "AdminRegistrationDlg", .control = "Next", .event = "NewDialog", .argument = "AdminInstallPointDlg", .condition = "ProductID", .ordering = 2 },
		{.dialog = "AdminRegistrationDlg", .control = "Next", .event = "ValidateProductID", .argument = "0", .condition = "0", .ordering = 1 },
	};
}
