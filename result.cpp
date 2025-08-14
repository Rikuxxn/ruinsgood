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
	m_pVtxBuff = NULL;		// ���_�o�b�t�@�ւ̃|�C���^
	m_pos = INIT_VEC3;		// �ʒu
	m_col = INIT_XCOL;		// �F
	m_fWidth = 0.0f;
	m_fHeight = 0.0f;
	m_nIdxTexture = 0;
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

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���̎擾
	m_nIdxTexture = CManager::GetTexture()->Register("data/TEXTURE/resultBG.png");

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// ������
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fWidth = 1920.0f;
	m_fHeight = 1080.0f;

	VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(m_pos.x, m_pos.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x, m_pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y + m_fHeight, 0.0f);

	// rhw�̐ݒ�
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// ���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();

	float fTimePosX = 1100.0f;
	float fTimeWidth = 72.0f;
	float fTimeHeight = 88.0f;

	// �^�C���̐���
	m_pTime = CTime::Create(m_nClearMinutes, m_nClearSeconds, fTimePosX, 695.0f, fTimeWidth, fTimeHeight);

	// �R�����̐���
	m_pColon = CColon::Create(D3DXVECTOR3(fTimePosX + 2 * fTimeWidth, 695.0f, 0.0f), fTimeWidth / 2, fTimeHeight);

	// UI�̐���
	m_pUi = CUi::Create(CUi::TYPE_RESULT01, "data/TEXTURE/ui_result001.png",D3DXVECTOR3(450.0f, 575.0f, 0.0f), 260.0f, 50.0f);
	m_pUi = CUi::Create(CUi::TYPE_RESULT01, "data/TEXTURE/ui_result002.png", D3DXVECTOR3(1250.0f, 595.0f, 0.0f), 260.0f, 50.0f);
	m_pUi = CUi::Create(CUi::TYPE_RESULT01, "data/TEXTURE/ui_result005.png", D3DXVECTOR3(860.0f, 155.0f, 0.0f), 150.0f, 60.0f);

	// UI(�����N)�̐���
	m_pUi = CUi::Create(CUi::TYPE_RESULT03, "data/TEXTURE/ui_result006.png", D3DXVECTOR3(860.0f, 355.0f, 0.0f), 120.0f, 130.0f);

	int stageId = CStageSelect::GetSelectedStage();

	switch (stageId)
	{
	case CStage::STAGE_ID_1:
		if (m_isMaskGet)
		{
			// UI(�����������ǂ���)�̐���
			m_pUi = CUi::Create(CUi::TYPE_RESULT02, "data/TEXTURE/ui_result004.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 120.0f);
		}
		else if (!m_isMaskGet)
		{
			// UI(�����������ǂ���)�̐���
			m_pUi = CUi::Create(CUi::TYPE_RESULT02, "data/TEXTURE/ui_result003.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 120.0f);
		}
		break;
	case CStage::STAGE_ID_2:
		if (m_isMaskGet)
		{
			// UI(�����������ǂ���)�̐���
			m_pUi = CUi::Create(CUi::TYPE_RESULT02, "data/TEXTURE/ui_result004.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 120.0f);
		}
		else if (!m_isMaskGet)
		{
			// UI(�����������ǂ���)�̐���
			m_pUi = CUi::Create(CUi::TYPE_RESULT02, "data/TEXTURE/ui_result003.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 120.0f);
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

	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}
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
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, CManager::GetTexture()->GetAddress(m_nIdxTexture));

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
