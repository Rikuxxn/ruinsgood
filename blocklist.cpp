//=============================================================================
//
// ブロックリスト処理 [blocklist.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "blocklist.h"
#include "blockmanager.h"
#include "player.h"
#include "game.h"
#include "manager.h"
#include "particle.h"
#include <algorithm>
#include "collisionUtils.h"

using namespace std;

//=============================================================================
// 木箱ブロックのコンストラクタ
//=============================================================================
CWoodBoxBlock::CWoodBoxBlock()
{
	// 値のクリア
	m_ResPos = INIT_VEC3;
}
//=============================================================================
// 木箱ブロックのデストラクタ
//=============================================================================
CWoodBoxBlock::~CWoodBoxBlock()
{
	// なし
}
//=============================================================================
// 木箱ブロックの初期化処理
//=============================================================================
HRESULT CWoodBoxBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	// 最初の位置をリスポーン位置に設定
	m_ResPos = GetPos();

	// 動的に戻す
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// 木箱ブロックの更新処理
//=============================================================================
void CWoodBoxBlock::Update()
{
	CBlock::Update(); // 共通処理

	if (GetPos().y <= -810.0f)
	{
		// リスポーン処理
		Respawn();
	}
}
//=============================================================================
// リスポーン処理
//=============================================================================
void CWoodBoxBlock::Respawn(void)
{
	// 動かすためにキネマティックにする
	SetEditMode(true);

	// ブロックの位置を取得
	D3DXVECTOR3 Pos = GetPos();
	D3DXVECTOR3 Rot = GetRot();

	D3DXVECTOR3 respawnPos(m_ResPos);// リスポーン位置
	D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);// 向きをリセット

	Pos = respawnPos;
	Rot = rot;

	SetPos(Pos);
	SetRot(Rot);

	// コライダーの更新
	UpdateCollider();

	// 動的に戻す
	SetEditMode(false);
}
//=============================================================================
// セット処理
//=============================================================================
void CWoodBoxBlock::Set(D3DXVECTOR3 pos)
{
	// 動かすためにキネマティックにする
	SetEditMode(true);

	// 鍵ブロックの位置を取得
	D3DXVECTOR3 Pos = GetPos();
	D3DXVECTOR3 Rot = GetRot();

	D3DXVECTOR3 targetPos(pos);			// 設置する目標位置
	D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);	// 向きをリセット

	Pos = targetPos;
	Rot = rot;

	SetPos(Pos);
	SetRot(Rot);

	// コライダーの更新
	UpdateCollider();
}


//=============================================================================
// 溶岩ブロックのコンストラクタ
//=============================================================================
CLavaBlock::CLavaBlock()
{
	// 値のクリア
}
//=============================================================================
// 溶岩ブロックのデストラクタ
//=============================================================================
CLavaBlock::~CLavaBlock()
{
	// なし
}
//=============================================================================
// 溶岩ブロックの更新処理
//=============================================================================
void CLavaBlock::Update(void)
{
	CBlock::Update();// 共通処理

	// ブロック AABB を取得
	D3DXVECTOR3 blockPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize(); // ブロック元のサイズ（中心原点）
	D3DXVECTOR3 scale = GetSize();// 拡大率

	// 拡大率を適用する
	D3DXVECTOR3 blockSize;
	blockSize.x = modelSize.x * scale.x;
	blockSize.y = modelSize.y * scale.y;
	blockSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 blockMin = blockPos - blockSize * 0.5f;
	D3DXVECTOR3 blockMax = blockPos + blockSize * 0.5f;

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block == this || block->GetType() != TYPE_KEY)
		{
			continue; // 自分 or 指定ブロック以外は無視
		}

		CKeyBlock* pKey = dynamic_cast<CKeyBlock*>(block);

		// ブロックの AABB を取得
		D3DXVECTOR3 pos = block->GetPos();
		D3DXVECTOR3 size = block->GetModelSize();
		D3DXVECTOR3 min = pos - size * 0.5f;
		D3DXVECTOR3 max = pos + size * 0.5f;

		// AABB同士の交差チェック
		bool isOverlap =
			blockMin.x <= max.x && blockMax.x >= min.x &&
			blockMin.y <= max.y && blockMax.y >= min.y &&
			blockMin.z <= max.z && blockMax.z >= min.z;

		if (isOverlap)
		{// 当たってたら
			// リスポーン
			Respawn(pKey->GetResPos());
		}
	}

	// --- プレイヤー接触判定 ---
	CPlayer* pPlayer = CGame::GetPlayer();

	if (pPlayer)
	{
		D3DXVECTOR3 pPos = pPlayer->GetColliderPos(); // カプセルコライダー中心位置

		// カプセルコライダーのサイズからAABBサイズを計算
		float radius = pPlayer->GetRadius();
		float height = pPlayer->GetHeight();

		D3DXVECTOR3 pSize;
		pSize.x = radius * 2.0f;
		pSize.z = radius * 2.0f;
		pSize.y = height + radius * 2.0f;

		// AABB計算
		D3DXVECTOR3 pMin = pPos - pSize * 0.5f;
		D3DXVECTOR3 pMax = pPos + pSize * 0.5f;

		bool isOverlap =
			blockMin.x <= pMax.x && blockMax.x >= pMin.x &&
			blockMin.y <= pMax.y && blockMax.y >= pMin.y &&
			blockMin.z <= pMax.z && blockMax.z >= pMin.z;

		if (isOverlap)
		{
			// リスポーン処理
			pPlayer->RespawnToCheckpoint();
		}
	}
}


//=============================================================================
// 水ブロックのコンストラクタ
//=============================================================================
CWaterBlock::CWaterBlock()
{
	// 値のクリア
	m_waterStayTime = 0;				// 水中滞在時間（秒）
	m_isInWater = false;				// 今水中にいるか
	m_bWasInWater = false;				// 水に入ったか
	m_isRespawn = true;					// リスポーン状態か
}
//=============================================================================
// 水ブロックのデストラクタ
//=============================================================================
CWaterBlock::~CWaterBlock()
{
	// なし
}
//=============================================================================
// 水ブロックの更新処理
//=============================================================================
void CWaterBlock::Update(void)
{
	ApplyToBlocks();// ブロックを浮かせる
	ApplyToPlayer();// プレイヤーを浮かせる

	CBlock::Update();// 共通処理
}
//=============================================================================
// 水ブロックのブロック浮遊処理
//=============================================================================
void CWaterBlock::ApplyToBlocks(void)
{
	// 水の AABB を取得
	D3DXVECTOR3 wtPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize();
	D3DXVECTOR3 scale = GetSize();

	D3DXVECTOR3 wtSize;

	// 拡大率を適用する
	wtSize.x = modelSize.x * scale.x;
	wtSize.y = modelSize.y * scale.y;
	wtSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 wtMin = wtPos - wtSize * 0.5f;
	D3DXVECTOR3 wtMax = wtPos + wtSize * 0.5f;

	// ブロックの浮力設定
	const float B_maxLiftSpeed = 40.0f;

	// ------------------------
	// 他のブロックを浮かせる
	// ------------------------
	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block == this || !block->IsDynamicBlock())
		{
			continue; // 自分 or 静的ブロックは無視
		}

		// ブロックの AABB を取得
		D3DXVECTOR3 pos = block->GetPos();
		D3DXVECTOR3 size = block->GetModelSize();
		D3DXVECTOR3 min = pos - size * 0.5f;
		D3DXVECTOR3 max = pos + size * 0.5f;

		// AABB同士の交差チェック
		bool isOverlap =
			wtMin.x <= max.x && wtMax.x >= min.x &&
			wtMin.y <= max.y && wtMax.y >= min.y &&
			wtMin.z <= max.z && wtMax.z >= min.z;

		if (!isOverlap)
		{// 当たってなかったら
			continue;
		}

		CWoodBoxBlock* pWoodBox = dynamic_cast<CWoodBoxBlock*>(block);

		// 木箱ブロックだったら
		if (block->GetType() == TYPE_WOODBOX && m_isRespawn)
		{
			// リスポーン処理
			pWoodBox->Respawn();
		}

		btRigidBody* pRigid = block->GetRigidBody();

		if (!pRigid || !block->IsDynamicBlock())
		{
			continue;
		}

		btVector3 velocity = pRigid->getLinearVelocity();

		if (velocity.getY() < B_maxLiftSpeed)
		{
			// 浮かばせる目標上向き速度
			const float targetUpSpeed = 125.0f; // 浮上スピード
			const float maxUpSpeed = 120.0f;    // 上限速度
			const float forceScale = 0.12f;		// 差分にかける係数（反応の速さ）

			btVector3 velocity = pRigid->getLinearVelocity();

			// 水中での摩擦（横方向制限）
			velocity.setX(velocity.getX() * 0.5f);
			velocity.setZ(velocity.getZ() * 0.5f);

			// 浮力：現在のY速度との差を補正
			float diffY = targetUpSpeed - velocity.getY();
			velocity.setY(velocity.getY() + diffY * forceScale);

			// 最大上昇速度制限
			if (velocity.getY() > maxUpSpeed)
			{
				velocity.setY(maxUpSpeed);
			}

			pRigid->setLinearVelocity(velocity);

			// 水中では回転を止めて安定させる
			pRigid->setAngularVelocity(btVector3(0, 0, 0));
		}
	}
}
//=============================================================================
// 水ブロックのプレイヤー浮遊処理
//============================================================================
void CWaterBlock::ApplyToPlayer(void)
{
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	CParticle* pParticle = NULL;

	// 水の AABB を取得
	D3DXVECTOR3 wtPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize();
	D3DXVECTOR3 scale = GetSize();

	D3DXVECTOR3 wtSize;

	// 拡大率を適用する
	wtSize.x = modelSize.x * scale.x;
	wtSize.y = modelSize.y * scale.y;
	wtSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 wtMin = wtPos - wtSize * 0.5f;
	D3DXVECTOR3 wtMax = wtPos + wtSize * 0.5f;

	// プレイヤーの浮力設定
	const float P_waterLiftPower = 18.0f;
	const float P_maxLiftSpeed = 53.0f;

	// ------------------------
	// プレイヤーを浮かせる
	// ------------------------
	CPlayer* pPlayer = CGame::GetPlayer();

	if (pPlayer)
	{
		D3DXVECTOR3 pPos = pPlayer->GetColliderPos(); // カプセルコライダー中心位置
		pPos.y += 40.0f;

		// カプセルコライダーのサイズからAABBサイズを計算
		float radius = pPlayer->GetRadius();
		float height = pPlayer->GetHeight();

		D3DXVECTOR3 pSize;
		pSize.x = radius * 2.0f;
		pSize.z = radius * 2.0f;
		pSize.y = height + radius * 2.0f;

		// AABB計算
		D3DXVECTOR3 pMin = pPos - pSize * 0.5f;
		D3DXVECTOR3 pMax = pPos + pSize * 0.5f;

		bool isOverlap =
			wtMin.x <= pMax.x && wtMax.x >= pMin.x &&
			wtMin.y <= pMax.y && wtMax.y >= pMin.y &&
			wtMin.z <= pMax.z && wtMax.z >= pMin.z;

		// === 着水瞬間判定 ===
		bool isNowInWater = isOverlap;

		if (isNowInWater && !m_bWasInWater)
		{
			// プレイヤーの位置
			D3DXVECTOR3 pos = pPlayer->GetPos();
			pos.y += 80.0f;

			// 入水SE
			CManager::GetSound()->Play(CSound::SOUND_LABEL_WATER);

			// 水しぶきパーティクル生成
			pParticle = CParticle::Create(INIT_VEC3, pos, D3DXCOLOR(0.3f, 0.6f, 1.0f, 0.8f), 50, CParticle::TYPE_WATER, 10);
			pParticle = CParticle::Create(INIT_VEC3, pos, D3DXCOLOR(0.3f, 0.5f, 1.0f, 0.5f), 50, CParticle::TYPE_WATER, 10);
		}

		// プレイヤーが水に入っていたかを記録
		m_bWasInWater = isNowInWater;

		if (!isOverlap)
		{// 当たってなかったら
			return;
		}

		btRigidBody* pRigid = pPlayer->GetRigidBody();

		if (pRigid)
		{
			btVector3 velocity = pRigid->getLinearVelocity();

			if (velocity.getY() < P_maxLiftSpeed)
			{
				velocity.setY(velocity.getY() + P_waterLiftPower);
				pRigid->setLinearVelocity(velocity);
			}

			// 水中にいるフラグON
			SetInWater(true); // フラグ用関数

			// タイマー更新
			AddWaterStayTime();
		}
		else
		{
			SetInWater(false);
			ResetWaterStayTime();
		}
	}
}
//=============================================================================
// 水中にいるフラグ設定処理
//=============================================================================
void CWaterBlock::SetInWater(bool flag)
{
	m_isInWater = flag;
}
//=============================================================================
// 水中滞在時間の設定
//=============================================================================
void CWaterBlock::AddWaterStayTime(void)
{
	if (m_isInWater)
	{
		m_waterStayTime++;

		if (m_waterStayTime >= 180) // 3秒以上滞在したら
		{
			CPlayer* pPlayer = CGame::GetPlayer();
			if (!pPlayer)
			{
				return;
			}

			pPlayer->RespawnToCheckpoint(D3DXVECTOR3(427.0f, 30.0f, 1110.0f)); // リスポーン
			m_waterStayTime = 0;
		}
	}
}
//=============================================================================
// 水中滞在時間のリセット
//=============================================================================
void CWaterBlock::ResetWaterStayTime(void)
{
	m_waterStayTime = 0;
}


