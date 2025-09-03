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
CHintText* CHintText::Create(TYPE type, const char* filepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot,
	float fWidth, float fHeight)
{
	// オブジェクトの生成
	CHintText* pHintText = nullptr;

	switch (type)
	{
	case TYPE_TEXT:
		pHintText = new CText;
		pHintText->SetCol(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
		break;
	case TYPE_NORMAL:
		pHintText = new CHintText;
		pHintText->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		break;
	default:
		break;
	}

	pHintText->SetPath(filepath);
	pHintText->SetPos(pos);
	pHintText->SetRot(D3DXToRadian(rot));
	pHintText->SetWidth(fWidth);
	pHintText->SetHeight(fHeight);

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


//=============================================================================
// ヒント壁画(文字)のコンストラクタ
//=============================================================================
CText::CText()
{
	// 値のクリア
	m_state = STATE_NORMAL;
	m_fGreen = 0.0f;
	m_fBlue = 0.0f;
}
//=============================================================================
// ヒント壁画(文字)のデストラクタ
//=============================================================================
CText::~CText()
{
	// なし
}
//=============================================================================
// ヒント壁画(文字)の初期化処理
//=============================================================================
HRESULT CText::Init(void)
{
	// ヒント壁画の初期化処理
	CHintText::Init();

	return S_OK;
}
//=============================================================================
// ヒント壁画(文字)の更新処理
//=============================================================================
void CText::Update(void)
{
	// ヒント壁画の更新処理
	CHintText::Update();

	if (!CGame::GetPlayer())
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 200.0f; // 反応距離

	switch (m_state)
	{
	case STATE_COLORINC:
		m_fGreen += 0.015f;
		m_fBlue += 0.015f;

		if (m_fGreen > 0.5f)
		{
			m_fGreen = 0.5f;
		}

		if (m_fBlue > 1.0f)
		{
			m_fBlue = 1.0f;
		}

		if (m_fGreen == 0.5f && m_fBlue == 1.0f)
		{
			m_state = STATE_NORMAL;
		}

		SetCol(D3DXCOLOR(0.0f, m_fGreen, m_fBlue, 1.0f));

		break;
	case STATE_NORMAL:
		if (distance < kTriggerDistance)
		{// 徐々に色を変える状態
			m_state = STATE_COLORINC;
		}
		else
		{// 徐々に通常(黒)に戻す状態
			m_state = STATE_COLORDEC;
		}
		break;
	case STATE_COLORDEC:
		m_fGreen -= 0.01f;
		m_fBlue -= 0.01f;

		if (m_fGreen < 0.0f)
		{
			m_fGreen = 0.0f;
		}

		if (m_fBlue < 0.0f)
		{
			m_fBlue = 0.0f;
		}

		if (m_fGreen == 0.0f && m_fBlue == 0.0f)
		{
			m_state = STATE_NORMAL;
		}

		SetCol(D3DXCOLOR(0.0f, m_fGreen, m_fBlue, 1.0f));
		break;
	}
}
//=============================================================================
// ヒント壁画(文字)の描画処理
//=============================================================================
void CText::Draw(void)
{
	// ヒント壁画の描画処理
	CHintText::Draw();
}
