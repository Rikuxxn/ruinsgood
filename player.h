//=============================================================================
//
// �v���C���[���� [player.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PLAYER_H_// ���̃}�N����`������Ă��Ȃ�������
#define _PLAYER_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "model.h"
#include "motion.h"
#include "imguimaneger.h"
#include "debugproc3D.h"
#include "block.h"
#include "shadowS.h"
#include "effect.h"
#include "state.h"

class CPlayer_StandState;
class CPlayer_MoveState;
class CPlayer_BesideMoveState;
class CPlayer_JumpState;

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_PARTS		(32)					// �ő�p�[�c��
#define PLAYER_SPEED	(200.0f)				// �ړ��X�s�[�h
#define SIDE_MOVE_SPEED (PLAYER_SPEED * 0.6f)	// ���ړ���60%�̃X�s�[�h
#define MAX_JUMP_POWER	(330.3f)				// �W�����v����
#define MAX_GRAVITY		(-0.26f)				// �d�͉����x

struct InputData
{
	D3DXVECTOR3 moveDir;      // �O��ړ��x�N�g��
	D3DXVECTOR3 sideMoveDir;  // ���ړ��x�N�g��
	bool jump;                // �W�����v����
};

//*****************************************************************************
// �v���C���[�N���X
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

	//*****************************************************************************
	// flagment�֐�
	//*****************************************************************************
	bool OnGround(btDiscreteDynamicsWorld* world, btRigidBody* playerBody, float rayLength);

	//*****************************************************************************
	// setter�֐�
	//*****************************************************************************
	void SetMove(D3DXVECTOR3 move) { m_move = move; }
	void SetIsJumping(bool flag) { m_isJumping = flag; }
	void SetMotion(CMotion::TYPE type, int nBlendFrame);

	//*****************************************************************************
	// getter�֐�
	//*****************************************************************************
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXVECTOR3 GetRot(void)const { return m_rot; };
	D3DXVECTOR3 GetMove(void) const { return m_move; }
	D3DXVECTOR3 GetColliderPos(void) const { return m_colliderPos; }
	btScalar GetRadius(void) const { return m_radius; }
	btScalar GetHeight(void) const { return m_height; }
	btRigidBody* GetRigidBody(void) const { return m_pRigidBody; }						// RigidBody�̎擾
	bool GetPlayerUse(void) const { return m_playerUse; }
	bool GetIsMoving(void) const { return m_bIsMoving; }
	bool GetIsSideMoving(void) const { return m_bIsSideMoving; }
	bool GetOnGround(void) { return m_bOnGround; }
	bool GetIsJumping(void) { return m_isJumping; }
	D3DXVECTOR3 GetForward(void) const;
	InputData GatherInput(void);
	D3DXVECTOR3 GetNearestRespawnPoint(void) const;
	CBlock* FindFrontBlockByRaycast(float rayLength);

	void ReleasePhysics(void);														// Physics�j���p
	void HoldBlock(void);
	void RespawnToCheckpoint(void);
	void RespawnToCheckpoint(D3DXVECTOR3 pos);
	void AddRespawnPoint(const D3DXVECTOR3& pos);
	D3DXVECTOR3 Lerp(const D3DXVECTOR3& a, const D3DXVECTOR3& b, float t);

	// �X�e�[�g�p�Ƀt���O�X�V
	void UpdateMovementFlags(const D3DXVECTOR3& moveDir, const D3DXVECTOR3& sideDir)
	{
		m_bIsMoving = (moveDir.x != 0.0f || moveDir.z != 0.0f);
		m_bIsSideMoving = (sideDir.x != 0.0f || sideDir.z != 0.0f);
	}

