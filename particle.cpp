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

	switch (nType)
	{
	case TYPE_FIRE:
		// 炎パーティクルオブジェクトの生成
		pParticle = new CFireParticle;
		break;
	case TYPE_WATER:
		// 水しぶきパーティクルオブジェクトの生成
		pParticle = new CWaterParticle;
		break;
	case TYPE_AURA:
		// オーラパーティクルオブジェクトの生成
		pParticle = new CAuraParticle;
		break;
	case TYPE_AURA2:
		// オーラ(仮面用)パーティクルオブジェクトの生成
		pParticle = new CAura2Particle;
		break;
	default:
		// パーティクルオブジェクトの生成
		pParticle = new CParticle;
		break;
	}

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
// 炎パーティクルのコンストラクタ
//=============================================================================
CFireParticle::CFireParticle()
{
	
}
//=============================================================================
// 炎パーティクルのデストラクタ
//=============================================================================
CFireParticle::~CFireParticle()
{
	// なし
}
//=============================================================================
// 炎パーティクルの初期化処理
//=============================================================================
HRESULT CFireParticle::Init(void)
{
	SetPath("data/TEXTURE/smoke.jpg");

	// パーティクルの初期化処理
	CParticle::Init();

	return S_OK;
}
//=============================================================================
// 炎パーティクルの更新処理
//=============================================================================
void CFireParticle::Update(void)
{
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DCOLOR col = D3DCOLOR_ARGB(255, 0, 0, 0);
	float fRadius = 0.0f;
	float fAngle = 0.0f;
	float fLength = 0.0f;
	const char* texPath = NULL;
	int nMaxParticle = GetMaxParticle();
	int nLife = GetLife();

	// パーティクル生成
	for (int nCnt = 0; nCnt < nMaxParticle; nCnt++)//発生させたい粒子の数
	{
		// テクスチャの指定
		texPath = "data/TEXTURE/smoke.jpg";

		// 位置の設定
		pos = GetPos();

		// 移動量の設定
		fAngle = ((rand() % 360) / 180.0f) * D3DX_PI;//角度

		fLength = (rand() % 10) / 30.0f + 0.2f;//移動量

		move.x = cosf(fAngle) * fLength;
		move.z = sinf(fAngle) * fLength;
		move.y = 0.5f + (rand() % 300) / 100.0f;

		// 色の設定
		col = GetCol();

		// 半径の設定
		fRadius = 10.0f;

		// エフェクトの設定
		CEffect::Create(texPath, pos, move, col, fRadius, nLife);
	}

	// パーティクルの更新処理
	CParticle::Update();
}


//=============================================================================
// 水しぶきパーティクルのコンストラクタ
//=============================================================================
CWaterParticle::CWaterParticle()
{

}
//=============================================================================
// 水しぶきパーティクルのデストラクタ
//=============================================================================
CWaterParticle::~CWaterParticle()
{
	// なし
}
//=============================================================================
// 水しぶきパーティクルの初期化処理
//=============================================================================
HRESULT CWaterParticle::Init(void)
{
	SetPath("data/TEXTURE/smoke.jpg");

	// パーティクルの初期化処理
	CParticle::Init();

	return S_OK;
}
//=============================================================================
// 水しぶきパーティクルの更新処理
//=============================================================================
void CWaterParticle::Update(void)
{
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	D3DXVECTOR3 move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DCOLOR col = D3DCOLOR_ARGB(255,0,0,0);
	float fRadius = 0.0f;
	float fAngle = 0.0f;
	float fLength = 0.0f;
	const char* texPath = NULL;
	int nMaxParticle = GetMaxParticle();
	int nLife = GetLife();

	// パーティクル生成
	for (int nCnt = 0; nCnt < nMaxParticle; nCnt++)//発生させたい粒子の数
	{
		// テクスチャの指定
		texPath = "data/TEXTURE/smoke.jpg";

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

		// エフェクトの設定
		CEffect::Create(texPath,pos, move, col, fRadius, nLife);
	}

	// パーティクルの更新処理
	CParticle::Update();
}


