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
// マクロ定義
//*****************************************************************************
#define MAX_PARTICLE (20)	// 発生させたい粒子の数


//*****************************************************************************
// パーティクルクラス
//*****************************************************************************
class CParticle : public CEffect
{
public:
	CParticle(int nPriority = 5);
	~CParticle();

	// パーティクルの種類
	typedef enum
	{
		PARTICLE_BACKFIRE = 0,
		PARTICLE_HORMING,
		PARTICLE_PLAYERBULLET,
		PARTICLE_ENEMYBULLET,
		PARTICLE_PLAYER_EXP,
		PARTICLE_ENEMY_EXP,
		PARTICLE_FLASH,
		PARTICLE_MAX
	}PARTICLETYPE;

	static CParticle*Create(D3DXVECTOR3 pos, D3DXCOLOR col, int nLife, int nType,int nMaxParticle);
	HRESULT Init(void);
	void Update(void);
	void SetParticle(int nType);
	void SetLifeParticle(int nLife);
private:
	int m_nType;		// 種類
	int m_nLife;		// 寿命
	int m_nMaxParticle;	// 粒子の数
};
#endif