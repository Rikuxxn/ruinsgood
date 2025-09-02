//=============================================================================
//
// ブロックリスト処理 [blocklist.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCKLIST_H_
#define _BLOCKLIST_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "block.h"

//*****************************************************************************
// 木箱ブロッククラス
//*****************************************************************************
class CWoodBoxBlock : public CBlock
{
public:
	CWoodBoxBlock();
	~CWoodBoxBlock();

	HRESULT Init(void);
	void Update(void);
	void Respawn(void);
	void Set(D3DXVECTOR3 pos);
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	btScalar GetMass(void) const { return 4.0f; }  // 質量の取得

private:
	D3DXVECTOR3 m_ResPos;	// リスポーン位置
};

//*****************************************************************************
// 柱ブロッククラス
//*****************************************************************************
class CPillarBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 1.0f); }
	btScalar GetMass(void) const { return 55.0f; }  // 質量の取得
};

//*****************************************************************************
// 木の橋ブロッククラス
//*****************************************************************************
class CWoodBridgeBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	btScalar GetMass(void) const { return 8.0f; }  // 質量の取得
};

//*****************************************************************************
// 筏ブロッククラス
//*****************************************************************************
class CRaftBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	float CarryTargetDis(void) { return 80.0f; }
	btScalar GetMass(void) const { return 7.5f; }  // 質量の取得
};

//*****************************************************************************
// 四角い岩ブロッククラス
//*****************************************************************************
class CBoxRockBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	float CarryTargetDis(void) { return 80.0f; }
	btScalar GetMass(void) const { return 7.0f; }  // 質量の取得

};

//*****************************************************************************
// 溶岩ブロッククラス
//*****************************************************************************
class CLavaBlock : public CBlock
{
public:
	CLavaBlock();
	~CLavaBlock();

	void Update(void);

private:

};

//*****************************************************************************
// 水ブロッククラス
//*****************************************************************************
class CWaterBlock : public CBlock
{
public:
	CWaterBlock();
	~CWaterBlock();

	void Update(void);
	void SetInWater(bool flag);
	void AddWaterStayTime(void);
	void ResetWaterStayTime(void);
	void IsRespawn(bool flag) { m_isRespawn = flag; }

private:
	void ApplyToBlocks(void);   // 他ブロックに浮力
	void ApplyToPlayer(void);   // プレイヤーに浮力

	int m_waterStayTime;		// 水中滞在時間（秒）
	bool m_isInWater;			// 今水中にいるか
	bool m_bWasInWater;			// 水に入ったか
	bool m_isRespawn;			// リスポーン状態か
};

//*****************************************************************************
// ドアブロッククラス
//*****************************************************************************
class CDoorBlock : public CBlock
{
public:
	CDoorBlock();
	~CDoorBlock();

	HRESULT Init(void);
	void Update(void);

private:
	bool m_isDoorOpened;			// 開いたかどうか
	D3DXVECTOR3 m_initialPos;		// 初期位置
};

//*****************************************************************************
// 最終エリアドアブロッククラス
//*****************************************************************************
class CBigDoorBlock : public CBlock
{
public:
	CBigDoorBlock();
	~CBigDoorBlock();

	HRESULT Init(void);
	void Update(void);

private:
	bool m_isDoorOpened;			// 開いたかどうか
	D3DXVECTOR3 m_openPos;
};

//*****************************************************************************
// スイッチブロッククラス
//*****************************************************************************
class CSwitchBlock : public CBlock
{
public:
	CSwitchBlock();
	~CSwitchBlock();

	HRESULT Init(void);
	void Update(void);

private:
	bool m_isSwitchOn;				// 押されたかどうか
	bool m_prevSwitchOn;			// 直前のスイッチ状態
	D3DXVECTOR3 m_closedPos;		// スイッチの閉じるときの位置
};

//*****************************************************************************
// 橋制御スイッチブロッククラス
//*****************************************************************************
class CBridgeSwitchBlock : public CBlock
{
public:
	CBridgeSwitchBlock();
	~CBridgeSwitchBlock();

