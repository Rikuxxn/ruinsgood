//=============================================================================
//
// 2Dオブジェクト処理 [object2D.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "object2D.h"
#include "manager.h"
#include "renderer.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CObject2D::CObject2D(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	m_pVtxBuff	   = NULL;
	m_pos		   = INIT_VEC3;
	m_rot		   = INIT_VEC3;
	m_move		   = INIT_VEC3;
	m_col		   = D3DCOLOR_ARGB(255, 0, 0, 0);
	m_fLength	   = 0.0f;
	m_fAngle	   = 0.0f;
	m_fWidth	   = 0.0f;
	m_fHeight	   = 0.0f;
	m_nCounterAnim = 0;
	m_nPatternAnim = 0;
	m_fUSpeed	   = 0.0f;
	m_fVSpeed	   = 0.0f;
	m_aPosTexU     = 0.0f;
	m_fTexU		   = 0.0f;
	m_fTexV		   = 0.0f;
}
//=============================================================================
// デストラクタ
//=============================================================================
CObject2D::~CObject2D()
{
	// 今はなし
}
//=============================================================================
// 生成処理
//=============================================================================
CObject2D* CObject2D::Create(void)
{
	CObject2D* pObject2D;

	// オブジェクトの生成
	pObject2D = new CObject2D;

	// 初期化処理
	pObject2D->Init();

	return pObject2D;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CObject2D::Init(void)
{
	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

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

	//float UV1 = 1.0f / nTexUV1;
	//float UV2 = 1.0f / nTexUV2;

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
void CObject2D::Uninit(void)
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
void CObject2D::Update()
{
	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 対角線の長さを算出する
	m_fLength = sqrtf(m_fWidth * m_fWidth + m_fHeight * m_fHeight) / 2.0f;

	// 対角線の角度を算出する
	m_fAngle = atan2f(m_fWidth, m_fHeight);

	// 頂点座標の設定
	pVtx[0].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI + m_fAngle)) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI + m_fAngle)) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + sinf(m_rot.z + (0.0f - m_fAngle)) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_rot.z + (0.0f - m_fAngle)) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + sinf(m_rot.z + (0.0f + m_fAngle)) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_rot.z + (0.0f + m_fAngle)) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}
//=============================================================================
// 描画処理
//=============================================================================
void CObject2D::Draw(void)
{
	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// テクスチャの設定
	pDevice->SetTexture(0, NULL);
}
//=============================================================================
// 位置の設定
//=============================================================================
void CObject2D::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}
//=============================================================================
// 向きの設定
//=============================================================================
void CObject2D::SetRot(D3DXVECTOR3 rot)
{
	m_rot = rot;
}
//=============================================================================
// 色の設定
//=============================================================================
void CObject2D::SetCol(D3DXCOLOR color)
{
	m_col = color;
}
//=============================================================================
// スクロールスピードの設定
//=============================================================================
void CObject2D::SetScrollSpeed(float fUSpeed)
{
	m_fUSpeed = fUSpeed;
}
//=============================================================================
// テクスチャUVの設定
//=============================================================================
void CObject2D::SetUV(int nTexU, int nTexV)
{
	m_fTexU = 1.0f / nTexU;
	m_fTexV = 1.0f / nTexV;

	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(m_fTexU, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, m_fTexV);
	pVtx[3].tex = D3DXVECTOR2(m_fTexU, m_fTexV);

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}
//=============================================================================
// 位置の取得
//=============================================================================
D3DXVECTOR3 CObject2D::GetPos(void)
{
	return m_pos;
}
//=============================================================================
// 移動量の取得
//=============================================================================
D3DXVECTOR3 CObject2D::GetMove(void)
{
	return m_move;
}
//=============================================================================
// 向きの取得
//=============================================================================
D3DXVECTOR3 CObject2D::GetRot(void)
{
	return m_rot;
}
//=============================================================================
// 色の取得
//=============================================================================
D3DCOLOR CObject2D::GetCol(void)
{
	return m_col;
}
//=============================================================================
// 爆破用テクスチャアニメーション処理
//=============================================================================
void CObject2D::TextureAnimExp(int nTexPosX, int nTexPosY,int nAnimSpeed)
{
	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	float UV1 = 1.0f / nTexPosX;
	float UV2 = 1.0f / nTexPosY;

	m_nCounterAnim++;// カウンターを加算

	if (m_nCounterAnim > nAnimSpeed)
	{
		m_nCounterAnim = 0;
		m_nPatternAnim++;

		// 8番目のテクスチャ超えたら
		if (m_nPatternAnim >= nTexPosX)
		{
			m_nPatternAnim = 0;
			Uninit();
			return;
		}

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(m_nPatternAnim * UV1,0.0f);
		pVtx[1].tex = D3DXVECTOR2(m_nPatternAnim * UV1 + UV1,0.0f);
		pVtx[2].tex = D3DXVECTOR2(m_nPatternAnim * UV1, UV2);
		pVtx[3].tex = D3DXVECTOR2(m_nPatternAnim * UV1 + UV1, UV2);

		// 頂点バッファをアンロックする
		m_pVtxBuff->Unlock();
	}
}
//=============================================================================
// 通常テクスチャアニメーション処理
//=============================================================================
void CObject2D::TextureAnim(int nTexPosX, int nTexPosY, int nAnimSpeed)
{
	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	float UV1 = 1.0f / nTexPosX;
	float UV2 = 1.0f / nTexPosY;

	m_nCounterAnim++;// カウンターを加算

	if ((m_nCounterAnim % nAnimSpeed) == 0)
	{
		m_nCounterAnim = 0;

		// nTexPosX枚目の次は1枚目に戻る
		m_nPatternAnim = (m_nPatternAnim + 1) % nTexPosX;

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(m_nPatternAnim * UV1, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(m_nPatternAnim * UV1 + UV1, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(m_nPatternAnim * UV1, UV2);
		pVtx[3].tex = D3DXVECTOR2(m_nPatternAnim * UV1 + UV1, UV2);

		// 頂点バッファをアンロックする
		m_pVtxBuff->Unlock();
	}
}
//=============================================================================
// テクスチャスクロール処理
//=============================================================================
void CObject2D::ScrollTexture(void)
{
	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	 m_aPosTexU += m_fUSpeed;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//テクスチャ座標の更新
	pVtx[0].tex = D3DXVECTOR2(m_aPosTexU, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(m_aPosTexU + 1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(m_aPosTexU, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(m_aPosTexU + 1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}
//=============================================================================
// サイズ設定
//=============================================================================
void CObject2D::SetSize(float fWidth, float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;
}
