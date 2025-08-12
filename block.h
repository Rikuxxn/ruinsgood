//=============================================================================
//
// ブロック処理 [block.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCK_H_
#define _BLOCK_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "objectX.h"
#include "debugproc3D.h"
#include <unordered_map>
#include <functional>

class CBlock;

using BlockCreateFunc = std::function<CBlock* ()>;

struct ColliderPart
{
	D3DXVECTOR3 size;
	D3DXVECTOR3 offset;
};

//*****************************************************************************
// ブロッククラス
//*****************************************************************************
class CBlock : public CObjectX
{
public:
	CBlock(int nPriority = 3);
	virtual ~CBlock() = default;

	//*****************************************************************************
	// ブロックの種類
	//*****************************************************************************
	typedef enum
	{
		TYPE_WOODBOX = 0,
		TYPE_WALL,
		TYPE_WALL2,
		TYPE_WALL3,
		TYPE_WALL4,
		TYPE_AXE,
		TYPE_RAFT,
		TYPE_ROCK,
		TYPE_TORCH,
		TYPE_TORCH2,
		TYPE_FLOOR,
		TYPE_FLOOR2,
		TYPE_DOOR,
		TYPE_CEILING,
		TYPE_CEILING2,
		TYPE_WATER,
		TYPE_SWITCH,
		TYPE_SWITCH_BODY,
		TYPE_BRIDGE,
		TYPE_DOOR_TOP,
		TYPE_DOOR_SIDE,
		TYPE_PILLAR,
		TYPE_BLOCK,
		TYPE_FENCE,
		TYPE_FENCE_PART,
		TYPE_BRIDGE2,
		TYPE_TARGET,
		TYPE_SWITCH2,
		TYPE_DOOR2,
		TYPE_MASK,
		TYPE_SWORD,
		TYPE_SWITCH3,
		TYPE_BAR,
		TYPE_BRIDGE3,
		TYPE_FIRE_STATUE,
		TYPE_WALL5,
		TYPE_FLOOR3,
		TYPE_TURN_FIRE_STATUE,
		TYPE_BLOCK2,
		TYPE_STAIRS,
		TYPE_PILLAR2,
		TYPE_BLOCK3,
		TYPE_FLOOR4,
		TYPE_MOVE_FIRE_STATUE,
		TYPE_TORCH3,
		TYPE_NETFLOOR,
		TYPE_KEYFENCE,
		TYPE_KEY,
		TYPE_KEY_PEDESTAL,
		TYPE_MAX
	}TYPE;

