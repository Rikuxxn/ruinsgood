//=============================================================================
//
// �u���b�N���� [block.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCK_H_
#define _BLOCK_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
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
// �u���b�N�N���X
//*****************************************************************************
class CBlock : public CObjectX
{
public:
	CBlock(int nPriority = 3);
	virtual ~CBlock() = default;

	//*****************************************************************************
	// �u���b�N�̎��
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

	static CBlock* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot,D3DXVECTOR3 size, TYPE type);	// �u���b�N�̐���
	void CreatePhysics(const D3DXVECTOR3& pos, const D3DXVECTOR3& size);										// �R���C�_�[�̐���
	void CreatePhysicsFromScale(const D3DXVECTOR3& scale);														// �u���b�N�X�P�[���ɂ��R���C�_�[�̐���
	static void InitFactory(void);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void UpdateCollider(void);
	void Draw(void);
	void DrawCollider(void);
	void ReleasePhysics(void);															// Physics�j���p
	void CreatePhysicsFromParts(void);

	//*****************************************************************************
	// flagment�֐�
	//*****************************************************************************
	bool IsSelected(void) const { return m_bSelected; }									// �u���b�N���I�𒆂̃t���O��Ԃ�
	bool IsCompoundCollider(void) const;
	virtual bool IsDynamicBlock(void) const { return false; }							// ���I�u���b�N�̔���
	virtual bool IsEnd(void) { return false; }
	virtual bool IsGet(void) { return false; }

	//*****************************************************************************
	// setter�֐�
	//*****************************************************************************
	void SetType(TYPE type) { m_Type = type; }											// �^�C�v�̐ݒ�
	void SetSelected(bool flag) { m_bSelected = flag; }									// �I�𒆂̃t���O��Ԃ�
	void SetColliderSize(const D3DXVECTOR3& size) { m_colliderSize = size; }			// �R���C�_�[�T�C�Y�̐ݒ�
	void SetColliderManual(const D3DXVECTOR3& newSize);									// �R���C�_�[�T�C�Y�̎蓮�ݒ�p
	void SetColliderOffset(const D3DXVECTOR3& offset) { m_colliderOffset = offset; }	// �R���C�_�[�̃I�t�Z�b�g�̐ݒ�
	void SetColliderHandle(const ColliderPart& handle) { m_colliderHandle = handle; }
	void SetColliderBlade(const ColliderPart& blade) { m_colliderBlade = blade; }
	void SetEditMode(bool enable);
	void SetMass(btScalar mass) { m_mass = mass; }

	//*****************************************************************************
	// getter�֐�
	//*****************************************************************************
	virtual D3DXCOLOR GetCol(void) const override;										// �J���[�̎擾
	TYPE GetType(void) const { return m_Type; }											// �^�C�v�̎擾
	btRigidBody* GetRigidBody(void) const { return m_pRigidBody; }						// RigidBody�̎擾
	D3DXVECTOR3 GetColliderSize(void) const { return m_colliderSize; }					// �R���C�_�[�T�C�Y�̎擾
	D3DXVECTOR3 GetColliderOffset(void) const { return m_colliderOffset; }				// �R���C�_�[�̃I�t�Z�b�g�̎擾
	const ColliderPart& GetColliderHandle(void) const { return m_colliderHandle; }
	const ColliderPart& GetColliderBlade(void) const { return m_colliderBlade; }
	bool IsEditMode(void) const { return m_isEditMode; }
	btScalar GetMass(void) const { return m_mass; }  // ���ʂ̎擾
	D3DXMATRIX GetWorldMatrix(void);
	virtual btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	virtual btScalar GetRollingFriction(void) const { return 0.7f; }
	virtual btScalar GetFriction(void) const { return 1.0f; }
	virtual float CarryTargetDis(void) { return 60.0f; }

private:
	char m_szPath[MAX_PATH];		// �t�@�C���p�X
	TYPE m_Type;					// ���
	D3DXCOLOR m_col;				// �A���t�@�l
	D3DXCOLOR m_baseCol;			// �x�[�X�̃A���t�@�l
	bool m_bSelected;				// �I���t���O
	btRigidBody* m_pRigidBody;		// ���̂ւ̃|�C���^
	btCollisionShape* m_pShape;		// �����蔻��̌`�ւ̃|�C���^
	CDebugProc3D* m_pDebug3D;		// 3D�f�o�b�O�\���ւ̃|�C���^
	D3DXVECTOR3 m_prevSize;			// �O��̃T�C�Y
	D3DXVECTOR3 m_colliderSize;		// �R���C�_�[�T�C�Y
	D3DXVECTOR3 m_colliderOffset;	// �R���C�_�[�̈ʒu
	bool m_isEditMode;				// �ҏW�����ǂ���
	ColliderPart m_colliderHandle;  // �_�̕���
	ColliderPart m_colliderBlade;   // �n�̕���
	std::vector<btCollisionShape*> m_childShapes;
	static std::unordered_map<TYPE, BlockCreateFunc> m_BlockFactoryMap;
	btScalar m_mass;				// ����
};

//*****************************************************************************
// �N�����v�֐�
//*****************************************************************************
template<typename T>
inline const T& clamp(const T& v, const T& lo, const T& hi)
{
	return (v < lo) ? lo : (hi < v) ? hi : v;
}
#endif