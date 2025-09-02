//=============================================================================
//
// �u���b�N���X�g���� [blocklist.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCKLIST_H_
#define _BLOCKLIST_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "block.h"

//*****************************************************************************
// �ؔ��u���b�N�N���X
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
	btScalar GetMass(void) const { return 4.0f; }  // ���ʂ̎擾

private:
	D3DXVECTOR3 m_ResPos;	// ���X�|�[���ʒu
};

//*****************************************************************************
// ���u���b�N�N���X
//*****************************************************************************
class CPillarBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 1.0f); }
	btScalar GetMass(void) const { return 55.0f; }  // ���ʂ̎擾
};

//*****************************************************************************
// �؂̋��u���b�N�N���X
//*****************************************************************************
class CWoodBridgeBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	btScalar GetMass(void) const { return 8.0f; }  // ���ʂ̎擾
};

//*****************************************************************************
// ���u���b�N�N���X
//*****************************************************************************
class CRaftBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	float CarryTargetDis(void) { return 80.0f; }
	btScalar GetMass(void) const { return 7.5f; }  // ���ʂ̎擾
};

//*****************************************************************************
// �l�p����u���b�N�N���X
//*****************************************************************************
class CBoxRockBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	float CarryTargetDis(void) { return 80.0f; }
	btScalar GetMass(void) const { return 7.0f; }  // ���ʂ̎擾

};

//*****************************************************************************
// �n��u���b�N�N���X
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
// ���u���b�N�N���X
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
	void ApplyToBlocks(void);   // ���u���b�N�ɕ���
	void ApplyToPlayer(void);   // �v���C���[�ɕ���

	int m_waterStayTime;		// �����؍ݎ��ԁi�b�j
	bool m_isInWater;			// �������ɂ��邩
	bool m_bWasInWater;			// ���ɓ�������
	bool m_isRespawn;			// ���X�|�[����Ԃ�
};

//*****************************************************************************
// �h�A�u���b�N�N���X
//*****************************************************************************
class CDoorBlock : public CBlock
{
public:
	CDoorBlock();
	~CDoorBlock();

	HRESULT Init(void);
	void Update(void);

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

	HRESULT Init(void);
	void Update(void);

private:
	bool m_isDoorOpened;			// �J�������ǂ���
	D3DXVECTOR3 m_openPos;
};

//*****************************************************************************
// �X�C�b�`�u���b�N�N���X
//*****************************************************************************
class CSwitchBlock : public CBlock
{
public:
	CSwitchBlock();
	~CSwitchBlock();

	HRESULT Init(void);
	void Update(void);

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

	HRESULT Init(void);
	void Update(void);
	float CalcStackMass(CBlock* base);
	bool IsOnTop(CBlock* base, CBlock* other);
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

	HRESULT Init(void);
	void Update(void);
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

	void Update(void);
	void Swing(void);
	void IsPlayerHit(void);
	btScalar GetMass(void) const { return 80.0f; }  // ���ʂ̎擾

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

	void Update(void);
	void Respawn(void);
	void AddPathPoint(const D3DXVECTOR3& point);// �`�F�b�N�|�C���g�ǉ� (�ʏ펞�p)
	void MoveToTarget(void);					// �]��������
	void IsPlayerHit(void);						// �v���C���[�Ƃ̐ڐG����
	void UseBridgeSwitch(bool enable) { m_isBridgeSwitchOn = enable; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	bool IsDynamicBlock(void) const override { return true; }
	btScalar GetMass(void) const { return 100.0f; }  // ���ʂ̎擾

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

	void Update(void);
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

	void Update(void);
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

	void Update(void);

private:
	int m_playedSoundID;	// �Đ����̉�ID
};

//*****************************************************************************
// �u���^�����u���b�N�N���X
//*****************************************************************************
class CTorch2Block : public CBlock
{
public:
	CTorch2Block();
	~CTorch2Block();

