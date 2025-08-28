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
#include "game.h"


//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
std::unordered_map<CUi::TYPE, UiCreateFunc> CUi::m_UiFactoryMap = {};

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CUi::CUi(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
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

	m_UiFactoryMap[CUi::TYPE_PAUSE]				= []() -> CUi* { return new CPauseUi(); };
	m_UiFactoryMap[CUi::TYPE_GET]				= []() -> CUi* { return new CGetUi(); };
	m_UiFactoryMap[CUi::TYPE_RESULT_UI]			= []() -> CUi* { return new CResultUi(); };
	m_UiFactoryMap[CUi::TYPE_RESULT_TREASURE]	= []() -> CUi* { return new CResultTreasureUi(); };
	m_UiFactoryMap[CUi::TYPE_RESULT_RANK]		= []() -> CUi* { return new CResultRankUi(); };
	m_UiFactoryMap[CUi::TYPE_RESULT_GET]		= []() -> CUi* { return new CResultGetUi(); };
	m_UiFactoryMap[CUi::TYPE_STAGE_NAME]		= []() -> CUi* { return new CStageUi(); };
	m_UiFactoryMap[CUi::TYPE_INTERACT]			= []() -> CUi* { return new CInteractUi(); };
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

	// 2D�I�u�W�F�N�g�̏���������
	CObject2D::Init();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CUi::Uninit(void)
{
	// �T�E���h�̒�~
	CManager::GetSound()->Stop();

	// 2D�I�u�W�F�N�g�̏I������
	CObject2D::Uninit();
}
//=============================================================================
// �X�V����
//=============================================================================
void CUi::Update(void)
{
	// 2D�I�u�W�F�N�g�̍X�V����
	CObject2D::Update();

	if (m_isUVDirty)
	{
		// �e�N�X�`��UV�ړ�����
		CObject2D::MoveTexUV(m_uvLeft, m_uvTop, m_uvWidth, m_uvHeight);

		m_isUVDirty = false;
	}

}
//=============================================================================
// �`�揈��
//=============================================================================
void CUi::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, CManager::GetTexture()->GetAddress(m_nIdxTexture));

	// 2D�I�u�W�F�N�g�̕`�揈��
	CObject2D::Draw();
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
	m_isGet = false;// �T�E���h�p�t���O
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

	if (!m_isGet)
	{
		// ����T�E���h�̍Đ�
		CManager::GetSound()->Play(CSound::SOUND_LABEL_GET);

		m_isGet = true;
	}

	// �F�̐ݒ�
	SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha));
}


//=============================================================================
// ���U���gUI�̃R���X�g���N�^
//=============================================================================
CResultUi::CResultUi()
{
	// �l�̃N���A
}
//=============================================================================
// ���U���gUI�̃f�X�g���N�^
//=============================================================================
CResultUi::~CResultUi()
{
	// �Ȃ�
}
//=============================================================================
// ���U���gUI�̏���������
//=============================================================================
HRESULT CResultUi::Init(void)
{
	// UI�̏���������
	CUi::Init();

	return S_OK;
}
//=============================================================================
// ���U���gUI�̍X�V����
//=============================================================================
void CResultUi::Update(void)
{
	// UI�̍X�V����
	CUi::Update();
}


//=============================================================================
// ���U���gUI(���)�̃R���X�g���N�^
//=============================================================================
CResultTreasureUi::CResultTreasureUi()
{
	// �l�̃N���A
	m_showDeleyTime = 0;	// �\���܂ł̒x������
	m_fAlpha = 0.0f;			// �A���t�@�l
	m_isShow = false;
	m_prevShow = false;
}
//=============================================================================
// ���U���gUI(���)�̃f�X�g���N�^
//=============================================================================
CResultTreasureUi::~CResultTreasureUi()
{
	// �Ȃ�
}
//=============================================================================
// ���U���gUI(���)�̏���������
//=============================================================================
HRESULT CResultTreasureUi::Init(void)
{
	// UI�̏���������
	CUi::Init();

	// �\���x�����Ԃ̐ݒ�(�b)
	SetDeleyTime(2);

	return S_OK;
}
//=============================================================================
// ���U���gUI(���)�̍X�V����
//=============================================================================
void CResultTreasureUi::Update(void)
{
	m_showDeleyTime--;

	if (m_showDeleyTime <= 0)
	{
		m_isShow = true;
		m_fAlpha += 0.01f;

		if (m_fAlpha > 1.0f)
		{
			m_fAlpha = 1.0f;
		}

		// �ʏ�ɖ߂�
		SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha));

		bool n = m_isShow;

		if (n && !m_prevShow)
		{// ��񂾂��ʂ�
			// �����N�\���T�E���h�̍Đ�
			CManager::GetSound()->Play(CSound::SOUND_LABEL_RANK);
		}

		m_prevShow = n;
	}
	else
	{
		// �����ɂ���
		SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	}

	// UI�̍X�V����
	CUi::Update();
}


