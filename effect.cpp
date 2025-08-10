//=============================================================================
//
// エフェクト処理 [effect.cpp]
// Author : RIKU TANEKAWA
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
CEffect::CEffect(int nPriority) : CObjectBillboard(nPriority)
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
CEffect* CEffect::Create(const char* path,D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fRadius, int nLife)
{
	CEffect* pEffect;

	// エフェクトオブジェクトの生成
	pEffect = new CEffect;

	pEffect->SetPath(path);

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
	// ビルボードオブジェクトの初期化処理
	CObjectBillboard::Init();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CEffect::Uninit(void)
{
	// ビルボードオブジェクトの終了処理
	CObjectBillboard::Uninit();
}
//=============================================================================
// 更新処理
//=============================================================================
void CEffect::Update(void)
{
	// ビルボードオブジェクトの更新処理
	CObjectBillboard::Update();

	// 位置の取得
	D3DXVECTOR3 Pos = GetPos();

	// 位置を更新
	Pos.x += m_move.x;
	Pos.y += m_move.y;
	Pos.z += m_move.z;

	// 位置の設定
	SetPos(Pos);
	SetSize(m_fRadius);

	m_fRadius--;

	if (m_fRadius <= 0.0f)
	{
		m_fRadius = 0.0f;
	}

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

	// αブレンディングを加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// αテストを有効
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);//デフォルトはfalse
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);//0より大きかったら描画

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// ビルボードオブジェクトの描画処理
	CObjectBillboard::Draw();

	// αテストを無効に戻す
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);//デフォルトはfalse

	// αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
