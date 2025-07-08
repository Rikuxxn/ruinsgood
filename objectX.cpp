//=============================================================================
//
// Xファイル処理 [objectX.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "objectX.h"
#include "renderer.h"
#include "manager.h"

using namespace std;

//=============================================================================
// コンストラクタ
//=============================================================================
CObjectX::CObjectX()
{
	// 値のクリア
	m_nIdxTexture = 0;
	m_pos = INIT_VEC3;						// 位置
	m_rot = INIT_VEC3;						// 向き
	m_move = INIT_VEC3;						// 移動量
	m_size = D3DXVECTOR3(1.0f, 1.0f, 1.0f);	// サイズ
	m_pMesh = NULL;							// メッシュへのポインタ
	m_pBuffMat = NULL;						// マテリアルへのポインタ
	m_dwNumMat = NULL;						// マテリアル数
	m_mtxWorld = {};						// ワールドマトリックス
	m_modelSize = INIT_VEC3;				// モデルの元サイズ（全体の幅・高さ・奥行き）
	for (int nCnt = 0; nCnt < MAX_PATH; nCnt++)
	{
		m_szPath[nCnt] = NULL;					// ファイルパス
	}
}
//=============================================================================
// デストラクタ
//=============================================================================
CObjectX::~CObjectX()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CObjectX* CObjectX::Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size)
{
	CObjectX* pObjectX;

	pObjectX = new CObjectX;

	pObjectX->m_pos = pos;
	pObjectX->m_rot = D3DXToRadian(rot);
	pObjectX->m_size = size;
	pObjectX->SetPath(pFilepath);	// パス保存

	// 初期化処理
	pObjectX->Init();

	return pObjectX;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CObjectX::Init(void)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// Xファイルの読み込み
	D3DXLoadMeshFromX(m_szPath,
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

	// メッシュが読み込まれているか確認
	if (m_pMesh == NULL)
	{
		MessageBox(NULL, "Xファイルの読み込みに失敗しました（メッシュがNULLです）", "エラー", MB_OK | MB_ICONERROR);
		return 0;
	}

	// 頂点数の取得
	nNumVtx = m_pMesh->GetNumVertices();

	// 頂点数が0なら、モデルが空とみなす
	if (nNumVtx == 0)
	{
		MessageBox(NULL, "Xファイルの読み込みに失敗しました（頂点数が0です）", "エラー", MB_OK | MB_ICONERROR);
		return 0;
	}
	else
	{// モデルあり

		// 頂点フォーマットの取得
		sizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

		// 頂点バッファのロック
		m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		// AABB計算用の最小・最大値初期化
		D3DXVECTOR3 vMin(FLT_MAX, FLT_MAX, FLT_MAX);
		D3DXVECTOR3 vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		// 最大値・最小値を求める
		for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
		{
			D3DXVECTOR3* p = (D3DXVECTOR3*)(pVtxBuff + sizeFVF * nCnt);

			vMin.x = min(vMin.x, p->x);
			vMin.y = min(vMin.y, p->y);
			vMin.z = min(vMin.z, p->z);

			vMax.x = max(vMax.x, p->x);
			vMax.y = max(vMax.y, p->y);
			vMax.z = max(vMax.z, p->z);
		}

		// 頂点バッファのアンロック
		m_pMesh->UnlockVertexBuffer();

		// サイズ = 最大 - 最小
		m_modelSize = vMax - vMin;

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
	}

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CObjectX::Uninit(void)
{
	// テクスチャインデックスの破棄
	if (m_nIdxTexture != NULL)
	{
		delete[] m_nIdxTexture;
		m_nIdxTexture = NULL;
	}

	//メッシュの破棄
	if (m_pMesh != NULL)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	//マテリアルの破棄
	if (m_pBuffMat != NULL)
	{
		m_pBuffMat->Release();
		m_pBuffMat = NULL;
	}

	// オブジェクトの破棄(自分自身)
	this->Release();
}
//=============================================================================
// 更新処理
//=============================================================================
void CObjectX::Update(void)
{
	//位置を更新
	m_pos.x += m_move.x;
	m_pos.y += m_move.y;
	m_pos.z += m_move.z;
}	
//=============================================================================
// 描画処理
//=============================================================================
void CObjectX::Draw(void)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	//pDevice->SetRenderState(D3DRS_LIGHTING, TRUE); // ライティング有効
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);// 法線正規化を有効にする

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxSize;

	D3DMATERIAL9 matDef;	// 現在のマテリアル保存用

	D3DXMATERIAL* pMat;		// マテリアルデータへのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// サイズを反映
	D3DXMatrixScaling(&mtxSize, m_size.x, m_size.y, m_size.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxSize);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルの取得
	pDevice->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	// 色の取得
	D3DXCOLOR col = GetCol();

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
#ifdef _DEBUG

		// 元のマテリアル色に補正を掛ける
		D3DMATERIAL9 mat = pMat[nCntMat].MatD3D;

		mat.Diffuse.r *= col.r;
		mat.Diffuse.g *= col.g;
		mat.Diffuse.b *= col.b;
		mat.Diffuse.a *= col.a;

		// マテリアルの設定
		pDevice->SetMaterial(&mat);

#else

		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

#endif

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

	//pDevice->SetRenderState(D3DRS_LIGHTING, FALSE); // ライティング無効
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);// 法線正規化を無効にする

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);

}
//=============================================================================
// ファイルパスの取得
//=============================================================================
const char* CObjectX::GetPath(void)
{
	return m_szPath;
}
//=============================================================================
// 位置の取得
//=============================================================================
D3DXVECTOR3 CObjectX::GetPos(void)
{
	return m_pos;
}
//=============================================================================
// 向きの取得
//=============================================================================
D3DXVECTOR3 CObjectX::GetRot(void)
{
	return m_rot;
}
//=============================================================================
// サイズの取得
//=============================================================================
D3DXVECTOR3 CObjectX::GetSize(void)
{
	return m_size;
}
//=============================================================================
// モデルの元サイズの取得
//=============================================================================
D3DXVECTOR3 CObjectX::GetModelSize(void)
{
	return m_modelSize;
}
//=============================================================================
// Xファイルパスの設定処理
//=============================================================================
void CObjectX::SetPath(const char* path)
{
	strcpy_s(m_szPath, MAX_PATH, path);
}
//=============================================================================
// サイズの設定処理
//=============================================================================
void CObjectX::SetSize(D3DXVECTOR3 size)
{
	m_size = size;
}
//=============================================================================
// 位置の設定処理
//=============================================================================
void CObjectX::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}
//=============================================================================
// 向きの設定処理
//=============================================================================
void CObjectX::SetRot(D3DXVECTOR3 rot)
{
	m_rot = rot;
}
//=============================================================================
// 色の取得
//=============================================================================
D3DXCOLOR CObjectX::GetCol(void) const
{
	return D3DXCOLOR();
}
//=============================================================================
// マテリアルの取得
//=============================================================================
D3DXCOLOR CObjectX::GetMaterialColor(void) const
{
	if (m_pBuffMat && m_dwNumMat > 0)
	{
		D3DXMATERIAL* pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();
		return pMat[0].MatD3D.Diffuse;  // 0番目のマテリアルの拡散光色を返す
	}

	return D3DXCOLOR(1, 1, 1, 1); // デフォルト白
}