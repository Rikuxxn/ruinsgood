//=============================================================================
//
// �v���C���[���� [player.h]
// Author : TANEKAWA RIKU
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

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_PARTS		(32)					// �ő�p�[�c��
#define PLAYER_SPEED	(200.0f)				// �ړ��X�s�[�h
#define SIDE_MOVE_SPEED (PLAYER_SPEED * 0.6f)	// ���ړ���60%�̃X�s�[�h
#define MAX_JUMP_POWER	(330.3f)				// �W�����v����
#define MAX_GRAVITY		(-0.26f)				// �d�͉����x

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
	D3DXVECTOR3 GetPos(void);
	D3DXVECTOR3 GetRot(void)const { return m_rot; };
	D3DXVECTOR3 GetMove(void) const { return m_move; }
	D3DXVECTOR3 GetColliderPos(void) const { return m_colliderPos; }
	btScalar GetRadius(void) const { return m_radius; }
	btScalar GetHeight(void) const { return m_height; }
	btRigidBody* GetRigidBody(void) const { return m_pRigidBody; }						// RigidBody�̎擾
	bool GetPlayerUse(void) const { return m_playerUse; }
	bool OnGround(btDiscreteDynamicsWorld* world, btRigidBody* playerBody, float rayLength);
	void ReleasePhysics(void);														// Physics�j���p
	void HoldBlock(void);
	D3DXVECTOR3 GetForward(void) const;
	CBlock* FindFrontBlockByRaycast(float rayLength);
	void Controll(void);
	void SetInWater(bool flag);
	void AddWaterStayTime(float delta);
	void ResetWaterStayTime(void);
	void RespawnToCheckpoint(void);
	void RespawnToCheckpoint(D3DXVECTOR3 pos);
	void AddRespawnPoint(const D3DXVECTOR3& pos);
	D3DXVECTOR3 GetNearestRespawnPoint(void) const;
	D3DXVECTOR3 Lerp(const D3DXVECTOR3& a, const D3DXVECTOR3& b, float t);

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
	const int JUMP_HOLD_FRAMES = 60;	// ���̃t���[�����܂ł̓W�����v���Ƃ݂Ȃ�
	bool m_playerUse;					// �g���Ă��邩�ǂ���
	bool m_isJumping;					// �W�����v���t���O
	bool m_bIsMoving;					// �ړ����̓t���O
	bool m_bIsSideMoving;				// ���ړ����̓t���O
	bool m_bOnGround;					// �ڒn�t���O
	float m_waterStayTime;				// �����؍ݎ��ԁi�b�j
	bool m_isInWater;					// �������ɂ��邩
	std::vector<D3DXVECTOR3> m_ResPos;
};

#endif
