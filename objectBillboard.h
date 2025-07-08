//=============================================================================
//
// ビルボード処理 [objectBillboard.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _OBJECTBILLBOARD_H_// このマクロ定義がされていなかったら
#define _OBJECTBILLBOARD_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"
#include "object.h"


//*****************************************************************************
// ビルボードクラス
//*****************************************************************************
class CObjectBillboard : public CObject
{
public:
	CObjectBillboard();
	~CObjectBillboard();

	//ビルボードの種類
	typedef enum
	{
		TYPE_ONE = 0,
		TYPE_LIGHT,
		TYPE_MAX
	}TYPE;

	static CObjectBillboard* Create(TYPE type, D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	D3DXVECTOR3 m_pos;
	TYPE m_nType;
	D3DXMATRIX m_mtxWorld;
	float m_fWidth;			// 幅
	float m_fHeight;		// 高さ
	int m_nIdxTexture;

	const char* BILLBOARD_TEXTURE[TYPE_MAX] =//ビルボードのテクスチャの設定
	{
		"data/TEXTURE/selectBG.png",	// 1
		"data/TEXTURE/light.png",		// 2

	};

};
#endif