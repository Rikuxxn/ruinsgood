//=============================================================================
//
// カメラ処理 [camera.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _CAMERA_H_// このマクロ定義がされていなかったら
#define _CAMERA_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// カメラクラス
//*****************************************************************************
class CCamera
{
public:
	CCamera();
	~CCamera();

	// カメラの種類
	typedef enum
	{
		MODE_EDIT = 0,	// エディターカメラ
		MODE_GAME,		// ゲームカメラ
		MODE_DIRECTION,	// 演出カメラ
		MODE_MAX
	}MODE;

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void UpdateInfo(void);
	void SetCamera(void);
	void EditCamera(void);
	void GameCamera(void);
	void CameraWithGamepad(float stickX, float stickY);
	void AdjustCameraPosition(const D3DXVECTOR3& playerPos);
	void DirectionCamera(int nTimer);

	void SetPosV(D3DXVECTOR3 posV) { m_posV = posV; }
	void SetPosR(D3DXVECTOR3 posR) { m_posR = posR; }
	void SetRot(D3DXVECTOR3 rot) { m_rot = rot; }
	void SetDis(float fDistance) { m_fDistance = fDistance; }
	int  SetTimer(int nTime) { return m_nTimer = nTime * 60; }
	void SetCamMode(int nTimer, D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 rot);

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	D3DXVECTOR3 GetRot(void) { return m_rot; }			// カメラの角度の取得
	MODE GetMode(void) { return m_Mode; }

private:
	D3DXVECTOR3 m_posV;			// 視点
	D3DXVECTOR3 m_posVDest;		// 目的の視点
	D3DXVECTOR3 m_posR;			// 注視点
	D3DXVECTOR3 m_posRDest;		// 目的の注視点
	D3DXVECTOR3 m_vecU;			// 上方向ベクトル
	D3DXMATRIX m_mtxProjection;	// プロジェクションマトリックス
	D3DXMATRIX m_mtxView;		// ビューマトリックス
	D3DXVECTOR3 m_rot;			// 向き
	float m_fDistance;			// 視点から注視点の距離
	MODE m_Mode;				// カメラのモード
	int m_nDirectionCamTimer;
	int m_nTimer;
};

#endif