	HRESULT Init(void);
	void Update(void);
	float CalcStackMass(CBlock* base);
	bool IsOnTop(CBlock* base, CBlock* other);
	bool IsSwitchOn(void) { return m_isSwitchOn; }

private:
	bool m_isSwitchOn;				// 押されたかどうか
	bool m_prevSwitchOn;			// 直前のスイッチ状態
	D3DXVECTOR3 m_closedPos;		// スイッチの閉じるときの位置
};

//*****************************************************************************
// 格子と足場制御スイッチブロッククラス
//*****************************************************************************
class CBarSwitchBlock : public CBlock
{
public:
	CBarSwitchBlock();
	~CBarSwitchBlock();

	HRESULT Init(void);
	void Update(void);
	bool IsSwitchOn(void) { return m_isSwitchOn; }
	void SetTimer(int nTimer) { m_Timer = nTimer * 60; }

private:
	bool m_isSwitchOn;				// 押されたかどうか
	bool m_prevSwitchOn;			// 直前のスイッチ状態
	D3DXVECTOR3 m_closedPos;		// スイッチの閉じるときの位置
	int m_timerCnt;
	int m_Timer;
};

//*****************************************************************************
// 斧ブロッククラス
//*****************************************************************************
class CAxeBlock : public CBlock
{
public:
	CAxeBlock();
	~CAxeBlock();

	void Update(void);
	void Swing(void);
	void IsPlayerHit(void);
	btScalar GetMass(void) const { return 80.0f; }  // 質量の取得

private:
	int m_nSwingCounter;		// フレームカウンター
	float m_swingAmplitude;		// ±振れ角
	float m_swingPeriod;		// 周期フレーム
	bool m_isPrevHit;
	float m_prevSwingAngle;		// 前回のスイング角度
	bool m_wasPositive;			// 前回の角度が正かどうか
	int m_playedSoundID;		// 再生中の音ID
};

//*****************************************************************************
// 岩ブロッククラス
//*****************************************************************************
class CRockBlock : public CBlock
{
public:
	CRockBlock();
	~CRockBlock();

	void Update(void);
	void Respawn(void);
	void AddPathPoint(const D3DXVECTOR3& point);// チェックポイント追加 (通常時用)
	void MoveToTarget(void);					// 転がし処理
	void IsPlayerHit(void);						// プレイヤーとの接触判定
	void UseBridgeSwitch(bool enable) { m_isBridgeSwitchOn = enable; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	bool IsDynamicBlock(void) const override { return true; }
	btScalar GetMass(void) const { return 100.0f; }  // 質量の取得

private:
	std::vector<D3DXVECTOR3> m_pathPoints;		// チェックポイントの配列 (代入用)
	int m_currentTargetIndex;					// 今の目標地点インデックス
	float m_speed;								// 力の強さ（速度の代わり）
	bool m_isBridgeSwitchOn;
	bool m_isHit;
	bool m_isPrevHit;
	bool m_isThrough;							// 通過したか
	bool m_isPrevThrough;						// 直前に通過したか
	int m_particleTimer;						// タイマー
	const int ROLL_PARTICLE_INTERVAL = 8;		// パーティクル発生間隔（フレーム数）
	int m_playedRollSoundID;					// 再生中の音ID
};

//*****************************************************************************
// 橋ブロッククラス
//*****************************************************************************
class CBridgeBlock : public CBlock
{
public:
	CBridgeBlock();
	~CBridgeBlock();

	void Update(void);
	void Move(void);

private:
};

//*****************************************************************************
// ターゲットブロッククラス
//*****************************************************************************
class CTargetBlock : public CBlock
{
public:
	CTargetBlock();
	~CTargetBlock();

	void Update(void);
	bool IsHit(void) { return m_isHit; }

private:
	bool m_isHit;		// 岩が当たったかどうか
	bool m_isPrevHit;	// 直前に当たったか
};

//*****************************************************************************
// 壁掛け松明ブロッククラス
//*****************************************************************************
class CTorchBlock : public CBlock
{
public:
	CTorchBlock();
	~CTorchBlock();

	void Update(void);

private:
	int m_playedSoundID;	// 再生中の音ID
};

//*****************************************************************************
// 置き型松明ブロッククラス
//*****************************************************************************
class CTorch2Block : public CBlock
{
public:
	CTorch2Block();
	~CTorch2Block();

