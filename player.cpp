//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "player.h"
#include "texture.h"
#include "manager.h"
#include "model.h"
#include "particle.h"
#include "game.h"
#include "stageselect.h"

using namespace std;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CPlayer::CPlayer(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	m_pos				= INIT_VEC3;					// �ʒu
	m_ResPos			= {};							// ��������ʒu
	m_rot				= INIT_VEC3;					// ����
	m_rotDest			= INIT_VEC3;					// ����
	m_move				= INIT_VEC3;					// �ړ���
	m_targetMove		= INIT_VEC3;					// �ڕW���x
	m_currentMove		= INIT_VEC3;					// ���ۂ̈ړ����x
	m_size				= D3DXVECTOR3(1.0f, 1.0f, 1.0f);// �T�C�Y
	m_mtxWorld			= {};							// ���[���h�}�g���b�N�X
	m_nNumModel			= 0;							// ���f��(�p�[�c)�̑���
	m_playerUse			= true;							// �g���Ă��邩�ǂ���
	m_pShadowS			= NULL;							// �X�e���V���V���h�E�ւ̃|�C���^
	m_pMotion			= NULL;							// ���[�V�����ւ̃|�C���^
	m_currentMotion		= CMotion::TYPE_NEUTRAL;		// ���݂̃��[�V����
	m_isJumping			= false;						// �W�����v���t���O
	m_bIsMoving			= false;						// �ړ����̓t���O
	m_bIsSideMoving		= false;						// ���ړ����̓t���O
	m_pRigidBody		= NULL;							// ���̂ւ̃|�C���^
	m_bOnGround			= false;						// �ڒn�t���O
	m_pShape			= NULL;							// �����蔻��̌`�ւ̃|�C���^
	m_pDebug3D			= NULL;							// 3D�f�o�b�O�\���ւ̃|�C���^
	m_radius			= 0.0f;							// �J�v�Z���R���C�_�[�̔��a
	m_height			= 0.0f;							// �J�v�Z���R���C�_�[�̍���
	m_colliderPos		= INIT_VEC3;					// �R���C�_�[�̈ʒu
	m_jumpFrame			= 0;							// �W�����v���Ă��牽�t���[���o�߂�����
	m_pCarryingBlock	= NULL;							// �^��ł���u���b�N
	m_particleTimer		= 0;							// �^�C�}�[
	for (int nCnt = 0; nCnt < MAX_PARTS; nCnt++)
	{
		m_apModel[nCnt] = {};						// ���f��(�p�[�c)�ւ̃|�C���^
	}
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CPlayer::~CPlayer()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CPlayer* CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CPlayer* pPlayer;

	pPlayer = new CPlayer;

	pPlayer->m_pos = pos;
	pPlayer->m_rot = D3DXToRadian(rot);

	// �X�e�[�WID�̎擾
	int stageId = CStageSelect::GetSelectedStage();

	if (stageId == 0)
	{
		// ���X�|�[���ʒu�̐ݒ�
		pPlayer->AddRespawnPoint(D3DXVECTOR3(427.0f, 30.0f, 1110.0f));
		pPlayer->AddRespawnPoint(D3DXVECTOR3(-1327.0f, 30.0f, -4586.0f));
		pPlayer->AddRespawnPoint(D3DXVECTOR3(2810.0f, 30.0f, -1518.0f));
		pPlayer->AddRespawnPoint(D3DXVECTOR3(-1380.0f, 30.0f, -6861.7f));
	}
	else if (stageId == 1)
	{
		pPlayer->AddRespawnPoint(D3DXVECTOR3(-1132.0f, 310.0f, 724.5f));
		pPlayer->AddRespawnPoint(D3DXVECTOR3(1132.0f, 310.0f, 724.5f));
		pPlayer->AddRespawnPoint(D3DXVECTOR3(1540.0f, 420.0f, -990.0f));
	}

	// ����������
	pPlayer->Init();

	return pPlayer;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CPlayer::Init(void)
{
	CModel* pModels[MAX_PARTS];
	int nNumModels = 0;

	// �p�[�c�̓ǂݍ���
	m_pMotion = CMotion::Load("data/motion.txt", pModels, nNumModels);

	for (int nCnt = 0; nCnt < nNumModels && nCnt < MAX_PARTS; nCnt++)
	{
		m_apModel[nCnt] = pModels[nCnt];

		// �I�t�Z�b�g�l��
		m_apModel[nCnt]->SetOffsetPos(m_apModel[nCnt]->GetPos());
		m_apModel[nCnt]->SetOffsetRot(m_apModel[nCnt]->GetRot());
	}

	// �p�[�c������
	m_nNumModel = nNumModels;

	// �v���C���[���g���Ă���
	m_playerUse = true;

	// �ϐ��̏�����
	m_rot = D3DXVECTOR3(0.0f, -D3DX_PI, 0.0f);

	//*********************************************************************
	// Bullet Physics �J�v�Z���R���C�_�[�̐ݒ�
	//*********************************************************************

	m_radius = 18.5f;
	m_height = 55.5f;

	m_pShape = new btCapsuleShape(m_radius, m_height);

	// �R���C�_�[���S = ���� + �I�t�Z�b�g
	m_colliderPos = m_pos + D3DXVECTOR3(0, 20.0f, 0);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(m_colliderPos.x, m_colliderPos.y, m_colliderPos.z));

	// ���ʂ�ݒ�
	btScalar mass = 1.0f;
	btVector3 inertia(0, 0, 0);  // ����

	m_pShape->calculateLocalInertia(mass, inertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, m_pShape, inertia);

	m_pRigidBody = new btRigidBody(rbInfo);

	m_pRigidBody->setAngularFactor(btVector3(0, 0, 0));
	m_pRigidBody->setFriction(0.0f);// ���C
	m_pRigidBody->setRollingFriction(0.0f);// �]���薀�C

	m_pRigidBody->setUserPointer(this);
	m_pRigidBody->setActivationState(DISABLE_DEACTIVATION);// �X���[�v��Ԃɂ��Ȃ�

	// �������[���h�ɒǉ�
	btDiscreteDynamicsWorld* pWorld = CManager::GetPhysicsWorld();

	if (pWorld != NULL)
	{
		pWorld->addRigidBody(m_pRigidBody);
	}

	// �X�e���V���V���h�E�̐���
	m_pShadowS = CShadowS::Create("data/MODELS/stencilshadow.x",m_pos);

	// ���������ɂ��̃C���X�^���X�̃|�C���^��n��
	m_stateMachine.Start(this);

	// ������Ԃ̃X�e�[�g���Z�b�g
	m_stateMachine.ChangeState<CPlayer_StandState>();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CPlayer::Uninit(void)
{
	ReleasePhysics();

	for (int nCnt = 0; nCnt < MAX_PARTS; nCnt++)
	{
		if (m_apModel[nCnt] != NULL)
		{
			m_apModel[nCnt]->Uninit();
			delete m_apModel[nCnt];
			m_apModel[nCnt] = NULL;
		}
	}

	if (m_pMotion != NULL)
	{
		delete m_pMotion;
		m_pMotion = NULL;
	}

	// �I�u�W�F�N�g�̔j��(�������g)
	this->Release();
}
//=============================================================================
// �X�V����
//=============================================================================
void CPlayer::Update(void)
{
	// �J�����̎擾
	CCamera* pCamera = CManager::GetCamera();

	// �J�����̊p�x�̎擾
	D3DXVECTOR3 CamRot = pCamera->GetRot();

	// �J�������[�h�̎擾
	CCamera::MODE camMode = pCamera->GetMode();

	if (CManager::GetCamera()->GetMode() != CCamera::MODE_DIRECTION)
	{
		// �u���b�N��������
		HoldBlock();
	}

	if (!m_isJumping)
	{
		// �ڒn����
		m_bOnGround = OnGround(CManager::GetPhysicsWorld(), m_pRigidBody, 55.0f);
	}

	// ���͔���̎擾
	InputData input = GatherInput();

	// �X�e�[�g�}�V���X�V
	m_stateMachine.Update();

	// �����̐��K��
	if (m_rotDest.y - m_rot.y > D3DX_PI)
	{
		m_rot.y += D3DX_PI * 2.0f;
	}
	else if (m_rotDest.y - m_rot.y < -D3DX_PI)
	{
		m_rot.y -= D3DX_PI * 2.0f;
	}

	m_rot.y += (m_rotDest.y - m_rot.y) * 0.09f;

	// �����X�V
	if (camMode == CCamera::MODE_EDIT)
	{
		// �ړ����͂�����΃v���C���[��������͕�����
		if (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f ||
			input.sideMoveDir.x != 0.0f || input.sideMoveDir.z != 0.0f)
		{
			m_rotDest.y = atan2f(-input.moveDir.x - input.sideMoveDir.x,
				-input.moveDir.z - input.sideMoveDir.z);
		}
	}
	else
	{
		// �J���������ɌŒ�
		m_rotDest.y = CamRot.y;
	}

	// ���݈ʒu�𕨗����[���h����擾���� m_pos �ɔ��f
	btTransform trans;
	m_pRigidBody->getMotionState()->getWorldTransform(trans);
	btVector3 pos = trans.getOrigin();
	m_colliderPos = D3DXVECTOR3(pos.getX(), pos.getY(), pos.getZ());
	m_pos = m_colliderPos - D3DXVECTOR3(0, 50.0f, 0); // �����ւ̃I�t�Z�b�g

	if (m_bIsMoving && !m_isJumping && m_bOnGround && !m_bIsSideMoving)
	{
		m_particleTimer++;

		if (m_particleTimer >= DASH_PARTICLE_INTERVAL)
		{
			m_particleTimer = 0;

			// �p�[�e�B�N���̐���
			CParticle::Create(INIT_VEC3,
				m_pos,
				D3DXCOLOR(0.6f, 0.6f, 0.6f, 0.4f),
				25,                    // ����
				CParticle::TYPE_MOVE,  // �p�[�e�B�N���^�C�v
				1);                    // ��
		}
	}
	else
	{
		m_particleTimer = 0; // ��~���̓��Z�b�g
	}

	if (m_pos.y < -480.0f)
	{
		// ���X�|�[������
		RespawnToCheckpoint();
	}

	if (m_pShadowS != NULL)
	{
		// �X�e���V���V���h�E�̈ʒu�ݒ�
		m_pShadowS->SetPosition(m_pos);
	}

	int nNumModels = 10;

	// ���[�V�����̍X�V����
	m_pMotion->Update(m_apModel, nNumModels);
}
//=============================================================================
// ���̍X�V����
//=============================================================================
void CPlayer::UpdateInfo(void)
{
	if (ImGui::TreeNode("Player Info"))
	{
		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		//*********************************************************************
		// POS �̒���
		//*********************************************************************

		// �ʒu
		ImGui::Text("POS"); ImGui::SameLine(70); // ���x���ʒu������

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80); // ��������
		ImGui::DragFloat("##Player_pos_x", &m_pos.x, 1.0f, -1000.0f, 1000.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_pos_y", &m_pos.y, 1.0f, -1000.0f, 1000.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_pos_z", &m_pos.z, 1.0f, -1000.0f, 1000.0f, "%.1f");

		//*********************************************************************
		// ROT �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ����
		ImGui::Text("ROT"); ImGui::SameLine(70); // ���x���ʒu������

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80); // ��������
		ImGui::DragFloat("##Player_rot_x", &m_rot.x, 0.1f, -D3DX_PI, D3DX_PI, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_rot_y", &m_rot.y, 0.1f, -D3DX_PI, D3DX_PI, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_rot_z", &m_rot.z, 0.1f, -D3DX_PI, D3DX_PI, "%.1f");

		//*********************************************************************
		// SIZE �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���x��
		ImGui::Text("SIZE"); ImGui::SameLine(70); // ���x���̈ʒu������ƒ���

		// X
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_size_x", &m_size.x, 0.1f, -100.0f, 100.0f, "%.1f");

		// Y
		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_size_y", &m_size.y, 0.1f, -100.0f, 100.0f, "%.1f");

		// Z
		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_size_z", &m_size.z, 0.1f, -100.0f, 100.0f, "%.1f");

		//*********************************************************************
		// �J�v�Z���R���C�_�[ �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���x��
		ImGui::Text("Capsule Collider"); // ���x���̈ʒu������ƒ���

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���a
		ImGui::Text("Radius:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_Radius", &m_radius, 0.1f, -500.0f, 500.0f, "%.1f");

		// ����
		ImGui::Text("Height:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_Height", &m_height, 0.1f, -500.0f, 500.0f, "%.1f");

		ImGui::TreePop(); // ����
	}

	// �J�v�Z���R���C�_�[�X�V�`�F�b�N
	static float oldRadius = -1.0f;
	static float oldHeight = -1.0f;

	if (m_radius != oldRadius || m_height != oldHeight)
	{
		// �����̃R���W�����`����폜
		if (m_pShape)
		{
			delete m_pShape;
			m_pShape = NULL;
		}

		// �V���� Capsule Shape ���쐬
		m_pShape = new btCapsuleShape(m_radius, m_height);

		// ���W�b�h�{�f�B�ɔ��f
		if (m_pRigidBody)
		{
			m_pRigidBody->setCollisionShape(m_pShape);
		}

		// �l���L�^
		oldRadius = m_radius;
		oldHeight = m_height;
	}

}
//=============================================================================
// �`�揈��
//=============================================================================
void CPlayer::Draw(void)
{
	// �f�o�C�X�̎擾
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans, mtxSize;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �T�C�Y�𔽉f
	D3DXMatrixScaling(&mtxSize, m_size.x, m_size.y, m_size.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxSize);

	// �����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	for (int nCntMat = 0; nCntMat < m_nNumModel; nCntMat++)
	{
		// ���f��(�p�[�c)�̕`��
		if (m_apModel[nCntMat])
		{
			m_apModel[nCntMat]->Draw();
		}
	}

#ifdef _DEBUG

	// �J�v�Z���R���C�_�[�̕`��
	if (m_pRigidBody && m_pShape)
	{
		btTransform transform;
		m_pRigidBody->getMotionState()->getWorldTransform(transform);

		m_pDebug3D->DrawCapsuleCollider((btCapsuleShape*)m_pShape, transform, D3DXCOLOR(1, 1, 1, 1));
	}

#endif

}
//=============================================================================
// ���C�ɂ��ڐG����
//=============================================================================
bool CPlayer::OnGround(btDiscreteDynamicsWorld* world, btRigidBody* playerBody, float rayLength)
{
	btTransform trans;
	playerBody->getMotionState()->getWorldTransform(trans);

	btVector3 start = trans.getOrigin();
	btVector3 end = start - btVector3(0, rayLength, 0);

	struct RayResultCallback : public btCollisionWorld::ClosestRayResultCallback
	{
		RayResultCallback(const btVector3& from, const btVector3& to)
			: btCollisionWorld::ClosestRayResultCallback(from, to) {}
	};

	RayResultCallback rayCallback(start, end);
	world->rayTest(start, end, rayCallback);

	if (rayCallback.hasHit())
	{// �ڐG
		return true;
	}

	return false;
}
//=============================================================================
// Physics�̔j��
//=============================================================================
void CPlayer::ReleasePhysics(void)
{
	if (m_pRigidBody)
	{
		btDiscreteDynamicsWorld* pWorld = CManager::GetPhysicsWorld();
		if (pWorld) pWorld->removeRigidBody(m_pRigidBody);
		delete m_pRigidBody->getMotionState();
		delete m_pRigidBody;
		m_pRigidBody = NULL;
	}

	if (m_pShape)
	{
		delete m_pShape;
		m_pShape = NULL;
	}
}
//=============================================================================
// �u���b�N��������
//=============================================================================
void CPlayer::HoldBlock(void)
{
	// �}�E�X�̎擾
	CInputMouse* pInputMouse = CManager::GetInputMouse();

	// �W���C�p�b�h�̎擾
	CInputJoypad* pInputJoypad = CManager::GetInputJoypad();

	CParticle* pParticle = NULL;

	if (pInputMouse->GetPress(0) || pInputJoypad->GetPressR2())
	{
		if (!m_pCarryingBlock)
		{
			CBlock* target = FindFrontBlockByRaycast(50.0f);// ���C�̒�����ݒ�

			if (!target)
			{
				return;
			}

			// �u���b�N�̎��ʂ��擾
			float blockMass = target->GetMass();

			// �u���b�N�̃T�C�Y(�g�嗦)���擾
			D3DXVECTOR3 blocksize = target->GetSize();// �g�嗦

			float maxsize = 2.0f;

			bool isTooLarge =
				blocksize.x >= maxsize ||
				blocksize.y >= maxsize ||
				blocksize.z >= maxsize;

			if (blockMass > 7.0f || isTooLarge)
			{// ��莿�ʂ𒴂����� �܂��� ���T�C�Y�𒴂�����
				return;
			}

			int stageId = CStageSelect::GetSelectedStage();
			if (stageId == 0 && (target->GetType() == CBlock::TYPE_BLOCK3 || target->GetType() == CBlock::TYPE_TORCH2))
			{
				return;
			}

			m_pCarryingBlock = target;

			if (m_pCarryingBlock->GetAngularFactor() == btVector3(0.0f, 0.0f, 0.0f))
			{
				// ����]
				m_pCarryingBlock->GetRigidBody()->setAngularFactor(m_pCarryingBlock->GetAngularFactor());
			}
			else
			{
				// Y���̂݉�]
				m_pCarryingBlock->GetRigidBody()->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
			}
		}
		else
		{
			btRigidBody* pRigid = m_pCarryingBlock->GetRigidBody();

			if (!pRigid || !m_pCarryingBlock->IsDynamicBlock())
			{
				return;
			}

			// ���݂̈ʒu
			D3DXVECTOR3 currentPos = m_pCarryingBlock->GetPos();
			D3DXVECTOR3 playerPos = GetPos();

			// �����`�F�b�N
			D3DXVECTOR3 diff = currentPos - playerPos;
			float distance = D3DXVec3Length(&diff);
			const float maxCarryDistance = 200.0f; // �����������(���̋����𒴂�����u���b�N�𗣂�)

			if (distance > maxCarryDistance || currentPos <= playerPos)
			{
				// ����
				//m_pCarryingBlock->GetRigidBody()->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));
				m_pCarryingBlock = NULL;
				return;
			}

			float ftargetDis = m_pCarryingBlock->CarryTargetDis();// ��������(�����グ���Ƃ��̖ڕW����)

			D3DXVECTOR3 targetPos;// �����グ�����^�[�Q�b�g�ʒu
			
			if (m_pCarryingBlock->GetType() == CBlock::TYPE_MOVE_FIRE_STATUE ||
				m_pCarryingBlock->GetType() == CBlock::TYPE_BLOCK3)
			{
				// XZ�̂ݒǏ]�AY�͌Œ�
				targetPos = currentPos;
				targetPos.x = playerPos.x + GetForward().x * ftargetDis;
				targetPos.z = playerPos.z + GetForward().z * ftargetDis;
				targetPos.y = currentPos.y; // ������ς��Ȃ�
			}
			else
			{
				// �ʏ�͑O���Ɏ����グ��
				targetPos = playerPos + GetForward() * ftargetDis;
				targetPos.y = GetPos().y + 70.0f; // �ʏ�͎����グ��

				// �p�[�e�B�N������
				pParticle = CParticle::Create(INIT_VEC3, targetPos, D3DXCOLOR(0.6f, 1.0f, 0.4f, 1.0f), 50, CParticle::TYPE_FLOATING, 1);
			}

			// Bullet�p�̍���
			btVector3 posDiff(
				targetPos.x - currentPos.x,
				targetPos.y - currentPos.y,
				targetPos.z - currentPos.z
			);

			// ���݂̑��x
			btVector3 vel = pRigid->getLinearVelocity();

			// �X�v�����O�萔�E�����W���i�����p�j
			const float stiffness = 300.0f;  // �o�l�̋���
			const float damping = 30.0f;   // �����̋���

			// �X�v�����O�{�_���p�[�� = -kX - cV
			btVector3 springForce = posDiff * stiffness;
			btVector3 dampingForce = vel * -damping;
			btVector3 totalForce = springForce + dampingForce;

			// �͂�������
			pRigid->applyCentralForce(totalForce);

			// ���̑��x��}���Ĉ��肳����i�I�v�V�����j
			vel.setX(vel.getX() * 1.0f);
			vel.setZ(vel.getZ() * 1.0f);
			pRigid->setLinearVelocity(vel);

			// ��]���}����ƈ��肷��
			pRigid->setAngularVelocity(btVector3(0, 0, 0));
		}
	}
	else
	{
		if (m_pCarryingBlock)
		{
			// ��]��������
			m_pCarryingBlock->GetRigidBody()->setAngularFactor(m_pCarryingBlock->GetAngularFactor());

			m_pCarryingBlock = NULL;
		}
	}
}
//=============================================================================
// �v���C���[�̑O���x�N�g���擾
//=============================================================================
D3DXVECTOR3 CPlayer::GetForward(void) const
{
	// �v���C���[�̉�]�p�x�iY���j����O���x�N�g�����v�Z
	float yaw = m_rot.y;

	D3DXVECTOR3 forward(-sinf(yaw), 0.0f, -cosf(yaw));

	// ���K������
	D3DXVec3Normalize(&forward, &forward);

	return forward;
}
//=============================================================================
// �v���C���[�̑O�����C���菈��
//=============================================================================
CBlock* CPlayer::FindFrontBlockByRaycast(float rayLength)
{
	btDiscreteDynamicsWorld* world = CManager::GetPhysicsWorld();

	if (!world)
	{
		return NULL;
	}

	D3DXVECTOR3 from = GetPos() + D3DXVECTOR3(0.0f,20.0f,0.0f);
	D3DXVECTOR3 to = from + GetForward() * rayLength;

	btVector3 btFrom(from.x, from.y, from.z);
	btVector3 btTo(to.x, to.y, to.z);

	struct RayResultCallback : public btCollisionWorld::ClosestRayResultCallback
	{
		RayResultCallback(const btVector3& from, const btVector3& to)
			: btCollisionWorld::ClosestRayResultCallback(from, to) {}

		virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{
			// �u���b�N�ȊO�𖳎�����ꍇ�͂����Ńt�B���^�[���Ă�����
			return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
		}
	};

	RayResultCallback rayCallback(btFrom, btTo);
	world->rayTest(btFrom, btTo, rayCallback);

	if (rayCallback.hasHit())
	{
		void* userPtr = rayCallback.m_collisionObject->getUserPointer();

		if (userPtr)
		{
			CBlock* pBlock = static_cast<CBlock*>(userPtr);

			if (pBlock->IsDynamicBlock())
			{
				return pBlock; // ���I�ȃu���b�N���O���ɂ�����
			}
		}
	}

	return NULL;
}
//=============================================================================
// ���X�|�[��(��ԋ߂��ʒu)����
//=============================================================================
void CPlayer::RespawnToCheckpoint(void)
{
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	D3DXVECTOR3 respawnPos = GetNearestRespawnPoint(); // �C�ӂ̕ۑ��ʒu

	m_pos = respawnPos;

	btRigidBody* pRigid = GetRigidBody();

	if (pRigid)
	{
		pRigid->setLinearVelocity(btVector3(0, 0, 0));
		pRigid->setAngularVelocity(btVector3(0, 0, 0));

		// ���[���h���W�X�V
		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(respawnPos.x, respawnPos.y, respawnPos.z));

		pRigid->setWorldTransform(trans);

		if (pRigid->getMotionState())
		{
			pRigid->getMotionState()->setWorldTransform(trans);
		}
	}
}
//=============================================================================
// ���X�|�[��(���ڐݒ�)����
//=============================================================================
void CPlayer::RespawnToCheckpoint(D3DXVECTOR3 pos)
{
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	D3DXVECTOR3 respawnPos = pos; // �C�ӂ̈ʒu

	m_pos = respawnPos;

	btRigidBody* pRigid = GetRigidBody();

	if (pRigid)
	{
		pRigid->setLinearVelocity(btVector3(0, 0, 0));
		pRigid->setAngularVelocity(btVector3(0, 0, 0));

		// ���[���h���W�X�V
		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(respawnPos.x, respawnPos.y, respawnPos.z));

		pRigid->setWorldTransform(trans);

		if (pRigid->getMotionState())
		{
			pRigid->getMotionState()->setWorldTransform(trans);
		}
	}
}
//=============================================================================
// ���X�|�[���ʒu�̒ǉ�����
//=============================================================================
void CPlayer::AddRespawnPoint(const D3DXVECTOR3& pos)
{
	m_ResPos.push_back(pos);
}
//=============================================================================
// ��ԋ߂����X�|�[���ʒu��Ԃ�����
//=============================================================================
D3DXVECTOR3 CPlayer::GetNearestRespawnPoint(void) const
{
	if (m_ResPos.empty())
	{
		return m_pos; // �����o�^����Ă��Ȃ��ꍇ�͍��̈ʒu
	}

	float minDistSq = FLT_MAX;
	D3DXVECTOR3 nearest = m_ResPos[0];

	for (const auto& pt : m_ResPos)
	{
		D3DXVECTOR3 diff = pt - m_pos;
		float distSq = D3DXVec3LengthSq(&diff);

		if (distSq < minDistSq)
		{
			minDistSq = distSq;
			nearest = pt;
		}
	}

	return nearest;
}
//=============================================================================
// ��Ԋ֐�
//=============================================================================
D3DXVECTOR3 CPlayer::Lerp(const D3DXVECTOR3& a, const D3DXVECTOR3& b, float t)
{
	return a + (b - a) * t;
}
//=============================================================================
// ���[�V�����̐ݒ菈��
//=============================================================================
void CPlayer::SetMotion(CMotion::TYPE type, int nBlendFrame)
{
	m_pMotion->StartBlendMotion(type, nBlendFrame);
	m_currentMotion = type;
}
//=============================================================================
// ���͔���擾�֐�
//=============================================================================
InputData CPlayer::GatherInput(void)
{
	InputData input{};
	input.moveDir = D3DXVECTOR3(0, 0, 0);
	input.sideMoveDir = D3DXVECTOR3(0, 0, 0);
	input.jump = false;

	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();	// �L�[�{�[�h�̎擾
	CInputJoypad* pJoypad = CManager::GetInputJoypad();			// �W���C�p�b�h�̎擾
	XINPUT_STATE* pStick = CInputJoypad::GetStickAngle();		// �X�e�B�b�N�̎擾
	CCamera* pCamera = CManager::GetCamera();					// �J�����̎擾
	D3DXVECTOR3 CamRot = pCamera->GetRot();						// �J�����p�x�̎擾

	// ---------------------------
	// �W�����v����
	// ---------------------------
	if (pKeyboard->GetTrigger(DIK_SPACE) || pJoypad->GetTrigger(pJoypad->JOYKEY_A))
	{
		input.jump = true;
	}

	// ---------------------------
	// �Q�[���p�b�h����
	// ---------------------------
	if (pJoypad->GetStick() && pStick)
	{
		float stickX = pStick->Gamepad.sThumbLX;
		float stickY = pStick->Gamepad.sThumbLY;
		float magnitude = sqrtf(stickX * stickX + stickY * stickY);
		const float DEADZONE = 10922.0f;

		if (magnitude >= DEADZONE)
		{
			stickX /= magnitude;
			stickY /= magnitude;
			float normMag = std::min((magnitude - DEADZONE) / (32767.0f - DEADZONE), 1.0f);
			stickX *= normMag;
			stickY *= normMag;

			D3DXVECTOR3 dir;
			float yaw = CamRot.y;

			dir.x = -(stickX * cosf(yaw) + stickY * sinf(yaw));
			dir.z = stickX * sinf(-yaw) + stickY * cosf(yaw);
			dir.z = -dir.z;

			if (fabsf(stickX) > 0.5f && fabsf(stickY) < 0.3f)
			{
				input.sideMoveDir += D3DXVECTOR3(dir.x, 0, dir.z);
			}
			else
			{
				input.moveDir += D3DXVECTOR3(dir.x, 0, dir.z);
			}
		}
	}

	// ---------------------------
	// �L�[�{�[�h����
	// ---------------------------
	if (pKeyboard->GetPress(DIK_W))
	{
		input.moveDir += D3DXVECTOR3(-sinf(CamRot.y), 0, -cosf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_S))
	{
		input.moveDir += D3DXVECTOR3(sinf(CamRot.y), 0, cosf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_A))
	{
		input.sideMoveDir += D3DXVECTOR3(cosf(CamRot.y), 0, -sinf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_D))
	{
		input.sideMoveDir += D3DXVECTOR3(-cosf(CamRot.y), 0, sinf(CamRot.y));
	}

	// ���K��
	if (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f)
	{
		D3DXVec3Normalize(&input.moveDir, &input.moveDir);
	}

	if (input.sideMoveDir.x != 0.0f || input.sideMoveDir.z != 0.0f)
	{
		D3DXVec3Normalize(&input.sideMoveDir, &input.sideMoveDir);
	}

	return input;
}