//=============================================================================
// ドアブロックのコンストラクタ
//=============================================================================
CDoorBlock::CDoorBlock()
{
	// 値のクリア
	m_isDoorOpened = false;
	m_initialPos = INIT_VEC3;
}
//=============================================================================
// ドアブロックのデストラクタ
//=============================================================================
CDoorBlock::~CDoorBlock()
{
	// なし
}
//=============================================================================
// ドアブロックの初期化処理
//=============================================================================
HRESULT CDoorBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	m_initialPos = GetPos();

	return S_OK;
}
//=============================================================================
// ドアブロックの更新処理
//=============================================================================
void CDoorBlock::Update(void)
{
	if (CManager::GetMode() != MODE_GAME || !CGame::GetPlayer() || IsEditMode())
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();
	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 330.0f;// 反応する距離
	const float kOpenHeight = 210.0f;       // どれくらい開くか

	if (distance < kTriggerDistance && !m_isDoorOpened)
	{
		m_isDoorOpened = true;	// フラグON
	}
	else if (distance >= kTriggerDistance && m_isDoorOpened)
	{
		m_isDoorOpened = false;
	}

	// 現在位置
	D3DXVECTOR3 newPos = GetPos();

	float targetY = m_isDoorOpened
		? m_initialPos.y + kOpenHeight // 開いた位置
		: m_initialPos.y;              // 閉じた位置

	// スムーズに移動
	if (fabs(newPos.y - targetY) > 0.01f)
	{
		float moveSpeed = 1.0f;

		if (newPos.y < targetY)
		{
			newPos.y = min(newPos.y + moveSpeed, targetY);
		}
		else
		{
			newPos.y = max(newPos.y - moveSpeed, targetY);
		}

		SetPos(newPos);
	}

	CBlock::Update(); // 共通処理
}


//=============================================================================
// 最終エリアドアブロックのコンストラクタ
//=============================================================================
CBigDoorBlock::CBigDoorBlock()
{
	// 値のクリア
	m_isDoorOpened = false;
}
//=============================================================================
// 最終エリアドアブロックのデストラクタ
//=============================================================================
CBigDoorBlock::~CBigDoorBlock()
{
	// なし
}
//=============================================================================
// 最終エリアドアブロックの初期化処理
//=============================================================================
HRESULT CBigDoorBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	// 最初の位置を保存
	m_openPos = GetPos();

	return S_OK;
}
//=============================================================================
// 最終エリアドアブロックの更新処理
//=============================================================================
void CBigDoorBlock::Update(void)
{
	CBlock::Update();// 共通処理

	if (!m_isDoorOpened)
	{
		// ターゲットブロックを探す
		for (CBlock* block : CBlockManager::GetAllBlocks())
		{
			if (block->GetType() != TYPE_TARGET)
			{
				continue;
			}

			CTargetBlock* pTarget = dynamic_cast<CTargetBlock*>(block);

			// 当たっていたら
			if (pTarget && pTarget->IsHit())
			{
				m_isDoorOpened = true;
				break;
			}

		}
	}

	if (m_isDoorOpened)
	{
		// ドアを開ける
		const float kOpenRange = 250.0f; // 開く高さ

		float targetY = m_openPos.y + kOpenRange;

		D3DXVECTOR3 pos = GetPos();
		if (pos.y < targetY)
		{
			pos.y += 1.0f;
			SetPos(pos);
		}
	}
}


//=============================================================================
// スイッチブロックのコンストラクタ
//=============================================================================
CSwitchBlock::CSwitchBlock()
{
	// 値のクリア
	m_closedPos = INIT_VEC3;
	m_isSwitchOn = false;
	m_prevSwitchOn = false;
}
//=============================================================================
// スイッチブロックのデストラクタ
//=============================================================================
CSwitchBlock::~CSwitchBlock()
{
	// なし
}
//=============================================================================
// スイッチブロックの初期化処理
//=============================================================================
HRESULT CSwitchBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	m_closedPos = GetPos();

	return S_OK;
}
//=============================================================================
// スイッチブロックの更新処理
//=============================================================================
void CSwitchBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	// スイッチの AABB を取得
	D3DXVECTOR3 swPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize(); // スイッチの元のサイズ（中心原点）
	D3DXVECTOR3 scale = GetSize();// 拡大率

	// 拡大率を適用する
	D3DXVECTOR3 swSize;
	swSize.x = modelSize.x * scale.x;
	swSize.y = modelSize.y * scale.y;
	swSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 swMin = swPos - swSize * 0.5f;
	D3DXVECTOR3 swMax = swPos + swSize * 0.5f;

	float totalMass = 0.0f;

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block == this || !block->IsDynamicBlock() || block->GetType() == TYPE_ROCK)
		{
			continue; // 自分 or 静的ブロックは無視
		}

		// ブロックの AABB を取得
		D3DXVECTOR3 pos = block->GetPos();
		D3DXVECTOR3 size = block->GetModelSize();
		D3DXVECTOR3 min = pos - size * 0.5f;
		D3DXVECTOR3 max = pos + size * 0.5f;

		// AABB同士の交差チェック
		bool isOverlap =
			swMin.x <= max.x && swMax.x >= min.x &&
			swMin.y <= max.y && swMax.y >= min.y &&
			swMin.z <= max.z && swMax.z >= min.z;

		if (isOverlap)
		{
			btScalar invMass = block->GetRigidBody()->getInvMass();
			float mass = (invMass == 0.0f) ? 0.0f : 1.0f / invMass;
			totalMass += mass;
		}
	}

	// 質量のしきい値を超えていたら沈む
	const float massThreshold = 12.0f;

	if (totalMass >= massThreshold)
	{
		m_isSwitchOn = true;
	}

	if (!m_isSwitchOn)
	{
		return;
	}

	// 押されている → 閉じた位置から少し下げる
	const float kPressDepth = 8.0f; // 下がる深さ

	float targetY = m_closedPos.y - kPressDepth;

	D3DXVECTOR3 pos = GetPos();
	if (pos.y > targetY)
	{
		pos.y = max(pos.y - 1.0f, targetY);
		SetPos(pos);
	}

	SetEditMode(true); // 動かすためにキネマティック

	bool n = m_isSwitchOn;

	if (n && !m_prevSwitchOn)
	{
		// スイッチSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_SWITCH);

		// 水位上昇SE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_WATERRISE);

		// 演出カメラにする
		CManager::GetCamera()->SetCamMode(5, D3DXVECTOR3(900.5f, 214.0f, 530.5f),
			D3DXVECTOR3(1120.0f, -27.0f, 670.0f),
			D3DXVECTOR3(0.75f, -2.15f, 0.0f));
	}

	// フラグを更新して次のフレームに備える
	m_prevSwitchOn = n;

	// 水位上昇処理
	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_WATER)
		{
			continue;
		}

		D3DXVECTOR3 waterPos = block->GetPos();
		CWaterBlock* pWater = dynamic_cast<CWaterBlock*>(block);

		if (waterPos.y < -280.0f)
		{
			// 徐々に水位を上げる
			waterPos.y += 0.5f;
			block->SetPos(waterPos);
		}
		else
		{
			pWater->IsRespawn(false);
			m_isSwitchOn = false;
		}
	}

}


//=============================================================================
// 橋制御ブロックのコンストラクタ
//=============================================================================
CBridgeSwitchBlock::CBridgeSwitchBlock()
{
	// 値のクリア
	m_closedPos = INIT_VEC3;
	m_isSwitchOn = false;
	m_prevSwitchOn = false;
	m_OnCnt = 120;
	m_isSinkOn = false;
	m_prevSinkOn = false;
}
//=============================================================================
// 橋制御ブロックのデストラクタ
//=============================================================================
CBridgeSwitchBlock::~CBridgeSwitchBlock()
{
	// なし
}
//=============================================================================
// 橋制御ブロックの初期化処理
//=============================================================================
HRESULT CBridgeSwitchBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	m_closedPos = GetPos();

	return S_OK;
}
//=============================================================================
// 橋制御ブロックの更新処理
//=============================================================================
void CBridgeSwitchBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	// スイッチの AABB を取得
	D3DXVECTOR3 swPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize(); // スイッチの元のサイズ（中心原点）
	D3DXVECTOR3 scale = GetSize();// 拡大率

	// 拡大率を適用する
	D3DXVECTOR3 swSize;
	swSize.x = modelSize.x * scale.x;
	swSize.y = modelSize.y * scale.y;
	swSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 swMin = swPos - swSize * 0.5f;
	D3DXVECTOR3 swMax = swPos + swSize * 0.5f;

	float totalMass = 0.0f;

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() == TYPE_MASSBLOCK_RED || block->GetType() == TYPE_MASSBLOCK_BLUE || 
			block->GetType() == TYPE_MASSBLOCK_YELLOW || block->GetType() == TYPE_MASSBLOCK_GREEN)
		{
			// ブロックの AABB を取得
			D3DXVECTOR3 pos = block->GetPos();
			D3DXVECTOR3 size = block->GetModelSize();
			D3DXVECTOR3 min = pos - size * 0.5f;
			D3DXVECTOR3 max = pos + size * 0.5f;

			// AABB同士の交差チェック
			bool isOverlap =
				swMin.x <= max.x && swMax.x >= min.x &&
				swMin.y <= max.y && swMax.y >= min.y &&
				swMin.z <= max.z && swMax.z >= min.z;

			if (isOverlap)
			{
				// 基点のブロックから積み上げ分を再帰的に調べる
				totalMass += CalcStackMass(block);
			}
		}
	}

	// 質量のしきい値になったら沈む
	const float massThreshold = 10.0f;

	if (totalMass == massThreshold)
	{
		m_isSinkOn = true;
	}

	bool b = m_isSinkOn;

	if (b && !m_prevSinkOn)
	{
		// スイッチSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_SWITCH);
	}

	m_prevSinkOn = b;

	if (m_isSinkOn)
	{
		m_OnCnt--;

		if (m_OnCnt < 0)
		{
			m_OnCnt = 0;
			m_isSwitchOn = true;
		}

		// 押されている → 閉じた位置から少し下げる
		const float kPressDepth = 10.0f; // 下がる深さ

		float targetY = m_closedPos.y - kPressDepth;

		D3DXVECTOR3 pos = GetPos();
		if (pos.y > targetY)
		{
			pos.y = max(pos.y - 1.0f, targetY);
			SetPos(pos);
		}
	}

	bool n = m_isSwitchOn;

	if (n && !m_prevSwitchOn) // 一回だけ実行
	{
		// 演出カメラにする
		CManager::GetCamera()->SetCamMode(9, D3DXVECTOR3(-1270.0f, 370.0f, -4382.0f),
			D3DXVECTOR3(-1527.0f, 194.0f, -4085.0f),
			D3DXVECTOR3(0.43f, 0.23f, 0.0f));

		for (CBlock* block : CBlockManager::GetAllBlocks())
		{
			if (block->GetType() != TYPE_ROCK)
			{
				continue;
			}

			CRockBlock* pRock = dynamic_cast<CRockBlock*>(block);
			if (pRock)
			{
				pRock->UseBridgeSwitch(true);
				pRock->Respawn();
			}

		}
	}

	// フラグを更新して次のフレームに備える
	m_prevSwitchOn = n;
}
//=============================================================================
// 積み上げ質量計算
//=============================================================================
float CBridgeSwitchBlock::CalcStackMass(CBlock* base)
{
	// ブロックの質量の取得
	float mass = base->GetMass();

	for (CBlock* other : CBlockManager::GetAllBlocks())
	{
		if (other == base)
		{
			continue;
		}

		// other が base の上に物理的に乗っているかどうか判定
		if (IsOnTop(base, other))
		{
			mass += CalcStackMass(other);
			return mass;
		}
	}
	return 0.0f;
}
//=============================================================================
// baseの上にotherがあるか判定
//=============================================================================
bool CBridgeSwitchBlock::IsOnTop(CBlock* base, CBlock* other)
{
	// モデル本来のサイズ
	D3DXVECTOR3 baseModelSize = base->GetModelSize();
	D3DXVECTOR3 otherModelSize = other->GetModelSize();

	// 拡大率
	D3DXVECTOR3 baseScale = base->GetSize();
	D3DXVECTOR3 otherScale = other->GetSize();

	// 実際の大きさ（スケーリング後）
	D3DXVECTOR3 baseSize = D3DXVECTOR3(baseModelSize.x * baseScale.x,
		baseModelSize.y * baseScale.y,
		baseModelSize.z * baseScale.z);

	D3DXVECTOR3 otherSize = D3DXVECTOR3(otherModelSize.x * otherScale.x,
		otherModelSize.y * otherScale.y,
		otherModelSize.z * otherScale.z);

	D3DXVECTOR3 baseMin = base->GetPos() - baseSize * 0.5f;
	D3DXVECTOR3 baseMax = base->GetPos() + baseSize * 0.5f;

	D3DXVECTOR3 otherMin = other->GetPos() - otherSize * 0.5f;
	D3DXVECTOR3 otherMax = other->GetPos() + otherSize * 0.5f;

	// --- XZ が重なっているか ---
	bool overlapXZ =
		(baseMin.x <= otherMax.x && baseMax.x >= otherMin.x) &&
		(baseMin.z <= otherMax.z && baseMax.z >= otherMin.z);

	if (!overlapXZ) return false;

	// --- other が base の上にあるか (少しだけ隙間許容) ---
	float epsilon = 1.0f;
	if (otherMin.y >= baseMax.y - epsilon)
	{
		return true;
	}

	return false;
}


