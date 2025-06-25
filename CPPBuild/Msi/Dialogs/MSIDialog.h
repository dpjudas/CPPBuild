#pragma once

#include "Msi/MSISchema.h"
#include "AdminWelcomeDlg.h"
#include "ExitDialog.h"
#include "FatalError.h"
#include "PrepareDlg.h"
#include "ProgressDlg.h"
#include "UserExit.h"
#include "AdminBrowseDlg.h"
#include "AdminInstallPointDlg.h"
#include "AdminRegistrationDlg.h"
#include "BrowseDlg.h"
#include "CancelDlg.h"
#include "CustomizeDlg.h"
#include "DiskCostDlg.h"
#include "ErrorDlg.h"
#include "FilesInUse.h"
#include "LicenseAgreementDlg.h"
#include "MaintenanceTypeDlg.h"
#include "MaintenanceWelcomeDlg.h"
#include "MsiRMFilesInUse.h"
#include "OutOfDiskDlg.h"
#include "OutOfRbDiskDlg.h"
#include "ResumeDlg.h"
#include "SetupTypeDlg.h"
#include "UserRegistrationDlg.h"
#include "VerifyReadyDlg.h"
#include "VerifyRemoveDlg.h"
#include "VerifyRepairDlg.h"
#include "WaitForCostingDlg.h"
#include "WelcomeDlg.h"
#include "WelcomeDlgLink.h"

class MSIDialogs
{
public:
	MSIDialogs();

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

	std::vector<MSIBinary> binary;

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

	std::vector<MSIProperty> property;
	std::vector<MSITextStyle> textStyle;
	std::vector<MSIUIText> uiText;
	std::vector<MSIError> error;

	void createTables(MSIDatabase* db, std::vector<MSIProperty>& propertyList);

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
