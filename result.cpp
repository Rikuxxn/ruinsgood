//=============================================================================
//
// ���U���g���� [result.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "result.h"
#include "input.h"
#include "manager.h"
#include "stageselect.h"
#include "background.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
CTime* CResult::m_pTime = NULL;					// �^�C���ւ̃|�C���^
CColon* CResult::m_pColon = NULL;				// �R�����ւ̃|�C���^
int CResult::m_nClearMinutes = 0;
int CResult::m_nClearSeconds = 0;
bool CResult::m_isMaskGet = false;
CUi* CResult::m_pUi = NULL;						// UI�ւ̃|�C���^

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CResult::CResult() : CScene(CScene::MODE_RESULT)
{
	// �l�̃N���A
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CResult::~CResult()
{
	// �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CResult::Init(void)
{
	// �}�E�X�J�[�\����\������
	CManager::GetInputMouse()->SetCursorVisibility(true);

	// �w�i�̐���
	CBackground::Create(D3DXVECTOR3(960.0f, 540.0f, 0.0f), 960.0f, 540.0f, "data/TEXTURE/resultBG.png");

	float fTimePosX = 1100.0f;
	float fTimeWidth = 72.0f;
	float fTimeHeight = 88.0f;

	// �^�C���̐���
	m_pTime = CTime::Create(m_nClearMinutes, m_nClearSeconds, fTimePosX, 695.0f, fTimeWidth, fTimeHeight);

	// �R�����̐���
	m_pColon = CColon::Create(D3DXVECTOR3(fTimePosX + 2 * fTimeWidth, 695.0f, 0.0f), fTimeWidth / 2, fTimeHeight);

	// UI�̐���
	m_pUi = CUi::Create(CUi::TYPE_RESULT_UI, "data/TEXTURE/ui_result001.png",D3DXVECTOR3(450.0f, 575.0f, 0.0f), 260.0f, 50.0f);
	m_pUi = CUi::Create(CUi::TYPE_RESULT_UI, "data/TEXTURE/ui_result002.png", D3DXVECTOR3(1250.0f, 595.0f, 0.0f), 260.0f, 50.0f);
	m_pUi = CUi::Create(CUi::TYPE_RESULT_UI, "data/TEXTURE/ui_result005.png", D3DXVECTOR3(860.0f, 155.0f, 0.0f), 150.0f, 60.0f);

	// UI(�����N)�̐���
	m_pUi = CUi::Create(CUi::TYPE_RESULT_RANK, "data/TEXTURE/ui_result006.png", D3DXVECTOR3(860.0f, 355.0f, 0.0f), 120.0f, 130.0f);

	int stageId = CStageSelect::GetSelectedStage();

	switch (stageId)
	{
	case CStage::STAGE_ID_1:
		if (m_isMaskGet)
		{
			// UI(�����������ǂ���)�̐���
			m_pUi = CUi::Create(CUi::TYPE_RESULT_UI, "data/TEXTURE/ui_result004.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 120.0f);
		}
		else if (!m_isMaskGet)
		{
			// UI(�����������ǂ���)�̐���
			m_pUi = CUi::Create(CUi::TYPE_RESULT_UI, "data/TEXTURE/ui_result003.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 120.0f);
		}
		break;
	case CStage::STAGE_ID_2:

		if (m_isMaskGet)
		{
			// UI(�����������ǂ���)�̐���
			m_pUi = CUi::Create(CUi::TYPE_RESULT_UI, "data/TEXTURE/ui_eggfound.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 190.0f);
		}
		else if (!m_isMaskGet)
		{
			// UI(�����������ǂ���)�̐���
			m_pUi = CUi::Create(CUi::TYPE_RESULT_UI, "data/TEXTURE/ui_eggnotfound.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 190.0f);
		}
		break;
	case CStage::STAGE_ID_3:
		break;
	case CStage::STAGE_ID_NONE:
		break;
	};

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CResult::Uninit(void)
{
	SetGet(false);

}
//=============================================================================
// �X�V����
//=============================================================================
void CResult::Update(void)
{
	CInputMouse* pInputMouse = CManager::GetInputMouse();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();
	CFade* pFade = CManager::GetFade();

	if (pFade->GetFade() == CFade::FADE_NONE && (pInputMouse->GetTrigger(0) || pJoypad->GetTrigger(CInputJoypad::JOYKEY_A)))
	{
		// �Q�[����ʂɈڍs
		pFade->SetFade(MODE_TITLE);
	}

#ifdef _DEBUG
	if (pFade->GetFade() == CFade::FADE_NONE && CManager::GetInputKeyboard()->GetTrigger(DIK_RETURN))
	{
		// �Q�[����ʂɈڍs
		pFade->SetFade(MODE_TITLE);
	}
#endif
}
//=============================================================================
// �`�揈��
//=============================================================================
void CResult::Draw(void)
{


}
