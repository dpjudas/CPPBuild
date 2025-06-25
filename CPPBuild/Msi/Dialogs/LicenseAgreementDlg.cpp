
#include "Precomp.h"
#include "LicenseAgreementDlg.h"

LicenseAgreementDlg::LicenseAgreementDlg()
{
	dialog =
	{
		.dialog = "LicenseAgreementDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 3,
		.title = "[ProductName] License Agreement",
		.control_First = "Buttons",
		.control_Default = "Next",
		.control_Cancel = "Cancel"
	};

	controls =
	{
		{.dialog = "LicenseAgreementDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "AgreementText" },
		{.dialog = "LicenseAgreementDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "LicenseAgreementDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "LicenseAgreementDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{.dialog = "LicenseAgreementDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "Please read the following license agreement carefully" },
		{.dialog = "LicenseAgreementDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]End-User License Agreement" },
		{.dialog = "LicenseAgreementDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{.dialog = "LicenseAgreementDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{.dialog = "LicenseAgreementDlg", .control = "AgreementText", .type = "ScrollableText", .x = 20, .y = 60, .width = 330, .height = 120, .attributes = 7, .text = "{\\rtf1\\ansi\\ansicpg1252\\deff0\\deftab720{\\fonttbl{\\f0\\froman\\fprq2 Times New Roman;}}{\\colortbl\\red0\\green0\\blue0;} \\deflang1033\\horzdoc{\\*\\fchars }{\\*\\lchars }\\pard\\plain\\f0\\fs20 <Your license agreement should go here.>\\par }", .control_Next = "Buttons" },
		{.dialog = "LicenseAgreementDlg", .control = "Buttons", .type = "RadioButtonGroup", .x = 20, .y = 187, .width = 260, .height = 40, .attributes = 3, .property = "IAgree", .control_Next = "Print" },
		{.dialog = "LicenseAgreementDlg", .control = "Print", .type = "PushButton", .x = 290, .y = 195, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Print]", .control_Next = "Back" },
	};

	controlCondition =
	{
		{.dialog = "LicenseAgreementDlg", .control = "Next", .action = "Disable", .condition = "IAgree <> \"Yes\"" },
		{.dialog = "LicenseAgreementDlg", .control = "Next", .action = "Enable", .condition = "IAgree = \"Yes\"" },
		{.dialog = "LicenseAgreementDlg", .control = "Print", .action = "Hide", .condition = "VersionHandler < \"5.00\"" },
	};

	controlEvent =
	{
		{.dialog = "LicenseAgreementDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{.dialog = "LicenseAgreementDlg", .control = "Back", .event = "NewDialog", .argument = "WelcomeDlg", .condition = "VersionHandler < \"5.00\"" },
		{.dialog = "LicenseAgreementDlg", .control = "Back", .event = "NewDialog", .argument = "WelcomeDlgLink", .condition = "VersionHandler >= \"5.00\"" },
		{.dialog = "LicenseAgreementDlg", .control = "Next", .event = "NewDialog", .argument = "SetupTypeDlg", .condition = "IAgree = \"Yes\" AND ShowUserRegistrationDlg <> 1", .ordering = 3 },
		{.dialog = "LicenseAgreementDlg", .control = "Next", .event = "NewDialog", .argument = "UserRegistrationDlg", .condition = "IAgree = \"Yes\" AND ShowUserRegistrationDlg = 1", .ordering = 1 },
		{.dialog = "LicenseAgreementDlg", .control = "Next", .event = "SpawnWaitDialog", .argument = "WaitForCostingDlg", .condition = "CostingComplete = 1", .ordering = 2 },
		{.dialog = "LicenseAgreementDlg", .control = "Print", .event = "MsiPrint", .argument = "0", .condition = "VersionHandler >= \"5.00\"" },
	};
}
