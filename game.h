//=============================================================================
//
// ゲーム処理 [game.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _GAME_H_// このマクロ定義がされていなかったら
#define _GAME_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "scene.h"
#include "player.h"
#include "score.h"
#include "time.h"
#include "pause.h"
#include "blockmanager.h"

//*****************************************************************************
// ゲームクラス
//*****************************************************************************
class CGame : public CScene
{
public:
	CGame();
	~CGame();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static bool GetisPaused(void) { return m_isPaused; };
	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CScore* GetScore(void) { return m_pScore; }
	static CTime* GetTime(void) { return m_pTime; }
	static CBlock* GetBlock(void) { return m_pBlock; }
	static CBlockManager* GetBlockManager(void) { return m_pBlockManager; }
	static CImGuiManager* GetImGuiManager(void) { return m_pImGuiManager; }
	
private:
	static CPlayer* m_pPlayer;					// プレイヤーへのポインタ
	static CScore* m_pScore;					// スコアへのポインタ
	static CTime* m_pTime;						// タイムへのポインタ
	static CColon* m_pColon;					// コロンへのポインタ
	static CPause* m_pPause;					// ポーズへのポインタ
	static CBlock* m_pBlock;					// ブロックへのポインタ
	static CBlockManager* m_pBlockManager;		// ブロックマネージャーへのポインタ
	static CImGuiManager* m_pImGuiManager;		// ImGuiマネージャーへのポインタ

	static bool m_isPaused;						// trueならポーズ中

};

#endif
