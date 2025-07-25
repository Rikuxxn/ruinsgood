//=============================================================================
//
// Xファイル処理 [objectX.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _OBJECTX_H_// このマクロ定義がされていなかったら
#define _OBJECTX_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "object.h"

//*****************************************************************************
// Xファイルクラス
//*****************************************************************************
class CObjectX : public CObject
{
public:
	CObjectX(int nPriority = 3);
	virtual ~CObjectX();

	static CObjectX* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	const char* GetPath(void);
	D3DXVECTOR3 GetPos(void);
	D3DXVECTOR3 GetRot(void);
	D3DXVECTOR3 GetSize(void);		// 拡大率
	D3DXVECTOR3 GetModelSize(void);	// モデルの元サイズ
	virtual D3DXCOLOR GetCol(void) const;
	D3DXCOLOR GetMaterialColor(void) const;

	void SetPath(const char* path);
	void SetSize(D3DXVECTOR3 size);
	void SetPos(D3DXVECTOR3 pos);
	void SetRot(D3DXVECTOR3 rot);

private:
	int* m_nIdxTexture;
	D3DXVECTOR3 m_pos;					// 位置
	D3DXVECTOR3 m_rot;					// 向き
	D3DXVECTOR3 m_move;					// 移動量
	D3DXVECTOR3 m_size;					// サイズ
	LPD3DXMESH m_pMesh;					// メッシュへのポインタ
	LPD3DXBUFFER m_pBuffMat;			// マテリアルへのポインタ
	DWORD m_dwNumMat;					// マテリアル数
	D3DXMATRIX m_mtxWorld;				// ワールドマトリックス
	char m_szPath[MAX_PATH];			// ファイルパス
	D3DXVECTOR3 m_modelSize;			// モデルの元サイズ（全体の幅・高さ・奥行き）
};

#endif