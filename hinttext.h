//=============================================================================
//
// �ǉ揈�� [hinttext.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _HINTTEXT_H_
#define _HINTTEXT_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object3D.h"

//*****************************************************************************
// �q���g�ǉ�N���X
//*****************************************************************************
class CHintText : public CObject3D
{
public:
	CHintText();
	~CHintText();

	// ���
	typedef enum
	{
		TYPE_TEXT = 0,
		TYPE_NORMAL,
		TYPE_MAX
	}TYPE;

	static CHintText* Create(TYPE type, const char* filepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }
	void SetType(TYPE type) { m_type = type; }
	TYPE GetType(void) { return m_type; }

private:
	int m_nIdxTexture;
	char m_szPath[MAX_PATH];			// �t�@�C���p�X
	TYPE m_type;
};

//*****************************************************************************
// �q���g�ǉ�(����)�N���X
//*****************************************************************************
class CText : public CHintText
{
public:
	CText();
	~CText();

	// ���
	typedef enum
	{
		STATE_COLORINC = 0,
		STATE_NORMAL,
		STATE_COLORDEC,
		STATE_MAX
	}STATE;

	HRESULT Init(void);
	void Update(void);
	void Draw(void);

private:
	STATE m_state;
	float m_fGreen;
	float m_fBlue;

};
#endif

