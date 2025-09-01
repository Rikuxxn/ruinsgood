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
#include "particle.h"
#include "game.h"
#include "result.h"
#include "blocklist.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
std::unordered_map<CBlock::TYPE, BlockCreateFunc> CBlock::m_BlockFactoryMap = {};

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

	m_mass			 = 1.0f;					// 質量
	m_col			 = INIT_XCOL;				// 色
	m_baseCol		 = INIT_XCOL;				// ベースの色
	m_bSelected		 = false;					// 選択フラグ
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
	m_BlockFactoryMap[CBlock::TYPE_STATUE]				= []() -> CBlock* { return new CStatueBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_STATUE2]				= []() -> CBlock* { return new CStatueBlock2(); };
	m_BlockFactoryMap[CBlock::TYPE_EGG]					= []() -> CBlock* { return new CEggBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_DOOR_TRIGGER]		= []() -> CBlock* { return new CDoorTriggerBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_MASSBLOCK_RED]		= []() -> CBlock* { return new CRedMassBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_MASSBLOCK_BLUE]		= []() -> CBlock* { return new CBlueMassBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_MASSBLOCK_YELLOW]	= []() -> CBlock* { return new CYellowMassBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_MASSBLOCK_GREEN]		= []() -> CBlock* { return new CGreenMassBlock(); };
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

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CBlock::Uninit(void)
{
	// サウンドを停止
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

	//CBlock* pSelectBlock = CBlockManager::GetSelectedBlock();

	//// 当たり判定の描画処理
	//pSelectBlock->DrawCollider();

#endif

}
//=============================================================================
// 当たり判定描画処理
//=============================================================================
void CBlock::DrawCollider(void)
{
	if (m_pRigidBody)
	{
		// コライダーの描画
		m_pDebug3D->DrawBlockCollider(m_pRigidBody, D3DXCOLOR(0.0f, 1.0f, 0.3f, 1.0f));
	}
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
	m_pRigidBody->setFriction(GetFriction());		// 摩擦
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