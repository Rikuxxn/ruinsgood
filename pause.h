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
	int m_nIdxTexture;
};

#endif
