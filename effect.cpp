//=============================================================================
//
// エフェクト処理 [effect.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "effect.h"
#include "renderer.h"
#include "manager.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CEffect::CEffect(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_move = INIT_VEC3;	// 位置
	m_fRadius = 0.0f;	// 半径
	m_nLife = 0;		// 寿命
	m_nIdxTexture = 0;	// テクスチャインデックス
}
//=============================================================================
// デストラクタ
//=============================================================================
CEffect::~CEffect()
{

	// なし

}
//=============================================================================
// 生成処理
//=============================================================================
CEffect* CEffect::Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DCOLOR col, float fRadius, int nLife)
{
	CEffect* pEffect;

	// エフェクトオブジェクトの生成
	pEffect = new CEffect;

	// 初期化処理
	pEffect->Init();

	pEffect->SetPos(pos);
	pEffect->SetMove(move);
	pEffect->SetCol(col);
	pEffect->SetRadius(fRadius);
	pEffect->SetLife(nLife);

	return pEffect;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CEffect::Init(void)
{
	// 2Dオブジェクトの初期化処理
	CObject2D::Init();

	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register("data/TEXTURE/effect000.jpg");

	// テクスチャUVの設定
	CObject2D::SetUV(1, 1);

	// サイズの設定
	CObject2D::SetSize(50.0f, 50.0f);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CEffect::Uninit(void)
{
	// 2Dオブジェクトの終了処理
	CObject2D::Uninit();
}
//=============================================================================
// 更新処理
//=============================================================================
void CEffect::Update(void)
{
	// 位置の取得
	D3DXVECTOR3 Pos = GetPos();

	// 位置を更新
	Pos.x += m_move.x;
	Pos.y += m_move.y;

	// 位置の設定
	CObject2D::SetPos(Pos);


	m_fRadius--;

	if (m_fRadius <= 0.0f)
	{
		m_fRadius = 0.0f;
	}

	// 半径に応じてサイズを変更
	CObject2D::SetSize(m_fRadius * 2.0f, m_fRadius * 2.0f); // 直径にする

	// 2Dオブジェクトの更新処理
	CObject2D::Update();

	m_nLife--;

	if (m_nLife <= 0)
	{
		// エフェクトの終了
		Uninit();
		return;
	}

}
//=============================================================================
// 描画処理
//=============================================================================
void CEffect::Draw(void)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// デバイスの取得
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	//αブレンディングを加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// 2Dオブジェクトの描画処理
	CObject2D::Draw();

	//αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
//=============================================================================
// 移動量の設定
//=============================================================================
void CEffect::SetMove(D3DXVECTOR3 move)
{
	m_move = move;
}
//=============================================================================
// 寿命の設定
//=============================================================================
void CEffect::SetLife(int nLife)
{
	m_nLife = nLife;
}
//=============================================================================
// 半径の設定
//=============================================================================
void CEffect::SetRadius(float fRadius)
{
	m_fRadius = fRadius;
}

