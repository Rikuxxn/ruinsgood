//=============================================================================
//
// メッシュフィールド処理 [meshfield.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "meshfield.h"
#include "renderer.h"
#include "manager.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CMeshfield::CMeshfield()
{
	// 値のクリア
	m_nIdxTexture = 0;
	m_pVtxBuff = NULL;	// 頂点バッファへのポインタ
	m_pIdxBuff = NULL;	// インデックスバッファへのポインタ
	m_pos = INIT_VEC3;	// 位置
	m_rot = INIT_VEC3;	// 向き
	m_mtxWorld = {};	// ワールドマトリックス
	m_fWidth = 0.0f;	// 幅
	m_fHeight = 0.0f;	// 高さ
	m_nPrimitive = 0;	// プリミティブ数
	m_nVertex = 0;		// 頂点数
}
//=============================================================================
// デストラクタ
//=============================================================================
CMeshfield::~CMeshfield()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CMeshfield* CMeshfield::Create(D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	CMeshfield* pMeshfield;

	pMeshfield = new CMeshfield;

	pMeshfield->m_pos = pos;
	pMeshfield->m_fWidth = fWidth;
	pMeshfield->m_fHeight = fHeight;

	// 初期化処理
	pMeshfield->Init();

	return pMeshfield;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CMeshfield::Init(void)
{
	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register("data/TEXTURE/ground.png");

	int meshX = 20;// X方向のブロック数
	int meshZ = 20;// Z方向のブロック数

	m_nPrimitive = (((meshX * meshZ) * 2)) + (4 * (meshZ - 1));	// プリミティブ数
	m_nVertex = ((meshX + 1) * (meshZ + 1));					// 頂点数
	int maxIndex = (m_nPrimitive + 2);							// インデックス数

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nVertex,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// インデックスバッファの生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * maxIndex,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	VERTEX_3D* pVtx = NULL;// 頂点情報へのポインタ

	int nCnt = 0;
	float tex = 10.0f / meshX;
	float tex2 = 10.0f / meshZ;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt1 = 0; nCnt1 <= meshZ; nCnt1++)
	{
		for (int nCnt2 = 0; nCnt2 <= meshX; nCnt2++)
		{

			pVtx[nCnt].pos = D3DXVECTOR3(0.0f + ((m_fWidth / meshX) * nCnt2) - (m_fWidth * 0.5f),
				0.0f,
				m_fHeight - ((m_fHeight / meshZ) * nCnt1) - (m_fHeight * 0.5f));

			// 各頂点の法線の設定
			pVtx[nCnt].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 頂点カラーの設定
			pVtx[nCnt].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			pVtx[nCnt].tex = D3DXVECTOR2(tex * nCnt2, tex2 * nCnt1);

			nCnt++;
		}
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	WORD* pIdx;// インデックス情報へのポインタ

	// インデックスバッファをロックし、頂点情報へのポインタを取得
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	int nCntIdx3 = meshX + 1;
	int Num = 0;
	int nCntNum = 0;

	for (int nCntIdx = 0; nCntIdx < meshZ; nCntIdx++)
	{
		for (int nCntIdx2 = 0; nCntIdx2 <= meshX; nCntIdx2++, nCntIdx3++, Num++)
		{
			pIdx[nCntNum] = (WORD)nCntIdx3;

			pIdx[nCntNum + 1] = (WORD)Num;

			nCntNum += 2;
		}

		// 最後の行かどうか
		// NOTE: 最後の行に縮退ポリゴンが無い
		if (nCntIdx != meshZ - 1)
		{
			pIdx[nCntNum] = (WORD)Num - 1;

			pIdx[nCntNum + 1] = (WORD)nCntIdx3;

			nCntNum += 2;
		}
	}

	// インデックスバッファをアンロックする
	m_pIdxBuff->Unlock();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CMeshfield::Uninit(void)
{
	// インデックスバッファの解放
	if (m_pIdxBuff != NULL)
	{
		m_pIdxBuff->Release();
		m_pIdxBuff = NULL;
	}

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
void CMeshfield::Update(void)
{
	// なし
}
//=============================================================================
// 描画処理
//=============================================================================
void CMeshfield::Draw(void)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//// 向きを反映
	//D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotMeshfield.y, g_rotMeshfield.x, g_rotMeshfield.z);
	//D3DXMatrixMultiply(&g_mtxWorldMeshfield, &g_mtxWorldMeshfield, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nVertex, 0, m_nPrimitive);
}
//=============================================================================
// 位置の取得
//=============================================================================
D3DXVECTOR3 CMeshfield::GetPos(void)
{
	return m_pos;
}
