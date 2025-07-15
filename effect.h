//=============================================================================
//
// �G�t�F�N�g���� [effect.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _EFFECT_H_// ���̃}�N����`������Ă��Ȃ�������
#define _EFFECT_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "objectbillboard.h"


//*****************************************************************************
// �G�t�F�N�g�N���X
//*****************************************************************************
class CEffect : public CObjectBillboard
{
public:
	CEffect(int nPriority = 5);
	~CEffect();

	static CEffect* Create(const char* path, D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DCOLOR col, float fRadius, int nLife);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetMove(D3DXVECTOR3 move);
	void SetLife(int nLife);
	void SetRadius(float fRadius);
	float GetRadius(void) { return m_fRadius; }
private:
	D3DXVECTOR3 m_move;						// �ړ���
	float m_fRadius;						// ���a
	int m_nLife;							// ����
	int m_nIdxTexture;						// �e�N�X�`���C���f�b�N�X
	char m_szPath[MAX_PATH];			// �t�@�C���p�X

};
#endif