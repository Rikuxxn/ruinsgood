//=============================================================================
//
// 3Dオブジェクト処理 [object3D.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "object3D.h"
#include "texture.h"
#include "manager.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CObject3D::CObject3D(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	m_pVtxBuff = NULL;		// 頂点バッファへのポインタ
	m_pos	   = INIT_VEC3;	// 位置
	m_rot	   = INIT_VEC3;	// 向き
	m_mtxWorld = {};		// ワールドマトリックス
	m_fWidth   = 0.0f;		// 幅
	m_fHeight  = 0.0f;		// 高さ
	m_fDepth   = 0.0f;		// 奥行き
	m_nAlpha   = 0;			// アルファ値
}
//=============================================================================
// デストラクタ
//=============================================================================
CObject3D::~CObject3D()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CObject3D* CObject3D::Create(void)
{
	CObject3D* pObject3D;

	// オブジェクトの生成
	pObject3D = new CObject3D;

	// 初期化処理
	pObject3D->Init();

	return pObject3D;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CObject3D::Init(void)
{
	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_3D* pVtx = NULL;//頂点情報へのポインタ

	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//各頂点の法線の設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	//頂点カラーの設定
	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	//テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CObject3D::Uninit(void)
{
	//頂点バッファの破棄
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
void CObject3D::Update(void)
{
	VERTEX_3D* pVtx;//頂点情報へのポインタ

	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-m_fWidth, +m_fHeight, +m_fDepth);
	pVtx[1].pos = D3DXVECTOR3(+m_fWidth, +m_fHeight, +m_fDepth);
	pVtx[2].pos = D3DXVECTOR3(-m_fWidth, +m_fHeight, -m_fDepth);
	pVtx[3].pos = D3DXVECTOR3(+m_fWidth, +m_fHeight, -m_fDepth);

	//頂点カラーの設定
	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);
	pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, m_nAlpha);

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}
//=============================================================================
// 描画処理
//=============================================================================
void CObject3D::Draw(void)
{
	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	//計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
//=============================================================================
// 位置の取得
//=============================================================================
D3DXVECTOR3 CObject3D::GetPos(void)
{
	return m_pos;
}
//=============================================================================
// 位置の設定処理
//=============================================================================
void CObject3D::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}
//=============================================================================
// アルファ値の設定処理
//=============================================================================
void CObject3D::SetAlpha(int nAlpha)
{
	m_nAlpha = nAlpha;
}
//=============================================================================
// 幅の設定処理
//=============================================================================
void CObject3D::SetWidth(float fWidth)
{
	m_fWidth = fWidth;
}
//=============================================================================
// 高さの設定処理
//=============================================================================
void CObject3D::SetHeight(float fHeight)
{
	m_fHeight = fHeight;
}
//=============================================================================
// 奥行の設定処理
//=============================================================================
void CObject3D::SetDepth(float fDepth)
{
	m_fDepth = fDepth;
}
