//=============================================================================
//
// UI処理 [ui.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _UI_H_// このマクロ定義がされていなかったら
#define _UI_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "object.h"

//*****************************************************************************
// UIクラス
//*****************************************************************************
class CUi : public CObject
{
public:
	CUi(int nPriority = 7);
	~CUi();

	// UIの種類
	typedef enum
	{
		TYPE_PAUSE = 0,
		TYPE_MASK,
	}TYPE;

	static CUi* Create(TYPE type, D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void) { return m_pos; }

	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetSize(float fWidth, float fHeight) { m_fWidth = fWidth; m_fHeight = fHeight; }
	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	D3DXVECTOR3 m_pos;
	float m_fWidth;					// 幅
	float m_fHeight;				// 高さ
	int m_nIdxTexture;
	char m_szPath[MAX_PATH];			// ファイルパス
	TYPE m_type;
};

//*****************************************************************************
// ポーズUIクラス
//*****************************************************************************
class CPauseUi : public CUi
{
public:
	CPauseUi();
	~CPauseUi();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

};

//*****************************************************************************
// 仮面取得UIクラス
//*****************************************************************************
class CMaskUi : public CUi
{
public:
	CMaskUi();
	~CMaskUi();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

};


#endif
