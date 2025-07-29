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
	CRockBlock* pRock = NULL;

	// タイプごとに派生クラスへ分岐
	switch (type)
	{
	case TYPE_WATER:
		pBlock = new CWaterBlock();
		break;
	case TYPE_DOOR:
		pBlock = new CDoorBlock();
		break;
	case TYPE_DOOR2:
		pBlock = new CBigDoorBlock();
		break;
	case TYPE_SWITCH:
		pBlock = new CSwitchBlock();
		break;
	case TYPE_SWITCH2:
		pBlock = new CBridgeSwitchBlock();
		break;
	case TYPE_AXE:
		pBlock = new CAxeBlock();
		break;
	case TYPE_BRIDGE2:
		pBlock = new CBridgeBlock();
		break;
	case TYPE_TARGET:
		pBlock = new CTargetBlock();
		break;
	case TYPE_TORCH:
		pBlock = new CTorchBlock();
		break;
	case TYPE_TORCH2:
		pBlock = new CTorch2Block();
		break;
	case TYPE_MASK:
		pBlock = new CMaskBlock();
		break;
	case TYPE_SWORD:
		pBlock = new CSwordBlock();
		break;
	case TYPE_SWITCH3:
		pBlock = new CBarSwitchBlock();
		break;
	case TYPE_BAR:
		pBlock = new CBarBlock();
		break;
	case TYPE_BRIDGE3:
		pBlock = new CFootingBlock();
		break;
	case TYPE_ROCK:
		pBlock = new CRockBlock();

		// チェックポイントを設定
		pRock = dynamic_cast<CRockBlock*>(pBlock);
		if (pRock)
		{
			// 通常ルート
			pRock->AddPathPoint(D3DXVECTOR3(2812.5f, 217.0f, -1989.0f));
			pRock->AddPathPoint(D3DXVECTOR3(2810.0f, 217.0f, -2821.5f));
			pRock->AddPathPoint(D3DXVECTOR3(2718.0f, 217.0f, -3045.0f));
			pRock->AddPathPoint(D3DXVECTOR3(1958.5f, 217.0f, -3815.0f));
			pRock->AddPathPoint(D3DXVECTOR3(1746.0f, 217.0f, -3898.0f));
			pRock->AddPathPoint(D3DXVECTOR3(343.0f, 217.0f, -3898.0f));
			pRock->AddPathPoint(D3DXVECTOR3(-660.0f, 217.0f, -3898.0f));
			pRock->AddPathPoint(D3DXVECTOR3(-1230.5f, 217.0f, -3898.0f));
			pRock->AddPathPoint(D3DXVECTOR3(-1430.5f, 217.0f, -3898.0f));
		}

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
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_WATER);
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_WATERRISE);
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_HIT);
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_ROCKHIT);
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_SWITCH);
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_ROLL);
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_FIRE);
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_MASK);
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_INSPIRATION);
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_TIMER);

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

	case TYPE_BRIDGE2:
		return "data/TEXTURE/bridge2.png";

	case TYPE_TARGET:
		return "data/TEXTURE/target.png";

	case TYPE_SWITCH2:
		return "data/TEXTURE/controlswitch.png";

	case TYPE_DOOR2:
		return "data/TEXTURE/door2.png";

	case TYPE_MASK:
		return "data/TEXTURE/mask.png";

	case TYPE_SWORD:
		return "data/TEXTURE/sword.png";

	case TYPE_SWITCH3:
		return "data/TEXTURE/controlswitch2.png";

	case TYPE_BAR:
		return "data/TEXTURE/bar.png";

	case TYPE_BRIDGE3:
		return "data/TEXTURE/bridge3.png";

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
	m_pRigidBody->setActivationState(DISABLE_DEACTIVATION);// スリープ状態にしない

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
	case TYPE_BRIDGE2:
	case TYPE_TARGET:
	case TYPE_SWITCH2:
	case TYPE_DOOR2:
	case TYPE_MASK:
	case TYPE_SWORD:
	case TYPE_SWITCH3:
	case TYPE_BAR:
	case TYPE_BRIDGE3:
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
	case TYPE_TORCH2:	return 6.0f;	// 置き型トーチ
	case TYPE_PILLAR:	return 55.0f;	// 柱
	case TYPE_ROCK:		return 100.0f;	// 岩
	case TYPE_BRIDGE:	return 8.0f;	// 橋
	case TYPE_RAFT:		return 5.0f;	// イカダ
	case TYPE_AXE:		return 80.0f;	// 斧
	default:			return 2.0f;	// デフォルト質量
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
// 水ブロックのコンストラクタ
//=============================================================================
CWaterBlock::CWaterBlock()
{
	SetType(TYPE_WATER);

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
			pParticle = CParticle::Create(pos, D3DXCOLOR(0.3f, 0.6f, 1.0f, 0.8f), 50, CParticle::TYPE_WATER, 10);
			pParticle = CParticle::Create(pos, D3DXCOLOR(0.3f, 0.5f, 1.0f, 0.5f), 50, CParticle::TYPE_WATER, 10);
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

			pPlayer->RespawnToCheckpoint(); // 任意の場所へリスポーン
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
	SetType(TYPE_DOOR);

	// 値のクリア
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
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
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
// 最終エリアドアブロックのコンストラクタ
//=============================================================================
CBigDoorBlock::CBigDoorBlock()
{
	SetType(TYPE_DOOR2);

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
	SetType(TYPE_SWITCH);

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
// スイッチブロックの更新処理
//=============================================================================
void CSwitchBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	m_closedPos = GetPos();

	// スイッチの AABB を取得
	D3DXVECTOR3 swPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize(); // スイッチの元のサイズ（中心原点）
	D3DXVECTOR3 scale = GetSize();// 拡大率

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
		if (block == this || block->IsStaticBlock() || block->GetType() == TYPE_ROCK)
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

	D3DXVECTOR3 pos = swPos;

	// 押されている（下に少し沈む）
	pos.y -= 1.0f; // 下に沈める

	if (pos.y > 12.0f)// TODO : いずれ下がる範囲を決めて判定するようにする
	{
		SetPos(pos);
	}

	SetEditMode(true); // 動かすためにキネマティック

	bool n = m_isSwitchOn;

	if (n && !m_prevSwitchOn)
	{
		// スイッチSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_SWITCH);

		// ひらめきSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_INSPIRATION);

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
	SetType(TYPE_SWITCH2);

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
// 橋制御ブロックの更新処理
//=============================================================================
void CBridgeSwitchBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	m_closedPos = GetPos();

	// スイッチの AABB を取得
	D3DXVECTOR3 swPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize(); // スイッチの元のサイズ（中心原点）
	D3DXVECTOR3 scale = GetSize();// 拡大率

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
		if (block == this || block->IsStaticBlock() || block->GetType() == TYPE_ROCK)
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

	D3DXVECTOR3 pos = swPos;

	// 押されている（下に少し沈む）
	pos.y -= 1.0f; // 下に沈める

	if (pos.y > 12.0f)// TODO : いずれ下がる範囲を決めて判定するようにする
	{
		SetPos(pos);
	}

	bool n = m_isSwitchOn;

	if (n && !m_prevSwitchOn) // 一回だけ実行
	{
		// スイッチSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_SWITCH);

		// ひらめきSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_INSPIRATION);

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
	SetType(TYPE_SWITCH3);

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
// 格子制御ブロックの更新処理
//=============================================================================
void CBarSwitchBlock::Update(void)
{
	CBlock::Update(); // 共通処理

	m_closedPos = GetPos();

	// スイッチの AABB を取得
	D3DXVECTOR3 swPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize(); // スイッチの元のサイズ（中心原点）
	D3DXVECTOR3 scale = GetSize();// 拡大率

	D3DXVECTOR3 swSize;

	// 拡大率を適用する
	swSize.x = modelSize.x * scale.x;
	swSize.y = modelSize.y * scale.y;
	swSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 swMin = swPos - swSize * 0.5f;
	D3DXVECTOR3 swMax = swPos + swSize * 0.5f;

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

		D3DXVECTOR3 pos = swPos;

		//// 押されている（下に少し沈む）
		//pos.y -= 1.0f; // 下に沈める

		//if (pos.y > 12.0f)// TODO : いずれ下がる範囲を決めて判定するようにする
		//{
		//	SetPos(pos);
		//}

		if (m_timerCnt >= m_Timer)
		{// 指定時間を経過したら
			// スイッチを戻す
			m_isSwitchOn = false;
			m_timerCnt = 0;


		}
	}

	bool n = m_isSwitchOn;

	if (n && !m_prevSwitchOn) // 一回だけ実行
	{
		// スイッチSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_SWITCH);

		// ひらめきSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_INSPIRATION);

		// 演出カメラにする
		CManager::GetCamera()->SetCamMode(3, D3DXVECTOR3(2466.5f, 230.0f, -154.5f),
			D3DXVECTOR3(3035.5f, 39.5f, -505.0f),
			D3DXVECTOR3(0.28f, -1.02f, 0.0f));
	}

	// フラグを更新して次のフレームに備える
	m_prevSwitchOn = n;
}


//=============================================================================
// 斧ブロックのコンストラクタ
//=============================================================================
CAxeBlock::CAxeBlock()
{
	SetType(TYPE_AXE);

	// 値のクリア
	m_nSwingCounter = 0;					// フレームカウンター
	m_swingAmplitude = D3DXToRadian(75.0f);	// ±振れ角
	m_swingPeriod = 300.0f;					// 周期フレーム
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

	const float kTriggerDistance = 1150.0f; // 反応距離

	if (distance > kTriggerDistance)
	{
		return;
	}

	m_nSwingCounter++;

	float angle = m_swingAmplitude * sinf((2.0f * D3DX_PI * m_nSwingCounter) / m_swingPeriod);

	SetRot(D3DXVECTOR3(0.0f, 0.0f, angle)); // Z軸スイング

	UpdateCollider();
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
	SetType(TYPE_ROCK);

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

		if (m_particleTimer >= DASH_PARTICLE_INTERVAL)
		{
			m_particleTimer = 0;

			// パーティクルの生成
			CParticle::Create(D3DXVECTOR3(GetPos().x, GetPos().y - 200.0f, GetPos().z),
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

		if (m_currentTargetIndex >= 8 && m_playedRollSoundID != -1)
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

		if (m_currentTargetIndex >= 8 && m_playedRollSoundID != -1)
		{
			// 転がる音の停止
			CManager::GetSound()->Stop(m_playedRollSoundID);
			m_playedRollSoundID = -1;
		}
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
	SetType(TYPE_BRIDGE2);

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
	SetType(TYPE_TARGET);

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
			m_isHit = true;

			// 岩の転がる音を止める
			CManager::GetSound()->Stop(CSound::SOUND_LABEL_ROLL);

			break;
		}
	}
}


//=============================================================================
// 壁掛け松明ブロックのコンストラクタ
//=============================================================================
CTorchBlock::CTorchBlock()
{
	SetType(TYPE_TORCH);

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

	if (CManager::GetMode() == MODE_GAME)
	{
		D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
		D3DXVECTOR3 disPos = playerPos - GetPos();

		float distance = D3DXVec3Length(&disPos);

		const float kTriggerDistance = 980.0f; // 反応距離

		if (distance < kTriggerDistance)
		{
			// オフセット
			D3DXVECTOR3 localOffset(0.0f, 30.0f, -8.0f); // 松明の先端（ローカル）
			D3DXVECTOR3 worldOffset;

			// ブロックのワールドマトリックスを取得
			D3DXMATRIX worldMtx = GetWorldMatrix();

			D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

			// パーティクル生成
			CParticle::Create(worldOffset, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_FIRE, 1);
			CParticle::Create(worldOffset, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_FIRE, 1);
		}

		const float SoundTriggerDistance = 550.0f; // 反応距離

		if (distance < SoundTriggerDistance)
		{
			if (m_playedFireSoundID == -1) // 再生していなければ再生開始
			{
				// 前の音を止める（念のため）
				CManager::GetSound()->Stop(CSound::SOUND_LABEL_FIRE);

				// 3Dサウンド再生してIDを保持
				m_playedFireSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, SoundTriggerDistance);
			}

			// 音源の位置更新はIDを使う
			if (m_playedFireSoundID != -1)
			{
				CManager::GetSound()->UpdateSoundPosition(m_playedFireSoundID, GetPos());
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
		CParticle::Create(worldOffset, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_FIRE, 1);
		CParticle::Create(worldOffset, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_FIRE, 1);
	}
}


//=============================================================================
// 置き型松明ブロックのコンストラクタ
//=============================================================================
CTorch2Block::CTorch2Block()
{
	SetType(TYPE_TORCH2);

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

	const float kTriggerDistance = 1280.0f; // 反応距離

	if (distance < kTriggerDistance)
	{
		// オフセット
		D3DXVECTOR3 localOffset(0.0f, 30.0f, 0.0f); // 松明の先端（ローカル）
		D3DXVECTOR3 worldOffset;

		// ブロックのワールドマトリックスを取得
		D3DXMATRIX worldMtx = GetWorldMatrix();

		D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

		// パーティクル生成
		pParticle = CParticle::Create(worldOffset, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_FIRE, 1);
		pParticle = CParticle::Create(worldOffset, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_FIRE, 1);
	}

	const float SoundTriggerDistance = 550.0f; // 反応距離

	if (distance < SoundTriggerDistance)
	{
		if (m_playedFireSoundID == -1) // 再生していなければ再生開始
		{
			// 前の音を止める（念のため）
			CManager::GetSound()->Stop(CSound::SOUND_LABEL_FIRE);

			// 3Dサウンド再生してIDを保持
			m_playedFireSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, SoundTriggerDistance);
		}

		// 音源の位置更新はIDを使う
		if (m_playedFireSoundID != -1)
		{
			CManager::GetSound()->UpdateSoundPosition(m_playedFireSoundID, GetPos());
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

}


//=============================================================================
// 仮面ブロックのコンストラクタ
//=============================================================================
CMaskBlock::CMaskBlock()
{
	SetType(TYPE_MASK);

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

		const float kTriggerDistance = 1280.0f; // 反応距離

		if (distance < kTriggerDistance && !m_isGet)
		{
			// オフセット
			D3DXVECTOR3 localOffset(0.0f, 15.0f, 0.0f);
			D3DXVECTOR3 worldOffset;

			// ブロックのワールドマトリックスを取得
			D3DXMATRIX worldMtx = GetWorldMatrix();

			D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

			// パーティクル生成
			pParticle = CParticle::Create(worldOffset, D3DXCOLOR(0.6f, 0.6f, 1.0f, 0.3f), 50, CParticle::TYPE_AURA2, 1);
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

		if (distance < kTriggerDistance && !m_isGet)
		{
			if (m_playedSoundID == -1) // 再生していなければ再生開始
			{
				// 前の音を止める（念のため）
				CManager::GetSound()->Stop(CSound::SOUND_LABEL_MASK);

				// 3Dサウンド再生してIDを保持
				m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_MASK, GetPos(), 250.0f, 1100.0f);
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
}


//=============================================================================
// 剣ブロックのコンストラクタ
//=============================================================================
CSwordBlock::CSwordBlock()
{
	SetType(TYPE_SWORD);

	// 値のクリア
	m_isEnd = false;
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
			pParticle = CParticle::Create(worldOffset, D3DXCOLOR(0.6f, 0.6f, 0.0f, 0.3f), 50, CParticle::TYPE_AURA, 1);
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
		pParticle = CParticle::Create(worldOffset, D3DXCOLOR(0.6f, 0.6f, 0.0f, 0.3f), 50, CParticle::TYPE_AURA, 1);
	}
}


//=============================================================================
// 鉄格子ブロックのコンストラクタ
//=============================================================================
CBarBlock::CBarBlock()
{
	SetType(TYPE_BAR);

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
	SetType(TYPE_BRIDGE3);

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
