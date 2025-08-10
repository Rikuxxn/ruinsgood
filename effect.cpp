//=============================================================================
//
// �G�t�F�N�g���� [effect.cpp]
// Author : RIKU TANEKAWA
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
CEffect::CEffect(int nPriority) : CObjectBillboard(nPriority)
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
CEffect* CEffect::Create(const char* path,D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fRadius, int nLife)
{
	CEffect* pEffect;

	// �G�t�F�N�g�I�u�W�F�N�g�̐���
	pEffect = new CEffect;

	pEffect->SetPath(path);

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
	// �r���{�[�h�I�u�W�F�N�g�̏���������
	CObjectBillboard::Init();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CEffect::Uninit(void)
{
	// �r���{�[�h�I�u�W�F�N�g�̏I������
	CObjectBillboard::Uninit();
}
//=============================================================================
// �X�V����
//=============================================================================
void CEffect::Update(void)
{
	// �r���{�[�h�I�u�W�F�N�g�̍X�V����
	CObjectBillboard::Update();

	// �ʒu�̎擾
	D3DXVECTOR3 Pos = GetPos();

	// �ʒu���X�V
	Pos.x += m_move.x;
	Pos.y += m_move.y;
	Pos.z += m_move.z;

	// �ʒu�̐ݒ�
	SetPos(Pos);
	SetSize(m_fRadius);

	m_fRadius--;

	if (m_fRadius <= 0.0f)
	{
		m_fRadius = 0.0f;
	}

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

	// ���u�����f�B���O�����Z�����ɐݒ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// ���e�X�g��L��
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);//�f�t�H���g��false
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);//0���傫��������`��

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// �r���{�[�h�I�u�W�F�N�g�̕`�揈��
	CObjectBillboard::Draw();

	// ���e�X�g�𖳌��ɖ߂�
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);//�f�t�H���g��false

	// ���u�����f�B���O�����ɖ߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
