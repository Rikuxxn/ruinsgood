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
		TYPE_KEY_DOOR,
		TYPE_SHIELD,
		TYPE_STATUE,
		TYPE_STATUE2,
		TYPE_EGG,
		TYPE_DOOR_TRIGGER,
		TYPE_MASSBLOCK_RED,
		TYPE_MASSBLOCK_BLUE,
		TYPE_MASSBLOCK_YELLOW,
		TYPE_MASSBLOCK_GREEN,
		TYPE_MAX
	}TYPE;

	static CBlock* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot,D3DXVECTOR3 size, TYPE type);	// ブロックの生成
	void CreatePhysics(const D3DXVECTOR3& pos, const D3DXVECTOR3& size);										// コライダーの生成
	void CreatePhysicsFromScale(const D3DXVECTOR3& scale);														// ブロックスケールによるコライダーの生成
	static void InitFactory(void);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void UpdateCollider(void);
	void Draw(void);
	void DrawCollider(void);
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
	void SetColliderSize(const D3DXVECTOR3& size) { m_colliderSize = size; }			// コライダーサイズの設定
	void SetColliderManual(const D3DXVECTOR3& newSize);									// コライダーサイズの手動設定用
	void SetColliderOffset(const D3DXVECTOR3& offset) { m_colliderOffset = offset; }	// コライダーのオフセットの設定
	void SetColliderHandle(const ColliderPart& handle) { m_colliderHandle = handle; }
	void SetColliderBlade(const ColliderPart& blade) { m_colliderBlade = blade; }
	void SetEditMode(bool enable);
	void SetMass(btScalar mass) { m_mass = mass; }

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	virtual D3DXCOLOR GetCol(void) const override;										// カラーの取得
	TYPE GetType(void) const { return m_Type; }											// タイプの取得
	btRigidBody* GetRigidBody(void) const { return m_pRigidBody; }						// RigidBodyの取得
	D3DXVECTOR3 GetColliderSize(void) const { return m_colliderSize; }					// コライダーサイズの取得
	D3DXVECTOR3 GetColliderOffset(void) const { return m_colliderOffset; }				// コライダーのオフセットの取得
	const ColliderPart& GetColliderHandle(void) const { return m_colliderHandle; }
	const ColliderPart& GetColliderBlade(void) const { return m_colliderBlade; }
	bool IsEditMode(void) const { return m_isEditMode; }
	btScalar GetMass(void) const { return m_mass; }  // 質量の取得
	D3DXMATRIX GetWorldMatrix(void);
	virtual btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	virtual btScalar GetRollingFriction(void) const { return 0.7f; }
	virtual btScalar GetFriction(void) const { return 1.0f; }
	virtual float CarryTargetDis(void) { return 60.0f; }

private:
	char m_szPath[MAX_PATH];		// ファイルパス
	TYPE m_Type;					// 種類
	D3DXCOLOR m_col;				// アルファ値
	D3DXCOLOR m_baseCol;			// ベースのアルファ値
	bool m_bSelected;				// 選択フラグ
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
	btScalar m_mass;				// 質量
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