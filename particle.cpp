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
CParticle::CParticle()
{
	// 値のクリア
	m_nType		   = 0;		// 種類
	m_nLife		   = 0;		// 寿命
	m_nMaxParticle = 0;		// 粒子の最大数
	m_pEffect = NULL;
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
	pParticle->SetLife(nLife);
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
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	D3DXVECTOR3 move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DCOLOR col = D3DCOLOR_ARGB(255,0,0,0);
	float fRadius = 0.0f;
	float fAngle = 0.0f;
	float fLength = 0.0f;

	// パーティクル生成
	for (int nCnt = 0; nCnt < m_nMaxParticle; nCnt++)//発生させたい粒子の数
	{
		if (m_nType == TYPE_FIRE)
		{// 炎
			// 位置の設定
			pos = GetPos();

			// 移動量の設定
			fAngle = ((rand() % 360) / 180.0f) * D3DX_PI;//角度

			fLength = (rand() % 5) / 20.0f + 0.1f;//移動量

			move.x = cosf(fAngle) * fLength;
			move.z = sinf(fAngle) * fLength;
			move.y = 0.5f + (rand() % 300) / 100.0f;

			// 色の設定
			col = GetCol();
			
			// 半径の設定
			fRadius = 10.0f;
		}
		else if (m_nType == TYPE_WATER)
		{// 水しぶき
			// 位置
			pos = GetPos();

			// ランダムな角度で横に広がる
			float angle = ((rand() % 360) / 180.0f) * D3DX_PI;
			float speed = (rand() % 150) / 30.0f + 0.2f;

			move.x = cosf(angle) * speed;
			move.z = sinf(angle) * speed;
			move.y = (rand() % 10) / 50.0f + 0.05f; // 少しだけ上方向

			// 色
			col = GetCol();

			// 半径
			fRadius = 15.0f + (rand() % 15);
		}

		// エフェクトの設定
		m_pEffect = CEffect::Create(pos, move, col, fRadius, m_nLife);
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
void CParticle::SetLife(int nLife)
{
	m_nLife = nLife;
}