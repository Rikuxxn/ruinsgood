//=============================================================================
//
// �X�e�[�W���� [stage.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "stage.h"
#include "renderer.h"
#include "manager.h"
#include "stageselect.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CStage::CStage(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	m_pVtxBuff = NULL;
	m_pos = INIT_VEC3;
	m_fWidth = 0.0f;
	m_fHeight = 0.0f;
	m_nIdxTexture = 0;
	memset(m_szPath, 0, sizeof(m_szPath));
	m_isSelected = false;
	m_startPos = INIT_VEC3;	// �X���C�h�C���J�n�ʒu�i��ʉE�O�j
	m_targetPos = INIT_VEC3;// �X���C�h�C�������ʒu�i�ړI�ʒu�j
	m_isSlidingIn = false;	// �X���C�h���t���O
	m_isSlidingOut = false;	// �X���C�h�A�E�g�t���O
	m_isSlideOutFinished = false;
	m_isSlideInFinished = false;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CStage::~CStage()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CStage* CStage::Create(STAGE type, D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	CStage* pStage = NULL;

	switch (type)
	{
	case STAGE_ID_1:
		pStage = new CStage1;
		pStage->SetPath("data/TEXTURE/stage_01.png");
		break;
	case STAGE_ID_2:
		pStage = new CStage2;
		pStage->SetPath("data/TEXTURE/stage_none.png");
		break;
	case STAGE_ID_3:
		pStage = new CStage3;
		pStage->SetPath("data/TEXTURE/stage_none.png");
		break;
	case STAGE_ID_NONE:
		pStage = new CBack;
		pStage->SetPath("data/TEXTURE/back.png");
		break;
	default:
		pStage = new CStage;
		break;
	}

	pStage->SetPos(pos);
	pStage->m_fWidth = fWidth;
	pStage->m_fHeight = fHeight;
	pStage->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	pStage->m_targetPos = D3DXVECTOR3(1100.0f, pos.y, pos.z);

	// ����������
	pStage->Init();

	return pStage;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CStage::Init(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	m_nIdxTexture = CManager::GetTexture()->Register(m_szPath);

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// �X���C�h�J�n�ʒu
	m_startPos = m_pos;

	// �X���C�h���t���O�͍ŏ���false
	m_isSlidingIn = false;

	VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_fWidth, m_pos.y - m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y - m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_fWidth, m_pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y + m_fHeight, 0.0f);

	// rhw�̐ݒ�
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// ���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CStage::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	this->Release();
}
//=============================================================================
// �X�V����
//=============================================================================
void CStage::Update(void)
{
	// �X���C�h�C������
	SlideIn();

	// �X���C�h�A�E�g����
	SlideOut();

	VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

	// �I�����ڂ̐F�̐؂�ւ�
	if (IsMouseOver() || m_isSelected)
	{
		m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	}

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_fWidth, m_pos.y - m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y - m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_fWidth, m_pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y + m_fHeight, 0.0f);

	// ���_�J���[�̐ݒ�
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// ���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();
}
//=============================================================================
// �`�揈��
//=============================================================================
void CStage::Draw(void)
{
	if (!CStageSelect::IsVisible())
	{
		return;
	}

	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//=============================================
	// �X�e�[�W����
	//=============================================
	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
//=============================================================================
// �ʒu�̎擾����
//=============================================================================
D3DXVECTOR3 CStage::GetPos(void)
{
	return m_pos;
}
//=============================================================================
// �}�E�X�J�[�\���̔��菈��
//=============================================================================
bool CStage::IsMouseOver(void)
{
	// �}�E�X�J�[�\���̈ʒu���擾
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	// �E�B���h�E�n���h�����擾
	HWND hwnd = GetActiveWindow();

	// �X�N���[�����W���N���C�A���g���W�ɕϊ�
	ScreenToClient(hwnd, &cursorPos);

	// �N���C�A���g�T�C�Y���擾
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	float left = m_pos.x - m_fWidth;
	float right = m_pos.x + m_fWidth;
	float top = m_pos.y - m_fHeight;
	float bottom = m_pos.y + m_fHeight;

	return (cursorPos.x >= left && cursorPos.x <= right &&
		cursorPos.y >= top && cursorPos.y <= bottom);
}
//=============================================================================
// �X���C�h�C������
//=============================================================================
void CStage::SlideIn(void)
{
	// �X���C�h���x
	const float slideSpeed = 10.0f;

	if (m_isSlidingIn)
	{
		// �ړI�n�֌����Ĉړ�
		if (m_pos.x > m_targetPos.x)
		{
			m_pos.x -= slideSpeed;

			if (m_pos.x <= m_targetPos.x)
			{
				m_pos.x = m_targetPos.x;
				m_isSlidingIn = false; // �X���C�h����
				m_isSlideOutFinished = false;
				m_isSlideInFinished = true;
			}
		}
	}
}
//=============================================================================
// �X���C�h�A�E�g����
//=============================================================================
void CStage::SlideOut(void)
{
	const float slideSpeed = 20.0f;

	if (m_isSlidingOut)
	{
		// �ړI�n�֌����Ĉړ�
		if (m_pos.x < m_startPos.x)
		{
			m_pos.x += slideSpeed;

			if (m_pos.x >= m_startPos.x)
			{
				m_pos.x = m_startPos.x;
				m_isSlidingOut = false; // �X���C�h����
				m_isSlideOutFinished = true;
				m_isSlideInFinished = false;
			}
		}
	}
}
//=============================================================================
// �X�e�[�WID��Ԃ�����
//=============================================================================
CStage::STAGE CStage::GetStageId(void) const
{
	if (dynamic_cast<const CStage1*>(this)) return STAGE_ID_1;
	if (dynamic_cast<const CStage2*>(this)) return STAGE_ID_2;
	if (dynamic_cast<const CStage3*>(this)) return STAGE_ID_3;
	if (dynamic_cast<const CBack*>(this))   return STAGE_ID_NONE;

	return STAGE_MAX; // �z��O
}


//=============================================================================
// �X�e�[�W1�̃R���X�g���N�^
//=============================================================================
CStage1::CStage1()
{
	// �l�̃N���A

}
//=============================================================================
// �X�e�[�W1�̃f�X�g���N�^
//=============================================================================
CStage1::~CStage1()
{
	// �Ȃ�
}
//=============================================================================
// �I�����̏���
//=============================================================================
void CStage1::Execute(void)
{
	// �Q�[����ʂɈڍs
	CManager::GetFade()->SetFade(CScene::MODE_GAME);
}


//=============================================================================
// �X�e�[�W2�̃R���X�g���N�^
//=============================================================================
CStage2::CStage2()
{
	// �l�̃N���A

}
//=============================================================================
// �X�e�[�W2�̃f�X�g���N�^
//=============================================================================
CStage2::~CStage2()
{
	// �Ȃ�
}
//=============================================================================
// �I�����̏���
//=============================================================================
void CStage2::Execute(void)
{
	// �Q�[����ʂɈڍs
	CManager::GetFade()->SetFade(CScene::MODE_GAME);
}


//=============================================================================
// �X�e�[�W3�̃R���X�g���N�^
//=============================================================================
CStage3::CStage3()
{
	// �l�̃N���A

}
//=============================================================================
// �X�e�[�W3�̃f�X�g���N�^
//=============================================================================
CStage3::~CStage3()
{
	// �Ȃ�
}
//=============================================================================
// �I�����̏���
//=============================================================================
void CStage3::Execute(void)
{
	//// �Q�[����ʂɈڍs
	//CManager::GetFade()->SetFade(CScene::MODE_GAME);
}

//=============================================================================
// �߂鍀�ڂ̃R���X�g���N�^
//=============================================================================
CBack::CBack()
{
	// �l�̃N���A

}
//=============================================================================
// �߂鍀�ڂ̃f�X�g���N�^
//=============================================================================
CBack::~CBack()
{
	// �Ȃ�
}
//=============================================================================
// �I�����̏���
//=============================================================================
void CBack::Execute(void)
{




}
