//=============================================================================
//
// リザルト処理 [result.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RESULT_H_// このマクロ定義がされていなかったら
#define _RESULT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "scene.h"
#include "time.h"
#include "ui.h"

//*****************************************************************************
// リザルトクラス
//*****************************************************************************
class CResult : public CScene
{
public:
	CResult();
	~CResult();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static void SetClearTime(int min, int sec) { m_nClearMinutes = min; m_nClearSeconds = sec; }
	static void SetGet(bool flag) { m_isMaskGet = flag; }
	static int GetClearMinutes(void) { return m_nClearMinutes; }
	static int GetClearSeconds(void) { return m_nClearSeconds; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	D3DXVECTOR3 m_pos;						// 2Dポリゴン描画用位置
	D3DCOLOR m_col;							// 2Dポリゴン描画用色
	float m_fWidth, m_fHeight;				// 2Dポリゴン描画用サイズ
	static CTime* m_pTime;					// タイムへのポインタ
	static CColon* m_pColon;				// コロンへのポインタ
	static int m_nClearMinutes;
	static int m_nClearSeconds;
	static bool m_isMaskGet;
	static CUi* m_pUi;							// UIへのポインタ
	int m_nIdxTexture;
};

#endif
