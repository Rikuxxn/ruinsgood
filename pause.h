//=============================================================================
//
// �|�[�Y���� [pause.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _PAUSE_H_// ���̃}�N����`������Ă��Ȃ�������
#define _PAUSE_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"
#include "object.h"


//*****************************************************************************
// �|�[�Y�N���X
//*****************************************************************************
class CPause : public CObject
{
public:
	CPause(int nPriority = 7);
	~CPause();

	static CPause* Create(D3DXVECTOR3 pos,float fWidth,float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;						// �ʒu
	D3DCOLOR m_col;							// �F
	float m_fWidth, m_fHeight;				// �T�C�Y
	int m_fAlpha;							// ���݂̃A���t�@�l
	float m_fTime;							// ���Ԍo�ߗp
	int m_nIdxTexture;
};

#endif