	void Update(void);
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	bool IsDynamicBlock(void) const override { return true; }
	btScalar GetMass(void) const { return 7.0f; }  // ���ʂ̎擾

private:
	int m_playedSoundID;// �Đ����̉�ID
};

//*****************************************************************************
// �u���^����(�����Ȃ�)�u���b�N�N���X
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
	int m_playedSoundID;// �Đ����̉�ID
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

	void Update(void);
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

	void Update(void);
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

	void Update(void);

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

	void Update(void);
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

	void Update(void);
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

	void Update(void);
	void SetParticle(void);

	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	btScalar GetFriction(void) const { return 2.0f; }
	float CarryTargetDis(void) { return 100.0f; }
	btScalar GetMass(void) const { return 7.0f; }  // ���ʂ̎擾

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

	void Update(void);
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
	void Update(void);

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
	void Update(void);
	void Respawn(void);
	void Set(D3DXVECTOR3 pos);

	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	bool IsDynamicBlock(void) const override { return true; }
	btScalar GetMass(void) const { return 6.0f; }  // ���ʂ̎擾

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
	void Update(void);
	bool IsSet(void) { return m_isSet; }

private:
	D3DXVECTOR3 m_Pos;
	bool m_isSet;// ����ɐݒu������
	bool m_prevIsSet;// ���O�ɐݒu������
};

//*****************************************************************************
// ���h�A�u���b�N�N���X
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
	bool m_isGoal;	// �ڕW�ʒu�Ɉړ�������
	bool m_isSet;	// �ݒu���ꂽ��
	int m_deleyTime;// �J���܂ł̒x������
	bool prevIsSet;	// ���O�ɐݒu������
};

//*****************************************************************************
// ���u���b�N�N���X
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
	int m_playedSoundID;					// �Đ����̉�ID

};

//*****************************************************************************
// �Α��u���b�N�N���X
//*****************************************************************************
class CStatueBlock : public CBlock
{
public:
	CStatueBlock();
	~CStatueBlock();

	void Update(void);

private:
	int m_playedSoundID;					// �Đ����̉�ID
};

//*****************************************************************************
// �΂�����Ɠ����Α��u���b�N�N���X
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
	int m_playedSoundID;					// �Đ����̉�ID
	bool m_triggerDis;						// ��������苗���ɓ�������
	bool m_isMoving;
	bool m_hasTriggered;
};

//*****************************************************************************
// ���u���b�N�N���X
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
	int m_playedSoundID;					// �Đ����̉�ID
};

//*****************************************************************************
// �h�A�g���K�[�u���b�N�N���X
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
	bool m_isSet;		// �ݒu������
	bool m_prevIsSet;	// ���O�ɐݒu������
	bool m_isSetCam;	// �J������ݒ肵����
	int m_nDeleyTime;	// �J�����ڍs�܂ł̒x������
};

//*****************************************************************************
// ���ʃu���b�N(��)�N���X
//*****************************************************************************
class CRedMassBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	float CarryTargetDis(void) { return 80.0f; }
	btScalar GetMass(void) const { return 1.0f; }  // ���ʂ̎擾

};

//*****************************************************************************
// ���ʃu���b�N(��)�N���X
//*****************************************************************************
class CBlueMassBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	float CarryTargetDis(void) { return 80.0f; }
	btScalar GetMass(void) const { return 2.0f; }  // ���ʂ̎擾

};

//*****************************************************************************
// ���ʃu���b�N(��)�N���X
//*****************************************************************************
class CYellowMassBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	float CarryTargetDis(void) { return 80.0f; }
	btScalar GetMass(void) const { return 3.0f; }  // ���ʂ̎擾

};

//*****************************************************************************
// ���ʃu���b�N(��)�N���X
//*****************************************************************************
class CGreenMassBlock : public CBlock
{
public:
	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	float CarryTargetDis(void) { return 80.0f; }
	btScalar GetMass(void) const { return 4.0f; }  // ���ʂ̎擾

};

#endif
