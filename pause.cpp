//=============================================================================
//
// ポーズ処理 [pause.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "pause.h"
#include "renderer.h"
#include "manager.h"
#include "game.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CPause::CPause(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	m_pVtxBuff		= NULL;
	m_pVtxBuffBack	= NULL;
	m_pos			= INIT_VEC3;
	m_fWidth		= 0.0f;
	m_fHeight		= 0.0f;
	m_nIdxTexture	= 0;
	memset(m_szPath, 0, sizeof(m_szPath));
	m_isSelected = false;
}
//=============================================================================
// デストラクタ
//=============================================================================
CPause::~CPause()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CPause* CPause::Create(MENU type,D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	CPause* pPause = NULL;

	switch (type)
	{
	case MENU_CONTINUE:
		pPause = new CContinue;
		pPause->SetPath("data/TEXTURE/continue.png");
		break;
	case MENU_RETRY:
		pPause = new CRetry;
		pPause->SetPath("data/TEXTURE/retry.png");
		break;
	case MENU_QUIT:
		pPause = new CQuit;
		pPause->SetPath("data/TEXTURE/quit.png");
		break;
	default:
		pPause = new CPause;
		break;
	}

	pPause->SetPos(pos);
	pPause->m_fWidth = fWidth;
	pPause->m_fHeight = fHeight;
	pPause->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	// 初期化処理
	pPause->Init();

	return pPause;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CPause::Init(void)
{
	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register(m_szPath);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffBack,
		NULL);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuffBack->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(1920.0f, 0.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f, 1080.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(1920.0f, 1080.0f, 0.0f);

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuffBack->Unlock();

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_fWidth, m_pos.y - m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y - m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_fWidth, m_pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y + m_fHeight, 0.0f);

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CPause::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// 頂点バッファの破棄
	if (m_pVtxBuffBack != NULL)
	{
		m_pVtxBuffBack->Release();
		m_pVtxBuffBack = NULL;
	}

	this->Release();
}
//=============================================================================
// 更新処理
//=============================================================================
void CPause::Update(void)
{
	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 選択項目の色の切り替え
	if (IsMouseOver() || m_isSelected)
	{
		m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	}

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_fWidth, m_pos.y - m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y - m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_fWidth, m_pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y + m_fHeight, 0.0f);

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}
//=============================================================================
// 描画処理
//=============================================================================
void CPause::Draw(void)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// ポーズ状態だったら
	if (CManager::GetisPaused())
	{
		// デバイスの取得
		CRenderer* renderer = CManager::GetRenderer();
		LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

		//=============================================
		// 背景
		//=============================================
		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, m_pVtxBuffBack, 0, sizeof(VERTEX_2D));

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_2D);

		// テクスチャの設定
		pDevice->SetTexture(0, NULL);

		//// ポリゴンの描画
		//pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		//=============================================
		// ポーズ
		//=============================================
		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_2D);

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}
}
//=============================================================================
// 位置の取得処理
//=============================================================================
D3DXVECTOR3 CPause::GetPos(void)
{
	return m_pos;
}
//=============================================================================
// マウスカーソルの判定処理
//=============================================================================
bool CPause::IsMouseOver(void)
{
	// マウスカーソルの位置を取得
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	// ウィンドウハンドルを取得
	HWND hwnd = GetActiveWindow();

	// スクリーン座標をクライアント座標に変換
	ScreenToClient(hwnd, &cursorPos);

	// クライアントサイズを取得
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	float left = m_pos.x - m_fWidth;
	float right = m_pos.x + m_fWidth;
	float top = m_pos.y - m_fHeight;
	float bottom = m_pos.y + m_fHeight;

	return (cursorPos.x >= left && cursorPos.x <= right &&
		cursorPos.y >= top && cursorPos.y <= bottom);
}


//=============================================================================
// コンティニュー項目のコンストラクタ
//=============================================================================
CContinue::CContinue()
{
	// 値のクリア

}
//=============================================================================
// コンティニュー項目のデストラクタ
//=============================================================================
CContinue::~CContinue()
{
	// なし
}
//=============================================================================
// コンティニュー項目の初期化処理
//=============================================================================
HRESULT CContinue::Init(void)
{
	// ポーズの初期化処理
	CPause::Init();

	return S_OK;
}
//=============================================================================
// コンティニュー項目の終了処理
//=============================================================================
void CContinue::Uninit(void)
{
	// ポーズの終了処理
	CPause::Uninit();
}
//=============================================================================
// コンティニュー項目の更新処理
//=============================================================================
void CContinue::Update(void)
{
	// ポーズの更新処理
	CPause::Update();
}
//=============================================================================
// コンティニュー項目の描画処理
//=============================================================================
void CContinue::Draw(void)
{
	// ポーズの描画処理
	CPause::Draw();
}
//=============================================================================
// 選択時の処理
//=============================================================================
void CContinue::Execute(void)
{
	// 続ける
	CManager::SetEnablePause(false);
}


//=============================================================================
// リトライ項目のコンストラクタ
//=============================================================================
CRetry::CRetry()
{
	// 値のクリア

}
//=============================================================================
// リトライ項目のデストラクタ
//=============================================================================
CRetry::~CRetry()
{
	// なし
}
//=============================================================================
// リトライ項目の初期化処理
//=============================================================================
HRESULT CRetry::Init(void)
{
	// ポーズの初期化処理
	CPause::Init();

	return S_OK;
}
//=============================================================================
// リトライ項目の終了処理
//=============================================================================
void CRetry::Uninit(void)
{
	// ポーズの終了処理
	CPause::Uninit();
}
//=============================================================================
// リトライ項目の更新処理
//=============================================================================
void CRetry::Update(void)
{
	// ポーズの更新処理
	CPause::Update();
}
//=============================================================================
// リトライ項目の描画処理
//=============================================================================
void CRetry::Draw(void)
{
	// ポーズの描画処理
	CPause::Draw();
}
//=============================================================================
// 選択時の処理
//=============================================================================
void CRetry::Execute(void)
{
	if (CManager::GetFade()->GetFade() != CFade::FADE_NONE)
	{
		return;
	}

	// ゲーム画面に移行
	CManager::GetFade()->SetFade(CScene::MODE_GAME);
}


//=============================================================================
// 終了項目のコンストラクタ
//=============================================================================
CQuit::CQuit()
{
	// 値のクリア

}
//=============================================================================
// 終了項目のデストラクタ
//=============================================================================
CQuit::~CQuit()
{
	// なし
}
//=============================================================================
// 終了項目の初期化処理
//=============================================================================
HRESULT CQuit::Init(void)
{
	// ポーズの初期化処理
	CPause::Init();

	return S_OK;
}
//=============================================================================
// 終了項目の終了処理
//=============================================================================
void CQuit::Uninit(void)
{
	// ポーズの終了処理
	CPause::Uninit();
}
//=============================================================================
// 終了項目の更新処理
//=============================================================================
void CQuit::Update(void)
{
	// ポーズの更新処理
	CPause::Update();
}
//=============================================================================
// 終了項目の描画処理
//=============================================================================
void CQuit::Draw(void)
{
	// ポーズの描画処理
	CPause::Draw();
}
//=============================================================================
// 選択時の処理
//=============================================================================
void CQuit::Execute(void)
{
	if (CManager::GetFade()->GetFade() != CFade::FADE_NONE)
	{
		return;
	}

	// タイトル画面に移行
	CManager::GetFade()->SetFade(CScene::MODE_TITLE);
}
