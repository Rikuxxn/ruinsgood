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
	static constexpr int MAX_PAUSE = 4;
	static constexpr float PAUSE_MIN_SCALE = 1.0f;
	static constexpr float PAUSE_MAX_SCALE = 1.2f;
	static constexpr float PAUSE_SCALE_SPEED = 0.02f;
	static constexpr float PAUSE_ALPHA_SPEED = 0.05f;

	// 選択項目の種類
	typedef enum
	{
		MENU_CONTINUE = 0,		// ゲームに戻る
		MENU_RETRY,				// ゲームをやり直す
		MENU_QUIT,				// タイトル画面に戻る
		MENU_MAX
	}MENU;

	CPause(int nPriority = 7);
	~CPause();

	static CPause* Create(D3DXVECTOR3 pos,float fWidth,float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DCOLOR m_col;							// 色
	float m_fWidth, m_fHeight;				// サイズ
	int m_fAlpha;							// 現在のアルファ値
	float m_fTime;							// 時間経過用
	int m_nIdxTexture;						// テクスチャインデックス
	bool m_bPauseSelect;					// 選ばれているか

};

#endif
