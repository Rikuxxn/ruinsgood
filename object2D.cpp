//=============================================================================
//
// 2D�I�u�W�F�N�g���� [object2D.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object2D.h"
#include "manager.h"
#include "renderer.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CObject2D::CObject2D(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	m_pVtxBuff	   = NULL;
	m_pos		   = INIT_VEC3;
	m_rot		   = INIT_VEC3;
	m_move		   = INIT_VEC3;
	m_col		   = D3DCOLOR_ARGB(255, 0, 0, 0);
	m_fLength	   = 0.0f;
	m_fAngle	   = 0.0f;
	m_fWidth	   = 0.0f;
	m_fHeight	   = 0.0f;
	m_nCounterAnim = 0;
	m_nPatternAnim = 0;
	m_fUSpeed	   = 0.0f;
	m_fVSpeed	   = 0.0f;
	m_aPosTexU     = 0.0f;
	m_fTexU		   = 0.0f;
	m_fTexV		   = 0.0f;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CObject2D::~CObject2D()
{
	// ���͂Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CObject2D* CObject2D::Create(void)
{
	CObject2D* pObject2D;

	// �I�u�W�F�N�g�̐���
	pObject2D = new CObject2D;

	// ����������
	pObject2D->Init();

	return pObject2D;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CObject2D::Init(void)
{
	// �f�o�C�X�̎擾
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

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

	//float UV1 = 1.0f / nTexUV1;
	//float UV2 = 1.0f / nTexUV2;

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
void CObject2D::Uninit(void)
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
void CObject2D::Update()
{
	VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// �Ίp���̒������Z�o����
	m_fLength = sqrtf(m_fWidth * m_fWidth + m_fHeight * m_fHeight) / 2.0f;

	// �Ίp���̊p�x���Z�o����
	m_fAngle = atan2f(m_fWidth, m_fHeight);

	// ���_���W�̐ݒ�
	pVtx[0].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI + m_fAngle)) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI + m_fAngle)) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + sinf(m_rot.z + (0.0f - m_fAngle)) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_rot.z + (0.0f - m_fAngle)) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + sinf(m_rot.z + (0.0f + m_fAngle)) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_rot.z + (0.0f + m_fAngle)) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	//���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();
}
//=============================================================================
// �`�揈��
//=============================================================================
void CObject2D::Draw(void)
{
	// �f�o�C�X�̎擾
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, NULL);
}
//=============================================================================
// �ʒu�̐ݒ�
//=============================================================================
void CObject2D::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}
//=============================================================================
// �����̐ݒ�
//=============================================================================
void CObject2D::SetRot(D3DXVECTOR3 rot)
{
	m_rot = rot;
}
//=============================================================================
// �F�̐ݒ�
//=============================================================================
void CObject2D::SetCol(D3DCOLOR color)
{
	m_col = color;
}
//=============================================================================
// �X�N���[���X�s�[�h�̐ݒ�
//=============================================================================
void CObject2D::SetScrollSpeed(float fUSpeed)
{
	m_fUSpeed = fUSpeed;
}
//=============================================================================
// �e�N�X�`��UV�̐ݒ�
//=============================================================================
void CObject2D::SetUV(int nTexU, int nTexV)
{
	m_fTexU = 1.0f / nTexU;
	m_fTexV = 1.0f / nTexV;

	VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(m_fTexU, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, m_fTexV);
	pVtx[3].tex = D3DXVECTOR2(m_fTexU, m_fTexV);

	//���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();
}
//=============================================================================
// �ʒu�̎擾
//=============================================================================
D3DXVECTOR3 CObject2D::GetPos(void)
{
	return m_pos;
}
//=============================================================================
// �ړ��ʂ̎擾
//=============================================================================
D3DXVECTOR3 CObject2D::GetMove(void)
{
	return m_move;
}
//=============================================================================
// �����̎擾
//=============================================================================
D3DXVECTOR3 CObject2D::GetRot(void)
{
	return m_rot;
}
//=============================================================================
// �F�̎擾
//=============================================================================
D3DCOLOR CObject2D::GetCol(void)
{
	return m_col;
}
//=============================================================================
// ���j�p�e�N�X�`���A�j���[�V��������
//=============================================================================
void CObject2D::TextureAnimExp(int nTexPosX, int nTexPosY,int nAnimSpeed)
{
	VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

	float UV1 = 1.0f / nTexPosX;
	float UV2 = 1.0f / nTexPosY;

	m_nCounterAnim++;// �J�E���^�[�����Z

	if (m_nCounterAnim > nAnimSpeed)
	{
		m_nCounterAnim = 0;
		m_nPatternAnim++;

		// 8�Ԗڂ̃e�N�X�`����������
		if (m_nPatternAnim >= nTexPosX)
		{
			m_nPatternAnim = 0;
			Uninit();
			return;
		}

		// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// �e�N�X�`�����W�̐ݒ�
		pVtx[0].tex = D3DXVECTOR2(m_nPatternAnim * UV1,0.0f);
		pVtx[1].tex = D3DXVECTOR2(m_nPatternAnim * UV1 + UV1,0.0f);
		pVtx[2].tex = D3DXVECTOR2(m_nPatternAnim * UV1, UV2);
		pVtx[3].tex = D3DXVECTOR2(m_nPatternAnim * UV1 + UV1, UV2);

		// ���_�o�b�t�@���A�����b�N����
		m_pVtxBuff->Unlock();
	}
}
//=============================================================================
// �ʏ�e�N�X�`���A�j���[�V��������
//=============================================================================
void CObject2D::TextureAnim(int nTexPosX, int nTexPosY, int nAnimSpeed)
{
	VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

	float UV1 = 1.0f / nTexPosX;
	float UV2 = 1.0f / nTexPosY;

	m_nCounterAnim++;// �J�E���^�[�����Z

	if ((m_nCounterAnim % nAnimSpeed) == 0)
	{
		m_nCounterAnim = 0;

		// nTexPosX���ڂ̎���1���ڂɖ߂�
		m_nPatternAnim = (m_nPatternAnim + 1) % nTexPosX;

		// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// �e�N�X�`�����W�̐ݒ�
		pVtx[0].tex = D3DXVECTOR2(m_nPatternAnim * UV1, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(m_nPatternAnim * UV1 + UV1, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(m_nPatternAnim * UV1, UV2);
		pVtx[3].tex = D3DXVECTOR2(m_nPatternAnim * UV1 + UV1, UV2);

		// ���_�o�b�t�@���A�����b�N����
		m_pVtxBuff->Unlock();
	}
}
//=============================================================================
// �e�N�X�`���X�N���[������
//=============================================================================
void CObject2D::ScrollTexture(void)
{
	VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

	 m_aPosTexU += m_fUSpeed;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//�e�N�X�`�����W�̍X�V
	pVtx[0].tex = D3DXVECTOR2(m_aPosTexU, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(m_aPosTexU + 1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(m_aPosTexU, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(m_aPosTexU + 1.0f, 1.0f);

	// ���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();
}
//=============================================================================
// �T�C�Y�ݒ�
//=============================================================================
void CObject2D::SetSize(float fWidth, float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;
}
