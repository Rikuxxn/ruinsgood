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
	m_pos = INIT_VEC3;		// 位置
	m_col = INIT_XCOL;		// 色
	m_fWidth = 0.0f;
	m_fHeight = 0.0f;
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
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

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
	pVtx[0].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return S_OK;

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CTitle::Uninit(void)
{
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
	CInputMouse* pInputMouse = CManager::GetInputMouse();
	CFade* pFade = CManager::GetFade();

	if (pInputMouse->GetTrigger(0))
	{
		// ゲーム画面に移行
		pFade->SetFade(MODE_GAME);
	}
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

	// 黒いポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// テクスチャの設定
	pDevice->SetTexture(0, NULL);
}
