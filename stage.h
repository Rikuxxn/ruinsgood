//=============================================================================
//
// ステージ処理 [stage.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _STAGE_H_// このマクロ定義がされていなかったら
#define _STAGE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "object.h"


//*****************************************************************************
// ステージクラス
//*****************************************************************************
class CStage : public CObject
{
public:
	CStage(int nPriority = 7);
	~CStage();

	// 選択項目の種類
	typedef enum
	{
		STAGE_ID_1 = 0,		
		STAGE_ID_2,
		STAGE_ID_3,
		STAGE_ID_NONE,
		STAGE_MAX
	}STAGE;

	static CStage* Create(STAGE type, D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	D3DXVECTOR3 GetPos(void);
	bool IsMouseOver(void);
	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }
	void SetCol(D3DXCOLOR col) { m_col = col; }
	virtual void Execute(void) {};

	// 選択状態設定・取得
	void SetSelected(bool selected) { m_isSelected = selected; }
	bool IsSelected(void) const { return m_isSelected; }
	void SetTargetPos(const D3DXVECTOR3& targetPos) { m_targetPos = targetPos; }
	void StartSlideIn(bool flag) { m_isSlidingIn = flag; }
	void StartSlideOut(bool flag) { m_isSlidingOut = flag; }
	bool IsSlideIn(void) { return m_isSlidingIn; }
	bool IsSlideOut(void) { return m_isSlidingOut; }
	bool IsSlideInFinished(void) { return m_isSlideInFinished; }
	bool IsSlideOutFinished(void) { return m_isSlideOutFinished; }
	void SlideIn(void);
	void SlideOut(void);
	STAGE GetStageId(void) const;

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DCOLOR m_col;							// 色
	float m_fWidth, m_fHeight;				// サイズ
	int m_nIdxTexture;						// テクスチャインデックス
	char m_szPath[MAX_PATH];				// ファイルパス
	bool m_isSelected;						// 選択したか
	D3DXVECTOR3 m_startPos;					// スライドイン開始位置（画面右外）
	D3DXVECTOR3 m_targetPos;				// スライドイン完了位置（目的位置）
	bool m_isSlidingIn ;					// スライド中フラグ
	bool m_isSlidingOut;					// スライドアウトフラグ
	bool m_isSlideInFinished;
	bool m_isSlideOutFinished;
};

//*****************************************************************************
// ステージ1クラス
//*****************************************************************************
class CStage1 : public CStage
{
public:
	CStage1();
	~CStage1();

	void Execute(void) override;

private:

};

//*****************************************************************************
// ステージ2クラス
//*****************************************************************************
class CStage2 : public CStage
{
public:
	CStage2();
	~CStage2();

	void Execute(void) override;

private:

};

//*****************************************************************************
// ステージ3クラス
//*****************************************************************************
class CStage3 : public CStage
{
public:
	CStage3();
	~CStage3();

	void Execute(void) override;

private:

};

//*****************************************************************************
// 戻る項目クラス
//*****************************************************************************
class CBack : public CStage
{
public:
	CBack();
	~CBack();

	void Execute(void) override;

private:

};

#endif