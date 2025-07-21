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
		TYPE_RESULT01,
		TYPE_RESULT02,
		TYPE_RESULT03,
		TYPE_MAX
	}TYPE;

	static CUi* Create(TYPE type, const char* path,D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void) { return m_pos; }

	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetCol(D3DXCOLOR col) { m_col = col; }
	void SetSize(float fWidth, float fHeight) { m_fWidth = fWidth; m_fHeight = fHeight; }
	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }
	void SetUV(float u, float v, float w, float h);  // UV�ݒ�

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;
	D3DXCOLOR m_col;
	float m_fWidth;					// ��
	float m_fHeight;				// ����
	int m_nIdxTexture;
	char m_szPath[MAX_PATH];			// �t�@�C���p�X
	TYPE m_type;
	float m_uvLeft;
	float m_uvTop;
	float m_uvWidth;
	float m_uvHeight;
	bool m_isUVDirty;
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

//*****************************************************************************
// ���U���gUI�N���X
//*****************************************************************************
class CResultUi01 : public CUi
{
public:
	CResultUi01();
	~CResultUi01();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

};

//*****************************************************************************
// ���U���gUI(�����������ǂ���)�N���X
//*****************************************************************************
class CResultUi02 : public CUi
{
public:
	CResultUi02();
	~CResultUi02();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

};

//*****************************************************************************
// ���U���gUI(�����N)�N���X
//*****************************************************************************
class CResultUi03 : public CUi
{
public:
	CResultUi03();
	~CResultUi03();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

};


#endif