//=============================================================================
// 格子制御ブロックのコンストラクタ
//=============================================================================
CBarSwitchBlock::CBarSwitchBlock()
{
	// 値のクリア
	m_closedPos = INIT_VEC3;
	m_isSwitchOn = false;
	m_prevSwitchOn = false;
	m_timerCnt = 0;
	m_Timer = 0;
}
//=============================================================================
// 格子制御ブロックのデストラクタ
//=============================================================================
CBarSwitchBlock::~CBarSwitchBlock()
{
	// なし
}
//=============================================================================
// 格子制御ブロックの初期化処理
//=============================================================================
HRESULT CBarSwitchBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	m_closedPos = GetPos();

	return S_OK;
}
//=============================================================================
// 格子制御ブロックの更新処理
//=============================================================================
void CBarSwitchBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	// スイッチの AABB を取得
	D3DXVECTOR3 swPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize(); // スイッチの元のサイズ（中心原点）
	D3DXVECTOR3 scale = GetSize();// 拡大率

	// 拡大率を適用する
	D3DXVECTOR3 swSize;
	swSize.x = modelSize.x * scale.x;
	swSize.y = modelSize.y * scale.y;
	swSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 swMin = swPos - swSize * 0.5f;
	D3DXVECTOR3 swMax = swPos + swSize * 0.5f;

	// --- スイッチ押下判定（プレイヤー接触時） ---
	CPlayer* pPlayer = CGame::GetPlayer();

	if (pPlayer)
	{
		D3DXVECTOR3 pPos = pPlayer->GetColliderPos(); // カプセルコライダー中心位置

		// カプセルコライダーのサイズからAABBサイズを計算
		float radius = pPlayer->GetRadius();
		float height = pPlayer->GetHeight();

		D3DXVECTOR3 pSize;
		pSize.x = radius * 2.0f;
		pSize.z = radius * 2.0f;
		pSize.y = height + radius * 2.0f;

		// AABB計算
		D3DXVECTOR3 pMin = pPos - pSize * 0.5f;
		D3DXVECTOR3 pMax = pPos + pSize * 0.5f;

		bool isOverlap =
			swMin.x <= pMax.x && swMax.x >= pMin.x &&
			swMin.y <= pMax.y && swMax.y >= pMin.y &&
			swMin.z <= pMax.z && swMax.z >= pMin.z;

		// 制御スイッチが存在するか確認
		std::vector<CBlock*> blocks = CBlockManager::GetAllBlocks();

		for (CBlock* block : blocks)
		{
			if (block->GetType() != TYPE_BRIDGE3)
			{
				continue;
			}

			CFootingBlock* pFooting = dynamic_cast<CFootingBlock*>(block);

			if (isOverlap && !pFooting->GetMove())
			{
				m_isSwitchOn = true;

				// タイムを設定
				SetTimer(12);

				// 演出カメラをONにする
				CManager::GetCamera()->IsDirection(true);
			}
		}
	}

	if (!m_isSwitchOn)
	{
		return;
	}

	if (m_isSwitchOn && !CManager::GetCamera()->GetDirection())
	{
		if (m_timerCnt == 0)
		{
			// タイマーSE
			CManager::GetSound()->Play(CSound::SOUND_LABEL_TIMER);
		}

		m_timerCnt++;

		// 押されている → 閉じた位置から少し下げる
		const float kPressDepth = 8.0f; // 下がる深さ

		float targetY = m_closedPos.y - kPressDepth;

		D3DXVECTOR3 pos = GetPos();
		if (pos.y > targetY)
		{
			pos.y = max(pos.y - 1.0f, targetY);
			SetPos(pos);
		}

		if (m_timerCnt >= m_Timer)
		{// 指定時間を経過したら
			m_isSwitchOn = false;
			m_timerCnt = 0;

			SetPos(m_closedPos); // 元の高さに戻す
		}
	}

	bool n = m_isSwitchOn;

	if (n && !m_prevSwitchOn) // 一回だけ実行
	{
		// スイッチSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_SWITCH);

		// 演出カメラにする
		CManager::GetCamera()->SetCamMode(3, D3DXVECTOR3(2572.5f, 218.7f, -76.0f),
			D3DXVECTOR3(3158.0f, -29.0f, -562.0f),
			D3DXVECTOR3(0.32f, -0.90f, 0.0f));
	}

	// フラグを更新して次のフレームに備える
	m_prevSwitchOn = n;
}


//=============================================================================
// 斧ブロックのコンストラクタ
//=============================================================================
CAxeBlock::CAxeBlock()
{
	// 値のクリア
	m_nSwingCounter = 0;					// フレームカウンター
	m_swingAmplitude = D3DXToRadian(75.0f);	// ±振れ角
	m_swingPeriod = 300.0f;					// 周期フレーム
	m_prevSwingAngle = 0.0f;				// 前回のスイング角度
	m_wasPositive = true;					// 前回の角度が正かどうか
	m_playedSoundID = -1;					// 再生中の音ID
}
//=============================================================================
// 斧ブロックのデストラクタ
//=============================================================================
CAxeBlock::~CAxeBlock()
{
	// なし
}
//=============================================================================
// 斧ブロックの更新処理
//=============================================================================
void CAxeBlock::Update(void)
{
	CBlock::Update();// 共通処理

	if (!CGame::GetPlayer())
	{
		return;
	}

	Swing();	// スイング処理

	IsPlayerHit();// プレイヤーとの接触判定
}
//=============================================================================
// 斧ブロックのスイング処理
//=============================================================================
void CAxeBlock::Swing(void)
{
	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 1350.0f; // 反応距離

	if (distance > kTriggerDistance)
	{
		return;
	}

	m_nSwingCounter++;

	float angle = m_swingAmplitude * sinf((2.0f * D3DX_PI * m_nSwingCounter) / m_swingPeriod);

	SetRot(D3DXVECTOR3(0.0f, 0.0f, angle)); // Z軸スイング

	// オフセット
	D3DXVECTOR3 localOffset(0.0f, -100.0f, 0.0f);
	D3DXVECTOR3 worldOffset;

	// ブロックのワールドマトリックスを取得
	D3DXMATRIX worldMtx = GetWorldMatrix();

	D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

	const float soundDistance = 1350.0f; // 反応距離

	if (distance < soundDistance)
	{
		// 極値通過検出：正→負 or 負→正 でスイングSEを鳴らす
		bool isNowPositive = (angle >= 0.0f);

		if (m_wasPositive != isNowPositive) // 再生していなければ再生開始
		{
			// 前の音を止める（念のため）
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3Dサウンド再生してIDを保持
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_SWING, GetPos(), 150.0f, kTriggerDistance);
		}

		m_wasPositive = isNowPositive;
	}
	else
	{
		// 離れたら音停止してIDリセット
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	m_prevSwingAngle = angle;

	// 音源の位置更新（再生中のみ）
	if (m_playedSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, worldOffset);
	}
}
//=============================================================================
// プレイヤーとの接触判定処理
//=============================================================================
void CAxeBlock::IsPlayerHit(void)
{
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	CPlayer* pPlayer = CGame::GetPlayer();

	btRigidBody* pPlayerBody = pPlayer->GetRigidBody();
	btRigidBody* pAxeBody = GetRigidBody();

	if (!pPlayerBody || !pAxeBody)
	{
		return;
	}

	struct HitResultCallback : public btCollisionWorld::ContactResultCallback
	{
		bool isHit = false;

		btScalar addSingleResult(btManifoldPoint& cp,
			const btCollisionObjectWrapper* colObj0, int partId0, int index0,
			const btCollisionObjectWrapper* colObj1, int partId1, int index1) override
		{
			isHit = true;
			return 0;
		}
	};

	HitResultCallback callback;

	// contactTest を使って片方の剛体だけチェック
	CManager::GetPhysicsWorld()->contactPairTest(pPlayerBody, pAxeBody, callback);

	bool isNowHit = callback.isHit;

	if (isNowHit && !m_isPrevHit)
	{
		// プレイヤーヒットSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_HIT);
	}

	// プレイヤーが当たっていたかを記録
	m_isPrevHit = isNowHit;

	if (callback.isHit)
	{
		// プレイヤーのリスポーン
		pPlayer->RespawnToCheckpoint();
	}
}


//=============================================================================
// 岩ブロックのコンストラクタ
//=============================================================================
CRockBlock::CRockBlock()
{
	// 値のクリア
	m_pathPoints = {};
	m_currentTargetIndex = 0;
	m_speed = 86500.0f;
	m_isBridgeSwitchOn = false;
	m_isHit = false;
	m_isPrevHit = false;
	m_isThrough = false;
	m_isPrevThrough = false;
	m_particleTimer = 0;
	m_playedRollSoundID = -1;
}
//=============================================================================
// 岩ブロックのデストラクタ
//=============================================================================
CRockBlock::~CRockBlock()
{
	// なし
}
//=============================================================================
// 岩ブロックの更新処理
//=============================================================================
void CRockBlock::Update(void)
{
	CBlock::Update();	// 共通処理

	if (!CGame::GetPlayer())
	{
		return;
	}

	const float RESET_HEIGHT = -480.0f;

	if (GetPos().y < RESET_HEIGHT)
	{
		Respawn();			// リスポーン処理
	}

	MoveToTarget();		// チェックポイントへ向けて移動

	IsPlayerHit();		// プレイヤーとの接触判定
}
//=============================================================================
// リスポーン処理
//=============================================================================
void CRockBlock::Respawn(void)
{
	// 転がる音の停止
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_ROLL);

	// 動かすためにキネマティックにする
	SetEditMode(true);

	// 岩ブロックの位置を取得
	D3DXVECTOR3 rockPos = GetPos();
	D3DXVECTOR3 rockRot = GetRot();

	D3DXVECTOR3 respawnPos(2815.5f, 700.0f, -1989.0f);// リスポーン位置
	D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);// 向きをリセット

	rockPos = respawnPos;
	rockRot = rot;

	SetPos(rockPos);
	SetRot(rockRot);

	// コライダーの更新
	UpdateCollider();

	// 現在のチェックポイントインデックスをリセットする
	m_currentTargetIndex = 0;

	// 動的に戻す
	SetEditMode(false);
}
//=============================================================================
// 通過ポイント追加処理
//=============================================================================
void CRockBlock::AddPathPoint(const D3DXVECTOR3& point)
{
	m_pathPoints.push_back(point);
}
//=============================================================================
// 目標に向かって移動する処理
//=============================================================================
void CRockBlock::MoveToTarget(void)
{
	bool allTargetHit = true;

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_TARGET)
		{
			continue;
		}

		CTargetBlock* pTarget = dynamic_cast<CTargetBlock*>(block);

		if (pTarget && !pTarget->IsHit())
		{
			allTargetHit = false;
			break;
		}
	}

	// ターゲットをすべて達成したら音を止める
	if (allTargetHit && m_playedRollSoundID != -1)
	{
		CManager::GetSound()->Stop(m_playedRollSoundID);
		m_playedRollSoundID = -1;
	}

	if (m_pathPoints.empty() || m_currentTargetIndex >= (int)m_pathPoints.size())
	{
		return;
	}

	btRigidBody* pRigid = GetRigidBody();

	if (!pRigid)
	{
		return;
	}

	if (m_currentTargetIndex >= 1)
	{
		m_particleTimer++;

		if (m_particleTimer >= ROLL_PARTICLE_INTERVAL)
		{
			m_particleTimer = 0;

			// パーティクルの生成
			CParticle::Create(INIT_VEC3,
				D3DXVECTOR3(GetPos().x, GetPos().y - 200.0f, GetPos().z),
				D3DXCOLOR(0.4f, 0.4f, 0.4f, 0.4f),
				30,						// 寿命
				CParticle::TYPE_WATER,  // パーティクルタイプ
				5);						// 数
		}
	}

	D3DXVECTOR3 currentPos = GetPos();
	D3DXVECTOR3 targetPos = m_pathPoints[m_currentTargetIndex];

	// ターゲット方向ベクトル
	D3DXVECTOR3 dir = targetPos - currentPos;
	float dist = D3DXVec3Length(&dir);

	if (dist < 100.0f)  // ある程度近づいたら次のポイントへ
	{
		m_currentTargetIndex++;
		return;
	}

	// 正規化
	D3DXVec3Normalize(&dir, &dir);

	btVector3 force(0.0f, 0.0f, 0.0f);

	if (m_currentTargetIndex >= 1)
	{
		m_isThrough = true;
	}
	else
	{
		m_isThrough = false;
	}

	bool isThroughNow = m_isThrough;

	if (isThroughNow && !m_isPrevThrough)
	{
		// ループ再生してIDを保存
		if (m_playedRollSoundID == -1)
		{
			m_playedRollSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_ROLL, currentPos, 850.0f, 1650.0f);
		}
	}

	m_isPrevThrough = isThroughNow;


	if (m_currentTargetIndex >= 6 && !m_isBridgeSwitchOn)
	{// インデックスが 6 を超えたら
		float fSpeedDown = 0.001f;

		// 減速させる
		force = btVector3(dir.x * (m_speed * fSpeedDown), 0.0f, dir.z * (m_speed * fSpeedDown));

		// スイッチがまだ押されていないときだけ止める
		if (!m_isBridgeSwitchOn && m_currentTargetIndex >= 8 && m_playedRollSoundID != -1)
		{
			// 転がる音の停止
			CManager::GetSound()->Stop(m_playedRollSoundID);
			m_playedRollSoundID = -1;
		}
	}
	else
	{// 通常
		// Z軸中心で転がす
		force = btVector3(dir.x * m_speed, 0.0f, dir.z * m_speed);
	}

	// 音源の位置更新（再生中のみ）
	if (m_playedRollSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedRollSoundID, currentPos);
	}

	// 適用中の速度に加える
	pRigid->applyCentralForce(force);
}
//=============================================================================
// プレイヤーとの接触判定処理
//=============================================================================
void CRockBlock::IsPlayerHit(void)
{
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	CPlayer* pPlayer = CGame::GetPlayer();

	D3DXVECTOR3 playerPos = pPlayer->GetPos();
	D3DXVECTOR3 rockPos = GetPos();

	float playerRadius = pPlayer->GetRadius();  // プレイヤーの当たり判定半径
	float rockRadius = 240.0f;					// 岩の半径

	float playerHeight = pPlayer->GetHeight();	// プレイヤーの高さ
	float rockHeight = 240.0f;					// 岩の高さ範囲

	// XZ距離チェック
	D3DXVECTOR2 diffXZ = D3DXVECTOR2(playerPos.x - rockPos.x, playerPos.z - rockPos.z);

	float distXZSq = D3DXVec2LengthSq(&diffXZ);
	float hitDistXZ = playerRadius + rockRadius;

	// Y差チェック
	float dy = fabsf(playerPos.y - rockPos.y);
	float hitHeight = (playerHeight * 0.5f) + rockHeight;

	bool isNowHit = m_isHit;

	if (isNowHit && !m_isPrevHit)
	{
		// プレイヤーヒットSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_HIT);
	}

	// プレイヤーが当たっていたかを記録
	m_isPrevHit = isNowHit;

	if (distXZSq < (hitDistXZ * hitDistXZ) && dy < hitHeight)
	{
		m_isHit = true;

		// プレイヤーのリスポーン
		pPlayer->RespawnToCheckpoint(D3DXVECTOR3(2810.0f, 30.0f, -1518.0f));
	}
	else
	{
		m_isHit = false;
	}
}


