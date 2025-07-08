//=============================================================================
//
// マネージャー処理 [manager.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _MANAGER_H_// このマクロ定義がされていなかったら
#define _MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"
#include "renderer.h"
#include "object2D.h"
#include "input.h"
#include "sound.h"
#include "player.h"
#include "effect.h"
#include "score.h"
#include "time.h"
#include "pause.h"
#include "texture.h"
#include "camera.h"
#include "light.h"
#include "motion.h"
#include "block.h"
#include "blockmanager.h"

//*****************************************************************************
// マネージャークラス
//*****************************************************************************
class CManager
{
public:
	CManager();
	~CManager();

	HRESULT Init(HINSTANCE HInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CRenderer* GetRenderer(void) { return m_pRenderer; }
	static CObject2D* GetObject2D(void) { return m_pObject2D; }
	static CInputKeyboard* GetInputKeyboard(void) { return m_pInputKeyboard; }
	static CInputJoypad* GetInputJoypad(void) { return m_pInputJoypad; }
	static CInputMouse* GetInputMouse(void) { return m_pInputMouse; }
	static CSound* GetSound(void) { return m_pSound; }
	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CEffect* GetEffect(void) { return m_pEffect; }
	static CScore* GetScore(void) { return m_pScore; }
	static CTime* GetTime(void) { return m_pTime; }
	static CColon* GetColon(void) { return m_pColon; }
	static CPause* GetPause(void) { return m_pPause; }
	static CTexture* GetTexture(void) { return m_pTexture; }
	static CCamera* GetCamera(void) { return m_pCamera; }
	static CLight* GetLight(void) { return m_pLight; }
	static CModel* GetModel(void) { return m_pModel; }
	static CMotion* GetMotion(void) { return m_pMotion; }
	static CObjectX* GetObjectX(void) { return m_pObjectX; }
	static CBlock* GetBlock(void) { return m_pBlock; }
	static CBlockManager* GetBlockManager(void) { return m_pBlockManager; }
	static CImGuiManager* GetImGuiManager(void) { return m_pImGuiManager; }

	int GetFPS(int fps) { return m_fps = fps; };
	int GetFPSCnt(void) { return m_fps; }
	static bool GetisPaused(void);
	// 他クラスから物理ワールドを触りたい時用
	static btDiscreteDynamicsWorld* GetPhysicsWorld(void) { return m_pDynamicsWorld; }
	static void CheckCollisions(void);

private:
	static CRenderer* m_pRenderer;				// レンダラーへのポインタ
	static CObject2D* m_pObject2D;				// 2Dオブジェクトへのポインタ
	static CInputKeyboard* m_pInputKeyboard;	// キーボードへのポインタ
	static CInputJoypad* m_pInputJoypad;		// ジョイパッドへのポインタ
	static CInputMouse* m_pInputMouse;			// マウスへのポインタ
	static CSound* m_pSound;					// マウスへのポインタ
	static CPlayer* m_pPlayer;					// プレイヤーへのポインタ
	static CEffect* m_pEffect;					// エフェクトへのポインタ
	static CScore* m_pScore;					// スコアへのポインタ
	static CTime* m_pTime;						// タイムへのポインタ
	static CColon* m_pColon;					// コロンへのポインタ
	static CPause* m_pPause;					// ポーズへのポインタ
	static CTexture* m_pTexture;				// テクスチャへのポインタ
	static CCamera* m_pCamera;					// カメラへのポインタ
	static CLight* m_pLight;					// ライトへのポインタ
	static CModel* m_pModel;					// モデルへのポインタ
	static CMotion* m_pMotion;					// モーションへのポインタ
	static CBlock* m_pBlock;					// ブロックへのポインタ
	static CObjectX* m_pObjectX;				// Xファイルオブジェクトへのポインタ
	static CBlockManager* m_pBlockManager;		// ブロックマネージャーへのポインタ
	static CImGuiManager* m_pImGuiManager;		// ImGuiマネージャーへのポインタ

	static std::vector<CBlock*> m_blocks;  // 複数のブロックを保持
	int m_fps;
	static bool m_isPaused; // trueならポーズ中

	btBroadphaseInterface* m_pBroadphase;						// おおまか衝突判定のクラスへのポインタ
	btDefaultCollisionConfiguration* m_pCollisionConfiguration ;// 衝突検出の設定を管理するクラスへのポインタ
	btCollisionDispatcher* m_pDispatcher;						// 実際に衝突判定処理を実行するクラスへのポインタ
	btSequentialImpulseConstraintSolver* m_pSolver;				// 物理シミュレーションの制約ソルバーへのポインタ
	static btDiscreteDynamicsWorld* m_pDynamicsWorld;			// 物理世界を管理するクラスへのポインタ
};

#endif