//=============================================================================
// ���U���gUI(�����N)�̃R���X�g���N�^
//=============================================================================
CResultRankUi::CResultRankUi()
{
	// �l�̃N���A
	m_showDeleyTime = 0;
	m_fAlpha = 0.0f;
	m_isShow = false;
	m_prevShow = false;
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

	// �\���x�����Ԃ̐ݒ�(�b)
	SetDeleyTime(5);

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

	// UV���W�̐ݒ�
	CUi::SetUV(uvLeft, 0.0f, 0.25f, 1.0f);  // ��4�����A�c1

	return S_OK;
}
//=============================================================================
// ���U���gUI(�����N)�̍X�V����
//=============================================================================
void CResultRankUi::Update(void)
{
	m_showDeleyTime--;

	if (m_showDeleyTime <= 0)
	{
		m_isShow = true;
		m_fAlpha += 0.01f;

		if (m_fAlpha > 1.0f)
		{
			m_fAlpha = 1.0f;
		}

		// �ʏ�ɖ߂�
		SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha));

		bool n = m_isShow;

		if (n && !m_prevShow)
		{// ��񂾂��ʂ�
			// �����N�\���T�E���h�̍Đ�
			CManager::GetSound()->Play(CSound::SOUND_LABEL_RANK);
		}

		m_prevShow = n;
	}
	else
	{
		// �����ɂ���
		SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	}

	// UI�̍X�V����
	CUi::Update();
}


//=============================================================================
// ���U���gUI(���肵����)�̃R���X�g���N�^
//=============================================================================
CResultGetUi::CResultGetUi()
{
	// �l�̃N���A
	m_showDeleyTime = 0;	// �\���܂ł̒x������
	m_isShow = false;
	m_prevShow = false;
}
//=============================================================================
// ���U���gUI(���肵����)�̃f�X�g���N�^
//=============================================================================
CResultGetUi::~CResultGetUi()
{
	// �Ȃ�
}
//=============================================================================
// ���U���gUI(���肵����)�̏���������
//=============================================================================
HRESULT CResultGetUi::Init(void)
{
	// UI�̏���������
	CUi::Init();

	// �\���x�����Ԃ̐ݒ�(�b)
	SetDeleyTime(3);

	return S_OK;
}
//=============================================================================
// ���U���gUI(���肵����)�̍X�V����
//=============================================================================
void CResultGetUi::Update(void)
{
	m_showDeleyTime--;

	if (m_showDeleyTime <= 0)
	{
		m_isShow = true;

		// �ʏ�ɖ߂�
		SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		bool n = m_isShow;

		if (n && !m_prevShow)
		{// ��񂾂��ʂ�
			// �����N�\���T�E���h�̍Đ�
			CManager::GetSound()->Play(CSound::SOUND_LABEL_RANK);
		}

		m_prevShow = n;
	}
	else
	{
		// �����ɂ���
		SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	}

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


//=============================================================================
// �C���^���N�gUI�̃R���X�g���N�^
//=============================================================================
CInteractUi::CInteractUi()
{
	// �l�̃N���A
	m_isInteract = false;
}
//=============================================================================
// �C���^���N�gUI�̃f�X�g���N�^
//=============================================================================
CInteractUi::~CInteractUi()
{
	// �Ȃ�
}
//=============================================================================
// �C���^���N�gUI�̏���������
//=============================================================================
HRESULT CInteractUi::Init(void)
{
	// UI�̏���������
	CUi::Init();

	return S_OK;
}
//=============================================================================
// �C���^���N�gUI�̍X�V����
//=============================================================================
void CInteractUi::Update(void)
{
	// UI�̍X�V����
	CUi::Update();

	CInputMouse* pMouse = CManager::GetInputMouse();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 HintTextPos = CGame::GetHintText()->GetPos();

	D3DXVECTOR3 disPos = playerPos - HintTextPos;

	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 180.0f; // ��������

	if (distance < kTriggerDistance)
	{
		if (CManager::GetInputMouse()->GetTrigger(0) || CManager::GetInputJoypad()->GetTriggerR2())
		{
			m_isInteract = true;
		}

		// �ʏ�ɖ߂�
		SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else
	{
		// �����ɂ���
		SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	}
}