//=============================================================================
// 橋ブロックのコンストラクタ
//=============================================================================
CBridgeBlock::CBridgeBlock()
{
	// 値のクリア
}
//=============================================================================
// 橋ブロックのデストラクタ
//=============================================================================
CBridgeBlock::~CBridgeBlock()
{
	// なし
}
//=============================================================================
// 橋ブロックの更新処理
//=============================================================================
void CBridgeBlock::Update(void)
{
	CBlock::Update();// 共通処理

	Move();
}
//=============================================================================
// 橋ブロックの移動処理
//=============================================================================
void CBridgeBlock::Move(void)
{
	// 制御スイッチが存在するか確認
	std::vector<CBlock*> blocks = CBlockManager::GetAllBlocks();

	for (CBlock* block : blocks)
	{
		if (block->GetType() != TYPE_SWITCH2)
		{
			continue;
		}

		CBridgeSwitchBlock* pSwitch = dynamic_cast<CBridgeSwitchBlock*>(block);

		if (!pSwitch || !pSwitch->IsSwitchOn())
		{
			continue;
		}

		// 現在位置取得
		D3DXVECTOR3 pos = GetPos();

		// 移動目標座標
		const float targetX = -1630.0f;

		// まだ目標位置に達してないなら移動
		if (pos.x > targetX)
		{
			const float speed = 2.0f; // 速度

			// 到達チェック：次のフレームで通り過ぎるか
			if (pos.x - speed <= targetX)
			{
				pos.x = targetX;
			}
			else
			{
				pos.x -= speed;
			}

			SetPos(pos);

			// コライダーの更新
			UpdateCollider();
		}
	}
}


//=============================================================================
// ターゲットブロックのコンストラクタ
//=============================================================================
CTargetBlock::CTargetBlock()
{
	// 値のクリア
	m_isHit = false;
	m_isPrevHit = false;
}
//=============================================================================
// ターゲットブロックのデストラクタ
//=============================================================================
CTargetBlock::~CTargetBlock()
{
	// なし
}
//=============================================================================
// ターゲットブロックの更新処理
//=============================================================================
void CTargetBlock::Update(void)
{
	CBlock::Update();// 共通処理

	if (m_isHit)
	{
		return; // すでに当たっていたらスキップ
	}

	// 自分の AABB を取得
	D3DXVECTOR3 myPos = GetPos();
	D3DXVECTOR3 mymodelSize = GetModelSize(); // 元のサイズ
	D3DXVECTOR3 myscale = GetSize();// 拡大率
	D3DXVECTOR3 mySize;

	// 拡大率を適用する
	mySize.x = mymodelSize.x * myscale.x;
	mySize.y = mymodelSize.y * myscale.y;
	mySize.z = mymodelSize.z * myscale.z;

	D3DXVECTOR3 myMin = myPos - mySize * 0.5f;
	D3DXVECTOR3 myMax = myPos + mySize * 0.5f;

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block == this || block->GetType() != TYPE_ROCK)
		{
			continue; // 自分 or 岩ブロック以外は無視
		}

		// 岩の AABB を取得
		D3DXVECTOR3 otherPos = block->GetPos();
		D3DXVECTOR3 otherModelSize = block->GetModelSize();
		D3DXVECTOR3 otherScale = block->GetSize();
		D3DXVECTOR3 otherSize(
			otherModelSize.x * otherScale.x,
			otherModelSize.y * otherScale.y,
			otherModelSize.z * otherScale.z
		);

		D3DXVECTOR3 otherMin = otherPos - otherSize * 0.5f;
		D3DXVECTOR3 otherMax = otherPos + otherSize * 0.5f;

		// AABB同士の交差チェック
		bool isOverlap =
			myMin.x <= otherMax.x && myMax.x >= otherMin.x &&
			myMin.y <= otherMax.y && myMax.y >= otherMin.y &&
			myMin.z <= otherMax.z && myMax.z >= otherMin.z;

		bool isNowHit = isOverlap;

		if (isNowHit && !m_isPrevHit)
		{
			// 岩ヒットSE
			CManager::GetSound()->Play(CSound::SOUND_LABEL_ROCKHIT);
		}

		m_isPrevHit = isNowHit;

		if (isOverlap)
		{
			if (!m_isPrevHit)
			{
				CManager::GetSound()->Play(CSound::SOUND_LABEL_ROCKHIT);
			}

			m_isHit = true;
			m_isPrevHit = true;
			CManager::GetSound()->Stop(CSound::SOUND_LABEL_ROLL);
			break;
		}
		else
		{
			m_isPrevHit = false;
		}
	}
}


//=============================================================================
// 壁掛け松明ブロックのコンストラクタ
//=============================================================================
CTorchBlock::CTorchBlock()
{
	// 値のクリア
	m_playedSoundID = -1;
}
//=============================================================================
// 壁掛け松明ブロックのデストラクタ
//=============================================================================
CTorchBlock::~CTorchBlock()
{
	// なし
}
//=============================================================================
// 壁掛け松明ブロックの更新処理
//=============================================================================
void CTorchBlock::Update(void)
{
	CBlock::Update();// 共通処理

	if (CManager::GetMode() == MODE_GAME && CGame::GetPlayer())
	{
		D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
		D3DXVECTOR3 disPos = playerPos - GetPos();

		float distance = D3DXVec3Length(&disPos);

		// オフセット
		D3DXVECTOR3 localOffset(0.0f, 30.0f, -8.0f); // 松明の先端（ローカル）
		D3DXVECTOR3 worldOffset;

		// ブロックのワールドマトリックスを取得
		D3DXMATRIX worldMtx = GetWorldMatrix();

		D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

		const float kTriggerDistance = 780.0f; // 反応距離

		if (distance < kTriggerDistance)
		{
			// パーティクル生成
			CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 16, CParticle::TYPE_FIRE, 1);
			CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_FIRE, 1);

			if (m_playedSoundID == -1) // 再生していなければ再生開始
			{
				// 前の音を止める（念のため）
				CManager::GetSound()->Stop(m_playedSoundID);

				// 3Dサウンド再生してIDを保持
				m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
			}

		}
		else
		{
			// 離れたら音停止してIDリセット
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->Stop(m_playedSoundID);
				m_playedSoundID = -1;
			}
		}

		// 音源の位置更新はIDを使う
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, worldOffset);
		}
	}

	if (CManager::GetMode() == MODE_TITLE)
	{
		// オフセット
		D3DXVECTOR3 localOffset(0.0f, 30.0f, -8.0f); // 松明の先端（ローカル）
		D3DXVECTOR3 worldOffset;

		// ブロックのワールドマトリックスを取得
		D3DXMATRIX worldMtx = GetWorldMatrix();

		D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

		// パーティクル生成
		CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_FIRE, 1);
		CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_FIRE, 1);
	}
}


//=============================================================================
// 置き型松明ブロックのコンストラクタ
//=============================================================================
CTorch2Block::CTorch2Block()
{
	// 値のクリア
	m_playedSoundID = -1;
}
//=============================================================================
// 置き型松明ブロックのデストラクタ
//=============================================================================
CTorch2Block::~CTorch2Block()
{
	// なし
}
//=============================================================================
// 置き型松明ブロックの更新処理
//=============================================================================
void CTorch2Block::Update(void)
{
	if (CManager::GetMode() != MODE_GAME || !CGame::GetPlayer())
	{
		return;
	}

	CBlock::Update(); // 共通処理

	CParticle* pParticle = NULL;
	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	// オフセット
	D3DXVECTOR3 localOffset(0.0f, 30.0f, 0.0f); // 松明の先端（ローカル）
	D3DXVECTOR3 worldOffset;

	// ブロックのワールドマトリックスを取得
	D3DXMATRIX worldMtx = GetWorldMatrix();

	D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

	const float kTriggerDistance = 1080.0f; // 反応距離

	if (distance < kTriggerDistance)
	{
		// パーティクル生成
		pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_FIRE, 1);
		pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_FIRE, 1);

		if (m_playedSoundID == -1) // 再生していなければ再生開始
		{
			// 前の音を止める（念のため）
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3Dサウンド再生してIDを保持
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
		}

	}
	else
	{
		// 離れたら音停止してIDリセット
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	// 音源の位置更新はIDを使う
	if (m_playedSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, worldOffset);
	}
}


//=============================================================================
// 置き型松明(動かない)ブロックのコンストラクタ
//=============================================================================
CTorch3Block::CTorch3Block()
{
	// 値のクリア
	m_playedSoundID = -1;
	m_isHit = false;
}
//=============================================================================
// 置き型松明(動かない)ブロックのデストラクタ
//=============================================================================
CTorch3Block::~CTorch3Block()
{
	// なし
}
//=============================================================================
// 置き型松明(動かない)ブロックの更新処理
//=============================================================================
void CTorch3Block::Update(void)
{
	if (CManager::GetMode() != MODE_GAME || !CGame::GetPlayer())
	{
		return;
	}

	CBlock::Update(); // 共通処理

	CParticle* pParticle = NULL;
	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	// オフセット
	D3DXVECTOR3 localOffset(0.0f, 30.0f, 0.0f); // 松明の先端（ローカル）
	D3DXVECTOR3 worldOffset;

	// ブロックのワールドマトリックスを取得
	D3DXMATRIX worldMtx = GetWorldMatrix();

	D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

	const float kTriggerDistance = 1180.0f; // 反応距離

	if (distance < kTriggerDistance && m_isHit)
	{
		// パーティクル生成
		pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_FIRE, 1);
		pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_FIRE, 1);

		if (m_playedSoundID == -1) // 再生していなければ再生開始
		{
			// 前の音を止める（念のため）
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3Dサウンド再生してIDを保持
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
		}
	}
	else
	{
		// 離れたら音停止してIDリセット
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	// 音源の位置更新はIDを使う
	if (m_playedSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, worldOffset);
	}
}


