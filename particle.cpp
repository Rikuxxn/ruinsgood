//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "particle.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
std::unordered_map<CParticle::TYPE, ParticleCreateFunc> CParticle::m_ParticleFactoryMap = {};

//=============================================================================
// コンストラクタ
//=============================================================================
CParticle::CParticle()
{
	// 値のクリア
	m_Dir			= INIT_VEC3;// 方向
	m_nType			= TYPE_FIRE;// 種類
	m_nLife			= 0;		// 寿命
	m_nMaxParticle	= 0;		// 粒子の最大数
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
CParticle* CParticle::Create(D3DXVECTOR3 dir, D3DXVECTOR3 pos, D3DXCOLOR col, int nLife, TYPE type, int nMaxParticle)
{
	if (m_ParticleFactoryMap.empty())
	{
		// ファクトリー
		InitFactory();
	}

	CParticle* pParticle = nullptr;

	auto it = m_ParticleFactoryMap.find(type);
	if (it != m_ParticleFactoryMap.end())
	{
		pParticle = it->second();
	}
	else
	{
		pParticle = new CParticle(); // デフォルト基底クラス
	}

	if (!pParticle)
	{
		return NULL;
	}

	// 初期化処理
	pParticle->Init();

	// dirが0じゃなかったら設定
	if (dir != INIT_VEC3)
	{// 方向を設定しない時はdirに0を設定
		pParticle->SetDir(dir);
	}

	pParticle->SetPos(pos);
	pParticle->SetCol(col);
	pParticle->SetLife(nLife);
	pParticle->SetParticle(type);
	pParticle->m_nMaxParticle = nMaxParticle;

	return pParticle;
}
//=============================================================================
// ファクトリー
//=============================================================================
void CParticle::InitFactory(void)
{
	// リストを空にする
	m_ParticleFactoryMap.clear();

	m_ParticleFactoryMap[CParticle::TYPE_FIRE]			= []() -> CParticle* { return new CFireParticle(); };
	m_ParticleFactoryMap[CParticle::TYPE_FLAMETHROWER]	= []() -> CParticle* { return new CFlamethrowerParticle(); };
	m_ParticleFactoryMap[CParticle::TYPE_WATER]			= []() -> CParticle* { return new CWaterParticle(); };
	m_ParticleFactoryMap[CParticle::TYPE_AURA]			= []() -> CParticle* { return new CAuraParticle(); };
	m_ParticleFactoryMap[CParticle::TYPE_AURA2]			= []() -> CParticle* { return new CAura2Particle(); };
	m_ParticleFactoryMap[CParticle::TYPE_MOVE]			= []() -> CParticle* { return new CMoveParticle(); };
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
	D3DXVECTOR3 pos;
	D3DXVECTOR3 move;
	D3DXCOLOR col;
	float fRadius;
	const char* texPath;
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
		float angle = ((rand() % 360) / 180.0f) * D3DX_PI;//角度

		float speed = (rand() % 10) / 30.0f + 0.2f;//移動量

		move.x = cosf(angle) * speed;
		move.z = sinf(angle) * speed;
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
// 火炎放射パーティクルのコンストラクタ
//=============================================================================
CFlamethrowerParticle::CFlamethrowerParticle()
{
	// 値のクリア
	m_speedScale = 1.0f;
}
//=============================================================================
// 火炎放射パーティクルのデストラクタ
//=============================================================================
CFlamethrowerParticle::~CFlamethrowerParticle()
{
	// なし
}
//=============================================================================
// 火炎放射パーティクルの初期化処理
//=============================================================================
HRESULT CFlamethrowerParticle::Init(void)
{
	SetPath("data/TEXTURE/smoke.jpg");

	// パーティクルの初期化処理
	CParticle::Init();

	return S_OK;
}
//=============================================================================
// 火炎放射パーティクルの更新処理
//=============================================================================
void CFlamethrowerParticle::Update(void)
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 move;
	D3DXVECTOR3 dir = GetDir();
	D3DXCOLOR col;
	float fRadius;
	const char* texPath;
	int nMaxParticle = GetMaxParticle();
	int nLife = GetLife();

	float speedBase = 10.0f;

	// パーティクル生成
	for (int nCnt = 0; nCnt < nMaxParticle; nCnt++)//発生させたい粒子の数
	{
		// テクスチャの指定
		texPath = "data/TEXTURE/smoke.jpg";

		// 位置の設定
		pos = GetPos();

		// 移動量の設定
		// Dir に少しランダムな揺らぎを足す
		float spread = 0.08f; // ばらつきの最大幅
		D3DXVECTOR3 randOffset(
			((rand() % 200) - 100) / 1000.0f * (spread / 0.1f),
			((rand() % 200) - 100) / 1000.0f * (spread / 0.1f),
			((rand() % 200) - 100) / 1000.0f * (spread / 0.1f)
		);

		D3DXVECTOR3 finalDir = dir + randOffset;
		D3DXVec3Normalize(&finalDir, &finalDir);

		float speed = speedBase + (rand() % 11); // ベース速度 + ランダム
		speed *= m_speedScale;                    // 速度倍率をかける

		move = finalDir * speed;

		// 上方向成分も少し強めに（火炎っぽく）
		move.y += 0.3f;

		// 色の設定
		col = GetCol();

		// 半径の設定
		fRadius = 15.0f;

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
	D3DXVECTOR3 pos;
	D3DXVECTOR3 move;
	D3DXCOLOR col;
	float fRadius;
	const char* texPath;
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
	D3DXVECTOR3 pos;
	D3DXVECTOR3 move;
	D3DXCOLOR col;
	float fRadius;
	const char* texPath;
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
	D3DXVECTOR3 pos;
	D3DXVECTOR3 move;
	D3DXCOLOR col;
	float fRadius;
	const char* texPath;
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


//=============================================================================
// 移動時パーティクルのコンストラクタ
//=============================================================================
CMoveParticle::CMoveParticle()
{

}
//=============================================================================
// 移動時パーティクルのデストラクタ
//=============================================================================
CMoveParticle::~CMoveParticle()
{
	// なし
}
//=============================================================================
// 移動時パーティクルの初期化処理
//=============================================================================
HRESULT CMoveParticle::Init(void)
{
	SetPath("data/TEXTURE/smoke.jpg");

	// パーティクルの初期化処理
	CParticle::Init();

	return S_OK;
}
//=============================================================================
// 移動時パーティクルの更新処理
//=============================================================================
void CMoveParticle::Update(void)
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 move;
	D3DXCOLOR col;
	float fRadius;
	const char* texPath;
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
		float speed = (rand() % 150) / 300.0f + 0.2f;

		move.x = cosf(angle) * speed;
		move.z = sinf(angle) * speed;
		move.y = (rand() % 30) / 50.0f + 0.05f; // 少しだけ上方向

		// 色
		col = GetCol();

		// 半径
		fRadius = 15.0f + (rand() % 10);

		// エフェクトの設定
		CEffect::Create(texPath, pos, move, col, fRadius, nLife);
	}

	// パーティクルの更新処理
	CParticle::Update();
}