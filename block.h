//=============================================================================
//
// �u���b�N���� [block.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _BLOCK_H_
#define _BLOCK_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "objectX.h"
#include "debugproc3D.h"

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
	~CBlock();

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
		TYPE_SORD,
		TYPE_MAX
	}TYPE;

	static CBlock* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot,D3DXVECTOR3 size, TYPE type);	// �u���b�N�̐���
	void CreatePhysics(const D3DXVECTOR3& pos, const D3DXVECTOR3& size);										// �R���C�_�[�̐���
	void CreatePhysicsFromScale(const D3DXVECTOR3& scale);														// �u���b�N�X�P�[���ɂ��R���C�_�[�̐���
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
	bool IsStaticBlock(void) const;														// �ÓI�u���b�N�̔���
	bool IsSelected(void) const { return m_bSelected; }									// �u���b�N���I�𒆂̃t���O��Ԃ�
	bool IsCompoundCollider(void) const;

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
	btScalar GetMassByType(TYPE type);
	D3DXMATRIX GetWorldMatrix(void);

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

	void Update(void) override;

private:
	bool m_isDoorOpened;			// �J�������ǂ���
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

	void Update(void) override;

private:
	bool m_isSwitchOn;				// �����ꂽ���ǂ���
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

	void Update(void) override;
	bool IsSwitchOn(void) { return m_isSwitchOn; }

private:
	bool m_isSwitchOn;				// �����ꂽ���ǂ���
	D3DXVECTOR3 m_closedPos;		// �X�C�b�`�̕���Ƃ��̈ʒu

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

private:
	int m_nSwingCounter;		// �t���[���J�E���^�[
	float m_swingAmplitude;		// �}�U��p
	float m_swingPeriod;		// �����t���[��
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
	void IsBridgeMove(bool end) { m_isBridgeMove = end; }

private:
	std::vector<D3DXVECTOR3> m_pathPoints;		// �`�F�b�N�|�C���g�̔z�� (����p)
	int m_currentTargetIndex;					// ���̖ڕW�n�_�C���f�b�N�X
	float m_speed;								// �͂̋����i���x�̑���j
	bool m_isBridgeSwitchOn;
	bool m_isBridgeMove;
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
	bool m_isHit;// �₪�����������ǂ���
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

private:

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
};

//*****************************************************************************
// ���u���b�N�N���X
//*****************************************************************************
class CSordBlock : public CBlock
{
public:
	CSordBlock();
	~CSordBlock();

	void Update(void) override;
	bool IsGet(void) { return m_isGet; }

private:
	bool m_isGet;
};

//=============================================================================
// �N�����v�֐�
//=============================================================================
template <typename T>
T Clamp(const T& value, const T& minVal, const T& maxVal)
{
	if (value < minVal) return minVal;
	if (value > maxVal) return maxVal;
	return value;
}

#endif