private:
	D3DXVECTOR3 m_pos;					// �ʒu
	D3DXVECTOR3 m_rot;					// ����
	D3DXVECTOR3 m_rotDest;				// ����
	D3DXVECTOR3 m_move;					// �ړ���
	D3DXVECTOR3 m_targetMove;			// �ڕW���x
	D3DXVECTOR3 m_currentMove;			// ���ۂ̈ړ����x
	D3DXVECTOR3 m_size;					// �T�C�Y
	D3DXVECTOR3 m_colliderPos;			// �J�v�Z���R���C�_�[�̈ʒu
	D3DXMATRIX m_mtxWorld;				// ���[���h�}�g���b�N�X
	CModel* m_apModel[MAX_PARTS];		// ���f��(�p�[�c)�ւ̃|�C���^
	CShadowS* m_pShadowS;				// �X�e���V���V���h�E�ւ̃|�C���^
	CMotion* m_pMotion;					// ���[�V�����ւ̃|�C���^
	CMotion::TYPE m_currentMotion;		// ���݂̃��[�V����
	btRigidBody* m_pRigidBody;			// ���̂ւ̃|�C���^
	btCollisionShape* m_pShape;			// �����蔻��̌`�ւ̃|�C���^
	CDebugProc3D* m_pDebug3D;			// 3D�f�o�b�O�\���ւ̃|�C���^
	btScalar m_radius;					// �J�v�Z���R���C�_�[�̔��a
	btScalar m_height;					// �J�v�Z���R���C�_�[�̍���
	CBlock* m_pCarryingBlock;			// �^��ł���u���b�N
	int m_nNumModel;					// ���f��(�p�[�c)�̑���
	int m_jumpFrame;					// �W�����v���Ă��牽�t���[���o�߂�����
	bool m_playerUse;					// �g���Ă��邩�ǂ���
	bool m_isJumping;					// �W�����v���t���O
	bool m_bIsMoving;					// �ړ����̓t���O
	bool m_bIsSideMoving;				// ���ړ����̓t���O
	bool m_bOnGround;					// �ڒn�t���O
	std::vector<D3DXVECTOR3> m_ResPos;	// ���X�|�[���n�_
	int m_particleTimer;				// �^�C�}�[
	const int DASH_PARTICLE_INTERVAL = 5; // �p�[�e�B�N�������Ԋu�i�t���[�����j

	// �X�e�[�g���Ǘ�����N���X�̃C���X�^���X
	StateMachine<CPlayer> m_stateMachine;
};


//*****************************************************************************
// �v���C���[�̑ҋ@���
//*****************************************************************************
class CPlayer_StandState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{
		// �ҋ@���[�V����
		pPlayer->SetMotion(CMotion::TYPE_NEUTRAL, 10);
	}

	void OnUpdate(CPlayer* pPlayer)override
	{

		// ���͂��擾
		InputData input = pPlayer->GatherInput();

		// �W�����v���͂�����΃W�����v�X�e�[�g�Ɉڍs
		if (input.jump && pPlayer->GetOnGround() && !pPlayer->GetIsJumping())
		{
			m_pMachine->ChangeState<CPlayer_JumpState>();
			return;
		}

		// �O���ړ����͂�����Βʏ�ړ��X�e�[�g�Ɉڍs
		if (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f)
		{
			m_pMachine->ChangeState<CPlayer_MoveState>();
		}

		// ���ړ����͂�����Ή��ړ��X�e�[�g�Ɉڍs
		if (input.sideMoveDir.x != 0.0f || input.sideMoveDir.z != 0.0f)
		{
			m_pMachine->ChangeState<CPlayer_BesideMoveState>();
		}

		D3DXVECTOR3 move = pPlayer->GetMove();
		move *= 0.95f; // ������
		if (fabsf(move.x) < 0.01f) move.x = 0;
		if (fabsf(move.z) < 0.01f) move.z = 0;

		// �ړ��ʂ�ݒ�
		pPlayer->SetMove(move);

		// ���W�b�h�{�f�B�ɔ��f
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);
	}

	void OnExit(CPlayer* pPlayer)override
	{

	}

private:

};

