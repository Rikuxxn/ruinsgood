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
		TYPE_MAX
	}TYPE;

	static CBlock* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot,D3DXVECTOR3 size, TYPE type);	// �u���b�N�̐���
	void CreatePhysics(const D3DXVECTOR3& pos, const D3DXVECTOR3& size);										// �R���C�_�[�̐���
	void CreatePhysicsFromScale(const D3DXVECTOR3& scale);														// �u���b�N�X�P�[���ɂ��R���C�_�[�̐���
	static void InitFactory(void);
	virtual HRESULT Init(void);
	void Uninit(void);
	virtual void Update(void);
	void UpdateCollider(void);
	void Draw(void);
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
	void SetColliderSize(const D3DXVECTOR3& size);										// �R���C�_�[�T�C�Y�̐ݒ�
	void SetColliderManual(const D3DXVECTOR3& newSize);									// �R���C�_�[�T�C�Y�̎蓮�ݒ�p
	void SetColliderOffset(const D3DXVECTOR3& offset);									// �R���C�_�[�̃I�t�Z�b�g�̐ݒ�
	void SetColliderHandle(const ColliderPart& handle) { m_colliderHandle = handle; }
	void SetColliderBlade(const ColliderPart& blade) { m_colliderBlade = blade; }
	void SetEditMode(bool enable);

	//*****************************************************************************
	// getter�֐�
	//*****************************************************************************
	virtual D3DXCOLOR GetCol(void) const override;										// �J���[�̎擾
	TYPE GetType(void) const { return m_Type; }											// �^�C�v�̎擾
	static const char* GetTexPathFromType(TYPE type);									// �^�C�v�Ńe�N�X�`�����擾
	btRigidBody* GetRigidBody(void) const { return m_pRigidBody; }						// RigidBody�̎擾
	D3DXVECTOR3 GetColliderSize(void) const { return m_colliderSize; }					// �R���C�_�[�T�C�Y�̎擾
	D3DXVECTOR3 GetColliderOffset(void) const { return m_colliderOffset; }				// �R���C�_�[�̃I�t�Z�b�g�̎擾
	const ColliderPart& GetColliderHandle(void) const { return m_colliderHandle; }
	const ColliderPart& GetColliderBlade(void) const { return m_colliderBlade; }
	bool IsEditMode(void) const { return m_isEditMode; }
	virtual btScalar GetMass(void) const { return 2.0f; }  // �f�t�H���g����
	D3DXMATRIX GetWorldMatrix(void);
	virtual btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	virtual btScalar GetRollingFriction(void) const { return 0.7f; }

private:
	char m_szPath[MAX_PATH];		// �t�@�C���p�X
	TYPE m_Type;					// ���
	D3DXCOLOR m_col;				// �A���t�@�l
	D3DXCOLOR m_baseCol;			// �x�[�X�̃A���t�@�l
	bool m_bSelected;				// �I���t���O
	int m_nIdxTexture;				// �e�N�X�`���C���f�b�N�X
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
	static const std::unordered_map<TYPE, const char*> s_TexturePathMap;
};


//*****************************************************************************
// �ؔ��u���b�N�N���X
//*****************************************************************************
class CWoodBoxBlock : public CBlock
{
public:
	btScalar GetMass(void) const override { return 4.0f; }
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
};

//*****************************************************************************
// ���u���b�N�N���X
//*****************************************************************************
class CPillarBlock : public CBlock
{
public:
	btScalar GetMass(void) const override { return 55.0f; }
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 1.0f); }
};

//*****************************************************************************
// �؂̋��u���b�N�N���X
//*****************************************************************************
class CWoodBridgeBlock : public CBlock
{
public:
	btScalar GetMass(void) const override { return 8.0f; }
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
};

//*****************************************************************************
// ���u���b�N�N���X
//*****************************************************************************
class CRaftBlock : public CBlock
{
public:
	btScalar GetMass(void) const override { return 5.0f; }
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
};

//*****************************************************************************
// �l�p����u���b�N�N���X
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
// �n��u���b�N�N���X
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
// ���u���b�N�N���X
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
	void ApplyToBlocks(void);   // ���u���b�N�ɕ���
	void ApplyToPlayer(void);   // �v���C���[�ɕ���

	int m_waterStayTime;		// �����؍ݎ��ԁi�b�j
	bool m_isInWater;			// �������ɂ��邩
	bool m_bWasInWater;			// ���ɓ�������
};

//*****************************************************************************
// �h�A�u���b�N�N���X
//*****************************************************************************
class CDoorBlock : public CBlock
{
public:
	CDoorBlock();
	~CDoorBlock();

	HRESULT Init(void) override;
	void Update(void) override;

private:
	bool m_isDoorOpened;			// �J�������ǂ���
	D3DXVECTOR3 m_initialPos;		// �����ʒu
};

//*****************************************************************************
// �ŏI�G���A�h�A�u���b�N�N���X
//*****************************************************************************
class CBigDoorBlock : public CBlock
{
public:
	CBigDoorBlock();
	~CBigDoorBlock();

	void Update(void) override;

private:
	bool m_isDoorOpened;			// �J�������ǂ���

};

//*****************************************************************************
// �X�C�b�`�u���b�N�N���X
//*****************************************************************************
class CSwitchBlock : public CBlock
{
public:
	CSwitchBlock();
	~CSwitchBlock();

	HRESULT Init(void) override;
	void Update(void) override;

private:
	bool m_isSwitchOn;				// �����ꂽ���ǂ���
	bool m_prevSwitchOn;			// ���O�̃X�C�b�`���
	D3DXVECTOR3 m_closedPos;		// �X�C�b�`�̕���Ƃ��̈ʒu
};

