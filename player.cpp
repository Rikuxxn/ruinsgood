//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : TANEKAWA RIKU
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
	m_colliderPos		= INIT_VEC3;
	m_jumpFrame			= 0;
	m_pCarryingBlock	= NULL;
	m_waterStayTime		= 0.0f;							// �����؍ݎ��ԁi�b�j
	m_isInWater			= false;						// �������ɂ��邩
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

	// ���X�|�[���ʒu�̐ݒ�
	pPlayer->AddRespawnPoint(D3DXVECTOR3(427.0f, 30.0f, 1110.0f));
	pPlayer->AddRespawnPoint(D3DXVECTOR3(-1327.0f, 30.0f, -4586.0f));
	pPlayer->AddRespawnPoint(D3DXVECTOR3(2810.0f, 30.0f, -1518.0f));

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
	//m_pRigidBody->setGravity(btVector3(0, -139.8f, 0)); // �d��

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
	m_bIsMoving = false;
	m_bIsSideMoving = false;

	// �R���g���[������
	Controll();

	if (CManager::GetCamera()->GetMode() != CCamera::MODE_DIRECTION)
	{
		// �u���b�N��������
		HoldBlock();
	}

	CParticle* pParticle = NULL;

	// ���[�V�����؂�ւ�
	if (m_isJumping)
	{
		m_jumpFrame++;

		if (m_jumpFrame > JUMP_HOLD_FRAMES && m_bOnGround)
		{
			m_isJumping = false;

			m_pMotion->StartBlendMotion(m_pMotion->TYPE_NEUTRAL, 10);
			m_currentMotion = m_pMotion->TYPE_NEUTRAL;
		}
	}
	else
	{
		if (m_bIsSideMoving)
		{
			if (m_currentMotion != m_pMotion->TYPE_BESIDEMOVE)
			{
				m_pMotion->StartBlendMotion(m_pMotion->TYPE_BESIDEMOVE, 10);
				m_currentMotion = m_pMotion->TYPE_BESIDEMOVE;
			}
		}
		else if (m_bIsMoving)
		{
			//pParticle = CParticle::Create(m_pos, D3DXCOLOR(0.6f, 0.6f, 0.6f, 0.3f), 30, CParticle::TYPE_DUSH, 1);

			if (m_currentMotion != m_pMotion->TYPE_MOVE)
			{
				m_pMotion->StartBlendMotion(m_pMotion->TYPE_MOVE, 10);
				m_currentMotion = m_pMotion->TYPE_MOVE;
			}
		}
		else
		{
			if (m_currentMotion != m_pMotion->TYPE_NEUTRAL)
			{
				m_pMotion->StartBlendMotion(m_pMotion->TYPE_NEUTRAL, 20);
				m_currentMotion = m_pMotion->TYPE_NEUTRAL;
			}
		}
	}

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


	// ���݈ʒu�𕨗����[���h����擾���� m_pos �ɔ��f
	btTransform trans;
	m_pRigidBody->getMotionState()->getWorldTransform(trans);
	btVector3 pos = trans.getOrigin();
	m_colliderPos = D3DXVECTOR3(pos.getX(), pos.getY(), pos.getZ());
	m_pos = m_colliderPos - D3DXVECTOR3(0, 50.0f, 0); // �����ւ̃I�t�Z�b�g

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
// �ʒu�̎擾
//=============================================================================
D3DXVECTOR3 CPlayer::GetPos(void)
{
	return m_pos;
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

	if (pInputMouse->GetPress(0) || pInputJoypad->GetPressR2())
	{
		if (!m_pCarryingBlock)
		{
			CBlock* target = FindFrontBlockByRaycast(50.0f);// ���C�̒�����ݒ�

			if (target)
			{
				// �u���b�N�̎��ʂ��擾
				float blockMass = target->GetMassByType(target->GetType());

				// �u���b�N�̃T�C�Y(�g�嗦)���擾
				D3DXVECTOR3 blocksize = target->GetSize();// �g�嗦

				bool isTooLarge =
					blocksize.x >= 3.0f ||
					blocksize.y >= 3.0f ||
					blocksize.z >= 3.0f;

				if (blockMass > 5.0f || isTooLarge)
				{// ��莿�ʂ𒴂����� �܂��� ���T�C�Y�𒴂�����
					return;
				}

				m_pCarryingBlock = target;
				m_pCarryingBlock->SetEditMode(true); // �L�l�}�e�B�b�N��

				// Y���̂݉�]
				m_pCarryingBlock->GetRigidBody()->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
			}
		}
		else
		{
			//btRigidBody* pRigid = m_pCarryingBlock->GetRigidBody();

			// �����Ă��� �� �v���C���[�O���Ɉړ�
			D3DXVECTOR3 targetPos = GetPos() + GetForward();

			targetPos.y = GetPos().y + 140.0f; // ��������

			D3DXVECTOR3 currentPos = m_pCarryingBlock->GetPos();
			float moveSpeed = 10.0f; // �ړ��X�s�[�h

			// ��Ԃ���
			D3DXVECTOR3 newPos = Lerp(currentPos, targetPos, moveSpeed * 1.0f/60.0f);
			m_pCarryingBlock->SetPos(newPos);

			// �v���C���[�̌����ɍ��킹��Y����]
			D3DXVECTOR3 rot = m_pCarryingBlock->GetRot();
			rot.x = 0.0f;                   // ������ۂ�
			rot.z = 0.0f;
			rot.y = m_rot.y;               // �v���C���[�̌����Ɠ�����
			m_pCarryingBlock->SetRot(rot);
		}
	}
	else
	{
		if (m_pCarryingBlock)
		{
			// ��]���������i���R�ɓ]����j
			m_pCarryingBlock->GetRigidBody()->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));

			m_pCarryingBlock->SetEditMode(false); // �����ɖ߂�
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

			if (!pBlock->IsStaticBlock())
			{
				return pBlock; // ���I�ȃu���b�N���O���ɂ�����
			}
		}
	}

	return NULL;
}
//=============================================================================
// �R���g���[������
//=============================================================================
void CPlayer::Controll(void)
{
	// �L�[�{�[�h�̎擾
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	// �W���C�p�b�h�̎擾
	CInputJoypad* pInputJoypad = CManager::GetInputJoypad();

	// �W���C�X�e�B�b�N���̎擾
	XINPUT_STATE* pStick = CInputJoypad::GetStickAngle();

	// �J�����̎擾
	CCamera* pCamera = CManager::GetCamera();

	// �J�����̊p�x�̎擾
	D3DXVECTOR3 CamRot = pCamera->GetRot();

	// �J�������[�h�̎擾
	CCamera::MODE camMode = pCamera->GetMode();

	// ���͕����x�N�g��
	D3DXVECTOR3 inputDir = { 0.0f, 0.0f, 0.0f };

	// ---------------------------
	// �Q�[���p�b�h����
	// ---------------------------
	if (pInputJoypad->GetStick() == true && pStick != NULL)
	{
		float stickX = pStick->Gamepad.sThumbLX;
		float stickY = pStick->Gamepad.sThumbLY;

		float magnitude = sqrtf(stickX * stickX + stickY * stickY);

		const float DEADZONE = 10922.0f;
		if (magnitude >= DEADZONE)
		{
			// ���K��
			stickX /= magnitude;
			stickY /= magnitude;

			float normalizedMagnitude = (magnitude - DEADZONE) / (32767.0f - DEADZONE);
			normalizedMagnitude = std::min(normalizedMagnitude, 1.0f);

			stickX *= normalizedMagnitude;
			stickY *= normalizedMagnitude;

			float cameraYaw = CamRot.y;

			// �J���������ŉ�]�����ړ��x�N�g��
			inputDir.x += -(stickX * cosf(cameraYaw) + stickY * sinf(cameraYaw));
			inputDir.z += stickX * sinf(-cameraYaw) + stickY * cosf(cameraYaw);

			// ��]�␳�iZ�����]�j
			inputDir.z = -inputDir.z;

			// �����X�V
			if (camMode == pCamera->MODE_EDIT)
			{
				m_rotDest.y = atan2f(-inputDir.x, -inputDir.z);
			}
			else
			{
				m_rotDest.y = CamRot.y;
			}

			// �t���O
			m_bIsMoving = true;

			// ���ړ������Ȃ� m_bIsSideMoving �𗧂Ă�
			if (fabsf(stickX) > 0.5f && fabsf(stickY) < 0.3f)
			{
				m_bIsSideMoving = true;
			}
		}
	}

	// ---------------------------
	// �L�[�{�[�h����
	// ---------------------------
	bool pressW = pInputKeyboard->GetPress(DIK_W);
	bool pressS = pInputKeyboard->GetPress(DIK_S);
	bool pressA = pInputKeyboard->GetPress(DIK_A);
	bool pressD = pInputKeyboard->GetPress(DIK_D);

	// ���͕������Z
	if (pressW)
	{
		inputDir += D3DXVECTOR3(-sinf(CamRot.y), 0, -cosf(CamRot.y));
	}
	if (pressS)
	{
		inputDir += D3DXVECTOR3(sinf(CamRot.y), 0, cosf(CamRot.y));
	}
	if (pressA)
	{
		inputDir += D3DXVECTOR3(cosf(CamRot.y), 0, -sinf(CamRot.y));
	}
	if (pressD)
	{
		inputDir += D3DXVECTOR3(-cosf(CamRot.y), 0, sinf(CamRot.y));
	}

	// �t���O����i�΂߂�m_bIsMoving��D��j
	if ((pressW || pressS))
	{
		m_bIsMoving = true;
	}
	else if ((pressA || pressD) && !(pressW || pressS))
	{
		m_bIsSideMoving = true;
	}

	if (inputDir.x != 0.0f || inputDir.z != 0.0f)
	{
		D3DXVec3Normalize(&inputDir, &inputDir);
	}

	// RayCast�Őڒn�`�F�b�N
	m_bOnGround = OnGround(CManager::GetPhysicsWorld(), m_pRigidBody, 65.0f);

	// �W�����v���͂������āA���n�ʂɗ����Ă��鎞�����W�����v
	if (!m_isJumping &&
		(pInputKeyboard->GetTrigger(DIK_SPACE) || pInputJoypad->GetTrigger(pInputJoypad->JOYKEY_A)) &&
		m_bOnGround)
	{
		m_isJumping = true;
		m_jumpFrame = 0;

		// �W�����v�C���p���X��Y�����ɉ�����
		btVector3 jumpImpulse(0, MAX_JUMP_POWER, 0);
		m_pRigidBody->applyCentralImpulse(jumpImpulse);

		// �W�����v���[�V����
		m_pMotion->StartBlendMotion(m_pMotion->TYPE_JUMP, 10);
		m_currentMotion = m_pMotion->TYPE_JUMP;
	}

	// ���݂̑��x���擾
	btVector3 velocity = m_pRigidBody->getLinearVelocity();

	// ---------------------------
	// ���ړ��̖ڕW���x���v�Z
	// ---------------------------
	if (m_bIsMoving || m_bIsSideMoving)
	{
		float moveSpeed = PLAYER_SPEED;

		// ���ړ������Ȃ�X�s�[�h���Ƃ�
		if (m_bIsSideMoving && !m_bIsMoving && !camMode == pCamera->MODE_EDIT)
		{
			moveSpeed = SIDE_MOVE_SPEED;
		}

		m_targetMove = inputDir * moveSpeed;

		// �����x��ԁi�C�[�W���O�j
		const float accelRate = 0.15f; // ���l���グ��Ƒ�����������
		m_currentMove.x += (m_targetMove.x - m_currentMove.x) * accelRate;
		m_currentMove.z += (m_targetMove.z - m_currentMove.z) * accelRate;
	}
	else
	{
		// �ړ����Ȃ��Ƃ��͏��X�Ɍ���
		m_targetMove = D3DXVECTOR3(0, 0, 0);
		m_currentMove *= 0.95f;
	}

	// ���x��ݒ�
	velocity.setX(m_currentMove.x);
	velocity.setZ(m_currentMove.z);

	// ���x�𕨗��G���W���ɃZ�b�g
	m_pRigidBody->setLinearVelocity(velocity);

	if (camMode == pCamera->MODE_EDIT)
	{
		// �v���C���[�̌����X�V
		if (m_bIsMoving || m_bIsSideMoving)
		{
			m_rotDest.y = atan2f(-inputDir.x, -inputDir.z);
		}
	}
	else
	{
		// �v���C���[�̌����X�V
		m_rotDest.y = CamRot.y;
	}
}
//=============================================================================
// �����ɂ���t���O�ݒ菈��
//=============================================================================
void CPlayer::SetInWater(bool flag)
{
	m_isInWater = flag;
}
//=============================================================================
// �����؍ݎ��Ԃ̐ݒ�
//=============================================================================
void CPlayer::AddWaterStayTime(float delta)
{
	if (m_isInWater)
	{
		m_waterStayTime += delta;

		if (m_waterStayTime >= 3.0f) // 3�b�ȏ�؍݂�����
		{
			RespawnToCheckpoint(); // �C�ӂ̏ꏊ�փ��X�|�[��
			m_waterStayTime = 0.0f;
		}
	}
	else
	{
		m_waterStayTime = 0.0f;
	}
}
//=============================================================================
// �����؍ݎ��Ԃ̃��Z�b�g
//=============================================================================
void CPlayer::ResetWaterStayTime(void)
{
	m_waterStayTime = 0.0f;
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
