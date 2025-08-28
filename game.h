//=============================================================================
//
// �Q�[������ [game.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _GAME_H_// ���̃}�N����`������Ă��Ȃ�������
#define _GAME_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "scene.h"
#include "player.h"
#include "score.h"
#include "time.h"
#include "blockmanager.h"
#include "ui.h"
#include "pausemanager.h"
#include "hinttext.h"

//*****************************************************************************
// �Q�[���N���X
//*****************************************************************************
class CGame : public CScene
{
public:
	CGame();
	~CGame();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void LoadObject(int stageId);

	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CTime* GetTime(void) { return m_pTime; }
	static CBlock* GetBlock(void) { return m_pBlock; }
	static CBlockManager* GetBlockManager(void) { return m_pBlockManager; }
	static CImGuiManager* GetImGuiManager(void) { return m_pImGuiManager; }
	static CObjectBillboard* GetBillboard(void) { return m_pBillboard; }
	static CUi* GetUi(void) { return m_pUi; }
	static CPauseManager* GetPauseManager(void) { return m_pPauseManager; }
	static CHintText* GetHintText(void) { return m_pHintText; }
	static bool GetisPaused(void) { return m_isPaused; };
	static void SetEnablePause(bool bPause);

private:
	static CPlayer* m_pPlayer;					// �v���C���[�ւ̃|�C���^
	static CTime* m_pTime;						// �^�C���ւ̃|�C���^
	static CColon* m_pColon;					// �R�����ւ̃|�C���^
	static CBlock* m_pBlock;					// �u���b�N�ւ̃|�C���^
	static CBlockManager* m_pBlockManager;		// �u���b�N�}�l�[�W���[�ւ̃|�C���^
	static CImGuiManager* m_pImGuiManager;		// ImGui�}�l�[�W���[�ւ̃|�C���^
	static CObjectBillboard* m_pBillboard;		// �r���{�[�h�ւ̃|�C���^
	static CUi* m_pUi;							// UI�ւ̃|�C���^
	static CPauseManager* m_pPauseManager;		// �|�[�Y�}�l�[�W���[�ւ̃|�C���^
	static CHintText* m_pHintText;				// �q���g�ǉ�ւ̃|�C���^
	static bool m_isPaused;						// true�Ȃ�|�[�Y��

};

#endif