//*****************************************************************************
// ������X�C�b�`�u���b�N�N���X
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
	bool m_isSwitchOn;				// �����ꂽ���ǂ���
	bool m_prevSwitchOn;			// ���O�̃X�C�b�`���
	D3DXVECTOR3 m_closedPos;		// �X�C�b�`�̕���Ƃ��̈ʒu
};

//*****************************************************************************
// �i�q�Ƒ��ꐧ��X�C�b�`�u���b�N�N���X
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
	bool m_isSwitchOn;				// �����ꂽ���ǂ���
	bool m_prevSwitchOn;			// ���O�̃X�C�b�`���
	D3DXVECTOR3 m_closedPos;		// �X�C�b�`�̕���Ƃ��̈ʒu
	int m_timerCnt;
	int m_Timer;
};

//*****************************************************************************
// ���u���b�N�N���X
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
	int m_nSwingCounter;		// �t���[���J�E���^�[
	float m_swingAmplitude;		// �}�U��p
	float m_swingPeriod;		// �����t���[��
	bool m_isPrevHit;
	float m_prevSwingAngle;		// �O��̃X�C���O�p�x
	bool m_wasPositive;			// �O��̊p�x�������ǂ���
	int m_playedSoundID;		// �Đ����̉�ID
};

//*****************************************************************************
// ��u���b�N�N���X
//*****************************************************************************
class CRockBlock : public CBlock
{
public:
	CRockBlock();
	~CRockBlock();

	void Update(void) override;
	void Respawn(void);
	void AddPathPoint(const D3DXVECTOR3& point);// �`�F�b�N�|�C���g�ǉ� (�ʏ펞�p)
	void MoveToTarget(void);					// �]��������
	void IsPlayerHit(void);						// �v���C���[�Ƃ̐ڐG����
	void UseBridgeSwitch(bool enable) { m_isBridgeSwitchOn = enable; }
	btScalar GetMass(void) const override { return 100.0f; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	bool IsDynamicBlock(void) const override { return true; }

private:
	std::vector<D3DXVECTOR3> m_pathPoints;		// �`�F�b�N�|�C���g�̔z�� (����p)
	int m_currentTargetIndex;					// ���̖ڕW�n�_�C���f�b�N�X
	float m_speed;								// �͂̋����i���x�̑���j
	bool m_isBridgeSwitchOn;
	bool m_isHit;
	bool m_isPrevHit;
	bool m_isThrough;							// �ʉ߂�����
	bool m_isPrevThrough;						// ���O�ɒʉ߂�����
	int m_particleTimer;						// �^�C�}�[
	const int ROLL_PARTICLE_INTERVAL = 8;		// �p�[�e�B�N�������Ԋu�i�t���[�����j
	int m_playedRollSoundID;					// �Đ����̉�ID
};

//*****************************************************************************
// ���u���b�N�N���X
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
// �^�[�Q�b�g�u���b�N�N���X
//*****************************************************************************
class CTargetBlock : public CBlock
{
public:
	CTargetBlock();
	~CTargetBlock();

	void Update(void) override;
	bool IsHit(void) { return m_isHit; }

private:
	bool m_isHit;		// �₪�����������ǂ���
	bool m_isPrevHit;	// ���O�ɓ���������
};

//*****************************************************************************
// �Ǌ|�������u���b�N�N���X
//*****************************************************************************
class CTorchBlock : public CBlock
{
public:
	CTorchBlock();
	~CTorchBlock();

	void Update(void) override;

private:
	int m_playedFireSoundID;	// �Đ����̉�ID
};

//*****************************************************************************
// �u���^�����u���b�N�N���X
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
	int m_playedFireSoundID;// �Đ����̉�ID
};

//*****************************************************************************
// �u���^����(�����Ȃ�)�u���b�N�N���X
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
	int m_playedFireSoundID;// �Đ����̉�ID
	bool m_isHit;
};

//*****************************************************************************
// ���ʃu���b�N�N���X
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
	int m_playedSoundID;					// �Đ����̉�ID
};

//*****************************************************************************
// ���u���b�N�N���X
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
	int m_playedSoundID;					// �Đ����̉�ID
};

//*****************************************************************************
// �S�i�q�u���b�N�N���X
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
// ���ꋴ�u���b�N�N���X
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
// �Ή����ˑ��u���b�N�N���X
//*****************************************************************************
class CFireStatueBlock : public CBlock
{
public:
	CFireStatueBlock();
	~CFireStatueBlock();

	void Update(void) override;
	void SetParticle(void);

private:
	int m_playedSoundID;					// �Đ����̉�ID
	bool m_isBlocked;
};

//*****************************************************************************
// �Ή����ˑ�(����)�u���b�N�N���X
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
	int m_playedSoundID;					// �Đ����̉�ID
};

//*****************************************************************************
// �Ή����ˑ�(��])�u���b�N�N���X
//*****************************************************************************
class CTurnFireStatueBlock : public CBlock
{
public:
	CTurnFireStatueBlock();
	~CTurnFireStatueBlock();

	void Update(void) override;
	void SetParticle(void);

private:
	int m_playedSoundID;					// �Đ����̉�ID
};

//*****************************************************************************
// ���̍�u���b�N�N���X
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
// ���u���b�N�N���X
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
// ���̑���u���b�N�N���X
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
// �N�����v�֐�
//*****************************************************************************
template<typename T>
inline const T& clamp(const T& v, const T& lo, const T& hi)
{
	return (v < lo) ? lo : (hi < v) ? hi : v;
}
#endif