//=============================================================================
//
// マネージャー処理 [manager.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "manager.h"
#include "renderer.h"
#include "sound.h"
#include "game.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
CRenderer* CManager::m_pRenderer = NULL;
CInputKeyboard* CManager::m_pInputKeyboard = NULL;
CInputJoypad* CManager::m_pInputJoypad = NULL;
CInputMouse* CManager::m_pInputMouse = NULL;
CSound* CManager::m_pSound = NULL;
CTexture* CManager::m_pTexture = NULL;
CCamera* CManager::m_pCamera = NULL;
CLight* CManager::m_pLight = NULL;

CScene* CManager::m_pScene = NULL;
CFade* CManager::m_pFade = NULL;

btDiscreteDynamicsWorld* CManager::m_pDynamicsWorld = NULL;

//=============================================================================
// コンストラクタ
//=============================================================================
CManager::CManager()
{
	// 値のクリア
	m_fps					  = 0;
	m_pBroadphase			  = NULL;	// 衝突判定のクラスへのポインタ
	m_pCollisionConfiguration = NULL;	// 衝突検出の設定を管理するクラスへのポインタ
	m_pDispatcher			  = NULL;	// 実際に衝突判定処理を実行するクラスへのポインタ
	m_pSolver				  = NULL;	// 物理シミュレーションの制約ソルバーへのポインタ
}
//=============================================================================
// デストラクタ
//=============================================================================
CManager::~CManager()
{

}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd)
{
	// レンダラーの生成
	m_pRenderer = new CRenderer;

	// レンダラーの初期化処理
	if (FAILED(m_pRenderer->Init(hWnd, TRUE)))
	{
		return -1;
	}

	// キーボードの生成
	m_pInputKeyboard = new CInputKeyboard;

	// キーボードの初期化処理
	if (FAILED(m_pInputKeyboard->Init(hInstance, hWnd)))
	{
		return -1;
	}

	// ジョイパッドの生成
	m_pInputJoypad = new CInputJoypad;

	// ジョイパッドの初期化処理
	if (FAILED(m_pInputJoypad->Init()))
	{
		return E_FAIL;
	}

	// マウスの生成
	m_pInputMouse = new CInputMouse;

	// マウスの初期化処理
	if (FAILED(m_pInputMouse->Init(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	// サウンドの生成
	m_pSound = new CSound;

	// サウンドの初期化処理
	if (FAILED(m_pSound->Init(hWnd)))
	{
		return E_FAIL;
	}


	// Bullet物理ワールドの生成
	m_pBroadphase = new btDbvtBroadphase();
	m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);
	m_pSolver = new btSequentialImpulseConstraintSolver();

	m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);

	// 重力を設定
	m_pDynamicsWorld->setGravity(btVector3(0, -659.8f, 0));

	// カメラの生成
	m_pCamera = new CCamera;

	// カメラの初期化処理
	m_pCamera->Init();

	// ライトの生成
	m_pLight = new CLight;

	// ライトの初期化処理
	m_pLight->Init();

	// テクスチャの生成
	m_pTexture = new CTexture;

	// テクスチャの読み込み
	m_pTexture->Load();

	// タイトル画面
	m_pFade = CFade::Create(CScene::MODE_TITLE);

	// タイトル画面
	m_pScene = CScene::Create(CScene::MODE_TITLE);

	// タイトル画面だったら
	if (m_pScene->GetMode() == MODE_TITLE)
	{// カメラの位置の設定
		D3DXVECTOR3 posV(D3DXVECTOR3(-1235.5f, 292.0f, -2220.2f));
		D3DXVECTOR3 posR(D3DXVECTOR3(-1524.1f, 142.5f, -1489.2f));

		m_pCamera->SetPosV(posV);
		m_pCamera->SetPosR(posR);
		m_pCamera->SetRot(D3DXVECTOR3(0.19f, 2.77f, 0.0f));
		m_pCamera->SetDis(sqrtf(
			((posV.x - posR.x) * (posV.x - posR.x)) +
			((posV.y - posR.y) * (posV.y - posR.y)) +
			((posV.z - posR.z) * (posV.z - posR.z))));
	}

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CManager::Uninit(void)
{
	// すべてのオブジェクトの破棄
	CObject::ReleaseAll();

	// テクスチャの破棄
	if (m_pTexture != NULL)
	{
		// 全てのテクスチャの破棄
		m_pTexture->Unload();

		delete m_pTexture;
		m_pTexture = NULL;
	}

	// キーボードの終了処理
	m_pInputKeyboard->Uninit();

	// ジョイパッドの終了処理
	m_pInputJoypad->Uninit();

	// マウスの終了処理
	m_pInputMouse->Uninit();

	// サウンドの終了処理
	m_pSound->Uninit();

	// キーボードの破棄
	if (m_pInputKeyboard != NULL)
	{
		// レンダラーの終了処理
		m_pInputKeyboard->Uninit();

		delete m_pInputKeyboard;
		m_pInputKeyboard = NULL;
	}

	// ジョイパッドの破棄
	if (m_pInputJoypad != NULL)
	{
		// ジョイパッドの終了処理
		m_pInputJoypad->Uninit();

		delete m_pInputJoypad;
		m_pInputJoypad = NULL;
	}

	// マウスの破棄
	if (m_pInputMouse != NULL)
	{
		// マウスの終了処理
		m_pInputMouse->Uninit();

		delete m_pInputMouse;
		m_pInputMouse = NULL;
	}

	// サウンドの破棄
	if (m_pSound != NULL)
	{
		// マウスの終了処理
		m_pSound->Uninit();

		delete m_pSound;
		m_pSound = NULL;
	}

	// カメラの破棄
	if (m_pCamera != NULL)
	{
		delete m_pCamera;
		m_pCamera = NULL;
	}

	// ライトの破棄
	if (m_pLight != NULL)
	{
		delete m_pLight;
		m_pLight = NULL;
	}

	if (m_pDynamicsWorld)
	{
		// 先にワールドから全RigidBodyを外す→delete
		int num = m_pDynamicsWorld->getNumCollisionObjects();
		for (int i = num - 1; i >= 0; i--)
		{
			btCollisionObject* obj = m_pDynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);

			if (body && body->getMotionState())
			{
				delete body->getMotionState();
				m_pDynamicsWorld->removeCollisionObject(obj);
				delete obj;
			}
		}
		delete m_pDynamicsWorld;  delete m_pSolver;
		delete m_pDispatcher;     delete m_pCollisionConfiguration;
		delete m_pBroadphase;
	}

	// フェードの破棄
	if (m_pFade != NULL)
	{
		// フェードの終了処理
		m_pFade->Uninit();

		delete m_pFade;
		m_pFade = NULL;
	}

	// レンダラーの破棄
	if (m_pRenderer != NULL)
	{
		// レンダラーの終了処理
		m_pRenderer->Uninit();

		delete m_pRenderer;
		m_pRenderer = NULL;
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void CManager::Update(void)
{
	// キーボードの更新
	m_pInputKeyboard->Update();

	// フェードの更新処理
	if (m_pFade != NULL)
	{
		// フェードの更新処理
		m_pFade->Update();
	}

	m_pDynamicsWorld->stepSimulation((btScalar)m_fps);

	// ジョイパッドの更新
	m_pInputJoypad->Update();

	// マウスの更新
	m_pInputMouse->Update();

	// カメラの更新
	m_pCamera->Update();

	// ライトの更新
	m_pLight->Update();

	// レンダラーの更新
	m_pRenderer->Update();
}
//=============================================================================
// 描画処理
//=============================================================================
void CManager::Draw(void)
{
	// レンダラーの描画
	m_pRenderer->Draw(m_fps);
}
//=============================================================================
// モードの設定
//=============================================================================
void CManager::SetMode(CScene::MODE mode)
{
	// カメラの初期化処理
	m_pCamera->Init();

	// サウンドの停止
	m_pSound->Stop();

	if (m_pScene != NULL)
	{	// 現在のモード破棄
		m_pScene->Uninit();
	}

	// 全てのオブジェクトを破棄
	CObject::ReleaseAll();

	// 新しいモードの生成
	m_pScene = CScene::Create(mode);
}
//=============================================================================
// 現在のモードの取得
//=============================================================================
CScene::MODE CManager::GetMode(void)
{
	return m_pScene->GetMode();
}
