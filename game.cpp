//=============================================================================
//
// ゲーム処理 [game.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "game.h"
#include "manager.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
CPlayer* CGame::m_pPlayer = NULL;
CScore* CGame::m_pScore = NULL;					// スコアへのポインタ
CTime* CGame::m_pTime = NULL;						// タイムへのポインタ
CColon* CGame::m_pColon = NULL;					// コロンへのポインタ
CPause* CGame::m_pPause= NULL;					// ポーズへのポインタ
CBlock* CGame::m_pBlock= NULL;					// ブロックへのポインタ
CBlockManager* CGame::m_pBlockManager= NULL;		// ブロックマネージャーへのポインタ
CImGuiManager* CGame::m_pImGuiManager= NULL;		// ImGuiマネージャーへのポインタ

bool CGame::m_isPaused = false;						// trueならポーズ中

//=============================================================================
// コンストラクタ
//=============================================================================
CGame::CGame() : CScene(CScene::MODE_GAME)
{
	// 値のクリア
}
//=============================================================================
// デストラクタ
//=============================================================================
CGame::~CGame()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CGame::Init(void)
{
	// ブロックマネージャーの生成
	m_pBlockManager = new CBlockManager;

	// ブロックマネージャーの初期化
	m_pBlockManager->Init();

	// プレイヤーの生成
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 100.0f, -300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	//m_pPlayer = CPlayer::Create(D3DXVECTOR3(-660.0f, 100.0f, -3898.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// JSONの読み込み
	m_pBlockManager->LoadFromJson("data/block_info.json");

	//// スコアの生成
	//m_pScore = CScore::Create(920.0f, 10.0f, 42.0f, 58.0f);

	float fTimePosX = 760.0f;
	float fTimeWidth = 42.0f;
	float fTimeHeight = 58.0f;

	// タイムの生成
	m_pTime = CTime::Create(12, 15, fTimePosX, 10.0f, fTimeWidth, fTimeHeight);

	// コロンの生成
	m_pColon = CColon::Create(D3DXVECTOR3(fTimePosX + 2 * fTimeWidth, 10.0f, 0.0f), fTimeWidth / 2, fTimeHeight);

	// ポーズの生成
	m_pPause = CPause::Create(D3DXVECTOR3(490.0f, 340.0f, 0.0f), 400.0f, 90.0f);

	//m_pSound->Play(CSound::SOUND_LABEL_GAMEBGM);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CGame::Uninit(void)
{
	// ブロックマネージャーの破棄
	if (m_pBlockManager != NULL)
	{
		m_pBlockManager->Uninit();

		delete m_pBlockManager;
		m_pBlockManager = NULL;
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void CGame::Update(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();
	CInputMouse* pInputMouse = CManager::GetInputMouse();
	CFade* pFade = CManager::GetFade();
	CBlock* pBlock = GetBlock();

	// PキーでポーズON/OFF
	if (pInputKeyboard->GetTrigger(DIK_P))
	{
		m_isPaused = !m_isPaused;
	}

	// ポーズ中はゲーム更新しない
	if (m_isPaused == true)
	{
		// ポーズの更新処理
		m_pPause->Update();

		return;
	}

	// ブロックマネージャーの更新処理
	m_pBlockManager->Update();

	if (pInputKeyboard->GetTrigger(DIK_RETURN))
	{
		// ゲーム画面に移行
		pFade->SetFade(MODE_RESULT);
	}
}
//=============================================================================
// 描画処理
//=============================================================================
void CGame::Draw(void)
{
	// ポーズ中だったら
	if (m_isPaused)
	{
		// ポーズの描画処理
		m_pPause->Draw();
	}
}