//*****************************************************************************
// �v���C���[�̈ړ����
//*****************************************************************************
class CPlayer_MoveState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{
		// �ړ����[�V����
		pPlayer->SetMotion(CMotion::TYPE_MOVE, 10);
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		// ���͎擾
		InputData input = pPlayer->GatherInput();

		// �t���O�X�V
		pPlayer->UpdateMovementFlags(input.moveDir, input.sideMoveDir);

		// �ڕW���x�v�Z
		float moveSpeed = PLAYER_SPEED;
		if (pPlayer->GetIsSideMoving() && !pPlayer->GetIsMoving())
		{
			moveSpeed = SIDE_MOVE_SPEED;
		}

		D3DXVECTOR3 targetMove = input.moveDir + input.sideMoveDir;

		if (targetMove.x != 0.0f || targetMove.z != 0.0f)
		{
			D3DXVec3Normalize(&targetMove, &targetMove);
			float moveSpeed = PLAYER_SPEED;
			if (pPlayer->GetIsSideMoving() && !pPlayer->GetIsMoving())
			{
				moveSpeed = SIDE_MOVE_SPEED;
			}

			targetMove *= moveSpeed;
		}
		else
		{
			targetMove = D3DXVECTOR3(0, 0, 0);
		}

		// ���ݑ��x�Ƃ̕�ԁi�C�[�W���O�j
		const float accelRate = 0.15f;
		D3DXVECTOR3 currentMove = pPlayer->GetMove();

		currentMove.x += (targetMove.x - currentMove.x) * accelRate;
		currentMove.z += (targetMove.z - currentMove.z) * accelRate;

		// ��Ԍ�̑��x���v���C���[�ɃZ�b�g
		pPlayer->SetMove(currentMove);

		// �����G���W���ɃZ�b�g
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(currentMove.x);
		velocity.setZ(currentMove.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);

		// �W�����v���͂�����΃W�����v�X�e�[�g�ɐؑ�
		if (input.jump && pPlayer->GetOnGround() && !pPlayer->GetIsJumping())
		{
			// �W�����v���
			m_pMachine->ChangeState<CPlayer_JumpState>();
		}

		// ���ړ����͂�����Ή��ړ��X�e�[�g�ɐؑ�
		if (pPlayer->GetIsSideMoving() && !pPlayer->GetIsMoving())
		{
			m_pMachine->ChangeState<CPlayer_BesideMoveState>();
		}

		// �ړ����Ă��Ȃ���Αҋ@�X�e�[�g�ɖ߂�
		if (!pPlayer->GetIsMoving() && !pPlayer->GetIsSideMoving() && !pPlayer->GetIsJumping())
		{
			// �ҋ@���
			m_pMachine->ChangeState<CPlayer_StandState>();
		}
	}

	void OnExit(CPlayer* pPlayer)override
	{

	}

private:

};

//*****************************************************************************
// �v���C���[�̉��ړ����
//*****************************************************************************
class CPlayer_BesideMoveState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{
		// ���ړ����[�V����
		pPlayer->SetMotion(CMotion::TYPE_BESIDEMOVE, 10);
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		// ���͂ɉ����ăt���O���X�V
		InputData input = pPlayer->GatherInput(); // ���͂��擾����֐�

		pPlayer->UpdateMovementFlags(input.moveDir, input.sideMoveDir);

		// ���ړ��݂̂̑��x�v�Z
		float moveSpeed = SIDE_MOVE_SPEED;

		D3DXVECTOR3 targetMove = input.sideMoveDir * moveSpeed;

		const float accelRate = 0.15f; // �������
		D3DXVECTOR3 currentMove = pPlayer->GetMove();

		currentMove.x += (targetMove.x - currentMove.x) * accelRate;
		currentMove.z += (targetMove.z - currentMove.z) * accelRate;

		// ��Ԍ�̑��x���v���C���[�ɃZ�b�g
		pPlayer->SetMove(currentMove);