//=============================================================================
// 仮面ブロックのコンストラクタ
//=============================================================================
CMaskBlock::CMaskBlock()
{
	// 値のクリア
	m_isGet = false;
	m_playedSoundID = -1;
}
//=============================================================================
// 仮面ブロックのデストラクタ
//=============================================================================
CMaskBlock::~CMaskBlock()
{
	// なし
}
//=============================================================================
// 仮面ブロックの更新処理
//=============================================================================
void CMaskBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	if (!CGame::GetPlayer())
	{
		return;
	}

	CParticle* pParticle = NULL;

	if (CManager::GetMode() == MODE_GAME)
	{
		D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
		D3DXVECTOR3 disPos = playerPos - GetPos();

		float distance = D3DXVec3Length(&disPos);

		const float kTriggerDistance = 1100.0f; // 反応距離

		if (distance < kTriggerDistance && !m_isGet)
		{
			// オフセット
			D3DXVECTOR3 localOffset(0.0f, 15.0f, 0.0f);
			D3DXVECTOR3 worldOffset;

			// ブロックのワールドマトリックスを取得
			D3DXMATRIX worldMtx = GetWorldMatrix();

			D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

			// パーティクル生成
			pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.6f, 0.6f, 1.0f, 0.3f), 50, CParticle::TYPE_AURA2, 1);

			if (m_playedSoundID == -1) // 再生していなければ再生開始
			{
				// 前の音を止める（念のため）
				CManager::GetSound()->Stop(m_playedSoundID);

				// 3Dサウンド再生してIDを保持
				m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_MASK, GetPos(), 250.0f, kTriggerDistance);
			}

			// 音源の位置更新はIDを使う
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
			}
		}
		else
		{
			// 離れたら音停止してIDリセット
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->Stop(m_playedSoundID);
				m_playedSoundID = -1;
			}
		}

		const float getDistance = 200.0f; // 反応距離

		if (distance < getDistance)
		{
			if (!m_isGet)
			{
				// 秘宝取得UIの生成
				CUi::Create(CUi::TYPE_GET, "data/TEXTURE/ui_mask.png", D3DXVECTOR3(900.0f, 220.0f, 0.0f), 320.0f, 140.0f);
			}

			m_isGet = true;

			// リスポーン処理
			CGame::GetPlayer()->RespawnToCheckpoint();
		}
	}
}


//=============================================================================
// 剣ブロックのコンストラクタ
//=============================================================================
CSwordBlock::CSwordBlock()
{
	// 値のクリア
	m_isEnd = false;
	m_playedSoundID = -1;
}
//=============================================================================
// 剣ブロックのデストラクタ
//=============================================================================
CSwordBlock::~CSwordBlock()
{
	// なし
}
//=============================================================================
// 剣ブロックの更新処理
//=============================================================================
void CSwordBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	if (!CGame::GetPlayer())
	{
		return;
	}

	CParticle* pParticle = NULL;

	if (CManager::GetMode() == MODE_GAME)
	{
		D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
		D3DXVECTOR3 disPos = playerPos - GetPos();

		float distance = D3DXVec3Length(&disPos);

		const float kTriggerDistance = 1280.0f; // 反応距離

		if (distance < kTriggerDistance)
		{
			// オフセット
			D3DXVECTOR3 localOffset(0.0f, -60.0f, 0.0f);
			D3DXVECTOR3 worldOffset;

			// ブロックのワールドマトリックスを取得
			D3DXMATRIX worldMtx = GetWorldMatrix();

			D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

			// パーティクル生成
			pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.6f, 0.6f, 0.0f, 0.3f), 50, CParticle::TYPE_AURA, 1);

			if (m_playedSoundID == -1) // 再生していなければ再生開始
			{
				// 前の音を止める（念のため）
				CManager::GetSound()->Stop(m_playedSoundID);

				// 3Dサウンド再生してIDを保持
				m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_TREASURE, GetPos(), 250.0f, kTriggerDistance);
			}

			// 音源の位置更新はIDを使う
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
			}
		}
		else
		{
			// 離れたら音停止してIDリセット
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->Stop(m_playedSoundID);
				m_playedSoundID = -1;
			}
		}

		const float getDistance = 250.0f; // 反応距離

		if (distance < getDistance)
		{// 手に入れた
			m_isEnd = true;
		}
	}
}


//=============================================================================
// 鉄格子ブロックのコンストラクタ
//=============================================================================
CBarBlock::CBarBlock()
{
	// 値のクリア
}
//=============================================================================
// 鉄格子ブロックのデストラクタ
//=============================================================================
CBarBlock::~CBarBlock()
{
	// なし
	m_isOpened = false;
}
//=============================================================================
// 鉄格子ブロックの更新処理
//=============================================================================
void CBarBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	if (!m_isOpened)
	{
		// 鉄格子制御スイッチブロックを探す
		for (CBlock* block : CBlockManager::GetAllBlocks())
		{
			if (block->GetType() != TYPE_SWITCH3)
			{
				continue;
			}

			CBarSwitchBlock* pBarSwitch = dynamic_cast<CBarSwitchBlock*>(block);

			// 押されていたら
			if (pBarSwitch && pBarSwitch->IsSwitchOn())
			{
				m_isOpened = true;
				break;
			}

		}
	}

	if (m_isOpened)
	{
		// ドアを開く
		D3DXVECTOR3 newPos = GetPos();

		if (newPos.y <= 290.0f)
		{
			newPos.y += 1.0f;
			SetPos(newPos);
		}
		else
		{
			m_isOpened = false;
		}
	}
	else
	{
		// ドアを閉める
		D3DXVECTOR3 newPos = GetPos();

		if (newPos.y > 90.0f && !m_isOpened)
		{
			newPos.y -= 1.0f;
			SetPos(newPos);
		}
	}
}


//=============================================================================
// 足場ブロックのコンストラクタ
//=============================================================================
CFootingBlock::CFootingBlock()
{
	// 値のクリア
	m_isMove = false;
}
//=============================================================================
// 足場ブロックのデストラクタ
//=============================================================================
CFootingBlock::~CFootingBlock()
{
	// なし
}
//=============================================================================
// 足場ブロックの更新処理
//=============================================================================
void CFootingBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	// 制御スイッチが存在するか確認
	std::vector<CBlock*> blocks = CBlockManager::GetAllBlocks();

	for (CBlock* block : blocks)
	{
		if (block->GetType() != TYPE_SWITCH3)
		{
			continue;
		}

		CBarSwitchBlock* pSwitch = dynamic_cast<CBarSwitchBlock*>(block);

		if (!pSwitch)
		{
			continue;
		}

		if (pSwitch->IsSwitchOn())
		{
			// 現在位置取得
			D3DXVECTOR3 pos = GetPos();

			// 移動目標座標
			const float targetX = 2957.0f;

			// まだ目標位置に達してないなら移動
			if (pos.x > targetX)
			{
				const float speed = 1.0f; // 速度

				// 到達チェック：次のフレームで通り過ぎるか
				if (pos.x - speed <= targetX)
				{
					pos.x = targetX;
				}
				else
				{
					pos.x -= speed;
					m_isMove = true;
				}

				SetPos(pos);
			}
		}
		else
		{
			CManager::GetSound()->Stop(CSound::SOUND_LABEL_TIMER);

			// 現在位置取得
			D3DXVECTOR3 pos = GetPos();

			// 移動目標座標
			const float targetX = 3160.0f;

			// まだ目標位置に達してないなら移動
			if (pos.x < targetX)
			{
				const float speed = 1.0f; // 速度

				// 到達チェック：次のフレームで通り過ぎるか
				if (pos.x - speed >= targetX)
				{
					pos.x = targetX;
					//m_isMove = false;
				}
				else
				{
					pos.x += speed;
				}

				SetPos(pos);
			}
			else
			{
				m_isMove = false;
			}
		}

		// コライダーの更新
		UpdateCollider();
	}
}


//=============================================================================
// 火炎放射像ブロックのコンストラクタ
//=============================================================================
CFireStatueBlock::CFireStatueBlock()
{
	// 値のクリア
	m_playedSoundID = -1;
	m_isBlocked = false;
}
//=============================================================================
// 火炎放射像ブロックのデストラクタ
//=============================================================================
CFireStatueBlock::~CFireStatueBlock()
{
	// なし
}
//=============================================================================
// 火炎放射像ブロックの更新処理
//=============================================================================
void CFireStatueBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	CPlayer* pPlayer = CGame::GetPlayer();

	if (!pPlayer)
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();
	float distance = D3DXVec3Length(&disPos);
	const float kTriggerDistance = 380.0f; // 反応距離

	// ブロックのワールドマトリックス
	D3DXMATRIX worldMtx = GetWorldMatrix();

	// ==== 音源位置====
	D3DXVECTOR3 localOffset(0.0f, -30.0f, -40.0f); // Z負方向にオフセット
	D3DXVECTOR3 worldPos;
	D3DXVec3TransformCoord(&worldPos, &localOffset, &worldMtx);

	if (distance < kTriggerDistance)
	{
		// パーティクルの設定
		SetParticle();

		if (m_playedSoundID == -1) // 再生していなければ再生開始
		{
			// 前の音を止める（念のため）
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3Dサウンド再生してIDを保持
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
		}
	}
	else
	{
		// 離れたら音停止してIDリセット
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	// 音源の位置更新はIDを使う
	if (m_playedSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, worldPos);
	}
}
//=============================================================================
// 火炎放射像ブロックのパーティクル設定処理
//=============================================================================
void CFireStatueBlock::SetParticle(void)
{
	CParticle* pParticle = NULL;

	// ブロックのワールドマトリックス
	D3DXMATRIX worldMtx = GetWorldMatrix();

	// ==== 前方向ベクトル（Z軸）====
	D3DXVECTOR3 localForward(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 forward;
	D3DXVec3TransformNormal(&forward, &localForward, &worldMtx);
	D3DXVec3Normalize(&forward, &forward);

	// ==== 発射位置（後側）====
	D3DXVECTOR3 localOffsetBack(0.0f, -30.0f, -40.0f); // Z負方向にオフセット
	D3DXVECTOR3 worldPosBack;
	D3DXVec3TransformCoord(&worldPosBack, &localOffsetBack, &worldMtx);

	// 元のシリンダーの判定サイズ
	float cylinderRadius = 60.0f;
	float maxCylinderHeight = 220.0f;

	// 判定用の中心位置は発射位置から軸方向に半分移動したところ（シリンダーの中心）
	float offsetDistance = maxCylinderHeight * 0.5f;
	D3DXVECTOR3 cylinderCenter = worldPosBack - forward * offsetDistance;

	// ブロックによる遮断判定
	float minHitDistance = maxCylinderHeight; // 縮める距離。最大長さスタート
	m_isBlocked = false;

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_BLOCK3 && block->GetType() != TYPE_BLOCK2)
		{
			continue;
		}


		D3DXVECTOR3 blockPos = block->GetPos();
		D3DXVECTOR3 modelSize = block->GetModelSize();// 元のサイズ
		D3DXVECTOR3 scale = GetSize();// 拡大率

		// 拡大率を適用する
		D3DXVECTOR3 blockSize;
		blockSize.x = modelSize.x * scale.x;
		blockSize.y = modelSize.y * scale.y;
		blockSize.z = modelSize.z * scale.z;

		D3DXVECTOR3 aabbMin = blockPos - blockSize * 0.5f;
		D3DXVECTOR3 aabbMax = blockPos + blockSize * 0.5f;

		// ここでシリンダー（最大長さ）とAABBの当たり判定をし、
		// 当たっていたら当たり面までの距離を計算
		float hitDistance = 0.0f;
		if (CCollision::CheckCylinderAABBCollisionWithHitDistance(cylinderCenter, cylinderRadius, maxCylinderHeight, forward, aabbMin, aabbMax, &hitDistance))
		{
			m_isBlocked = true;
			if (hitDistance < minHitDistance)
			{
				minHitDistance = hitDistance;
			}
		}
	}

	// シリンダーの長さを縮める（最小の当たり距離）
	float cylinderHeight = m_isBlocked ? minHitDistance : maxCylinderHeight;

	// 中心位置も調整
	offsetDistance = cylinderHeight * 0.5f;
	cylinderCenter = worldPosBack - forward * offsetDistance;

	// パーティクル生成（縮めた長さに合わせて）
	pParticle = CParticle::Create(-forward, worldPosBack, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 40, CParticle::TYPE_FLAMETHROWER, 5);

	// パーティクルの速度倍率セット（縮めた割合で速度を調整）
	if (pParticle)
	{
		auto flamethrowerParticle = dynamic_cast<CFlamethrowerParticle*>(pParticle);
		if (flamethrowerParticle)
		{
			// 速度スケールを発生位置基準の距離割合で単純計算
			float speedScale = clamp(minHitDistance / maxCylinderHeight, 0.0f, 1.0f);
			flamethrowerParticle->SetSpeedScale(speedScale);
		}
	}

	// プレイヤーの当たり判定も縮めた範囲で
	CPlayer* pPlayer = CGame::GetPlayer();
	D3DXVECTOR3 playerPos = pPlayer->GetColliderPos();
	float playerRadius = pPlayer->GetRadius();
	float playerHeight = pPlayer->GetHeight();

	if (CCollision::CheckCapsuleCylinderCollision_Dir(playerPos, playerRadius, playerHeight,
		cylinderCenter, cylinderRadius, cylinderHeight, forward, m_isBlocked))
	{
		// リスポーン処理
		pPlayer->RespawnToCheckpoint();
	}
}


