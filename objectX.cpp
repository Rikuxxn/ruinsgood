//=============================================================================
//
// X�t�@�C������ [objectX.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "objectX.h"
#include "renderer.h"
#include "manager.h"

using namespace std;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CObjectX::CObjectX()
{
	// �l�̃N���A
	m_nIdxTexture = 0;
	m_pos = INIT_VEC3;						// �ʒu
	m_rot = INIT_VEC3;						// ����
	m_move = INIT_VEC3;						// �ړ���
	m_size = D3DXVECTOR3(1.0f, 1.0f, 1.0f);	// �T�C�Y
	m_pMesh = NULL;							// ���b�V���ւ̃|�C���^
	m_pBuffMat = NULL;						// �}�e���A���ւ̃|�C���^
	m_dwNumMat = NULL;						// �}�e���A����
	m_mtxWorld = {};						// ���[���h�}�g���b�N�X
	m_modelSize = INIT_VEC3;				// ���f���̌��T�C�Y�i�S�̂̕��E�����E���s���j
	for (int nCnt = 0; nCnt < MAX_PATH; nCnt++)
	{
		m_szPath[nCnt] = NULL;					// �t�@�C���p�X
	}
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CObjectX::~CObjectX()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CObjectX* CObjectX::Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size)
{
	CObjectX* pObjectX;

	pObjectX = new CObjectX;

	pObjectX->m_pos = pos;
	pObjectX->m_rot = D3DXToRadian(rot);
	pObjectX->m_size = size;
	pObjectX->SetPath(pFilepath);	// �p�X�ۑ�

	// ����������
	pObjectX->Init();

	return pObjectX;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CObjectX::Init(void)
{
	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();

	// �f�o�C�X�̎擾
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// X�t�@�C���̓ǂݍ���
	D3DXLoadMeshFromX(m_szPath,
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

	// ���b�V�����ǂݍ��܂�Ă��邩�m�F
	if (m_pMesh == NULL)
	{
		MessageBox(NULL, "X�t�@�C���̓ǂݍ��݂Ɏ��s���܂����i���b�V����NULL�ł��j", "�G���[", MB_OK | MB_ICONERROR);
		return 0;
	}

	// ���_���̎擾
	nNumVtx = m_pMesh->GetNumVertices();

	// ���_����0�Ȃ�A���f������Ƃ݂Ȃ�
	if (nNumVtx == 0)
	{
		MessageBox(NULL, "X�t�@�C���̓ǂݍ��݂Ɏ��s���܂����i���_����0�ł��j", "�G���[", MB_OK | MB_ICONERROR);
		return 0;
	}
	else
	{// ���f������

		// ���_�t�H�[�}�b�g�̎擾
		sizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

		// ���_�o�b�t�@�̃��b�N
		m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		// AABB�v�Z�p�̍ŏ��E�ő�l������
		D3DXVECTOR3 vMin(FLT_MAX, FLT_MAX, FLT_MAX);
		D3DXVECTOR3 vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		// �ő�l�E�ŏ��l�����߂�
		for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
		{
			D3DXVECTOR3* p = (D3DXVECTOR3*)(pVtxBuff + sizeFVF * nCnt);

			vMin.x = min(vMin.x, p->x);
			vMin.y = min(vMin.y, p->y);
			vMin.z = min(vMin.z, p->z);

			vMax.x = max(vMax.x, p->x);
			vMax.y = max(vMax.y, p->y);
			vMax.z = max(vMax.z, p->z);
		}

		// ���_�o�b�t�@�̃A�����b�N
		m_pMesh->UnlockVertexBuffer();

		// �T�C�Y = �ő� - �ŏ�
		m_modelSize = vMax - vMin;

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
	}

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CObjectX::Uninit(void)
{
	// �e�N�X�`���C���f�b�N�X�̔j��
	if (m_nIdxTexture != NULL)
	{
		delete[] m_nIdxTexture;
		m_nIdxTexture = NULL;
	}

	//���b�V���̔j��
	if (m_pMesh != NULL)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	//�}�e���A���̔j��
	if (m_pBuffMat != NULL)
	{
		m_pBuffMat->Release();
		m_pBuffMat = NULL;
	}

	// �I�u�W�F�N�g�̔j��(�������g)
	this->Release();
}
//=============================================================================
// �X�V����
//=============================================================================
void CObjectX::Update(void)
{
	//�ʒu���X�V
	m_pos.x += m_move.x;
	m_pos.y += m_move.y;
	m_pos.z += m_move.z;
}	
//=============================================================================
// �`�揈��
//=============================================================================
void CObjectX::Draw(void)
{
	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();

	// �f�o�C�X�̎擾
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	//pDevice->SetRenderState(D3DRS_LIGHTING, TRUE); // ���C�e�B���O�L��
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);// �@�����K����L���ɂ���

	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans, mtxSize;

	D3DMATERIAL9 matDef;	// ���݂̃}�e���A���ۑ��p

	D3DXMATERIAL* pMat;		// �}�e���A���f�[�^�ւ̃|�C���^

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �T�C�Y�𔽉f
	D3DXMatrixScaling(&mtxSize, m_size.x, m_size.y, m_size.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxSize);

	// �����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ���݂̃}�e���A���̎擾
	pDevice->GetMaterial(&matDef);

	// �}�e���A���f�[�^�ւ̃|�C���^���擾
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	// �F�̎擾
	D3DXCOLOR col = GetCol();

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
#ifdef _DEBUG

		// ���̃}�e���A���F�ɕ␳���|����
		D3DMATERIAL9 mat = pMat[nCntMat].MatD3D;

		mat.Diffuse.r *= col.r;
		mat.Diffuse.g *= col.g;
		mat.Diffuse.b *= col.b;
		mat.Diffuse.a *= col.a;

		// �}�e���A���̐ݒ�
		pDevice->SetMaterial(&mat);

#else

		// �}�e���A���̐ݒ�
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

#endif

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

	//pDevice->SetRenderState(D3DRS_LIGHTING, FALSE); // ���C�e�B���O����
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);// �@�����K���𖳌��ɂ���

	// �ۑ����Ă����}�e���A����߂�
	pDevice->SetMaterial(&matDef);

}
//=============================================================================
// �t�@�C���p�X�̎擾
//=============================================================================
const char* CObjectX::GetPath(void)
{
	return m_szPath;
}
//=============================================================================
// �ʒu�̎擾
//=============================================================================
D3DXVECTOR3 CObjectX::GetPos(void)
{
	return m_pos;
}
//=============================================================================
// �����̎擾
//=============================================================================
D3DXVECTOR3 CObjectX::GetRot(void)
{
	return m_rot;
}
//=============================================================================
// �T�C�Y�̎擾
//=============================================================================
D3DXVECTOR3 CObjectX::GetSize(void)
{
	return m_size;
}
//=============================================================================
// ���f���̌��T�C�Y�̎擾
//=============================================================================
D3DXVECTOR3 CObjectX::GetModelSize(void)
{
	return m_modelSize;
}
//=============================================================================
// X�t�@�C���p�X�̐ݒ菈��
//=============================================================================
void CObjectX::SetPath(const char* path)
{
	strcpy_s(m_szPath, MAX_PATH, path);
}
//=============================================================================
// �T�C�Y�̐ݒ菈��
//=============================================================================
void CObjectX::SetSize(D3DXVECTOR3 size)
{
	m_size = size;
}
//=============================================================================
// �ʒu�̐ݒ菈��
//=============================================================================
void CObjectX::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}
//=============================================================================
// �����̐ݒ菈��
//=============================================================================
void CObjectX::SetRot(D3DXVECTOR3 rot)
{
	m_rot = rot;
}
//=============================================================================
// �F�̎擾
//=============================================================================
D3DXCOLOR CObjectX::GetCol(void) const
{
	return D3DXCOLOR();
}
//=============================================================================
// �}�e���A���̎擾
//=============================================================================
D3DXCOLOR CObjectX::GetMaterialColor(void) const
{
	if (m_pBuffMat && m_dwNumMat > 0)
	{
		D3DXMATERIAL* pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();
		return pMat[0].MatD3D.Diffuse;  // 0�Ԗڂ̃}�e���A���̊g�U���F��Ԃ�
	}

	return D3DXCOLOR(1, 1, 1, 1); // �f�t�H���g��
}