//=============================================================================
//
// 影の処理 [shadow.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "shadow.h"
#include "texture.h"
#include "manager.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
int CShadow::m_nIdxTexture = 0;

//=============================================================================
// コンストラクタ
//=============================================================================
CShadow::CShadow()
{
	// 値のクリア
	m_pos	   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置
	m_mtxWorld = {};								// ワールドマトリックス
}
//=============================================================================
// デストラクタ
//=============================================================================
CShadow::~CShadow()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CShadow* CShadow::Create(D3DXVECTOR3 pos, int nAlpha, float fWidth, float fHeight, float fDepth)
{
	CShadow* pShadow;

	pShadow = new CShadow;

	// 初期化処理
	pShadow->Init();

	pShadow->m_pos = pos;
	pShadow->SetAlpha(nAlpha);
	pShadow->SetWidth(fWidth);
	pShadow->SetHeight(fHeight);
	pShadow->SetDepth(fDepth);

	return pShadow;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CShadow::Init(void)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register("data/TEXTURE/shadow000.jpg");

	// 3Dオブジェクトの初期化処理
	CObject3D::Init();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CShadow::Uninit(void)
{
	// 3Dオブジェクトの終了処理
	CObject3D::Uninit();
}
//=============================================================================
// 更新処理
//=============================================================================
void CShadow::Update(void)
{
	// 3Dオブジェクトの更新処理
	CObject3D::Update();

	// 位置の設定
	SetPos(m_pos);
}
//=============================================================================
// 描画処理
//=============================================================================
void CShadow::Draw(void)
{
	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// αブレンディングを減算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// 3Dオブジェクトの描画処理
	CObject3D::Draw();

	// αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

}
