//=============================================================================
//
// �ǉ揈�� [hinttext.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "hinttext.h"
#include "manager.h"
#include "game.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CHintText::CHintText()
{
	// �l�̃N���A
	m_nIdxTexture = 0;
	memset(m_szPath, 0, sizeof(m_szPath));
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CHintText::~CHintText()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CHintText* CHintText::Create(TYPE type, const char* filepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot,
	float fWidth, float fHeight)
{
	// �I�u�W�F�N�g�̐���
	CHintText* pHintText = nullptr;

	switch (type)
	{
	case TYPE_TEXT:
		pHintText = new CText;
		pHintText->SetCol(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
		break;
	case TYPE_NORMAL:
		pHintText = new CHintText;
		pHintText->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		break;
	default:
		break;
	}

	pHintText->SetPath(filepath);
	pHintText->SetPos(pos);
	pHintText->SetRot(D3DXToRadian(rot));
	pHintText->SetWidth(fWidth);
	pHintText->SetHeight(fHeight);

	// ����������
	pHintText->Init();

	return pHintText;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CHintText::Init(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���̎擾
	m_nIdxTexture = CManager::GetTexture()->Register(m_szPath);

	// 3D�I�u�W�F�N�g�̏���������
	CObject3D::Init();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CHintText::Uninit(void)
{
	// 3D�I�u�W�F�N�g�̏I������
	CObject3D::Uninit();
}
//=============================================================================
// �X�V����
//=============================================================================
void CHintText::Update(void)
{
	// 3D�I�u�W�F�N�g�̍X�V����
	CObject3D::Update();
}
//=============================================================================
// �`�揈��
//=============================================================================
void CHintText::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, CManager::GetTexture()->GetAddress(m_nIdxTexture));

	// 3D�I�u�W�F�N�g�̕`�揈��
	CObject3D::Draw();
}


//=============================================================================
// �q���g�ǉ�(����)�̃R���X�g���N�^
//=============================================================================
CText::CText()
{
	// �l�̃N���A
	m_state = STATE_NORMAL;
	m_fGreen = 0.0f;
	m_fBlue = 0.0f;
}
//=============================================================================
// �q���g�ǉ�(����)�̃f�X�g���N�^
//=============================================================================
CText::~CText()
{
	// �Ȃ�
}
//=============================================================================
// �q���g�ǉ�(����)�̏���������
//=============================================================================
HRESULT CText::Init(void)
{
	// �q���g�ǉ�̏���������
	CHintText::Init();

	return S_OK;
}
//=============================================================================
// �q���g�ǉ�(����)�̍X�V����
//=============================================================================
void CText::Update(void)
{
	// �q���g�ǉ�̍X�V����
	CHintText::Update();

	if (!CGame::GetPlayer())
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 200.0f; // ��������

	switch (m_state)
	{
	case STATE_COLORINC:
		m_fGreen += 0.015f;
		m_fBlue += 0.015f;

		if (m_fGreen > 0.5f)
		{
			m_fGreen = 0.5f;
		}

		if (m_fBlue > 1.0f)
		{
			m_fBlue = 1.0f;
		}

		if (m_fGreen == 0.5f && m_fBlue == 1.0f)
		{
			m_state = STATE_NORMAL;
		}

		SetCol(D3DXCOLOR(0.0f, m_fGreen, m_fBlue, 1.0f));

		break;
	case STATE_NORMAL:
		if (distance < kTriggerDistance)
		{// ���X�ɐF��ς�����
			m_state = STATE_COLORINC;
		}
		else
		{// ���X�ɒʏ�(��)�ɖ߂����
			m_state = STATE_COLORDEC;
		}
		break;
	case STATE_COLORDEC:
		m_fGreen -= 0.01f;
		m_fBlue -= 0.01f;

		if (m_fGreen < 0.0f)
		{
			m_fGreen = 0.0f;
		}

		if (m_fBlue < 0.0f)
		{
			m_fBlue = 0.0f;
		}

		if (m_fGreen == 0.0f && m_fBlue == 0.0f)
		{
			m_state = STATE_NORMAL;
		}

		SetCol(D3DXCOLOR(0.0f, m_fGreen, m_fBlue, 1.0f));
		break;
	}
}
//=============================================================================
// �q���g�ǉ�(����)�̕`�揈��
//=============================================================================
void CText::Draw(void)
{
	// �q���g�ǉ�̕`�揈��
	CHintText::Draw();
}
