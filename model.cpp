//=============================================================================
//
// モデル処理 [model.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "model.h"
#include "texture.h"
#include "renderer.h"
#include "manager.h"
#include <cstdio>

//=============================================================================
// コンストラクタ
//=============================================================================
CModel::CModel()
{
	// 値のクリア
	m_nIdxTexture = NULL;
	m_pos = INIT_VEC3;								// 位置
	m_rot = INIT_VEC3;								// 向き
	m_move = INIT_VEC3;								// 移動量
	m_pMesh = NULL;									// メッシュへのポインタ
	m_pBuffMat = NULL;								// マテリアルへのポインタ
	m_dwNumMat = NULL;								// マテリアル数
	m_mtxWorld = {};								// ワールドマトリックス
	m_pParent = NULL;								// 親モデルへのポインタ
	for (int nCnt = 0; nCnt < MAX_PATH; nCnt++)
	{
		m_Path[nCnt] = NULL;						// ファイルパス格納用
	}
}
//=============================================================================
// デストラクタ
//=============================================================================
CModel::~CModel()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CModel* CModel::Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CModel* pModel;

	pModel = new CModel;

	pModel->m_pos = pos;
	pModel->m_rot = D3DXToRadian(rot);
	strcpy_s(pModel->m_Path, pFilepath); // パス保存

	// 初期化処理
	pModel->Init();

	return pModel;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CModel::Init(void)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// Xファイルの読み込み
	D3DXLoadMeshFromX(m_Path,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&m_pBuffMat,
		NULL,
		&m_dwNumMat,
		&m_pMesh);

	int nNumVtx;		// 頂点数
	DWORD sizeFVF;		// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;		// 頂点バッファへのポインタ

	// 頂点数の取得
	nNumVtx = m_pMesh->GetNumVertices();

	// 頂点フォーマットの取得
	sizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

	// 頂点バッファのロック
	m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);


	// 最大値とかはいったんなし


	// 頂点バッファのアンロック
	m_pMesh->UnlockVertexBuffer();

	D3DXMATERIAL* pMat;// マテリアルへのポインタ

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	m_nIdxTexture = new int[m_dwNumMat];

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		if (pMat[nCntMat].pTextureFilename != NULL)
		{// テクスチャファイルが存在する
			// テクスチャの登録
			m_nIdxTexture[nCntMat] = pTexture->RegisterX(pMat[nCntMat].pTextureFilename);
		}
		else
		{// テクスチャが存在しない
			m_nIdxTexture[nCntMat] = -1;
		}
	}

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CModel::Uninit(void)
{
	// テクスチャインデックスの破棄
	if (m_nIdxTexture != NULL)
	{
		delete[] m_nIdxTexture;
		m_nIdxTexture = NULL;
	}

	// メッシュの破棄
	if (m_pMesh != NULL)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	// マテリアルの破棄
	if (m_pBuffMat != NULL)
	{
		m_pBuffMat->Release();
		m_pBuffMat = NULL;
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void CModel::Update(void)
{
	// 位置を更新
	m_pos.x += m_move.x;
	m_pos.z += m_move.z;
	m_pos.y += m_move.y;
}
//=============================================================================
// 描画処理
//=============================================================================
void CModel::Draw(void)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	D3DMATERIAL9 matDef;	// 現在のマテリアル保存用

	D3DXMATERIAL* pMat;		// マテリアルデータへのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y + m_OffsetRot.y, m_rot.x + m_OffsetRot.x, m_rot.z + m_OffsetRot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x + m_OffsetPos.x, m_pos.y + m_OffsetPos.y, m_pos.z + m_OffsetPos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	D3DXMATRIX mtxParent;

	if (m_pParent != NULL)
	{// 親が存在する
		// 親モデルのマトリックスを取得
		mtxParent = m_pParent->GetMtxWorld();
	}
	else
	{// 親が存在しない
		// 現在(最新)のマトリックスを取得する
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
	}

	// 親のマトリックスと掛け合わせる
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルの取得
	pDevice->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		if (m_nIdxTexture[nCntMat] == -1)
		{
			// テクスチャの設定
			pDevice->SetTexture(0, NULL);
		}
		else
		{
			// テクスチャの設定
			pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture[nCntMat]));
		}

		// モデル(パーツ)の描画
		m_pMesh->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}
//=============================================================================
// 親の設定処理
//=============================================================================
void CModel::SetParent(CModel* pModel)
{
	m_pParent = pModel;
}
//=============================================================================
// ワールドマトリックスの取得
//=============================================================================
D3DXMATRIX CModel::GetMtxWorld(void)
{
	return m_mtxWorld;
}
//=============================================================================
// 位置の取得
//=============================================================================
D3DXVECTOR3 CModel::GetPos(void)
{
	return m_pos;
}
//=============================================================================
// 向きの取得
//=============================================================================
D3DXVECTOR3 CModel::GetRot(void)
{
	return m_rot;
}
//=============================================================================
// 位置の設定
//=============================================================================
void CModel::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}
//=============================================================================
// 向きの設定
//=============================================================================
void CModel::SetRot(D3DXVECTOR3 rot)
{
	m_rot = D3DXToRadian(rot);
}