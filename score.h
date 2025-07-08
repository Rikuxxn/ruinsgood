//=============================================================================
//
// スコア処理 [score.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _SCORE_H_// このマクロ定義がされていなかったら
#define _SCORE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "number.h"
#include "object.h"
#include "number.h"


//*****************************************************************************
// スコアクラス
//*****************************************************************************
class CScore : public CObject
{
public:
	CScore(int nPriority = 7);
	~CScore();

	static CScore* Create(float baseX, float baseY, float digitWidth, float digitHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);
	void SetScore(int nScore);
	static void AddScore(int nValue);
	int DigitNum(int nScore);

private:
	static const int MAX_DIGITS = 8;		// 桁数
	CNumber* m_apNumber[MAX_DIGITS];		// 各桁の数字表示用
	static int m_nScore;					// スコア
	float m_digitWidth;						// 数字1桁あたりの幅
	float m_digitHeight;					// 数字1桁あたりの高さ
	D3DXVECTOR3 m_basePos;					// 表示の開始位置
	int m_nIdxTexture;

};

#endif
