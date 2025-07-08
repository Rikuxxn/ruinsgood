//=============================================================================
//
// �G�t�F�N�g���� [effect.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "effect.h"
#include "renderer.h"
#include "manager.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CEffect::CEffect(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_move = INIT_VEC3;	// �ʒu
	m_fRadius = 0.0f;	// ���a
	m_nLife = 0;		// ����
	m_nIdxTexture = 0;	// �e�N�X�`���C���f�b�N�X
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CEffect::~CEffect()
{

	// �Ȃ�

}
//=============================================================================
// ��������
//=============================================================================
CEffect* CEffect::Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DCOLOR col, float fRadius, int nLife)
{
	CEffect* pEffect;

	// �G�t�F�N�g�I�u�W�F�N�g�̐���
	pEffect = new CEffect;

	// ����������
	pEffect->Init();

	pEffect->SetPos(pos);
	pEffect->SetMove(move);
	pEffect->SetCol(col);
	pEffect->SetRadius(fRadius);
	pEffect->SetLife(nLife);

	return pEffect;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CEffect::Init(void)
{
	// 2D�I�u�W�F�N�g�̏���������
	CObject2D::Init();

	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register("data/TEXTURE/effect000.jpg");

	// �e�N�X�`��UV�̐ݒ�
	CObject2D::SetUV(1, 1);

	// �T�C�Y�̐ݒ�
	CObject2D::SetSize(50.0f, 50.0f);

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CEffect::Uninit(void)
{
	// 2D�I�u�W�F�N�g�̏I������
	CObject2D::Uninit();
}
//=============================================================================
// �X�V����
//=============================================================================
void CEffect::Update(void)
{
	// �ʒu�̎擾
	D3DXVECTOR3 Pos = GetPos();

	// �ʒu���X�V
	Pos.x += m_move.x;
	Pos.y += m_move.y;

	// �ʒu�̐ݒ�
	CObject2D::SetPos(Pos);


	m_fRadius--;

	if (m_fRadius <= 0.0f)
	{
		m_fRadius = 0.0f;
	}

	// ���a�ɉ����ăT�C�Y��ύX
	CObject2D::SetSize(m_fRadius * 2.0f, m_fRadius * 2.0f); // ���a�ɂ���

	// 2D�I�u�W�F�N�g�̍X�V����
	CObject2D::Update();

	m_nLife--;

	if (m_nLife <= 0)
	{
		// �G�t�F�N�g�̏I��
		Uninit();
		return;
	}

}
//=============================================================================
// �`�揈��
//=============================================================================
void CEffect::Draw(void)
{
	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();

	// �f�o�C�X�̎擾
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	//���u�����f�B���O�����Z�����ɐݒ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// 2D�I�u�W�F�N�g�̕`�揈��
	CObject2D::Draw();

	//���u�����f�B���O�����ɖ߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
//=============================================================================
// �ړ��ʂ̐ݒ�
//=============================================================================
void CEffect::SetMove(D3DXVECTOR3 move)
{
	m_move = move;
}
//=============================================================================
// �����̐ݒ�
//=============================================================================
void CEffect::SetLife(int nLife)
{
	m_nLife = nLife;
}
//=============================================================================
// ���a�̐ݒ�
//=============================================================================
void CEffect::SetRadius(float fRadius)
{
	m_fRadius = fRadius;
}

