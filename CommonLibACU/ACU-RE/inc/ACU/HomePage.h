#pragma once

#include "StandardPage.h"

class HomePage : public StandardPage
{
public:
	UIString m_MainMenuTitleTxt; //0x0088
	UIString m_OnlineTxt; //0x008C
	UIString m_NotOnlinePS4Txt; //0x0090
	UIString m_NotOnlinePS4DescTxt; //0x0094
	UIString m_ReloadLastCheckpointTxt; //0x0098
	UIString m_ReloadLastCheckpointTitleTxt; //0x009C
	UIString m_ReloadLastCheckpointDescTxt; //0x00A0
	UIString m_RestartMissionTxt; //0x00A4
	UIString m_RestartMissionTitleTxt; //0x00A8
	UIString m_RestartMissionDescTxt; //0x00AC
	UIString m_BackToFreeRoamTxt; //0x00B0
	UIString m_BackToFreeRoamWithGroupTxt; //0x00B4
	UIString m_QuitGroupAndMissionTxt; //0x00B8
	UIString m_QuitGroupTxt; //0x00BC
	UIString m_QuitFreeRoamConfirmation; //0x00C0
	UIString m_QuitWithGroupDescription; //0x00C4
	UIString m_QuitGroupDescription; //0x00C8
	UIString m_QuitFreeRoamDescription; //0x00CC
	UIString m_ContinueTxt; //0x00D0
	UIString m_UPlayTxt; //0x00D4
	UIString m_CharacterTxt; //0x00D8
	UIString m_ProgressTrackerTxt; //0x00DC
	UIString m_DatabaseTxt; //0x00E0
	UIString m_OptionsTxt; //0x00E4
	UIString m_TestsTxt; //0x00E8
	UIString m_PlayTogetherTxt; //0x00EC
	UIString m_OpenPlayersListTxt; //0x00F0
	UIString m_InvitePlayersTxt; //0x00F4
	UIString m_GoOnlineTxt; //0x00F8
	UIString m_QuickMatchTxt; //0x00FC
	UIString m_BrotherhoodTxt; //0x0100
	UIString m_EStoreTxt; //0x0104
	UIString m_InitiatesTxt; //0x0108
	UIString m_ExitToWindowsTxt; //0x010C
	UIString m_ExitToWindowsMessageBoxTitle; //0x0110
	UIString m_ExitToWindowsMessageBoxDescription; //0x0114
	UIString m_DurangoHelpAndSupportText; //0x0118
	char pad_011C[4]; //0x011C
	SharedPtrNew<Entity>* m_PlayTogetherPanel; //0x0120
	SharedPtrNew<Entity>* m_BrotherhoodPanel; //0x0128
	SharedPtrNew<Entity>* m_ContinuePanel; //0x0130
	SharedPtrNew<Entity>* m_InitiatesPanel; //0x0138
	SharedPtrNew<Entity>* m_CharacterPanel; //0x0140
	SharedPtrNew<Entity>* m_ProgressTrackerPanel; //0x0148
	SharedPtrNew<Entity>* PCVersionNumber; //0x0150
	SharedPtrNew<Entity>* shared_UI_GenMenu_MainNav_ListItem_WithMessage; //0x0158
	char pad_0160[632]; //0x0160
}; //Size: 0x03D8
assert_sizeof(HomePage, 0x3D8);
