//=============================================================================
//
// �Q�[������ [game.h]
// Author : TANEKAWA RIKU
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
#include "pause.h"
#include "blockmanager.h"

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

	static bool GetisPaused(void) { return m_isPaused; };
	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CScore* GetScore(void) { return m_pScore; }
	static CTime* GetTime(void) { return m_pTime; }
	static CBlock* GetBlock(void) { return m_pBlock; }
	static CBlockManager* GetBlockManager(void) { return m_pBlockManager; }
	static CImGuiManager* GetImGuiManager(void) { return m_pImGuiManager; }
	
private:
	static CPlayer* m_pPlayer;					// �v���C���[�ւ̃|�C���^
	static CScore* m_pScore;					// �X�R�A�ւ̃|�C���^
	static CTime* m_pTime;						// �^�C���ւ̃|�C���^
	static CColon* m_pColon;					// �R�����ւ̃|�C���^
	static CPause* m_pPause;					// �|�[�Y�ւ̃|�C���^
	static CBlock* m_pBlock;					// �u���b�N�ւ̃|�C���^
	static CBlockManager* m_pBlockManager;		// �u���b�N�}�l�[�W���[�ւ̃|�C���^
	static CImGuiManager* m_pImGuiManager;		// ImGui�}�l�[�W���[�ւ̃|�C���^

	static bool m_isPaused;						// true�Ȃ�|�[�Y��

};

#endif
