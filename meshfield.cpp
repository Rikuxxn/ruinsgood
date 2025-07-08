//=============================================================================
//
// ���b�V���t�B�[���h���� [meshfield.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "meshfield.h"
#include "renderer.h"
#include "manager.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CMeshfield::CMeshfield()
{
	// �l�̃N���A
	m_nIdxTexture = 0;
	m_pVtxBuff = NULL;	// ���_�o�b�t�@�ւ̃|�C���^
	m_pIdxBuff = NULL;	// �C���f�b�N�X�o�b�t�@�ւ̃|�C���^
	m_pos = INIT_VEC3;	// �ʒu
	m_rot = INIT_VEC3;	// ����
	m_mtxWorld = {};	// ���[���h�}�g���b�N�X
	m_fWidth = 0.0f;	// ��
	m_fHeight = 0.0f;	// ����
	m_nPrimitive = 0;	// �v���~�e�B�u��
	m_nVertex = 0;		// ���_��
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CMeshfield::~CMeshfield()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CMeshfield* CMeshfield::Create(D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	CMeshfield* pMeshfield;

	pMeshfield = new CMeshfield;

	pMeshfield->m_pos = pos;
	pMeshfield->m_fWidth = fWidth;
	pMeshfield->m_fHeight = fHeight;

	// ����������
	pMeshfield->Init();

	return pMeshfield;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CMeshfield::Init(void)
{
	// �f�o�C�X�̎擾
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register("data/TEXTURE/ground.png");

	int meshX = 20;// X�����̃u���b�N��
	int meshZ = 20;// Z�����̃u���b�N��

	m_nPrimitive = (((meshX * meshZ) * 2)) + (4 * (meshZ - 1));	// �v���~�e�B�u��
	m_nVertex = ((meshX + 1) * (meshZ + 1));					// ���_��
	int maxIndex = (m_nPrimitive + 2);							// �C���f�b�N�X��

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nVertex,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// �C���f�b�N�X�o�b�t�@�̐���
	pDevice->CreateIndexBuffer(sizeof(WORD) * maxIndex,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	VERTEX_3D* pVtx = NULL;// ���_���ւ̃|�C���^

	int nCnt = 0;
	float tex = 10.0f / meshX;
	float tex2 = 10.0f / meshZ;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt1 = 0; nCnt1 <= meshZ; nCnt1++)
	{
		for (int nCnt2 = 0; nCnt2 <= meshX; nCnt2++)
		{

			pVtx[nCnt].pos = D3DXVECTOR3(0.0f + ((m_fWidth / meshX) * nCnt2) - (m_fWidth * 0.5f),
				0.0f,
				m_fHeight - ((m_fHeight / meshZ) * nCnt1) - (m_fHeight * 0.5f));

			// �e���_�̖@���̐ݒ�
			pVtx[nCnt].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// ���_�J���[�̐ݒ�
			pVtx[nCnt].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// �e�N�X�`�����W�̐ݒ�
			pVtx[nCnt].tex = D3DXVECTOR2(tex * nCnt2, tex2 * nCnt1);

			nCnt++;
		}
	}

	// ���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();

	WORD* pIdx;// �C���f�b�N�X���ւ̃|�C���^

	// �C���f�b�N�X�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	int nCntIdx3 = meshX + 1;
	int Num = 0;
	int nCntNum = 0;

	for (int nCntIdx = 0; nCntIdx < meshZ; nCntIdx++)
	{
		for (int nCntIdx2 = 0; nCntIdx2 <= meshX; nCntIdx2++, nCntIdx3++, Num++)
		{
			pIdx[nCntNum] = (WORD)nCntIdx3;

			pIdx[nCntNum + 1] = (WORD)Num;

			nCntNum += 2;
		}

		// �Ō�̍s���ǂ���
		// NOTE: �Ō�̍s�ɏk�ރ|���S��������
		if (nCntIdx != meshZ - 1)
		{
			pIdx[nCntNum] = (WORD)Num - 1;

			pIdx[nCntNum + 1] = (WORD)nCntIdx3;

			nCntNum += 2;
		}
	}

	// �C���f�b�N�X�o�b�t�@���A�����b�N����
	m_pIdxBuff->Unlock();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CMeshfield::Uninit(void)
{
	// �C���f�b�N�X�o�b�t�@�̉��
	if (m_pIdxBuff != NULL)
	{
		m_pIdxBuff->Release();
		m_pIdxBuff = NULL;
	}

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
void CMeshfield::Update(void)
{
	// �Ȃ�
}
//=============================================================================
// �`�揈��
//=============================================================================
void CMeshfield::Draw(void)
{
	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();

	// �f�o�C�X�̎擾
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	//// �����𔽉f
	//D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotMeshfield.y, g_rotMeshfield.x, g_rotMeshfield.z);
	//D3DXMatrixMultiply(&g_mtxWorldMeshfield, &g_mtxWorldMeshfield, &mtxRot);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// �C���f�b�N�X�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetIndices(m_pIdxBuff);

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// �|���S���̕`��
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nVertex, 0, m_nPrimitive);
}
//=============================================================================
// �ʒu�̎擾
//=============================================================================
D3DXVECTOR3 CMeshfield::GetPos(void)
{
	return m_pos;
}
