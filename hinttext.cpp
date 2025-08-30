//=============================================================================
//
// 壁画処理 [hinttext.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "hinttext.h"
#include "manager.h"
#include "game.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CHintText::CHintText()
{
	// 値のクリア
	m_nIdxTexture = 0;
	memset(m_szPath, 0, sizeof(m_szPath));
}
//=============================================================================
// デストラクタ
//=============================================================================
CHintText::~CHintText()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CHintText* CHintText::Create(const char* filepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot,
	float fWidth, float fHeight)
{
	// オブジェクトの生成
	CHintText* pHintText = new CHintText;

	pHintText->SetPath(filepath);
	pHintText->SetPos(pos);
	pHintText->SetRot(D3DXToRadian(rot));
	pHintText->SetWidth(fWidth);
	pHintText->SetHeight(fHeight);
	pHintText->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	// 初期化処理
	pHintText->Init();

	return pHintText;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CHintText::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャの取得
	m_nIdxTexture = CManager::GetTexture()->Register(m_szPath);

	// 3Dオブジェクトの初期化処理
	CObject3D::Init();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CHintText::Uninit(void)
{
	// 3Dオブジェクトの終了処理
	CObject3D::Uninit();
}
//=============================================================================
// 更新処理
//=============================================================================
void CHintText::Update(void)
{
	// 3Dオブジェクトの更新処理
	CObject3D::Update();
}
//=============================================================================
// 描画処理
//=============================================================================
void CHintText::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャの設定
	pDevice->SetTexture(0, CManager::GetTexture()->GetAddress(m_nIdxTexture));

	// 3Dオブジェクトの描画処理
	CObject3D::Draw();
}