	void Update(void);
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	bool IsDynamicBlock(void) const override { return true; }
	btScalar GetMass(void) const { return 7.0f; }  // 質量の取得

private:
	int m_playedSoundID;// 再生中の音ID
};

//*****************************************************************************
// 置き型松明(動かない)ブロッククラス
//*****************************************************************************
class CTorch3Block : public CBlock
{
public:
	CTorch3Block();
	~CTorch3Block();

	void Update(void);
	void SetHit(bool flag) { m_isHit = flag; }
	bool IsHit(void) { return m_isHit; }

private:
	int m_playedSoundID;// 再生中の音ID
	bool m_isHit;
};

//*****************************************************************************
// 仮面ブロッククラス
//*****************************************************************************
class CMaskBlock : public CBlock
{
public:
	CMaskBlock();
	~CMaskBlock();

	void Update(void);
	bool IsGet(void) { return m_isGet; }

private:
	bool m_isGet;
	int m_playedSoundID;					// 再生中の音ID
};

//*****************************************************************************
// 剣ブロッククラス
//*****************************************************************************
class CSwordBlock : public CBlock
{
public:
	CSwordBlock();
	~CSwordBlock();

	void Update(void);
	bool IsEnd(void) { return m_isEnd; }

private:
	bool m_isEnd;
	int m_playedSoundID;					// 再生中の音ID
};

//*****************************************************************************
// 鉄格子ブロッククラス
//*****************************************************************************
class CBarBlock : public CBlock
{
public:
	CBarBlock();
	~CBarBlock();

	void Update(void);

private:
	bool m_isOpened;
};

//*****************************************************************************
// 足場橋ブロッククラス
//*****************************************************************************
class CFootingBlock : public CBlock
{
public:
	CFootingBlock();
	~CFootingBlock();

	void Update(void);
	bool GetMove(void) { return m_isMove; }

private:
	bool m_isMove;
};

//*****************************************************************************
// 火炎放射像ブロッククラス
//*****************************************************************************
class CFireStatueBlock : public CBlock
{
public:
	CFireStatueBlock();
	~CFireStatueBlock();

	void Update(void);
	void SetParticle(void);

private:
	int m_playedSoundID;					// 再生中の音ID
	bool m_isBlocked;
};

//*****************************************************************************
// 火炎放射像(動く)ブロッククラス
//*****************************************************************************
class CMoveFireStatueBlock : public CBlock
{
public:
	CMoveFireStatueBlock();
	~CMoveFireStatueBlock();

	void Update(void);
	void SetParticle(void);

	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	btScalar GetFriction(void) const { return 2.0f; }
	float CarryTargetDis(void) { return 100.0f; }
	btScalar GetMass(void) const { return 7.0f; }  // 質量の取得

private:
	int m_playedSoundID;					// 再生中の音ID
};

//*****************************************************************************
// 火炎放射像(回転)ブロッククラス
//*****************************************************************************
class CTurnFireStatueBlock : public CBlock
{
public:
	CTurnFireStatueBlock();
	~CTurnFireStatueBlock();

	void Update(void);
	void SetParticle(void);

private:
	int m_playedSoundID;					// 再生中の音ID
};

//*****************************************************************************
// 鍵の柵ブロッククラス
//*****************************************************************************
class CKeyFenceBlock : public CBlock
{
public:
	CKeyFenceBlock();
	~CKeyFenceBlock();

	HRESULT Init(void);
	void Update(void);

private:
	D3DXVECTOR3 m_closedPos;
	bool m_prevDown;
};

//*****************************************************************************
// 鍵ブロッククラス
//*****************************************************************************
class CKeyBlock : public CBlock
{
public:
	CKeyBlock();
	~CKeyBlock();

	HRESULT Init(void);
	void Update(void);
	void Respawn(void);
	void Set(D3DXVECTOR3 pos);

	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	bool IsDynamicBlock(void) const override { return true; }
	btScalar GetMass(void) const { return 6.0f; }  // 質量の取得

private:
	D3DXVECTOR3 m_ResPos;

};

//*****************************************************************************
// 鍵の台座ブロッククラス
//*****************************************************************************
class CKeyPedestalBlock : public CBlock
{
public:
	CKeyPedestalBlock();
	~CKeyPedestalBlock();

