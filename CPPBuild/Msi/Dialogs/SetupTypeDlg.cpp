
#include "Precomp.h"
#include "SetupTypeDlg.h"

SetupTypeDlg::SetupTypeDlg()
{
	dialog =
	{
		.dialog = "SetupTypeDlg",
		.hCentering = 50,
		.vCentering = 50,
		.width = 370,
		.height = 270,
		.attributes = 3,
		.title = "[ProductName] [Setup]",
		.control_First = "TypicalLabel",
		.control_Default = "TypicalButton",
		.control_Cancel = "Cancel"
	};

	controls =
	{
		{.dialog = "SetupTypeDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "TypicalLabel" },
		{.dialog = "SetupTypeDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "SetupTypeDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{.dialog = "SetupTypeDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{.dialog = "SetupTypeDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "Choose the setup type that best suits your needs" },
		{.dialog = "SetupTypeDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Choose Setup Type" },
		{.dialog = "SetupTypeDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{.dialog = "SetupTypeDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{.dialog = "SetupTypeDlg", .control = "CompleteButton", .type = "PushButton", .x = 35, .y = 171, .width = 38, .height = 38, .attributes = 5767171, .text = "[CompleteSetupIcon]", .control_Next = "Back", .help = "Complete Installation|" },
		{.dialog = "SetupTypeDlg", .control = "CompleteLabel", .type = "Text", .x = 105, .y = 174, .width = 100, .height = 10, .attributes = 3, .text = "[DlgTitleFont]C&omplete", .control_Next = "CompleteButton" },
		{.dialog = "SetupTypeDlg", .control = "CompleteText", .type = "Text", .x = 105, .y = 187, .width = 230, .height = 20, .attributes = 3, .text = "All program features will be installed.  (Requires most disk space)" },
		{.dialog = "SetupTypeDlg", .control = "CustomButton", .type = "PushButton", .x = 35, .y = 118, .width = 38, .height = 38, .attributes = 5767171, .text = "[CustomSetupIcon]", .control_Next = "CompleteLabel", .help = "Custom Installation|" },
		{.dialog = "SetupTypeDlg", .control = "CustomLabel", .type = "Text", .x = 105, .y = 121, .width = 100, .height = 10, .attributes = 3, .text = "[DlgTitleFont]C&ustom", .control_Next = "CustomButton" },
		{.dialog = "SetupTypeDlg", .control = "CustomText", .type = "Text", .x = 105, .y = 134, .width = 230, .height = 30, .attributes = 3, .text = "Allows users to choose which program features will be installed and where they will be installed. Recommended for advanced users." },
		{.dialog = "SetupTypeDlg", .control = "TypicalButton", .type = "PushButton", .x = 35, .y = 65, .width = 38, .height = 38, .attributes = 5767171, .text = "[InstallerIcon]", .control_Next = "CustomLabel", .help = "Typical Installation|" },
		{.dialog = "SetupTypeDlg", .control = "TypicalLabel", .type = "Text", .x = 105, .y = 68, .width = 100, .height = 10, .attributes = 3, .text = "[DlgTitleFont]&Typical", .control_Next = "TypicalButton" },
		{.dialog = "SetupTypeDlg", .control = "TypicalText", .type = "Text", .x = 105, .y = 81, .width = 230, .height = 20, .attributes = 3, .text = "Installs the most common program features. Recommended for most users." },
	};

	controlEvent =
	{
		{.dialog = "SetupTypeDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{.dialog = "SetupTypeDlg", .control = "Back", .event = "NewDialog", .argument = "LicenseAgreementDlg", .condition = "ShowUserRegistrationDlg <> 1" },
		{.dialog = "SetupTypeDlg", .control = "Back", .event = "NewDialog", .argument = "UserRegistrationDlg", .condition = "ShowUserRegistrationDlg = 1" },
		{.dialog = "SetupTypeDlg", .control = "CompleteButton", .event = "NewDialog", .argument = "VerifyReadyDlg", .condition = "1", .ordering = 3 },
		{.dialog = "SetupTypeDlg", .control = "CompleteButton", .event = "[InstallMode]", .argument = "Complete", .condition = "1", .ordering = 1 },
		{.dialog = "SetupTypeDlg", .control = "CompleteButton", .event = "SetInstallLevel", .argument = "1000", .condition = "1", .ordering = 2 },
		{.dialog = "SetupTypeDlg", .control = "CustomButton", .event = "NewDialog", .argument = "CustomizeDlg", .condition = "1", .ordering = 2 },
		{.dialog = "SetupTypeDlg", .control = "CustomButton", .event = "[InstallMode]", .argument = "Custom", .condition = "1", .ordering = 1 },
		{.dialog = "SetupTypeDlg", .control = "TypicalButton", .event = "NewDialog", .argument = "VerifyReadyDlg", .condition = "1", .ordering = 3 },
		{.dialog = "SetupTypeDlg", .control = "TypicalButton", .event = "[InstallMode]", .argument = "Typical", .condition = "1", .ordering = 1 },
		{.dialog = "SetupTypeDlg", .control = "TypicalButton", .event = "SetInstallLevel", .argument = "3", .condition = "1", .ordering = 2 },
	};
}
