//=============================================================================
//
// UI���� [ui.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "ui.h"
#include "manager.h"
#include "result.h"


//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
std::unordered_map<CUi::TYPE, UiCreateFunc> CUi::m_UiFactoryMap = {};

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CUi::CUi(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	m_pVtxBuff = NULL;		// ���_�o�b�t�@�ւ̃|�C���^
	m_pos = INIT_VEC3;
	m_col = INIT_XCOL;
	m_fWidth = 0.0f;			// ��
	m_fHeight = 0.0f;			// ����
	m_nIdxTexture = 0;
	memset(m_szPath, 0, sizeof(m_szPath));
	m_type = TYPE_PAUSE;
	m_uvLeft = 0.0f;
	m_uvTop = 0.0f;
	m_uvWidth = 0.0f;
	m_uvHeight = 0.0f;
	m_isUVDirty = false;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CUi::~CUi()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CUi* CUi::Create(TYPE type, const char* path,D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	if (m_UiFactoryMap.empty())
	{
		// �t�@�N�g���[
		InitFactory();
	}

	CUi* pUi = nullptr;

	auto it = m_UiFactoryMap.find(type);
	if (it != m_UiFactoryMap.end())
	{
		pUi = it->second();
	}
	else
	{
		pUi = new CUi(); // �f�t�H���g���N���X
	}

	if (!pUi)
	{
		return NULL;
	}

	pUi->SetPath(path);
	pUi->SetPos(pos);
	pUi->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	pUi->SetSize(fWidth,fHeight);

	// ����������
	pUi->Init();

	return pUi;
}
//=============================================================================
// �t�@�N�g���[
//=============================================================================
void CUi::InitFactory(void)
{
	// ���X�g����ɂ���
	m_UiFactoryMap.clear();

	m_UiFactoryMap[CUi::TYPE_PAUSE]			= []() -> CUi* { return new CPauseUi(); };
	m_UiFactoryMap[CUi::TYPE_GET]			= []() -> CUi* { return new CGetUi(); };
	m_UiFactoryMap[CUi::TYPE_RESULT_UI]		= []() -> CUi* { return new CResultUi(); };
	m_UiFactoryMap[CUi::TYPE_RESULT_RANK]	= []() -> CUi* { return new CResultRankUi(); };
	m_UiFactoryMap[CUi::TYPE_STAGE_NAME]	= []() -> CUi* { return new CStageUi(); };

}
//=============================================================================
// ����������
//=============================================================================
HRESULT CUi::Init(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���̎擾
	m_nIdxTexture = CManager::GetTexture()->Register(m_szPath);

	//���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_2D* pVtx = NULL;// ���_���ւ̃|�C���^

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
void CUi::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// �I�u�W�F�N�g�̔j��(�������g)
	this->Release();
}
//=============================================================================
// �X�V����
//=============================================================================
void CUi::Update(void)
{
	VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

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

	if (m_isUVDirty)
	{
		pVtx[0].tex = D3DXVECTOR2(m_uvLeft, m_uvTop);
		pVtx[1].tex = D3DXVECTOR2(m_uvLeft + m_uvWidth, m_uvTop);
		pVtx[2].tex = D3DXVECTOR2(m_uvLeft, m_uvTop + m_uvHeight);
		pVtx[3].tex = D3DXVECTOR2(m_uvLeft + m_uvWidth, m_uvTop + m_uvHeight);

		m_isUVDirty = false;
	}

	//���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();
}
//=============================================================================
// �`�揈��
//=============================================================================
void CUi::Draw(void)
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
//=============================================================================
// UV�̐ݒ菈��
//=============================================================================
void CUi::SetUV(float u, float v, float w, float h)
{
	m_uvLeft = u;
	m_uvTop = v;
	m_uvWidth = w;
	m_uvHeight = h;
	m_isUVDirty = true;
}


//=============================================================================
// �|�[�YUI�̃R���X�g���N�^
//=============================================================================
CPauseUi::CPauseUi()
{
	// �l�̃N���A
}
//=============================================================================
// �|�[�YUI�̃f�X�g���N�^
//=============================================================================
CPauseUi::~CPauseUi()
{
	// �Ȃ�
}
//=============================================================================
// �|�[�YUI�̏���������
//=============================================================================
HRESULT CPauseUi::Init(void)
{
	// UI�̏���������
	CUi::Init();

	return S_OK;
}
//=============================================================================
// �|�[�YUI�̍X�V����
//=============================================================================
void CPauseUi::Update(void)
{
	// UI�̍X�V����
	CUi::Update();
}


//=============================================================================
// ���擾UI�̃R���X�g���N�^
//=============================================================================
CGetUi::CGetUi()
{
	// �l�̃N���A
	m_fTimer = 0.0f;// �o�ߎ���(�b)
	m_fAlpha = 1.0f;// �A���t�@�l
	m_bFading = false;// �t�F�[�h�J�n�t���O
}
//=============================================================================
// ���擾UI�̃f�X�g���N�^
//=============================================================================
CGetUi::~CGetUi()
{
	// �Ȃ�
}
//=============================================================================
// ���擾UI�̏���������
//=============================================================================
HRESULT CGetUi::Init(void)
{
	// UI�̏���������
	CUi::Init();

	return S_OK;
}
//=============================================================================
// ���擾UI�̍X�V����
//=============================================================================
void CGetUi::Update(void)
{
	// UI�̍X�V����
	CUi::Update();

	// ���Ԍo��
	m_fTimer++; // ���t���[���̌o�ߎ��Ԃ����Z

	// 3�b�o�߂Ńt�F�[�h�J�n
	if (m_fTimer >= 180.0f)
	{
		m_bFading = true;
	}

	// �t�F�[�h�����i���t���[�����������炷�j
	if (m_bFading)
	{
		m_fAlpha -= 0.008f; // 1�b�ŃA���t�@0�ɂȂ鑬��

		if (m_fAlpha < 0.0f)
		{
			m_fAlpha = 0.0f;
		}
	}

	SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha));
}


