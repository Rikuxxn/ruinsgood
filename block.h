//=============================================================================
//
// ブロック処理 [block.h]
// Author : TANEKAWA RIKU
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
	CBlock();
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
		TYPE_MAX
	}TYPE;

	static CBlock* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot,D3DXVECTOR3 size, TYPE type);	// ブロックの生成
	static CBlock* CreateFromType(TYPE type, D3DXVECTOR3 pos);													// タイプ指定でブロックの生成
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

private:
	void ApplyToBlocks(void);   // 他ブロックに浮力
	void ApplyToPlayer(void);   // プレイヤーに浮力

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
	D3DXVECTOR3 m_lerpStartPos;
	D3DXVECTOR3 m_lerpTargetPos;
	float m_lerpTimer;
	float m_lerpDuration;
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
	D3DXVECTOR3 GetClosePos(void) { return m_closedPos; }

private:
	bool m_isSwitchOn;				// 押されたかどうか
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

private:

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
	void AddPathPoint(const D3DXVECTOR3& point);// チェックポイント追加

private:
	std::vector<D3DXVECTOR3> m_pathPoints;  // チェックポイントの配列
	int m_currentTargetIndex;               // 今の目標地点インデックス
	float m_speed;							// 力の強さ（速度の代わり）

	void MoveToTarget(void);				// 転がし処理
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