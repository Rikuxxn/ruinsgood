//=============================================================================
//
// モーション処理 [motion.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MOTION_H_// このマクロ定義がされていなかったら
#define _MOTION_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "model.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_WORD (1024)		// 最大文字数
#define MAX_PARTS (32)		// 最大パーツ数
#define MAX_KEY (128)		// 最大キー数

//*****************************************************************************
// モーションクラス
//*****************************************************************************
class CMotion
{
public:
	CMotion();
	~CMotion();

	// モーションの種類
	typedef enum
	{
		TYPE_NEUTRAL = 0,	// 待機
		TYPE_MOVE,			// 移動
		TYPE_HOLD,			// 掴み
		TYPE_JUMP,			// ジャンプ
		TYPE_BESIDEMOVE,	// 横移動
		TYPE_MAX
	}TYPE;

	static CMotion* Load(const char* pFilepath, CModel* pModel[], int& nNumModel);
	void LoadModelInfo(FILE* pFile, char* aString, CModel* pModel[], int& nNumModel, int& nIdx);
	void LoadCharacterSet(FILE* pFile, char* aString, CModel* pModel[], int nNumModel, int parentIdx[]);
	void LoadMotionSet(FILE* pFile, char* aString, CMotion* pMotion, int& nCntMotion);
	void Update(CModel* pModel[], int& nNumModel);
	void StartBlendMotion(TYPE motionTypeBlend, int nFrameBlend);
	void SetMotion(TYPE motionType);

private:

	//*************************************************************************
	// キー構造体
	//*************************************************************************
	typedef struct
	{
		float fPosX;							// 位置(X)
		float fPosY;							// 位置(Y)
		float fPosZ;							// 位置(Z)
		float fRotX;							// 向き(X)
		float fRotY;							// 向き(Y)
		float fRotZ;							// 向き(Z)
	}KEY;

	//*************************************************************************
	// キー情報の構造体
	//*************************************************************************
	typedef struct
	{
		int nFrame;								// 再生フレーム
		KEY aKey[MAX_PARTS];					// 各パーツのキー要素
	}KEY_INFO;

	//*************************************************************************
	// モーション情報の構造体
	//*************************************************************************
	typedef struct
	{
		bool bLoop;								// ループするかどうか
		int nNumKey;							// キーの総数
		KEY_INFO aKeyInfo[MAX_KEY];				// キー情報
		int startKey, startFrame;
	}MOTION_INFO;

	MOTION_INFO m_aMotionInfo[TYPE_MAX];		// モーション情報
	TYPE m_motionType;							// モーションの種類
	int m_nNumMotion;							// モーション総数
	bool m_bLoopMotion;							// ループするかどうか
	int m_nNumKey;								// キーの総数
	int m_nKey;									// 現在のキーNo.
	int m_nCounterMotion;						// モーションのカウンター

	bool m_bFinishMotion;						// 現在のモーションが終了しているかどうか
	bool m_bBlendMotion;						// ブレンドモーションがあるかどうか
	TYPE m_motionTypeBlend;						// ブレンドモーションがあるかどうか
	bool m_bLoopMotionBlend;					// ブレンドモーションがループするかどうか
	int m_nNumKeyBlend;							// ブレンドモーションのキー数
	int m_nKeyBlend;							// ブレンドモーションの現在のキーNo.
	int m_nCounterMotionBlend;					// ブレンドモーションのカウンター
	int m_nFrameBlend;							// ブレンドのフレーム数(何フレームかけてブレンドするか)
	int m_nCounterBlend;						// ブレンドカウンター

};

#endif