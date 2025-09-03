//=============================================================================
//
// 壁画処理 [hinttext.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _HINTTEXT_H_
#define _HINTTEXT_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "object3D.h"

//*****************************************************************************
// ヒント壁画クラス
//*****************************************************************************
class CHintText : public CObject3D
{
public:
	CHintText();
	~CHintText();

	// 種類
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
	char m_szPath[MAX_PATH];			// ファイルパス
	TYPE m_type;
};

//*****************************************************************************
// ヒント壁画(文字)クラス
//*****************************************************************************
class CText : public CHintText
{
public:
	CText();
	~CText();

	// 状態
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

