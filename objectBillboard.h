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
	CObjectBillboard(int nPriority = 5);
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
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXCOLOR GetCol(void) { return m_col; }
	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetCol(D3DXCOLOR col) { m_col = col; }
	void SetSize(float fRadius) { m_fSize = fRadius; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	D3DXVECTOR3 m_pos;
	D3DXCOLOR m_col;
	TYPE m_nType;
	D3DXMATRIX m_mtxWorld;
	float m_fSize;			// サイズ
	int m_nIdxTexture;

};
#endif