	static CBlock* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot,D3DXVECTOR3 size, TYPE type);	// ブロックの生成
	void CreatePhysics(const D3DXVECTOR3& pos, const D3DXVECTOR3& size);										// コライダーの生成
	void CreatePhysicsFromScale(const D3DXVECTOR3& scale);														// ブロックスケールによるコライダーの生成
	static void InitFactory(void);
	virtual HRESULT Init(void);
	void Uninit(void);
	virtual void Update(void);
	void UpdateCollider(void);
	void Draw(void);
	void ReleasePhysics(void);															// Physics破棄用
	void CreatePhysicsFromParts(void);

	//*****************************************************************************
	// flagment関数
	//*****************************************************************************
	bool IsSelected(void) const { return m_bSelected; }									// ブロックが選択中のフラグを返す
	bool IsCompoundCollider(void) const;
	virtual bool IsDynamicBlock(void) const { return false; }							// 動的ブロックの判別
	virtual bool IsEnd(void) { return false; }
	virtual bool IsGet(void) { return false; }

	//*****************************************************************************
	// setter関数
	//*****************************************************************************
	void SetType(TYPE type) { m_Type = type; }											// タイプの設定
	void SetSelected(bool flag) { m_bSelected = flag; }									// 選択中のフラグを返す
	void SetColliderSize(const D3DXVECTOR3& size);										// コライダーサイズの設定
	void SetColliderManual(const D3DXVECTOR3& newSize);									// コライダーサイズの手動設定用
	void SetColliderOffset(const D3DXVECTOR3& offset);									// コライダーのオフセットの設定
	void SetColliderHandle(const ColliderPart& handle) { m_colliderHandle = handle; }
	void SetColliderBlade(const ColliderPart& blade) { m_colliderBlade = blade; }
	void SetEditMode(bool enable);

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	virtual D3DXCOLOR GetCol(void) const override;										// カラーの取得
	TYPE GetType(void) const { return m_Type; }											// タイプの取得
	static const char* GetTexPathFromType(TYPE type);									// タイプでテクスチャを取得
	btRigidBody* GetRigidBody(void) const { return m_pRigidBody; }						// RigidBodyの取得
	D3DXVECTOR3 GetColliderSize(void) const { return m_colliderSize; }					// コライダーサイズの取得
	D3DXVECTOR3 GetColliderOffset(void) const { return m_colliderOffset; }				// コライダーのオフセットの取得
	const ColliderPart& GetColliderHandle(void) const { return m_colliderHandle; }
	const ColliderPart& GetColliderBlade(void) const { return m_colliderBlade; }
	bool IsEditMode(void) const { return m_isEditMode; }
	virtual btScalar GetMass(void) const { return 2.0f; }  // デフォルト質量
	D3DXMATRIX GetWorldMatrix(void);
	virtual btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	virtual btScalar GetRollingFriction(void) const { return 0.7f; }

private:
	char m_szPath[MAX_PATH];		// ファイルパス
	TYPE m_Type;					// 種類
	D3DXCOLOR m_col;				// アルファ値
	D3DXCOLOR m_baseCol;			// ベースのアルファ値
	bool m_bSelected;				// 選択フラグ
	int m_nIdxTexture;				// テクスチャインデックス
	btRigidBody* m_pRigidBody;		// 剛体へのポインタ
	btCollisionShape* m_pShape;		// 当たり判定の形へのポインタ
	CDebugProc3D* m_pDebug3D;		// 3Dデバッグ表示へのポインタ
	D3DXVECTOR3 m_prevSize;			// 前回のサイズ
	D3DXVECTOR3 m_colliderSize;		// コライダーサイズ
	D3DXVECTOR3 m_colliderOffset;	// コライダーの位置
	bool m_isEditMode;				// 編集中かどうか
	ColliderPart m_colliderHandle;  // 棒の部分
	ColliderPart m_colliderBlade;   // 刃の部分
	std::vector<btCollisionShape*> m_childShapes;
	static std::unordered_map<TYPE, BlockCreateFunc> m_BlockFactoryMap;
	static const std::unordered_map<TYPE, const char*> s_TexturePathMap;
};


//*****************************************************************************
// 木箱ブロッククラス
//*****************************************************************************
class CWoodBoxBlock : public CBlock
{
public:
	btScalar GetMass(void) const override { return 4.0f; }
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
};

//*****************************************************************************
// 柱ブロッククラス
//*****************************************************************************
class CPillarBlock : public CBlock
{
public:
	btScalar GetMass(void) const override { return 55.0f; }
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 1.0f); }
};

//*****************************************************************************
// 木の橋ブロッククラス
//*****************************************************************************
class CWoodBridgeBlock : public CBlock
{
public:
	btScalar GetMass(void) const override { return 8.0f; }
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
};

//*****************************************************************************
// 筏ブロッククラス
//*****************************************************************************
class CRaftBlock : public CBlock
{
public:
	btScalar GetMass(void) const override { return 5.0f; }
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
};

//*****************************************************************************
// 四角い岩ブロッククラス
//*****************************************************************************
class CBoxRockBlock : public CBlock
{
public:
	btScalar GetMass(void) const override { return 8.0f; }
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetRollingFriction(void) const { return 5.7f; }

private:

};

