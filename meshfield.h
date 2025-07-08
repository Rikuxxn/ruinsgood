//=============================================================================
//
// メッシュフィールド処理 [meshfield.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _MESHFIELD_H_// このマクロ定義がされていなかったら
#define _MESHFIELD_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"
#include "object.h"


//*****************************************************************************
// メッシュフィールドクラス
//*****************************************************************************
class CMeshfield : public CObject
{
public:
	CMeshfield();
	~CMeshfield();

	static CMeshfield* Create(D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);

private:
	int m_nIdxTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;		// インデックスバッファへのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	float m_fWidth;							// 幅
	float m_fHeight;						// 高さ
	int m_nPrimitive;						// プリミティブ数
	int m_nVertex;							// 頂点数

};

#endif