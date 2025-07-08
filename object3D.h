//=============================================================================
//
// 3D�I�u�W�F�N�g���� [object3D.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _OBJECT3D_H_// ���̃}�N����`������Ă��Ȃ�������
#define _OBJECT3D_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"
#include "object.h"


//*****************************************************************************
// 3D�I�u�W�F�N�g�N���X
//*****************************************************************************
class CObject3D : public CObject
{
public:
	CObject3D(int nPriority = 6);
	~CObject3D();

	static CObject3D* Create(void);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);
	void SetPos(D3DXVECTOR3 pos);
	void SetAlpha(int nAlpha);
	void SetWidth(float fWidth);
	void SetHeight(float fHeight);
	void SetDepth(float fDepth);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;						// �ʒu
	D3DXVECTOR3 m_rot;						// ����
	D3DXMATRIX m_mtxWorld;					// ���[���h�}�g���b�N�X
	float m_fWidth;							// ��
	float m_fHeight;						// ����
	float m_fDepth;							// ���s��
	int m_nAlpha;							// �A���t�@�l
};

#endif
