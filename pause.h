//=============================================================================
//
// ポーズ処理 [pause.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _PAUSE_H_// このマクロ定義がされていなかったら
#define _PAUSE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"
#include "object.h"


//*****************************************************************************
// ポーズクラス
//*****************************************************************************
class CPause : public CObject
{
public:
	CPause(int nPriority = 7);
	~CPause();

	// 選択項目の種類
	typedef enum
	{
		MENU_CONTINUE = 0,		// ゲームに戻る
		MENU_RETRY,				// ゲームをやり直す
		MENU_QUIT,				// タイトル画面に戻る
		MENU_MAX
	}MENU;

	static CPause* Create(MENU type, D3DXVECTOR3 pos,float fWidth,float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	D3DXVECTOR3 GetPos(void);
	bool IsMouseOver(void);
	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }
	void SetCol(D3DXCOLOR col) { m_col = col; }
	virtual void Execute(void) {}

	// 選択状態設定・取得
	void SetSelected(bool selected) { m_isSelected = selected; }
	bool IsSelected(void) const { return m_isSelected; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffBack; // 背景用頂点バッファへのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DCOLOR m_col;							// 色
	float m_fWidth, m_fHeight;				// サイズ
	int m_nIdxTexture;						// テクスチャインデックス
	char m_szPath[MAX_PATH];				// ファイルパス
	bool m_isSelected;
};

//*****************************************************************************
// コンティニュー項目クラス
//*****************************************************************************
class CContinue : public CPause
{
public:
	CContinue();
	~CContinue();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Execute(void) override;

private:

};

//*****************************************************************************
// リトライ項目クラス
//*****************************************************************************
class CRetry : public CPause
{
public:
	CRetry();
	~CRetry();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Execute(void) override;

private:

};

//*****************************************************************************
// 終了項目クラス
//*****************************************************************************
class CQuit : public CPause
{
public:
	CQuit();
	~CQuit();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Execute(void) override;

private:

};

#endif
