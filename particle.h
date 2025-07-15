//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _PARTICLE_H_// このマクロ定義がされていなかったら
#define _PARTICLE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"
#include "effect.h"

//*****************************************************************************
// パーティクルクラス
//*****************************************************************************
class CParticle : public CEffect
{
public:
	CParticle();
	~CParticle();

	// パーティクルの種類
	typedef enum
	{
		TYPE_FIRE = 0,
		TYPE_WATER,
		TYPE_AURA,
		TYPE_MAX
	}TYPE;

	static CParticle*Create(D3DXVECTOR3 pos, D3DXCOLOR col, int nLife, int nType,int nMaxParticle);
	HRESULT Init(void);
	virtual void Update(void);
	void SetParticle(int nType);
	void SetLife(int nLife);
private:
	int m_nType;		// 種類
	int m_nLife;		// 寿命
	int m_nMaxParticle;	// 粒子の数
	CEffect* m_pEffect;	// エフェクトへのポインタ
};

//*****************************************************************************
// 炎パーティクルクラス
//*****************************************************************************
class CFireParticle : public CParticle
{
public:
	CFireParticle();
	~CFireParticle();

	static CFireParticle* Create(D3DXVECTOR3 pos, D3DXCOLOR col, int nLife, int nType, int nMaxParticle);
	void Update(void) override;

private:
	D3DXVECTOR3 m_move;
	int m_nLife;
};


//*****************************************************************************
// 水しぶきパーティクルクラス
//*****************************************************************************

#endif