		// �����G���W���ɑ��x�Z�b�g
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(currentMove.x);
		velocity.setZ(currentMove.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);

		// ���ړ��I������
		if (!pPlayer->GetIsSideMoving())
		{
			m_pMachine->ChangeState<CPlayer_StandState>();
		}

		// �O���ړ����͂�����Βʏ�ړ��X�e�[�g�Ɉڍs
		if (pPlayer->GetIsMoving())
		{
			m_pMachine->ChangeState<CPlayer_MoveState>();
		}

		// �W�����v���͂�����΃W�����v�X�e�[�g�Ɉڍs
		if (input.jump && pPlayer->GetOnGround() && !pPlayer->GetIsJumping())
		{
			m_pMachine->ChangeState<CPlayer_JumpState>();
		}
	}

	void OnExit(CPlayer* pPlayer)override
	{

	}

private:

};

//*****************************************************************************
// �v���C���[�̃W�����v���
//*****************************************************************************
class CPlayer_JumpState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{
		pPlayer->SetIsJumping(true);

		// �W�����v���[�V����
		pPlayer->SetMotion(CMotion::TYPE_JUMP, 10);

		// �W�����v�C���p���X
		btVector3 jumpImpulse(0, MAX_JUMP_POWER, 0);
		pPlayer->GetRigidBody()->applyCentralImpulse(jumpImpulse);

		m_count = 0;
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		m_count++;

		// ���͎擾
		InputData input = pPlayer->GatherInput();

		// �t���O�X�V
		pPlayer->UpdateMovementFlags(input.moveDir, input.sideMoveDir);

		// �ڕW���x�v�Z
		float moveSpeed = PLAYER_SPEED;
		if (pPlayer->GetIsSideMoving() && !pPlayer->GetIsMoving())
		{
			moveSpeed = SIDE_MOVE_SPEED;
		}

		D3DXVECTOR3 targetMove = input.moveDir + input.sideMoveDir;

		if (targetMove.x != 0.0f || targetMove.z != 0.0f)
		{
			D3DXVec3Normalize(&targetMove, &targetMove);
			float moveSpeed = PLAYER_SPEED;
			if (pPlayer->GetIsSideMoving() && !pPlayer->GetIsMoving())
			{
				moveSpeed = SIDE_MOVE_SPEED;
			}

			targetMove *= moveSpeed;
		}
		else
		{
			targetMove = D3DXVECTOR3(0, 0, 0);
		}

		// ���ݑ��x�Ƃ̕�ԁi�C�[�W���O�j
		const float accelRate = 0.15f;
		D3DXVECTOR3 currentMove = pPlayer->GetMove();

		currentMove.x += (targetMove.x - currentMove.x) * accelRate;
		currentMove.z += (targetMove.z - currentMove.z) * accelRate;

		// ��Ԍ�̑��x���v���C���[�ɃZ�b�g
		pPlayer->SetMove(currentMove);

		// �����G���W���ɃZ�b�g
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(currentMove.x);
		velocity.setZ(currentMove.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);

		if (m_count >= 55)
		{// �����x������false�ɂ���
			pPlayer->SetIsJumping(false);
		}

		// �W�����v���̓X�e�[�g�؂�ւ����������X�L�b�v
		if (pPlayer->GetOnGround() && !pPlayer->GetIsJumping())
		{
			// �ړ���Ԃ֐؂�ւ�
			if (input.sideMoveDir.x != 0.0f || input.sideMoveDir.z != 0.0f)
			{
				// ���ړ����
				m_pMachine->ChangeState<CPlayer_BesideMoveState>();
			}
			else if (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f)
			{
				// �ړ����
				m_pMachine->ChangeState<CPlayer_MoveState>();
			}
			else
			{
				// �ҋ@���
				m_pMachine->ChangeState<CPlayer_StandState>();
			}
		}
	}

	void OnExit(CPlayer* pPlayer)override
	{

	}

private:
	int m_count;
};

#endif
