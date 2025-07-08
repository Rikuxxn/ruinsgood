//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "score.h"
#include "number.h"
#include "renderer.h"
#include "manager.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
int CScore::m_nScore = 0;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CScore::CScore(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		m_apNumber[nCnt] = {};				
	}
	m_nScore	  = 0;						// �X�R�A
	m_digitWidth  = 0.0f;					// ����1��������̕�
	m_digitHeight = 0.0f;					// ����1��������̍���
	m_basePos     = INIT_VEC3;				// �\���̊J�n�ʒu
	m_nIdxTexture = 0;						// �e�N�X�`���C���f�b�N�X
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CScore::~CScore()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CScore* CScore::Create(float baseX,float baseY,float digitWidth,float digitHeight)
{
	CScore* pScore;

	pScore = new CScore;

	pScore->m_basePos = D3DXVECTOR3(baseX, baseY, 0.0f);
	pScore->m_digitWidth = digitWidth;
	pScore->m_digitHeight = digitHeight;

	// ����������
	pScore->Init();
	
	return pScore;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CScore::Init(void)
{
	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		float posX = m_basePos.x + nCnt * m_digitWidth;
		float posY = m_basePos.y;

		m_apNumber[nCnt] = CNumber::Create(posX, posY, m_digitWidth, m_digitHeight);

		if (!m_apNumber[nCnt])
		{
			return E_FAIL;
		}
	}

	// �e�N�X�`�����蓖��
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register("data/TEXTURE/num001.png");

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CScore::Uninit(void)
{
	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		if (m_apNumber[nCnt] != NULL)
		{
			// �i���o�[�̏I������
			m_apNumber[nCnt]->Uninit();

			delete m_apNumber[nCnt];
			m_apNumber[nCnt] = NULL;
		}
	}

	CObject::Release();
}
//=============================================================================
// �X�V����
//=============================================================================
void CScore::Update(void)
{
	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		if (m_apNumber[nCnt])
		{
			// �i���o�[�̍X�V����
			m_apNumber[nCnt]->Update();
		}
	}
}
//=============================================================================
// �`�揈��
//=============================================================================
void CScore::Draw(void)
{
	int score = m_nScore;

	// �X�R�A���e���ɕ����i�E����j
	for (int nCnt = MAX_DIGITS - 1; nCnt >= 0; nCnt--)
	{
		int digit = score % 10;
		score /= 10;

		if (m_apNumber[nCnt])
		{
			// ���ݒ菈��
			m_apNumber[nCnt]->SetDigit(digit);

			// �e�N�X�`���̎擾
			CTexture* pTexture = CManager::GetTexture();

			// �f�o�C�X�̎擾
			CRenderer* renderer = CManager::GetRenderer();
			LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

			m_apNumber[nCnt]->Draw();
		}
	}
}
//=============================================================================
// �ʒu�̎擾
//=============================================================================
D3DXVECTOR3 CScore::GetPos(void)
{
	return D3DXVECTOR3();
}
//=============================================================================
// �X�R�A�ݒ菈��
//=============================================================================
void CScore::SetScore(int nScore)
{
	m_nScore = nScore;
}
//=============================================================================
// �X�R�A���Z����
//=============================================================================
void CScore::AddScore(int nValue)
{
	m_nScore += nValue;

	if (m_nScore > 99999999)
	{
		// 8������
		m_nScore = 99999999;
	}
}
//=============================================================================
// ����������
//=============================================================================
int CScore::DigitNum(int nScore)
{
	if (nScore == 0)
	{
		return 1;
	}

	int nCnt = 0;
	while (nScore > 0)
	{
		nScore /= 10;
		nCnt++;
	}

	return nCnt;
}