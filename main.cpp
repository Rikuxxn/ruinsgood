//=============================================================================
//
// メイン処理 [main.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"
#include "renderer.h"
#include "object.h"
#include "object2D.h"
#include <stdlib.h>
#include <crtdbg.h>
#include "manager.h"
#include <tchar.h>
#include <stdio.h>
#include "imguimaneger.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ToggleFullScreen(HWND hWnd);// ウィンドウをフルスクリーン

//=============================================================================
// メイン関数
//=============================================================================
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hInstancePrev, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// メモリリーク検知
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	//_CrtSetBreakAlloc(12455);// リークの原因のnewに強制的にブレークポイントを発生させる

	int nCountFPS = 0;// FPSカウンター

	CManager* pManager = NULL;

	CImGuiManager* pImGuiManager = NULL;

	// マネージャーの生成
	pManager = new CManager;

	// ImGuiの生成
	pImGuiManager = new CImGuiManager;


	pManager->GetFPS(nCountFPS);

	DWORD dwCurrentTime;// 現在時刻
	DWORD dwExecLastTime;// 最後に処理した時刻

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),             // WNDCLASSEXのメモリサイズ
		CS_CLASSDC,                     // ウィンドウのスタイル
		WindowProc,                     // ウィンドウプロシージャ
		0,                              // 0にする(通常は使用しない)
		0,                              // 0にする
		hInstance,                      // インスタンスハンドル
		LoadIcon(NULL,IDI_APPLICATION), // タスクバーのアイコン
		LoadCursor(NULL,IDC_ARROW),     // マウスカーソル
		(HBRUSH)(COLOR_WINDOW + 1),     // クライアント領域の背景色
		NULL,                           // メニューバー
		CLASS_NAME,                     // ウィンドウクラスの名前
		LoadIcon(NULL,IDI_APPLICATION)  // ファイルのアイコン
	};


	HWND hWnd;   // ウィンドウハンドル
	MSG msg;     // メッセージを格納する変数

	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// クライアント領域を指定のサイズに調整
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウィンドウを生成
	hWnd = CreateWindowEx(0,    // 拡張ウィンドウスタイル
		CLASS_NAME,             // ウィンドウクラスの名前
		WINDOW_NAME,            // ウィンドウの名前
		WS_OVERLAPPEDWINDOW,    // ウィンドウスタイル
		CW_USEDEFAULT,          // ウィンドウの左上x座標
		CW_USEDEFAULT,          // ウィンドウの左上y座標

        (rect.right-rect.left), // 幅
		(rect.bottom-rect.top), // 高さ

		NULL,                   // 親ウィンドウのハンドル
		NULL,                   // メニューハンドルまたは子ウィンドウID
		hInstance,              // インスタンスハンドル
		NULL);                  // ウィンドウ作成データ


	// マネージャーの初期化処理
	if (FAILED(pManager->Init(hInstance, hWnd)))
	{
		return -1;
	}

	// 分解能を設定
	timeBeginPeriod(1);
	dwCurrentTime = 0;					// 初期化する
	dwExecLastTime = timeGetTime();		// 現在時刻を取得（保存）

	// ウィンドウの表示
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	DWORD dwFrameCount;					// フレームカウント
	DWORD dwFPSLastTime;				// 最後にFPSを計測した時刻

	dwFrameCount = 0;
	dwFPSLastTime = timeGetTime();

	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	//メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{// windowsの処理
			if (msg.message == WM_QUIT)
			{// WM_QUITメッセージを受け取ったらメッセージループを抜ける
				break;
			}
			else
			{
				//メッセージの設定
				TranslateMessage(&msg);   // 仮想メッセージを文字メッセージへ変換
				DispatchMessage(&msg);    // ウィンドウ
			}
		}
		else
		{// DirectXの処理

			dwCurrentTime = timeGetTime();// 現在時刻を取得

			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{// 0.5秒経過

				// FPSを計測
				nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);

				pManager->GetFPS(nCountFPS);

				dwFPSLastTime = dwCurrentTime;// FPSを計測した時刻を保存

				dwFrameCount = 0;// フレームカウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{// 60分の1秒経過

				dwExecLastTime = dwCurrentTime;// 処理開始の時刻[現在時刻]を保存

				dwCurrentTime = timeGetTime();// 現在時刻を取得

				dwFrameCount++;// フレームカウントを加算

				if (pRenderer->NeedsReset())
				{
					// デバイスのリセット
					pRenderer->ResetDevice();
				}

				// フレーム開始
				pImGuiManager->NewFlameImgui();

				// マネージャーの更新処理
				pManager->Update();

				// フレーム終了
				ImGui::EndFrame();

				//マネージャーの描画処理
				pManager->Draw();
			}
		}
	}

	// マネージャーの破棄
	if (pManager != NULL)
	{
		// マネージャーの終了処理
		pManager->Uninit();

		delete pManager;
		pManager = NULL;
	}

	// ImGuiマネージャーの破棄
	if (pImGuiManager != NULL)
	{
		// ImGuiマネージャーの終了処理
		pImGuiManager->Uninit();

		delete pImGuiManager;
		pImGuiManager = NULL;
	}

	// 分解能を戻す
	timeEndPeriod(1);

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	return (int)msg.wParam;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//=============================================================================
// ウィンドウプロシージャ
//=============================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//HDC hDC;//デバイスコンテキスト(GDIオブジェクト)のハンドル

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	const RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	int nID;

	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	switch (uMsg)
	{
	case WM_DESTROY:  //ウィンドウ破棄のメッセージ
		//WM_QUITメッセージを送る
		PostQuitMessage(0);

		break;
	case WM_SIZE:

		if (wParam == SIZE_MINIMIZED)
		{
			return 0;
		}

		pRenderer->OnResize(LOWORD(lParam), HIWORD(lParam));

		return 0;

	case WM_KEYDOWN://キー押下のメッセージ
		switch (wParam)
		{
		case VK_ESCAPE://[ESC]キーが押された

			//ウィンドウを破棄する
			nID = MessageBox(hWnd, "終了しますか?", "終了メッセージ", MB_YESNO);
			if (nID == IDYES)
			{
				DestroyWindow(hWnd);//ウインドウ破棄メッセージ
			}
			else
			{
				return 0;            //返す
			}

			break;

		case VK_F11:

			// フルスクリーン
			ToggleFullScreen(hWnd);

			break;
		}
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);//既定の処理を繰り返す
}
//=============================================================================
// ウィンドウフルスクリーン処理
//=============================================================================
void ToggleFullScreen(HWND hWnd)
{
	static bool isFullscreen = true;

	// 現在のウィンドウスタイルを取得
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);

	// ウィンドウを切り替えるための変数
	RECT windowRect = {};

	if (isFullscreen)
	{
		// ウィンドウモードに切り替え
		SetWindowLong(hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, windowRect.left, windowRect.top,
			windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_NORMAL);
	}
	else
	{
		// フルスクリーンモードに切り替え
		GetWindowRect(hWnd, &windowRect);
		SetWindowLong(hWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_MAXIMIZE);
	}

	isFullscreen = !isFullscreen;
}