//*****************************************************************************
// 溶岩ブロッククラス
//*****************************************************************************
class CLavaBlock : public CBlock
{
public:
	CLavaBlock();
	~CLavaBlock();

	void Update(void) override;

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
	
	void Update(void) override;
	void SetInWater(bool flag);
	void AddWaterStayTime(void);
	void ResetWaterStayTime(void);

private:
	void ApplyToBlocks(void);   // 他ブロックに浮力
	void ApplyToPlayer(void);   // プレイヤーに浮力

	int m_waterStayTime;		// 水中滞在時間（秒）
	bool m_isInWater;			// 今水中にいるか
	bool m_bWasInWater;			// 水に入ったか
};

//*****************************************************************************
// ドアブロッククラス
//*****************************************************************************
class CDoorBlock : public CBlock
{
public:
	CDoorBlock();
	~CDoorBlock();

	HRESULT Init(void) override;
	void Update(void) override;

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

	void Update(void) override;

private:
	bool m_isDoorOpened;			// 開いたかどうか

};

//*****************************************************************************
// スイッチブロッククラス
//*****************************************************************************
class CSwitchBlock : public CBlock
{
public:
	CSwitchBlock();
	~CSwitchBlock();

	HRESULT Init(void) override;
	void Update(void) override;

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

	HRESULT Init(void) override;
	void Update(void) override;
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

	HRESULT Init(void) override;
	void Update(void) override;
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

	void Update(void) override;
	void Swing(void);
	void IsPlayerHit(void);
	btScalar GetMass(void) const override { return 80.0f; }

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

	void Update(void) override;
	void Respawn(void);
	void AddPathPoint(const D3DXVECTOR3& point);// チェックポイント追加 (通常時用)
	void MoveToTarget(void);					// 転がし処理
	void IsPlayerHit(void);						// プレイヤーとの接触判定
	void UseBridgeSwitch(bool enable) { m_isBridgeSwitchOn = enable; }
	btScalar GetMass(void) const override { return 100.0f; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	bool IsDynamicBlock(void) const override { return true; }

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

	void Update(void) override;
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

	void Update(void) override;
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

	void Update(void) override;

private:
	int m_playedFireSoundID;	// 再生中の音ID
};

//*****************************************************************************
// 置き型松明ブロッククラス
//*****************************************************************************
class CTorch2Block : public CBlock
{
public:
	CTorch2Block();
	~CTorch2Block();

	void Update(void) override;
	btScalar GetMass(void) const override { return 6.0f; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	bool IsDynamicBlock(void) const override { return true; }

private:
	int m_playedFireSoundID;// 再生中の音ID
};

//*****************************************************************************
// 置き型松明(動かない)ブロッククラス
//*****************************************************************************
class CTorch3Block : public CBlock
{
public:
	CTorch3Block();
	~CTorch3Block();

	void Update(void) override;
	void SetHit(bool flag) { m_isHit = flag; }
	bool IsHit(void) { return m_isHit; }

private:
	int m_playedFireSoundID;// 再生中の音ID
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

	void Update(void) override;
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

	void Update(void) override;
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

	void Update(void) override;

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

	void Update(void) override;
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

	void Update(void) override;
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

	void Update(void) override;
	void SetParticle(void);

	btScalar GetMass(void) const override { return 7.0f; }
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetRollingFriction(void) const { return 5.7f; }

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

	void Update(void) override;
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
	void Update(void) override;

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
	void Update(void) override;
	void Respawn(void);

	btScalar GetMass(void) const override { return 6.0f; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	bool IsDynamicBlock(void) const override { return true; }

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
	void Update(void) override;

private:
	D3DXVECTOR3 m_Pos;

};

//*****************************************************************************
// クランプ関数
//*****************************************************************************
template<typename T>
inline const T& clamp(const T& v, const T& lo, const T& hi)
{
	return (v < lo) ? lo : (hi < v) ? hi : v;
}
#endif