//=============================================================================
// 火炎放射像(動く)ブロックのコンストラクタ
//=============================================================================
CMoveFireStatueBlock::CMoveFireStatueBlock()
{
	// 値のクリア
	m_playedSoundID = -1;
}
//=============================================================================
// 火炎放射像(動く)ブロックのデストラクタ
//=============================================================================
CMoveFireStatueBlock::~CMoveFireStatueBlock()
{
	// なし
}
//=============================================================================
// 火炎放射像(動く)ブロックの更新処理
//=============================================================================
void CMoveFireStatueBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	CPlayer* pPlayer = CGame::GetPlayer();

	if (!pPlayer)
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();
	float distance = D3DXVec3Length(&disPos);
	const float kTriggerDistance = 880.0f; // 反応距離

	// ブロックのワールドマトリックス
	D3DXMATRIX worldMtx = GetWorldMatrix();

	// ==== 音源位置====
	D3DXVECTOR3 localOffset(0.0f, -30.0f, -40.0f); // Z負方向にオフセット
	D3DXVECTOR3 worldPos;
	D3DXVec3TransformCoord(&worldPos, &localOffset, &worldMtx);

	if (distance < kTriggerDistance)
	{
		// パーティクルの設定
		SetParticle();

		if (m_playedSoundID == -1) // 再生していなければ再生開始
		{
			// 前の音を止める（念のため）
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3Dサウンド再生してIDを保持
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
		}
	}
	else
	{
		// 離れたら音停止してIDリセット
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	// 音源の位置更新はIDを使う
	if (m_playedSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, worldPos);
	}
}
//=============================================================================
// 火炎放射像(動く)ブロックのパーティクル設定処理
//=============================================================================
void CMoveFireStatueBlock::SetParticle(void)
{
	CParticle* pParticle = NULL;

	// ブロックのワールドマトリックス
	D3DXMATRIX worldMtx = GetWorldMatrix();

	// ==== 前方向ベクトル（Z軸）====
	D3DXVECTOR3 localForward(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 forward;
	D3DXVec3TransformNormal(&forward, &localForward, &worldMtx);
	D3DXVec3Normalize(&forward, &forward);

	// ==== 発射位置（後側）====
	D3DXVECTOR3 localOffsetBack(0.0f, -30.0f, -40.0f); // Z負方向にオフセット
	D3DXVECTOR3 worldPosBack;
	D3DXVec3TransformCoord(&worldPosBack, &localOffsetBack, &worldMtx);

	// 元のシリンダーの判定サイズ
	float cylinderRadius = 60.0f;
	float maxCylinderHeight = 220.0f;

	// 判定用の中心位置は発射位置から軸方向に半分移動したところ（シリンダーの中心）
	float offsetDistance = maxCylinderHeight * 0.5f;
	D3DXVECTOR3 cylinderCenter = worldPosBack - forward * offsetDistance;

	// 置き型松明(動かない)との当たり判定
	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_TORCH3)
		{
			continue;
		}

		CTorch3Block* pTorch3 = dynamic_cast<CTorch3Block*>(block);

		D3DXVECTOR3 blockPos = block->GetPos();
		D3DXVECTOR3 blockSize = block->GetModelSize();
		D3DXVECTOR3 aabbMin = blockPos - blockSize * 0.5f;
		D3DXVECTOR3 aabbMax = blockPos + blockSize * 0.5f;

		// 当たっていたらフラグをtrueにする
		if (CCollision::CheckCylinderAABBCollisionWithHitDistance(cylinderCenter, cylinderRadius, maxCylinderHeight, forward, aabbMin, aabbMax, NULL))
		{
			pTorch3->SetHit(true);
		}
	}

	// パーティクル生成
	pParticle = CParticle::Create(-forward, worldPosBack, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 40, CParticle::TYPE_FLAMETHROWER, 5);

	CPlayer* pPlayer = CGame::GetPlayer();
	D3DXVECTOR3 playerPos = pPlayer->GetColliderPos();  // カプセル中心位置
	float playerRadius = pPlayer->GetRadius();
	float playerHeight = pPlayer->GetHeight();

	// プレイヤーとの当たり判定
	if (CCollision::CheckCapsuleCylinderCollision_Dir(playerPos, playerRadius, playerHeight,
		cylinderCenter, cylinderRadius, maxCylinderHeight, forward, false))
	{
		// リスポーン処理
		pPlayer->RespawnToCheckpoint(D3DXVECTOR3(-1132.0f, 299.5f, 724.5f));
	}
}


//=============================================================================
// 火炎放射像(回転)ブロックのコンストラクタ
//=============================================================================
CTurnFireStatueBlock::CTurnFireStatueBlock()
{
	// 値のクリア
	m_playedSoundID = -1;
}
//=============================================================================
// 火炎放射像(回転)ブロックのデストラクタ
//=============================================================================
CTurnFireStatueBlock::~CTurnFireStatueBlock()
{
	// なし
}
//=============================================================================
// 火炎放射像(回転)ブロックの更新処理
//=============================================================================
void CTurnFireStatueBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	CPlayer* pPlayer = CGame::GetPlayer();

	if (!pPlayer)
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();
	float distance = D3DXVec3Length(&disPos);
	const float kTriggerDistance = 720.0f; // 反応距離

	if (distance < kTriggerDistance)
	{
		// パーティクルの設定
		SetParticle();

		if (m_playedSoundID == -1) // 再生していなければ再生開始
		{
			// 前の音を止める（念のため）
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3Dサウンド再生してIDを保持
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
		}

		D3DXVECTOR3 rot = GetRot();

		rot.y += 0.01f;

		if (rot.y > D3DX_PI * 2.0f)
		{
			rot.y -= D3DX_PI * 2.0f;
		}

		SetRot(rot);
	}
	else
	{
		// 離れたら音停止してIDリセット
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	// 音源の位置更新はIDを使う
	if (m_playedSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
	}
}
//=============================================================================
// 火炎放射像(回転)ブロックのパーティクル設定処理
//=============================================================================
void CTurnFireStatueBlock::SetParticle(void)
{
	CParticle* pParticle = NULL;

	// ブロックのワールドマトリックス
	D3DXMATRIX worldMtx = GetWorldMatrix();

	// ==== 前方向ベクトル（Z軸）====
	D3DXVECTOR3 localForward(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 forward;
	D3DXVec3TransformNormal(&forward, &localForward, &worldMtx);
	D3DXVec3Normalize(&forward, &forward);

	// ==== 発射位置（前側）====
	D3DXVECTOR3 localOffsetFront(0.0f, -30.0f, 40.0f); // Z正方向にオフセット
	D3DXVECTOR3 worldPosFront;
	D3DXVec3TransformCoord(&worldPosFront, &localOffsetFront, &worldMtx);

	// ==== 発射位置（後側）====
	D3DXVECTOR3 localOffsetBack(0.0f, -30.0f, -40.0f); // Z負方向にオフセット
	D3DXVECTOR3 worldPosBack;
	D3DXVec3TransformCoord(&worldPosBack, &localOffsetBack, &worldMtx);

	// 元のシリンダーの判定サイズ
	float cylinderRadius = 50.0f;
	float maxCylinderHeight = 200.0f;

	// 判定用の中心位置は発射位置から軸方向に半分移動したところ（シリンダーの中心）
	float offsetDistanceFront = maxCylinderHeight * 0.5f;
	float offsetDistanceBack = maxCylinderHeight * 0.5f;
	D3DXVECTOR3 cylinderCenterFront = worldPosFront + forward * offsetDistanceFront;
	D3DXVECTOR3 cylinderCenterBack = worldPosBack - forward * offsetDistanceBack;

	// パーティクル生成
	pParticle = CParticle::Create(forward, worldPosFront, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 40, CParticle::TYPE_FLAMETHROWER, 5);
	pParticle = CParticle::Create(-forward, worldPosBack, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 40, CParticle::TYPE_FLAMETHROWER, 5);

	CPlayer* pPlayer = CGame::GetPlayer();
	D3DXVECTOR3 playerPos = pPlayer->GetColliderPos();  // カプセル中心位置
	float playerRadius = pPlayer->GetRadius();
	float playerHeight = pPlayer->GetHeight();

	// プレイヤーとの当たり判定(前側)
	if (CCollision::CheckCapsuleCylinderCollision_Dir(playerPos, playerRadius, playerHeight,
		cylinderCenterFront, cylinderRadius, maxCylinderHeight, forward, false))
	{
		pPlayer->RespawnToCheckpoint();
	}

	// プレイヤーとの当たり判定(後側)
	if (CCollision::CheckCapsuleCylinderCollision_Dir(playerPos, playerRadius, playerHeight,
		cylinderCenterBack, cylinderRadius, maxCylinderHeight, forward, false))
	{
		pPlayer->RespawnToCheckpoint();
	}
}


//=============================================================================
// 鍵の柵ブロックのコンストラクタ
//=============================================================================
CKeyFenceBlock::CKeyFenceBlock()
{
	// 値のクリア
	m_closedPos = INIT_VEC3;
	m_prevDown = false;
}
//=============================================================================
// 鍵の柵ブロックのデストラクタ
//=============================================================================
CKeyFenceBlock::~CKeyFenceBlock()
{
	// なし
}
//=============================================================================
// 鍵の柵ブロックの初期化処理
//=============================================================================
HRESULT CKeyFenceBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	m_closedPos = GetPos();

	return S_OK;
}
//=============================================================================
// 鍵の柵ブロックの更新処理
//=============================================================================
void CKeyFenceBlock::Update()
{
	CBlock::Update(); // 共通処理

	if (CBlockManager::CheckAllTorches())
	{// 全ての松明に火が付いたら

		const float kDownRange = 190.0f; // 下がる深さ

		float targetY = m_closedPos.y - kDownRange;

		D3DXVECTOR3 pos = GetPos();
		if (pos.y > targetY)
		{
			pos.y = max(pos.y - 1.0f, targetY);
			SetPos(pos);
		}

		bool n = CBlockManager::CheckAllTorches();

		if (n && !m_prevDown)
		{
			// 演出カメラにする
			CManager::GetCamera()->SetCamMode(5, D3DXVECTOR3(-1571.0f, 644.5f, 820.5f),
				D3DXVECTOR3(-2180.5f, 181.0f, 589.0f),
				D3DXVECTOR3(0.62f, 1.21f, 0.0f));
		}

		// フラグを更新して次のフレームに備える
		m_prevDown = n;
	}
}


//=============================================================================
// 鍵ブロックのコンストラクタ
//=============================================================================
CKeyBlock::CKeyBlock()
{
	// 値のクリア
	m_ResPos = INIT_VEC3;
}
//=============================================================================
// 鍵ブロックのデストラクタ
//=============================================================================
CKeyBlock::~CKeyBlock()
{
	// なし
}
//=============================================================================
// 鍵ブロックの初期化処理
//=============================================================================
HRESULT CKeyBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	// 最初の位置をリスポーン位置に設定
	m_ResPos = GetPos();

	// 動的に戻す
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// 鍵ブロックの更新処理
//=============================================================================
void CKeyBlock::Update()
{
	CBlock::Update(); // 共通処理
}
//=============================================================================
// セット処理
//=============================================================================
void CKeyBlock::Set(D3DXVECTOR3 pos)
{
	// 動かすためにキネマティックにする
	SetEditMode(true);

	// 鍵ブロックの位置を取得
	D3DXVECTOR3 keyPos = GetPos();
	D3DXVECTOR3 keyRot = GetRot();

	D3DXVECTOR3 targetPos(pos);			// 設置する目標位置
	D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);	// 向きをリセット

	keyPos = targetPos;
	keyRot = rot;

	SetPos(keyPos);
	SetRot(keyRot);

	// コライダーの更新
	UpdateCollider();
}


