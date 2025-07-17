//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "camera.h"
#include "renderer.h"
#include "manager.h"
#include "game.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CCamera::CCamera()
{
	// 値のクリア
	m_posV				= D3DXVECTOR3(0.0f, 0.0f, 0.0f);// 視点
	m_posVDest			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);// 目的の視点
	m_posR				= D3DXVECTOR3(0.0f, 0.0f, 0.0f);// 注視点
	m_posRDest			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);// 目的の注視点
	m_vecU				= D3DXVECTOR3(0.0f, 0.0f, 0.0f);// 上方向ベクトル
	m_mtxProjection		= {};							// プロジェクションマトリックス
	m_mtxView			= {};							// ビューマトリックス
	m_rot				= D3DXVECTOR3(0.0f, 0.0f, 0.0f);// 向き
	m_fDistance			= 0.0f;							// 視点から注視点の距離
#ifdef _DEBUG
	m_Mode = MODE_EDIT;									// カメラのモード
#else
	m_Mode = MODE_GAME;
#endif
}
//=============================================================================
// デストラクタ
//=============================================================================
CCamera::~CCamera()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CCamera::Init(void)
{	
	m_posV = D3DXVECTOR3(0.0f, 80.0f, -540.0f);
	m_posR = D3DXVECTOR3(0.0f, 80.0f, 0.0f);
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);// 固定でいい
	m_rot = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);
	m_fDistance = sqrtf(
		((m_posV.x - m_posR.x) * (m_posV.x - m_posR.x)) +
		((m_posV.y - m_posR.y) * (m_posV.y - m_posR.y)) +
		((m_posV.z - m_posR.z) * (m_posV.z - m_posR.z)));
	
#ifdef _DEBUG
	m_Mode = MODE_EDIT;									// カメラのモード
#else
	m_Mode = MODE_GAME;
#endif

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CCamera::Uninit(void)
{


}
//=============================================================================
// 更新処理
//=============================================================================
void CCamera::Update(void)
{
	// タイトル画面だったら
	if (CManager::GetMode() == MODE_TITLE)
	{// カメラの位置の設定
		m_posV = D3DXVECTOR3(-1235.5f, 292.0f, -2220.2f);
		m_posR = D3DXVECTOR3(-1524.1f, 142.5f, -1489.2f);
		m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);// 固定でいい
		m_rot = D3DXVECTOR3(0.19f, 2.77f, 0.0f);
		m_fDistance = sqrtf(
			((m_posV.x - m_posR.x) * (m_posV.x - m_posR.x)) +
			((m_posV.y - m_posR.y) * (m_posV.y - m_posR.y)) +
			((m_posV.z - m_posR.z) * (m_posV.z - m_posR.z)));
	}

#ifdef _DEBUG
	// キーボードの取得
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	if (m_Mode == MODE_EDIT && pInputKeyboard->GetTrigger(DIK_C) == true)
	{
		// ゲームカメラ
		m_Mode = MODE_GAME;
	}
	else if (m_Mode == MODE_GAME && pInputKeyboard->GetTrigger(DIK_C) == true)
	{
		// エディターカメラ
		m_Mode = MODE_EDIT;
	}
