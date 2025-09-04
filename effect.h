//=============================================================================
//
// エフェクト処理 [effect.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _EFFECT_H_// このマクロ定義がされていなかったら
#define _EFFECT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "objectbillboard.h"


//*****************************************************************************
// エフェクトクラス
//*****************************************************************************
class CEffect : public CObjectBillboard
{
public:
	CEffect(int nPriority = 5);
	~CEffect();

	static CEffect* Create(const char* path,D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fRadius, int nLife, float fGravity, float fDecRad);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetMove(D3DXVECTOR3 move) { m_move = move; }
	void SetLife(int nLife) { m_nLife = nLife; }
	void SetRadius(float fRadius) { m_fRadius = fRadius; }
	void SetGravity(float fGrav) { m_fGravity = fGrav; }
	void SetDecRadius(float fDecRad) { m_fDecRadius = fDecRad; }
	float GetRadius(void) { return m_fRadius; }
	float GetGravity(void) { return m_fGravity; }

private:
	D3DXVECTOR3 m_move;		// 移動量
	float m_fRadius;		// 半径
	int m_nLife;			// 寿命
	int m_nIdxTexture;		// テクスチャインデックス
	float m_fGravity;		// 重力
	float m_fDecRadius;		// 半径の減衰量
};

#endif