//=============================================================================
// ���U���gUI(���)�̃R���X�g���N�^
//=============================================================================
CResultUi::CResultUi()
{
	// �l�̃N���A
}
//=============================================================================
// ���U���gUI(���)�̃f�X�g���N�^
//=============================================================================
CResultUi::~CResultUi()
{
	// �Ȃ�
}
//=============================================================================
// ���U���gUI(���)�̏���������
//=============================================================================
HRESULT CResultUi::Init(void)
{
	// UI�̏���������
	CUi::Init();

	return S_OK;
}
//=============================================================================
// ���U���gUI(���)�̍X�V����
//=============================================================================
void CResultUi::Update(void)
{
	// UI�̍X�V����
	CUi::Update();
}


//=============================================================================
// ���U���gUI(�����N)�̃R���X�g���N�^
//=============================================================================
CResultRankUi::CResultRankUi()
{
	// �l�̃N���A
}
//=============================================================================
// ���U���gUI(�����N)�̃f�X�g���N�^
//=============================================================================
CResultRankUi::~CResultRankUi()
{
	// �Ȃ�
}
//=============================================================================
// ���U���gUI(�����N)�̏���������
//=============================================================================
HRESULT CResultRankUi::Init(void)
{
	// UI�̏���������
	CUi::Init();

	int min = CResult::GetClearMinutes();
	int sec = CResult::GetClearSeconds();
	float totalSec = min * 60.0f + sec;

	float uvLeft = 0.0f;

	if (totalSec < 300.0f)
	{// 5������
		uvLeft = 0.0f;   // S
	}
	else if (totalSec < 360.0f)
	{// 6������
		uvLeft = 0.25f;  // A
	}
	else if (totalSec < 420.0f)
	{// 7������
		uvLeft = 0.5f;   // B
	}
	else
	{// ����ȍ~
		uvLeft = 0.75f;  // C
	}

	CUi::SetUV(uvLeft, 0.0f, 0.25f, 1.0f);  // ��4�����A�c1

	return S_OK;
}
//=============================================================================
// ���U���gUI(�����N)�̍X�V����
//=============================================================================
void CResultRankUi::Update(void)
{
	// UI�̍X�V����
	CUi::Update();
}


//=============================================================================
// �X�e�[�W���\���̃R���X�g���N�^
//=============================================================================
CStageUi::CStageUi()
{
	// �l�̃N���A
	m_fTimer = 0.0f;// �o�ߎ���(�b)
	m_fAlpha = 0.0f;// �A���t�@�l
	m_state = STATE_FADEIN; // ���
}
//=============================================================================
// �X�e�[�W���\���̃f�X�g���N�^
//=============================================================================
CStageUi::~CStageUi()
{
	// �Ȃ�
}
//=============================================================================
// �X�e�[�W���\���̏���������
//=============================================================================
HRESULT CStageUi::Init(void)
{
	// UI�̏���������
	CUi::Init();

	return S_OK;
}
//=============================================================================
// �X�e�[�W���\���̍X�V����
//=============================================================================
void CStageUi::Update(void)
{
	// UI�̍X�V����
	CUi::Update();

	// ���Ԍo��
	m_fTimer++; // ���t���[���̌o�ߎ��Ԃ����Z

	switch (m_state)
	{
	case STATE_FADEIN:
		m_fAlpha += 0.005f; // 1�b�ŃA���t�@0�ɂȂ鑬��

		if (m_fAlpha >= 1.0f)
		{
			m_fAlpha = 1.0f;
			m_fTimer = 0.0f;
			m_state = STATE_SHOW;
		}
		break;
	case STATE_SHOW:
		// ��莞�ԕ\��
		if (m_fTimer >= 180.0f)
		{
			m_state = STATE_FADEOUT;
		}
		break;
	case STATE_FADEOUT:
		m_fAlpha -= 0.005f; // 1�b�ŃA���t�@0�ɂȂ鑬��

		if (m_fAlpha <= 0.0f)
		{
			m_fAlpha = 0.0f;
			m_state = STATE_END;
		}
	case STATE_END:
		// �������Ȃ�
		break;
	}

	SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha));
}