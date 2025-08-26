//=============================================================================
//
// UI処理 [ui.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _UI_H_// このマクロ定義がされていなかったら
#define _UI_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "object2D.h"
#include <unordered_map>
#include <functional>

class CUi;

using UiCreateFunc = std::function<CUi* ()>;

//*****************************************************************************
// UIクラス
//*****************************************************************************
class CUi : public CObject2D
{
public:
	CUi(int nPriority = 6);
	~CUi();

	// UIの種類
	typedef enum
	{
		TYPE_PAUSE = 0,
		TYPE_GET,
		TYPE_RESULT_UI,
		TYPE_RESULT_RANK,
		TYPE_STAGE_NAME,
		TYPE_MAX
	}TYPE;

	static CUi* Create(TYPE type, const char* path,D3DXVECTOR3 pos, float fWidth, float fHeight);
	static void InitFactory(void);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }
	void SetUV(float u, float v, float w, float h);  // UV設定

private:
	int m_nIdxTexture;
	char m_szPath[MAX_PATH];			// ファイルパス
	TYPE m_type;
	float m_uvLeft;
	float m_uvTop;
	float m_uvWidth;
	float m_uvHeight;
	bool m_isUVDirty;
	static std::unordered_map<TYPE, UiCreateFunc> m_UiFactoryMap;
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
	void Update(void);

};

//*****************************************************************************
// 秘宝取得UIクラス
//*****************************************************************************
class CGetUi : public CUi
{
public:
	CGetUi();
	~CGetUi();

	HRESULT Init(void);
	void Update(void);

private:
	float m_fTimer;// 経過時間(秒)
	float m_fAlpha;// アルファ値
	bool m_bFading;// フェード開始フラグ
};

//*****************************************************************************
// リザルトUIクラス
//*****************************************************************************
class CResultUi : public CUi
{
public:
	CResultUi();
	~CResultUi();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// リザルトUI(ランク)クラス
//*****************************************************************************
class CResultRankUi : public CUi
{
public:
	CResultRankUi();
	~CResultRankUi();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// ステージ名表示クラス
//*****************************************************************************
class CStageUi : public CUi
{
public:
	CStageUi();
	~CStageUi();

	// 状態
	typedef enum
	{
		STATE_FADEIN = 0,
		STATE_SHOW,
		STATE_FADEOUT,
		STATE_END,
		STATE_MAX
	}STATE;

	HRESULT Init(void);
	void Update(void);

private:
	float m_fTimer;	// 経過時間(秒)
	float m_fAlpha;	// アルファ値
	STATE m_state;	// 状態
};

#endif
