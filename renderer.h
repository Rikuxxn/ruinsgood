//=============================================================================
//
// レンダリング処理 [renderer.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RENDERER_H_// このマクロ定義がされていなかったら
#define _RENDERER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "debugproc.h"
#include "debugproc3D.h"

//*****************************************************************************
// レンダラークラス
//*****************************************************************************
class CRenderer
{
public:
	CRenderer();
	~CRenderer();

	HRESULT Init(HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(int fps);
	void ResetDevice(void);
	void OnResize(UINT width, UINT height);

	//*****************************************************************************
	// flagment関数
	//*****************************************************************************
	bool NeedsReset(void) const;

	//*****************************************************************************
	// setter関数
	//*****************************************************************************
	void SetFPS(int fps) { m_nFPS = fps; }
	void SetBgCol(D3DXCOLOR col) { m_bgCol = col; }

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	static CDebugProc* GetDebug(void) { return m_pDebug; }
	static CDebugProc3D* GetDebug3D(void) { return m_pDebug3D; }
	static int GetFPS(void) { return m_nFPS; }
	LPDIRECT3DDEVICE9 GetDevice(void) { return m_pD3DDevice; };
	D3DXCOLOR GetBgCol(void) { return m_bgCol; }

private:
	LPDIRECT3D9 m_pD3D;						// DirectX3Dオブジェクトへのポインタ
	LPDIRECT3DDEVICE9 m_pD3DDevice;			// デバイスへのポインタ
	static CDebugProc* m_pDebug;			// デバッグフォントへのポインタ
	static CDebugProc3D* m_pDebug3D;		// 3Dデバッグ表示へのポインタ
	D3DXCOLOR m_bgCol;						// 画面背景の色
	UINT m_ResizeWidth;						// 再設定用の画面の幅
	UINT m_ResizeHeight;					// 再設定用の画面の高さ
	D3DPRESENT_PARAMETERS m_d3dpp;			// 再設定用のパラメーター
	static int m_nFPS;						// FPS値の代入用
};
#endif