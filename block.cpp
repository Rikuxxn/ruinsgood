//=============================================================================
//
// ブロック処理 [block.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "block.h"
#include "manager.h"
#include <algorithm>
#include "particle.h"
#include "game.h"
#include "result.h"
#include "collisionUtils.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
std::unordered_map<CBlock::TYPE, BlockCreateFunc> CBlock::m_BlockFactoryMap = {};
std::unordered_map<CBlock::TYPE, const char*> s_TexturePathMap = {};

using namespace std;

//=============================================================================
// コンストラクタ
//=============================================================================
CBlock::CBlock(int nPriority) : CObjectX(nPriority)
{
	// 値のクリア
	for (int nCnt = 0; nCnt < MAX_PATH; nCnt++)
	{
		m_szPath[nCnt] = NULL;					// ファイルパス
	}

	m_col			 = INIT_XCOL;				// 色
	m_baseCol		 = INIT_XCOL;				// ベースの色
	m_bSelected		 = false;					// 選択フラグ
	m_nIdxTexture	 = 0;						// テクスチャインデックス
	m_pDebug3D		 = NULL;					// 3Dデバッグ表示へのポインタ
	m_prevSize		 = INIT_VEC3;				// 前回のサイズ
	m_colliderSize	 = INIT_VEC3;				// コライダーサイズ
	m_colliderOffset = INIT_VEC3;				// コライダーのオフセット
	m_isEditMode	 = false;					// 編集中かどうか
}
//=============================================================================
// 生成処理
//=============================================================================
CBlock* CBlock::Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size, TYPE type)
{
	if (m_BlockFactoryMap.empty())
	{
		// ファクトリー
		InitFactory();
	}

	CBlock* pBlock = nullptr;

	auto it = m_BlockFactoryMap.find(type);
	if (it != m_BlockFactoryMap.end())
	{
		pBlock = it->second();
	}
	else
	{
		pBlock = new CBlock(); // デフォルト基底クラス
	}

	if (!pBlock)
	{
		return NULL;
	}

	pBlock->SetPos(pos);
	pBlock->SetRot(rot);
	pBlock->SetSize(size);
	pBlock->SetType(type);
	pBlock->SetPath(pFilepath);

	pBlock->Init();
	pBlock->CreatePhysicsFromScale(size);

	return pBlock;
}
//=============================================================================
// ファクトリー
//=============================================================================
void CBlock::InitFactory(void)
{
	// リストを空にする
	m_BlockFactoryMap.clear();

	m_BlockFactoryMap[CBlock::TYPE_WOODBOX]				= []() -> CBlock* { return new CWoodBoxBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_PILLAR]				= []() -> CBlock* { return new CPillarBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_BRIDGE]				= []() -> CBlock* { return new CWoodBridgeBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_RAFT]				= []() -> CBlock* { return new CRaftBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_WATER]				= []() -> CBlock* { return new CWaterBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_DOOR]				= []() -> CBlock* { return new CDoorBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_DOOR2]				= []() -> CBlock* { return new CBigDoorBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_SWITCH]				= []() -> CBlock* { return new CSwitchBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_SWITCH2]				= []() -> CBlock* { return new CBridgeSwitchBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_AXE]					= []() -> CBlock* { return new CAxeBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_BRIDGE2]				= []() -> CBlock* { return new CBridgeBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_TARGET]				= []() -> CBlock* { return new CTargetBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_TORCH]				= []() -> CBlock* { return new CTorchBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_TORCH2]				= []() -> CBlock* { return new CTorch2Block(); };
	m_BlockFactoryMap[CBlock::TYPE_MASK]				= []() -> CBlock* { return new CMaskBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_SWORD]				= []() -> CBlock* { return new CSwordBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_SWITCH3]				= []() -> CBlock* { return new CBarSwitchBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_BAR]					= []() -> CBlock* { return new CBarBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_BRIDGE3]				= []() -> CBlock* { return new CFootingBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_FIRE_STATUE]			= []() -> CBlock* { return new CFireStatueBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_TURN_FIRE_STATUE]	= []() -> CBlock* { return new CTurnFireStatueBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_BLOCK3]				= []() -> CBlock* { return new CBoxRockBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_FLOOR4]				= []() -> CBlock* { return new CLavaBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_MOVE_FIRE_STATUE]	= []() -> CBlock* { return new CMoveFireStatueBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_TORCH3]				= []() -> CBlock* { return new CTorch3Block(); };
	m_BlockFactoryMap[CBlock::TYPE_KEYFENCE]			= []() -> CBlock* { return new CKeyFenceBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_KEY]					= []() -> CBlock* { return new CKeyBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_KEY_PEDESTAL]		= []() -> CBlock* { return new CKeyPedestalBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_KEY_DOOR]			= []() -> CBlock* { return new CKeyDoorBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_SHIELD]				= []() -> CBlock* { return new CShieldBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_ROCK]				= []() -> CBlock*
	{
		CRockBlock* pRock = new CRockBlock();

		// チェックポイント追加
		pRock->AddPathPoint(D3DXVECTOR3(2812.5f, 217.0f, -1989.0f));
		pRock->AddPathPoint(D3DXVECTOR3(2810.0f, 217.0f, -2821.5f));
		pRock->AddPathPoint(D3DXVECTOR3(2718.0f, 217.0f, -3045.0f));
		pRock->AddPathPoint(D3DXVECTOR3(1958.5f, 217.0f, -3815.0f));
		pRock->AddPathPoint(D3DXVECTOR3(1746.0f, 217.0f, -3898.0f));
		pRock->AddPathPoint(D3DXVECTOR3(343.0f, 217.0f, -3898.0f));
		pRock->AddPathPoint(D3DXVECTOR3(-660.0f, 217.0f, -3898.0f));
		pRock->AddPathPoint(D3DXVECTOR3(-1230.5f, 217.0f, -3898.0f));
		pRock->AddPathPoint(D3DXVECTOR3(-1430.5f, 217.0f, -3898.0f));
		return pRock;
	};
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CBlock::Init(void)
{
	// オブジェクトXの初期化処理
	CObjectX::Init();

	// マテリアル色をブロックの色に設定
	m_col = GetMaterialColor();
	m_col = m_baseCol;              // 現在の色にも一度入れておく

	// ====棒の初期コライダー====
	m_colliderHandle.size = D3DXVECTOR3(21.0f, 248.5f, 21.5f);
	m_colliderHandle.offset = D3DXVECTOR3(0.5f, -118.0f, 0.0f);

	// ====刃の初期コライダー====
	m_colliderBlade.size = D3DXVECTOR3(304.0f, 127.0f, 24.5f);
	m_colliderBlade.offset = D3DXVECTOR3(0.0f, -307.0f, 0.0f);

	// テクスチャ割り当て
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register(GetTexPathFromType(m_Type));

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CBlock::Uninit(void)
{
	// 指定したラベルのサウンドを停止
	CManager::GetSound()->Stop();

	ReleasePhysics();

	// オブジェクトXの終了処理
	CObjectX::Uninit();
}
//=============================================================================
// 更新処理
//=============================================================================
void CBlock::Update(void)
{
	// 静的ブロックは Transform を手動で更新
	if (!IsDynamicBlock() || IsEditMode())
	{
		D3DXVECTOR3 Pos = GetPos() + m_colliderOffset;
		D3DXVECTOR3 Rot = GetRot();

		btTransform trans;
		trans.setIdentity();

		D3DXMATRIX matRot;
		D3DXMatrixRotationYawPitchRoll(&matRot, Rot.y, Rot.x, Rot.z);

		D3DXQUATERNION dq;
		D3DXQuaternionRotationMatrix(&dq, &matRot);

		btQuaternion q(dq.x, dq.y, dq.z, dq.w);
		trans.setOrigin(btVector3(Pos.x, Pos.y, Pos.z));
		trans.setRotation(q);

		if (m_pRigidBody && m_pRigidBody->getMotionState())
		{
			m_pRigidBody->setWorldTransform(trans);
			m_pRigidBody->getMotionState()->setWorldTransform(trans);
		}
	}
	else
	{
		// 動的ブロックは物理の変換を取得して反映

		if (!m_pRigidBody || !m_pRigidBody->getMotionState())
		{
			return;
		}

		btTransform trans;

		m_pRigidBody->getMotionState()->getWorldTransform(trans);

		btVector3 pos = trans.getOrigin();

		// 位置セット（オフセット差し引き）
		D3DXVECTOR3 newPos(pos.x(), pos.y(), pos.z());
		SetPos(newPos - m_colliderOffset);

		btQuaternion rot = m_pRigidBody->getOrientation();
		rot.normalize();

		// Z軸反転で座標系補正
		D3DXQUATERNION dq(rot.x(), rot.y(), rot.z(), rot.w());

		// 回転行列に変換
		D3DXMATRIX matRot;
		D3DXMatrixRotationQuaternion(&matRot, &dq);

		// 行列 -> オイラー角（XYZ順）
		D3DXVECTOR3 euler;
		float sy = -matRot._32; // 右手座標

		// Clamp範囲を厳密にして異常角回避
		sy = std::max(-1.0f, std::min(1.0f, sy));
		euler.x = asinf(sy);  // pitch (X)

		// cos(pitch) が0に近いとジンバルロックなので、その回避処理
		if (fabsf(cosf(euler.x)) > 1e-4f)
		{
			euler.y = atan2f(matRot._31, matRot._33);  // yaw (Y)
			euler.z = atan2f(matRot._12, matRot._22);  // roll (Z)
		}
		else
		{
			euler.y = 0.0f;
			euler.z = atan2f(-matRot._21, matRot._11); // 代替値（Rollだけ抽出）
		}

		// 連続補正
		static D3DXVECTOR3 prevEuler(0, 0, 0);
		auto FixAngleJump = [](float prev, float current) -> float

		{
			if (_isnan(current))
			{
				return prev;
			}

			float diff = current - prev;

			if (diff > D3DX_PI)
			{
				current -= 2 * D3DX_PI;
			}
			else if (diff < -D3DX_PI)
			{
				current += 2 * D3DX_PI;
			}

			return current;
		};

		euler.x = FixAngleJump(prevEuler.x, euler.x);
		euler.y = FixAngleJump(prevEuler.y, euler.y);
		euler.z = FixAngleJump(prevEuler.z, euler.z);

		prevEuler = euler;

		// セット
		SetRot(euler);
	}
}
//=============================================================================
// コライダーの更新処理
//=============================================================================
void CBlock::UpdateCollider(void)
{
	if (!m_pRigidBody)
	{
		return;
	}

	// 位置の取得
	D3DXVECTOR3 Pos = GetPos();

	// 削除して再生成
	ReleasePhysics();

	CreatePhysics(Pos, m_colliderSize);
}
//=============================================================================
// 描画処理
//=============================================================================
void CBlock::Draw(void)
{
	// オブジェクトXの描画処理
	CObjectX::Draw();

#ifdef _DEBUG

	//if (m_pRigidBody)
	//{
	//	// コライダーの描画
	//	m_pDebug3D->DrawBlockCollider(m_pRigidBody, D3DXCOLOR(0.0f, 1.0f, 0.3f, 1.0f));
	//}

#endif

}
//=============================================================================
// 色の取得
//=============================================================================
D3DXCOLOR CBlock::GetCol(void) const
{
	if (m_bSelected)
	{// 赤くする
		
		return D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.6f);
	}
	else
	{// 無補正

		return D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); 
	}
}
//=============================================================================
// 画像表示用テクスチャパスの取得
//=============================================================================
const char* CBlock::GetTexPathFromType(TYPE type)
{
	auto it = s_TexturePathMap.find(type);
	if (it != s_TexturePathMap.end())
	{
		return it->second;
	}

	return "";
}
//=============================================================================
// 画像表示用テクスチャパス
//=============================================================================
const std::unordered_map<CBlock::TYPE, const char*> CBlock::s_TexturePathMap = 
{
	{ TYPE_WOODBOX,			"data/TEXTURE/woodbox.png" },
	{ TYPE_WALL,			"data/TEXTURE/wall1.png" },
	{ TYPE_WALL2,			"data/TEXTURE/wall2.png" },
	{ TYPE_WALL3,			"data/TEXTURE/wall3.png" },
	{ TYPE_WALL4,			"data/TEXTURE/wall4.png" },
	{ TYPE_AXE,				"data/TEXTURE/Axe.png" },
	{ TYPE_RAFT,			"data/TEXTURE/ikada.png" },
	{ TYPE_ROCK,			"data/TEXTURE/rock.png" },
	{ TYPE_TORCH,			"data/TEXTURE/torch1.png" },
	{ TYPE_TORCH2,			"data/TEXTURE/torch2.png" },
	{ TYPE_FLOOR,			"data/TEXTURE/floor1.png" },
	{ TYPE_FLOOR2,			"data/TEXTURE/floor2.png" },
	{ TYPE_DOOR,			"data/TEXTURE/door1.png" },
	{ TYPE_CEILING,			"data/TEXTURE/ceiling1.png" },
	{ TYPE_CEILING2,		"data/TEXTURE/ceiling2.png" },
	{ TYPE_WATER,			"data/TEXTURE/water.png" },
	{ TYPE_SWITCH,			"data/TEXTURE/switch.png" },
	{ TYPE_SWITCH_BODY,		"data/TEXTURE/switch_body.png" },
	{ TYPE_BRIDGE,			"data/TEXTURE/bridge.png" },
	{ TYPE_DOOR_TOP,		"data/TEXTURE/door_top.png" },
	{ TYPE_DOOR_SIDE,		"data/TEXTURE/door_left.png" },
	{ TYPE_PILLAR,			"data/TEXTURE/pillar.png" },
	{ TYPE_BLOCK,			"data/TEXTURE/block.png" },
	{ TYPE_FENCE,			"data/TEXTURE/fence.png" },
	{ TYPE_FENCE_PART,		"data/TEXTURE/fence_part.png" },
	{ TYPE_BRIDGE2,			"data/TEXTURE/bridge2.png" },
	{ TYPE_TARGET,			"data/TEXTURE/target.png" },
	{ TYPE_SWITCH2,			"data/TEXTURE/controlswitch.png" },
	{ TYPE_DOOR2,			"data/TEXTURE/door2.png" },
	{ TYPE_MASK,			"data/TEXTURE/mask.png" },
	{ TYPE_SWORD,			"data/TEXTURE/sword.png" },
	{ TYPE_SWITCH3,			"data/TEXTURE/controlswitch2.png" },
	{ TYPE_BAR,				"data/TEXTURE/bar.png" },
	{ TYPE_BRIDGE3,			"data/TEXTURE/bridge3.png" },
	{ TYPE_FIRE_STATUE,		"data/TEXTURE/fire_statue.png" },
	{ TYPE_WALL5,			"data/TEXTURE/wall5.png" },
	{ TYPE_FLOOR3,			"data/TEXTURE/floor3.png" },
	{ TYPE_TURN_FIRE_STATUE,"data/TEXTURE/turn_fire_statue.png" },
	{ TYPE_BLOCK2,			"data/TEXTURE/block2.png" },
	{ TYPE_STAIRS,			"data/TEXTURE/stairs.png" },
	{ TYPE_PILLAR2,			"data/TEXTURE/pillar2.png" },
	{ TYPE_BLOCK3,			"data/TEXTURE/block3.png" },
	{ TYPE_FLOOR4,			"data/TEXTURE/floor4.png" },
	{ TYPE_MOVE_FIRE_STATUE,"data/TEXTURE/fire_statue.png" },
	{ TYPE_TORCH3,			"data/TEXTURE/torch2.png" },
	{ TYPE_NETFLOOR,		"data/TEXTURE/netfloor.png" },
	{ TYPE_KEYFENCE,		"data/TEXTURE/keyfence.png" },
	{ TYPE_KEY,				"data/TEXTURE/key.png" },
	{ TYPE_KEY_PEDESTAL,	"data/TEXTURE/key_pedestal.png" },
	{ TYPE_KEY_DOOR,		"data/TEXTURE/keydoor.png" },
	{ TYPE_SHIELD,			"data/TEXTURE/shield.png" },
};
//=============================================================================
// 当たり判定の生成処理
//=============================================================================
void CBlock::CreatePhysics(const D3DXVECTOR3& pos, const D3DXVECTOR3& size)
{
	ReleasePhysics();

	m_colliderSize = size;

	if (m_Type == TYPE_ROCK || m_Type == TYPE_KEY)
	{
		float radius = std::max(std::max(size.x, size.y), size.z) * 0.5f;
		m_pShape = new btSphereShape(radius);
	}
	else if (m_Type == TYPE_AXE)
	{
		btCompoundShape* compound = new btCompoundShape();

		// 前の子シェイプをクリア
		m_childShapes.clear();

		// Handle
		btBoxShape* boxHandle = new btBoxShape(btVector3(
			m_colliderHandle.size.x * 0.5f,
			m_colliderHandle.size.y * 0.5f,
			m_colliderHandle.size.z * 0.5f));
		btTransform transHandle;

		transHandle.setIdentity();
		transHandle.setOrigin(btVector3(
			m_colliderHandle.offset.x,
			m_colliderHandle.offset.y,
			m_colliderHandle.offset.z));

		compound->addChildShape(transHandle, boxHandle);
		m_childShapes.push_back(boxHandle); // 後で delete する用

		// Blade
		btBoxShape* boxBlade = new btBoxShape(btVector3(
			m_colliderBlade.size.x * 0.5f,
			m_colliderBlade.size.y * 0.5f,
			m_colliderBlade.size.z * 0.5f));
		btTransform transBlade;

		transBlade.setIdentity();
		transBlade.setOrigin(btVector3(
			m_colliderBlade.offset.x,
			m_colliderBlade.offset.y,
			m_colliderBlade.offset.z));

		compound->addChildShape(transBlade, boxBlade);
		m_childShapes.push_back(boxBlade); // 後で delete する用

		m_pShape = compound;
	}
	else
	{
		btVector3 halfExtents(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
		m_pShape = new btBoxShape(halfExtents);
	}

	btTransform transform;
	transform.setIdentity();
	btVector3 origin(pos.x + m_colliderOffset.x, pos.y + m_colliderOffset.y, pos.z + m_colliderOffset.z);
	transform.setOrigin(origin);

	// 回転を反映
	D3DXVECTOR3 euler = GetRot();
	D3DXMATRIX matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot, euler.y, euler.x, euler.z);

	D3DXQUATERNION dq;
	D3DXQuaternionRotationMatrix(&dq, &matRot);
	btQuaternion quat(dq.x, dq.y, dq.z, dq.w);
	transform.setRotation(quat);

	// 編集中は強制的に動的（キネマティック用）
	btScalar mass = m_isEditMode ? GetMass() : (!IsDynamicBlock() ? 0.0f : GetMass());
	btVector3 inertia(0, 0, 0);

	if (mass != 0.0f)
	{
		m_pShape->calculateLocalInertia(mass, inertia);
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, m_pShape, inertia);

	m_pRigidBody = new btRigidBody(rbInfo);
	m_pRigidBody->setUserPointer(this);
	m_pRigidBody->setActivationState(DISABLE_DEACTIVATION);// スリープ状態にしない

	int flags = m_pRigidBody->getCollisionFlags();

	if (m_Type == TYPE_WATER)
	{
		flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
		m_pRigidBody->setCollisionFlags(flags);
	}

	m_pRigidBody->setAngularFactor(GetAngularFactor());
	m_pRigidBody->setFriction(1.0f);		// 摩擦
	m_pRigidBody->setRollingFriction(GetRollingFriction());	// 転がり摩擦
	m_pRigidBody->setDamping(0.1f, 0.5f);	// linearDamping, angularDamping

	btDiscreteDynamicsWorld* pWorld = CManager::GetPhysicsWorld();

	if (pWorld != NULL)
	{
		pWorld->addRigidBody(m_pRigidBody);
	}
}
//=============================================================================
// 複合コライダーの生成処理
//=============================================================================
void CBlock::CreatePhysicsFromParts(void)
{
	ReleasePhysics();

	btCompoundShape* compound = new btCompoundShape();

	// 安全のため前の子シェイプリストをクリア
	m_childShapes.clear();

	// 棒（Handle）
	btBoxShape* boxHandle = new btBoxShape(btVector3(
		m_colliderHandle.size.x * 0.5f,
		m_colliderHandle.size.y * 0.5f,
		m_colliderHandle.size.z * 0.5f));

	btTransform transHandle;
	transHandle.setIdentity();
	transHandle.setOrigin(btVector3(
		m_colliderHandle.offset.x,
		m_colliderHandle.offset.y,
		m_colliderHandle.offset.z));

	compound->addChildShape(transHandle, boxHandle);
	m_childShapes.push_back(boxHandle); // 後で delete するため記録

	// 刃（Blade）
	btBoxShape* boxBlade = new btBoxShape(btVector3(
		m_colliderBlade.size.x * 0.5f,
		m_colliderBlade.size.y * 0.5f,
		m_colliderBlade.size.z * 0.5f));

	btTransform transBlade;
	transBlade.setIdentity();
	transBlade.setOrigin(btVector3(
		m_colliderBlade.offset.x,
		m_colliderBlade.offset.y,
		m_colliderBlade.offset.z));

	compound->addChildShape(transBlade, boxBlade);
	m_childShapes.push_back(boxBlade); // 後で delete するため記録

	m_pShape = compound;

	// ワールド変換（位置）
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(
		GetPos().x + m_colliderOffset.x,
		GetPos().y + m_colliderOffset.y,
		GetPos().z + m_colliderOffset.z));

	btScalar mass = !IsDynamicBlock() ? 0.0f : 3.0f;
	btVector3 inertia(0, 0, 0);

	if (mass != 0.0f)
	{
		m_pShape->calculateLocalInertia(mass, inertia);
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, m_pShape, inertia);

	m_pRigidBody = new btRigidBody(rbInfo);
	m_pRigidBody->setUserPointer(this);

	m_pRigidBody->setLinearFactor(btVector3(1, 1, 1));     // 動かす軸
	m_pRigidBody->setAngularFactor(btVector3(1, 1, 1));    // 回転させる軸
	m_pRigidBody->setFriction(0.5f);
	m_pRigidBody->setDamping(0.05f, 0.05f);
	m_pRigidBody->setGravity(btVector3(0, -59.8f, 0));
	m_pRigidBody->setActivationState(DISABLE_DEACTIVATION);

	btDiscreteDynamicsWorld* pWorld = CManager::GetPhysicsWorld();
	if (pWorld != NULL)
	{
		pWorld->addRigidBody(m_pRigidBody);
	}
}
//=============================================================================
// スケールによるコライダーの生成処理
//=============================================================================
void CBlock::CreatePhysicsFromScale(const D3DXVECTOR3& scale)
{
	D3DXVECTOR3 modelSize = GetModelSize();

	D3DXVECTOR3 newColliderSize =
	{
		modelSize.x * scale.x,
		modelSize.y * scale.y,
		modelSize.z * scale.z
	};

	CreatePhysics(GetPos(), newColliderSize); // 再生成
}
//=============================================================================
// 当たり判定の手動設定用
//=============================================================================
void CBlock::SetColliderManual(const D3DXVECTOR3& newSize)
{
	m_colliderSize = newSize;

	ReleasePhysics();							// 以前の剛体を削除

	CreatePhysics(GetPos(), m_colliderSize);	// 再生成
}
//=============================================================================
// コライダーのオフセットの設定
//=============================================================================
void CBlock::SetColliderOffset(const D3DXVECTOR3& offset)
{
	m_colliderOffset = offset;
}
//=============================================================================
// Physicsの破棄
//=============================================================================
void CBlock::ReleasePhysics(void)
{
	auto world = CManager::GetPhysicsWorld();

	if (m_pRigidBody)
	{
		if (world)
			world->removeRigidBody(m_pRigidBody);

		delete m_pRigidBody->getMotionState();
		delete m_pRigidBody;
		m_pRigidBody = NULL;
	}

	// 子Shape（AXEなど）を明示的に削除
	for (btCollisionShape* shape : m_childShapes)
	{
		delete shape;
	}
	m_childShapes.clear();

	if (m_pShape)
	{
		delete m_pShape;
		m_pShape = NULL;
	}
}
//=============================================================================
// コライダーサイズの設定処理
//=============================================================================
void CBlock::SetColliderSize(const D3DXVECTOR3& size)
{
	m_colliderSize = size;
}
//=============================================================================
// 複合コライダーの判定処理
//=============================================================================
bool CBlock::IsCompoundCollider(void) const
{
	if (!m_pRigidBody)
	{
		return false;
	}

	btCollisionShape* shape = m_pRigidBody->getCollisionShape();

	if (!shape)
	{
		return false;
	}

	return (shape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE);
}
//=============================================================================
// エディター中かどうかでキネマティックにするか判定する処理
//=============================================================================
void CBlock::SetEditMode(bool enable)
{
	m_isEditMode = enable;

	if (!m_pRigidBody)
	{
		return;
	}

	if (enable)
	{
		m_pRigidBody->setCollisionFlags(m_pRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		m_pRigidBody->setActivationState(DISABLE_DEACTIVATION);
	}
	else
	{
		m_pRigidBody->setCollisionFlags(m_pRigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		m_pRigidBody->setActivationState(ACTIVE_TAG);
	}
}
//=============================================================================
// ワールドマトリックスの取得
//=============================================================================
D3DXMATRIX CBlock::GetWorldMatrix(void)
{
	D3DXMATRIX matScale, matRot, matTrans;

	// スケール行列
	D3DXVECTOR3 scale = GetSize(); // 拡大率
	D3DXMatrixScaling(&matScale, scale.x, scale.y, scale.z);

	// 回転行列
	D3DXVECTOR3 rot = GetRot(); // ラジアン角
	D3DXMatrixRotationYawPitchRoll(&matRot, rot.y, rot.x, rot.z);

	// 平行移動行列
	D3DXVECTOR3 pos = GetPos();
	D3DXMatrixTranslation(&matTrans, pos.x, pos.y, pos.z);

	// 合成：S * R * T
	D3DXMATRIX world = matScale * matRot * matTrans;

	return world;
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
			pKey->Respawn();
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
	const float B_maxLiftSpeed = 20.0f;

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

		btRigidBody* pRigid = block->GetRigidBody();

		if (pRigid && block->IsDynamicBlock())
		{
			btVector3 velocity = pRigid->getLinearVelocity();

			if (velocity.getY() < B_maxLiftSpeed)
			{
				// 浮かばせる目標上向き速度
				const float targetUpSpeed = 130.0f; // 浮上スピード
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
	m_isDoorOpened	= false;
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
// 最終エリアドアブロックの更新処理
//=============================================================================
void CBigDoorBlock::Update(void)
{
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
		// ドアを開く
		D3DXVECTOR3 newPos = GetPos();

		if (newPos.y <= 385.0f)
		{
			newPos.y += 0.5f;
			SetPos(newPos);
		}
	}

	CBlock::Update();// 共通処理
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
	const float kPressDepth = 10.0f; // 下がる深さ

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

		if (waterPos.y < -280.0f)
		{
			// 徐々に水位を上げる
			waterPos.y += 0.5f;
			block->SetPos(waterPos);
		}
		else
		{
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
	const float massThreshold = 4.0f;

	if (totalMass >= massThreshold)
	{
		m_isSwitchOn = true;
	}

	if (!m_isSwitchOn)
	{
		return;
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

	bool n = m_isSwitchOn;

	if (n && !m_prevSwitchOn) // 一回だけ実行
	{
		// スイッチSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_SWITCH);

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
				SetTimer(26);

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

	UpdateCollider();

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
			CManager::GetSound()->Stop(CSound::SOUND_LABEL_SWING);

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
				30,                    // 寿命
				CParticle::TYPE_WATER,  // パーティクルタイプ
				5);                    // 数
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
		if (block == this ||block->GetType() != TYPE_ROCK)
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
	m_playedFireSoundID = -1;
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
			CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_FIRE, 1);
			CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_FIRE, 1);

			if (m_playedFireSoundID == -1) // 再生していなければ再生開始
			{
				// 前の音を止める（念のため）
				CManager::GetSound()->Stop(CSound::SOUND_LABEL_FIRE);

				// 3Dサウンド再生してIDを保持
				m_playedFireSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
			}

		}
		else
		{
			// 離れたら音停止してIDリセット
			if (m_playedFireSoundID != -1)
			{
				CManager::GetSound()->Stop(m_playedFireSoundID);
				m_playedFireSoundID = -1;
			}
		}

		// 音源の位置更新はIDを使う
		if (m_playedFireSoundID != -1)
		{
			CManager::GetSound()->UpdateSoundPosition(m_playedFireSoundID, worldOffset);
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
	m_playedFireSoundID = -1;
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
	if (CManager::GetMode() != MODE_GAME)
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

		if (m_playedFireSoundID == -1) // 再生していなければ再生開始
		{
			// 前の音を止める（念のため）
			CManager::GetSound()->Stop(CSound::SOUND_LABEL_FIRE);

			// 3Dサウンド再生してIDを保持
			m_playedFireSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
		}

	}
	else
	{
		// 離れたら音停止してIDリセット
		if (m_playedFireSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedFireSoundID);
			m_playedFireSoundID = -1;
		}
	}

	// 音源の位置更新はIDを使う
	if (m_playedFireSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedFireSoundID, worldOffset);
	}
}


//=============================================================================
// 置き型松明(動かない)ブロックのコンストラクタ
//=============================================================================
CTorch3Block::CTorch3Block()
{
	// 値のクリア
	m_playedFireSoundID = -1;
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
	if (CManager::GetMode() != MODE_GAME)
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

		if (m_playedFireSoundID == -1) // 再生していなければ再生開始
		{
			// 前の音を止める（念のため）
			CManager::GetSound()->Stop(CSound::SOUND_LABEL_FIRE);

			// 3Dサウンド再生してIDを保持
			m_playedFireSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
		}
	}
	else
	{
		// 離れたら音停止してIDリセット
		if (m_playedFireSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedFireSoundID);
			m_playedFireSoundID = -1;
		}
	}

	// 音源の位置更新はIDを使う
	if (m_playedFireSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedFireSoundID, worldOffset);
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
				CManager::GetSound()->Stop(CSound::SOUND_LABEL_MASK);

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
				// 仮面取得UIの生成
				CUi::Create(CUi::TYPE_MASK, "data/TEXTURE/ui_mask.png", D3DXVECTOR3(900.0f, 220.0f, 0.0f), 320.0f, 140.0f);
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
				CManager::GetSound()->Stop(CSound::SOUND_LABEL_TREASURE);

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

	if (CManager::GetMode() == MODE_TITLE)
	{
		// オフセット
		D3DXVECTOR3 localOffset(0.0f, -60.0f, 0.0f);
		D3DXVECTOR3 worldOffset;

		// ブロックのワールドマトリックスを取得
		D3DXMATRIX worldMtx = GetWorldMatrix();

		D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

		// パーティクル生成
		pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.6f, 0.6f, 0.0f, 0.3f), 50, CParticle::TYPE_AURA, 1);
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

		if(pSwitch->IsSwitchOn())
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
			CManager::GetSound()->Stop(CSound::SOUND_LABEL_FIRE);

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
		cylinderCenter, cylinderRadius, cylinderHeight, forward,m_isBlocked))
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
			CManager::GetSound()->Stop(CSound::SOUND_LABEL_FIRE);

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
			CManager::GetSound()->Stop(CSound::SOUND_LABEL_FIRE);

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
// リスポーン処理
//=============================================================================
void CKeyBlock::Respawn(void)
{
	// 動かすためにキネマティックにする
	SetEditMode(true);

	// 鍵ブロックの位置を取得
	D3DXVECTOR3 keyPos = GetPos();
	D3DXVECTOR3 keyRot = GetRot();

	D3DXVECTOR3 respawnPos(m_ResPos);// リスポーン位置
	D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);// 向きをリセット

	keyPos = respawnPos;
	keyRot = rot;

	SetPos(keyPos);
	SetRot(keyRot);

	// コライダーの更新
	UpdateCollider();

	// 動的に戻す
	SetEditMode(false);
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
	}
}


//=============================================================================
// 鍵ドアブロックのコンストラクタ
//=============================================================================
CKeyDoorBlock::CKeyDoorBlock()
{
	// 値のクリア
	m_openPos = INIT_VEC3;
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

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_KEY_PEDESTAL)
		{// 台座じゃなかったら
			continue;
		}

		CKeyPedestalBlock* pPedestal = dynamic_cast<CKeyPedestalBlock*>(block);

		if (pPedestal->IsSet())
		{// 台座に設置されていたら

			// ドアを開ける
			const float kOpenRange = 210.0f; // 開く高さ

			float targetY = m_openPos.y + kOpenRange;

			D3DXVECTOR3 pos = GetPos();
			if (pos.y < targetY)
			{
				pos.y += 1.0f;
				SetPos(pos);
			}
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
				CManager::GetSound()->Stop(CSound::SOUND_LABEL_TREASURE);

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