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
CHintText* CHintText::Create(const char* filepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot,
	float fWidth, float fHeight)
{
	// �I�u�W�F�N�g�̐���
	CHintText* pHintText = new CHintText;

	pHintText->SetPath(filepath);
	pHintText->SetPos(pos);
	pHintText->SetRot(D3DXToRadian(rot));
	pHintText->SetWidth(fWidth);
	pHintText->SetHeight(fHeight);
	pHintText->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

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