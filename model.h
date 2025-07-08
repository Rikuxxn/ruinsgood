//=============================================================================
//
// ���f������ [model.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _MODEL_H_// ���̃}�N����`������Ă��Ȃ�������
#define _MODEL_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"

//*****************************************************************************
// ���f���N���X
//*****************************************************************************
class CModel
{
public:
	CModel();
	~CModel();

	static CModel* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetParent(CModel* pModel);
	D3DXMATRIX GetMtxWorld(void);
	D3DXVECTOR3 GetPos(void);
	D3DXVECTOR3 GetRot(void);
	void SetPos(D3DXVECTOR3 pos);
	void SetRot(D3DXVECTOR3 rot);

	D3DXVECTOR3 GetOffsetPos(void) const { return m_OffsetPos; }
	D3DXVECTOR3 GetOffsetRot(void) const { return m_OffsetRot; }

	void SetOffsetPos(const D3DXVECTOR3& pos) { m_OffsetPos = pos; }
	void SetOffsetRot(const D3DXVECTOR3& rot) { m_OffsetRot = rot; }

private:
	int* m_nIdxTexture;
	D3DXVECTOR3 m_pos;					// �ʒu
	D3DXVECTOR3 m_rot;					// ����
	D3DXVECTOR3 m_move;					// �ړ���
	LPD3DXMESH m_pMesh;					// ���b�V���ւ̃|�C���^
	LPD3DXBUFFER m_pBuffMat;			// �}�e���A���ւ̃|�C���^
	DWORD m_dwNumMat;					// �}�e���A����
	D3DXMATRIX m_mtxWorld;				// ���[���h�}�g���b�N�X
	CModel* m_pParent;					// �e���f���ւ̃|�C���^
	char m_Path[MAX_PATH];
	D3DXVECTOR3 m_OffsetPos;
	D3DXVECTOR3 m_OffsetRot;
};

#endif
