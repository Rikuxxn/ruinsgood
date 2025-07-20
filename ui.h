//=============================================================================
//
// UI���� [ui.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _UI_H_// ���̃}�N����`������Ă��Ȃ�������
#define _UI_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"

//*****************************************************************************
// UI�N���X
//*****************************************************************************
class CUi : public CObject
{
public:
	CUi(int nPriority = 7);
	~CUi();

	// UI�̎��
	typedef enum
	{
		TYPE_PAUSE = 0,
		TYPE_MASK,
	}TYPE;

	static CUi* Create(TYPE type, D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void) { return m_pos; }

	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetCol(D3DXCOLOR col) { m_col = col; }
	void SetSize(float fWidth, float fHeight) { m_fWidth = fWidth; m_fHeight = fHeight; }
	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;
	D3DXCOLOR m_col;
	float m_fWidth;					// ��
	float m_fHeight;				// ����
	int m_nIdxTexture;
	char m_szPath[MAX_PATH];			// �t�@�C���p�X
	TYPE m_type;
};

//*****************************************************************************
// �|�[�YUI�N���X
//*****************************************************************************
class CPauseUi : public CUi
{
public:
	CPauseUi();
	~CPauseUi();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

};

//*****************************************************************************
// ���ʎ擾UI�N���X
//*****************************************************************************
class CMaskUi : public CUi
{
public:
	CMaskUi();
	~CMaskUi();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	float m_fTimer;// �o�ߎ���(�b)
	float m_fAlpha;// �A���t�@�l
	bool m_bFading;// �t�F�[�h�J�n�t���O
};


#endif
