//=============================================================================
//
// �Q�[������ [game.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "game.h"
#include "manager.h"
#include "result.h"
#include "stageselect.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
CPlayer* CGame::m_pPlayer = NULL;
CTime* CGame::m_pTime = NULL;					// �^�C���ւ̃|�C���^
CColon* CGame::m_pColon = NULL;					// �R�����ւ̃|�C���^
CBlock* CGame::m_pBlock= NULL;					// �u���b�N�ւ̃|�C���^
CBlockManager* CGame::m_pBlockManager= NULL;	// �u���b�N�}�l�[�W���[�ւ̃|�C���^
CImGuiManager* CGame::m_pImGuiManager= NULL;	// ImGui�}�l�[�W���[�ւ̃|�C���^
CObjectBillboard* CGame::m_pBillboard = NULL;	// �r���{�[�h�ւ̃|�C���^
CUi* CGame::m_pUi = NULL;						// UI�ւ̃|�C���^
CPauseManager* CGame::m_pPauseManager = NULL;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CGame::CGame() : CScene(CScene::MODE_GAME)
{
	// �l�̃N���A
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CGame::~CGame()
{
	// �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CGame::Init(void)
{
	// �u���b�N�}�l�[�W���[�̐���
	m_pBlockManager = new CBlockManager;

	// �u���b�N�}�l�[�W���[�̏�����
	m_pBlockManager->Init();


	int stageId = CStageSelect::GetSelectedStage();

	switch (stageId)
	{
	case CStage::STAGE_ID_1:
		// �r���{�[�h�̐���
		m_pBillboard = CObjectBillboard::Create("data/TEXTURE/ui_move.png", D3DXVECTOR3(145.0f, 130.0f, -20.0f), 85.0f, 20.0f);
		m_pBillboard = CObjectBillboard::Create("data/TEXTURE/ui_jump.png", D3DXVECTOR3(-150.0f, 150.0f, 385.0f), 80.0f, 20.0f);
		m_pBillboard = CObjectBillboard::Create("data/TEXTURE/ui_pick.png", D3DXVECTOR3(150.0f, 130.0f, 1220.0f), 80.0f, 20.0f);

		// �v���C���[�̐���
		m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 100.0f, -300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		//m_pPlayer = CPlayer::Create(D3DXVECTOR3(-660.0f, 100.0f, -3898.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

		m_pUi = CUi::Create(CUi::TYPE_STAGE_NAME, "data/TEXTURE/stage_01.png", D3DXVECTOR3(860.0f, 480.0f, 0.0f), 230.0f, 50.0f);

		// JSON�̓ǂݍ���
		m_pBlockManager->LoadFromJson("data/stage_01.json");

		break;
	case CStage::STAGE_ID_2:
		// �v���C���[�̐���
		m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 100.0f, -300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		//m_pPlayer = CPlayer::Create(D3DXVECTOR3(1542.3f, 360.0f, -850.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		//m_pPlayer = CPlayer::Create(D3DXVECTOR3(1540.0f, 420.0f, -1296.5f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

		m_pUi = CUi::Create(CUi::TYPE_STAGE_NAME, "data/TEXTURE/stage_02.png", D3DXVECTOR3(860.0f, 480.0f, 0.0f), 230.0f, 50.0f);

		// JSON�̓ǂݍ���
		m_pBlockManager->LoadFromJson("data/stage_02.json");

		break;
	case CStage::STAGE_ID_3:
		// �v���C���[�̐���
		m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 100.0f, -300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

		// JSON�̓ǂݍ���
		m_pBlockManager->LoadFromJson("data/stage_03.json");

		break;
	case CStage::STAGE_ID_NONE:
		break;
	}

	float fTimePosX = 760.0f;
	float fTimeWidth = 42.0f;
	float fTimeHeight = 58.0f;

	// �^�C���̐���
	m_pTime = CTime::Create(0, 0, fTimePosX, 10.0f, fTimeWidth, fTimeHeight);

	// �R�����̐���
	m_pColon = CColon::Create(D3DXVECTOR3(fTimePosX + 2 * fTimeWidth, 10.0f, 0.0f), fTimeWidth / 2, fTimeHeight);

	// �|�[�YUI�̐���
	m_pUi = CUi::Create(CUi::TYPE_PAUSE, "data/TEXTURE/ui_pause.png",D3DXVECTOR3(210.0f, 855.0f, 0.0f), 160.0f, 35.0f);

	// �Q�[��BGM�̍Đ�
	CManager::GetSound()->Play(CSound::SOUND_LABEL_GAMEBGM);

	// �|�[�Y�}�l�[�W���[�̐���
	m_pPauseManager = new CPauseManager();

	// �|�[�Y�}�l�[�W���[�̏�����
	m_pPauseManager->Init();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CGame::Uninit(void)
{
	// ���̒�~
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_GAMEBGM);

	// �u���b�N�}�l�[�W���[�̔j��
	if (m_pBlockManager != NULL)
	{
		m_pBlockManager->Uninit();

		delete m_pBlockManager;
		m_pBlockManager = NULL;
	}

	// �|�[�Y�}�l�[�W���[�̔j��
	if (m_pPauseManager != NULL)
	{
		// �|�[�Y�}�l�[�W���[�̏I������
		m_pPauseManager->Uninit();

		delete m_pPauseManager;
		m_pPauseManager = NULL;
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void CGame::Update(void)
{
	CFade* pFade = CManager::GetFade();

	// �u���b�N�}�l�[�W���[�̍X�V����
	m_pBlockManager->Update();

	// �I������`�F�b�N
	if (m_pBlockManager)
	{
		for (auto block : m_pBlockManager->GetAllBlocks())
		{
			if (block->IsGet())
			{// ���ʂ���ɓ��ꂽ��
				CResult::SetGet(true);
			}

			if (block->IsEnd())
			{
				// ���U���g�ɃZ�b�g
				CResult::SetClearTime(m_pTime->GetMinutes(), m_pTime->GetnSeconds());

				// ���U���g��ʂɈڍs
				pFade->SetFade(MODE_RESULT);

				break;
			}
		}
	}

#ifdef _DEBUG
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	if (pFade->GetFade() == CFade::FADE_NONE && pInputKeyboard->GetTrigger(DIK_RETURN))
	{
		// ���U���g�ɃZ�b�g
		CResult::SetClearTime(m_pTime->GetMinutes(), m_pTime->GetnSeconds());

		// ���U���g��ʂɈڍs
		pFade->SetFade(MODE_RESULT);
	}
#endif
}
//=============================================================================
// �`�揈��
//=============================================================================
void CGame::Draw(void)
{
	// �|�[�Y����������
	if (CManager::GetisPaused())
	{
		// �|�[�Y�}�l�[�W���[�̕`�揈��
		m_pPauseManager->Draw();
	}
}