//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _PLAYER_H_// このマクロ定義がされていなかったら
#define _PLAYER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "model.h"
#include "motion.h"
#include "imguimaneger.h"
#include "debugproc3D.h"
#include "block.h"
#include "shadowS.h"
#include "effect.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PARTS		(32)					// 最大パーツ数
#define PLAYER_SPEED	(200.0f)				// 移動スピード
#define SIDE_MOVE_SPEED (PLAYER_SPEED * 0.6f)	// 横移動は60%のスピード
#define MAX_JUMP_POWER	(330.3f)				// ジャンプ初速
#define MAX_GRAVITY		(-0.26f)				// 重力加速度

//*****************************************************************************
// プレイヤークラス
//*****************************************************************************
class CPlayer : public CObject
{
public:
	CPlayer(int nPriority = 2);
	~CPlayer();

	static CPlayer* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void UpdateInfo(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);
	D3DXVECTOR3 GetRot(void)const { return m_rot; };
	D3DXVECTOR3 GetMove(void) const { return m_move; }
	D3DXVECTOR3 GetColliderPos(void) const { return m_colliderPos; }
	btScalar GetRadius(void) const { return m_radius; }
	btScalar GetHeight(void) const { return m_height; }
	btRigidBody* GetRigidBody(void) const { return m_pRigidBody; }						// RigidBodyの取得
	bool GetPlayerUse(void) const { return m_playerUse; }
	bool OnGround(btDiscreteDynamicsWorld* world, btRigidBody* playerBody, float rayLength);
	void ReleasePhysics(void);														// Physics破棄用
	void HoldBlock(void);
	D3DXVECTOR3 GetForward(void) const;
	CBlock* FindFrontBlockByRaycast(float rayLength);
	void Controll(void);
	void SetInWater(bool flag);
	void AddWaterStayTime(float delta);
	void ResetWaterStayTime(void);
	void RespawnToCheckpoint(void);
	void RespawnToCheckpoint(D3DXVECTOR3 pos);
	void AddRespawnPoint(const D3DXVECTOR3& pos);
	D3DXVECTOR3 GetNearestRespawnPoint(void) const;
	D3DXVECTOR3 Lerp(const D3DXVECTOR3& a, const D3DXVECTOR3& b, float t);

private:
	D3DXVECTOR3 m_pos;					// 位置
	D3DXVECTOR3 m_rot;					// 向き
	D3DXVECTOR3 m_rotDest;				// 向き
	D3DXVECTOR3 m_move;					// 移動量
	D3DXVECTOR3 m_targetMove;			// 目標速度
	D3DXVECTOR3 m_currentMove;			// 実際の移動速度
	D3DXVECTOR3 m_size;					// サイズ
	D3DXVECTOR3 m_colliderPos;			// カプセルコライダーの位置
	D3DXMATRIX m_mtxWorld;				// ワールドマトリックス
	CModel* m_apModel[MAX_PARTS];		// モデル(パーツ)へのポインタ
	CShadowS* m_pShadowS;				// ステンシルシャドウへのポインタ
	CMotion* m_pMotion;					// モーションへのポインタ
	CMotion::TYPE m_currentMotion;		// 現在のモーション
	btRigidBody* m_pRigidBody;			// 剛体へのポインタ
	btCollisionShape* m_pShape;			// 当たり判定の形へのポインタ
	CDebugProc3D* m_pDebug3D;			// 3Dデバッグ表示へのポインタ
	btScalar m_radius;					// カプセルコライダーの半径
	btScalar m_height;					// カプセルコライダーの高さ
	CBlock* m_pCarryingBlock;			// 運んでいるブロック
	int m_nNumModel;					// モデル(パーツ)の総数
	int m_jumpFrame;					// ジャンプしてから何フレーム経過したか
	const int JUMP_HOLD_FRAMES = 60;	// このフレーム数まではジャンプ中とみなす
	bool m_playerUse;					// 使われているかどうか
	bool m_isJumping;					// ジャンプ中フラグ
	bool m_bIsMoving;					// 移動入力フラグ
	bool m_bIsSideMoving;				// 横移動入力フラグ
	bool m_bOnGround;					// 接地フラグ
	float m_waterStayTime;				// 水中滞在時間（秒）
	bool m_isInWater;					// 今水中にいるか
	std::vector<D3DXVECTOR3> m_ResPos;
};

#endif
