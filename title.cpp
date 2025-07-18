//=============================================================================
//
// タイトル処理 [title.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "title.h"
#include "input.h"
#include "manager.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CTitle::CTitle() : CScene(CScene::MODE_TITLE)
{
	// 値のクリア
	m_pVtxBuff = NULL;		// 頂点バッファへのポインタ
	m_nIdxTextureTitle = 0;
	m_nIdxTexturePress = 0;
	m_alphaPress = 0.0f;          // 現在のα値（0.0f ～ 1.0f）
	m_isAlphaDown = false;         // 点滅用フラグ（上げる/下げる）
	m_isEnterPressed = false;      // エンターキー押された

	for (int nCnt = 0; nCnt < TYPE_MAX; nCnt++)
	{
		m_vertexRanges[nCnt] = { -1, -1 }; // 未使用値で初期化
	}
}
//=============================================================================
// デストラクタ
//=============================================================================
CTitle::~CTitle()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CTitle::Init(void)
{
	// ブロックマネージャーの生成
	m_pBlockManager = new CBlockManager;

	// ブロックマネージャーの初期化
	m_pBlockManager->Init();

	// JSONの読み込み
	m_pBlockManager->LoadFromJson("data/block_title.json");

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャの取得
	m_nIdxTextureTitle = CManager::GetTexture()->Register("data/TEXTURE/press.png");
	m_nIdxTexturePress = CManager::GetTexture()->Register("data/TEXTURE/press.png");

	m_vertexRanges[TYPE_FIRST] = { 0, 3 }; // タイトル
	m_vertexRanges[TYPE_SECOND] = { 4, 7 }; // PRESS

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * 2,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	ImageInfo images[2] =
	{
		   { D3DXVECTOR3(900.0f, 400.0f, 0.0f), 520.0f, 200.0f }, // タイトルロゴ
		   { D3DXVECTOR3(900.0f, 720.0f, 0.0f), 300.0f, 50.0f }  // PRESS
	};

	for (int nCnt = 0; nCnt < 2; nCnt++)
	{
		pVtx[0].pos = D3DXVECTOR3(images[nCnt].pos.x - images[nCnt].width, images[nCnt].pos.y - images[nCnt].height, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(images[nCnt].pos.x + images[nCnt].width, images[nCnt].pos.y - images[nCnt].height, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(images[nCnt].pos.x - images[nCnt].width, images[nCnt].pos.y + images[nCnt].height, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(images[nCnt].pos.x + images[nCnt].width, images[nCnt].pos.y + images[nCnt].height, 0.0f);

		// rhwの設定
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		//頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CTitle::Uninit(void)
{
	// ブロックマネージャーの破棄
	if (m_pBlockManager != NULL)
	{
		m_pBlockManager->Uninit();

		delete m_pBlockManager;
		m_pBlockManager = NULL;
	}

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
void CTitle::Update(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();
	CInputMouse* pInputMouse = CManager::GetInputMouse();
	CFade* pFade = CManager::GetFade();

	if (pInputKeyboard->GetTrigger(DIK_RETURN))
	{
		m_isEnterPressed = true;

		// ゲーム画面に移行
		pFade->SetFade(MODE_GAME);
	}

	if (!m_isEnterPressed)
	{
		// 点滅ロジック
		float speed = 0.01f;

		if (m_isAlphaDown)
		{
			m_alphaPress -= speed;

			if (m_alphaPress < 0.1f) // 最小値
			{
				m_alphaPress = 0.1f;
				m_isAlphaDown = false;
			}
		}
		else
		{
			m_alphaPress += speed;

			if (m_alphaPress > 1.0f) // 最大値
			{
				m_alphaPress = 1.0f;
				m_isAlphaDown = true;
			}
		}
	}
	else
	{
		// フェードアウト
		float fadeSpeed = 0.03f;

		m_alphaPress -= fadeSpeed;

		if (m_alphaPress < 0.0f)
		{
			m_alphaPress = 0.0f;
		}
	}

	// アルファ値をタイトルのPRESS用頂点に適用
	int start = m_vertexRanges[TYPE_SECOND].start;
	int end = m_vertexRanges[TYPE_SECOND].end;

	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = start; nCnt <= end; nCnt++)
	{
		pVtx[nCnt].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_alphaPress);
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	// ブロックマネージャーの更新処理
	m_pBlockManager->Update();
}
//=============================================================================
// 描画処理
//=============================================================================
void CTitle::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャインデックス配列
	int textures[2] = { m_nIdxTextureTitle, m_nIdxTexturePress };

	// 各テクスチャごとに描画
	for (int nCnt = 0; nCnt < TYPE_MAX; nCnt++)
	{
		// テクスチャの設定
		pDevice->SetTexture(0, CManager::GetTexture()->GetAddress(textures[nCnt]));

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_vertexRanges[nCnt].start, 2);
	}
}