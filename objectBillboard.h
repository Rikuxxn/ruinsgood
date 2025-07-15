//=============================================================================
//
// �r���{�[�h���� [objectBillboard.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _OBJECTBILLBOARD_H_// ���̃}�N����`������Ă��Ȃ�������
#define _OBJECTBILLBOARD_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"
#include "object.h"


//*****************************************************************************
// �r���{�[�h�N���X
//*****************************************************************************
class CObjectBillboard : public CObject
{
public:
	CObjectBillboard(int nPriority = 5);
	~CObjectBillboard();

	//�r���{�[�h�̎��
	typedef enum
	{
		TYPE_ONE = 0,
		TYPE_LIGHT,
		TYPE_MAX
	}TYPE;

	static CObjectBillboard* Create(TYPE type, D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXCOLOR GetCol(void) { return m_col; }
	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetCol(D3DXCOLOR col) { m_col = col; }
	void SetSize(float fRadius) { m_fSize = fRadius; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;
	D3DXCOLOR m_col;
	TYPE m_nType;
	D3DXMATRIX m_mtxWorld;
	float m_fSize;			// �T�C�Y
	int m_nIdxTexture;

};
#endif