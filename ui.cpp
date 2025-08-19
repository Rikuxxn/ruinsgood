//=============================================================================
//
// UI処理 [ui.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ui.h"
#include "manager.h"
#include "result.h"


//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
std::unordered_map<CUi::TYPE, UiCreateFunc> CUi::m_UiFactoryMap = {};

//=============================================================================
// コンストラクタ
//=============================================================================
CUi::CUi(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	m_pVtxBuff = NULL;		// 頂点バッファへのポインタ
	m_pos = INIT_VEC3;
	m_col = INIT_XCOL;
	m_fWidth = 0.0f;			// 幅
	m_fHeight = 0.0f;			// 高さ
	m_nIdxTexture = 0;
	memset(m_szPath, 0, sizeof(m_szPath));
	m_type = TYPE_PAUSE;
	m_uvLeft = 0.0f;
	m_uvTop = 0.0f;
	m_uvWidth = 0.0f;
	m_uvHeight = 0.0f;
	m_isUVDirty = false;
}
//=============================================================================
// デストラクタ
//=============================================================================
CUi::~CUi()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CUi* CUi::Create(TYPE type, const char* path,D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	if (m_UiFactoryMap.empty())
	{
		// ファクトリー
		InitFactory();
	}

	CUi* pUi = nullptr;

	auto it = m_UiFactoryMap.find(type);
	if (it != m_UiFactoryMap.end())
	{
		pUi = it->second();
	}
	else
	{
		pUi = new CUi(); // デフォルト基底クラス
	}

	if (!pUi)
	{
		return NULL;
	}

	pUi->SetPath(path);
	pUi->SetPos(pos);
	pUi->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	pUi->SetSize(fWidth,fHeight);

	// 初期化処理
	pUi->Init();

	return pUi;
}
//=============================================================================
// ファクトリー
//=============================================================================
void CUi::InitFactory(void)
{
	// リストを空にする
	m_UiFactoryMap.clear();

	m_UiFactoryMap[CUi::TYPE_PAUSE]			= []() -> CUi* { return new CPauseUi(); };
	m_UiFactoryMap[CUi::TYPE_GET]			= []() -> CUi* { return new CGetUi(); };
	m_UiFactoryMap[CUi::TYPE_RESULT_UI]		= []() -> CUi* { return new CResultUi(); };
	m_UiFactoryMap[CUi::TYPE_RESULT_RANK]	= []() -> CUi* { return new CResultRankUi(); };
	m_UiFactoryMap[CUi::TYPE_STAGE_NAME]	= []() -> CUi* { return new CStageUi(); };

}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CUi::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャの取得
	m_nIdxTexture = CManager::GetTexture()->Register(m_szPath);

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_2D* pVtx = NULL;// 頂点情報へのポインタ

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
void CUi::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// オブジェクトの破棄(自分自身)
	this->Release();
}
//=============================================================================
// 更新処理
//=============================================================================
void CUi::Update(void)
{
	VERTEX_2D* pVtx;// 頂点情報へのポインタ

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

	if (m_isUVDirty)
	{
		pVtx[0].tex = D3DXVECTOR2(m_uvLeft, m_uvTop);
		pVtx[1].tex = D3DXVECTOR2(m_uvLeft + m_uvWidth, m_uvTop);
		pVtx[2].tex = D3DXVECTOR2(m_uvLeft, m_uvTop + m_uvHeight);
		pVtx[3].tex = D3DXVECTOR2(m_uvLeft + m_uvWidth, m_uvTop + m_uvHeight);

		m_isUVDirty = false;
	}

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}
//=============================================================================
// 描画処理
//=============================================================================
void CUi::Draw(void)
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
//=============================================================================
// UVの設定処理
//=============================================================================
void CUi::SetUV(float u, float v, float w, float h)
{
	m_uvLeft = u;
	m_uvTop = v;
	m_uvWidth = w;
	m_uvHeight = h;
	m_isUVDirty = true;
}


//=============================================================================
// ポーズUIのコンストラクタ
//=============================================================================
CPauseUi::CPauseUi()
{
	// 値のクリア
}
//=============================================================================
// ポーズUIのデストラクタ
//=============================================================================
CPauseUi::~CPauseUi()
{
	// なし
}
//=============================================================================
// ポーズUIの初期化処理
//=============================================================================
HRESULT CPauseUi::Init(void)
{
	// UIの初期化処理
	CUi::Init();

	return S_OK;
}
//=============================================================================
// ポーズUIの更新処理
//=============================================================================
void CPauseUi::Update(void)
{
	// UIの更新処理
	CUi::Update();
}


