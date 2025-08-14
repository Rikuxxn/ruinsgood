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
	m_pVtxBuff = NULL;		// 頂点バッファへのポインタ
	m_pos = INIT_VEC3;		// 位置
	m_col = INIT_XCOL;		// 色
	m_fWidth = 0.0f;
	m_fHeight = 0.0f;
	m_nIdxTexture = 0;
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

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャの取得
	m_nIdxTexture = CManager::GetTexture()->Register("data/TEXTURE/resultBG.png");

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 初期化
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fWidth = 1920.0f;
	m_fHeight = 1080.0f;

	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x, m_pos.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x, m_pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y + m_fHeight, 0.0f);

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	float fTimePosX = 1100.0f;
	float fTimeWidth = 72.0f;
	float fTimeHeight = 88.0f;

	// タイムの生成
	m_pTime = CTime::Create(m_nClearMinutes, m_nClearSeconds, fTimePosX, 695.0f, fTimeWidth, fTimeHeight);

	// コロンの生成
	m_pColon = CColon::Create(D3DXVECTOR3(fTimePosX + 2 * fTimeWidth, 695.0f, 0.0f), fTimeWidth / 2, fTimeHeight);

	// UIの生成
	m_pUi = CUi::Create(CUi::TYPE_RESULT01, "data/TEXTURE/ui_result001.png",D3DXVECTOR3(450.0f, 575.0f, 0.0f), 260.0f, 50.0f);
	m_pUi = CUi::Create(CUi::TYPE_RESULT01, "data/TEXTURE/ui_result002.png", D3DXVECTOR3(1250.0f, 595.0f, 0.0f), 260.0f, 50.0f);
	m_pUi = CUi::Create(CUi::TYPE_RESULT01, "data/TEXTURE/ui_result005.png", D3DXVECTOR3(860.0f, 155.0f, 0.0f), 150.0f, 60.0f);

	// UI(ランク)の生成
	m_pUi = CUi::Create(CUi::TYPE_RESULT03, "data/TEXTURE/ui_result006.png", D3DXVECTOR3(860.0f, 355.0f, 0.0f), 120.0f, 130.0f);

	int stageId = CStageSelect::GetSelectedStage();

	switch (stageId)
	{
	case CStage::STAGE_ID_1:
		if (m_isMaskGet)
		{
			// UI(発見したかどうか)の生成
			m_pUi = CUi::Create(CUi::TYPE_RESULT02, "data/TEXTURE/ui_result004.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 120.0f);
		}
		else if (!m_isMaskGet)
		{
			// UI(発見したかどうか)の生成
			m_pUi = CUi::Create(CUi::TYPE_RESULT02, "data/TEXTURE/ui_result003.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 120.0f);
		}
		break;
	case CStage::STAGE_ID_2:
		if (m_isMaskGet)
		{
			// UI(発見したかどうか)の生成
			m_pUi = CUi::Create(CUi::TYPE_RESULT02, "data/TEXTURE/ui_result004.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 120.0f);
		}
		else if (!m_isMaskGet)
		{
			// UI(発見したかどうか)の生成
			m_pUi = CUi::Create(CUi::TYPE_RESULT02, "data/TEXTURE/ui_result003.png", D3DXVECTOR3(450.0f, 755.0f, 0.0f), 190.0f, 120.0f);
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

	// 頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}
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
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, CManager::GetTexture()->GetAddress(m_nIdxTexture));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