#endif

	switch (m_Mode)
	{
	case MODE_EDIT:

#ifdef _DEBUG
		if (CManager::GetMode() == MODE_TITLE)
		{
			return;
		}

		// エディターカメラの処理
		EditCamera();
#endif
		break;
	case MODE_GAME:
		// ゲームのカメラ処理
		GameCamera();

		break;
	}

}
//=============================================================================
// カメラ情報の更新処理
//=============================================================================
void CCamera::UpdateInfo(void)
{
	if (ImGui::TreeNode("Camera Info"))
	{
		ImGui::Text("Change Camera Mode : [Press C]");

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		ImGui::Text("PosV Move : [Press L ALT & MouseButton L]");
		ImGui::Text("PosR Move : [Press MouseButton R]");
		ImGui::Text("Zoom : [MouseWheel]");

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// 視点の位置
		ImGui::Text("PosV [X:%.1f Y:%.1f Z:%.1f]", m_posV.x, m_posV.y, m_posV.z);

		// 注視点の位置
		ImGui::Text("PosR [X:%.1f Y:%.1f Z:%.1f]", m_posR.x, m_posR.y, m_posR.z);

		// 角度
		ImGui::Text("Rot  [X:%.2f Y:%.2f Z:%.2f]", m_rot.x, m_rot.y, m_rot.z);

		ImGui::TreePop(); // 閉じる
	}
}
//=============================================================================
// カメラの設定処理
//=============================================================================
void CCamera::SetCamera(void)
{
	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	//ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	//ビューマトリックスの作成
	D3DXMatrixLookAtLH(&m_mtxView,
		&m_posV,
		&m_posR,
		&m_vecU);

	//ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);

	//プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	//プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
		D3DXToRadian(80.0f),						// 視野角
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, // アスペクト比
		1.0f,										// 近クリップ面
		2500.0f);									// 遠クリップ面

	//プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);
}
//=============================================================================
// エディターカメラの処理
//=============================================================================
void CCamera::EditCamera(void)
{
	// キーボードの取得
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	// マウスの取得
	CInputMouse* pInputMouse = CManager::GetInputMouse();

	// マウスカーソルを表示する
	pInputMouse->SetCursorVisibility(true);

	// 現在のカーソル位置を取得
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	// 前フレームからのマウス移動量を取得
	static POINT prevCursorPos = { cursorPos.x, cursorPos.y };
	float deltaX = (float)(cursorPos.x - prevCursorPos.x);
	float deltaY = (float)(cursorPos.y - prevCursorPos.y);

	// 現在のカーソル位置を保存（次のフレームでの比較用）
	prevCursorPos = cursorPos;

	// マウス感度
	const float mouseSensitivity = 0.004f;

	deltaX *= mouseSensitivity;
	deltaY *= mouseSensitivity;

	//====================================
	// マウスホイールでズームイン・アウト
	//====================================
	int wheel = pInputMouse->GetWheel();
	const float zoomSpeed = 15.0f;

	if (wheel != 0)
	{
		m_fDistance -= wheel * zoomSpeed;

		// カメラ距離制限
		if (m_fDistance < 100.0f)
		{
			m_fDistance = 100.0f;
		}
		if (m_fDistance > 800.0f)
		{
			m_fDistance = 800.0f;
		}
	}

	if (pInputKeyboard->GetPress(DIK_LALT) && pInputMouse->GetPress(0)) // 左クリック押しながらマウス移動 → 視点回転
	{
		m_rot.y += deltaX; // 水平回転
		m_rot.x += deltaY; // 垂直回転

		//角度の正規化
		if (m_rot.y > D3DX_PI)
		{
			m_rot.y -= D3DX_PI * 2.0f;
		}
		else if (m_rot.y < -D3DX_PI)
		{
			m_rot.y += D3DX_PI * 2.0f;
		}

		// 垂直回転の制限
		if (m_rot.x > 1.57f)
		{
			m_rot.x = 1.57f;
		}

		if (m_rot.x < -1.57f)
		{
			m_rot.x = -1.57f;
		}

		// 視点の更新（カメラの方向を適用）
		m_posV.x = m_posR.x + sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posV.y = m_posR.y + sinf(m_rot.x) * m_fDistance;
		m_posV.z = m_posR.z + cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;

	}
	else if (pInputMouse->GetPress(1)) // 右クリック押しながらマウス移動 → 注視点回転
	{
		m_rot.y += deltaX; // 水平回転
		m_rot.x += deltaY; // 垂直回転

		//角度の正規化
		if (m_rot.y > D3DX_PI)
		{
			m_rot.y -= D3DX_PI * 2.0f;
		}
		else if (m_rot.y < -D3DX_PI)
		{
			m_rot.y += D3DX_PI * 2.0f;
		}

		// 垂直回転の制限
		if (m_rot.x > 1.57f)
		{
			m_rot.x = 1.57f;
		}
		if (m_rot.x < -1.57f)
		{
			m_rot.x = -1.57f;
		}

		// 注視点の更新
		m_posR.x = m_posV.x - sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posR.y = m_posV.y - sinf(m_rot.x) * m_fDistance;
		m_posR.z = m_posV.z - cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	}
	else
	{
		// 入力がない場合でもズーム反映のために視点を更新
		m_posV.x = m_posR.x + sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posV.y = m_posR.y + sinf(m_rot.x) * m_fDistance;
		m_posV.z = m_posR.z + cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	}

	// 注視点の更新
	m_posR.x = m_posV.x - sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	m_posR.y = m_posV.y - sinf(m_rot.x) * m_fDistance;
	m_posR.z = m_posV.z - cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
}
//=============================================================================
// ゲームカメラの処理
//=============================================================================
void CCamera::GameCamera(void)
{
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	// 初期距離を短くする
	static bool firstTime = true;

	if (firstTime)
	{
		m_fDistance = 220.0f;
		firstTime = false;
	}

	// マウスの取得
	CInputMouse* pInputMouse = CManager::GetInputMouse();

	// スティックの取得
	XINPUT_STATE* pStick = CInputJoypad::GetStickAngle();

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	if (pPlayer != NULL)
	{
		// プレイヤーの位置の取得
		D3DXVECTOR3 playerPos = pPlayer->GetPos();

		// マウスの状態を取得
		DIMOUSESTATE mouseState;

		// マウスカーソルを非表示にする
		pInputMouse->SetCursorVisibility(false);

		// ゲームパッド右スティックカメラ操作
		if (pStick != NULL)
		{
			// 右スティックの値を取得
			float stickX = pStick->Gamepad.sThumbRX;
			float stickY = pStick->Gamepad.sThumbRY;

			// デッドゾーン処理
			const float DEADZONE = 10922.0f;
			if (fabsf(stickX) < DEADZONE)
			{
				stickX = 0.0f;
			}
			if (fabsf(stickY) < DEADZONE)
			{
				stickY = 0.0f;
			}

			// 正規化
			stickX /= 32768.0f;
			stickY /= 32768.0f;

			// カメラ回転の更新
			CameraWithGamepad(stickX, stickY);
		}
		if (pInputMouse->GetMouseState(&mouseState))
		{
			// 前フレームのカーソル位置を記録する静的変数
			static POINT prevCursorPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

			// 現在のマウスの位置の取得
			POINT cursorPos;
			GetCursorPos(&cursorPos);

			// マウス感度
			const float mouseSensitivity = 0.003f;

			float deltaX = (float)(cursorPos.x - prevCursorPos.x) * mouseSensitivity;
			float deltaY = (float)(cursorPos.y - prevCursorPos.y) * mouseSensitivity;

			prevCursorPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
			SetCursorPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

			m_rot.y += deltaX;
			m_rot.x += deltaY;

			// 上下回転制限
			if (m_rot.x < -1.3f)
			{
				m_rot.x = -1.3f;
			}
			if (m_rot.x > 1.3f)
			{
				m_rot.x = 1.3f;
			}

			//====================================
			// マウスホイールでズームイン・アウト
			//====================================
			int wheel = pInputMouse->GetWheel();
			const float zoomSpeed = 2.0f;

			if (wheel != 0)
			{
				m_fDistance -= wheel * zoomSpeed;

				// カメラ距離制限
				if (m_fDistance < 100.0f)
				{
					m_fDistance = 100.0f;
				}
				if (m_fDistance > 200.0f)
				{
					m_fDistance = 200.0f;
				}
			}
		}

		// カメラ位置計算
		m_posV.x = playerPos.x + sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posV.y = playerPos.y + sinf(m_rot.x) * m_fDistance + 80.0f;
		m_posV.z = playerPos.z + cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;

		// 注視点
		m_posR = playerPos;
		m_posR.y += 60.0f;

		// カメラの位置補正処理
		AdjustCameraPosition(playerPos);
	}
}
//=============================================================================
// ゲームパッドのカメラ回転処理
//=============================================================================
void CCamera::CameraWithGamepad(float stickX, float stickY)
{
	const float rotationSpeed = 0.07f; // 回転速度

	 // 水平方向の回転（Y軸）
	m_rot.y += stickX * rotationSpeed;

	// 垂直方向の回転（X軸）
	m_rot.x -= stickY * rotationSpeed;

	// 垂直方向の回転を制限 (-90度 ～ 90度)
	if (m_rot.x > D3DX_PI / 2)
	{
		m_rot.x = D3DX_PI / 2;
	}
	if (m_rot.x < -D3DX_PI / 2)
	{
		m_rot.x = -D3DX_PI / 2;
	}

	// 水平方向の回転を正規化 (-π ～ π)
	if (m_rot.y > D3DX_PI)
	{
		m_rot.y -= D3DX_PI * 2.0f;
	}
	else if (m_rot.y < -D3DX_PI)
	{
		m_rot.y += D3DX_PI * 2.0f;
	}
}
//=============================================================================
// カメラの位置補正(壁貫通をなくす)処理
//=============================================================================
void CCamera::AdjustCameraPosition(const D3DXVECTOR3& playerPos)
{
	// プレイヤーの頭位置を注視点とする
	D3DXVECTOR3 playerEye = playerPos;
	playerEye.y += 60.0f; // プレイヤーの頭の高さ
	m_posR = playerEye;

	// カメラの理想位置
	D3DXVECTOR3 offsetFromPlayer;
	offsetFromPlayer.x = sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	offsetFromPlayer.y = sinf(m_rot.x) * m_fDistance + 20.0f;
	offsetFromPlayer.z = cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;

	D3DXVECTOR3 idealCamPos = playerEye + offsetFromPlayer;

	// Bullet Physics でレイキャスト
	btVector3 from(playerEye.x, playerEye.y, playerEye.z);
	btVector3 to(idealCamPos.x, idealCamPos.y, idealCamPos.z);

	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
	CManager::GetPhysicsWorld()->rayTest(from, to, rayCallback);

	if (rayCallback.hasHit())
	{
		// 衝突対象のユーザーポインタからブロックを特定
		const btCollisionObject* hitObj = rayCallback.m_collisionObject;
		void* userPtr = hitObj->getUserPointer();

		if (userPtr != NULL)
		{
			CBlock* hitBlock = static_cast<CBlock*>(userPtr);

			// TYPE_WATER なら補正をスキップ
			if (hitBlock->GetType() == CBlock::TYPE_WATER)
			{
				m_posV = idealCamPos;
			}
			else
			{
				// 衝突点の少し手前にカメラを配置
				btVector3 hitPoint = rayCallback.m_hitPointWorld;
				btVector3 camDir = (from - hitPoint).normalized();
				hitPoint += camDir * 10.0f;

				m_posV = D3DXVECTOR3(hitPoint.x(), hitPoint.y(), hitPoint.z());
			}
		}
		else
		{// ユーザーポインタが null の場合

			// 補正しておく
			btVector3 hitPoint = rayCallback.m_hitPointWorld;
			btVector3 camDir = (from - hitPoint).normalized();
			hitPoint += camDir * 10.0f;

			m_posV = D3DXVECTOR3(hitPoint.x(), hitPoint.y(), hitPoint.z());
		}
	}
	else
	{// 衝突なし
		// 理想位置そのまま
		m_posV = idealCamPos;
	}

	// 注視点はプレイヤーの頭
	m_posR = playerEye;
}