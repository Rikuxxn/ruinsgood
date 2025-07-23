//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "player.h"
#include "texture.h"
#include "manager.h"
#include "model.h"
#include "particle.h"
#include "game.h"

using namespace std;

//=============================================================================
// コンストラクタ
//=============================================================================
CPlayer::CPlayer(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	m_pos				= INIT_VEC3;					// 位置
	m_ResPos			= {};							// 復活する位置
	m_rot				= INIT_VEC3;					// 向き
	m_rotDest			= INIT_VEC3;					// 向き
	m_move				= INIT_VEC3;					// 移動量
	m_targetMove		= INIT_VEC3;					// 目標速度
	m_currentMove		= INIT_VEC3;					// 実際の移動速度
	m_size				= D3DXVECTOR3(1.0f, 1.0f, 1.0f);// サイズ
	m_mtxWorld			= {};							// ワールドマトリックス
	m_nNumModel			= 0;							// モデル(パーツ)の総数
	m_playerUse			= true;							// 使われているかどうか
	m_pShadowS			= NULL;							// ステンシルシャドウへのポインタ
	m_pMotion			= NULL;							// モーションへのポインタ
	m_currentMotion		= CMotion::TYPE_NEUTRAL;		// 現在のモーション
	m_isJumping			= false;						// ジャンプ中フラグ
	m_bIsMoving			= false;						// 移動入力フラグ
	m_bIsSideMoving		= false;						// 横移動入力フラグ
	m_pRigidBody		= NULL;							// 剛体へのポインタ
	m_bOnGround			= false;						// 接地フラグ
	m_pShape			= NULL;							// 当たり判定の形へのポインタ
	m_pDebug3D			= NULL;							// 3Dデバッグ表示へのポインタ
	m_radius			= 0.0f;							// カプセルコライダーの半径
	m_height			= 0.0f;							// カプセルコライダーの高さ
	m_colliderPos		= INIT_VEC3;
	m_jumpFrame			= 0;
	m_pCarryingBlock	= NULL;
	m_waterStayTime		= 0.0f;							// 水中滞在時間（秒）
	m_isInWater			= false;						// 今水中にいるか
	for (int nCnt = 0; nCnt < MAX_PARTS; nCnt++)
	{
		m_apModel[nCnt] = {};						// モデル(パーツ)へのポインタ
	}
}
//=============================================================================
// デストラクタ
//=============================================================================
CPlayer::~CPlayer()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CPlayer* CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CPlayer* pPlayer;

	pPlayer = new CPlayer;

	pPlayer->m_pos = pos;
	pPlayer->m_rot = D3DXToRadian(rot);

	// リスポーン位置の設定
	pPlayer->AddRespawnPoint(D3DXVECTOR3(427.0f, 30.0f, 1110.0f));
	pPlayer->AddRespawnPoint(D3DXVECTOR3(-1327.0f, 30.0f, -4586.0f));
	pPlayer->AddRespawnPoint(D3DXVECTOR3(2810.0f, 30.0f, -1518.0f));

	// 初期化処理
	pPlayer->Init();

	return pPlayer;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CPlayer::Init(void)
{
	CModel* pModels[MAX_PARTS];
	int nNumModels = 0;

	// パーツの読み込み
	m_pMotion = CMotion::Load("data/motion.txt", pModels, nNumModels);

	for (int nCnt = 0; nCnt < nNumModels && nCnt < MAX_PARTS; nCnt++)
	{
		m_apModel[nCnt] = pModels[nCnt];

		// オフセット考慮
		m_apModel[nCnt]->SetOffsetPos(m_apModel[nCnt]->GetPos());
		m_apModel[nCnt]->SetOffsetRot(m_apModel[nCnt]->GetRot());
	}

	// パーツ数を代入
	m_nNumModel = nNumModels;

	// プレイヤーが使われている
	m_playerUse = true;

	// 変数の初期化
	m_rot = D3DXVECTOR3(0.0f, -D3DX_PI, 0.0f);

	//*********************************************************************
	// Bullet Physics カプセルコライダーの設定
	//*********************************************************************

	m_radius = 18.5f;
	m_height = 55.5f;

	m_pShape = new btCapsuleShape(m_radius, m_height);

	// コライダー中心 = 足元 + オフセット
	m_colliderPos = m_pos + D3DXVECTOR3(0, 20.0f, 0);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(m_colliderPos.x, m_colliderPos.y, m_colliderPos.z));

	// 質量を設定
	btScalar mass = 1.0f;
	btVector3 inertia(0, 0, 0);  // 慣性

	m_pShape->calculateLocalInertia(mass, inertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, m_pShape, inertia);

	m_pRigidBody = new btRigidBody(rbInfo);

	m_pRigidBody->setAngularFactor(btVector3(0, 0, 0));
	m_pRigidBody->setFriction(0.0f);// 摩擦
	m_pRigidBody->setRollingFriction(0.0f);// 転がり摩擦
	//m_pRigidBody->setGravity(btVector3(0, -139.8f, 0)); // 重力

	m_pRigidBody->setUserPointer(this);
	m_pRigidBody->setActivationState(DISABLE_DEACTIVATION);// スリープ状態にしない

	// 物理ワールドに追加
	btDiscreteDynamicsWorld* pWorld = CManager::GetPhysicsWorld();

	if (pWorld != NULL)
	{
		pWorld->addRigidBody(m_pRigidBody);
	}

	// ステンシルシャドウの生成
	m_pShadowS = CShadowS::Create("data/MODELS/stencilshadow.x",m_pos);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CPlayer::Uninit(void)
{
	ReleasePhysics();

	for (int nCnt = 0; nCnt < MAX_PARTS; nCnt++)
	{
		if (m_apModel[nCnt] != NULL)
		{
			m_apModel[nCnt]->Uninit();
			delete m_apModel[nCnt];
			m_apModel[nCnt] = NULL;
		}
	}

	if (m_pMotion != NULL)
	{
		delete m_pMotion;
		m_pMotion = NULL;
	}

	// オブジェクトの破棄(自分自身)
	this->Release();
}
//=============================================================================
// 更新処理
//=============================================================================
void CPlayer::Update(void)
{
	m_bIsMoving = false;
	m_bIsSideMoving = false;

	// コントロール処理
	Controll();

	if (CManager::GetCamera()->GetMode() != CCamera::MODE_DIRECTION)
	{
		// ブロックを持つ処理
		HoldBlock();
	}

	CParticle* pParticle = NULL;

	// モーション切り替え
	if (m_isJumping)
	{
		m_jumpFrame++;

		if (m_jumpFrame > JUMP_HOLD_FRAMES && m_bOnGround)
		{
			m_isJumping = false;

			m_pMotion->StartBlendMotion(m_pMotion->TYPE_NEUTRAL, 10);
			m_currentMotion = m_pMotion->TYPE_NEUTRAL;
		}
	}
	else
	{
		if (m_bIsSideMoving)
		{
			if (m_currentMotion != m_pMotion->TYPE_BESIDEMOVE)
			{
				m_pMotion->StartBlendMotion(m_pMotion->TYPE_BESIDEMOVE, 10);
				m_currentMotion = m_pMotion->TYPE_BESIDEMOVE;
			}
		}
		else if (m_bIsMoving)
		{
			//pParticle = CParticle::Create(m_pos, D3DXCOLOR(0.6f, 0.6f, 0.6f, 0.3f), 30, CParticle::TYPE_DUSH, 1);

			if (m_currentMotion != m_pMotion->TYPE_MOVE)
			{
				m_pMotion->StartBlendMotion(m_pMotion->TYPE_MOVE, 10);
				m_currentMotion = m_pMotion->TYPE_MOVE;
			}
		}
		else
		{
			if (m_currentMotion != m_pMotion->TYPE_NEUTRAL)
			{
				m_pMotion->StartBlendMotion(m_pMotion->TYPE_NEUTRAL, 20);
				m_currentMotion = m_pMotion->TYPE_NEUTRAL;
			}
		}
	}

	// 向きの正規化
	if (m_rotDest.y - m_rot.y > D3DX_PI)
	{
		m_rot.y += D3DX_PI * 2.0f;
	}
	else if (m_rotDest.y - m_rot.y < -D3DX_PI)
	{
		m_rot.y -= D3DX_PI * 2.0f;
	}

	m_rot.y += (m_rotDest.y - m_rot.y) * 0.09f;


	// 現在位置を物理ワールドから取得して m_pos に反映
	btTransform trans;
	m_pRigidBody->getMotionState()->getWorldTransform(trans);
	btVector3 pos = trans.getOrigin();
	m_colliderPos = D3DXVECTOR3(pos.getX(), pos.getY(), pos.getZ());
	m_pos = m_colliderPos - D3DXVECTOR3(0, 50.0f, 0); // 足元へのオフセット

	if (m_pos.y < -480.0f)
	{
		// リスポーン処理
		RespawnToCheckpoint();
	}

	if (m_pShadowS != NULL)
	{
		// ステンシルシャドウの位置設定
		m_pShadowS->SetPosition(m_pos);
	}

	int nNumModels = 10;

	// モーションの更新処理
	m_pMotion->Update(m_apModel, nNumModels);
}
//=============================================================================
// 情報の更新処理
//=============================================================================
void CPlayer::UpdateInfo(void)
{
	if (ImGui::TreeNode("Player Info"))
	{
		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		//*********************************************************************
		// POS の調整
		//*********************************************************************

		// 位置
		ImGui::Text("POS"); ImGui::SameLine(70); // ラベル位置微調整

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80); // 幅を狭く
		ImGui::DragFloat("##Player_pos_x", &m_pos.x, 1.0f, -1000.0f, 1000.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_pos_y", &m_pos.y, 1.0f, -1000.0f, 1000.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_pos_z", &m_pos.z, 1.0f, -1000.0f, 1000.0f, "%.1f");

		//*********************************************************************
		// ROT の調整
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// 向き
		ImGui::Text("ROT"); ImGui::SameLine(70); // ラベル位置微調整

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80); // 幅を狭く
		ImGui::DragFloat("##Player_rot_x", &m_rot.x, 0.1f, -D3DX_PI, D3DX_PI, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_rot_y", &m_rot.y, 0.1f, -D3DX_PI, D3DX_PI, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_rot_z", &m_rot.z, 0.1f, -D3DX_PI, D3DX_PI, "%.1f");

		//*********************************************************************
		// SIZE の調整
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// ラベル
		ImGui::Text("SIZE"); ImGui::SameLine(70); // ラベルの位置ちょっと調整

		// X
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_size_x", &m_size.x, 0.1f, -100.0f, 100.0f, "%.1f");

		// Y
		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_size_y", &m_size.y, 0.1f, -100.0f, 100.0f, "%.1f");

		// Z
		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_size_z", &m_size.z, 0.1f, -100.0f, 100.0f, "%.1f");

		//*********************************************************************
		// カプセルコライダー の調整
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// ラベル
		ImGui::Text("Capsule Collider"); // ラベルの位置ちょっと調整

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// 半径
		ImGui::Text("Radius:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_Radius", &m_radius, 0.1f, -500.0f, 500.0f, "%.1f");

		// 高さ
		ImGui::Text("Height:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_Height", &m_height, 0.1f, -500.0f, 500.0f, "%.1f");

		ImGui::TreePop(); // 閉じる
	}

	// カプセルコライダー更新チェック
	static float oldRadius = -1.0f;
	static float oldHeight = -1.0f;

	if (m_radius != oldRadius || m_height != oldHeight)
	{
		// 既存のコリジョン形状を削除
		if (m_pShape)
		{
			delete m_pShape;
			m_pShape = NULL;
		}

		// 新しい Capsule Shape を作成
		m_pShape = new btCapsuleShape(m_radius, m_height);

		// リジッドボディに反映
		if (m_pRigidBody)
		{
			m_pRigidBody->setCollisionShape(m_pShape);
		}

		// 値を記録
		oldRadius = m_radius;
		oldHeight = m_height;
	}

}
//=============================================================================
// 描画処理
//=============================================================================
void CPlayer::Draw(void)
{
	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxSize;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// サイズを反映
	D3DXMatrixScaling(&mtxSize, m_size.x, m_size.y, m_size.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxSize);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	for (int nCntMat = 0; nCntMat < m_nNumModel; nCntMat++)
	{
		// モデル(パーツ)の描画
		if (m_apModel[nCntMat])
		{
			m_apModel[nCntMat]->Draw();
		}
	}

#ifdef _DEBUG

	// カプセルコライダーの描画
	if (m_pRigidBody && m_pShape)
	{
		btTransform transform;
		m_pRigidBody->getMotionState()->getWorldTransform(transform);

		m_pDebug3D->DrawCapsuleCollider((btCapsuleShape*)m_pShape, transform, D3DXCOLOR(1, 1, 1, 1));
	}

#endif

}
//=============================================================================
// 位置の取得
//=============================================================================
D3DXVECTOR3 CPlayer::GetPos(void)
{
	return m_pos;
}
//=============================================================================
// レイによる接触判定
//=============================================================================
bool CPlayer::OnGround(btDiscreteDynamicsWorld* world, btRigidBody* playerBody, float rayLength)
{
	btTransform trans;
	playerBody->getMotionState()->getWorldTransform(trans);
	btVector3 start = trans.getOrigin();
	btVector3 end = start - btVector3(0, rayLength, 0);

	struct RayResultCallback : public btCollisionWorld::ClosestRayResultCallback
	{
		RayResultCallback(const btVector3& from, const btVector3& to)
			: btCollisionWorld::ClosestRayResultCallback(from, to) {}
	};

	RayResultCallback rayCallback(start, end);
	world->rayTest(start, end, rayCallback);

	if (rayCallback.hasHit())
	{// 接触
		return true;
	}

	return false;
}
//=============================================================================
// Physicsの破棄
//=============================================================================
void CPlayer::ReleasePhysics(void)
{
	if (m_pRigidBody)
	{
		btDiscreteDynamicsWorld* pWorld = CManager::GetPhysicsWorld();
		if (pWorld) pWorld->removeRigidBody(m_pRigidBody);
		delete m_pRigidBody->getMotionState();
		delete m_pRigidBody;
		m_pRigidBody = NULL;
	}

	if (m_pShape)
	{
		delete m_pShape;
		m_pShape = NULL;
	}
}
//=============================================================================
// ブロックを持つ処理
//=============================================================================
void CPlayer::HoldBlock(void)
{
	// マウスの取得
	CInputMouse* pInputMouse = CManager::GetInputMouse();

	// ジョイパッドの取得
	CInputJoypad* pInputJoypad = CManager::GetInputJoypad();

	if (pInputMouse->GetPress(0) || pInputJoypad->GetPressR2())
	{
		if (!m_pCarryingBlock)
		{
			CBlock* target = FindFrontBlockByRaycast(50.0f);// レイの長さを設定

			if (target)
			{
				// ブロックの質量を取得
				float blockMass = target->GetMassByType(target->GetType());

				// ブロックのサイズ(拡大率)を取得
				D3DXVECTOR3 blocksize = target->GetSize();// 拡大率

				bool isTooLarge =
					blocksize.x >= 3.0f ||
					blocksize.y >= 3.0f ||
					blocksize.z >= 3.0f;

				if (blockMass > 5.0f || isTooLarge)
				{// 一定質量を超えたら または 一定サイズを超えたら
					return;
				}

				m_pCarryingBlock = target;
				m_pCarryingBlock->SetEditMode(true); // キネマティック化

				// Y軸のみ回転
				m_pCarryingBlock->GetRigidBody()->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
			}
		}
		else
		{
			//btRigidBody* pRigid = m_pCarryingBlock->GetRigidBody();

			// 持っている → プレイヤー前方に移動
			D3DXVECTOR3 targetPos = GetPos() + GetForward();

			targetPos.y = GetPos().y + 140.0f; // 高さ調整

			D3DXVECTOR3 currentPos = m_pCarryingBlock->GetPos();
			float moveSpeed = 10.0f; // 移動スピード

			// 補間する
			D3DXVECTOR3 newPos = Lerp(currentPos, targetPos, moveSpeed * 1.0f/60.0f);
			m_pCarryingBlock->SetPos(newPos);

			// プレイヤーの向きに合わせてY軸回転
			D3DXVECTOR3 rot = m_pCarryingBlock->GetRot();
			rot.x = 0.0f;                   // 水平を保つ
			rot.z = 0.0f;
			rot.y = m_rot.y;               // プレイヤーの向きと同じに
			m_pCarryingBlock->SetRot(rot);
		}
	}
	else
	{
		if (m_pCarryingBlock)
		{
			// 回転制限解除（自由に転がる）
			m_pCarryingBlock->GetRigidBody()->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));

			m_pCarryingBlock->SetEditMode(false); // 物理に戻す
			m_pCarryingBlock = NULL;
		}
	}
}
//=============================================================================
// プレイヤーの前方ベクトル取得
//=============================================================================
D3DXVECTOR3 CPlayer::GetForward(void) const
{
	// プレイヤーの回転角度（Y軸）から前方ベクトルを計算
	float yaw = m_rot.y;

	D3DXVECTOR3 forward(-sinf(yaw), 0.0f, -cosf(yaw));

	// 正規化する
	D3DXVec3Normalize(&forward, &forward);

	return forward;
}
//=============================================================================
// プレイヤーの前方レイ判定処理
//=============================================================================
CBlock* CPlayer::FindFrontBlockByRaycast(float rayLength)
{
	btDiscreteDynamicsWorld* world = CManager::GetPhysicsWorld();

	if (!world)
	{
		return NULL;
	}

	D3DXVECTOR3 from = GetPos() + D3DXVECTOR3(0.0f,20.0f,0.0f);
	D3DXVECTOR3 to = from + GetForward() * rayLength;

	btVector3 btFrom(from.x, from.y, from.z);
	btVector3 btTo(to.x, to.y, to.z);

	struct RayResultCallback : public btCollisionWorld::ClosestRayResultCallback
	{
		RayResultCallback(const btVector3& from, const btVector3& to)
			: btCollisionWorld::ClosestRayResultCallback(from, to) {}

		virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{
			// ブロック以外を無視する場合はここでフィルターしてもいい
			return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
		}
	};

	RayResultCallback rayCallback(btFrom, btTo);
	world->rayTest(btFrom, btTo, rayCallback);

	if (rayCallback.hasHit())
	{
		void* userPtr = rayCallback.m_collisionObject->getUserPointer();

		if (userPtr)
		{
			CBlock* pBlock = static_cast<CBlock*>(userPtr);

			if (!pBlock->IsStaticBlock())
			{
				return pBlock; // 動的なブロックが前方にあった
			}
		}
	}

	return NULL;
}
//=============================================================================
// コントロール処理
//=============================================================================
void CPlayer::Controll(void)
{
	// キーボードの取得
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	// ジョイパッドの取得
	CInputJoypad* pInputJoypad = CManager::GetInputJoypad();

	// ジョイスティック情報の取得
	XINPUT_STATE* pStick = CInputJoypad::GetStickAngle();

	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	// カメラの角度の取得
	D3DXVECTOR3 CamRot = pCamera->GetRot();

	// カメラモードの取得
	CCamera::MODE camMode = pCamera->GetMode();

	// 入力方向ベクトル
	D3DXVECTOR3 inputDir = { 0.0f, 0.0f, 0.0f };

	// ---------------------------
	// ゲームパッド入力
	// ---------------------------
	if (pInputJoypad->GetStick() == true && pStick != NULL)
	{
		float stickX = pStick->Gamepad.sThumbLX;
		float stickY = pStick->Gamepad.sThumbLY;

		float magnitude = sqrtf(stickX * stickX + stickY * stickY);

		const float DEADZONE = 10922.0f;
		if (magnitude >= DEADZONE)
		{
			// 正規化
			stickX /= magnitude;
			stickY /= magnitude;

			float normalizedMagnitude = (magnitude - DEADZONE) / (32767.0f - DEADZONE);
			normalizedMagnitude = std::min(normalizedMagnitude, 1.0f);

			stickX *= normalizedMagnitude;
			stickY *= normalizedMagnitude;

			float cameraYaw = CamRot.y;

			// カメラ向きで回転した移動ベクトル
			inputDir.x += -(stickX * cosf(cameraYaw) + stickY * sinf(cameraYaw));
			inputDir.z += stickX * sinf(-cameraYaw) + stickY * cosf(cameraYaw);

			// 回転補正（Z軸反転）
			inputDir.z = -inputDir.z;

			// 向き更新
			if (camMode == pCamera->MODE_EDIT)
			{
				m_rotDest.y = atan2f(-inputDir.x, -inputDir.z);
			}
			else
			{
				m_rotDest.y = CamRot.y;
			}

			// フラグ
			m_bIsMoving = true;

			// 横移動だけなら m_bIsSideMoving を立てる
			if (fabsf(stickX) > 0.5f && fabsf(stickY) < 0.3f)
			{
				m_bIsSideMoving = true;
			}
		}
	}

	// ---------------------------
	// キーボード入力
	// ---------------------------
	bool pressW = pInputKeyboard->GetPress(DIK_W);
	bool pressS = pInputKeyboard->GetPress(DIK_S);
	bool pressA = pInputKeyboard->GetPress(DIK_A);
	bool pressD = pInputKeyboard->GetPress(DIK_D);

	// 入力方向加算
	if (pressW)
	{
		inputDir += D3DXVECTOR3(-sinf(CamRot.y), 0, -cosf(CamRot.y));
	}
	if (pressS)
	{
		inputDir += D3DXVECTOR3(sinf(CamRot.y), 0, cosf(CamRot.y));
	}
	if (pressA)
	{
		inputDir += D3DXVECTOR3(cosf(CamRot.y), 0, -sinf(CamRot.y));
	}
	if (pressD)
	{
		inputDir += D3DXVECTOR3(-cosf(CamRot.y), 0, sinf(CamRot.y));
	}

	// フラグ判定（斜めはm_bIsMovingを優先）
	if ((pressW || pressS))
	{
		m_bIsMoving = true;
	}
	else if ((pressA || pressD) && !(pressW || pressS))
	{
		m_bIsSideMoving = true;
	}

	if (inputDir.x != 0.0f || inputDir.z != 0.0f)
	{
		D3DXVec3Normalize(&inputDir, &inputDir);
	}

	// RayCastで接地チェック
	m_bOnGround = OnGround(CManager::GetPhysicsWorld(), m_pRigidBody, 65.0f);

	// ジャンプ入力があって、かつ地面に立っている時だけジャンプ
	if (!m_isJumping &&
		(pInputKeyboard->GetTrigger(DIK_SPACE) || pInputJoypad->GetTrigger(pInputJoypad->JOYKEY_A)) &&
		m_bOnGround)
	{
		m_isJumping = true;
		m_jumpFrame = 0;

		// ジャンプインパルスをY方向に加える
		btVector3 jumpImpulse(0, MAX_JUMP_POWER, 0);
		m_pRigidBody->applyCentralImpulse(jumpImpulse);

		// ジャンプモーション
		m_pMotion->StartBlendMotion(m_pMotion->TYPE_JUMP, 10);
		m_currentMotion = m_pMotion->TYPE_JUMP;
	}

	// 現在の速度を取得
	btVector3 velocity = m_pRigidBody->getLinearVelocity();

	// ---------------------------
	// 横移動の目標速度を計算
	// ---------------------------
	if (m_bIsMoving || m_bIsSideMoving)
	{
		float moveSpeed = PLAYER_SPEED;

		// 横移動だけならスピード落とす
		if (m_bIsSideMoving && !m_bIsMoving && !camMode == pCamera->MODE_EDIT)
		{
			moveSpeed = SIDE_MOVE_SPEED;
		}

		m_targetMove = inputDir * moveSpeed;

		// 加速度補間（イージング）
		const float accelRate = 0.15f; // 数値を上げると早く加速する
		m_currentMove.x += (m_targetMove.x - m_currentMove.x) * accelRate;
		m_currentMove.z += (m_targetMove.z - m_currentMove.z) * accelRate;
	}
	else
	{
		// 移動しないときは徐々に減速
		m_targetMove = D3DXVECTOR3(0, 0, 0);
		m_currentMove *= 0.95f;
	}

	// 速度を設定
	velocity.setX(m_currentMove.x);
	velocity.setZ(m_currentMove.z);

	// 速度を物理エンジンにセット
	m_pRigidBody->setLinearVelocity(velocity);

	if (camMode == pCamera->MODE_EDIT)
	{
		// プレイヤーの向き更新
		if (m_bIsMoving || m_bIsSideMoving)
		{
			m_rotDest.y = atan2f(-inputDir.x, -inputDir.z);
		}
	}
	else
	{
		// プレイヤーの向き更新
		m_rotDest.y = CamRot.y;
	}
}
//=============================================================================
// 水中にいるフラグ設定処理
//=============================================================================
void CPlayer::SetInWater(bool flag)
{
	m_isInWater = flag;
}
//=============================================================================
// 水中滞在時間の設定
//=============================================================================
void CPlayer::AddWaterStayTime(float delta)
{
	if (m_isInWater)
	{
		m_waterStayTime += delta;

		if (m_waterStayTime >= 3.0f) // 3秒以上滞在したら
		{
			RespawnToCheckpoint(); // 任意の場所へリスポーン
			m_waterStayTime = 0.0f;
		}
	}
	else
	{
		m_waterStayTime = 0.0f;
	}
}
//=============================================================================
// 水中滞在時間のリセット
//=============================================================================
void CPlayer::ResetWaterStayTime(void)
{
	m_waterStayTime = 0.0f;
}
//=============================================================================
// リスポーン(一番近い位置)処理
//=============================================================================
void CPlayer::RespawnToCheckpoint(void)
{
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	D3DXVECTOR3 respawnPos = GetNearestRespawnPoint(); // 任意の保存位置

	m_pos = respawnPos;

	btRigidBody* pRigid = GetRigidBody();

	if (pRigid)
	{
		pRigid->setLinearVelocity(btVector3(0, 0, 0));
		pRigid->setAngularVelocity(btVector3(0, 0, 0));

		// ワールド座標更新
		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(respawnPos.x, respawnPos.y, respawnPos.z));

		pRigid->setWorldTransform(trans);

		if (pRigid->getMotionState())
		{
			pRigid->getMotionState()->setWorldTransform(trans);
		}
	}
}
//=============================================================================
// リスポーン(直接設定)処理
//=============================================================================
void CPlayer::RespawnToCheckpoint(D3DXVECTOR3 pos)
{
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	D3DXVECTOR3 respawnPos = pos; // 任意の位置

	m_pos = respawnPos;

	btRigidBody* pRigid = GetRigidBody();

	if (pRigid)
	{
		pRigid->setLinearVelocity(btVector3(0, 0, 0));
		pRigid->setAngularVelocity(btVector3(0, 0, 0));

		// ワールド座標更新
		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(respawnPos.x, respawnPos.y, respawnPos.z));

		pRigid->setWorldTransform(trans);

		if (pRigid->getMotionState())
		{
			pRigid->getMotionState()->setWorldTransform(trans);
		}
	}
}
//=============================================================================
// リスポーン位置の追加処理
//=============================================================================
void CPlayer::AddRespawnPoint(const D3DXVECTOR3& pos)
{
	m_ResPos.push_back(pos);
}
//=============================================================================
// 一番近いリスポーン位置を返す処理
//=============================================================================
D3DXVECTOR3 CPlayer::GetNearestRespawnPoint(void) const
{
	if (m_ResPos.empty())
	{
		return m_pos; // 何も登録されていない場合は今の位置
	}

	float minDistSq = FLT_MAX;
	D3DXVECTOR3 nearest = m_ResPos[0];

	for (const auto& pt : m_ResPos)
	{
		D3DXVECTOR3 diff = pt - m_pos;
		float distSq = D3DXVec3LengthSq(&diff);

		if (distSq < minDistSq)
		{
			minDistSq = distSq;
			nearest = pt;
		}
	}

	return nearest;
}
//=============================================================================
// 補間関数
//=============================================================================
D3DXVECTOR3 CPlayer::Lerp(const D3DXVECTOR3& a, const D3DXVECTOR3& b, float t)
{
	return a + (b - a) * t;
}
