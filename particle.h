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
		TYPE_AURA2,
		TYPE_MAX
	}TYPE;

	static CParticle*Create(D3DXVECTOR3 pos, D3DXCOLOR col, int nLife, int nType,int nMaxParticle);
	HRESULT Init(void);
	void Update(void);

	void SetParticle(int nType) { m_nType = nType; }
	void SetLife(int nLife) { m_nLife = nLife; }

	int GetLife(void) { return m_nLife; }
	int GetMaxParticle(void) { return m_nMaxParticle; }

private:
	int m_nType;		// 種類
	int m_nLife;		// 寿命
	int m_nMaxParticle;	// 粒子の数
};

//*****************************************************************************
// 炎パーティクルクラス
//*****************************************************************************
class CFireParticle : public CParticle
{
public:
	CFireParticle();
	~CFireParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// 水しぶきパーティクルクラス
//*****************************************************************************
class CWaterParticle : public CParticle
{
public:
	CWaterParticle();
	~CWaterParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// オーラパーティクルクラス
//*****************************************************************************
class CAuraParticle : public CParticle
{
public:
	CAuraParticle();
	~CAuraParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// オーラ(仮面用)パーティクルクラス
//*****************************************************************************
class CAura2Particle : public CParticle
{
public:
	CAura2Particle();
	~CAura2Particle();

	HRESULT Init(void);
	void Update(void);

private:

};

#endif