//=============================================================================
// 秘宝取得UIのコンストラクタ
//=============================================================================
CGetUi::CGetUi()
{
	// 値のクリア
	m_fTimer = 0.0f;// 経過時間(秒)
	m_fAlpha = 1.0f;// アルファ値
	m_bFading = false;// フェード開始フラグ
}
//=============================================================================
// 秘宝取得UIのデストラクタ
//=============================================================================
CGetUi::~CGetUi()
{
	// なし
}
//=============================================================================
// 秘宝取得UIの初期化処理
//=============================================================================
HRESULT CGetUi::Init(void)
{
	// UIの初期化処理
	CUi::Init();

	return S_OK;
}
//=============================================================================
// 秘宝取得UIの更新処理
//=============================================================================
void CGetUi::Update(void)
{
	// UIの更新処理
	CUi::Update();

	// 時間経過
	m_fTimer++; // 毎フレームの経過時間を加算

	// 3秒経過でフェード開始
	if (m_fTimer >= 180.0f)
	{
		m_bFading = true;
	}

	// フェード処理（毎フレーム少しずつ減らす）
	if (m_bFading)
	{
		m_fAlpha -= 0.008f; // 1秒でアルファ0になる速さ

		if (m_fAlpha < 0.0f)
		{
			m_fAlpha = 0.0f;
		}
	}

	SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha));
}


//=============================================================================
// リザルトUI(秘宝)のコンストラクタ
//=============================================================================
CResultUi::CResultUi()
{
	// 値のクリア
}
//=============================================================================
// リザルトUI(秘宝)のデストラクタ
//=============================================================================
CResultUi::~CResultUi()
{
	// なし
}
//=============================================================================
// リザルトUI(秘宝)の初期化処理
//=============================================================================
HRESULT CResultUi::Init(void)
{
	// UIの初期化処理
	CUi::Init();

	return S_OK;
}
//=============================================================================
// リザルトUI(秘宝)の更新処理
//=============================================================================
void CResultUi::Update(void)
{
	// UIの更新処理
	CUi::Update();
}


//=============================================================================
// リザルトUI(ランク)のコンストラクタ
//=============================================================================
CResultRankUi::CResultRankUi()
{
	// 値のクリア
}
//=============================================================================
// リザルトUI(ランク)のデストラクタ
//=============================================================================
CResultRankUi::~CResultRankUi()
{
	// なし
}
//=============================================================================
// リザルトUI(ランク)の初期化処理
//=============================================================================
HRESULT CResultRankUi::Init(void)
{
	// UIの初期化処理
	CUi::Init();

	int min = CResult::GetClearMinutes();
	int sec = CResult::GetClearSeconds();
	float totalSec = min * 60.0f + sec;

	float uvLeft = 0.0f;

	if (totalSec < 300.0f)
	{// 5分未満
		uvLeft = 0.0f;   // S
	}
	else if (totalSec < 360.0f)
	{// 6分未満
		uvLeft = 0.25f;  // A
	}
	else if (totalSec < 420.0f)
	{// 7分未満
		uvLeft = 0.5f;   // B
	}
	else
	{// それ以降
		uvLeft = 0.75f;  // C
	}

	CUi::SetUV(uvLeft, 0.0f, 0.25f, 1.0f);  // 横4分割、縦1

	return S_OK;
}
//=============================================================================
// リザルトUI(ランク)の更新処理
//=============================================================================
void CResultRankUi::Update(void)
{
	// UIの更新処理
	CUi::Update();
}


//=============================================================================
// ステージ名表示のコンストラクタ
//=============================================================================
CStageUi::CStageUi()
{
	// 値のクリア
	m_fTimer = 0.0f;// 経過時間(秒)
	m_fAlpha = 0.0f;// アルファ値
	m_state = STATE_FADEIN; // 状態
}
//=============================================================================
// ステージ名表示のデストラクタ
//=============================================================================
CStageUi::~CStageUi()
{
	// なし
}
//=============================================================================
// ステージ名表示の初期化処理
//=============================================================================
HRESULT CStageUi::Init(void)
{
	// UIの初期化処理
	CUi::Init();

	return S_OK;
}
//=============================================================================
// ステージ名表示の更新処理
//=============================================================================
void CStageUi::Update(void)
{
	// UIの更新処理
	CUi::Update();

	// 時間経過
	m_fTimer++; // 毎フレームの経過時間を加算

	switch (m_state)
	{
	case STATE_FADEIN:
		m_fAlpha += 0.005f; // 1秒でアルファ0になる速さ

		if (m_fAlpha >= 1.0f)
		{
			m_fAlpha = 1.0f;
			m_fTimer = 0.0f;
			m_state = STATE_SHOW;
		}
		break;
	case STATE_SHOW:
		// 一定時間表示
		if (m_fTimer >= 180.0f)
		{
			m_state = STATE_FADEOUT;
		}
		break;
	case STATE_FADEOUT:
		m_fAlpha -= 0.005f; // 1秒でアルファ0になる速さ

		if (m_fAlpha <= 0.0f)
		{
			m_fAlpha = 0.0f;
			m_state = STATE_END;
		}
	case STATE_END:
		// 何もしない
		break;
	}

	SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha));
}