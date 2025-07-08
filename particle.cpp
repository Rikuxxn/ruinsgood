//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "particle.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CParticle::CParticle(int nPriority) : CEffect(nPriority)
{
	// 値のクリア
	m_nType		   = 0;		// 種類
	m_nLife		   = 0;		// 寿命
	m_nMaxParticle = 0;		// 粒子の最大数
}
//=============================================================================
// デストラクタ
//=============================================================================
CParticle::~CParticle()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CParticle*CParticle::Create(D3DXVECTOR3 pos, D3DXCOLOR col, int nLife, int nType,int nMaxParticle)
{
	CParticle* pParticle;

	// エフェクトオブジェクトの生成
	pParticle = new CParticle;

	// 初期化処理
	pParticle->Init();

	pParticle->SetPos(pos);
	pParticle->SetCol(col);
	pParticle->SetLifeParticle(nLife);
	pParticle->SetParticle(nType);
	pParticle->m_nMaxParticle = nMaxParticle;

	return pParticle;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CParticle::Init(void)
{
	// エフェクトの初期化
	CEffect::Init();

	return S_OK;
}
//=============================================================================
// 更新処理
//=============================================================================
void CParticle::Update(void)
{
	CEffect* pEffect = NULL;

	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	D3DXVECTOR3 move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DCOLOR col = D3DCOLOR_ARGB(255,0,0,0);
	float fRadius = 0.0f;
	float fAngle = 0.0f;
	float fLength = 0.0f;

	// パーティクル生成
	for (int nCnt = 0; nCnt < m_nMaxParticle; nCnt++)//発生させたい粒子の数
	{
		if (m_nType == PARTICLE_BACKFIRE)
		{// バックファイア

			// 位置の設定
			pos = GetPos();

			// 移動量の設定
			fAngle = (float)(rand() % 55 - 180) / 100.0f;//角度

			fLength = (float)(rand() % 8) / 1.0f;//移動量

			move.x = sinf(fAngle) * fLength;
			move.y = cosf(fAngle) * fLength;

			// 色の設定
			col = GetCol();
			
			// 半径の設定
			fRadius = 9.0f;

		}
		else if (m_nType == PARTICLE_HORMING)
		{// ホーミング

			// 位置の設定
			pos = GetPos();

			// 移動量の設定
			fAngle = (float)(rand() % 360) * (D3DX_PI / 180.0f); // 0〜360度ランダム
			fLength = (float)(rand() % 3) / 5.0f;                // 0.0〜0.6くらいの小さい移動

			move.x = sinf(fAngle) * fLength;
			move.y = cosf(fAngle) * fLength;

			// 色の設定
			col = GetCol();

			// 半径の設定
			fRadius = 6.0f; 
		}
		else if (m_nType == PARTICLE_ENEMYBULLET)
		{// 敵の弾

			// 位置の設定
			pos = GetPos();

			// 移動量の設定
			fAngle = (float)(rand() % 360) * (D3DX_PI / 180.0f); // 0〜360度ランダム
			fLength = (float)(rand() % 3) / 5.0f;                // 0.0〜0.6くらいの小さい移動

			move.x = sinf(fAngle) * fLength;
			move.y = cosf(fAngle) * fLength;

			// 色の設定
			col = GetCol();

			// 半径の設定
			fRadius = 8.0f;
		}
		else if (m_nType == PARTICLE_PLAYER_EXP)
		{// プレイヤーが死んだときの爆発

			// 位置の設定
			pos = GetPos();

			// 移動量の設定
			fAngle = (float)(rand() % 360) * (D3DX_PI / 180.0f); // 0〜360度ランダム
			fLength = (float)(rand() % 5) / 1.0f;                // 0.0〜0.6くらいの小さい移動

			move.x = sinf(fAngle) * fLength;
			move.y = cosf(fAngle) * fLength;

			// 色の設定
			col = GetCol();

			// 半径の設定
			fRadius = 40.0f;
		}
		else if (m_nType == PARTICLE_ENEMY_EXP)
		{// 敵が死んだときの爆発

			// 位置の設定
			pos = GetPos();

			// 移動量の設定
			fAngle = (float)(rand() % 360) * (D3DX_PI / 180.0f); // 0〜360度ランダム
			fLength = (float)(rand() % 5) / 1.0f;                // 0.0〜0.6くらいの小さい移動

			move.x = sinf(fAngle) * fLength;
			move.y = cosf(fAngle) * fLength;

			// 色の設定
			col = GetCol();

			// 半径の設定
			fRadius = 40.0f;
		}
		else if (m_nType == PARTICLE_FLASH)
		{// ノズルフラッシュ
			// 位置の設定
			pos = GetPos();

			// 移動量の設定
			fAngle = (float)(rand() % 360) * (D3DX_PI / 180.0f); // 0〜360度ランダム
			fLength = (float)(rand() % 3) / 1.0f;//移動量

			move.x = sinf(fAngle) * fLength;
			move.y = cosf(fAngle) * fLength;

			// 色の設定
			col = GetCol();

			// 半径の設定
			fRadius = 12.0f;
		}

		// エフェクトの設定
		pEffect = CEffect::Create(pos, move, col, fRadius, m_nLife);
	}

	 // 寿命が尽きたら
	m_nLife--;

	if (m_nLife <= 0)
	{
		// 終了
		Uninit();
		return;
	}

	// エフェクトの更新
	CEffect::Update();
}
//=============================================================================
// パーティクル設定処理
//=============================================================================
void CParticle::SetParticle(int nType)
{
	m_nType = nType;
}
//=============================================================================
// パーティクルの寿命処理
//=============================================================================
void CParticle::SetLifeParticle(int nLife)
{
	m_nLife = nLife;
}
