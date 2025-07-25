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
	~CBlock();

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
		TYPE_MAX
	}TYPE;

	static CBlock* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot,D3DXVECTOR3 size, TYPE type);	// ブロックの生成
	void CreatePhysics(const D3DXVECTOR3& pos, const D3DXVECTOR3& size);										// コライダーの生成
	void CreatePhysicsFromScale(const D3DXVECTOR3& scale);														// ブロックスケールによるコライダーの生成
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
	bool IsStaticBlock(void) const;														// 静的ブロックの判別
	bool IsSelected(void) const { return m_bSelected; }									// ブロックが選択中のフラグを返す
	bool IsCompoundCollider(void) const;
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
	btScalar GetMassByType(TYPE type);
	D3DXMATRIX GetWorldMatrix(void);


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

	void Update(void) override;

private:
	bool m_isDoorOpened;			// 開いたかどうか
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

	void Update(void) override;
	bool IsSwitchOn(void) { return m_isSwitchOn; }

private:
	bool m_isSwitchOn;				// 押されたかどうか
	bool m_prevSwitchOn;			// 直前のスイッチ状態
	D3DXVECTOR3 m_closedPos;		// スイッチの閉じるときの位置
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

private:
	int m_nSwingCounter;		// フレームカウンター
	float m_swingAmplitude;		// ±振れ角
	float m_swingPeriod;		// 周期フレーム
	bool m_isPrevHit;
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
	const int DASH_PARTICLE_INTERVAL = 8;		// パーティクル発生間隔（フレーム数）
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
	bool m_isHit;// 岩が当たったかどうか
	bool m_isPrevHit;// 直前に当たったか
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

private:
	int m_playedFireSoundID;// 再生中の音ID
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
};

//=============================================================================
// クランプ関数
//=============================================================================
template <typename T>
T Clamp(const T& value, const T& minVal, const T& maxVal)
{
	if (value < minVal) return minVal;
	if (value > maxVal) return maxVal;
	return value;
}

#endif