	HRESULT Init(void);
	void Update(void);
	bool IsSet(void) { return m_isSet; }

private:
	D3DXVECTOR3 m_Pos;
	bool m_isSet;// 台座に設置したか
	bool m_prevIsSet;// 直前に設置したか
};

//*****************************************************************************
// 鍵ドアブロッククラス
//*****************************************************************************
class CKeyDoorBlock : public CBlock
{
public:
	CKeyDoorBlock();
	~CKeyDoorBlock();

	HRESULT Init(void);
	void Update(void);

private:
	D3DXVECTOR3 m_openPos;
	bool m_isGoal;	// 目標位置に移動したか
	bool m_isSet;	// 設置されたか
	int m_deleyTime;// 開くまでの遅延時間
	bool prevIsSet;	// 直前に設置したか
};

//*****************************************************************************
// 盾ブロッククラス
//*****************************************************************************
class CShieldBlock : public CBlock
{
public:
	CShieldBlock();
	~CShieldBlock();

	void Update(void);
	bool IsEnd(void) { return m_isEnd; }

private:
	bool m_isEnd;
	int m_playedSoundID;					// 再生中の音ID

};

//*****************************************************************************
// 石像ブロッククラス
//*****************************************************************************
class CStatueBlock : public CBlock
{
public:
	CStatueBlock();
	~CStatueBlock();

	void Update(void);

private:
	int m_playedSoundID;					// 再生中の音ID
};

//*****************************************************************************
// 火をつけると動く石像ブロッククラス
//*****************************************************************************
class CStatueBlock2 : public CBlock
{
public:
	CStatueBlock2();
	~CStatueBlock2();

	HRESULT Init(void);
	void Update(void);
	void Move(void);

private:
	D3DXVECTOR3 m_startPos;
	int m_playedSoundID;					// 再生中の音ID
	bool m_triggerDis;						// 松明が一定距離に入ったか
	bool m_isMoving;
	bool m_hasTriggered;
};

//*****************************************************************************
// 卵ブロッククラス
//*****************************************************************************
class CEggBlock : public CBlock
{
public:
	CEggBlock();
	~CEggBlock();

	void Update(void);
	bool IsGet(void) { return m_isGet; }

private:
	bool m_isGet;
	int m_playedSoundID;					// 再生中の音ID
};

//*****************************************************************************
// ドアトリガーブロッククラス
//*****************************************************************************
class CDoorTriggerBlock : public CBlock
{
public:
	CDoorTriggerBlock();
	~CDoorTriggerBlock();

	HRESULT Init(void);
	void Update(void);
	bool IsSet(void) { return m_isSet; }

private:
	D3DXVECTOR3 m_Pos;
	bool m_isSet;		// 設置したか
	bool m_prevIsSet;	// 直前に設置したか
	bool m_isSetCam;	// カメラを設定したか
	int m_nDeleyTime;	// カメラ移行までの遅延時間
};

//*****************************************************************************
// 質量ブロック(赤)クラス
//*****************************************************************************
class CRedMassBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	float CarryTargetDis(void) { return 80.0f; }
	btScalar GetMass(void) const { return 1.0f; }  // 質量の取得

};

//*****************************************************************************
// 質量ブロック(青)クラス
//*****************************************************************************
class CBlueMassBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	float CarryTargetDis(void) { return 80.0f; }
	btScalar GetMass(void) const { return 2.0f; }  // 質量の取得

};

//*****************************************************************************
// 質量ブロック(黄)クラス
//*****************************************************************************
class CYellowMassBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	float CarryTargetDis(void) { return 80.0f; }
	btScalar GetMass(void) const { return 3.0f; }  // 質量の取得

};

//*****************************************************************************
// 質量ブロック(緑)クラス
//*****************************************************************************
class CGreenMassBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	float CarryTargetDis(void) { return 80.0f; }
	btScalar GetMass(void) const { return 4.0f; }  // 質量の取得

};

#endif