//=============================================================================
// 鍵の台座ブロックのコンストラクタ
//=============================================================================
CKeyPedestalBlock::CKeyPedestalBlock()
{
	// 値のクリア
	m_Pos = INIT_VEC3;
	m_isSet = false;
	m_prevIsSet = false;
}
//=============================================================================
// 鍵の台座ブロックのデストラクタ
//=============================================================================
CKeyPedestalBlock::~CKeyPedestalBlock()
{
	// なし
}
//=============================================================================
// 鍵の台座ブロックの初期化処理
//=============================================================================
HRESULT CKeyPedestalBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	// 最初の位置をリスポーン位置に設定
	m_Pos = GetPos();

	return S_OK;
}
//=============================================================================
// 鍵の台座ブロックの更新処理
//=============================================================================
void CKeyPedestalBlock::Update()
{
	CBlock::Update(); // 共通処理

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_KEY)
		{
			continue;
		}

		CKeyBlock* pKey = dynamic_cast<CKeyBlock*>(block);

		D3DXVECTOR3 keyPos = block->GetPos();
		D3DXVECTOR3 disPos = keyPos - GetPos();
		float distance = D3DXVec3Length(&disPos);
		const float kTriggerDistance = 130.0f; // 反応距離

		if (distance < kTriggerDistance)
		{// 鍵を台座にはめる
			D3DXVECTOR3 targetPos(GetPos().x, GetPos().y + 40.0f, GetPos().z);
			pKey->Set(targetPos);
			m_isSet = true;
		}

		bool n = m_isSet;

		if (n && !m_prevIsSet)
		{
			// 台座のサウンド再生
			CManager::GetSound()->Play(CSound::SOUND_LABEL_SHINE);
		}

		m_prevIsSet = n;
	}
}


//=============================================================================
// 鍵ドアブロックのコンストラクタ
//=============================================================================
CKeyDoorBlock::CKeyDoorBlock()
{
	// 値のクリア
	m_openPos = INIT_VEC3;
	m_isGoal = false;	// 目標位置に移動したか
	m_isSet = false;	// 設置されたか
	m_deleyTime = 120;	// 開くまでの遅延時間
	prevIsSet = false;	// 直前に設置したか
}
//=============================================================================
// 鍵ドアブロックのデストラクタ
//=============================================================================
CKeyDoorBlock::~CKeyDoorBlock()
{
	// なし
}
//=============================================================================
// 鍵ドアブロックの初期化処理
//=============================================================================
HRESULT CKeyDoorBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	// 最初の位置を保存
	m_openPos = GetPos();

	return S_OK;
}
//=============================================================================
// 鍵ドアブロックの更新処理
//=============================================================================
void CKeyDoorBlock::Update()
{
	CBlock::Update(); // 共通処理

	if (m_isGoal)
	{// 処理を通さない
		return;
	}

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_KEY_PEDESTAL)
		{// 台座じゃなかったら
			continue;
		}

		CKeyPedestalBlock* pPedestal = dynamic_cast<CKeyPedestalBlock*>(block);

		if (pPedestal->IsSet())
		{// 台座に設置されていたら
			m_isSet = true;
		}
	}

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_DOOR_TRIGGER)
		{// トリガーブロックじゃなかったら
			continue;
		}

		CDoorTriggerBlock* pTrigger = dynamic_cast<CDoorTriggerBlock*>(block);

		if (pTrigger->IsSet())
		{// 設置されていたら
			m_isSet = true;
		}
	}

	if (m_isSet)
	{
		m_deleyTime--;

		if (m_deleyTime > 0)
		{
			return;
		}

		bool n = m_isSet;

		if (n && !prevIsSet)
		{// 一回だけ通す
			// ドアSEの再生
			CManager::GetSound()->Play(CSound::SOUND_LABEL_DOOR);
		}

		prevIsSet = n;

		// ドアを開ける
		const float kOpenRange = 210.0f; // 開く高さ

		float targetY = m_openPos.y + kOpenRange;

		D3DXVECTOR3 pos = GetPos();
		if (pos.y < targetY)
		{
			pos.y += 1.0f;
			SetPos(pos);
		}
		else
		{// 目標位置に到達した
			m_isGoal = true;
		}
	}
}


//=============================================================================
// 盾ブロックのコンストラクタ
//=============================================================================
CShieldBlock::CShieldBlock()
{
	// 値のクリア
	m_isEnd = false;
	m_playedSoundID = -1;
}
//=============================================================================
// 盾ブロックのデストラクタ
//=============================================================================
CShieldBlock::~CShieldBlock()
{
	// なし
}
//=============================================================================
// 盾ブロックの更新処理
//=============================================================================
void CShieldBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	if (!CGame::GetPlayer())
	{
		return;
	}

	CParticle* pParticle = NULL;

	if (CManager::GetMode() == MODE_GAME)
	{
		D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
		D3DXVECTOR3 disPos = playerPos - GetPos();

		float distance = D3DXVec3Length(&disPos);

		const float kTriggerDistance = 780.0f; // 反応距離

		if (distance < kTriggerDistance)
		{
			// オフセット
			D3DXVECTOR3 localOffset(0.0f, -60.0f, 0.0f);
			D3DXVECTOR3 worldOffset;

			// ブロックのワールドマトリックスを取得
			D3DXMATRIX worldMtx = GetWorldMatrix();

			D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

			// パーティクル生成
			pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.6f, 0.6f, 0.0f, 0.3f), 50, CParticle::TYPE_AURA, 1);

			if (m_playedSoundID == -1) // 再生していなければ再生開始
			{
				// 前の音を止める（念のため）
				CManager::GetSound()->Stop(m_playedSoundID);

				// 3Dサウンド再生してIDを保持
				m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_TREASURE, GetPos(), 250.0f, kTriggerDistance);
			}

			// 音源の位置更新はIDを使う
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
			}
		}
		else
		{
			// 離れたら音停止してIDリセット
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->Stop(m_playedSoundID);
				m_playedSoundID = -1;
			}
		}

		const float getDistance = 250.0f; // 反応距離

		if (distance < getDistance)
		{// 手に入れた
			m_isEnd = true;
		}
	}
}


//=============================================================================
// 石像ブロックのコンストラクタ
//=============================================================================
CStatueBlock::CStatueBlock()
{
	// 値のクリア
	m_playedSoundID = -1;
}
//=============================================================================
// 石像ブロックのデストラクタ
//=============================================================================
CStatueBlock::~CStatueBlock()
{
	// なし
}
//=============================================================================
// 石像ブロックの更新処理
//=============================================================================
void CStatueBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	if (!CGame::GetPlayer())
	{
		return;
	}

	CParticle* pParticle = NULL;

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 780.0f; // 反応距離

	if (distance < kTriggerDistance)
	{
		// オフセット
		D3DXVECTOR3 localOffset(0.0f, -140.0f, 0.0f);
		D3DXVECTOR3 worldOffset;

		// ブロックのワールドマトリックスを取得
		D3DXMATRIX worldMtx = GetWorldMatrix();

		D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

		// パーティクル生成
		pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_STATUE_FIRE, 1);
		pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_STATUE_FIRE, 1);

		if (m_playedSoundID == -1) // 再生していなければ再生開始
		{
			// 前の音を止める（念のため）
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3Dサウンド再生してIDを保持
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 250.0f, kTriggerDistance);
		}

		// 音源の位置更新はIDを使う
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
		}
	}
	else
	{
		// 離れたら音停止してIDリセット
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}
}


//=============================================================================
// 火をつけると動く石像ブロックのコンストラクタ
//=============================================================================
CStatueBlock2::CStatueBlock2()
{
	// 値のクリア
	m_startPos = INIT_VEC3;
	m_playedSoundID = -1;
	m_triggerDis = false;
	m_isMoving = false;
	m_hasTriggered = false;
}
//=============================================================================
// 火をつけると動く石像ブロックのデストラクタ
//=============================================================================
CStatueBlock2::~CStatueBlock2()
{
	// なし
}
//=============================================================================
// 火をつけると動く石像ブロックの初期化処理
//=============================================================================
HRESULT CStatueBlock2::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	// 最初の位置を保存
	m_startPos = GetPos();

	return S_OK;
}
//=============================================================================
// 火をつけると動く石像ブロックの更新処理
//=============================================================================
void CStatueBlock2::Update(void)
{
	CBlock::Update(); // 共通処理

	if (!CGame::GetPlayer())
	{
		return;
	}

	CParticle* pParticle = NULL;

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 680.0f; // 反応距離

	if (distance < kTriggerDistance)
	{
		// オフセット
		D3DXVECTOR3 localOffset1(90.0f, -200.0f, 0.0f);
		D3DXVECTOR3 localOffset2(-90.0f, -170.0f, 0.0f);
		D3DXVECTOR3 worldOffset1;
		D3DXVECTOR3 worldOffset2;

		// ブロックのワールドマトリックスを取得
		D3DXMATRIX worldMtx = GetWorldMatrix();

		D3DXVec3TransformCoord(&worldOffset1, &localOffset1, &worldMtx);
		D3DXVec3TransformCoord(&worldOffset2, &localOffset2, &worldMtx);

		// パーティクル生成
		pParticle = CParticle::Create(INIT_VEC3, worldOffset1, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_STATUE_FIRE, 1);
		pParticle = CParticle::Create(INIT_VEC3, worldOffset1, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_STATUE_FIRE, 1);

		if (m_triggerDis)
		{
			pParticle = CParticle::Create(INIT_VEC3, worldOffset2, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_STATUE_FIRE, 1);
			pParticle = CParticle::Create(INIT_VEC3, worldOffset2, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_STATUE_FIRE, 1);
		}

		if (m_playedSoundID == -1) // 再生していなければ再生開始
		{
			// 前の音を止める（念のため）
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3Dサウンド再生してIDを保持
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 250.0f, kTriggerDistance);
		}

		// 音源の位置更新はIDを使う
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
		}
	}
	else
	{
		// 離れたら音停止してIDリセット
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	// 移動処理
	Move();
}
//=============================================================================
// 火をつけると動く石像ブロックの移動処理
//=============================================================================
void CStatueBlock2::Move(void)
{
	D3DXVECTOR3 pos = GetPos();

	if (!m_hasTriggered) // まだトリガーしてない
	{
		// --- 松明との距離チェック ---
		float minDistance = FLT_MAX;
		bool torchFound = false;

		for (CBlock* block : CBlockManager::GetAllBlocks())
		{
			if (block->GetType() != TYPE_TORCH2)
			{// 置き型松明じゃなかったら
				continue;
			}

			D3DXVECTOR3 torchPos = block->GetPos();
			D3DXVECTOR3 disPos = torchPos - pos;

			float distance = D3DXVec3Length(&disPos);
			if (distance < minDistance)
			{
				minDistance = distance;
				torchFound = true;
			}


		}

		// 一定距離以内に松明があったら動かす
		const float kTriggerDistance = 210.0f; // 反応距離

		// 距離内ならトリガーON
		if (torchFound && minDistance < kTriggerDistance)
		{
			// ひらめきSE
			CManager::GetSound()->Play(CSound::SOUND_LABEL_INSPIRATION);

			m_triggerDis = true;
			m_isMoving = true;
			m_hasTriggered = true;
		}
	}

	// --- 移動処理 ---
	if (m_isMoving)
	{
		// 移動速度
		const float speed = 1.0f;

		// 動かす
		const float Range = 180.0f; // 動く距離

		float targetX = m_startPos.x + Range;

		if (pos.x < targetX)
		{
			pos.x += speed;
			SetPos(pos);
		}
	}
}


//=============================================================================
// 卵ブロックのコンストラクタ
//=============================================================================
CEggBlock::CEggBlock()
{
	// 値のクリア
	m_isGet = false;
	m_playedSoundID = -1;
}
//=============================================================================
// 卵ブロックのデストラクタ
//=============================================================================
CEggBlock::~CEggBlock()
{
	// なし
}
//=============================================================================
// 卵ブロックの更新処理
//=============================================================================
void CEggBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	if (!CGame::GetPlayer())
	{
		return;
	}

	CParticle* pParticle = NULL;

	if (CManager::GetMode() == MODE_GAME)
	{
		D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
		D3DXVECTOR3 disPos = playerPos - GetPos();

		float distance = D3DXVec3Length(&disPos);

		const float kTriggerDistance = 1100.0f; // 反応距離

		if (distance < kTriggerDistance && !m_isGet)
		{
			// オフセット
			D3DXVECTOR3 localOffset(0.0f, 15.0f, 0.0f);
			D3DXVECTOR3 worldOffset;

			// ブロックのワールドマトリックスを取得
			D3DXMATRIX worldMtx = GetWorldMatrix();

			D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

			// パーティクル生成
			pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.8f, 0.2f, 0.2f, 0.3f), 50, CParticle::TYPE_AURA2, 1);

			if (m_playedSoundID == -1) // 再生していなければ再生開始
			{
				// 前の音を止める（念のため）
				CManager::GetSound()->Stop(m_playedSoundID);

				// 3Dサウンド再生してIDを保持
				m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_MASK, GetPos(), 250.0f, kTriggerDistance);
			}

			// 音源の位置更新はIDを使う
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
			}
		}
		else
		{
			// 離れたら音停止してIDリセット
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->Stop(m_playedSoundID);
				m_playedSoundID = -1;
			}
		}

		const float getDistance = 250.0f; // 反応距離

		if (distance < getDistance)
		{
			if (!m_isGet)
			{
				// 秘宝取得UIの生成
				CUi::Create(CUi::TYPE_GET, "data/TEXTURE/ui_egg.png", D3DXVECTOR3(900.0f, 220.0f, 0.0f), 320.0f, 140.0f);
			}

			m_isGet = true;
		}
	}
}


