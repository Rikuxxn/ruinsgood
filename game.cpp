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

	// ビルボードの生成
	m_pBillboard = CObjectBillboard::Create("data/TEXTURE/move.png", D3DXVECTOR3(145.0f, 130.0f, -20.0f), 85.0f, 20.0f);
	m_pBillboard = CObjectBillboard::Create("data/TEXTURE/jump.png", D3DXVECTOR3(-150.0f, 150.0f, 385.0f), 80.0f, 20.0f);
	m_pBillboard = CObjectBillboard::Create("data/TEXTURE/pick.png", D3DXVECTOR3(150.0f, 130.0f, 1220.0f), 80.0f, 20.0f);

	// プレイヤーの生成
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 100.0f, -300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	//m_pPlayer = CPlayer::Create(D3DXVECTOR3(-660.0f, 100.0f, -3898.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// JSONの読み込み
	m_pBlockManager->LoadFromJson("data/block_info.json");

	float fTimePosX = 760.0f;
	float fTimeWidth = 42.0f;
	float fTimeHeight = 58.0f;

	// タイムの生成
	m_pTime = CTime::Create(0, 0, fTimePosX, 10.0f, fTimeWidth, fTimeHeight);

	// コロンの生成
	m_pColon = CColon::Create(D3DXVECTOR3(fTimePosX + 2 * fTimeWidth, 10.0f, 0.0f), fTimeWidth / 2, fTimeHeight);

	// ポーズUIの生成
	m_pUi = CUi::Create(CUi::TYPE_PAUSE, "data/TEXTURE/ui_pause.png",D3DXVECTOR3(210.0f, 895.0f, 0.0f), 160.0f, 35.0f);

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

}