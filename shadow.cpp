//=============================================================================
//
// �e�̏��� [shadow.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "shadow.h"
#include "texture.h"
#include "manager.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
int CShadow::m_nIdxTexture = 0;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CShadow::CShadow()
{
	// �l�̃N���A
	m_pos	   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �ʒu
	m_mtxWorld = {};								// ���[���h�}�g���b�N�X
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CShadow::~CShadow()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CShadow* CShadow::Create(D3DXVECTOR3 pos, int nAlpha, float fWidth, float fHeight, float fDepth)
{
	CShadow* pShadow;

	pShadow = new CShadow;

	// ����������
	pShadow->Init();

	pShadow->m_pos = pos;
	pShadow->SetAlpha(nAlpha);
	pShadow->SetWidth(fWidth);
	pShadow->SetHeight(fHeight);
	pShadow->SetDepth(fDepth);

	return pShadow;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CShadow::Init(void)
{
	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register("data/TEXTURE/shadow000.jpg");

	// 3D�I�u�W�F�N�g�̏���������
	CObject3D::Init();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CShadow::Uninit(void)
{
	// 3D�I�u�W�F�N�g�̏I������
	CObject3D::Uninit();
}
//=============================================================================
// �X�V����
//=============================================================================
void CShadow::Update(void)
{
	// 3D�I�u�W�F�N�g�̍X�V����
	CObject3D::Update();

	// �ʒu�̐ݒ�
	SetPos(m_pos);
}
//=============================================================================
// �`�揈��
//=============================================================================
void CShadow::Draw(void)
{
	// �f�o�C�X�̎擾
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();

	// ���u�����f�B���O�����Z�����ɐݒ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// 3D�I�u�W�F�N�g�̕`�揈��
	CObject3D::Draw();

	// ���u�����f�B���O�����ɖ߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

}
