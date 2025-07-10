//=============================================================================
//
// ブロック処理 [block.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "block.h"
#include "manager.h"
#include <algorithm>

using namespace std;

//=============================================================================
// コンストラクタ
//=============================================================================
CBlock::CBlock()
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
// デストラクタ
//=============================================================================
CBlock::~CBlock()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CBlock* CBlock::Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size, TYPE type)
{
	CBlock* pBlock = NULL;

	// タイプごとに派生クラスへ分岐
	switch (type)
	{
	case TYPE_WATER:
		pBlock = new CWaterBlock();
		break;
	case TYPE_DOOR:
		pBlock = new CDoorBlock();
		break;
	case TYPE_SWITCH:
		pBlock = new CSwitchBlock();
		break;
	default:
		pBlock = new CBlock();
		break;
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
// タイプでの生成処理
//=============================================================================
CBlock* CBlock::CreateFromType(TYPE type, D3DXVECTOR3 pos)
{
	const char* path = "";
	D3DXVECTOR3 size = { 1.0f, 1.0f, 1.0f };
	D3DXVECTOR3 rot = { 0.0f, 0.0f, 0.0f };

	switch (type)
	{
	case TYPE_WOODBOX:
		path = "data/MODELS/woodbox_001.x";
		size = { 1.5f, 1.5f, 1.5f };
		break;

	case TYPE_WALL:
		path = "data/MODELS/wall_01.x";
		size = { 1.0f, 1.0f, 1.0f };
		break;

	case TYPE_WALL2:
		path = "data/MODELS/wall_02.x";
		size = { 1.0f, 1.0f, 1.0f };
		break;

	case TYPE_WALL3:
		path = "data/MODELS/wall_03.x";
		size = { 1.0f, 1.0f, 1.0f };
		break;

	case TYPE_WALL4:
		path = "data/MODELS/wall_04.x";
		size = { 1.0f, 1.0f, 1.0f };
		break;

	case TYPE_AXE:
		path = "data/MODELS/Axe_01.x";
		size = { 2.0f, 2.0f, 2.0f };
		break;

	case TYPE_RAFT:
		path = "data/MODELS/ikada.x";
		size = { 1.0f, 1.0f, 1.0f };
		break;

	case TYPE_ROCK:
		path = "data/MODELS/Rock_001.x";
		size = { 1.0f, 1.0f, 1.0f };
		break;

	case TYPE_DOOR:
		path = "data/MODELS/door_01.x";
		size = { 1.0f, 1.0f, 1.0f };
		break;

	default:
		return NULL;
	}

	CBlock* block = CBlock::Create(path, pos, rot, size, type);

	return block;
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
	if (IsStaticBlock() || IsEditMode())
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

		m_pRigidBody->setWorldTransform(trans);
		m_pRigidBody->getMotionState()->setWorldTransform(trans);

	}
	else
	{
		// 動的ブロックは物理の変換を取得して反映

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

		// cos(pitch) が0に近いとgimbal lockなので、その回避処理
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

	if (m_pRigidBody)
	{
		// コライダーの描画
		m_pDebug3D->DrawBlockCollider(m_pRigidBody, D3DXCOLOR(0.0f, 1.0f, 0.3f, 1.0f));
	}

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
	switch (type)
	{
	case TYPE_WOODBOX: 
		return "data/TEXTURE/woodbox.png";

	case TYPE_WALL: 
		return "data/TEXTURE/wall1.png";

	case TYPE_WALL2:
		return "data/TEXTURE/wall2.png";

	case TYPE_WALL3:
		return "data/TEXTURE/wall3.png";

	case TYPE_WALL4:
		return "data/TEXTURE/wall4.png";

	case TYPE_AXE: 
		return "data/TEXTURE/Axe.png";

	case TYPE_RAFT: 
		return "data/TEXTURE/ikada.png";

	case TYPE_ROCK:
		return "data/TEXTURE/rock.png";

	case TYPE_TORCH:
		return "data/TEXTURE/torch1.png";

	case TYPE_TORCH2:
		return "data/TEXTURE/torch2.png";

	case TYPE_FLOOR:
		return "data/TEXTURE/floor1.png";

	case TYPE_FLOOR2:
		return "data/TEXTURE/floor2.png";

	case TYPE_DOOR:
		return "data/TEXTURE/door1.png";

	case TYPE_CEILING:
		return "data/TEXTURE/ceiling1.png";

	case TYPE_CEILING2:
		return "data/TEXTURE/ceiling2.png";

	case TYPE_WATER:
		return "data/TEXTURE/water.png";

	case TYPE_SWITCH:
		return "data/TEXTURE/switch.png";

	case TYPE_SWITCH_BODY:
		return "data/TEXTURE/switch_body.png";

	case TYPE_BRIDGE:
		return "data/TEXTURE/bridge.png";

	case TYPE_DOOR_TOP:
		return "data/TEXTURE/door_top.png";

	case TYPE_DOOR_SIDE:
		return "data/TEXTURE/door_left.png";

	case TYPE_PILLAR:
		return "data/TEXTURE/pillar.png";

	case TYPE_BLOCK:
		return "data/TEXTURE/block.png";

	case TYPE_FENCE:
		return "data/TEXTURE/fence.png";

	case TYPE_FENCE_PART:
		return "data/TEXTURE/fence_part.png";

	default: 
		return "";
	}
}
//=============================================================================
// 当たり判定の生成処理
//=============================================================================
void CBlock::CreatePhysics(const D3DXVECTOR3& pos, const D3DXVECTOR3& size)
{
	ReleasePhysics();

	m_colliderSize = size;

	if (m_Type == TYPE_ROCK)
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
	btScalar mass = m_isEditMode ? GetMassByType(m_Type) : (IsStaticBlock() ? 0.0f : GetMassByType(m_Type));
	btVector3 inertia(0, 0, 0);

	if (mass != 0.0f)
	{
		m_pShape->calculateLocalInertia(mass, inertia);
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, m_pShape, inertia);

	m_pRigidBody = new btRigidBody(rbInfo);
	m_pRigidBody->setUserPointer(this);

	int flags = m_pRigidBody->getCollisionFlags();

	if (m_Type == TYPE_WATER)
	{
		flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
		m_pRigidBody->setCollisionFlags(flags);
	}

	if (m_Type == TYPE_PILLAR)
	{
		m_pRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 1.0f));
	}
	else
	{
		m_pRigidBody->setFriction(1.0f);		// 摩擦
		m_pRigidBody->setRollingFriction(0.7f);	// 転がり摩擦
		m_pRigidBody->setDamping(0.1f, 0.5f);	// (linearDamping, angularDamping)
	}


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

	btScalar mass = IsStaticBlock() ? 0.0f : 3.0f;
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
// 静的なブロックか判定する処理
//=============================================================================
bool CBlock::IsStaticBlock(void) const
{
	switch (m_Type)
	{
	case TYPE_WALL:
	case TYPE_WALL2:
	case TYPE_WALL3:
	case TYPE_WALL4:
	case TYPE_TORCH:
	case TYPE_FLOOR:
	case TYPE_FLOOR2:
	case TYPE_CEILING:
	case TYPE_CEILING2:
	case TYPE_DOOR:
	case TYPE_SWITCH_BODY:
	case TYPE_DOOR_TOP:
	case TYPE_DOOR_SIDE:
	case TYPE_WATER:
	case TYPE_AXE:
	case TYPE_SWITCH:
	case TYPE_BLOCK:
	case TYPE_FENCE:
	case TYPE_FENCE_PART:
		return true; // 静的（動かない）

	default:
		return false; // 動く可能性あり
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
// ブロックタイプごとの質量を返す
//=============================================================================
btScalar CBlock::GetMassByType(TYPE type)
{
	switch (type)
	{
	case TYPE_WOODBOX:	return 4.0f;	// 木箱
	case TYPE_TORCH2:	return 5.0f;	// 置き型トーチ
	case TYPE_PILLAR:	return 55.0f;	// 柱
	case TYPE_ROCK:		return 10.0f;	// 岩
	case TYPE_BRIDGE:	return 8.0f;	// 橋
	case TYPE_RAFT:		return 6.0f;	// イカダ
	case TYPE_AXE:		return 80.0f;	// 斧
	default:			return 2.0f;	// デフォルト質量
	}
}


//=============================================================================
// 水ブロックのコンストラクタ
//=============================================================================
CWaterBlock::CWaterBlock()
{
	SetType(TYPE_WATER);
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
		if (block == this || block->IsStaticBlock())
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

		if (pRigid && !block->IsStaticBlock())
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
	CPlayer* pPlayer = CManager::GetPlayer();

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
			pPlayer->SetInWater(true); // フラグ用関数（下記追加）

			// タイマー更新
			pPlayer->AddWaterStayTime(1.0f / 60.0f); // 毎フレーム加算
		}
		else
		{
			pPlayer->SetInWater(false);
			pPlayer->ResetWaterStayTime();
		}
	}
}


//=============================================================================
// ドアブロックのコンストラクタ
//=============================================================================
CDoorBlock::CDoorBlock()
{
	SetType(TYPE_DOOR);

	// 値のクリア
	m_lerpStartPos	= INIT_VEC3;
	m_lerpTargetPos = INIT_VEC3;
	m_lerpTimer		= 0.0f;;
	m_lerpDuration	= 0.0f;
	m_isDoorOpened	= false;
}
//=============================================================================
// ドアブロックのデストラクタ
//=============================================================================
CDoorBlock::~CDoorBlock()
{
	// なし
}
//=============================================================================
// ドアブロックの更新処理
//=============================================================================
void CDoorBlock::Update(void)
{
	D3DXVECTOR3 playerPos = CManager::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 280.0f;// 反応する距離

	if (distance < kTriggerDistance && !m_isDoorOpened)
	{
		m_isDoorOpened = true;	// フラグON
	}

	if (m_isDoorOpened)
	{
		// ドアを開く
		D3DXVECTOR3 newPos = GetPos();

		if (newPos.y <= 300.0f)
		{
			newPos.y += 1.0f;
			SetPos(newPos);
		}
		else
		{
			m_isDoorOpened = false;
		}
	}
	else
	{
		// ドアを閉める
		D3DXVECTOR3 newPos = GetPos();

		if (newPos.y > 90.0f && !m_isDoorOpened)
		{
			newPos.y -= 1.0f;
			SetPos(newPos);
		}
	}

	CBlock::Update(); // 共通処理
}


//=============================================================================
// スイッチブロックのコンストラクタ
//=============================================================================
CSwitchBlock::CSwitchBlock()
{
	SetType(TYPE_SWITCH);

	// 値のクリア
	m_closedPos = INIT_VEC3;
	m_isSwitchOn = false;
}
//=============================================================================
// スイッチブロックのデストラクタ
//=============================================================================
CSwitchBlock::~CSwitchBlock()
{
	// なし
}
//=============================================================================
// スイッチブロックの更新処理
//=============================================================================
void CSwitchBlock::Update(void)
{
	m_closedPos = GetPos();

	// スイッチの AABB を取得
	D3DXVECTOR3 swPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize(); // スイッチのサイズ（中心原点）
	D3DXVECTOR3 scale = GetSize();

	D3DXVECTOR3 swSize;

	// 拡大率を適用する
	swSize.x = modelSize.x * scale.x;
	swSize.y = modelSize.y * scale.y;
	swSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 swMin = swPos - swSize * 0.5f;
	D3DXVECTOR3 swMax = swPos + swSize * 0.5f;

	float totalMass = 0.0f;

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block == this || block->IsStaticBlock())
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
		D3DXVECTOR3 pos = swPos;

		// 押されている（下に少し沈む）
		pos.y -= 1.0f; // 下に沈める

		if (pos.y > 12.0f)// TODO : いずれ下がる範囲を決めて判定するようにする
		{
			SetPos(pos);
		}

		SetEditMode(true); // 動かすためにキネマティック

		m_isSwitchOn = true;
	}

	if (!m_isSwitchOn)
	{
		return;
	}

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
	}

	CBlock::Update(); // 共通処理
}