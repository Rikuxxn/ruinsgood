//=============================================================================
//
// ビルボード処理 [objectBillboard.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _OBJECTBILLBOARD_H_// このマクロ定義がされていなかったら
#define _OBJECTBILLBOARD_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "object.h"


//*****************************************************************************
// ビルボードクラス
//*****************************************************************************
class CObjectBillboard : public CObject
{
public:
	CObjectBillboard(int nPriority = 5);
	~CObjectBillboard();

	// ビルボードの種類
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
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	D3DXVECTOR3 m_pos;
	D3DXCOLOR m_col;
	D3DXMATRIX m_mtxWorld;
	float m_fSize;			// サイズ(エフェクト半径)
	float m_fHeight;// サイズ(ビルボード)
	int m_nIdxTexture;
	char m_szPath[MAX_PATH];			// ファイルパス
	TYPE m_type;
};

//*****************************************************************************
// ノーマルビルボードクラス
//*****************************************************************************
class CNormalBillboard : public CObjectBillboard
{
public:
	CNormalBillboard();
	~CNormalBillboard();

	// ビルボードの状態
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
	float m_fAlpha;// アルファ値
	STATE m_state;
};

//*****************************************************************************
// ヒントビルボードクラス
//*****************************************************************************
class CHintBillboard : public CObjectBillboard
{
public:
	CHintBillboard();
	~CHintBillboard();

	// ビルボードの状態
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
	float m_fAlpha;// アルファ値
	STATE m_state;

};

#endif