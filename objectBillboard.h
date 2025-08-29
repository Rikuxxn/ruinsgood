//=============================================================================
//
// �r���{�[�h���� [objectBillboard.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _OBJECTBILLBOARD_H_// ���̃}�N����`������Ă��Ȃ�������
#define _OBJECTBILLBOARD_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"


//*****************************************************************************
// �r���{�[�h�N���X
//*****************************************************************************
class CObjectBillboard : public CObject
{
public:
	CObjectBillboard(int nPriority = 5);
	~CObjectBillboard();

	// �r���{�[�h�̎��
	typedef enum
	{
		TYPE_NORMAL,
		TYPE_HINT,
		TYPE_MAX
	}TYPE;

	static CObjectBillboard* Create(TYPE type, const char* path, D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXCOLOR GetCol(void) { return m_col; }
	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetCol(D3DXCOLOR col) { m_col = col; }
	void SetSize(float fRadius) { m_fSize = fRadius; }
	void SetHeight(float fHeight) { m_fHeight = fHeight; }
	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;
	D3DXCOLOR m_col;
	D3DXMATRIX m_mtxWorld;
	float m_fSize;			// �T�C�Y(�G�t�F�N�g���a)
	float m_fHeight;// �T�C�Y(�r���{�[�h)
	int m_nIdxTexture;
	char m_szPath[MAX_PATH];			// �t�@�C���p�X
	TYPE m_type;
};

//*****************************************************************************
// �m�[�}���r���{�[�h�N���X
//*****************************************************************************
class CNormalBillboard : public CObjectBillboard
{
public:
	CNormalBillboard();
	~CNormalBillboard();

	// �r���{�[�h�̏��
	typedef enum
	{
		STATE_FADEIN = 0,
		STATE_NORMAL,
		STATE_FADEOUT,
		STATE_MAX
	}STATE;

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	float m_fAlpha;// �A���t�@�l
	STATE m_state;
};

//*****************************************************************************
// �q���g�r���{�[�h�N���X
//*****************************************************************************
class CHintBillboard : public CObjectBillboard
{
public:
	CHintBillboard();
	~CHintBillboard();

	// �r���{�[�h�̏��
	typedef enum
	{
		STATE_FADEIN = 0,
		STATE_NORMAL,
		STATE_FADEOUT,
		STATE_MAX
	}STATE;

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	float m_fAlpha;// �A���t�@�l
	STATE m_state;

};

#endif