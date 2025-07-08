//=============================================================================
//
// �e�N�X�`������ [texture.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "texture.h"
#include "renderer.h"
#include "manager.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
int CTexture::m_nNumAll = 0;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CTexture::CTexture()
{
	// �l�̃N���A
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		m_apTexture[nCnt] = {};
	}
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CTexture::~CTexture()
{
	// �Ȃ�
}
//=============================================================================
// �e�N�X�`���̓ǂݍ���
//=============================================================================
HRESULT CTexture::Load(void)
{
	// �f�o�C�X�̎擾
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �S�Ẵe�N�X�`���̓ǂݍ���
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		D3DXCreateTextureFromFile(pDevice,
			TEXTURE[nCnt],
			&m_apTexture[nCnt]);
	}

	return S_OK;
}
//=============================================================================
// �e�N�X�`���̔j��
//=============================================================================
void CTexture::Unload(void)
{
	// �S�Ẵe�N�X�`���̔j��
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		if (m_apTexture[nCnt] != NULL)
		{
			m_apTexture[nCnt]->Release();
			m_apTexture[nCnt] = NULL;
		}
	}
}
//=============================================================================
// �e�N�X�`���̎w��
//=============================================================================
int CTexture::Register(const char* pFilename)
{
	int nIdx = -1;

	nIdx++;
	
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		if (TEXTURE[nCnt] != NULL)
		{
			if (strcmp(pFilename, TEXTURE[nCnt]) == 0)
			{
				nIdx = nCnt;
				break;
			}
		}
	}

	return nIdx;
}
//=============================================================================
// X�t�@�C���p�e�N�X�`���̎w��
//=============================================================================
int CTexture::RegisterX(const char* pFilename)
{
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		if (TEXTURE[nCnt] != NULL)
		{
			if (strcmp(pFilename, TEXTURE[nCnt]) == 0)
			{
				return nCnt;
			}
		}
	}

	return -1; // ������Ȃ�����
}
//=============================================================================
// �e�N�X�`���̃A�h���X�擾
//=============================================================================
LPDIRECT3DTEXTURE9 CTexture::GetAddress(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_TEXTURE)
	{// �͈͊O
		return NULL;
	}

	return m_apTexture[nIdx];
}