//=============================================================================
//
// ���f������ [model.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "model.h"
#include "texture.h"
#include "renderer.h"
#include "manager.h"
#include <cstdio>

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CModel::CModel()
{
	// �l�̃N���A
	m_nIdxTexture = NULL;
	m_pos = INIT_VEC3;								// �ʒu
	m_rot = INIT_VEC3;								// ����
	m_move = INIT_VEC3;								// �ړ���
	m_pMesh = NULL;									// ���b�V���ւ̃|�C���^
	m_pBuffMat = NULL;								// �}�e���A���ւ̃|�C���^
	m_dwNumMat = NULL;								// �}�e���A����
	m_mtxWorld = {};								// ���[���h�}�g���b�N�X
	m_pParent = NULL;								// �e���f���ւ̃|�C���^
	for (int nCnt = 0; nCnt < MAX_PATH; nCnt++)
	{
		m_Path[nCnt] = NULL;						// �t�@�C���p�X�i�[�p
	}
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CModel::~CModel()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CModel* CModel::Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CModel* pModel;

	pModel = new CModel;

	pModel->m_pos = pos;
	pModel->m_rot = D3DXToRadian(rot);
	strcpy_s(pModel->m_Path, pFilepath); // �p�X�ۑ�

	// ����������
	pModel->Init();

	return pModel;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CModel::Init(void)
{
	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();

	// �f�o�C�X�̎擾
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// X�t�@�C���̓ǂݍ���
	D3DXLoadMeshFromX(m_Path,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&m_pBuffMat,
		NULL,
		&m_dwNumMat,
		&m_pMesh);

	int nNumVtx;		// ���_��
	DWORD sizeFVF;		// ���_�t�H�[�}�b�g�̃T�C�Y
	BYTE* pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^

	// ���_���̎擾
	nNumVtx = m_pMesh->GetNumVertices();

	// ���_�t�H�[�}�b�g�̎擾
	sizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

	// ���_�o�b�t�@�̃��b�N
	m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);


	// �ő�l�Ƃ��͂�������Ȃ�


	// ���_�o�b�t�@�̃A�����b�N
	m_pMesh->UnlockVertexBuffer();

	D3DXMATERIAL* pMat;// �}�e���A���ւ̃|�C���^

	// �}�e���A���f�[�^�ւ̃|�C���^���擾
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	m_nIdxTexture = new int[m_dwNumMat];

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		if (pMat[nCntMat].pTextureFilename != NULL)
		{// �e�N�X�`���t�@�C�������݂���
			// �e�N�X�`���̓o�^
			m_nIdxTexture[nCntMat] = pTexture->RegisterX(pMat[nCntMat].pTextureFilename);
		}
		else
		{// �e�N�X�`�������݂��Ȃ�
			m_nIdxTexture[nCntMat] = -1;
		}
	}

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CModel::Uninit(void)
{
	// �e�N�X�`���C���f�b�N�X�̔j��
	if (m_nIdxTexture != NULL)
	{
		delete[] m_nIdxTexture;
		m_nIdxTexture = NULL;
	}

	// ���b�V���̔j��
	if (m_pMesh != NULL)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	// �}�e���A���̔j��
	if (m_pBuffMat != NULL)
	{
		m_pBuffMat->Release();
		m_pBuffMat = NULL;
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void CModel::Update(void)
{
	// �ʒu���X�V
	m_pos.x += m_move.x;
	m_pos.z += m_move.z;
	m_pos.y += m_move.y;
}
//=============================================================================
// �`�揈��
//=============================================================================
void CModel::Draw(void)
{
	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();

	// �f�o�C�X�̎擾
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans;

	D3DMATERIAL9 matDef;	// ���݂̃}�e���A���ۑ��p

	D3DXMATERIAL* pMat;		// �}�e���A���f�[�^�ւ̃|�C���^

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y + m_OffsetRot.y, m_rot.x + m_OffsetRot.x, m_rot.z + m_OffsetRot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x + m_OffsetPos.x, m_pos.y + m_OffsetPos.y, m_pos.z + m_OffsetPos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	D3DXMATRIX mtxParent;

	if (m_pParent != NULL)
	{// �e�����݂���
		// �e���f���̃}�g���b�N�X���擾
		mtxParent = m_pParent->GetMtxWorld();
	}
	else
	{// �e�����݂��Ȃ�
		// ����(�ŐV)�̃}�g���b�N�X���擾����
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
	}

	// �e�̃}�g���b�N�X�Ɗ|�����킹��
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);

	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ���݂̃}�e���A���̎擾
	pDevice->GetMaterial(&matDef);

	// �}�e���A���f�[�^�ւ̃|�C���^���擾
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		// �}�e���A���̐ݒ�
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		if (m_nIdxTexture[nCntMat] == -1)
		{
			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, NULL);
		}
		else
		{
			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture[nCntMat]));
		}

		// ���f��(�p�[�c)�̕`��
		m_pMesh->DrawSubset(nCntMat);
	}

	// �ۑ����Ă����}�e���A����߂�
	pDevice->SetMaterial(&matDef);
}
//=============================================================================
// �e�̐ݒ菈��
//=============================================================================
void CModel::SetParent(CModel* pModel)
{
	m_pParent = pModel;
}
//=============================================================================
// ���[���h�}�g���b�N�X�̎擾
//=============================================================================
D3DXMATRIX CModel::GetMtxWorld(void)
{
	return m_mtxWorld;
}
//=============================================================================
// �ʒu�̎擾
//=============================================================================
D3DXVECTOR3 CModel::GetPos(void)
{
	return m_pos;
}
//=============================================================================
// �����̎擾
//=============================================================================
D3DXVECTOR3 CModel::GetRot(void)
{
	return m_rot;
}
//=============================================================================
// �ʒu�̐ݒ�
//=============================================================================
void CModel::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}
//=============================================================================
// �����̐ݒ�
//=============================================================================
void CModel::SetRot(D3DXVECTOR3 rot)
{
	m_rot = D3DXToRadian(rot);
}