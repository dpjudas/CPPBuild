#pragma once

#include "MSISchema.h"

class AdminWelcomeDlg
{
public:
	MSIDialog dialog = { .dialog = "AdminWelcomeDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "Next", .control_Default = "Next", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "AdminWelcomeDlg", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "Back" },
		{ .dialog = "AdminWelcomeDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "AdminWelcomeDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{ .dialog = "AdminWelcomeDlg", .control = "Description", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 30, .attributes = 196611, .text = "The [Wizard] will create a server image of [ProductName], at a specified network location.  Click Next to continue or Cancel to exit the [Wizard]." },
		{ .dialog = "AdminWelcomeDlg", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}Welcome to the [ProductName] [Wizard]" },
		{ .dialog = "AdminWelcomeDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{ .dialog = "AdminWelcomeDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "AdminWelcomeDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "AdminWelcomeDlg", .control = "Next", .event = "NewDialog", .argument = "AdminRegistrationDlg", .condition = "1", .ordering = 2 },
		{ .dialog = "AdminWelcomeDlg", .control = "Next", .event = "[InstallMode]", .argument = "Server Image", .condition = "1", .ordering = 1 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class ExitDialog
{
public:
	MSIDialog dialog = { .dialog = "ExitDialog", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "Finish", .control_Default = "Finish", .control_Cancel = "Finish" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "ExitDialog", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "LaunchAppBox" },
		{ .dialog = "ExitDialog", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "ExitDialog", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{ .dialog = "ExitDialog", .control = "Description", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 20, .attributes = 196611, .text = "Click the Finish button to exit the [Wizard]." },
		{ .dialog = "ExitDialog", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}Completing the [ProductName] [Wizard]" },
		{ .dialog = "ExitDialog", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Finish" },
		{ .dialog = "ExitDialog", .control = "Finish", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Finish]", .control_Next = "Cancel" },
		{ .dialog = "ExitDialog", .control = "LaunchAppBox", .type = "CheckBox", .x = 135, .y = 120, .width = 140, .height = 20, .attributes = 3, .property = "LaunchApp", .text = "Launch Application When finished", .control_Next = "Back" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
		{ .dialog = "ExitDialog", .control = "LaunchAppBox", .action = "Hide", .condition = "Installed" },
		{ .dialog = "ExitDialog", .control = "LaunchAppBox", .action = "Hide", .condition = "VersionHandler < \"5.00\"" },
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "ExitDialog", .control = "Finish", .event = "EndDialog", .argument = "Return", .condition = "1" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class FatalError
{
public:
	MSIDialog dialog = { .dialog = "FatalError", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "Finish", .control_Default = "Finish", .control_Cancel = "Finish" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "FatalError", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "Back" },
		{ .dialog = "FatalError", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "FatalError", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{ .dialog = "FatalError", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}[ProductName] [Wizard] ended prematurely" },
		{ .dialog = "FatalError", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Finish" },
		{ .dialog = "FatalError", .control = "Finish", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Finish]", .control_Next = "Cancel" },
		{ .dialog = "FatalError", .control = "Description1", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 40, .attributes = 196611, .text = "[ProductName] setup ended prematurely because of an error.  Your system has not been modified.  To install this program at a later time, please run the installation again." },
		{ .dialog = "FatalError", .control = "Description2", .type = "Text", .x = 135, .y = 115, .width = 220, .height = 20, .attributes = 196611, .text = "Click the Finish button to exit the [Wizard]." },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "FatalError", .control = "Finish", .event = "EndDialog", .argument = "Exit", .condition = "1" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class PrepareDlg
{
public:
	MSIDialog dialog = { .dialog = "PrepareDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 1, .title = "[ProductName] [Setup]", .control_First = "Cancel", .control_Default = "Cancel", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "PrepareDlg", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "Cancel" },
		{ .dialog = "PrepareDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "PrepareDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{ .dialog = "PrepareDlg", .control = "Description", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 20, .attributes = 196611, .text = "Please wait while the [Wizard] prepares to guide you through the installation." },
		{ .dialog = "PrepareDlg", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}Welcome to the [ProductName] [Wizard]" },
		{ .dialog = "PrepareDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]" },
		{ .dialog = "PrepareDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Next]" },
		{ .dialog = "PrepareDlg", .control = "ActionData", .type = "Text", .x = 135, .y = 125, .width = 220, .height = 30, .attributes = 196611 },
		{ .dialog = "PrepareDlg", .control = "ActionText", .type = "Text", .x = 135, .y = 100, .width = 220, .height = 20, .attributes = 196611 },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "PrepareDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
		{ .dialog = "PrepareDlg", .control = "ActionData", .event = "ActionData", .attribute = "Text" },
		{ .dialog = "PrepareDlg", .control = "ActionText", .event = "ActionText", .attribute = "Text" },
	};
};

class ProgressDlg
{
public:
	MSIDialog dialog = { .dialog = "ProgressDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 1, .title = "[ProductName] [Setup]", .control_First = "Cancel", .control_Default = "Cancel", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "ProgressDlg", .control = "Text", .type = "Text", .x = 35, .y = 65, .width = 300, .height = 20, .attributes = 3, .text = "Please wait while the [Wizard] [Progress2] [ProductName].  This may take several minutes." },
		{ .dialog = "ProgressDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Back" },
		{ .dialog = "ProgressDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "ProgressDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "ProgressDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{ .dialog = "ProgressDlg", .control = "Title", .type = "Text", .x = 20, .y = 15, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont][Progress1] [ProductName]" },
		{ .dialog = "ProgressDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{ .dialog = "ProgressDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{ .dialog = "ProgressDlg", .control = "ActionText", .type = "Text", .x = 70, .y = 100, .width = 265, .height = 10, .attributes = 3 },
		{ .dialog = "ProgressDlg", .control = "ProgressBar", .type = "ProgressBar", .x = 35, .y = 115, .width = 300, .height = 10, .attributes = 65537, .text = "Progress done" },
		{ .dialog = "ProgressDlg", .control = "StatusLabel", .type = "Text", .x = 35, .y = 100, .width = 35, .height = 10, .attributes = 3, .text = "Status:" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "ProgressDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
		{ .dialog = "ProgressDlg", .control = "ActionText", .event = "ActionText", .attribute = "Text" },
		{ .dialog = "ProgressDlg", .control = "ProgressBar", .event = "SetProgress", .attribute = "Progress" },
	};
};

class UserExit
{
public:
	MSIDialog dialog = { .dialog = "UserExit", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "Finish", .control_Default = "Finish", .control_Cancel = "Finish" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "UserExit", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "Back" },
		{ .dialog = "UserExit", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "UserExit", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{ .dialog = "UserExit", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}[ProductName] [Wizard] was interrupted" },
		{ .dialog = "UserExit", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Finish" },
		{ .dialog = "UserExit", .control = "Finish", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Finish]", .control_Next = "Cancel" },
		{ .dialog = "UserExit", .control = "Description1", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 40, .attributes = 196611, .text = "[ProductName] setup was interrupted.  Your system has not been modified.  To install this program at a later time, please run the installation again." },
		{ .dialog = "UserExit", .control = "Description2", .type = "Text", .x = 135, .y = 115, .width = 220, .height = 20, .attributes = 196611, .text = "Click the Finish button to exit the [Wizard]." },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "UserExit", .control = "Finish", .event = "EndDialog", .argument = "Exit", .condition = "1" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class AdminBrowseDlg
{
public:
	MSIDialog dialog = { .dialog = "AdminBrowseDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "PathEdit", .control_Default = "OK", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "AdminBrowseDlg", .control = "Up", .type = "PushButton", .x = 298, .y = 55, .width = 19, .height = 19, .attributes = 3670019, .text = "Up", .control_Next = "NewFolder", .help = "Up One Level|" },
		{ .dialog = "AdminBrowseDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "PathEdit" },
		{ .dialog = "AdminBrowseDlg", .control = "PathEdit", .type = "PathEdit", .x = 84, .y = 202, .width = 261, .height = 17, .attributes = 3, .property = "TARGETDIR", .control_Next = "OK" },
		{ .dialog = "AdminBrowseDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "AdminBrowseDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "AdminBrowseDlg", .control = "Cancel", .type = "PushButton", .x = 240, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "ComboLabel" },
		{ .dialog = "AdminBrowseDlg", .control = "ComboLabel", .type = "Text", .x = 25, .y = 58, .width = 44, .height = 10, .attributes = 3, .text = "&Look in:", .control_Next = "DirectoryCombo" },
		{ .dialog = "AdminBrowseDlg", .control = "DirectoryCombo", .type = "DirectoryCombo", .x = 70, .y = 55, .width = 220, .height = 80, .attributes = 458755, .property = "TARGETDIR", .control_Next = "Up" },
		{ .dialog = "AdminBrowseDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "Browse to the destination folder" },
		{ .dialog = "AdminBrowseDlg", .control = "DirectoryList", .type = "DirectoryList", .x = 25, .y = 83, .width = 320, .height = 110, .attributes = 7, .property = "TARGETDIR", .control_Next = "PathLabel" },
		{ .dialog = "AdminBrowseDlg", .control = "PathLabel", .type = "Text", .x = 25, .y = 205, .width = 59, .height = 10, .attributes = 3, .text = "&Folder name:", .control_Next = "BannerBitmap" },
		{ .dialog = "AdminBrowseDlg", .control = "NewFolder", .type = "PushButton", .x = 325, .y = 55, .width = 19, .height = 19, .attributes = 3670019, .text = "New", .control_Next = "DirectoryList", .help = "Create A New Folder|" },
		{ .dialog = "AdminBrowseDlg", .control = "OK", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_OK]", .control_Next = "Cancel" },
		{ .dialog = "AdminBrowseDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Change current destination folder" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "AdminBrowseDlg", .control = "Up", .event = "DirectoryListUp", .argument = "0", .condition = "1" },
		{ .dialog = "AdminBrowseDlg", .control = "Cancel", .event = "Reset", .argument = "0", .condition = "1", .ordering = 1 },
		{ .dialog = "AdminBrowseDlg", .control = "Cancel", .event = "EndDialog", .argument = "Return", .condition = "1", .ordering = 2 },
		{ .dialog = "AdminBrowseDlg", .control = "NewFolder", .event = "DirectoryListNew", .argument = "0", .condition = "1" },
		{ .dialog = "AdminBrowseDlg", .control = "OK", .event = "EndDialog", .argument = "Return", .condition = "1", .ordering = 2 },
		{ .dialog = "AdminBrowseDlg", .control = "OK", .event = "SetTargetPath", .argument = "TARGETDIR", .condition = "1", .ordering = 1 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
		{ .dialog = "AdminBrowseDlg", .control = "DirectoryCombo", .event = "IgnoreChange", .attribute = "IgnoreChange" },
	};
};

class AdminInstallPointDlg
{
public:
	MSIDialog dialog = { .dialog = "AdminInstallPointDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "Text", .control_Default = "Next", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "AdminInstallPointDlg", .control = "Text", .type = "Text", .x = 25, .y = 80, .width = 320, .height = 10, .attributes = 3, .text = "&Enter a new network location or click Browse to browse to one.", .control_Next = "PathEdit" },
		{ .dialog = "AdminInstallPointDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Text" },
		{ .dialog = "AdminInstallPointDlg", .control = "PathEdit", .type = "PathEdit", .x = 25, .y = 93, .width = 320, .height = 18, .attributes = 3, .property = "TARGETDIR", .control_Next = "Browse" },
		{ .dialog = "AdminInstallPointDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "AdminInstallPointDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "AdminInstallPointDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{ .dialog = "AdminInstallPointDlg", .control = "Description", .type = "Text", .x = 25, .y = 20, .width = 280, .height = 20, .attributes = 196611, .text = "Please specify a network location for the server image of [ProductName] product" },
		{ .dialog = "AdminInstallPointDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Network Location" },
		{ .dialog = "AdminInstallPointDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{ .dialog = "AdminInstallPointDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{ .dialog = "AdminInstallPointDlg", .control = "Browse", .type = "PushButton", .x = 289, .y = 119, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Browse]", .control_Next = "Back" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "AdminInstallPointDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "AdminInstallPointDlg", .control = "Back", .event = "NewDialog", .argument = "AdminRegistrationDlg", .condition = "1" },
		{ .dialog = "AdminInstallPointDlg", .control = "Next", .event = "SetTargetPath", .argument = "TARGETDIR", .condition = "1", .ordering = 1 },
		{ .dialog = "AdminInstallPointDlg", .control = "Next", .event = "NewDialog", .argument = "VerifyReadyDlg", .condition = "1", .ordering = 2 },
		{ .dialog = "AdminInstallPointDlg", .control = "Browse", .event = "SpawnDialog", .argument = "AdminBrowseDlg", .condition = "1" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class AdminRegistrationDlg
{
public:
	MSIDialog dialog = { .dialog = "AdminRegistrationDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "OrganizationLabel", .control_Default = "Next", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "AdminRegistrationDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "OrganizationLabel" },
		{ .dialog = "AdminRegistrationDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "AdminRegistrationDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "AdminRegistrationDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{ .dialog = "AdminRegistrationDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "Please enter your company information" },
		{ .dialog = "AdminRegistrationDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Company Information" },
		{ .dialog = "AdminRegistrationDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 65539, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{ .dialog = "AdminRegistrationDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{ .dialog = "AdminRegistrationDlg", .control = "OrganizationLabel", .type = "Text", .x = 45, .y = 71, .width = 285, .height = 30, .attributes = 3, .text = "&Please enter the name of your organization in the box below.  This will be used as default company name for subsequent installations of [ProductName]:", .control_Next = "OrganizationEdit" },
		{ .dialog = "AdminRegistrationDlg", .control = "CDKeyEdit", .type = "MaskedEdit", .x = 45, .y = 143, .width = 250, .height = 16, .attributes = 3, .property = "PIDKEY", .text = "[PIDTemplate]", .control_Next = "Back" },
		{ .dialog = "AdminRegistrationDlg", .control = "CDKeyLabel", .type = "Text", .x = 45, .y = 130, .width = 50, .height = 10, .attributes = 3, .text = "CD &Key:", .control_Next = "CDKeyEdit" },
		{ .dialog = "AdminRegistrationDlg", .control = "OrganizationEdit", .type = "Edit", .x = 45, .y = 105, .width = 220, .height = 18, .attributes = 3, .property = "COMPANYNAME", .text = "{80}", .control_Next = "CDKeyLabel" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "AdminRegistrationDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "AdminRegistrationDlg", .control = "Back", .event = "NewDialog", .argument = "AdminWelcomeDlg", .condition = "1" },
		{ .dialog = "AdminRegistrationDlg", .control = "Next", .event = "NewDialog", .argument = "AdminInstallPointDlg", .condition = "ProductID", .ordering = 2 },
		{ .dialog = "AdminRegistrationDlg", .control = "Next", .event = "ValidateProductID", .argument = "0", .condition = "0", .ordering = 1 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class BrowseDlg
{
public:
	MSIDialog dialog = { .dialog = "BrowseDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "PathEdit", .control_Default = "OK", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "BrowseDlg", .control = "Up", .type = "PushButton", .x = 298, .y = 55, .width = 19, .height = 19, .attributes = 3670019, .text = "Up", .control_Next = "NewFolder", .help = "Up One Level|" },
		{ .dialog = "BrowseDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "PathEdit" },
		{ .dialog = "BrowseDlg", .control = "PathEdit", .type = "PathEdit", .x = 84, .y = 202, .width = 261, .height = 18, .attributes = 11, .property = "_BrowseProperty", .control_Next = "OK" },
		{ .dialog = "BrowseDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "BrowseDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "BrowseDlg", .control = "Cancel", .type = "PushButton", .x = 240, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "ComboLabel" },
		{ .dialog = "BrowseDlg", .control = "ComboLabel", .type = "Text", .x = 25, .y = 58, .width = 44, .height = 10, .attributes = 3, .text = "&Look in:", .control_Next = "DirectoryCombo" },
		{ .dialog = "BrowseDlg", .control = "DirectoryCombo", .type = "DirectoryCombo", .x = 70, .y = 55, .width = 220, .height = 80, .attributes = 393227, .property = "_BrowseProperty", .control_Next = "Up" },
		{ .dialog = "BrowseDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "Browse to the destination folder" },
		{ .dialog = "BrowseDlg", .control = "DirectoryList", .type = "DirectoryList", .x = 25, .y = 83, .width = 320, .height = 110, .attributes = 15, .property = "_BrowseProperty", .control_Next = "PathLabel" },
		{ .dialog = "BrowseDlg", .control = "PathLabel", .type = "Text", .x = 25, .y = 205, .width = 59, .height = 10, .attributes = 3, .text = "&Folder name:", .control_Next = "BannerBitmap" },
		{ .dialog = "BrowseDlg", .control = "NewFolder", .type = "PushButton", .x = 325, .y = 55, .width = 19, .height = 19, .attributes = 3670019, .text = "New", .control_Next = "DirectoryList", .help = "Create A New Folder|" },
		{ .dialog = "BrowseDlg", .control = "OK", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_OK]", .control_Next = "Cancel" },
		{ .dialog = "BrowseDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Change current destination folder" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "BrowseDlg", .control = "Up", .event = "DirectoryListUp", .argument = "0", .condition = "1" },
		{ .dialog = "BrowseDlg", .control = "Cancel", .event = "Reset", .argument = "0", .condition = "1", .ordering = 1 },
		{ .dialog = "BrowseDlg", .control = "Cancel", .event = "EndDialog", .argument = "Return", .condition = "1", .ordering = 2 },
		{ .dialog = "BrowseDlg", .control = "NewFolder", .event = "DirectoryListNew", .argument = "0", .condition = "1" },
		{ .dialog = "BrowseDlg", .control = "OK", .event = "EndDialog", .argument = "Return", .condition = "1", .ordering = 2 },
		{ .dialog = "BrowseDlg", .control = "OK", .event = "SetTargetPath", .argument = "[_BrowseProperty]", .condition = "1", .ordering = 1 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
		{ .dialog = "BrowseDlg", .control = "DirectoryCombo", .event = "IgnoreChange", .attribute = "IgnoreChange" },
	};
};

class CancelDlg
{
public:
	MSIDialog dialog = { .dialog = "CancelDlg", .hCentering = 50, .vCentering = 10, .width = 260, .height = 85, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "No", .control_Default = "No", .control_Cancel = "No" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "CancelDlg", .control = "Text", .type = "Text", .x = 48, .y = 15, .width = 194, .height = 30, .attributes = 3, .text = "Are you sure you want to cancel [ProductName] installation?" },
		{ .dialog = "CancelDlg", .control = "Icon", .type = "Icon", .x = 15, .y = 15, .width = 24, .height = 24, .attributes = 5242881, .text = "[InfoIcon]", .help = "Information icon|" },
		{ .dialog = "CancelDlg", .control = "No", .type = "PushButton", .x = 132, .y = 57, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_No]", .control_Next = "Yes" },
		{ .dialog = "CancelDlg", .control = "Yes", .type = "PushButton", .x = 72, .y = 57, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Yes]", .control_Next = "No" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "CancelDlg", .control = "No", .event = "EndDialog", .argument = "Return", .condition = "1" },
		{ .dialog = "CancelDlg", .control = "Yes", .event = "EndDialog", .argument = "Exit", .condition = "1" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class CustomizeDlg
{
public:
	MSIDialog dialog = { .dialog = "CustomizeDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 35, .title = "[ProductName] [Setup]", .control_First = "Tree", .control_Default = "Next", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "CustomizeDlg", .control = "Text", .type = "Text", .x = 25, .y = 55, .width = 320, .height = 20, .attributes = 3, .text = "Click on the icons in the tree below to change the way features will be installed." },
		{ .dialog = "CustomizeDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Tree" },
		{ .dialog = "CustomizeDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "CustomizeDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "CustomizeDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{ .dialog = "CustomizeDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "Select the way you want features to be installed." },
		{ .dialog = "CustomizeDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Custom Setup" },
		{ .dialog = "CustomizeDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{ .dialog = "CustomizeDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{ .dialog = "CustomizeDlg", .control = "Browse", .type = "PushButton", .x = 304, .y = 200, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Browse]", .control_Next = "Reset" },
		{ .dialog = "CustomizeDlg", .control = "Tree", .type = "SelectionTree", .x = 25, .y = 85, .width = 175, .height = 95, .attributes = 7, .property = "_BrowseProperty", .text = "Tree of selections", .control_Next = "Browse" },
		{ .dialog = "CustomizeDlg", .control = "Box", .type = "GroupBox", .x = 210, .y = 81, .width = 140, .height = 98, .attributes = 1 },
		{ .dialog = "CustomizeDlg", .control = "Reset", .type = "PushButton", .x = 42, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Reset]", .control_Next = "DiskCost" },
		{ .dialog = "CustomizeDlg", .control = "DiskCost", .type = "PushButton", .x = 111, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "Disk &Usage", .control_Next = "Back" },
		{ .dialog = "CustomizeDlg", .control = "ItemDescription", .type = "Text", .x = 215, .y = 90, .width = 131, .height = 30, .attributes = 3, .text = "Multiline description of the currently selected item." },
		{ .dialog = "CustomizeDlg", .control = "ItemSize", .type = "Text", .x = 215, .y = 130, .width = 131, .height = 45, .attributes = 3, .text = "The size of the currently selected item." },
		{ .dialog = "CustomizeDlg", .control = "Location", .type = "Text", .x = 75, .y = 200, .width = 215, .height = 20, .attributes = 3, .text = "<The selection's path>" },
		{ .dialog = "CustomizeDlg", .control = "LocationLabel", .type = "Text", .x = 25, .y = 200, .width = 50, .height = 10, .attributes = 3, .text = "Location:" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
		{ .dialog = "CustomizeDlg", .control = "Browse", .action = "Hide", .condition = "Installed" },
		{ .dialog = "CustomizeDlg", .control = "Location", .action = "Hide", .condition = "Installed" },
		{ .dialog = "CustomizeDlg", .control = "LocationLabel", .action = "Hide", .condition = "Installed" },
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "CustomizeDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "CustomizeDlg", .control = "Back", .event = "NewDialog", .argument = "MaintenanceTypeDlg", .condition = "InstallMode = \"Change\"" },
		{ .dialog = "CustomizeDlg", .control = "Back", .event = "NewDialog", .argument = "SetupTypeDlg", .condition = "InstallMode = \"Custom\"" },
		{ .dialog = "CustomizeDlg", .control = "Next", .event = "NewDialog", .argument = "VerifyReadyDlg", .condition = "1" },
		{ .dialog = "CustomizeDlg", .control = "Browse", .event = "SelectionBrowse", .argument = "BrowseDlg", .condition = "1" },
		{ .dialog = "CustomizeDlg", .control = "Reset", .event = "Reset", .argument = "0", .condition = "1" },
		{ .dialog = "CustomizeDlg", .control = "DiskCost", .event = "SpawnDialog", .argument = "DiskCostDlg", .condition = "1", .ordering = 2 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
		{ .dialog = "CustomizeDlg", .control = "Next", .event = "SelectionNoItems", .attribute = "Enabled" },
		{ .dialog = "CustomizeDlg", .control = "Reset", .event = "SelectionNoItems", .attribute = "Enabled" },
		{ .dialog = "CustomizeDlg", .control = "DiskCost", .event = "SelectionNoItems", .attribute = "Enabled" },
		{ .dialog = "CustomizeDlg", .control = "ItemDescription", .event = "SelectionDescription", .attribute = "Text" },
		{ .dialog = "CustomizeDlg", .control = "ItemSize", .event = "SelectionSize", .attribute = "Text" },
		{ .dialog = "CustomizeDlg", .control = "Location", .event = "SelectionPath", .attribute = "Text" },
		{ .dialog = "CustomizeDlg", .control = "Location", .event = "SelectionPathOn", .attribute = "Visible" },
		{ .dialog = "CustomizeDlg", .control = "LocationLabel", .event = "SelectionPathOn", .attribute = "Visible" },
	};
};

class DiskCostDlg
{
public:
	MSIDialog dialog = { .dialog = "DiskCostDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "OK", .control_Default = "OK", .control_Cancel = "OK" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "DiskCostDlg", .control = "Text", .type = "Text", .x = 20, .y = 53, .width = 330, .height = 40, .attributes = 3, .text = "The highlighted volumes (if any) do not have enough disk space available for the currently selected features.  You can either remove some files from the highlighted volumes, or choose to install less features onto local drive(s), or select different destination drive(s)." },
		{ .dialog = "DiskCostDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "OK" },
		{ .dialog = "DiskCostDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "DiskCostDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "DiskCostDlg", .control = "Description", .type = "Text", .x = 20, .y = 20, .width = 280, .height = 20, .attributes = 196611, .text = "The disk space required for the installation of the selected features." },
		{ .dialog = "DiskCostDlg", .control = "OK", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_OK]", .control_Next = "BannerBitmap" },
		{ .dialog = "DiskCostDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Disk Space Requirements" },
		{ .dialog = "DiskCostDlg", .control = "VolumeList", .type = "VolumeCostList", .x = 20, .y = 100, .width = 330, .height = 120, .attributes = 393223, .text = "{120}{70}{70}{70}{70}" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "DiskCostDlg", .control = "OK", .event = "EndDialog", .argument = "Return", .condition = "1" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class ErrorDlg
{
public:
	MSIDialog dialog = { .dialog = "ErrorDlg", .hCentering = 50, .vCentering = 10, .width = 270, .height = 105, .attributes = 65539, .title = "Installer Information", .control_First = "ErrorText" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "ErrorDlg", .control = "Y", .type = "PushButton", .x = 100, .y = 80, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Yes]" },
		{ .dialog = "ErrorDlg", .control = "A", .type = "PushButton", .x = 100, .y = 80, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]" },
		{ .dialog = "ErrorDlg", .control = "C", .type = "PushButton", .x = 100, .y = 80, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]" },
		{ .dialog = "ErrorDlg", .control = "ErrorIcon", .type = "Icon", .x = 15, .y = 15, .width = 24, .height = 24, .attributes = 5242881, .text = "[InfoIcon]", .help = "Information icon|" },
		{ .dialog = "ErrorDlg", .control = "ErrorText", .type = "Text", .x = 48, .y = 15, .width = 205, .height = 60, .attributes = 3, .text = "Information text" },
		{ .dialog = "ErrorDlg", .control = "I", .type = "PushButton", .x = 100, .y = 80, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Ignore]" },
		{ .dialog = "ErrorDlg", .control = "N", .type = "PushButton", .x = 100, .y = 80, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_No]" },
		{ .dialog = "ErrorDlg", .control = "O", .type = "PushButton", .x = 100, .y = 80, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_OK]" },
		{ .dialog = "ErrorDlg", .control = "R", .type = "PushButton", .x = 100, .y = 80, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Retry]" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "ErrorDlg", .control = "Y", .event = "EndDialog", .argument = "ErrorYes", .condition = "1" },
		{ .dialog = "ErrorDlg", .control = "A", .event = "EndDialog", .argument = "ErrorAbort", .condition = "1" },
		{ .dialog = "ErrorDlg", .control = "C", .event = "EndDialog", .argument = "ErrorCancel", .condition = "1" },
		{ .dialog = "ErrorDlg", .control = "I", .event = "EndDialog", .argument = "ErrorIgnore", .condition = "1" },
		{ .dialog = "ErrorDlg", .control = "N", .event = "EndDialog", .argument = "ErrorNo", .condition = "1" },
		{ .dialog = "ErrorDlg", .control = "O", .event = "EndDialog", .argument = "ErrorOk", .condition = "1" },
		{ .dialog = "ErrorDlg", .control = "R", .event = "EndDialog", .argument = "ErrorRetry", .condition = "1" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class FilesInUse
{
public:
	MSIDialog dialog = { .dialog = "FilesInUse", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 19, .title = "[ProductName] [Setup]", .control_First = "Retry", .control_Default = "Retry", .control_Cancel = "Retry" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "FilesInUse", .control = "Text", .type = "Text", .x = 20, .y = 55, .width = 330, .height = 30, .attributes = 3, .text = "The following applications are using files that need to be updated by this setup. Close these applications and then click Retry to continue the installation or Cancel to exit it." },
		{ .dialog = "FilesInUse", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Retry" },
		{ .dialog = "FilesInUse", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "FilesInUse", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "FilesInUse", .control = "Description", .type = "Text", .x = 20, .y = 23, .width = 280, .height = 20, .attributes = 196611, .text = "Some files that need to be updated are currently in use." },
		{ .dialog = "FilesInUse", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Files in Use" },
		{ .dialog = "FilesInUse", .control = "Retry", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Retry]", .control_Next = "Ignore" },
		{ .dialog = "FilesInUse", .control = "Exit", .type = "PushButton", .x = 166, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Exit]", .control_Next = "BannerBitmap" },
		{ .dialog = "FilesInUse", .control = "Ignore", .type = "PushButton", .x = 235, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Ignore]", .control_Next = "Exit" },
		{ .dialog = "FilesInUse", .control = "List", .type = "ListBox", .x = 20, .y = 87, .width = 330, .height = 130, .attributes = 7, .property = "FileInUseProcess" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "FilesInUse", .control = "Retry", .event = "EndDialog", .argument = "Retry", .condition = "1" },
		{ .dialog = "FilesInUse", .control = "Exit", .event = "EndDialog", .argument = "Exit", .condition = "1" },
		{ .dialog = "FilesInUse", .control = "Ignore", .event = "EndDialog", .argument = "Ignore", .condition = "1" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class LicenseAgreementDlg
{
public:
	MSIDialog dialog = { .dialog = "LicenseAgreementDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] License Agreement", .control_First = "Buttons", .control_Default = "Next", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "LicenseAgreementDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "AgreementText" },
		{ .dialog = "LicenseAgreementDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "LicenseAgreementDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "LicenseAgreementDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{ .dialog = "LicenseAgreementDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "Please read the following license agreement carefully" },
		{ .dialog = "LicenseAgreementDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]End-User License Agreement" },
		{ .dialog = "LicenseAgreementDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{ .dialog = "LicenseAgreementDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{ .dialog = "LicenseAgreementDlg", .control = "AgreementText", .type = "ScrollableText", .x = 20, .y = 60, .width = 330, .height = 120, .attributes = 7, .text = "{\\rtf1\\ansi\\ansicpg1252\\deff0\\deftab720{\\fonttbl{\\f0\\froman\\fprq2 Times New Roman;}}{\\colortbl\\red0\\green0\\blue0;} \\deflang1033\\horzdoc{\\*\\fchars }{\\*\\lchars }\\pard\\plain\\f0\\fs20 <Your license agreement should go here.>\\par }", .control_Next = "Buttons" },
		{ .dialog = "LicenseAgreementDlg", .control = "Buttons", .type = "RadioButtonGroup", .x = 20, .y = 187, .width = 260, .height = 40, .attributes = 3, .property = "IAgree", .control_Next = "Print" },
		{ .dialog = "LicenseAgreementDlg", .control = "Print", .type = "PushButton", .x = 290, .y = 195, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Print]", .control_Next = "Back" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
		{ .dialog = "LicenseAgreementDlg", .control = "Next", .action = "Disable", .condition = "IAgree <> \"Yes\"" },
		{ .dialog = "LicenseAgreementDlg", .control = "Next", .action = "Enable", .condition = "IAgree = \"Yes\"" },
		{ .dialog = "LicenseAgreementDlg", .control = "Print", .action = "Hide", .condition = "VersionHandler < \"5.00\"" },
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "LicenseAgreementDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "LicenseAgreementDlg", .control = "Back", .event = "NewDialog", .argument = "WelcomeDlg", .condition = "VersionHandler < \"5.00\"" },
		{ .dialog = "LicenseAgreementDlg", .control = "Back", .event = "NewDialog", .argument = "WelcomeDlgLink", .condition = "VersionHandler >= \"5.00\"" },
		{ .dialog = "LicenseAgreementDlg", .control = "Next", .event = "NewDialog", .argument = "SetupTypeDlg", .condition = "IAgree = \"Yes\" AND ShowUserRegistrationDlg <> 1", .ordering = 3 },
		{ .dialog = "LicenseAgreementDlg", .control = "Next", .event = "NewDialog", .argument = "UserRegistrationDlg", .condition = "IAgree = \"Yes\" AND ShowUserRegistrationDlg = 1", .ordering = 1 },
		{ .dialog = "LicenseAgreementDlg", .control = "Next", .event = "SpawnWaitDialog", .argument = "WaitForCostingDlg", .condition = "CostingComplete = 1", .ordering = 2 },
		{ .dialog = "LicenseAgreementDlg", .control = "Print", .event = "MsiPrint", .argument = "0", .condition = "VersionHandler >= \"5.00\"" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class MaintenanceTypeDlg
{
public:
	MSIDialog dialog = { .dialog = "MaintenanceTypeDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "ChangeLabel", .control_Default = "ChangeButton", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "MaintenanceTypeDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "ChangeLabel" },
		{ .dialog = "MaintenanceTypeDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "MaintenanceTypeDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "MaintenanceTypeDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{ .dialog = "MaintenanceTypeDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 20, .attributes = 196611, .text = "Select the operation you wish to perform." },
		{ .dialog = "MaintenanceTypeDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 240, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Modify, Repair or Remove installation" },
		{ .dialog = "MaintenanceTypeDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{ .dialog = "MaintenanceTypeDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{ .dialog = "MaintenanceTypeDlg", .control = "ChangeLabel", .type = "Text", .x = 105, .y = 65, .width = 100, .height = 10, .attributes = 3, .text = "[DlgTitleFont]&Modify", .control_Next = "ChangeButton" },
		{ .dialog = "MaintenanceTypeDlg", .control = "ChangeButton", .type = "PushButton", .x = 50, .y = 65, .width = 38, .height = 38, .attributes = 5767171, .text = "[CustomSetupIcon]", .control_Next = "RepairLabel", .help = "Modify Installation|" },
		{ .dialog = "MaintenanceTypeDlg", .control = "RepairLabel", .type = "Text", .x = 105, .y = 114, .width = 100, .height = 10, .attributes = 3, .text = "[DlgTitleFont]Re&pair", .control_Next = "RepairButton" },
		{ .dialog = "MaintenanceTypeDlg", .control = "ChangeText", .type = "Text", .x = 105, .y = 78, .width = 230, .height = 20, .attributes = 3, .text = "Allows users to change the way features are installed." },
		{ .dialog = "MaintenanceTypeDlg", .control = "RemoveButton", .type = "PushButton", .x = 50, .y = 163, .width = 38, .height = 38, .attributes = 5767171, .text = "[RemoveIcon]", .control_Next = "Back", .help = "Remove Installation|" },
		{ .dialog = "MaintenanceTypeDlg", .control = "RemoveLabel", .type = "Text", .x = 105, .y = 163, .width = 100, .height = 10, .attributes = 3, .text = "[DlgTitleFont]&Remove", .control_Next = "RemoveButton" },
		{ .dialog = "MaintenanceTypeDlg", .control = "RemoveText", .type = "Text", .x = 105, .y = 176, .width = 230, .height = 20, .attributes = 3, .text = "Removes [ProductName] from your computer." },
		{ .dialog = "MaintenanceTypeDlg", .control = "RepairButton", .type = "PushButton", .x = 50, .y = 114, .width = 38, .height = 38, .attributes = 5767171, .text = "[RepairIcon]", .control_Next = "RemoveLabel", .help = "Repair Installation|" },
		{ .dialog = "MaintenanceTypeDlg", .control = "RepairText", .type = "Text", .x = 105, .y = 127, .width = 230, .height = 30, .attributes = 3, .text = "Repairs errors in the most recent installation state - fixes missing or corrupt files, shortcuts and registry entries." },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "MaintenanceTypeDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "MaintenanceTypeDlg", .control = "Back", .event = "NewDialog", .argument = "MaintenanceWelcomeDlg", .condition = "1" },
		{ .dialog = "MaintenanceTypeDlg", .control = "ChangeButton", .event = "NewDialog", .argument = "CustomizeDlg", .condition = "1", .ordering = 4 },
		{ .dialog = "MaintenanceTypeDlg", .control = "ChangeButton", .event = "[InstallMode]", .argument = "Change", .condition = "1", .ordering = 1 },
		{ .dialog = "MaintenanceTypeDlg", .control = "ChangeButton", .event = "[Progress1]", .argument = "Changing", .condition = "1", .ordering = 2 },
		{ .dialog = "MaintenanceTypeDlg", .control = "ChangeButton", .event = "[Progress2]", .argument = "changes", .condition = "1", .ordering = 3 },
		{ .dialog = "MaintenanceTypeDlg", .control = "RemoveButton", .event = "NewDialog", .argument = "VerifyRemoveDlg", .condition = "1", .ordering = 4 },
		{ .dialog = "MaintenanceTypeDlg", .control = "RemoveButton", .event = "[InstallMode]", .argument = "Remove", .condition = "1", .ordering = 1 },
		{ .dialog = "MaintenanceTypeDlg", .control = "RemoveButton", .event = "[Progress1]", .argument = "Removing", .condition = "1", .ordering = 2 },
		{ .dialog = "MaintenanceTypeDlg", .control = "RemoveButton", .event = "[Progress2]", .argument = "removes", .condition = "1", .ordering = 3 },
		{ .dialog = "MaintenanceTypeDlg", .control = "RepairButton", .event = "NewDialog", .argument = "VerifyRepairDlg", .condition = "1", .ordering = 4 },
		{ .dialog = "MaintenanceTypeDlg", .control = "RepairButton", .event = "[InstallMode]", .argument = "Repair", .condition = "1", .ordering = 1 },
		{ .dialog = "MaintenanceTypeDlg", .control = "RepairButton", .event = "[Progress1]", .argument = "Repairing", .condition = "1", .ordering = 2 },
		{ .dialog = "MaintenanceTypeDlg", .control = "RepairButton", .event = "[Progress2]", .argument = "repairs", .condition = "1", .ordering = 3 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class MaintenanceWelcomeDlg
{
public:
	MSIDialog dialog = { .dialog = "MaintenanceWelcomeDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "Next", .control_Default = "Next", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "MaintenanceWelcomeDlg", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "Back" },
		{ .dialog = "MaintenanceWelcomeDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "MaintenanceWelcomeDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{ .dialog = "MaintenanceWelcomeDlg", .control = "Description", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 60, .attributes = 196611, .text = "The [Wizard] will allow you to change the way [ProductName] features are installed on your computer or even to remove [ProductName] from your computer.  Click Next to continue or Cancel to exit the [Wizard]." },
		{ .dialog = "MaintenanceWelcomeDlg", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}Welcome to the [ProductName] [Wizard]" },
		{ .dialog = "MaintenanceWelcomeDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{ .dialog = "MaintenanceWelcomeDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "MaintenanceWelcomeDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "MaintenanceWelcomeDlg", .control = "Next", .event = "NewDialog", .argument = "MaintenanceTypeDlg", .condition = "1", .ordering = 2 },
		{ .dialog = "MaintenanceWelcomeDlg", .control = "Next", .event = "SpawnWaitDialog", .argument = "WaitForCostingDlg", .condition = "CostingComplete = 1", .ordering = 1 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class MsiRMFilesInUse
{
public:
	MSIDialog dialog = { .dialog = "MsiRMFilesInUse", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 19, .title = "[ProductName] [Setup]", .control_First = "OK", .control_Default = "OK", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "MsiRMFilesInUse", .control = "Text", .type = "Text", .x = 20, .y = 55, .width = 330, .height = 30, .attributes = 3, .text = "The following applications are using files that need to be updated by this setup." },
		{ .dialog = "MsiRMFilesInUse", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Options" },
		{ .dialog = "MsiRMFilesInUse", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "MsiRMFilesInUse", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "MsiRMFilesInUse", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{ .dialog = "MsiRMFilesInUse", .control = "Description", .type = "Text", .x = 20, .y = 23, .width = 280, .height = 20, .attributes = 196611, .text = "Some files that need to be updated are currently in use." },
		{ .dialog = "MsiRMFilesInUse", .control = "OK", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_OK]", .control_Next = "Cancel" },
		{ .dialog = "MsiRMFilesInUse", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 25, .attributes = 196611, .text = "[DlgTitleFont]Files in Use" },
		{ .dialog = "MsiRMFilesInUse", .control = "List", .type = "ListBox", .x = 20, .y = 85, .width = 330, .height = 89, .attributes = 7, .property = "FileInUseProcess" },
		{ .dialog = "MsiRMFilesInUse", .control = "Options", .type = "RadioButtonGroup", .x = 20, .y = 172, .width = 330, .height = 54, .attributes = 3, .property = "AppsShutdownOption", .control_Next = "OK" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "MsiRMFilesInUse", .control = "Cancel", .event = "EndDialog", .argument = "Exit", .condition = "1" },
		{ .dialog = "MsiRMFilesInUse", .control = "OK", .event = "EndDialog", .argument = "Return", .condition = "1", .ordering = 2 },
		{ .dialog = "MsiRMFilesInUse", .control = "OK", .event = "RMShutdownAndRestart", .argument = "0", .condition = "AppsShutdownOption = \"All\"", .ordering = 1 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class OutOfDiskDlg
{
public:
	MSIDialog dialog = { .dialog = "OutOfDiskDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "OK", .control_Default = "OK", .control_Cancel = "OK" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "OutOfDiskDlg", .control = "Text", .type = "Text", .x = 20, .y = 53, .width = 330, .height = 40, .attributes = 3, .text = "The highlighted volumes do not have enough disk space available for the currently selected features.  You can either remove some files from the highlighted volumes, or choose to install less features onto local drive(s), or select different destination drive(s)." },
		{ .dialog = "OutOfDiskDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "OK" },
		{ .dialog = "OutOfDiskDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "OutOfDiskDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "OutOfDiskDlg", .control = "Description", .type = "Text", .x = 20, .y = 20, .width = 280, .height = 20, .attributes = 196611, .text = "Disk space required for the installation exceeds available disk space." },
		{ .dialog = "OutOfDiskDlg", .control = "OK", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_OK]", .control_Next = "BannerBitmap" },
		{ .dialog = "OutOfDiskDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Out of Disk Space" },
		{ .dialog = "OutOfDiskDlg", .control = "VolumeList", .type = "VolumeCostList", .x = 20, .y = 100, .width = 330, .height = 120, .attributes = 393223, .text = "{120}{70}{70}{70}{70}" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "OutOfDiskDlg", .control = "OK", .event = "EndDialog", .argument = "Return", .condition = "1" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class OutOfRbDiskDlg
{
public:
	MSIDialog dialog = { .dialog = "OutOfRbDiskDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "No", .control_Default = "No", .control_Cancel = "No" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "OutOfRbDiskDlg", .control = "Text", .type = "Text", .x = 20, .y = 53, .width = 330, .height = 40, .attributes = 3, .text = "The highlighted volumes do not have enough disk space available for the currently selected features.  You can either remove some files from the highlighted volumes, or choose to install less features onto local drive(s), or select different destination drive(s)." },
		{ .dialog = "OutOfRbDiskDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "No" },
		{ .dialog = "OutOfRbDiskDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "OutOfRbDiskDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "OutOfRbDiskDlg", .control = "Description", .type = "Text", .x = 20, .y = 20, .width = 280, .height = 20, .attributes = 196611, .text = "Disk space required for the installation exceeds available disk space." },
		{ .dialog = "OutOfRbDiskDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Out of Disk Space" },
		{ .dialog = "OutOfRbDiskDlg", .control = "No", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_No]", .control_Next = "Yes" },
		{ .dialog = "OutOfRbDiskDlg", .control = "Yes", .type = "PushButton", .x = 240, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Yes]", .control_Next = "BannerBitmap" },
		{ .dialog = "OutOfRbDiskDlg", .control = "VolumeList", .type = "VolumeCostList", .x = 20, .y = 140, .width = 330, .height = 80, .attributes = 4587527, .text = "{120}{70}{70}{70}{70}" },
		{ .dialog = "OutOfRbDiskDlg", .control = "Text2", .type = "Text", .x = 20, .y = 94, .width = 330, .height = 40, .attributes = 3, .text = "Alternatively, you may choose to disable the installer's rollback functionality.  This allows the installer to restore your computer's original state should the installation be interrupted in any way.  Click Yes if you wish to take the risk to disable rollback." },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "OutOfRbDiskDlg", .control = "No", .event = "EndDialog", .argument = "Return", .condition = "1" },
		{ .dialog = "OutOfRbDiskDlg", .control = "Yes", .event = "EndDialog", .argument = "Return", .condition = "1", .ordering = 2 },
		{ .dialog = "OutOfRbDiskDlg", .control = "Yes", .event = "EnableRollback", .argument = "False", .condition = "1", .ordering = 1 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class ResumeDlg
{
public:
	MSIDialog dialog = { .dialog = "ResumeDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "Install", .control_Default = "Install", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "ResumeDlg", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "Back" },
		{ .dialog = "ResumeDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "ResumeDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{ .dialog = "ResumeDlg", .control = "Description", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 30, .attributes = 196611, .text = "The [Wizard] will complete the installation of [ProductName] on your computer.  Click Install to continue or Cancel to exit the [Wizard]." },
		{ .dialog = "ResumeDlg", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}Resuming the [ProductName] [Wizard]" },
		{ .dialog = "ResumeDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Install" },
		{ .dialog = "ResumeDlg", .control = "Install", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Install]", .control_Next = "Cancel" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "ResumeDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "ResumeDlg", .control = "Install", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 4 },
		{ .dialog = "ResumeDlg", .control = "Install", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace <> 1", .ordering = 2 },
		{ .dialog = "ResumeDlg", .control = "Install", .event = "SpawnDialog", .argument = "OutOfDiskDlg", .condition = "(OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 1) OR (OutOfDiskSpace = 1 AND PROMPTROLLBACKCOST=\"F\")", .ordering = 6 },
		{ .dialog = "ResumeDlg", .control = "Install", .event = "SpawnDialog", .argument = "OutOfRbDiskDlg", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND (PROMPTROLLBACKCOST=\"P\" OR NOT PROMPTROLLBACKCOST)", .ordering = 3 },
		{ .dialog = "ResumeDlg", .control = "Install", .event = "SpawnWaitDialog", .argument = "WaitForCostingDlg", .condition = "CostingComplete = 1", .ordering = 1 },
		{ .dialog = "ResumeDlg", .control = "Install", .event = "EnableRollback", .argument = "False", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 5 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class SetupTypeDlg
{
public:
	MSIDialog dialog = { .dialog = "SetupTypeDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "TypicalLabel", .control_Default = "TypicalButton", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "SetupTypeDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "TypicalLabel" },
		{ .dialog = "SetupTypeDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "SetupTypeDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "SetupTypeDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{ .dialog = "SetupTypeDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "Choose the setup type that best suits your needs" },
		{ .dialog = "SetupTypeDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Choose Setup Type" },
		{ .dialog = "SetupTypeDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{ .dialog = "SetupTypeDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{ .dialog = "SetupTypeDlg", .control = "TypicalLabel", .type = "Text", .x = 105, .y = 65, .width = 100, .height = 10, .attributes = 3, .text = "[DlgTitleFont]&Typical", .control_Next = "TypicalButton" },
		{ .dialog = "SetupTypeDlg", .control = "CompleteButton", .type = "PushButton", .x = 50, .y = 171, .width = 38, .height = 38, .attributes = 5767171, .text = "[CompleteSetupIcon]", .control_Next = "Back", .help = "Complete Installation|" },
		{ .dialog = "SetupTypeDlg", .control = "CompleteLabel", .type = "Text", .x = 105, .y = 171, .width = 100, .height = 10, .attributes = 3, .text = "[DlgTitleFont]C&omplete", .control_Next = "CompleteButton" },
		{ .dialog = "SetupTypeDlg", .control = "CompleteText", .type = "Text", .x = 105, .y = 184, .width = 230, .height = 20, .attributes = 3, .text = "All program features will be installed.  (Requires most disk space)" },
		{ .dialog = "SetupTypeDlg", .control = "CustomButton", .type = "PushButton", .x = 50, .y = 118, .width = 38, .height = 38, .attributes = 5767171, .text = "[CustomSetupIcon]", .control_Next = "CompleteLabel", .help = "Custom Installation|" },
		{ .dialog = "SetupTypeDlg", .control = "CustomLabel", .type = "Text", .x = 105, .y = 118, .width = 100, .height = 10, .attributes = 3, .text = "[DlgTitleFont]C&ustom", .control_Next = "CustomButton" },
		{ .dialog = "SetupTypeDlg", .control = "CustomText", .type = "Text", .x = 105, .y = 131, .width = 230, .height = 30, .attributes = 3, .text = "Allows users to choose which program features will be installed and where they will be installed. Recommended for advanced users." },
		{ .dialog = "SetupTypeDlg", .control = "TypicalButton", .type = "PushButton", .x = 50, .y = 65, .width = 38, .height = 38, .attributes = 5767171, .text = "[InstallerIcon]", .control_Next = "CustomLabel", .help = "Typical Installation|" },
		{ .dialog = "SetupTypeDlg", .control = "TypicalText", .type = "Text", .x = 105, .y = 78, .width = 230, .height = 20, .attributes = 3, .text = "Installs the most common program features. Recommended for most users." },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "SetupTypeDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "SetupTypeDlg", .control = "Back", .event = "NewDialog", .argument = "LicenseAgreementDlg", .condition = "ShowUserRegistrationDlg <> 1" },
		{ .dialog = "SetupTypeDlg", .control = "Back", .event = "NewDialog", .argument = "UserRegistrationDlg", .condition = "ShowUserRegistrationDlg = 1" },
		{ .dialog = "SetupTypeDlg", .control = "CompleteButton", .event = "NewDialog", .argument = "VerifyReadyDlg", .condition = "1", .ordering = 3 },
		{ .dialog = "SetupTypeDlg", .control = "CompleteButton", .event = "[InstallMode]", .argument = "Complete", .condition = "1", .ordering = 1 },
		{ .dialog = "SetupTypeDlg", .control = "CompleteButton", .event = "SetInstallLevel", .argument = "1000", .condition = "1", .ordering = 2 },
		{ .dialog = "SetupTypeDlg", .control = "CustomButton", .event = "NewDialog", .argument = "CustomizeDlg", .condition = "1", .ordering = 2 },
		{ .dialog = "SetupTypeDlg", .control = "CustomButton", .event = "[InstallMode]", .argument = "Custom", .condition = "1", .ordering = 1 },
		{ .dialog = "SetupTypeDlg", .control = "TypicalButton", .event = "NewDialog", .argument = "VerifyReadyDlg", .condition = "1", .ordering = 3 },
		{ .dialog = "SetupTypeDlg", .control = "TypicalButton", .event = "[InstallMode]", .argument = "Typical", .condition = "1", .ordering = 1 },
		{ .dialog = "SetupTypeDlg", .control = "TypicalButton", .event = "SetInstallLevel", .argument = "3", .condition = "1", .ordering = 2 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class UserRegistrationDlg
{
public:
	MSIDialog dialog = { .dialog = "UserRegistrationDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "NameLabel", .control_Default = "Next", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "UserRegistrationDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "NameLabel" },
		{ .dialog = "UserRegistrationDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "UserRegistrationDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "UserRegistrationDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{ .dialog = "UserRegistrationDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "Please enter your customer information" },
		{ .dialog = "UserRegistrationDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Customer Information" },
		{ .dialog = "UserRegistrationDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{ .dialog = "UserRegistrationDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{ .dialog = "UserRegistrationDlg", .control = "OrganizationLabel", .type = "Text", .x = 45, .y = 110, .width = 100, .height = 15, .attributes = 3, .text = "&Organization:", .control_Next = "OrganizationEdit" },
		{ .dialog = "UserRegistrationDlg", .control = "CDKeyEdit", .type = "MaskedEdit", .x = 45, .y = 159, .width = 250, .height = 16, .attributes = 3, .property = "PIDKEY", .text = "[PIDTemplate]", .control_Next = "Back" },
		{ .dialog = "UserRegistrationDlg", .control = "CDKeyLabel", .type = "Text", .x = 45, .y = 147, .width = 50, .height = 10, .attributes = 3, .text = "CD &Key:", .control_Next = "CDKeyEdit" },
		{ .dialog = "UserRegistrationDlg", .control = "OrganizationEdit", .type = "Edit", .x = 45, .y = 122, .width = 220, .height = 18, .attributes = 3, .property = "COMPANYNAME", .text = "{80}", .control_Next = "CDKeyLabel" },
		{ .dialog = "UserRegistrationDlg", .control = "NameLabel", .type = "Text", .x = 45, .y = 73, .width = 100, .height = 15, .attributes = 3, .text = "&User Name:", .control_Next = "NameEdit" },
		{ .dialog = "UserRegistrationDlg", .control = "NameEdit", .type = "Edit", .x = 45, .y = 85, .width = 220, .height = 18, .attributes = 3, .property = "USERNAME", .text = "{80}", .control_Next = "OrganizationLabel" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "UserRegistrationDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "UserRegistrationDlg", .control = "Back", .event = "NewDialog", .argument = "LicenseAgreementDlg", .condition = "1" },
		{ .dialog = "UserRegistrationDlg", .control = "Next", .event = "NewDialog", .argument = "SetupTypeDlg", .condition = "ProductID", .ordering = 3 },
		{ .dialog = "UserRegistrationDlg", .control = "Next", .event = "ValidateProductID", .argument = "0", .condition = "0", .ordering = 1 },
		{ .dialog = "UserRegistrationDlg", .control = "Next", .event = "SpawnWaitDialog", .argument = "WaitForCostingDlg", .condition = "CostingComplete = 1", .ordering = 2 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class VerifyReadyDlg
{
public:
	MSIDialog dialog = { .dialog = "VerifyReadyDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 35, .title = "[ProductName] [Setup]", .control_First = "Install", .control_Default = "Install", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "VerifyReadyDlg", .control = "Text", .type = "Text", .x = 25, .y = 70, .width = 320, .height = 20, .attributes = 3, .text = "Click Install to begin the installation.  If you want to review or change any of your installation settings, click Back.  Click Cancel to exit the wizard." },
		{ .dialog = "VerifyReadyDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Back" },
		{ .dialog = "VerifyReadyDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "VerifyReadyDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "VerifyReadyDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{ .dialog = "VerifyReadyDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "The [Wizard] is ready to begin the [InstallMode] installation" },
		{ .dialog = "VerifyReadyDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Ready to Install" },
		{ .dialog = "VerifyReadyDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Install" },
		{ .dialog = "VerifyReadyDlg", .control = "Install", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 8388611, .text = "[ButtonText_Install]", .control_Next = "Cancel" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "VerifyReadyDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "VerifyReadyDlg", .control = "Back", .event = "NewDialog", .argument = "AdminInstallPointDlg", .condition = "InstallMode = \"Server Image\"" },
		{ .dialog = "VerifyReadyDlg", .control = "Back", .event = "NewDialog", .argument = "CustomizeDlg", .condition = "InstallMode = \"Custom\" OR InstallMode = \"Change\"" },
		{ .dialog = "VerifyReadyDlg", .control = "Back", .event = "NewDialog", .argument = "MaintenanceTypeDlg", .condition = "InstallMode = \"Repair\"" },
		{ .dialog = "VerifyReadyDlg", .control = "Back", .event = "NewDialog", .argument = "SetupTypeDlg", .condition = "InstallMode = \"Typical\" OR InstallMode = \"Complete\"" },
		{ .dialog = "VerifyReadyDlg", .control = "Install", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 3 },
		{ .dialog = "VerifyReadyDlg", .control = "Install", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace <> 1", .ordering = 1 },
		{ .dialog = "VerifyReadyDlg", .control = "Install", .event = "SpawnDialog", .argument = "OutOfDiskDlg", .condition = "(OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 1) OR (OutOfDiskSpace = 1 AND PROMPTROLLBACKCOST=\"F\")", .ordering = 5 },
		{ .dialog = "VerifyReadyDlg", .control = "Install", .event = "SpawnDialog", .argument = "OutOfRbDiskDlg", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND (PROMPTROLLBACKCOST=\"P\" OR NOT PROMPTROLLBACKCOST)", .ordering = 2 },
		{ .dialog = "VerifyReadyDlg", .control = "Install", .event = "EnableRollback", .argument = "False", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 4 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class VerifyRemoveDlg
{
public:
	MSIDialog dialog = { .dialog = "VerifyRemoveDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 35, .title = "[ProductName] [Setup]", .control_First = "Back", .control_Default = "Back", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "VerifyRemoveDlg", .control = "Text", .type = "Text", .x = 25, .y = 70, .width = 320, .height = 30, .attributes = 3, .text = "Click Remove to remove [ProductName] from your computer.  If you want to review or change any of your installation settings, click Back.  Click Cancel to exit the wizard." },
		{ .dialog = "VerifyRemoveDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Back" },
		{ .dialog = "VerifyRemoveDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "VerifyRemoveDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "VerifyRemoveDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{ .dialog = "VerifyRemoveDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "You have chosen to remove the program from your computer." },
		{ .dialog = "VerifyRemoveDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Remove [ProductName]" },
		{ .dialog = "VerifyRemoveDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Remove" },
		{ .dialog = "VerifyRemoveDlg", .control = "Remove", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 8388611, .text = "[ButtonText_Remove]", .control_Next = "Cancel" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "VerifyRemoveDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "VerifyRemoveDlg", .control = "Back", .event = "NewDialog", .argument = "MaintenanceTypeDlg", .condition = "1" },
		{ .dialog = "VerifyRemoveDlg", .control = "Remove", .event = "Remove", .argument = "All", .condition = "OutOfDiskSpace <> 1", .ordering = 1 },
		{ .dialog = "VerifyRemoveDlg", .control = "Remove", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 4 },
		{ .dialog = "VerifyRemoveDlg", .control = "Remove", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace <> 1", .ordering = 2 },
		{ .dialog = "VerifyRemoveDlg", .control = "Remove", .event = "SpawnDialog", .argument = "OutOfDiskDlg", .condition = "(OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 1) OR (OutOfDiskSpace = 1 AND PROMPTROLLBACKCOST=\"F\")", .ordering = 6 },
		{ .dialog = "VerifyRemoveDlg", .control = "Remove", .event = "SpawnDialog", .argument = "OutOfRbDiskDlg", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND (PROMPTROLLBACKCOST=\"P\" OR NOT PROMPTROLLBACKCOST)", .ordering = 3 },
		{ .dialog = "VerifyRemoveDlg", .control = "Remove", .event = "EnableRollback", .argument = "False", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 5 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class VerifyRepairDlg
{
public:
	MSIDialog dialog = { .dialog = "VerifyRepairDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 35, .title = "[ProductName] [Setup]", .control_First = "Repair", .control_Default = "Repair", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "VerifyRepairDlg", .control = "Text", .type = "Text", .x = 25, .y = 70, .width = 320, .height = 30, .attributes = 3, .text = "Click Repair to repair the installation of [ProductName].  If you want to review or change any of your installation settings, click Back.  Click Cancel to exit the wizard." },
		{ .dialog = "VerifyRepairDlg", .control = "BannerBitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 44, .attributes = 1, .text = "[BannerBitmap]", .control_Next = "Back" },
		{ .dialog = "VerifyRepairDlg", .control = "BannerLine", .type = "Line", .x = 0, .y = 44, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "VerifyRepairDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "VerifyRepairDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "BannerBitmap" },
		{ .dialog = "VerifyRepairDlg", .control = "Description", .type = "Text", .x = 25, .y = 23, .width = 280, .height = 15, .attributes = 196611, .text = "The [Wizard] is ready to begin the repair of [ProductName]." },
		{ .dialog = "VerifyRepairDlg", .control = "Title", .type = "Text", .x = 15, .y = 6, .width = 200, .height = 15, .attributes = 196611, .text = "[DlgTitleFont]Repair [ProductName]" },
		{ .dialog = "VerifyRepairDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Back]", .control_Next = "Repair" },
		{ .dialog = "VerifyRepairDlg", .control = "Repair", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 8388611, .text = "[ButtonText_Repair]", .control_Next = "Cancel" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "VerifyRepairDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "VerifyRepairDlg", .control = "Back", .event = "NewDialog", .argument = "MaintenanceTypeDlg", .condition = "1" },
		{ .dialog = "VerifyRepairDlg", .control = "Repair", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 5 },
		{ .dialog = "VerifyRepairDlg", .control = "Repair", .event = "EndDialog", .argument = "Return", .condition = "OutOfDiskSpace <> 1", .ordering = 3 },
		{ .dialog = "VerifyRepairDlg", .control = "Repair", .event = "SpawnDialog", .argument = "OutOfDiskDlg", .condition = "(OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 1) OR (OutOfDiskSpace = 1 AND PROMPTROLLBACKCOST=\"F\")", .ordering = 7 },
		{ .dialog = "VerifyRepairDlg", .control = "Repair", .event = "SpawnDialog", .argument = "OutOfRbDiskDlg", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND (PROMPTROLLBACKCOST=\"P\" OR NOT PROMPTROLLBACKCOST)", .ordering = 4 },
		{ .dialog = "VerifyRepairDlg", .control = "Repair", .event = "EnableRollback", .argument = "False", .condition = "OutOfDiskSpace = 1 AND OutOfNoRbDiskSpace = 0 AND PROMPTROLLBACKCOST=\"D\"", .ordering = 6 },
		{ .dialog = "VerifyRepairDlg", .control = "Repair", .event = "Reinstall", .argument = "All", .condition = "OutOfDiskSpace <> 1", .ordering = 2 },
		{ .dialog = "VerifyRepairDlg", .control = "Repair", .event = "ReinstallMode", .argument = "ecmus", .condition = "OutOfDiskSpace <> 1", .ordering = 1 },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class WaitForCostingDlg
{
public:
	MSIDialog dialog = { .dialog = "WaitForCostingDlg", .hCentering = 50, .vCentering = 10, .width = 260, .height = 85, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "Return", .control_Default = "Return", .control_Cancel = "Return" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "WaitForCostingDlg", .control = "Text", .type = "Text", .x = 48, .y = 15, .width = 194, .height = 30, .attributes = 3, .text = "Please wait while the installer finishes determining your disk space requirements." },
		{ .dialog = "WaitForCostingDlg", .control = "Icon", .type = "Icon", .x = 15, .y = 15, .width = 24, .height = 24, .attributes = 5242881, .text = "[ExclamationIcon]", .help = "Exclamation icon|" },
		{ .dialog = "WaitForCostingDlg", .control = "Return", .type = "PushButton", .x = 102, .y = 57, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Return]" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "WaitForCostingDlg", .control = "Return", .event = "EndDialog", .argument = "Exit", .condition = "1" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class WelcomeDlg
{
public:
	MSIDialog dialog = { .dialog = "WelcomeDlg", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "Next", .control_Default = "Next", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "WelcomeDlg", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "Back" },
		{ .dialog = "WelcomeDlg", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "WelcomeDlg", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{ .dialog = "WelcomeDlg", .control = "Description", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 30, .attributes = 196611, .text = "The [Wizard] will install [ProductName] on your computer.  Click Next to continue or Cancel to exit the [Wizard]." },
		{ .dialog = "WelcomeDlg", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}Welcome to the [ProductName] [Wizard]" },
		{ .dialog = "WelcomeDlg", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{ .dialog = "WelcomeDlg", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "WelcomeDlg", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "WelcomeDlg", .control = "Next", .event = "NewDialog", .argument = "LicenseAgreementDlg", .condition = "1" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class WelcomeDlgLink
{
public:
	MSIDialog dialog = { .dialog = "WelcomeDlgLink", .hCentering = 50, .vCentering = 50, .width = 370, .height = 270, .attributes = 3, .title = "[ProductName] [Setup]", .control_First = "Next", .control_Default = "Next", .control_Cancel = "Cancel" };

	std::vector<MSIControl> controls =
	{
		{ .dialog = "WelcomeDlgLink", .control = "Bitmap", .type = "Bitmap", .x = 0, .y = 0, .width = 370, .height = 234, .attributes = 1, .text = "[DialogBitmap]", .control_Next = "WebLink" },
		{ .dialog = "WelcomeDlgLink", .control = "BottomLine", .type = "Line", .x = 0, .y = 234, .width = 370, .height = 0, .attributes = 1 },
		{ .dialog = "WelcomeDlgLink", .control = "Cancel", .type = "PushButton", .x = 304, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Cancel]", .control_Next = "Bitmap" },
		{ .dialog = "WelcomeDlgLink", .control = "Description", .type = "Text", .x = 135, .y = 70, .width = 220, .height = 30, .attributes = 196611, .text = "The [Wizard] will install [ProductName] on your computer.  Click Next to continue or Cancel to exit the [Wizard]." },
		{ .dialog = "WelcomeDlgLink", .control = "Title", .type = "Text", .x = 135, .y = 20, .width = 220, .height = 60, .attributes = 196611, .text = "{\\VerdanaBold13}Welcome to the [ProductName] [Wizard]" },
		{ .dialog = "WelcomeDlgLink", .control = "Back", .type = "PushButton", .x = 180, .y = 243, .width = 56, .height = 17, .attributes = 1, .text = "[ButtonText_Back]", .control_Next = "Next" },
		{ .dialog = "WelcomeDlgLink", .control = "Next", .type = "PushButton", .x = 236, .y = 243, .width = 56, .height = 17, .attributes = 3, .text = "[ButtonText_Next]", .control_Next = "Cancel" },
		{ .dialog = "WelcomeDlgLink", .control = "WebLink", .type = "Hyperlink", .x = 135, .y = 120, .width = 220, .height = 30, .attributes = 196611, .text = "<A HREF=\"[LinkURL]\">[Link]</A>", .control_Next = "Back" },
	};

	std::vector<MSIControlCondition> controlCondition =
	{
	};

	std::vector<MSIControlEvent> controlEvent =
	{
		{ .dialog = "WelcomeDlgLink", .control = "Cancel", .event = "SpawnDialog", .argument = "CancelDlg", .condition = "1" },
		{ .dialog = "WelcomeDlgLink", .control = "Next", .event = "NewDialog", .argument = "LicenseAgreementDlg", .condition = "1" },
	};

	std::vector<MSIEventMapping> eventMapping =
	{
	};
};

class MSIDialogs
{
public:
	AdminWelcomeDlg adminWelcomeDlg;
	ExitDialog exitDialog;
	FatalError fatalError;
	PrepareDlg prepareDlg;
	ProgressDlg progressDlg;
	UserExit userExit;
	AdminBrowseDlg adminBrowseDlg;
	AdminInstallPointDlg adminInstallPointDlg;
	AdminRegistrationDlg adminRegistrationDlg;
	BrowseDlg browseDlg;
	CancelDlg cancelDlg;
	CustomizeDlg customizeDlg;
	DiskCostDlg diskCostDlg;
	ErrorDlg errorDlg;
	FilesInUse filesInUse;
	LicenseAgreementDlg licenseAgreementDlg;
	MaintenanceTypeDlg maintenanceTypeDlg;
	MaintenanceWelcomeDlg maintenanceWelcomeDlg;
	MsiRMFilesInUse msiRMFilesInUse;
	OutOfDiskDlg outOfDiskDlg;
	OutOfRbDiskDlg outOfRbDiskDlg;
	ResumeDlg resumeDlg;
	SetupTypeDlg setupTypeDlg;
	UserRegistrationDlg userRegistrationDlg;
	VerifyReadyDlg verifyReadyDlg;
	VerifyRemoveDlg verifyRemoveDlg;
	VerifyRepairDlg verifyRepairDlg;
	WaitForCostingDlg waitForCostingDlg;
	WelcomeDlg welcomeDlg;
	WelcomeDlgLink welcomeDlgLink;

	std::vector<MSIBinary> binary =
	{
		{ .name = "bannrbmp" },
		{ .name = "completi" },
		{ .name = "custicon" },
		{ .name = "dlgbmp" },
		{ .name = "exclamic" },
		{ .name = "info" },
		{ .name = "insticon" },
		{ .name = "New" },
		{ .name = "removico" },
		{ .name = "repairic" },
		{ .name = "Up" },
	};

	std::vector<MSIBBControl> bbControl;
	std::vector<MSIBillboard> billboard;
	std::vector<MSICheckBox> checkBox;
	std::vector<MSIComboBox> comboBox;
	std::vector<MSIListBox> listBox;
	std::vector<MSIListView> listView;
	std::vector<MSIRadioButton> radioButton =
	{
		{ .property = "IAgree", .order = 1, .value = "Yes", .x = 5, .y = 0, .width = 250, .height = 15, .text = "{\\DlgFont8}I &accept the terms in the License Agreement" },
		{ .property = "IAgree", .order = 2, .value = "No", .x = 5, .y = 20, .width = 250, .height = 15, .text = "{\\DlgFont8}I &do not accept the terms in the License Agreement" },
		{ .property = "AppsShutdownOption", .order = 1, .value = "All", .x = 5, .y = 5, .width = 320, .height = 15, .text = "Automatically &close applications and attempt to restart them after setup is complete." },
		{ .property = "AppsShutdownOption", .order = 2, .value = "None", .x = 5, .y = 25, .width = 320, .height = 15, .text = "Do &not close applications. (A Reboot will be required.)" },
	};

	std::vector<MSIProperty> property = 
	{
		{ .property = "BannerBitmap", .value = "bannrbmp" },
		{ .property = "IAgree", .value = "No" },
		{ .property = "AppsShutdownOption", .value = "All" },
		{ .property = "ProductID", .value = "none" },
		{ .property = "ARPHELPLINK", .value = "https://example.com" },
		{ .property = "ButtonText_Back", .value = "< &Back" },
		{ .property = "ButtonText_Browse", .value = "Br&owse" },
		{ .property = "ButtonText_Cancel", .value = "Cancel" },
		{ .property = "ButtonText_Exit", .value = "&Exit" },
		{ .property = "ButtonText_Finish", .value = "&Finish" },
		{ .property = "ButtonText_Ignore", .value = "&Ignore" },
		{ .property = "ButtonText_Install", .value = "&Install" },
		{ .property = "ButtonText_Next", .value = "&Next >" },
		{ .property = "ButtonText_No", .value = "&No" },
		{ .property = "ButtonText_OK", .value = "OK" },
		{ .property = "ButtonText_Print", .value = "&Print" },
		{ .property = "ButtonText_Remove", .value = "&Remove" },
		{ .property = "ButtonText_Repair", .value = "&Repair" },
		{ .property = "ButtonText_Reset", .value = "&Reset" },
		{ .property = "ButtonText_Resume", .value = "&Resume" },
		{ .property = "ButtonText_Retry", .value = "&Retry" },
		{ .property = "ButtonText_Return", .value = "&Return" },
		{ .property = "ButtonText_Yes", .value = "&Yes" },
		{ .property = "CompleteSetupIcon", .value = "completi" },
		{ .property = "ComponentDownload", .value = "https://example.com/components/" },
		{ .property = "CustomSetupIcon", .value = "custicon" },
		{ .property = "DefaultUIFont", .value = "DlgFont8" },
		{ .property = "DialogBitmap", .value = "dlgbmp" },
		{ .property = "DlgTitleFont", .value = "{&DlgFontBold8}" },
		{ .property = "ErrorDialog", .value = "ErrorDlg" },
		{ .property = "ExclamationIcon", .value = "exclamic" },
		{ .property = "InfoIcon", .value = "info" },
		{ .property = "InstallerIcon", .value = "insticon" },
		{ .property = "INSTALLLEVEL", .value = "3" },
		{ .property = "InstallMode", .value = "Typical" },
		{ .property = "Link", .value = "Visit us on example.com!" },
		{ .property = "LinkURL", .value = "https://example.com" },
		{ .property = "PIDTemplate", .value = "12345<###-%%%%%%%>@@@@@" },
		{ .property = "Progress1", .value = "Installing" },
		{ .property = "Progress2", .value = "installs" },
		{ .property = "PROMPTROLLBACKCOST", .value = "P" },
		{ .property = "RemoveIcon", .value = "removico" },
		{ .property = "RepairIcon", .value = "repairic" },
		{ .property = "Setup", .value = "Setup" },
		{ .property = "ShowUserRegistrationDlg", .value = "1" },
		{ .property = "Wizard", .value = "Setup Wizard" },
	};

	std::vector<MSITextStyle> textStyle = 
	{
		{ .textStyle = "DlgFont8", .faceName = "Tahoma", .size = 8, .styleBits = 0 },
		{ .textStyle = "DlgFontBold8", .faceName = "Tahoma", .size = 8, .styleBits = 1 },
		{ .textStyle = "VerdanaBold13", .faceName = "Verdana", .size = 13, .styleBits = 1 },
	};

	std::vector<MSIUIText> uiText = 
	{
		{ .key = "AbsentPath" },
		{ .key = "bytes", .text = "bytes" },
		{ .key = "GB", .text = "GB" },
		{ .key = "KB", .text = "KB" },
		{ .key = "MB", .text = "MB" },
		{ .key = "MenuAbsent", .text = "Entire feature will be unavailable" },
		{ .key = "MenuAdvertise", .text = "Feature will be installed when required" },
		{ .key = "MenuAllCD", .text = "Entire feature will be installed to run from CD" },
		{ .key = "MenuAllLocal", .text = "Entire feature will be installed on local hard drive" },
		{ .key = "MenuAllNetwork", .text = "Entire feature will be installed to run from network" },
		{ .key = "MenuCD", .text = "Will be installed to run from CD" },
		{ .key = "MenuLocal", .text = "Will be installed on local hard drive" },
		{ .key = "MenuNetwork", .text = "Will be installed to run from network" },
		{ .key = "ScriptInProgress", .text = "Gathering required information..." },
		{ .key = "SelAbsentAbsent", .text = "This feature will remain uninstalled" },
		{ .key = "SelAbsentAdvertise", .text = "This feature will be set to be installed when required" },
		{ .key = "SelAbsentCD", .text = "This feature will be installed to run from CD" },
		{ .key = "SelAbsentLocal", .text = "This feature will be installed on the local hard drive" },
		{ .key = "SelAbsentNetwork", .text = "This feature will be installed to run from the network" },
		{ .key = "SelAdvertiseAbsent", .text = "This feature will become unavailable" },
		{ .key = "SelAdvertiseAdvertise", .text = "Will be installed when required" },
		{ .key = "SelAdvertiseCD", .text = "This feature will be available to run from CD" },
		{ .key = "SelAdvertiseLocal", .text = "This feature will be installed on your local hard drive" },
		{ .key = "SelAdvertiseNetwork", .text = "This feature will be available to run from the network" },
		{ .key = "SelCDAbsent", .text = "This feature will be uninstalled completely, you won't be able to run it from CD" },
		{ .key = "SelCDAdvertise", .text = "This feature will change from run from CD state to set to be installed when required" },
		{ .key = "SelCDCD", .text = "This feature will remain to be run from CD" },
		{ .key = "SelCDLocal", .text = "This feature will change from run from CD state to be installed on the local hard drive" },
		{ .key = "SelChildCostNeg", .text = "This feature frees up [1] on your hard drive." },
		{ .key = "SelChildCostPos", .text = "This feature requires [1] on your hard drive." },
		{ .key = "SelCostPending", .text = "Compiling cost for this feature..." },
		{ .key = "SelLocalAbsent", .text = "This feature will be completely removed" },
		{ .key = "SelLocalAdvertise", .text = "This feature will be removed from your local hard drive, but will be set to be installed when required" },
		{ .key = "SelLocalCD", .text = "This feature will be removed from your local hard drive, but will be still available to run from CD" },
		{ .key = "SelLocalLocal", .text = "This feature will remain on you local hard drive" },
		{ .key = "SelLocalNetwork", .text = "This feature will be removed from your local hard drive, but will be still available to run from the network" },
		{ .key = "SelNetworkAbsent", .text = "This feature will be uninstalled completely, you won't be able to run it from the network" },
		{ .key = "SelNetworkAdvertise", .text = "This feature will change from run from network state to set to be installed when required" },
		{ .key = "SelNetworkLocal", .text = "This feature will change from run from network state to be installed on the local hard drive" },
		{ .key = "SelNetworkNetwork", .text = "This feature will remain to be run from the network" },
		{ .key = "SelParentCostNegNeg", .text = "This feature frees up [1] on your hard drive. It has [2] of [3] subfeatures selected. The subfeatures free up [4] on your hard drive." },
		{ .key = "SelParentCostNegPos", .text = "This feature frees up [1] on your hard drive. It has [2] of [3] subfeatures selected. The subfeatures require [4] on your hard drive." },
		{ .key = "SelParentCostPosNeg", .text = "This feature requires [1] on your hard drive. It has [2] of [3] subfeatures selected. The subfeatures free up [4] on your hard drive." },
		{ .key = "SelParentCostPosPos", .text = "This feature requires [1] on your hard drive. It has [2] of [3] subfeatures selected. The subfeatures require [4] on your hard drive." },
		{ .key = "TimeRemaining", .text = "Time remaining: {[1] minutes }{[2] seconds}" },
		{ .key = "VolumeCostAvailable", .text = "Available" },
		{ .key = "VolumeCostDifference", .text = "Difference" },
		{ .key = "VolumeCostRequired", .text = "Required" },
		{ .key = "VolumeCostSize", .text = "Disk Size" },
		{ .key = "VolumeCostVolume", .text = "Volume" },
	};

	std::vector<MSIError> error = 
	{
		{ .error = 0, .message = "{{Fatal error: }}" },
		{ .error = 1, .message = "{{Error [1]. }}" },
		{ .error = 2, .message = "Warning [1]. " },
		{ .error = 3 },
		{ .error = 4, .message = "Info [1]. " },
		{ .error = 5, .message = "The installer has encountered an unexpected error installing this package. This may indicate a problem with this package. The error code is [1]. {{The arguments are: [2], [3], [4]}}" },
		{ .error = 6 },
		{ .error = 7, .message = "{{Disk full: }}" },
		{ .error = 8, .message = "Action [Time]: [1]. [2]" },
		{ .error = 9, .message = "[ProductName]" },
		{ .error = 10, .message = "{[2]}{, [3]}{, [4]}" },
		{ .error = 11, .message = "Message type: [1], Argument: [2]{, [3]}" },
		{ .error = 12, .message = "=== Logging started: [Date]  [Time] ===" },
		{ .error = 13, .message = "=== Logging stopped: [Date]  [Time] ===" },
		{ .error = 14, .message = "Action start [Time]: [1]." },
		{ .error = 15, .message = "Action ended [Time]: [1]. Return value [2]." },
		{ .error = 16, .message = "Time remaining: {[1] minutes }{[2] seconds}" },
		{ .error = 17, .message = "Out of memory. Shut down other applications before retrying." },
		{ .error = 18, .message = "Installer is no longer responding." },
		{ .error = 19, .message = "Installer stopped prematurely." },
		{ .error = 20, .message = "Please wait while Windows configures [ProductName]" },
		{ .error = 21, .message = "Gathering required information..." },
		{ .error = 22, .message = "Removing older versions of this application..." },
		{ .error = 23, .message = "Preparing to remove older versions of this application..." },
		{ .error = 32, .message = "{[ProductName] }Setup completed successfully." },
		{ .error = 33, .message = "{[ProductName] }Setup failed." },
		{ .error = 1101, .message = "Error reading from file: [2]. {{ System error [3].}}  Verify that the file exists and that you can access it." },
		{ .error = 1301, .message = "Cannot create the file '[2]'.  A directory with this name already exists.  Cancel the install and try installing to a different location." },
		{ .error = 1302, .message = "Please insert the disk: [2]" },
		{ .error = 1303, .message = "The installer has insufficient privileges to access this directory: [2].  The installation cannot continue.  Log on as administrator or contact your system administrator." },
		{ .error = 1304, .message = "Error writing to file: [2].  Verify that you have access to that directory." },
		{ .error = 1305, .message = "Error reading from file [2]. {{ System error [3].}} Verify that the file exists and that you can access it." },
		{ .error = 1306, .message = "Another application has exclusive access to the file '[2]'.  Please shut down all other applications, then click Retry." },
		{ .error = 1307, .message = "There is not enough disk space to install this file: [2].  Free some disk space and click Retry, or click Cancel to exit." },
		{ .error = 1308, .message = "Source file not found: [2].  Verify that the file exists and that you can access it." },
		{ .error = 1309, .message = "Error reading from file: [3]. {{ System error [2].}}  Verify that the file exists and that you can access it." },
		{ .error = 1310, .message = "Error writing to file: [3]. {{ System error [2].}}  Verify that you have access to that directory." },
		{ .error = 1311, .message = "Source file not found{{(cabinet)}}: [2].  Verify that the file exists and that you can access it." },
		{ .error = 1312, .message = "Cannot create the directory '[2]'.  A file with this name already exists.  Please rename or remove the file and click retry, or click Cancel to exit." },
		{ .error = 1313, .message = "The volume [2] is currently unavailable.  Please select another." },
		{ .error = 1314, .message = "The specified path '[2]' is unavailable." },
		{ .error = 1315, .message = "Unable to write to the specified folder: [2]." },
		{ .error = 1316, .message = "A network error occurred while attempting to read from the file: [2]" },
		{ .error = 1317, .message = "An error occurred while attempting to create the directory: [2]" },
		{ .error = 1318, .message = "A network error occurred while attempting to create the directory: [2]" },
		{ .error = 1319, .message = "A network error occurred while attempting to open the source file cabinet: [2]" },
		{ .error = 1320, .message = "The specified path is too long: [2]" },
		{ .error = 1321, .message = "The Installer has insufficient privileges to modify this file: [2]." },
		{ .error = 1322, .message = "A portion of the folder path '[2]' is invalid.  It is either empty or exceeds the length allowed by the system." },
		{ .error = 1323, .message = "The folder path '[2]' contains words that are not valid in folder paths." },
		{ .error = 1324, .message = "The folder path '[2]' contains an invalid character." },
		{ .error = 1325, .message = "'[2]' is not a valid short file name." },
		{ .error = 1326, .message = "Error getting file security: [3] GetLastError: [2]" },
		{ .error = 1327, .message = "Invalid Drive: [2]" },
		{ .error = 1328, .message = "Error applying patch to file [2].  It has probably been updated by other means, and can no longer be modified by this patch.  For more information contact your patch vendor.  {{System Error: [3]}}" },
		{ .error = 1329, .message = "A file that is required cannot be installed because the cabinet file [2] is not digitally signed.  This may indicate that the cabinet file is corrupt." },
		{ .error = 1330, .message = "A file that is required cannot be installed because the cabinet file [2] has an invalid digital signature.  This may indicate that the cabinet file is corrupt.{{  Error [3] was returned by WinVerifyTrust.}}" },
		{ .error = 1331, .message = "Failed to correctly copy [2] file: CRC error." },
		{ .error = 1332, .message = "Failed to correctly move [2] file: CRC error." },
		{ .error = 1333, .message = "Failed to correctly patch [2] file: CRC error." },
		{ .error = 1334, .message = "The file '[2]' cannot be installed because the file cannot be found in cabinet file '[3]'. This could indicate a network error, an error reading from the CD-ROM, or a problem with this package." },
		{ .error = 1335, .message = "The cabinet file '[2]' required for this installation is corrupt and cannot be used. This could indicate a network error, an error reading from the CD-ROM, or a problem with this package." },
		{ .error = 1336, .message = "There was an error creating a temporary file that is needed to complete this installation.{{  Folder: [3]. System error code: [2]}}" },
		{ .error = 1401, .message = "Could not create key: [2]. {{ System error [3].}}  Verify that you have sufficient access to that key, or contact your support personnel. " },
		{ .error = 1402, .message = "Could not open key: [2]. {{ System error [3].}}  Verify that you have sufficient access to that key, or contact your support personnel. " },
		{ .error = 1403, .message = "Could not delete value [2] from key [3]. {{ System error [4].}}  Verify that you have sufficient access to that key, or contact your support personnel. " },
		{ .error = 1404, .message = "Could not delete key [2]. {{ System error [3].}}  Verify that you have sufficient access to that key, or contact your support personnel. " },
		{ .error = 1405, .message = "Could not read value [2] from key [3]. {{ System error [4].}}  Verify that you have sufficient access to that key, or contact your support personnel. " },
		{ .error = 1406, .message = "Could not write value [2] to key [3]. {{ System error [4].}}  Verify that you have sufficient access to that key, or contact your support personnel." },
		{ .error = 1407, .message = "Could not get value names for key [2]. {{ System error [3].}}  Verify that you have sufficient access to that key, or contact your support personnel." },
		{ .error = 1408, .message = "Could not get sub key names for key [2]. {{ System error [3].}}  Verify that you have sufficient access to that key, or contact your support personnel." },
		{ .error = 1409, .message = "Could not read security information for key [2]. {{ System error [3].}}  Verify that you have sufficient access to that key, or contact your support personnel." },
		{ .error = 1410, .message = "Could not increase the available registry space. [2] KB of free registry space is required for the installation of this application." },
		{ .error = 1500, .message = "Another installation is in progress. You must complete that installation before continuing this one." },
		{ .error = 1501, .message = "Error accessing secured data. Please make sure the Windows Installer is configured properly and try the install again." },
		{ .error = 1502, .message = "User '[2]' has previously initiated an install for product '[3]'.  That user will need to run that install again before they can use that product.  Your current install will now continue." },
		{ .error = 1503, .message = "User '[2]' has previously initiated an install for product '[3]'.  That user will need to run that install again before they can use that product." },
		{ .error = 1601, .message = "Out of disk space -- Volume: '[2]'; required space: [3] KB; available space: [4] KB.  Free some disk space and retry." },
		{ .error = 1602, .message = "Are you sure you want to cancel?" },
		{ .error = 1603, .message = "The file [2][3] is being held in use{ by the following process: Name: [4], Id: [5], Window Title: '[6]'}.  Close that application and retry." },
		{ .error = 1604, .message = "The product '[2]' is already installed, preventing the installation of this product.  The two products are incompatible." },
		{ .error = 1605, .message = "There is not enough disk space on the volume '[2]' to continue the install with recovery enabled. [3] KB are required, but only [4] KB are available. Click Ignore to continue the install without saving recovery information, click Retry to check for available space again, or click Cancel to quit the installation." },
		{ .error = 1606, .message = "Could not access network location [2]." },
		{ .error = 1607, .message = "The following applications should be closed before continuing the install:" },
		{ .error = 1608, .message = "Could not find any previously installed compliant products on the machine for installing this product." },
		{ .error = 1609, .message = "An error occurred while applying security settings. [2] is not a valid user or group. This could be a problem with the package, or a problem connecting to a domain controller on the network. Check your network connection and click Retry, or Cancel to end the install. {{Unable to locate the user's SID, system error [3]}}" },
		{ .error = 1610, .message = "The setup must update files or services that cannot be updated while the system is running. If you choose to continue, a reboot will be required to complete the setup." },
		{ .error = 1611, .message = "The setup was unable to automatically close all applications that are using files that need to be updated. If you choose to continue, a reboot will be required to complete the setup." },
		{ .error = 1701, .message = "The key [2] is not valid.  Verify that you entered the correct key." },
		{ .error = 1702, .message = "The installer must restart your system before configuration of [2] can continue.  Click Yes to restart now or No if you plan to manually restart later." },
		{ .error = 1703, .message = "You must restart your system for the configuration changes made to [2] to take effect. Click Yes to restart now or No if you plan to manually restart later." },
		{ .error = 1704, .message = "An installation for [2] is currently suspended.  You must undo the changes made by that installation to continue.  Do you want to undo those changes?" },
		{ .error = 1705, .message = "A previous installation for this product is in progress.  You must undo the changes made by that installation to continue.  Do you want to undo those changes?" },
		{ .error = 1706, .message = "An installation package for the product [2] cannot be found. Try the installation again using a valid copy of the installation package '[3]'." },
		{ .error = 1707, .message = "Installation completed successfully." },
		{ .error = 1708, .message = "Installation failed." },
		{ .error = 1709, .message = "Product: [2] -- [3]" },
		{ .error = 1710, .message = "You may either restore your computer to its previous state or continue the install later. Would you like to restore?" },
		{ .error = 1711, .message = "An error occurred while writing installation information to disk.  Check to make sure enough disk space is available, and click Retry, or Cancel to end the install." },
		{ .error = 1712, .message = "One or more of the files required to restore your computer to its previous state could not be found.  Restoration will not be possible." },
		{ .error = 1713, .message = "[2] cannot install one of its required products. Contact your technical support group.  {{System Error: [3].}}" },
		{ .error = 1714, .message = "The older version of [2] cannot be removed.  Contact your technical support group.  {{System Error [3].}}" },
		{ .error = 1715, .message = "Installed [2]" },
		{ .error = 1716, .message = "Configured [2]" },
		{ .error = 1717, .message = "Removed [2]" },
		{ .error = 1718, .message = "File [2] was rejected by digital signature policy." },
		{ .error = 1719, .message = "The Windows Installer Service could not be accessed. This can occur if the Windows Installer is not correctly installed. Contact your support personnel for assistance." },
		{ .error = 1720, .message = "There is a problem with this Windows Installer package. A script required for this install to complete could not be run. Contact your support personnel or package vendor.  {{Custom action [2] script error [3], [4]: [5] Line [6], Column [7], [8] }}" },
		{ .error = 1721, .message = "There is a problem with this Windows Installer package. A program required for this install to complete could not be run. Contact your support personnel or package vendor. {{Action: [2], location: [3], command: [4] }}" },
		{ .error = 1722, .message = "There is a problem with this Windows Installer package. A program run as part of the setup did not finish as expected. Contact your support personnel or package vendor.  {{Action [2], location: [3], command: [4] }}" },
		{ .error = 1723, .message = "There is a problem with this Windows Installer package. A DLL required for this install to complete could not be run. Contact your support personnel or package vendor.  {{Action [2], entry: [3], library: [4] }}" },
		{ .error = 1724, .message = "Removal completed successfully." },
		{ .error = 1725, .message = "Removal failed." },
		{ .error = 1726, .message = "Advertisement completed successfully." },
		{ .error = 1727, .message = "Advertisement failed." },
		{ .error = 1728, .message = "Configuration completed successfully." },
		{ .error = 1729, .message = "Configuration failed." },
		{ .error = 1730, .message = "You must be an Administrator to remove this application. To remove this application, you can log on as an Administrator, or contact your technical support group for assistance." },
		{ .error = 1731, .message = "The source installation package for the product [2] is out of sync with the client package. Try the installation again using a valid copy of the installation package '[3]'." },
		{ .error = 1732, .message = "In order to complete the installation of [2], you must restart the computer. Other users are currently logged on to this computer, and restarting may cause them to lose their work. Do you want to restart now?" },
		{ .error = 1733, .message = "The Windows Installer service is not accessible in Safe Mode. Please try again when your computer is not in Safe Mode or you can use System Restore to return your machine to a previous good state." },
		{ .error = 1801, .message = "The path [2] is not valid.  Please specify a valid path." },
		{ .error = 1802, .message = "Out of memory. Shut down other applications before retrying." },
		{ .error = 1803, .message = "There is no disk in drive [2]. Please insert one and click Retry, or click Cancel to go back to the previously selected volume." },
		{ .error = 1804, .message = "There is no disk in drive [2]. Please insert one and click Retry, or click Cancel to return to the browse dialog and select a different volume." },
		{ .error = 1805, .message = "The folder [2] does not exist.  Please enter a path to an existing folder." },
		{ .error = 1806, .message = "You have insufficient privileges to read this folder." },
		{ .error = 1807, .message = "A valid destination folder for the install could not be determined." },
		{ .error = 1901, .message = "Error attempting to read from the source install database: [2]." },
		{ .error = 1902, .message = "Scheduling reboot operation: Renaming file [2] to [3]. Must reboot to complete operation." },
		{ .error = 1903, .message = "Scheduling reboot operation: Deleting file [2]. Must reboot to complete operation." },
		{ .error = 1904, .message = "Module [2] failed to register.  HRESULT [3].  Contact your support personnel." },
		{ .error = 1905, .message = "Module [2] failed to unregister.  HRESULT [3].  Contact your support personnel." },
		{ .error = 1906, .message = "Failed to cache package [2]. Error: [3]. Contact your support personnel." },
		{ .error = 1907, .message = "Could not register font [2].  Verify that you have sufficient permissions to install fonts, and that the system supports this font." },
		{ .error = 1908, .message = "Could not unregister font [2]. Verify that you that you have sufficient permissions to remove fonts." },
		{ .error = 1909, .message = "Could not create Shortcut [2]. Verify that the destination folder exists and that you can access it." },
		{ .error = 1910, .message = "Could not remove Shortcut [2]. Verify that the shortcut file exists and that you can access it." },
		{ .error = 1911, .message = "Could not register type library for file [2].  Contact your support personnel." },
		{ .error = 1912, .message = "Could not unregister type library for file [2].  Contact your support personnel." },
		{ .error = 1913, .message = "Could not update the ini file [2][3].  Verify that the file exists and that you can access it." },
		{ .error = 1914, .message = "Could not schedule file [2] to replace file [3] on reboot.  Verify that you have write permissions to file [3]." },
		{ .error = 1915, .message = "Error removing ODBC driver manager, ODBC error [2]: [3]. Contact your support personnel." },
		{ .error = 1916, .message = "Error installing ODBC driver manager, ODBC error [2]: [3]. Contact your support personnel." },
		{ .error = 1917, .message = "Error removing ODBC driver: [4], ODBC error [2]: [3]. Verify that you have sufficient privileges to remove ODBC drivers." },
		{ .error = 1918, .message = "Error installing ODBC driver: [4], ODBC error [2]: [3]. Verify that the file [4] exists and that you can access it." },
		{ .error = 1919, .message = "Error configuring ODBC data source: [4], ODBC error [2]: [3]. Verify that the file [4] exists and that you can access it." },
		{ .error = 1920, .message = "Service '[2]' ([3]) failed to start.  Verify that you have sufficient privileges to start system services." },
		{ .error = 1921, .message = "Service '[2]' ([3]) could not be stopped.  Verify that you have sufficient privileges to stop system services." },
		{ .error = 1922, .message = "Service '[2]' ([3]) could not be deleted.  Verify that you have sufficient privileges to remove system services." },
		{ .error = 1923, .message = "Service '[2]' ([3]) could not be installed.  Verify that you have sufficient privileges to install system services." },
		{ .error = 1924, .message = "Could not update environment variable '[2]'.  Verify that you have sufficient privileges to modify environment variables." },
		{ .error = 1925, .message = "You do not have sufficient privileges to complete this installation for all users of the machine.  Log on as administrator and then retry this installation." },
		{ .error = 1926, .message = "Could not set file security for file '[3]'. Error: [2].  Verify that you have sufficient privileges to modify the security permissions for this file." },
		{ .error = 1927, .message = "Component Services (COM+ 1.0) are not installed on this computer.  This installation requires Component Services in order to complete successfully.  Component Services are available on Windows 2000." },
		{ .error = 1928, .message = "Error registering COM+ Application.  Contact your support personnel for more information." },
		{ .error = 1929, .message = "Error unregistering COM+ Application.  Contact your support personnel for more information." },
		{ .error = 1930, .message = "The description for service '[2]' ([3]) could not be changed." },
		{ .error = 1931, .message = "The Windows Installer service cannot update the system file [2] because the file is protected by Windows.  You may need to update your operating system for this program to work correctly. {{Package version: [3], OS Protected version: [4]}}" },
		{ .error = 1932, .message = "The Windows Installer service cannot update the protected Windows file [2]. {{Package version: [3], OS Protected version: [4], SFP Error: [5]}}" },
		{ .error = 1933, .message = "The Windows Installer service cannot update one or more protected Windows files. {{SFP Error: [2].  List of protected files:\\r\\n[3]}}" },
		{ .error = 1934, .message = "User installations are disabled via policy on the machine." },
		{ .error = 1935, .message = "An error occurred during the installation of assembly '[6]'. Please refer to Help and Support for more information. {{HRESULT: [3]. assembly interface: [4], function: [5], component: [2]}}" },
		{ .error = 1936, .message = "An error occurred during the installation of assembly '[6]'. The assembly is not strongly named or is not signed with the minimal key length. {{HRESULT: [3]. assembly interface: [4], function: [5], component: [2]}}" },
		{ .error = 1937, .message = "An error occurred during the installation of assembly '[6]'. The signature or catalog could not be verified or is not valid. {{HRESULT: [3]. assembly interface: [4], function: [5], component: [2]}}" },
		{ .error = 1938, .message = "An error occurred during the installation of assembly '[6]'. One or more modules of the assembly could not be found. {{HRESULT: [3]. assembly interface: [4], function: [5], component: [2]}}" },
	};

	void createTables(MSIDatabase* db, std::vector<MSIProperty>& propertyList)
	{
		propertyList.insert(propertyList.end(), property.begin(), property.end());

		std::vector<MSIDialog> dialogs;
		std::vector<MSIControl> controls;
		std::vector<MSIControlCondition> controlCondition;
		std::vector<MSIControlEvent> controlEvent;
		std::vector<MSIEventMapping> eventMapping;

		addDialog(adminWelcomeDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(exitDialog, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(fatalError, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(prepareDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(progressDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(userExit, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(adminBrowseDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(adminInstallPointDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(adminRegistrationDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(browseDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(cancelDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(customizeDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(diskCostDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(errorDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(filesInUse, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(licenseAgreementDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(maintenanceTypeDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(maintenanceWelcomeDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(msiRMFilesInUse, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(outOfDiskDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(outOfRbDiskDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(resumeDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(setupTypeDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(userRegistrationDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(verifyReadyDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(verifyRemoveDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(verifyRepairDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(waitForCostingDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(welcomeDlg, dialogs, controls, controlCondition, controlEvent, eventMapping);
		addDialog(welcomeDlgLink, dialogs, controls, controlCondition, controlEvent, eventMapping);

		db->createTable(dialogs);
		db->createTable(controls);
		db->createTable(controlCondition);
		db->createTable(controlEvent);
		db->createTable(eventMapping);

		db->createTable(textStyle);
		db->createTable(bbControl);
		db->createTable(billboard);
		db->createTable(checkBox);
		db->createTable(radioButton);
		db->createTable(comboBox);
		db->createTable(listBox);
		db->createTable(listView);
		// db->createTable(binary);
	}

private:
	template<typename T>
	void addDialog(T& dialog, std::vector<MSIDialog>& dialogs, std::vector<MSIControl>& controls, std::vector<MSIControlCondition>& controlCondition, std::vector<MSIControlEvent>& controlEvent, std::vector<MSIEventMapping>& eventMapping)
	{
		dialogs.push_back(dialog.dialog);
		controls.insert(controls.end(), dialog.controls.begin(), dialog.controls.end());
		controlCondition.insert(controlCondition.end(), dialog.controlCondition.begin(), dialog.controlCondition.end());
		controlEvent.insert(controlEvent.end(), dialog.controlEvent.begin(), dialog.controlEvent.end());
		eventMapping.insert(eventMapping.end(), dialog.eventMapping.begin(), dialog.eventMapping.end());
	}
};