//=============================================================================
// オーラパーティクルのコンストラクタ
//=============================================================================
CAuraParticle::CAuraParticle()
{

}
//=============================================================================
// オーラパーティクルのデストラクタ
//=============================================================================
CAuraParticle::~CAuraParticle()
{
	// なし
}
//=============================================================================
// オーラパーティクルの初期化処理
//=============================================================================
HRESULT CAuraParticle::Init(void)
{
	SetPath("data/TEXTURE/treasure_effect.png");

	// パーティクルの初期化処理
	CParticle::Init();

	return S_OK;
}
//=============================================================================
// オーラパーティクルの更新処理
//=============================================================================
void CAuraParticle::Update(void)
{
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	D3DXVECTOR3 move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DCOLOR col = D3DCOLOR_ARGB(255,0,0,0);
	float fRadius = 0.0f;
	float fAngle = 0.0f;
	float fLength = 0.0f;
	const char* texPath = NULL;
	int nMaxParticle = GetMaxParticle();
	int nLife = GetLife();

	// パーティクル生成
	for (int nCnt = 0; nCnt < nMaxParticle; nCnt++)//発生させたい粒子の数
	{
		// テクスチャの指定
		texPath = "data/TEXTURE/treasure_effect.png";

		// 位置
		pos = GetPos();

		// ランダムな角度で横に広がる
		float angle = ((rand() % 360) / 180.0f) * D3DX_PI;
		float speed = (rand() % 150) / 300.0f + 0.2f;

		move.x = cosf(angle) * speed;
		move.z = sinf(angle) * speed;
		move.y = (rand() % 300) / 100.0f + 0.9f; // 上方向

		// 色
		col = GetCol();

		// 半径
		fRadius = 35.0f + (rand() % 40);

		// エフェクトの設定
		CEffect::Create(texPath,pos, move, col, fRadius, nLife);
	}

	// パーティクルの更新処理
	CParticle::Update();
}


//=============================================================================
// オーラ(仮面用)パーティクルのコンストラクタ
//=============================================================================
CAura2Particle::CAura2Particle()
{

}
//=============================================================================
// オーラ(仮面用)パーティクルのデストラクタ
//=============================================================================
CAura2Particle::~CAura2Particle()
{
	// なし
}
//=============================================================================
// オーラ(仮面用)パーティクルの初期化処理
//=============================================================================
HRESULT CAura2Particle::Init(void)
{
	SetPath("data/TEXTURE/effect000.jpg");

	// パーティクルの初期化処理
	CParticle::Init();

	return S_OK;
}
//=============================================================================
// オーラ(仮面用)パーティクルの更新処理
//=============================================================================
void CAura2Particle::Update(void)
{
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DCOLOR col = D3DCOLOR_ARGB(255, 0, 0, 0);
	float fRadius = 0.0f;
	float fAngle = 0.0f;
	float fLength = 0.0f;
	const char* texPath = NULL;
	int nMaxParticle = GetMaxParticle();
	int nLife = GetLife();

	// パーティクル生成
	for (int nCnt = 0; nCnt < nMaxParticle; nCnt++)//発生させたい粒子の数
	{
		// テクスチャの指定
		texPath = "data/TEXTURE/effect000.jpg";

		// 位置
		pos = GetPos();

		// ランダムな角度で横に広がる
		float angle = ((rand() % 360) / 180.0f) * D3DX_PI;
		float speed = (rand() % 150) / 300.0f + 0.2f;

		move.x = cosf(angle) * speed;
		move.z = sinf(angle) * speed;
		move.y = (rand() % 300) / 100.0f + 0.9f; // 上方向

		// 色
		col = GetCol();

		// 半径
		fRadius = 35.0f + (rand() % 40);

		// エフェクトの設定
		CEffect::Create(texPath, pos, move, col, fRadius, nLife);
	}

	// パーティクルの更新処理
	CParticle::Update();
}
