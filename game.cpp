//=============================================================================
//
// �Q�[������ [game.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "game.h"
#include "manager.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
CPlayer* CGame::m_pPlayer = NULL;
CScore* CGame::m_pScore = NULL;					// �X�R�A�ւ̃|�C���^
CTime* CGame::m_pTime = NULL;						// �^�C���ւ̃|�C���^
CColon* CGame::m_pColon = NULL;					// �R�����ւ̃|�C���^
CPause* CGame::m_pPause= NULL;					// �|�[�Y�ւ̃|�C���^
CBlock* CGame::m_pBlock= NULL;					// �u���b�N�ւ̃|�C���^
CBlockManager* CGame::m_pBlockManager= NULL;		// �u���b�N�}�l�[�W���[�ւ̃|�C���^
CImGuiManager* CGame::m_pImGuiManager= NULL;		// ImGui�}�l�[�W���[�ւ̃|�C���^

bool CGame::m_isPaused = false;						// true�Ȃ�|�[�Y��

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

	// �v���C���[�̐���
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 100.0f, -300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	//m_pPlayer = CPlayer::Create(D3DXVECTOR3(-660.0f, 100.0f, -3898.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// JSON�̓ǂݍ���
	m_pBlockManager->LoadFromJson("data/block_info.json");

	//// �X�R�A�̐���
	//m_pScore = CScore::Create(920.0f, 10.0f, 42.0f, 58.0f);

	float fTimePosX = 760.0f;
	float fTimeWidth = 42.0f;
	float fTimeHeight = 58.0f;

	// �^�C���̐���
	m_pTime = CTime::Create(12, 15, fTimePosX, 10.0f, fTimeWidth, fTimeHeight);

	// �R�����̐���
	m_pColon = CColon::Create(D3DXVECTOR3(fTimePosX + 2 * fTimeWidth, 10.0f, 0.0f), fTimeWidth / 2, fTimeHeight);

	// �|�[�Y�̐���
	m_pPause = CPause::Create(D3DXVECTOR3(490.0f, 340.0f, 0.0f), 400.0f, 90.0f);

	//m_pSound->Play(CSound::SOUND_LABEL_GAMEBGM);

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CGame::Uninit(void)
{
	// �u���b�N�}�l�[�W���[�̔j��
	if (m_pBlockManager != NULL)
	{
		m_pBlockManager->Uninit();

		delete m_pBlockManager;
		m_pBlockManager = NULL;
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void CGame::Update(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();
	CInputMouse* pInputMouse = CManager::GetInputMouse();
	CFade* pFade = CManager::GetFade();
	CBlock* pBlock = GetBlock();

	// P�L�[�Ń|�[�YON/OFF
	if (pInputKeyboard->GetTrigger(DIK_P))
	{
		m_isPaused = !m_isPaused;
	}

	// �|�[�Y���̓Q�[���X�V���Ȃ�
	if (m_isPaused == true)
	{
		// �|�[�Y�̍X�V����
		m_pPause->Update();

		return;
	}

	// �u���b�N�}�l�[�W���[�̍X�V����
	m_pBlockManager->Update();

	if (pInputKeyboard->GetTrigger(DIK_RETURN))
	{
		// �Q�[����ʂɈڍs
		pFade->SetFade(MODE_RESULT);
	}
}
//=============================================================================
// �`�揈��
//=============================================================================
void CGame::Draw(void)
{
	// �|�[�Y����������
	if (m_isPaused)
	{
		// �|�[�Y�̕`�揈��
		m_pPause->Draw();
	}
}