//=============================================================================
// ドアトリガーブロックのコンストラクタ
//=============================================================================
CDoorTriggerBlock::CDoorTriggerBlock()
{
	// 値のクリア
	m_Pos = INIT_VEC3;
	m_isSet = false;
	m_prevIsSet = false;
	m_isSetCam = false;
	m_nDeleyTime = 120;// カメラ移行までの遅延時間
}
//=============================================================================
// ドアトリガーブロックのデストラクタ
//=============================================================================
CDoorTriggerBlock::~CDoorTriggerBlock()
{
	// なし
}
//=============================================================================
// ドアトリガーブロックの初期化処理
//=============================================================================
HRESULT CDoorTriggerBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	// 最初の位置をリスポーン位置に設定
	m_Pos = GetPos();

	return S_OK;
}
//=============================================================================
// ドアトリガーブロックの更新処理
//=============================================================================
void CDoorTriggerBlock::Update()
{
	CBlock::Update(); // 共通処理

	if (m_isSet && !m_isSetCam)
	{
		m_nDeleyTime--;
	}

	if (m_nDeleyTime <= 0 && !m_isSetCam)
	{
		// 演出カメラにする
		CManager::GetCamera()->SetCamMode(3, D3DXVECTOR3(1405.0f, 170.0f, 300.0f),
			D3DXVECTOR3(1894.0f, 93.0f, 297.0f),
			D3DXVECTOR3(0.16f, -1.57f, 0.0f));

		m_nDeleyTime = 0;
		m_isSetCam = true;
	}

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_WOODBOX || m_isSet)
		{
			continue;
		}

		CWoodBoxBlock* pWoodBox = dynamic_cast<CWoodBoxBlock*>(block);

		D3DXVECTOR3 woodBoxPos = block->GetPos();
		D3DXVECTOR3 disPos = woodBoxPos - GetPos();
		float distance = D3DXVec3Length(&disPos);
		const float kTriggerDistance = 130.0f; // 反応距離

		if (distance < kTriggerDistance)
		{// 木箱を口にはめる
			// オフセット
			D3DXVECTOR3 localOffset(0.0f, -25.0f, -5.0f);
			D3DXVECTOR3 worldOffset;

			// ブロックのワールドマトリックスを取得
			D3DXMATRIX worldMtx = GetWorldMatrix();

			D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

			pWoodBox->Set(worldOffset);
			m_isSet = true;
		}

		bool n = m_isSet;

		if (n && !m_prevIsSet)
		{
			// 台座のサウンド再生
			CManager::GetSound()->Play(CSound::SOUND_LABEL_SHINE);
		}

		m_prevIsSet = n;
	}
}


//=============================================================================
// リスポーンブロックのコンストラクタ
//=============================================================================
CRespawnBlock::CRespawnBlock()
{
	// 値のクリア
}
//=============================================================================
// リスポーンブロックのデストラクタ
//=============================================================================
CRespawnBlock::~CRespawnBlock()
{
	// なし
}
//=============================================================================
// リスポーンブロックの更新処理
//=============================================================================
void CRespawnBlock::Update(void)
{
	// ブロックの更新処理
	CBlock::Update();

	// ブロックの AABB を取得
	D3DXVECTOR3 blockPos = GetPos();		// ブロックの位置
	D3DXVECTOR3 modelSize = GetModelSize(); // スイッチの元のサイズ（中心原点）
	D3DXVECTOR3 scale = GetSize();			// 拡大率

	// 拡大率を適用する
	D3DXVECTOR3 blockSize;
	blockSize.x = modelSize.x * scale.x;
	blockSize.y = modelSize.y * scale.y;
	blockSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 blockMin = blockPos - blockSize * 0.5f;
	D3DXVECTOR3 blockMax = blockPos + blockSize * 0.5f;

	// --- プレイヤー接触判定 ---
	CPlayer* pPlayer = CGame::GetPlayer();

	if (pPlayer)
	{
		D3DXVECTOR3 pPos = pPlayer->GetColliderPos(); // カプセルコライダー中心位置

		// カプセルコライダーのサイズからAABBサイズを計算
		float radius = pPlayer->GetRadius();
		float height = pPlayer->GetHeight();

		D3DXVECTOR3 pSize;
		pSize.x = radius * 2.0f;
		pSize.z = radius * 2.0f;
		pSize.y = height + radius * 2.0f;

		// AABB計算
		D3DXVECTOR3 pMin = pPos - pSize * 0.5f;
		D3DXVECTOR3 pMax = pPos + pSize * 0.5f;

		bool isOverlap =
			blockMin.x <= pMax.x && blockMax.x >= pMin.x &&
			blockMin.y <= pMax.y && blockMax.y >= pMin.y &&
			blockMin.z <= pMax.z && blockMax.z >= pMin.z;

		// 当たっている
		if (isOverlap)
		{
			// プレイヤーのリスポーン処理
			CGame::GetPlayer()->RespawnToCheckpoint(D3DXVECTOR3(2810.0f, 100.0f, -1518.0f));
		}
	}
}


//=============================================================================
// 質量ブロック(赤)ブロックのコンストラクタ
//=============================================================================
CRedMassBlock::CRedMassBlock()
{
	// 値のクリア
	m_ResPos = INIT_VEC3;
}
//=============================================================================
// 質量ブロック(赤)ブロックのデストラクタ
//=============================================================================
CRedMassBlock::~CRedMassBlock()
{
	// なし
}
//=============================================================================
// 質量ブロック(赤)ブロックの初期化処理
//=============================================================================
HRESULT CRedMassBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	m_ResPos = GetPos();

	// 動的に戻す
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// 質量ブロック(赤)ブロックの更新処理
//=============================================================================
void CRedMassBlock::Update(void)
{
	// ブロックの更新処理
	CBlock::Update();

	if (GetPos().y <= -410.0f)
	{
		// リスポーン処理
		Respawn(m_ResPos);
	}
}

//=============================================================================
// 質量ブロック(青)ブロックのコンストラクタ
//=============================================================================
CBlueMassBlock::CBlueMassBlock()
{
	// 値のクリア
	m_ResPos = INIT_VEC3;
}
//=============================================================================
// 質量ブロック(青)ブロックのデストラクタ
//=============================================================================
CBlueMassBlock::~CBlueMassBlock()
{
	// なし
}
//=============================================================================
// 質量ブロック(青)ブロックの初期化処理
//=============================================================================
HRESULT CBlueMassBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	m_ResPos = GetPos();

	// 動的に戻す
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// 質量ブロック(青)ブロックの更新処理
//=============================================================================
void CBlueMassBlock::Update(void)
{
	// ブロックの更新処理
	CBlock::Update();

	if (GetPos().y <= -410.0f)
	{
		// リスポーン処理
		Respawn(m_ResPos);
	}
}


//=============================================================================
// 質量ブロック(黄)ブロックのコンストラクタ
//=============================================================================
CYellowMassBlock::CYellowMassBlock()
{
	// 値のクリア
	m_ResPos = INIT_VEC3;
}
//=============================================================================
// 質量ブロック(黄)ブロックのデストラクタ
//=============================================================================
CYellowMassBlock::~CYellowMassBlock()
{
	// なし
}
//=============================================================================
// 質量ブロック(黄)ブロックの初期化処理
//=============================================================================
HRESULT CYellowMassBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	m_ResPos = GetPos();

	// 動的に戻す
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// 質量ブロック(黄)ブロックの更新処理
//=============================================================================
void CYellowMassBlock::Update(void)
{
	// ブロックの更新処理
	CBlock::Update();

	if (GetPos().y <= -410.0f)
	{
		// リスポーン処理
		Respawn(m_ResPos);
	}
}


//=============================================================================
// 質量ブロック(緑)ブロックのコンストラクタ
//=============================================================================
CGreenMassBlock::CGreenMassBlock()
{
	// 値のクリア
	m_ResPos = INIT_VEC3;
}
//=============================================================================
// 質量ブロック(緑)ブロックのデストラクタ
//=============================================================================
CGreenMassBlock::~CGreenMassBlock()
{
	// なし
}
//=============================================================================
// 質量ブロック(緑)ブロックの初期化処理
//=============================================================================
HRESULT CGreenMassBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	m_ResPos = GetPos();

	// 動的に戻す
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// 質量ブロック(緑)ブロックの更新処理
//=============================================================================
void CGreenMassBlock::Update(void)
{
	// ブロックの更新処理
	CBlock::Update();

	if (GetPos().y <= -410.0f)
	{
		// リスポーン処理
		Respawn(m_ResPos);
	}
}


//=============================================================================
// 水車ブロックのコンストラクタ
//=============================================================================
CWaterWheelBlock::CWaterWheelBlock()
{
	// 値のクリア
	m_playedSoundID = -1;
	m_isRotation = false;
}
//=============================================================================
// 水車ブロックのデストラクタ
//=============================================================================
CWaterWheelBlock::~CWaterWheelBlock()
{
	// なし
}
//=============================================================================
// 水車ブロックの更新処理
//=============================================================================
void CWaterWheelBlock::Update(void)
{
	CBlock::Update();// 共通処理

	Rotation();
}
//=============================================================================
// 水車ブロックの回転処理
//=============================================================================
void CWaterWheelBlock::Rotation(void)
{
	// 制御スイッチが存在するか確認
	std::vector<CBlock*> blocks = CBlockManager::GetAllBlocks();

	for (CBlock* block : blocks)
	{
		if (block->GetType() != TYPE_SWITCH2)
		{
			continue;
		}

		CBridgeSwitchBlock* pSwitch = dynamic_cast<CBridgeSwitchBlock*>(block);

		if (!pSwitch || !pSwitch->IsSinkOn())
		{
			continue;
		}

		m_isRotation = true;

		// 回転
		D3DXVECTOR3 rot = GetRot();

		rot.z += 0.02f;

		// 向きの設定
		SetRot(rot);
	}

	if (!m_isRotation)
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();
	float distance = D3DXVec3Length(&disPos);
	const float kTriggerDistance = 880.0f; // 反応距離

	if (distance < kTriggerDistance)
	{
		if (m_playedSoundID == -1) // 再生していなければ再生開始
		{
			// 前の音を止める（念のため）
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3Dサウンド再生してIDを保持
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_WATERWHEEL, GetPos(), 150.0f, kTriggerDistance);
		}

	}
	else
	{
		// 離れたら音停止してIDリセット
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	// 音源の位置更新はIDを使う
	if (m_playedSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
	}
}


//=============================================================================
// パイプブロックのコンストラクタ
//=============================================================================
CPipeBlock::CPipeBlock()
{
	// 値のクリア
	m_playedSoundID = -1;
	m_isOn = false;
}
//=============================================================================
// パイプブロックのデストラクタ
//=============================================================================
CPipeBlock::~CPipeBlock()
{
	// なし
}
//=============================================================================
// パイプブロックの更新処理
//=============================================================================
void CPipeBlock::Update(void)
{
	// ブロックの更新処理
	CBlock::Update();

	// 制御スイッチが存在するか確認
	std::vector<CBlock*> blocks = CBlockManager::GetAllBlocks();

	for (CBlock* block : blocks)
	{
		if (block->GetType() != TYPE_SWITCH2)
		{
			continue;
		}

		CBridgeSwitchBlock* pSwitch = dynamic_cast<CBridgeSwitchBlock*>(block);

		if (!pSwitch || !pSwitch->IsSinkOn())
		{
			continue;
		}

		m_isOn = true;
	}

	if (!m_isOn)
	{
		return;
	}

	CParticle* pParticle = NULL;
	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	// オフセット
	D3DXVECTOR3 localOffset(0.0f, 140.0f, 50.0f); // 松明の先端（ローカル）
	D3DXVECTOR3 worldOffset;

	// ブロックのワールドマトリックスを取得
	D3DXMATRIX worldMtx = GetWorldMatrix();

	D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

	// ==== 前方向ベクトル（Z軸）====
	D3DXVECTOR3 localForward(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 forward;
	D3DXVec3TransformNormal(&forward, &localForward, &worldMtx);
	D3DXVec3Normalize(&forward, &forward);

	// パーティクル生成
	pParticle = CParticle::Create(forward, worldOffset, D3DXCOLOR(0.3f, 0.6f, 1.0f, 0.8f), 50, CParticle::TYPE_WATERFLOW, 10);

	const float kTriggerDistance = 1180.0f; // 反応距離

	if (distance < kTriggerDistance)
	{

		//if (m_playedSoundID == -1) // 再生していなければ再生開始
		//{
		//	// 前の音を止める（念のため）
		//	CManager::GetSound()->Stop(m_playedSoundID);

		//	// 3Dサウンド再生してIDを保持
		//	m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
		//}
	}
	//else
	//{
	//	// 離れたら音停止してIDリセット
	//	if (m_playedSoundID != -1)
	//	{
	//		CManager::GetSound()->Stop(m_playedSoundID);
	//		m_playedSoundID = -1;
	//	}
	//}

	//// 音源の位置更新はIDを使う
	//if (m_playedSoundID != -1)
	//{
	//	CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, worldOffset);
	//}
}