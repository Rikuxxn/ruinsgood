//=============================================================================
//
// �e�̏��� [shadow.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _SHADOW_H_// ���̃}�N����`������Ă��Ȃ�������
#define _SHADOW_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object3D.h"

//*****************************************************************************
// �e�N���X
//*****************************************************************************
class CShadow : public CObject3D
{
public:
	CShadow();
	~CShadow();

	static CShadow* Create(D3DXVECTOR3 pos, int nAlpha, float fWidth, float fHeight, float fDepth);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; }

private:
	static int m_nIdxTexture;
	D3DXVECTOR3 m_pos;					// �ʒu
	D3DXMATRIX m_mtxWorld;				// ���[���h�}�g���b�N�X

};

#endif