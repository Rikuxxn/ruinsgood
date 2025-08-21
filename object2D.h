//=============================================================================
//
// 2Dオブジェクト処理 [object2D.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _OBJECT2D_H_// このマクロ定義がされていなかったら
#define _OBJECT2D_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"
#include "object.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define OBJECT_WIDTH (55)
#define OBJECT_HEIGHT (35)


//*****************************************************************************
// 2Dオブジェクトクラス
//*****************************************************************************
class CObject2D : public CObject
{
public:
	CObject2D(int nPriority = 6);
	~CObject2D();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static CObject2D* Create(D3DXVECTOR3 pos, float fWidth, float fHeight);
	void SetPos(D3DXVECTOR3 pos);
	void SetRot(D3DXVECTOR3 rot);
	void SetCol(D3DXCOLOR color);
	void SetSize(float fWidth,float fHeight);
	void SetScrollSpeed(float fUSpeed);
	void SetUV(int nTexU, int nTexV);
	D3DXVECTOR3 GetPos(void);
	D3DXVECTOR3 GetMove(void);
	D3DXVECTOR3 GetRot(void);
	D3DCOLOR GetCol(void);
	void TextureAnimExp(int nTexPosX,int nTexPosY, int nAnimSpeed);
	void TextureAnim(int nTexPosX, int nTexPosY, int nAnimSpeed);
	void ScrollTexture(void);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXVECTOR3 m_move;						// 移動量
	D3DCOLOR m_col;							// 色
	float m_fTexU;							// テクスチャUV
	float m_fTexV;							// テクスチャUV
	int m_nCounterAnim;						// アニメーションカウンター
	int m_nPatternAnim;						// アニメーションパターンNo.
	float m_fWidth;							// 幅
	float m_fHeight;						// 高さ
	float m_fLength;						// 長さ
	float m_fAngle;							// 角度
	float m_fUSpeed;						// 横のスクロールスピード
	float m_fVSpeed;						// 縦のスクロールスピード
	float m_aPosTexU;						// 横のスクロール用変数
};

#endif