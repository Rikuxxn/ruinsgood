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
#include "input.h"
#include "sound.h"
#include "texture.h"
#include "camera.h"
#include "light.h"
#include "scene.h"
#include "fade.h"

//*****************************************************************************
// マネージャークラス
//*****************************************************************************
class CManager
{
public:
	CManager();
	~CManager();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CRenderer* GetRenderer(void) { return m_pRenderer; }
	static CInputKeyboard* GetInputKeyboard(void) { return m_pInputKeyboard; }
	static CInputJoypad* GetInputJoypad(void) { return m_pInputJoypad; }
	static CInputMouse* GetInputMouse(void) { return m_pInputMouse; }
	static CSound* GetSound(void) { return m_pSound; }
	static CTexture* GetTexture(void) { return m_pTexture; }
	static CCamera* GetCamera(void) { return m_pCamera; }
	static CLight* GetLight(void) { return m_pLight; }
	static CFade* GetFade(void) { return m_pFade; }

	int GetFPS(int fps) { return m_fps = fps; };
	int GetFPSCnt(void) { return m_fps; }
	static btDiscreteDynamicsWorld* GetPhysicsWorld(void) { return m_pDynamicsWorld; }

	static void SetMode(CScene::MODE mode);
	static CScene::MODE GetMode(void);

private:
	static CRenderer* m_pRenderer;				// レンダラーへのポインタ
	static CInputKeyboard* m_pInputKeyboard;	// キーボードへのポインタ
	static CInputJoypad* m_pInputJoypad;		// ジョイパッドへのポインタ
	static CInputMouse* m_pInputMouse;			// マウスへのポインタ
	static CSound* m_pSound;					// サウンドへのポインタ
	static CTexture* m_pTexture;				// テクスチャへのポインタ
	static CCamera* m_pCamera;					// カメラへのポインタ
	static CLight* m_pLight;					// ライトへのポインタ
	int m_fps;									// FPS値

	btBroadphaseInterface* m_pBroadphase;						// 衝突判定のクラスへのポインタ
	btDefaultCollisionConfiguration* m_pCollisionConfiguration ;// 衝突検出の設定を管理するクラスへのポインタ
	btCollisionDispatcher* m_pDispatcher;						// 実際に衝突判定処理を実行するクラスへのポインタ
	btSequentialImpulseConstraintSolver* m_pSolver;				// 物理シミュレーションの制約ソルバーへのポインタ
	static btDiscreteDynamicsWorld* m_pDynamicsWorld;			// 物理世界を管理するクラスへのポインタ

	static CFade* m_pFade;
	static CScene* m_pScene;
};

#endif