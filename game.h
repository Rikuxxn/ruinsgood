//=============================================================================
//
// ゲーム処理 [game.h]
// Author : RIKU TANEKAWA
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
#include "blockmanager.h"
#include "ui.h"
#include "pausemanager.h"
#include "hinttext.h"

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
	void LoadObject(int stageId);

	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CTime* GetTime(void) { return m_pTime; }
	static CBlock* GetBlock(void) { return m_pBlock; }
	static CBlockManager* GetBlockManager(void) { return m_pBlockManager; }
	static CImGuiManager* GetImGuiManager(void) { return m_pImGuiManager; }
	static CObjectBillboard* GetBillboard(void) { return m_pBillboard; }
	static CUi* GetUi(void) { return m_pUi; }
	static CPauseManager* GetPauseManager(void) { return m_pPauseManager; }
	static CHintText* GetHintText(void) { return m_pHintText; }
	static bool GetisPaused(void) { return m_isPaused; };
	static void SetEnablePause(bool bPause);

private:
	static CPlayer* m_pPlayer;					// プレイヤーへのポインタ
	static CTime* m_pTime;						// タイムへのポインタ
	static CColon* m_pColon;					// コロンへのポインタ
	static CBlock* m_pBlock;					// ブロックへのポインタ
	static CBlockManager* m_pBlockManager;		// ブロックマネージャーへのポインタ
	static CImGuiManager* m_pImGuiManager;		// ImGuiマネージャーへのポインタ
	static CObjectBillboard* m_pBillboard;		// ビルボードへのポインタ
	static CUi* m_pUi;							// UIへのポインタ
	static CPauseManager* m_pPauseManager;		// ポーズマネージャーへのポインタ
	static CHintText* m_pHintText;				// ヒント壁画へのポインタ
	static bool m_isPaused;						// trueならポーズ中

};

#endif
