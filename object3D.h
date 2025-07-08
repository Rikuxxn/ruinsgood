//=============================================================================
//
// 3Dオブジェクト処理 [object3D.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _OBJECT3D_H_// このマクロ定義がされていなかったら
#define _OBJECT3D_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"
#include "object.h"


//*****************************************************************************
// 3Dオブジェクトクラス
//*****************************************************************************
class CObject3D : public CObject
{
public:
	CObject3D(int nPriority = 6);
	~CObject3D();

	static CObject3D* Create(void);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);
	void SetPos(D3DXVECTOR3 pos);
	void SetAlpha(int nAlpha);
	void SetWidth(float fWidth);
	void SetHeight(float fHeight);
	void SetDepth(float fDepth);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	float m_fWidth;							// 幅
	float m_fHeight;						// 高さ
	float m_fDepth;							// 奥行き
	int m_nAlpha;							// アルファ値
};

#endif
