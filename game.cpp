//=============================================================================
//
// ゲーム処理 [game.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "game.h"
#include "manager.h"
#include "result.h"
#include "stageselect.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
CPlayer* CGame::m_pPlayer = NULL;
CTime* CGame::m_pTime = NULL;					// タイムへのポインタ
CColon* CGame::m_pColon = NULL;					// コロンへのポインタ
CBlock* CGame::m_pBlock= NULL;					// ブロックへのポインタ
CBlockManager* CGame::m_pBlockManager= NULL;	// ブロックマネージャーへのポインタ
CImGuiManager* CGame::m_pImGuiManager= NULL;	// ImGuiマネージャーへのポインタ
CObjectBillboard* CGame::m_pBillboard = NULL;	// ビルボードへのポインタ
CUi* CGame::m_pUi = NULL;						// UIへのポインタ
CPauseManager* CGame::m_pPauseManager = NULL;	// ポーズマネージャーへのポインタ
CHintText* CGame::m_pHint = NULL;			// ヒント壁画へのポインタ
bool CGame::m_isPaused = false;					// trueならポーズ中

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

	// ステージIDの取得
	int stageId = CStageSelect::GetSelectedStage();

	// ステージごとのオブジェクトの読み込み処理
	LoadObject(stageId);

	float fTimePosX = 760.0f;
	float fTimeWidth = 42.0f;
	float fTimeHeight = 58.0f;

	// タイムの生成
	m_pTime = CTime::Create(0, 0, fTimePosX, 10.0f, fTimeWidth, fTimeHeight);

	// コロンの生成
	m_pColon = CColon::Create(D3DXVECTOR3(fTimePosX + 2 * fTimeWidth, 10.0f, 0.0f), fTimeWidth / 2, fTimeHeight);

	// ポーズUIの生成
	m_pUi = CUi::Create(CUi::TYPE_PAUSE, "data/TEXTURE/ui_pause.png",D3DXVECTOR3(210.0f, 855.0f, 0.0f), 160.0f, 35.0f);

	// ポーズマネージャーの生成
	m_pPauseManager = new CPauseManager();

	// ポーズマネージャーの初期化
	m_pPauseManager->Init();

	CResult::SetGet(false);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CGame::Uninit(void)
{
	// 音の停止
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_GAMEBGM);

	// ブロックマネージャーの破棄
	if (m_pBlockManager != NULL)
	{
		m_pBlockManager->Uninit();

		delete m_pBlockManager;
		m_pBlockManager = NULL;
	}

	// ポーズマネージャーの破棄
	if (m_pPauseManager != NULL)
	{
		// ポーズマネージャーの終了処理
		m_pPauseManager->Uninit();

		delete m_pPauseManager;
		m_pPauseManager = NULL;
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void CGame::Update(void)
{
	CFade* pFade = CManager::GetFade();
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	CInputMouse* pMouse = CManager::GetInputMouse();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// TABキーでポーズON/OFF
	if (pKeyboard->GetTrigger(DIK_TAB) || pJoypad->GetTrigger(CInputJoypad::JOYKEY_START))
	{
		// ポーズSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_PAUSE);

		// ポーズ切り替え前の状態を記録
		bool wasPaused = m_isPaused;

		m_isPaused = !m_isPaused;

		// ポーズ状態に応じて音を制御
		if (m_isPaused && !wasPaused)
		{
			// 一時停止する
			CManager::GetSound()->PauseAll();
		}
		else if (!m_isPaused && wasPaused)
		{
			// 再開する
			CManager::GetSound()->ResumeAll();
		}
	}

	// ブロックマネージャーの更新処理
	m_pBlockManager->Update();

	// 終了判定チェック
	if (m_pBlockManager)
	{
		for (auto block : m_pBlockManager->GetAllBlocks())
		{
			if (block->IsGet())
			{// 仮面を手に入れたか
				CResult::SetGet(true);
			}

			if (block->IsEnd())
			{
				// リザルトにセット
				CResult::SetClearTime(m_pTime->GetMinutes(), m_pTime->GetnSeconds());

				// リザルト画面に移行
				pFade->SetFade(MODE_RESULT);

				break;
			}
		}
	}

#ifdef _DEBUG
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	if (pFade->GetFade() == CFade::FADE_NONE && pInputKeyboard->GetTrigger(DIK_RETURN))
	{
		// リザルトにセット
		CResult::SetClearTime(m_pTime->GetMinutes(), m_pTime->GetnSeconds());

		// リザルト画面に移行
		pFade->SetFade(MODE_RESULT);
	}
#endif
}
//=============================================================================
// 描画処理
//=============================================================================
void CGame::Draw(void)
{
	// ブロックマネージャーの描画処理
	m_pBlockManager->Draw();

	// ポーズ中だったら
	if (m_isPaused)
	{
		// ポーズマネージャーの描画処理
		m_pPauseManager->Draw();
	}
}
//=============================================================================
// ステージごとのオブジェクトの読み込み処理
//=============================================================================
void CGame::LoadObject(int stageId)
{
	switch (stageId)
	{
	case CStage::STAGE_ID_1:
		// ゲームBGMの再生
		CManager::GetSound()->Play(CSound::SOUND_LABEL_GAMEBGM);

		// ビルボードの生成
		m_pBillboard = CObjectBillboard::Create(CObjectBillboard::TYPE_NORMAL, "data/TEXTURE/ui_move.png", D3DXVECTOR3(145.0f, 130.0f, -20.0f), 85.0f, 0.0f);
		m_pBillboard = CObjectBillboard::Create(CObjectBillboard::TYPE_NORMAL, "data/TEXTURE/ui_jump.png", D3DXVECTOR3(-150.0f, 150.0f, 385.0f), 80.0f, 0.0f);
		m_pBillboard = CObjectBillboard::Create(CObjectBillboard::TYPE_NORMAL, "data/TEXTURE/ui_pick.png", D3DXVECTOR3(150.0f, 130.0f, 1220.0f), 80.0f, 0.0f);

		// プレイヤーの生成
		m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 100.0f, -300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		//m_pPlayer = CPlayer::Create(D3DXVECTOR3(-660.0f, 100.0f, -3898.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

		// UIの生成
		m_pUi = CUi::Create(CUi::TYPE_STAGE_NAME, "data/TEXTURE/stage_01.png", D3DXVECTOR3(860.0f, 480.0f, 0.0f), 230.0f, 50.0f);

		// JSONの読み込み
		m_pBlockManager->LoadFromJson("data/stage_01.json");

		break;
	case CStage::STAGE_ID_2:
		// ゲームBGMの再生
		CManager::GetSound()->Play(CSound::SOUND_LABEL_GAMEBGM2);

		// ヒント壁画の生成
		m_pHint = CHintText::Create(CHintText::TYPE_TEXT, "data/TEXTURE/text.png", D3DXVECTOR3(-1736.0f, 308.0f, 723.0f), D3DXVECTOR3(90.0f, -90.0f, 0.0f), 110.0f, 40.0f);
		m_pHint = CHintText::Create(CHintText::TYPE_NORMAL,"data/TEXTURE/hint3.png", D3DXVECTOR3(1422.0f, 308.0f, 940.0f), D3DXVECTOR3(90.0f, -90.0f, 0.0f), 65.0f, 65.0f);
		m_pHint = CHintText::Create(CHintText::TYPE_NORMAL,"data/TEXTURE/hint3.png", D3DXVECTOR3(1742.0f, 308.0f, 940.0f), D3DXVECTOR3(90.0f, -90.0f, 0.0f), 65.0f, 65.0f);
		m_pHint = CHintText::Create(CHintText::TYPE_NORMAL,"data/TEXTURE/hint3.png", D3DXVECTOR3(1422.0f, 308.0f, 510.0f), D3DXVECTOR3(90.0f, -90.0f, 0.0f), 65.0f, 65.0f);
		m_pHint = CHintText::Create(CHintText::TYPE_TEXT, "data/TEXTURE/text.png", D3DXVECTOR3(1252.0f, 308.0f, 723.0f), D3DXVECTOR3(90.0f, 90.0f, 0.0f), 110.0f, 40.0f);

		// ビルボードの生成
		m_pBillboard = CObjectBillboard::Create(CObjectBillboard::TYPE_HINT, "data/TEXTURE/hint4.png", D3DXVECTOR3(-1736.0f, 360.0f, 723.0f), 80.0f, -50.0f);
		m_pBillboard = CObjectBillboard::Create(CObjectBillboard::TYPE_HINT, "data/TEXTURE/hint6.png", D3DXVECTOR3(1252.0f, 360.0f, 723.0f), 80.0f, -50.0f);

		// プレイヤーの生成
		m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 100.0f, -300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		//m_pPlayer = CPlayer::Create(D3DXVECTOR3(1540.0f, 420.0f, -1296.5f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

		m_pUi = CUi::Create(CUi::TYPE_STAGE_NAME, "data/TEXTURE/stage_02.png", D3DXVECTOR3(860.0f, 480.0f, 0.0f), 230.0f, 50.0f);

		// JSONの読み込み
		m_pBlockManager->LoadFromJson("data/stage_02.json");

		break;
	case CStage::STAGE_ID_3:
		// プレイヤーの生成
		m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 100.0f, -300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

		// JSONの読み込み
		m_pBlockManager->LoadFromJson("data/stage_03.json");

		break;
	case CStage::STAGE_ID_NONE:
		break;
	}
}
//=============================================================================
// ポーズの設定
//=============================================================================
void CGame::SetEnablePause(bool bPause)
{
	m_isPaused = bPause;

	if (bPause)
	{
		// 音を一時停止
		CManager::GetSound()->PauseAll();
	}
	else
	{
		// 音を再開
		CManager::GetSound()->ResumeAll();
	}
}
