//=============================================================================
//
// リザルト処理 [result.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "result.h"
#include "input.h"
#include "manager.h"
#include "stageselect.h"
#include "background.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
CTime* CResult::m_pTime = NULL;					// タイムへのポインタ
CColon* CResult::m_pColon = NULL;				// コロンへのポインタ
int CResult::m_nClearMinutes = 0;
int CResult::m_nClearSeconds = 0;
bool CResult::m_isMaskGet = false;
CUi* CResult::m_pUi = NULL;						// UIへのポインタ

//=============================================================================
// コンストラクタ
//=============================================================================
CResult::CResult() : CScene(CScene::MODE_RESULT)
{
	// 値のクリア
}
//=============================================================================
// デストラクタ
//=============================================================================
CResult::~CResult()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CResult::Init(void)
{
	// マウスカーソルを表示する
	CManager::GetInputMouse()->SetCursorVisibility(true);

	// 背景の生成
	CBackground::Create(D3DXVECTOR3(960.0f, 540.0f, 0.0f), 960.0f, 540.0f, "data/TEXTURE/resultBG.png");

	float fTimePosX = 1100.0f;
	float fTimeWidth = 72.0f;
	float fTimeHeight = 88.0f;

	// タイムの生成
	m_pTime = CTime::Create(m_nClearMinutes, m_nClearSeconds, fTimePosX, 695.0f, fTimeWidth, fTimeHeight);

	// コロンの生成
	m_pColon = CColon::Create(D3DXVECTOR3(fTimePosX + 2 * fTimeWidth, 695.0f, 0.0f), fTimeWidth / 2, fTimeHeight);

	// UIの生成
	m_pUi = CUi::Create(CUi::TYPE_RESULT_UI, "data/TEXTURE/ui_result001.png",D3DXVECTOR3(450.0f, 575.0f, 0.0f), 260.0f, 50.0f);
	m_pUi = CUi::Create(CUi::TYPE_RESULT_UI, "data/TEXTURE/ui_result002.png", D3DXVECTOR3(1250.0f, 595.0f, 0.0f), 260.0f, 50.0f);
	m_pUi = CUi::Create(CUi::TYPE_RESULT_UI, "data/TEXTURE/ui_result005.png", D3DXVECTOR3(860.0f, 155.0f, 0.0f), 150.0f, 60.0f);

	// UI(ランク)の生成
	m_pUi = CUi::Create(CUi::TYPE_RESULT_RANK, "data/TEXTURE/ui_result006.png", D3DXVECTOR3(860.0f, 355.0f, 0.0f), 120.0f, 130.0f);

	int stageId = CStageSelect::GetSelectedStage();

	switch (stageId)
	{
	case CStage::STAGE_ID_1:
		if (m_isMaskGet)
		{
			// UI(発見したかどうか)の生成
			m_pUi = CUi::Create(CUi::TYPE_RESULT_UI, "data/TEXTURE/ui_result004.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 120.0f);
		}
		else if (!m_isMaskGet)
		{
			// UI(発見したかどうか)の生成
			m_pUi = CUi::Create(CUi::TYPE_RESULT_UI, "data/TEXTURE/ui_result003.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 120.0f);
		}
		break;
	case CStage::STAGE_ID_2:

		if (m_isMaskGet)
		{
			// UI(発見したかどうか)の生成
			m_pUi = CUi::Create(CUi::TYPE_RESULT_UI, "data/TEXTURE/ui_eggfound.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 190.0f);
		}
		else if (!m_isMaskGet)
		{
			// UI(発見したかどうか)の生成
			m_pUi = CUi::Create(CUi::TYPE_RESULT_UI, "data/TEXTURE/ui_eggnotfound.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 190.0f);
		}
		break;
	case CStage::STAGE_ID_3:
		break;
	case CStage::STAGE_ID_NONE:
		break;
	};

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CResult::Uninit(void)
{
	SetGet(false);

}
//=============================================================================
// 更新処理
//=============================================================================
void CResult::Update(void)
{
	CInputMouse* pInputMouse = CManager::GetInputMouse();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();
	CFade* pFade = CManager::GetFade();

	if (pFade->GetFade() == CFade::FADE_NONE && (pInputMouse->GetTrigger(0) || pJoypad->GetTrigger(CInputJoypad::JOYKEY_A)))
	{
		// ゲーム画面に移行
		pFade->SetFade(MODE_TITLE);
	}

#ifdef _DEBUG
	if (pFade->GetFade() == CFade::FADE_NONE && CManager::GetInputKeyboard()->GetTrigger(DIK_RETURN))
	{
		// ゲーム画面に移行
		pFade->SetFade(MODE_TITLE);
	}
#endif
}
//=============================================================================
// 描画処理
//=============================================================================
void CResult::Draw(void)
{


}
