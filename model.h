//=============================================================================
//
// モデル処理 [model.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _MODEL_H_// このマクロ定義がされていなかったら
#define _MODEL_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "object.h"

//*****************************************************************************
// モデルクラス
//*****************************************************************************
class CModel
{
public:
	CModel();
	~CModel();

	static CModel* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetParent(CModel* pModel);
	D3DXMATRIX GetMtxWorld(void);
	D3DXVECTOR3 GetPos(void);
	D3DXVECTOR3 GetRot(void);
	void SetPos(D3DXVECTOR3 pos);
	void SetRot(D3DXVECTOR3 rot);

	D3DXVECTOR3 GetOffsetPos(void) const { return m_OffsetPos; }
	D3DXVECTOR3 GetOffsetRot(void) const { return m_OffsetRot; }

	void SetOffsetPos(const D3DXVECTOR3& pos) { m_OffsetPos = pos; }
	void SetOffsetRot(const D3DXVECTOR3& rot) { m_OffsetRot = rot; }

private:
	int* m_nIdxTexture;
	D3DXVECTOR3 m_pos;					// 位置
	D3DXVECTOR3 m_rot;					// 向き
	D3DXVECTOR3 m_move;					// 移動量
	LPD3DXMESH m_pMesh;					// メッシュへのポインタ
	LPD3DXBUFFER m_pBuffMat;			// マテリアルへのポインタ
	DWORD m_dwNumMat;					// マテリアル数
	D3DXMATRIX m_mtxWorld;				// ワールドマトリックス
	CModel* m_pParent;					// 親モデルへのポインタ
	char m_Path[MAX_PATH];
	D3DXVECTOR3 m_OffsetPos;
	D3DXVECTOR3 m_OffsetRot;
};

#endif
