//=============================================================================
//
// ステージ処理 [stage.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "stage.h"
#include "renderer.h"
#include "manager.h"
#include "stageselect.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CStage::CStage(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	m_pVtxBuff = NULL;
	m_pos = INIT_VEC3;
	m_fWidth = 0.0f;
	m_fHeight = 0.0f;
	m_nIdxTexture = 0;
	memset(m_szPath, 0, sizeof(m_szPath));
	m_isSelected = false;
	m_startPos = INIT_VEC3;	// スライドイン開始位置（画面右外）
	m_targetPos = INIT_VEC3;// スライドイン完了位置（目的位置）
	m_isSlidingIn = false;	// スライド中フラグ
	m_isSlidingOut = false;	// スライドアウトフラグ
	m_isSlideOutFinished = false;
	m_isSlideInFinished = false;
}
//=============================================================================
// デストラクタ
//=============================================================================
CStage::~CStage()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CStage* CStage::Create(STAGE type, D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	CStage* pStage = NULL;

	switch (type)
	{
	case STAGE_ID_1:
		pStage = new CStage1;
		pStage->SetPath("data/TEXTURE/stage_01.png");
		break;
	case STAGE_ID_2:
		pStage = new CStage2;
		pStage->SetPath("data/TEXTURE/stage_none.png");
		break;
	case STAGE_ID_3:
		pStage = new CStage3;
		pStage->SetPath("data/TEXTURE/stage_none.png");
		break;
	case STAGE_ID_NONE:
		pStage = new CBack;
		pStage->SetPath("data/TEXTURE/back.png");
		break;
	default:
		pStage = new CStage;
		break;
	}

	pStage->SetPos(pos);
	pStage->m_fWidth = fWidth;
	pStage->m_fHeight = fHeight;
	pStage->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	pStage->m_targetPos = D3DXVECTOR3(1100.0f, pos.y, pos.z);

	// 初期化処理
	pStage->Init();

	return pStage;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CStage::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	m_nIdxTexture = CManager::GetTexture()->Register(m_szPath);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// スライド開始位置
	m_startPos = m_pos;

	// スライド中フラグは最初はfalse
	m_isSlidingIn = false;

	VERTEX_2D* pVtx;// 頂点情報へのポインタ

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
void CStage::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	this->Release();
}
//=============================================================================
// 更新処理
//=============================================================================
void CStage::Update(void)
{
	// スライドイン処理
	SlideIn();

	// スライドアウト処理
	SlideOut();

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
void CStage::Draw(void)
{
	if (!CStageSelect::IsVisible())
	{
		return;
	}

	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//=============================================
	// ステージ項目
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
//=============================================================================
// 位置の取得処理
//=============================================================================
D3DXVECTOR3 CStage::GetPos(void)
{
	return m_pos;
}
//=============================================================================
// マウスカーソルの判定処理
//=============================================================================
bool CStage::IsMouseOver(void)
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
// スライドイン処理
//=============================================================================
void CStage::SlideIn(void)
{
	// スライド速度
	const float slideSpeed = 10.0f;

	if (m_isSlidingIn)
	{
		// 目的地へ向けて移動
		if (m_pos.x > m_targetPos.x)
		{
			m_pos.x -= slideSpeed;

			if (m_pos.x <= m_targetPos.x)
			{
				m_pos.x = m_targetPos.x;
				m_isSlidingIn = false; // スライド完了
				m_isSlideOutFinished = false;
				m_isSlideInFinished = true;
			}
		}
	}
}
//=============================================================================
// スライドアウト処理
//=============================================================================
void CStage::SlideOut(void)
{
	const float slideSpeed = 20.0f;

	if (m_isSlidingOut)
	{
		// 目的地へ向けて移動
		if (m_pos.x < m_startPos.x)
		{
			m_pos.x += slideSpeed;

			if (m_pos.x >= m_startPos.x)
			{
				m_pos.x = m_startPos.x;
				m_isSlidingOut = false; // スライド完了
				m_isSlideOutFinished = true;
				m_isSlideInFinished = false;
			}
		}
	}
}
//=============================================================================
// ステージIDを返す処理
//=============================================================================
CStage::STAGE CStage::GetStageId(void) const
{
	if (dynamic_cast<const CStage1*>(this)) return STAGE_ID_1;
	if (dynamic_cast<const CStage2*>(this)) return STAGE_ID_2;
	if (dynamic_cast<const CStage3*>(this)) return STAGE_ID_3;
	if (dynamic_cast<const CBack*>(this))   return STAGE_ID_NONE;

	return STAGE_MAX; // 想定外
}


//=============================================================================
// ステージ1のコンストラクタ
//=============================================================================
CStage1::CStage1()
{
	// 値のクリア

}
//=============================================================================
// ステージ1のデストラクタ
//=============================================================================
CStage1::~CStage1()
{
	// なし
}
//=============================================================================
// 選択時の処理
//=============================================================================
void CStage1::Execute(void)
{
	// ゲーム画面に移行
	CManager::GetFade()->SetFade(CScene::MODE_GAME);
}


//=============================================================================
// ステージ2のコンストラクタ
//=============================================================================
CStage2::CStage2()
{
	// 値のクリア

}
//=============================================================================
// ステージ2のデストラクタ
//=============================================================================
CStage2::~CStage2()
{
	// なし
}
//=============================================================================
// 選択時の処理
//=============================================================================
void CStage2::Execute(void)
{
	// ゲーム画面に移行
	CManager::GetFade()->SetFade(CScene::MODE_GAME);
}


//=============================================================================
// ステージ3のコンストラクタ
//=============================================================================
CStage3::CStage3()
{
	// 値のクリア

}
//=============================================================================
// ステージ3のデストラクタ
//=============================================================================
CStage3::~CStage3()
{
	// なし
}
//=============================================================================
// 選択時の処理
//=============================================================================
void CStage3::Execute(void)
{
	//// ゲーム画面に移行
	//CManager::GetFade()->SetFade(CScene::MODE_GAME);
}

//=============================================================================
// 戻る項目のコンストラクタ
//=============================================================================
CBack::CBack()
{
	// 値のクリア

}
//=============================================================================
// 戻る項目のデストラクタ
//=============================================================================
CBack::~CBack()
{
	// なし
}
//=============================================================================
// 選択時の処理
//=============================================================================
void CBack::Execute(void)
{




}
