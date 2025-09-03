//=============================================================================
//
// �u���b�N���X�g���� [blocklist.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "blocklist.h"
#include "blockmanager.h"
#include "player.h"
#include "game.h"
#include "manager.h"
#include "particle.h"
#include <algorithm>
#include "collisionUtils.h"

using namespace std;

//=============================================================================
// �ؔ��u���b�N�̃R���X�g���N�^
//=============================================================================
CWoodBoxBlock::CWoodBoxBlock()
{
	// �l�̃N���A
	m_ResPos = INIT_VEC3;
}
//=============================================================================
// �ؔ��u���b�N�̃f�X�g���N�^
//=============================================================================
CWoodBoxBlock::~CWoodBoxBlock()
{
	// �Ȃ�
}
//=============================================================================
// �ؔ��u���b�N�̏���������
//=============================================================================
HRESULT CWoodBoxBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	// �ŏ��̈ʒu�����X�|�[���ʒu�ɐݒ�
	m_ResPos = GetPos();

	// ���I�ɖ߂�
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// �ؔ��u���b�N�̍X�V����
//=============================================================================
void CWoodBoxBlock::Update()
{
	CBlock::Update(); // ���ʏ���

	if (GetPos().y <= -810.0f)
	{
		// ���X�|�[������
		Respawn();
	}
}
//=============================================================================
// ���X�|�[������
//=============================================================================
void CWoodBoxBlock::Respawn(void)
{
	// ���������߂ɃL�l�}�e�B�b�N�ɂ���
	SetEditMode(true);

	// �u���b�N�̈ʒu���擾
	D3DXVECTOR3 Pos = GetPos();
	D3DXVECTOR3 Rot = GetRot();

	D3DXVECTOR3 respawnPos(m_ResPos);// ���X�|�[���ʒu
	D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);// ���������Z�b�g

	Pos = respawnPos;
	Rot = rot;

	SetPos(Pos);
	SetRot(Rot);

	// �R���C�_�[�̍X�V
	UpdateCollider();

	// ���I�ɖ߂�
	SetEditMode(false);
}
//=============================================================================
// �Z�b�g����
//=============================================================================
void CWoodBoxBlock::Set(D3DXVECTOR3 pos)
{
	// ���������߂ɃL�l�}�e�B�b�N�ɂ���
	SetEditMode(true);

	// ���u���b�N�̈ʒu���擾
	D3DXVECTOR3 Pos = GetPos();
	D3DXVECTOR3 Rot = GetRot();

	D3DXVECTOR3 targetPos(pos);			// �ݒu����ڕW�ʒu
	D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);	// ���������Z�b�g

	Pos = targetPos;
	Rot = rot;

	SetPos(Pos);
	SetRot(Rot);

	// �R���C�_�[�̍X�V
	UpdateCollider();
}


//=============================================================================
// �n��u���b�N�̃R���X�g���N�^
//=============================================================================
CLavaBlock::CLavaBlock()
{
	// �l�̃N���A
}
//=============================================================================
// �n��u���b�N�̃f�X�g���N�^
//=============================================================================
CLavaBlock::~CLavaBlock()
{
	// �Ȃ�
}
//=============================================================================
// �n��u���b�N�̍X�V����
//=============================================================================
void CLavaBlock::Update(void)
{
	CBlock::Update();// ���ʏ���

	// �u���b�N AABB ���擾
	D3DXVECTOR3 blockPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize(); // �u���b�N���̃T�C�Y�i���S���_�j
	D3DXVECTOR3 scale = GetSize();// �g�嗦

	// �g�嗦��K�p����
	D3DXVECTOR3 blockSize;
	blockSize.x = modelSize.x * scale.x;
	blockSize.y = modelSize.y * scale.y;
	blockSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 blockMin = blockPos - blockSize * 0.5f;
	D3DXVECTOR3 blockMax = blockPos + blockSize * 0.5f;

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block == this || block->GetType() != TYPE_KEY)
		{
			continue; // ���� or �w��u���b�N�ȊO�͖���
		}

		CKeyBlock* pKey = dynamic_cast<CKeyBlock*>(block);

		// �u���b�N�� AABB ���擾
		D3DXVECTOR3 pos = block->GetPos();
		D3DXVECTOR3 size = block->GetModelSize();
		D3DXVECTOR3 min = pos - size * 0.5f;
		D3DXVECTOR3 max = pos + size * 0.5f;

		// AABB���m�̌����`�F�b�N
		bool isOverlap =
			blockMin.x <= max.x && blockMax.x >= min.x &&
			blockMin.y <= max.y && blockMax.y >= min.y &&
			blockMin.z <= max.z && blockMax.z >= min.z;

		if (isOverlap)
		{// �������Ă���
			// ���X�|�[��
			Respawn(pKey->GetResPos());
		}
	}

	// --- �v���C���[�ڐG���� ---
	CPlayer* pPlayer = CGame::GetPlayer();

	if (pPlayer)
	{
		D3DXVECTOR3 pPos = pPlayer->GetColliderPos(); // �J�v�Z���R���C�_�[���S�ʒu

		// �J�v�Z���R���C�_�[�̃T�C�Y����AABB�T�C�Y���v�Z
		float radius = pPlayer->GetRadius();
		float height = pPlayer->GetHeight();

		D3DXVECTOR3 pSize;
		pSize.x = radius * 2.0f;
		pSize.z = radius * 2.0f;
		pSize.y = height + radius * 2.0f;

		// AABB�v�Z
		D3DXVECTOR3 pMin = pPos - pSize * 0.5f;
		D3DXVECTOR3 pMax = pPos + pSize * 0.5f;

		bool isOverlap =
			blockMin.x <= pMax.x && blockMax.x >= pMin.x &&
			blockMin.y <= pMax.y && blockMax.y >= pMin.y &&
			blockMin.z <= pMax.z && blockMax.z >= pMin.z;

		if (isOverlap)
		{
			// ���X�|�[������
			pPlayer->RespawnToCheckpoint();
		}
	}
}


//=============================================================================
// ���u���b�N�̃R���X�g���N�^
//=============================================================================
CWaterBlock::CWaterBlock()
{
	// �l�̃N���A
	m_waterStayTime = 0;				// �����؍ݎ��ԁi�b�j
	m_isInWater = false;				// �������ɂ��邩
	m_bWasInWater = false;				// ���ɓ�������
	m_isRespawn = true;					// ���X�|�[����Ԃ�
}
//=============================================================================
// ���u���b�N�̃f�X�g���N�^
//=============================================================================
CWaterBlock::~CWaterBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���u���b�N�̍X�V����
//=============================================================================
void CWaterBlock::Update(void)
{
	ApplyToBlocks();// �u���b�N�𕂂�����
	ApplyToPlayer();// �v���C���[�𕂂�����

	CBlock::Update();// ���ʏ���
}
//=============================================================================
// ���u���b�N�̃u���b�N���V����
//=============================================================================
void CWaterBlock::ApplyToBlocks(void)
{
	// ���� AABB ���擾
	D3DXVECTOR3 wtPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize();
	D3DXVECTOR3 scale = GetSize();

	D3DXVECTOR3 wtSize;

	// �g�嗦��K�p����
	wtSize.x = modelSize.x * scale.x;
	wtSize.y = modelSize.y * scale.y;
	wtSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 wtMin = wtPos - wtSize * 0.5f;
	D3DXVECTOR3 wtMax = wtPos + wtSize * 0.5f;

	// �u���b�N�̕��͐ݒ�
	const float B_maxLiftSpeed = 40.0f;

	// ------------------------
	// ���̃u���b�N�𕂂�����
	// ------------------------
	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block == this || !block->IsDynamicBlock())
		{
			continue; // ���� or �ÓI�u���b�N�͖���
		}

		// �u���b�N�� AABB ���擾
		D3DXVECTOR3 pos = block->GetPos();
		D3DXVECTOR3 size = block->GetModelSize();
		D3DXVECTOR3 min = pos - size * 0.5f;
		D3DXVECTOR3 max = pos + size * 0.5f;

		// AABB���m�̌����`�F�b�N
		bool isOverlap =
			wtMin.x <= max.x && wtMax.x >= min.x &&
			wtMin.y <= max.y && wtMax.y >= min.y &&
			wtMin.z <= max.z && wtMax.z >= min.z;

		if (!isOverlap)
		{// �������ĂȂ�������
			continue;
		}

		CWoodBoxBlock* pWoodBox = dynamic_cast<CWoodBoxBlock*>(block);

		// �ؔ��u���b�N��������
		if (block->GetType() == TYPE_WOODBOX && m_isRespawn)
		{
			// ���X�|�[������
			pWoodBox->Respawn();
		}

		btRigidBody* pRigid = block->GetRigidBody();

		if (!pRigid || !block->IsDynamicBlock())
		{
			continue;
		}

		btVector3 velocity = pRigid->getLinearVelocity();

		if (velocity.getY() < B_maxLiftSpeed)
		{
			// �����΂���ڕW��������x
			const float targetUpSpeed = 125.0f; // ����X�s�[�h
			const float maxUpSpeed = 120.0f;    // ������x
			const float forceScale = 0.12f;		// �����ɂ�����W���i�����̑����j

			btVector3 velocity = pRigid->getLinearVelocity();

			// �����ł̖��C�i�����������j
			velocity.setX(velocity.getX() * 0.5f);
			velocity.setZ(velocity.getZ() * 0.5f);

			// ���́F���݂�Y���x�Ƃ̍���␳
			float diffY = targetUpSpeed - velocity.getY();
			velocity.setY(velocity.getY() + diffY * forceScale);

			// �ő�㏸���x����
			if (velocity.getY() > maxUpSpeed)
			{
				velocity.setY(maxUpSpeed);
			}

			pRigid->setLinearVelocity(velocity);

			// �����ł͉�]���~�߂Ĉ��肳����
			pRigid->setAngularVelocity(btVector3(0, 0, 0));
		}
	}
}
//=============================================================================
// ���u���b�N�̃v���C���[���V����
//============================================================================
void CWaterBlock::ApplyToPlayer(void)
{
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	CParticle* pParticle = NULL;

	// ���� AABB ���擾
	D3DXVECTOR3 wtPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize();
	D3DXVECTOR3 scale = GetSize();

	D3DXVECTOR3 wtSize;

	// �g�嗦��K�p����
	wtSize.x = modelSize.x * scale.x;
	wtSize.y = modelSize.y * scale.y;
	wtSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 wtMin = wtPos - wtSize * 0.5f;
	D3DXVECTOR3 wtMax = wtPos + wtSize * 0.5f;

	// �v���C���[�̕��͐ݒ�
	const float P_waterLiftPower = 18.0f;
	const float P_maxLiftSpeed = 53.0f;

	// ------------------------
	// �v���C���[�𕂂�����
	// ------------------------
	CPlayer* pPlayer = CGame::GetPlayer();

	if (pPlayer)
	{
		D3DXVECTOR3 pPos = pPlayer->GetColliderPos(); // �J�v�Z���R���C�_�[���S�ʒu
		pPos.y += 40.0f;

		// �J�v�Z���R���C�_�[�̃T�C�Y����AABB�T�C�Y���v�Z
		float radius = pPlayer->GetRadius();
		float height = pPlayer->GetHeight();

		D3DXVECTOR3 pSize;
		pSize.x = radius * 2.0f;
		pSize.z = radius * 2.0f;
		pSize.y = height + radius * 2.0f;

		// AABB�v�Z
		D3DXVECTOR3 pMin = pPos - pSize * 0.5f;
		D3DXVECTOR3 pMax = pPos + pSize * 0.5f;

		bool isOverlap =
			wtMin.x <= pMax.x && wtMax.x >= pMin.x &&
			wtMin.y <= pMax.y && wtMax.y >= pMin.y &&
			wtMin.z <= pMax.z && wtMax.z >= pMin.z;

		// === �����u�Ԕ��� ===
		bool isNowInWater = isOverlap;

		if (isNowInWater && !m_bWasInWater)
		{
			// �v���C���[�̈ʒu
			D3DXVECTOR3 pos = pPlayer->GetPos();
			pos.y += 80.0f;

			// ����SE
			CManager::GetSound()->Play(CSound::SOUND_LABEL_WATER);

			// �����Ԃ��p�[�e�B�N������
			pParticle = CParticle::Create(INIT_VEC3, pos, D3DXCOLOR(0.3f, 0.6f, 1.0f, 0.8f), 50, CParticle::TYPE_WATER, 10);
			pParticle = CParticle::Create(INIT_VEC3, pos, D3DXCOLOR(0.3f, 0.5f, 1.0f, 0.5f), 50, CParticle::TYPE_WATER, 10);
		}

		// �v���C���[�����ɓ����Ă��������L�^
		m_bWasInWater = isNowInWater;

		if (!isOverlap)
		{// �������ĂȂ�������
			return;
		}

		btRigidBody* pRigid = pPlayer->GetRigidBody();

		if (pRigid)
		{
			btVector3 velocity = pRigid->getLinearVelocity();

			if (velocity.getY() < P_maxLiftSpeed)
			{
				velocity.setY(velocity.getY() + P_waterLiftPower);
				pRigid->setLinearVelocity(velocity);
			}

			// �����ɂ���t���OON
			SetInWater(true); // �t���O�p�֐�

			// �^�C�}�[�X�V
			AddWaterStayTime();
		}
		else
		{
			SetInWater(false);
			ResetWaterStayTime();
		}
	}
}
//=============================================================================
// �����ɂ���t���O�ݒ菈��
//=============================================================================
void CWaterBlock::SetInWater(bool flag)
{
	m_isInWater = flag;
}
//=============================================================================
// �����؍ݎ��Ԃ̐ݒ�
//=============================================================================
void CWaterBlock::AddWaterStayTime(void)
{
	if (m_isInWater)
	{
		m_waterStayTime++;

		if (m_waterStayTime >= 180) // 3�b�ȏ�؍݂�����
		{
			CPlayer* pPlayer = CGame::GetPlayer();
			if (!pPlayer)
			{
				return;
			}

			pPlayer->RespawnToCheckpoint(D3DXVECTOR3(427.0f, 30.0f, 1110.0f)); // ���X�|�[��
			m_waterStayTime = 0;
		}
	}
}
//=============================================================================
// �����؍ݎ��Ԃ̃��Z�b�g
//=============================================================================
void CWaterBlock::ResetWaterStayTime(void)
{
	m_waterStayTime = 0;
}


//=============================================================================
// �h�A�u���b�N�̃R���X�g���N�^
//=============================================================================
CDoorBlock::CDoorBlock()
{
	// �l�̃N���A
	m_isDoorOpened = false;
	m_initialPos = INIT_VEC3;
}
//=============================================================================
// �h�A�u���b�N�̃f�X�g���N�^
//=============================================================================
CDoorBlock::~CDoorBlock()
{
	// �Ȃ�
}
//=============================================================================
// �h�A�u���b�N�̏���������
//=============================================================================
HRESULT CDoorBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	m_initialPos = GetPos();

	return S_OK;
}
//=============================================================================
// �h�A�u���b�N�̍X�V����
//=============================================================================
void CDoorBlock::Update(void)
{
	if (CManager::GetMode() != MODE_GAME || !CGame::GetPlayer() || IsEditMode())
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();
	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 330.0f;// �������鋗��
	const float kOpenHeight = 210.0f;       // �ǂꂭ�炢�J����

	if (distance < kTriggerDistance && !m_isDoorOpened)
	{
		m_isDoorOpened = true;	// �t���OON
	}
	else if (distance >= kTriggerDistance && m_isDoorOpened)
	{
		m_isDoorOpened = false;
	}

	// ���݈ʒu
	D3DXVECTOR3 newPos = GetPos();

	float targetY = m_isDoorOpened
		? m_initialPos.y + kOpenHeight // �J�����ʒu
		: m_initialPos.y;              // �����ʒu

	// �X���[�Y�Ɉړ�
	if (fabs(newPos.y - targetY) > 0.01f)
	{
		float moveSpeed = 1.0f;

		if (newPos.y < targetY)
		{
			newPos.y = min(newPos.y + moveSpeed, targetY);
		}
		else
		{
			newPos.y = max(newPos.y - moveSpeed, targetY);
		}

		SetPos(newPos);
	}

	CBlock::Update(); // ���ʏ���
}


//=============================================================================
// �ŏI�G���A�h�A�u���b�N�̃R���X�g���N�^
//=============================================================================
CBigDoorBlock::CBigDoorBlock()
{
	// �l�̃N���A
	m_isDoorOpened = false;
}
//=============================================================================
// �ŏI�G���A�h�A�u���b�N�̃f�X�g���N�^
//=============================================================================
CBigDoorBlock::~CBigDoorBlock()
{
	// �Ȃ�
}
//=============================================================================
// �ŏI�G���A�h�A�u���b�N�̏���������
//=============================================================================
HRESULT CBigDoorBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	// �ŏ��̈ʒu��ۑ�
	m_openPos = GetPos();

	return S_OK;
}
//=============================================================================
// �ŏI�G���A�h�A�u���b�N�̍X�V����
//=============================================================================
void CBigDoorBlock::Update(void)
{
	CBlock::Update();// ���ʏ���

	if (!m_isDoorOpened)
	{
		// �^�[�Q�b�g�u���b�N��T��
		for (CBlock* block : CBlockManager::GetAllBlocks())
		{
			if (block->GetType() != TYPE_TARGET)
			{
				continue;
			}

			CTargetBlock* pTarget = dynamic_cast<CTargetBlock*>(block);

			// �������Ă�����
			if (pTarget && pTarget->IsHit())
			{
				m_isDoorOpened = true;
				break;
			}

		}
	}

	if (m_isDoorOpened)
	{
		// �h�A���J����
		const float kOpenRange = 250.0f; // �J������

		float targetY = m_openPos.y + kOpenRange;

		D3DXVECTOR3 pos = GetPos();
		if (pos.y < targetY)
		{
			pos.y += 1.0f;
			SetPos(pos);
		}
	}
}


//=============================================================================
// �X�C�b�`�u���b�N�̃R���X�g���N�^
//=============================================================================
CSwitchBlock::CSwitchBlock()
{
	// �l�̃N���A
	m_closedPos = INIT_VEC3;
	m_isSwitchOn = false;
	m_prevSwitchOn = false;
}
//=============================================================================
// �X�C�b�`�u���b�N�̃f�X�g���N�^
//=============================================================================
CSwitchBlock::~CSwitchBlock()
{
	// �Ȃ�
}
//=============================================================================
// �X�C�b�`�u���b�N�̏���������
//=============================================================================
HRESULT CSwitchBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	m_closedPos = GetPos();

	return S_OK;
}
//=============================================================================
// �X�C�b�`�u���b�N�̍X�V����
//=============================================================================
void CSwitchBlock::Update(void)
{
	CBlock::Update(); // ���ʏ���

	// �X�C�b�`�� AABB ���擾
	D3DXVECTOR3 swPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize(); // �X�C�b�`�̌��̃T�C�Y�i���S���_�j
	D3DXVECTOR3 scale = GetSize();// �g�嗦

	// �g�嗦��K�p����
	D3DXVECTOR3 swSize;
	swSize.x = modelSize.x * scale.x;
	swSize.y = modelSize.y * scale.y;
	swSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 swMin = swPos - swSize * 0.5f;
	D3DXVECTOR3 swMax = swPos + swSize * 0.5f;

	float totalMass = 0.0f;

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block == this || !block->IsDynamicBlock() || block->GetType() == TYPE_ROCK)
		{
			continue; // ���� or �ÓI�u���b�N�͖���
		}

		// �u���b�N�� AABB ���擾
		D3DXVECTOR3 pos = block->GetPos();
		D3DXVECTOR3 size = block->GetModelSize();
		D3DXVECTOR3 min = pos - size * 0.5f;
		D3DXVECTOR3 max = pos + size * 0.5f;

		// AABB���m�̌����`�F�b�N
		bool isOverlap =
			swMin.x <= max.x && swMax.x >= min.x &&
			swMin.y <= max.y && swMax.y >= min.y &&
			swMin.z <= max.z && swMax.z >= min.z;

		if (isOverlap)
		{
			btScalar invMass = block->GetRigidBody()->getInvMass();
			float mass = (invMass == 0.0f) ? 0.0f : 1.0f / invMass;
			totalMass += mass;
		}
	}

	// ���ʂ̂������l�𒴂��Ă����璾��
	const float massThreshold = 12.0f;

	if (totalMass >= massThreshold)
	{
		m_isSwitchOn = true;
	}

	if (!m_isSwitchOn)
	{
		return;
	}

	// ������Ă��� �� �����ʒu���班��������
	const float kPressDepth = 8.0f; // ������[��

	float targetY = m_closedPos.y - kPressDepth;

	D3DXVECTOR3 pos = GetPos();
	if (pos.y > targetY)
	{
		pos.y = max(pos.y - 1.0f, targetY);
		SetPos(pos);
	}

	SetEditMode(true); // ���������߂ɃL�l�}�e�B�b�N

	bool n = m_isSwitchOn;

	if (n && !m_prevSwitchOn)
	{
		// �X�C�b�`SE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_SWITCH);

		// ���ʏ㏸SE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_WATERRISE);

		// ���o�J�����ɂ���
		CManager::GetCamera()->SetCamMode(5, D3DXVECTOR3(900.5f, 214.0f, 530.5f),
			D3DXVECTOR3(1120.0f, -27.0f, 670.0f),
			D3DXVECTOR3(0.75f, -2.15f, 0.0f));
	}

	// �t���O���X�V���Ď��̃t���[���ɔ�����
	m_prevSwitchOn = n;

	// ���ʏ㏸����
	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_WATER)
		{
			continue;
		}

		D3DXVECTOR3 waterPos = block->GetPos();
		CWaterBlock* pWater = dynamic_cast<CWaterBlock*>(block);

		if (waterPos.y < -280.0f)
		{
			// ���X�ɐ��ʂ��グ��
			waterPos.y += 0.5f;
			block->SetPos(waterPos);
		}
		else
		{
			pWater->IsRespawn(false);
			m_isSwitchOn = false;
		}
	}

}


//=============================================================================
// ������u���b�N�̃R���X�g���N�^
//=============================================================================
CBridgeSwitchBlock::CBridgeSwitchBlock()
{
	// �l�̃N���A
	m_closedPos = INIT_VEC3;
	m_isSwitchOn = false;
	m_prevSwitchOn = false;
	m_OnCnt = 120;
	m_isSinkOn = false;
	m_prevSinkOn = false;
}
//=============================================================================
// ������u���b�N�̃f�X�g���N�^
//=============================================================================
CBridgeSwitchBlock::~CBridgeSwitchBlock()
{
	// �Ȃ�
}
//=============================================================================
// ������u���b�N�̏���������
//=============================================================================
HRESULT CBridgeSwitchBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	m_closedPos = GetPos();

	return S_OK;
}
//=============================================================================
// ������u���b�N�̍X�V����
//=============================================================================
void CBridgeSwitchBlock::Update(void)
{
	CBlock::Update(); // ���ʏ���

	// �X�C�b�`�� AABB ���擾
	D3DXVECTOR3 swPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize(); // �X�C�b�`�̌��̃T�C�Y�i���S���_�j
	D3DXVECTOR3 scale = GetSize();// �g�嗦

	// �g�嗦��K�p����
	D3DXVECTOR3 swSize;
	swSize.x = modelSize.x * scale.x;
	swSize.y = modelSize.y * scale.y;
	swSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 swMin = swPos - swSize * 0.5f;
	D3DXVECTOR3 swMax = swPos + swSize * 0.5f;

	float totalMass = 0.0f;

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() == TYPE_MASSBLOCK_RED || block->GetType() == TYPE_MASSBLOCK_BLUE || 
			block->GetType() == TYPE_MASSBLOCK_YELLOW || block->GetType() == TYPE_MASSBLOCK_GREEN)
		{
			// �u���b�N�� AABB ���擾
			D3DXVECTOR3 pos = block->GetPos();
			D3DXVECTOR3 size = block->GetModelSize();
			D3DXVECTOR3 min = pos - size * 0.5f;
			D3DXVECTOR3 max = pos + size * 0.5f;

			// AABB���m�̌����`�F�b�N
			bool isOverlap =
				swMin.x <= max.x && swMax.x >= min.x &&
				swMin.y <= max.y && swMax.y >= min.y &&
				swMin.z <= max.z && swMax.z >= min.z;

			if (isOverlap)
			{
				// ��_�̃u���b�N����ςݏグ�����ċA�I�ɒ��ׂ�
				totalMass += CalcStackMass(block);
			}
		}
	}

	// ���ʂ̂������l�ɂȂ����璾��
	const float massThreshold = 10.0f;

	if (totalMass == massThreshold)
	{
		m_isSinkOn = true;
	}

	bool b = m_isSinkOn;

	if (b && !m_prevSinkOn)
	{
		// �X�C�b�`SE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_SWITCH);
	}

	m_prevSinkOn = b;

	if (m_isSinkOn)
	{
		m_OnCnt--;

		if (m_OnCnt < 0)
		{
			m_OnCnt = 0;
			m_isSwitchOn = true;
		}

		// ������Ă��� �� �����ʒu���班��������
		const float kPressDepth = 10.0f; // ������[��

		float targetY = m_closedPos.y - kPressDepth;

		D3DXVECTOR3 pos = GetPos();
		if (pos.y > targetY)
		{
			pos.y = max(pos.y - 1.0f, targetY);
			SetPos(pos);
		}
	}

	bool n = m_isSwitchOn;

	if (n && !m_prevSwitchOn) // ��񂾂����s
	{
		// ���o�J�����ɂ���
		CManager::GetCamera()->SetCamMode(9, D3DXVECTOR3(-1270.0f, 370.0f, -4382.0f),
			D3DXVECTOR3(-1527.0f, 194.0f, -4085.0f),
			D3DXVECTOR3(0.43f, 0.23f, 0.0f));

		for (CBlock* block : CBlockManager::GetAllBlocks())
		{
			if (block->GetType() != TYPE_ROCK)
			{
				continue;
			}

			CRockBlock* pRock = dynamic_cast<CRockBlock*>(block);
			if (pRock)
			{
				pRock->UseBridgeSwitch(true);
				pRock->Respawn();
			}

		}
	}

	// �t���O���X�V���Ď��̃t���[���ɔ�����
	m_prevSwitchOn = n;
}
//=============================================================================
// �ςݏグ���ʌv�Z
//=============================================================================
float CBridgeSwitchBlock::CalcStackMass(CBlock* base)
{
	// �u���b�N�̎��ʂ̎擾
	float mass = base->GetMass();

	for (CBlock* other : CBlockManager::GetAllBlocks())
	{
		if (other == base)
		{
			continue;
		}

		// other �� base �̏�ɕ����I�ɏ���Ă��邩�ǂ�������
		if (IsOnTop(base, other))
		{
			mass += CalcStackMass(other);
			return mass;
		}
	}
	return 0.0f;
}
//=============================================================================
// base�̏��other�����邩����
//=============================================================================
bool CBridgeSwitchBlock::IsOnTop(CBlock* base, CBlock* other)
{
	// ���f���{���̃T�C�Y
	D3DXVECTOR3 baseModelSize = base->GetModelSize();
	D3DXVECTOR3 otherModelSize = other->GetModelSize();

	// �g�嗦
	D3DXVECTOR3 baseScale = base->GetSize();
	D3DXVECTOR3 otherScale = other->GetSize();

	// ���ۂ̑傫���i�X�P�[�����O��j
	D3DXVECTOR3 baseSize = D3DXVECTOR3(baseModelSize.x * baseScale.x,
		baseModelSize.y * baseScale.y,
		baseModelSize.z * baseScale.z);

	D3DXVECTOR3 otherSize = D3DXVECTOR3(otherModelSize.x * otherScale.x,
		otherModelSize.y * otherScale.y,
		otherModelSize.z * otherScale.z);

	D3DXVECTOR3 baseMin = base->GetPos() - baseSize * 0.5f;
	D3DXVECTOR3 baseMax = base->GetPos() + baseSize * 0.5f;

	D3DXVECTOR3 otherMin = other->GetPos() - otherSize * 0.5f;
	D3DXVECTOR3 otherMax = other->GetPos() + otherSize * 0.5f;

	// --- XZ ���d�Ȃ��Ă��邩 ---
	bool overlapXZ =
		(baseMin.x <= otherMax.x && baseMax.x >= otherMin.x) &&
		(baseMin.z <= otherMax.z && baseMax.z >= otherMin.z);

	if (!overlapXZ) return false;

	// --- other �� base �̏�ɂ��邩 (�����������ԋ��e) ---
	float epsilon = 1.0f;
	if (otherMin.y >= baseMax.y - epsilon)
	{
		return true;
	}

	return false;
}


//=============================================================================
// �i�q����u���b�N�̃R���X�g���N�^
//=============================================================================
CBarSwitchBlock::CBarSwitchBlock()
{
	// �l�̃N���A
	m_closedPos = INIT_VEC3;
	m_isSwitchOn = false;
	m_prevSwitchOn = false;
	m_timerCnt = 0;
	m_Timer = 0;
}
//=============================================================================
// �i�q����u���b�N�̃f�X�g���N�^
//=============================================================================
CBarSwitchBlock::~CBarSwitchBlock()
{
	// �Ȃ�
}
//=============================================================================
// �i�q����u���b�N�̏���������
//=============================================================================
HRESULT CBarSwitchBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	m_closedPos = GetPos();

	return S_OK;
}
//=============================================================================
// �i�q����u���b�N�̍X�V����
//=============================================================================
void CBarSwitchBlock::Update(void)
{
	CBlock::Update(); // ���ʏ���

	// �X�C�b�`�� AABB ���擾
	D3DXVECTOR3 swPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize(); // �X�C�b�`�̌��̃T�C�Y�i���S���_�j
	D3DXVECTOR3 scale = GetSize();// �g�嗦

	// �g�嗦��K�p����
	D3DXVECTOR3 swSize;
	swSize.x = modelSize.x * scale.x;
	swSize.y = modelSize.y * scale.y;
	swSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 swMin = swPos - swSize * 0.5f;
	D3DXVECTOR3 swMax = swPos + swSize * 0.5f;

	// --- �X�C�b�`��������i�v���C���[�ڐG���j ---
	CPlayer* pPlayer = CGame::GetPlayer();

	if (pPlayer)
	{
		D3DXVECTOR3 pPos = pPlayer->GetColliderPos(); // �J�v�Z���R���C�_�[���S�ʒu

		// �J�v�Z���R���C�_�[�̃T�C�Y����AABB�T�C�Y���v�Z
		float radius = pPlayer->GetRadius();
		float height = pPlayer->GetHeight();

		D3DXVECTOR3 pSize;
		pSize.x = radius * 2.0f;
		pSize.z = radius * 2.0f;
		pSize.y = height + radius * 2.0f;

		// AABB�v�Z
		D3DXVECTOR3 pMin = pPos - pSize * 0.5f;
		D3DXVECTOR3 pMax = pPos + pSize * 0.5f;

		bool isOverlap =
			swMin.x <= pMax.x && swMax.x >= pMin.x &&
			swMin.y <= pMax.y && swMax.y >= pMin.y &&
			swMin.z <= pMax.z && swMax.z >= pMin.z;

		// ����X�C�b�`�����݂��邩�m�F
		std::vector<CBlock*> blocks = CBlockManager::GetAllBlocks();

		for (CBlock* block : blocks)
		{
			if (block->GetType() != TYPE_BRIDGE3)
			{
				continue;
			}

			CFootingBlock* pFooting = dynamic_cast<CFootingBlock*>(block);

			if (isOverlap && !pFooting->GetMove())
			{
				m_isSwitchOn = true;

				// �^�C����ݒ�
				SetTimer(12);

				// ���o�J������ON�ɂ���
				CManager::GetCamera()->IsDirection(true);
			}
		}
	}

	if (!m_isSwitchOn)
	{
		return;
	}

	if (m_isSwitchOn && !CManager::GetCamera()->GetDirection())
	{
		if (m_timerCnt == 0)
		{
			// �^�C�}�[SE
			CManager::GetSound()->Play(CSound::SOUND_LABEL_TIMER);
		}

		m_timerCnt++;

		// ������Ă��� �� �����ʒu���班��������
		const float kPressDepth = 8.0f; // ������[��

		float targetY = m_closedPos.y - kPressDepth;

		D3DXVECTOR3 pos = GetPos();
		if (pos.y > targetY)
		{
			pos.y = max(pos.y - 1.0f, targetY);
			SetPos(pos);
		}

		if (m_timerCnt >= m_Timer)
		{// �w�莞�Ԃ��o�߂�����
			m_isSwitchOn = false;
			m_timerCnt = 0;

			SetPos(m_closedPos); // ���̍����ɖ߂�
		}
	}

	bool n = m_isSwitchOn;

	if (n && !m_prevSwitchOn) // ��񂾂����s
	{
		// �X�C�b�`SE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_SWITCH);

		// ���o�J�����ɂ���
		CManager::GetCamera()->SetCamMode(3, D3DXVECTOR3(2572.5f, 218.7f, -76.0f),
			D3DXVECTOR3(3158.0f, -29.0f, -562.0f),
			D3DXVECTOR3(0.32f, -0.90f, 0.0f));
	}

	// �t���O���X�V���Ď��̃t���[���ɔ�����
	m_prevSwitchOn = n;
}


//=============================================================================
// ���u���b�N�̃R���X�g���N�^
//=============================================================================
CAxeBlock::CAxeBlock()
{
	// �l�̃N���A
	m_nSwingCounter = 0;					// �t���[���J�E���^�[
	m_swingAmplitude = D3DXToRadian(75.0f);	// �}�U��p
	m_swingPeriod = 300.0f;					// �����t���[��
	m_prevSwingAngle = 0.0f;				// �O��̃X�C���O�p�x
	m_wasPositive = true;					// �O��̊p�x�������ǂ���
	m_playedSoundID = -1;					// �Đ����̉�ID
}
//=============================================================================
// ���u���b�N�̃f�X�g���N�^
//=============================================================================
CAxeBlock::~CAxeBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���u���b�N�̍X�V����
//=============================================================================
void CAxeBlock::Update(void)
{
	CBlock::Update();// ���ʏ���

	if (!CGame::GetPlayer())
	{
		return;
	}

	Swing();	// �X�C���O����

	IsPlayerHit();// �v���C���[�Ƃ̐ڐG����
}
//=============================================================================
// ���u���b�N�̃X�C���O����
//=============================================================================
void CAxeBlock::Swing(void)
{
	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 1350.0f; // ��������

	if (distance > kTriggerDistance)
	{
		return;
	}

	m_nSwingCounter++;

	float angle = m_swingAmplitude * sinf((2.0f * D3DX_PI * m_nSwingCounter) / m_swingPeriod);

	SetRot(D3DXVECTOR3(0.0f, 0.0f, angle)); // Z���X�C���O

	// �I�t�Z�b�g
	D3DXVECTOR3 localOffset(0.0f, -100.0f, 0.0f);
	D3DXVECTOR3 worldOffset;

	// �u���b�N�̃��[���h�}�g���b�N�X���擾
	D3DXMATRIX worldMtx = GetWorldMatrix();

	D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

	const float soundDistance = 1350.0f; // ��������

	if (distance < soundDistance)
	{
		// �ɒl�ʉߌ��o�F������ or ������ �ŃX�C���OSE��炷
		bool isNowPositive = (angle >= 0.0f);

		if (m_wasPositive != isNowPositive) // �Đ����Ă��Ȃ���΍Đ��J�n
		{
			// �O�̉����~�߂�i�O�̂��߁j
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3D�T�E���h�Đ�����ID��ێ�
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_SWING, GetPos(), 150.0f, kTriggerDistance);
		}

		m_wasPositive = isNowPositive;
	}
	else
	{
		// ���ꂽ�特��~����ID���Z�b�g
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	m_prevSwingAngle = angle;

	// �����̈ʒu�X�V�i�Đ����̂݁j
	if (m_playedSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, worldOffset);
	}
}
//=============================================================================
// �v���C���[�Ƃ̐ڐG���菈��
//=============================================================================
void CAxeBlock::IsPlayerHit(void)
{
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	CPlayer* pPlayer = CGame::GetPlayer();

	btRigidBody* pPlayerBody = pPlayer->GetRigidBody();
	btRigidBody* pAxeBody = GetRigidBody();

	if (!pPlayerBody || !pAxeBody)
	{
		return;
	}

	struct HitResultCallback : public btCollisionWorld::ContactResultCallback
	{
		bool isHit = false;

		btScalar addSingleResult(btManifoldPoint& cp,
			const btCollisionObjectWrapper* colObj0, int partId0, int index0,
			const btCollisionObjectWrapper* colObj1, int partId1, int index1) override
		{
			isHit = true;
			return 0;
		}
	};

	HitResultCallback callback;

	// contactTest ���g���ĕЕ��̍��̂����`�F�b�N
	CManager::GetPhysicsWorld()->contactPairTest(pPlayerBody, pAxeBody, callback);

	bool isNowHit = callback.isHit;

	if (isNowHit && !m_isPrevHit)
	{
		// �v���C���[�q�b�gSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_HIT);
	}

	// �v���C���[���������Ă��������L�^
	m_isPrevHit = isNowHit;

	if (callback.isHit)
	{
		// �v���C���[�̃��X�|�[��
		pPlayer->RespawnToCheckpoint();
	}
}


//=============================================================================
// ��u���b�N�̃R���X�g���N�^
//=============================================================================
CRockBlock::CRockBlock()
{
	// �l�̃N���A
	m_pathPoints = {};
	m_currentTargetIndex = 0;
	m_speed = 86500.0f;
	m_isBridgeSwitchOn = false;
	m_isHit = false;
	m_isPrevHit = false;
	m_isThrough = false;
	m_isPrevThrough = false;
	m_particleTimer = 0;
	m_playedRollSoundID = -1;
}
//=============================================================================
// ��u���b�N�̃f�X�g���N�^
//=============================================================================
CRockBlock::~CRockBlock()
{
	// �Ȃ�
}
//=============================================================================
// ��u���b�N�̍X�V����
//=============================================================================
void CRockBlock::Update(void)
{
	CBlock::Update();	// ���ʏ���

	if (!CGame::GetPlayer())
	{
		return;
	}

	const float RESET_HEIGHT = -480.0f;

	if (GetPos().y < RESET_HEIGHT)
	{
		Respawn();			// ���X�|�[������
	}

	MoveToTarget();		// �`�F�b�N�|�C���g�֌����Ĉړ�

	IsPlayerHit();		// �v���C���[�Ƃ̐ڐG����
}
//=============================================================================
// ���X�|�[������
//=============================================================================
void CRockBlock::Respawn(void)
{
	// �]���鉹�̒�~
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_ROLL);

	// ���������߂ɃL�l�}�e�B�b�N�ɂ���
	SetEditMode(true);

	// ��u���b�N�̈ʒu���擾
	D3DXVECTOR3 rockPos = GetPos();
	D3DXVECTOR3 rockRot = GetRot();

	D3DXVECTOR3 respawnPos(2815.5f, 700.0f, -1989.0f);// ���X�|�[���ʒu
	D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);// ���������Z�b�g

	rockPos = respawnPos;
	rockRot = rot;

	SetPos(rockPos);
	SetRot(rockRot);

	// �R���C�_�[�̍X�V
	UpdateCollider();

	// ���݂̃`�F�b�N�|�C���g�C���f�b�N�X�����Z�b�g����
	m_currentTargetIndex = 0;

	// ���I�ɖ߂�
	SetEditMode(false);
}
//=============================================================================
// �ʉ߃|�C���g�ǉ�����
//=============================================================================
void CRockBlock::AddPathPoint(const D3DXVECTOR3& point)
{
	m_pathPoints.push_back(point);
}
//=============================================================================
// �ڕW�Ɍ������Ĉړ����鏈��
//=============================================================================
void CRockBlock::MoveToTarget(void)
{
	bool allTargetHit = true;

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_TARGET)
		{
			continue;
		}

		CTargetBlock* pTarget = dynamic_cast<CTargetBlock*>(block);

		if (pTarget && !pTarget->IsHit())
		{
			allTargetHit = false;
			break;
		}
	}

	// �^�[�Q�b�g�����ׂĒB�������特���~�߂�
	if (allTargetHit && m_playedRollSoundID != -1)
	{
		CManager::GetSound()->Stop(m_playedRollSoundID);
		m_playedRollSoundID = -1;
	}

	if (m_pathPoints.empty() || m_currentTargetIndex >= (int)m_pathPoints.size())
	{
		return;
	}

	btRigidBody* pRigid = GetRigidBody();

	if (!pRigid)
	{
		return;
	}

	if (m_currentTargetIndex >= 1)
	{
		m_particleTimer++;

		if (m_particleTimer >= ROLL_PARTICLE_INTERVAL)
		{
			m_particleTimer = 0;

			// �p�[�e�B�N���̐���
			CParticle::Create(INIT_VEC3,
				D3DXVECTOR3(GetPos().x, GetPos().y - 200.0f, GetPos().z),
				D3DXCOLOR(0.4f, 0.4f, 0.4f, 0.4f),
				30,						// ����
				CParticle::TYPE_WATER,  // �p�[�e�B�N���^�C�v
				5);						// ��
		}
	}

	D3DXVECTOR3 currentPos = GetPos();
	D3DXVECTOR3 targetPos = m_pathPoints[m_currentTargetIndex];

	// �^�[�Q�b�g�����x�N�g��
	D3DXVECTOR3 dir = targetPos - currentPos;
	float dist = D3DXVec3Length(&dir);

	if (dist < 100.0f)  // ������x�߂Â����玟�̃|�C���g��
	{
		m_currentTargetIndex++;
		return;
	}

	// ���K��
	D3DXVec3Normalize(&dir, &dir);

	btVector3 force(0.0f, 0.0f, 0.0f);

	if (m_currentTargetIndex >= 1)
	{
		m_isThrough = true;
	}
	else
	{
		m_isThrough = false;
	}

	bool isThroughNow = m_isThrough;

	if (isThroughNow && !m_isPrevThrough)
	{
		// ���[�v�Đ�����ID��ۑ�
		if (m_playedRollSoundID == -1)
		{
			m_playedRollSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_ROLL, currentPos, 850.0f, 1650.0f);
		}
	}

	m_isPrevThrough = isThroughNow;


	if (m_currentTargetIndex >= 6 && !m_isBridgeSwitchOn)
	{// �C���f�b�N�X�� 6 �𒴂�����
		float fSpeedDown = 0.001f;

		// ����������
		force = btVector3(dir.x * (m_speed * fSpeedDown), 0.0f, dir.z * (m_speed * fSpeedDown));

		// �X�C�b�`���܂�������Ă��Ȃ��Ƃ������~�߂�
		if (!m_isBridgeSwitchOn && m_currentTargetIndex >= 8 && m_playedRollSoundID != -1)
		{
			// �]���鉹�̒�~
			CManager::GetSound()->Stop(m_playedRollSoundID);
			m_playedRollSoundID = -1;
		}
	}
	else
	{// �ʏ�
		// Z�����S�œ]����
		force = btVector3(dir.x * m_speed, 0.0f, dir.z * m_speed);
	}

	// �����̈ʒu�X�V�i�Đ����̂݁j
	if (m_playedRollSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedRollSoundID, currentPos);
	}

	// �K�p���̑��x�ɉ�����
	pRigid->applyCentralForce(force);
}
//=============================================================================
// �v���C���[�Ƃ̐ڐG���菈��
//=============================================================================
void CRockBlock::IsPlayerHit(void)
{
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	CPlayer* pPlayer = CGame::GetPlayer();

	D3DXVECTOR3 playerPos = pPlayer->GetPos();
	D3DXVECTOR3 rockPos = GetPos();

	float playerRadius = pPlayer->GetRadius();  // �v���C���[�̓����蔻�蔼�a
	float rockRadius = 240.0f;					// ��̔��a

	float playerHeight = pPlayer->GetHeight();	// �v���C���[�̍���
	float rockHeight = 240.0f;					// ��̍����͈�

	// XZ�����`�F�b�N
	D3DXVECTOR2 diffXZ = D3DXVECTOR2(playerPos.x - rockPos.x, playerPos.z - rockPos.z);

	float distXZSq = D3DXVec2LengthSq(&diffXZ);
	float hitDistXZ = playerRadius + rockRadius;

	// Y���`�F�b�N
	float dy = fabsf(playerPos.y - rockPos.y);
	float hitHeight = (playerHeight * 0.5f) + rockHeight;

	bool isNowHit = m_isHit;

	if (isNowHit && !m_isPrevHit)
	{
		// �v���C���[�q�b�gSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_HIT);
	}

	// �v���C���[���������Ă��������L�^
	m_isPrevHit = isNowHit;

	if (distXZSq < (hitDistXZ * hitDistXZ) && dy < hitHeight)
	{
		m_isHit = true;

		// �v���C���[�̃��X�|�[��
		pPlayer->RespawnToCheckpoint(D3DXVECTOR3(2810.0f, 30.0f, -1518.0f));
	}
	else
	{
		m_isHit = false;
	}
}


//=============================================================================
// ���u���b�N�̃R���X�g���N�^
//=============================================================================
CBridgeBlock::CBridgeBlock()
{
	// �l�̃N���A
}
//=============================================================================
// ���u���b�N�̃f�X�g���N�^
//=============================================================================
CBridgeBlock::~CBridgeBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���u���b�N�̍X�V����
//=============================================================================
void CBridgeBlock::Update(void)
{
	CBlock::Update();// ���ʏ���

	Move();
}
//=============================================================================
// ���u���b�N�̈ړ�����
//=============================================================================
void CBridgeBlock::Move(void)
{
	// ����X�C�b�`�����݂��邩�m�F
	std::vector<CBlock*> blocks = CBlockManager::GetAllBlocks();

	for (CBlock* block : blocks)
	{
		if (block->GetType() != TYPE_SWITCH2)
		{
			continue;
		}

		CBridgeSwitchBlock* pSwitch = dynamic_cast<CBridgeSwitchBlock*>(block);

		if (!pSwitch || !pSwitch->IsSwitchOn())
		{
			continue;
		}

		// ���݈ʒu�擾
		D3DXVECTOR3 pos = GetPos();

		// �ړ��ڕW���W
		const float targetX = -1630.0f;

		// �܂��ڕW�ʒu�ɒB���ĂȂ��Ȃ�ړ�
		if (pos.x > targetX)
		{
			const float speed = 2.0f; // ���x

			// ���B�`�F�b�N�F���̃t���[���Œʂ�߂��邩
			if (pos.x - speed <= targetX)
			{
				pos.x = targetX;
			}
			else
			{
				pos.x -= speed;
			}

			SetPos(pos);

			// �R���C�_�[�̍X�V
			UpdateCollider();
		}
	}
}


//=============================================================================
// �^�[�Q�b�g�u���b�N�̃R���X�g���N�^
//=============================================================================
CTargetBlock::CTargetBlock()
{
	// �l�̃N���A
	m_isHit = false;
	m_isPrevHit = false;
}
//=============================================================================
// �^�[�Q�b�g�u���b�N�̃f�X�g���N�^
//=============================================================================
CTargetBlock::~CTargetBlock()
{
	// �Ȃ�
}
//=============================================================================
// �^�[�Q�b�g�u���b�N�̍X�V����
//=============================================================================
void CTargetBlock::Update(void)
{
	CBlock::Update();// ���ʏ���

	if (m_isHit)
	{
		return; // ���łɓ������Ă�����X�L�b�v
	}

	// ������ AABB ���擾
	D3DXVECTOR3 myPos = GetPos();
	D3DXVECTOR3 mymodelSize = GetModelSize(); // ���̃T�C�Y
	D3DXVECTOR3 myscale = GetSize();// �g�嗦
	D3DXVECTOR3 mySize;

	// �g�嗦��K�p����
	mySize.x = mymodelSize.x * myscale.x;
	mySize.y = mymodelSize.y * myscale.y;
	mySize.z = mymodelSize.z * myscale.z;

	D3DXVECTOR3 myMin = myPos - mySize * 0.5f;
	D3DXVECTOR3 myMax = myPos + mySize * 0.5f;

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block == this || block->GetType() != TYPE_ROCK)
		{
			continue; // ���� or ��u���b�N�ȊO�͖���
		}

		// ��� AABB ���擾
		D3DXVECTOR3 otherPos = block->GetPos();
		D3DXVECTOR3 otherModelSize = block->GetModelSize();
		D3DXVECTOR3 otherScale = block->GetSize();
		D3DXVECTOR3 otherSize(
			otherModelSize.x * otherScale.x,
			otherModelSize.y * otherScale.y,
			otherModelSize.z * otherScale.z
		);

		D3DXVECTOR3 otherMin = otherPos - otherSize * 0.5f;
		D3DXVECTOR3 otherMax = otherPos + otherSize * 0.5f;

		// AABB���m�̌����`�F�b�N
		bool isOverlap =
			myMin.x <= otherMax.x && myMax.x >= otherMin.x &&
			myMin.y <= otherMax.y && myMax.y >= otherMin.y &&
			myMin.z <= otherMax.z && myMax.z >= otherMin.z;

		bool isNowHit = isOverlap;

		if (isNowHit && !m_isPrevHit)
		{
			// ��q�b�gSE
			CManager::GetSound()->Play(CSound::SOUND_LABEL_ROCKHIT);
		}

		m_isPrevHit = isNowHit;

		if (isOverlap)
		{
			if (!m_isPrevHit)
			{
				CManager::GetSound()->Play(CSound::SOUND_LABEL_ROCKHIT);
			}

			m_isHit = true;
			m_isPrevHit = true;
			CManager::GetSound()->Stop(CSound::SOUND_LABEL_ROLL);
			break;
		}
		else
		{
			m_isPrevHit = false;
		}
	}
}


//=============================================================================
// �Ǌ|�������u���b�N�̃R���X�g���N�^
//=============================================================================
CTorchBlock::CTorchBlock()
{
	// �l�̃N���A
	m_playedSoundID = -1;
}
//=============================================================================
// �Ǌ|�������u���b�N�̃f�X�g���N�^
//=============================================================================
CTorchBlock::~CTorchBlock()
{
	// �Ȃ�
}
//=============================================================================
// �Ǌ|�������u���b�N�̍X�V����
//=============================================================================
void CTorchBlock::Update(void)
{
	CBlock::Update();// ���ʏ���

	if (CManager::GetMode() == MODE_GAME && CGame::GetPlayer())
	{
		D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
		D3DXVECTOR3 disPos = playerPos - GetPos();

		float distance = D3DXVec3Length(&disPos);

		// �I�t�Z�b�g
		D3DXVECTOR3 localOffset(0.0f, 30.0f, -8.0f); // �����̐�[�i���[�J���j
		D3DXVECTOR3 worldOffset;

		// �u���b�N�̃��[���h�}�g���b�N�X���擾
		D3DXMATRIX worldMtx = GetWorldMatrix();

		D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

		const float kTriggerDistance = 780.0f; // ��������

		if (distance < kTriggerDistance)
		{
			// �p�[�e�B�N������
			CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 16, CParticle::TYPE_FIRE, 1);
			CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_FIRE, 1);

			if (m_playedSoundID == -1) // �Đ����Ă��Ȃ���΍Đ��J�n
			{
				// �O�̉����~�߂�i�O�̂��߁j
				CManager::GetSound()->Stop(m_playedSoundID);

				// 3D�T�E���h�Đ�����ID��ێ�
				m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
			}

		}
		else
		{
			// ���ꂽ�特��~����ID���Z�b�g
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->Stop(m_playedSoundID);
				m_playedSoundID = -1;
			}
		}

		// �����̈ʒu�X�V��ID���g��
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, worldOffset);
		}
	}

	if (CManager::GetMode() == MODE_TITLE)
	{
		// �I�t�Z�b�g
		D3DXVECTOR3 localOffset(0.0f, 30.0f, -8.0f); // �����̐�[�i���[�J���j
		D3DXVECTOR3 worldOffset;

		// �u���b�N�̃��[���h�}�g���b�N�X���擾
		D3DXMATRIX worldMtx = GetWorldMatrix();

		D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

		// �p�[�e�B�N������
		CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_FIRE, 1);
		CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_FIRE, 1);
	}
}


//=============================================================================
// �u���^�����u���b�N�̃R���X�g���N�^
//=============================================================================
CTorch2Block::CTorch2Block()
{
	// �l�̃N���A
	m_playedSoundID = -1;
}
//=============================================================================
// �u���^�����u���b�N�̃f�X�g���N�^
//=============================================================================
CTorch2Block::~CTorch2Block()
{
	// �Ȃ�
}
//=============================================================================
// �u���^�����u���b�N�̍X�V����
//=============================================================================
void CTorch2Block::Update(void)
{
	if (CManager::GetMode() != MODE_GAME || !CGame::GetPlayer())
	{
		return;
	}

	CBlock::Update(); // ���ʏ���

	CParticle* pParticle = NULL;
	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	// �I�t�Z�b�g
	D3DXVECTOR3 localOffset(0.0f, 30.0f, 0.0f); // �����̐�[�i���[�J���j
	D3DXVECTOR3 worldOffset;

	// �u���b�N�̃��[���h�}�g���b�N�X���擾
	D3DXMATRIX worldMtx = GetWorldMatrix();

	D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

	const float kTriggerDistance = 1080.0f; // ��������

	if (distance < kTriggerDistance)
	{
		// �p�[�e�B�N������
		pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_FIRE, 1);
		pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_FIRE, 1);

		if (m_playedSoundID == -1) // �Đ����Ă��Ȃ���΍Đ��J�n
		{
			// �O�̉����~�߂�i�O�̂��߁j
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3D�T�E���h�Đ�����ID��ێ�
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
		}

	}
	else
	{
		// ���ꂽ�特��~����ID���Z�b�g
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	// �����̈ʒu�X�V��ID���g��
	if (m_playedSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, worldOffset);
	}
}


//=============================================================================
// �u���^����(�����Ȃ�)�u���b�N�̃R���X�g���N�^
//=============================================================================
CTorch3Block::CTorch3Block()
{
	// �l�̃N���A
	m_playedSoundID = -1;
	m_isHit = false;
}
//=============================================================================
// �u���^����(�����Ȃ�)�u���b�N�̃f�X�g���N�^
//=============================================================================
CTorch3Block::~CTorch3Block()
{
	// �Ȃ�
}
//=============================================================================
// �u���^����(�����Ȃ�)�u���b�N�̍X�V����
//=============================================================================
void CTorch3Block::Update(void)
{
	if (CManager::GetMode() != MODE_GAME || !CGame::GetPlayer())
	{
		return;
	}

	CBlock::Update(); // ���ʏ���

	CParticle* pParticle = NULL;
	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	// �I�t�Z�b�g
	D3DXVECTOR3 localOffset(0.0f, 30.0f, 0.0f); // �����̐�[�i���[�J���j
	D3DXVECTOR3 worldOffset;

	// �u���b�N�̃��[���h�}�g���b�N�X���擾
	D3DXMATRIX worldMtx = GetWorldMatrix();

	D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

	const float kTriggerDistance = 1180.0f; // ��������

	if (distance < kTriggerDistance && m_isHit)
	{
		// �p�[�e�B�N������
		pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_FIRE, 1);
		pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_FIRE, 1);

		if (m_playedSoundID == -1) // �Đ����Ă��Ȃ���΍Đ��J�n
		{
			// �O�̉����~�߂�i�O�̂��߁j
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3D�T�E���h�Đ�����ID��ێ�
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
		}
	}
	else
	{
		// ���ꂽ�特��~����ID���Z�b�g
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	// �����̈ʒu�X�V��ID���g��
	if (m_playedSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, worldOffset);
	}
}


//=============================================================================
// ���ʃu���b�N�̃R���X�g���N�^
//=============================================================================
CMaskBlock::CMaskBlock()
{
	// �l�̃N���A
	m_isGet = false;
	m_playedSoundID = -1;
}
//=============================================================================
// ���ʃu���b�N�̃f�X�g���N�^
//=============================================================================
CMaskBlock::~CMaskBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���ʃu���b�N�̍X�V����
//=============================================================================
void CMaskBlock::Update(void)
{
	CBlock::Update(); // ���ʏ���

	if (!CGame::GetPlayer())
	{
		return;
	}

	CParticle* pParticle = NULL;

	if (CManager::GetMode() == MODE_GAME)
	{
		D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
		D3DXVECTOR3 disPos = playerPos - GetPos();

		float distance = D3DXVec3Length(&disPos);

		const float kTriggerDistance = 1100.0f; // ��������

		if (distance < kTriggerDistance && !m_isGet)
		{
			// �I�t�Z�b�g
			D3DXVECTOR3 localOffset(0.0f, 15.0f, 0.0f);
			D3DXVECTOR3 worldOffset;

			// �u���b�N�̃��[���h�}�g���b�N�X���擾
			D3DXMATRIX worldMtx = GetWorldMatrix();

			D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

			// �p�[�e�B�N������
			pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.6f, 0.6f, 1.0f, 0.3f), 50, CParticle::TYPE_AURA2, 1);

			if (m_playedSoundID == -1) // �Đ����Ă��Ȃ���΍Đ��J�n
			{
				// �O�̉����~�߂�i�O�̂��߁j
				CManager::GetSound()->Stop(m_playedSoundID);

				// 3D�T�E���h�Đ�����ID��ێ�
				m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_MASK, GetPos(), 250.0f, kTriggerDistance);
			}

			// �����̈ʒu�X�V��ID���g��
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
			}
		}
		else
		{
			// ���ꂽ�特��~����ID���Z�b�g
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->Stop(m_playedSoundID);
				m_playedSoundID = -1;
			}
		}

		const float getDistance = 200.0f; // ��������

		if (distance < getDistance)
		{
			if (!m_isGet)
			{
				// ���擾UI�̐���
				CUi::Create(CUi::TYPE_GET, "data/TEXTURE/ui_mask.png", D3DXVECTOR3(900.0f, 220.0f, 0.0f), 320.0f, 140.0f);
			}

			m_isGet = true;

			// ���X�|�[������
			CGame::GetPlayer()->RespawnToCheckpoint();
		}
	}
}


//=============================================================================
// ���u���b�N�̃R���X�g���N�^
//=============================================================================
CSwordBlock::CSwordBlock()
{
	// �l�̃N���A
	m_isEnd = false;
	m_playedSoundID = -1;
}
//=============================================================================
// ���u���b�N�̃f�X�g���N�^
//=============================================================================
CSwordBlock::~CSwordBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���u���b�N�̍X�V����
//=============================================================================
void CSwordBlock::Update(void)
{
	CBlock::Update(); // ���ʏ���

	if (!CGame::GetPlayer())
	{
		return;
	}

	CParticle* pParticle = NULL;

	if (CManager::GetMode() == MODE_GAME)
	{
		D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
		D3DXVECTOR3 disPos = playerPos - GetPos();

		float distance = D3DXVec3Length(&disPos);

		const float kTriggerDistance = 1280.0f; // ��������

		if (distance < kTriggerDistance)
		{
			// �I�t�Z�b�g
			D3DXVECTOR3 localOffset(0.0f, -60.0f, 0.0f);
			D3DXVECTOR3 worldOffset;

			// �u���b�N�̃��[���h�}�g���b�N�X���擾
			D3DXMATRIX worldMtx = GetWorldMatrix();

			D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

			// �p�[�e�B�N������
			pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.6f, 0.6f, 0.0f, 0.3f), 50, CParticle::TYPE_AURA, 1);

			if (m_playedSoundID == -1) // �Đ����Ă��Ȃ���΍Đ��J�n
			{
				// �O�̉����~�߂�i�O�̂��߁j
				CManager::GetSound()->Stop(m_playedSoundID);

				// 3D�T�E���h�Đ�����ID��ێ�
				m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_TREASURE, GetPos(), 250.0f, kTriggerDistance);
			}

			// �����̈ʒu�X�V��ID���g��
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
			}
		}
		else
		{
			// ���ꂽ�特��~����ID���Z�b�g
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->Stop(m_playedSoundID);
				m_playedSoundID = -1;
			}
		}

		const float getDistance = 250.0f; // ��������

		if (distance < getDistance)
		{// ��ɓ��ꂽ
			m_isEnd = true;
		}
	}
}


//=============================================================================
// �S�i�q�u���b�N�̃R���X�g���N�^
//=============================================================================
CBarBlock::CBarBlock()
{
	// �l�̃N���A
}
//=============================================================================
// �S�i�q�u���b�N�̃f�X�g���N�^
//=============================================================================
CBarBlock::~CBarBlock()
{
	// �Ȃ�
	m_isOpened = false;
}
//=============================================================================
// �S�i�q�u���b�N�̍X�V����
//=============================================================================
void CBarBlock::Update(void)
{
	CBlock::Update(); // ���ʏ���

	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	if (!m_isOpened)
	{
		// �S�i�q����X�C�b�`�u���b�N��T��
		for (CBlock* block : CBlockManager::GetAllBlocks())
		{
			if (block->GetType() != TYPE_SWITCH3)
			{
				continue;
			}

			CBarSwitchBlock* pBarSwitch = dynamic_cast<CBarSwitchBlock*>(block);

			// ������Ă�����
			if (pBarSwitch && pBarSwitch->IsSwitchOn())
			{
				m_isOpened = true;
				break;
			}

		}
	}

	if (m_isOpened)
	{
		// �h�A���J��
		D3DXVECTOR3 newPos = GetPos();

		if (newPos.y <= 290.0f)
		{
			newPos.y += 1.0f;
			SetPos(newPos);
		}
		else
		{
			m_isOpened = false;
		}
	}
	else
	{
		// �h�A��߂�
		D3DXVECTOR3 newPos = GetPos();

		if (newPos.y > 90.0f && !m_isOpened)
		{
			newPos.y -= 1.0f;
			SetPos(newPos);
		}
	}
}


//=============================================================================
// ����u���b�N�̃R���X�g���N�^
//=============================================================================
CFootingBlock::CFootingBlock()
{
	// �l�̃N���A
	m_isMove = false;
}
//=============================================================================
// ����u���b�N�̃f�X�g���N�^
//=============================================================================
CFootingBlock::~CFootingBlock()
{
	// �Ȃ�
}
//=============================================================================
// ����u���b�N�̍X�V����
//=============================================================================
void CFootingBlock::Update(void)
{
	CBlock::Update(); // ���ʏ���

	// ����X�C�b�`�����݂��邩�m�F
	std::vector<CBlock*> blocks = CBlockManager::GetAllBlocks();

	for (CBlock* block : blocks)
	{
		if (block->GetType() != TYPE_SWITCH3)
		{
			continue;
		}

		CBarSwitchBlock* pSwitch = dynamic_cast<CBarSwitchBlock*>(block);

		if (!pSwitch)
		{
			continue;
		}

		if (pSwitch->IsSwitchOn())
		{
			// ���݈ʒu�擾
			D3DXVECTOR3 pos = GetPos();

			// �ړ��ڕW���W
			const float targetX = 2957.0f;

			// �܂��ڕW�ʒu�ɒB���ĂȂ��Ȃ�ړ�
			if (pos.x > targetX)
			{
				const float speed = 1.0f; // ���x

				// ���B�`�F�b�N�F���̃t���[���Œʂ�߂��邩
				if (pos.x - speed <= targetX)
				{
					pos.x = targetX;
				}
				else
				{
					pos.x -= speed;
					m_isMove = true;
				}

				SetPos(pos);
			}
		}
		else
		{
			CManager::GetSound()->Stop(CSound::SOUND_LABEL_TIMER);

			// ���݈ʒu�擾
			D3DXVECTOR3 pos = GetPos();

			// �ړ��ڕW���W
			const float targetX = 3160.0f;

			// �܂��ڕW�ʒu�ɒB���ĂȂ��Ȃ�ړ�
			if (pos.x < targetX)
			{
				const float speed = 1.0f; // ���x

				// ���B�`�F�b�N�F���̃t���[���Œʂ�߂��邩
				if (pos.x - speed >= targetX)
				{
					pos.x = targetX;
					//m_isMove = false;
				}
				else
				{
					pos.x += speed;
				}

				SetPos(pos);
			}
			else
			{
				m_isMove = false;
			}
		}

		// �R���C�_�[�̍X�V
		UpdateCollider();
	}
}


//=============================================================================
// �Ή����ˑ��u���b�N�̃R���X�g���N�^
//=============================================================================
CFireStatueBlock::CFireStatueBlock()
{
	// �l�̃N���A
	m_playedSoundID = -1;
	m_isBlocked = false;
}
//=============================================================================
// �Ή����ˑ��u���b�N�̃f�X�g���N�^
//=============================================================================
CFireStatueBlock::~CFireStatueBlock()
{
	// �Ȃ�
}
//=============================================================================
// �Ή����ˑ��u���b�N�̍X�V����
//=============================================================================
void CFireStatueBlock::Update(void)
{
	CBlock::Update(); // ���ʏ���

	CPlayer* pPlayer = CGame::GetPlayer();

	if (!pPlayer)
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();
	float distance = D3DXVec3Length(&disPos);
	const float kTriggerDistance = 380.0f; // ��������

	// �u���b�N�̃��[���h�}�g���b�N�X
	D3DXMATRIX worldMtx = GetWorldMatrix();

	// ==== �����ʒu====
	D3DXVECTOR3 localOffset(0.0f, -30.0f, -40.0f); // Z�������ɃI�t�Z�b�g
	D3DXVECTOR3 worldPos;
	D3DXVec3TransformCoord(&worldPos, &localOffset, &worldMtx);

	if (distance < kTriggerDistance)
	{
		// �p�[�e�B�N���̐ݒ�
		SetParticle();

		if (m_playedSoundID == -1) // �Đ����Ă��Ȃ���΍Đ��J�n
		{
			// �O�̉����~�߂�i�O�̂��߁j
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3D�T�E���h�Đ�����ID��ێ�
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
		}
	}
	else
	{
		// ���ꂽ�特��~����ID���Z�b�g
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	// �����̈ʒu�X�V��ID���g��
	if (m_playedSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, worldPos);
	}
}
//=============================================================================
// �Ή����ˑ��u���b�N�̃p�[�e�B�N���ݒ菈��
//=============================================================================
void CFireStatueBlock::SetParticle(void)
{
	CParticle* pParticle = NULL;

	// �u���b�N�̃��[���h�}�g���b�N�X
	D3DXMATRIX worldMtx = GetWorldMatrix();

	// ==== �O�����x�N�g���iZ���j====
	D3DXVECTOR3 localForward(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 forward;
	D3DXVec3TransformNormal(&forward, &localForward, &worldMtx);
	D3DXVec3Normalize(&forward, &forward);

	// ==== ���ˈʒu�i�㑤�j====
	D3DXVECTOR3 localOffsetBack(0.0f, -30.0f, -40.0f); // Z�������ɃI�t�Z�b�g
	D3DXVECTOR3 worldPosBack;
	D3DXVec3TransformCoord(&worldPosBack, &localOffsetBack, &worldMtx);

	// ���̃V�����_�[�̔���T�C�Y
	float cylinderRadius = 60.0f;
	float maxCylinderHeight = 220.0f;

	// ����p�̒��S�ʒu�͔��ˈʒu���玲�����ɔ����ړ������Ƃ���i�V�����_�[�̒��S�j
	float offsetDistance = maxCylinderHeight * 0.5f;
	D3DXVECTOR3 cylinderCenter = worldPosBack - forward * offsetDistance;

	// �u���b�N�ɂ��Ւf����
	float minHitDistance = maxCylinderHeight; // �k�߂鋗���B�ő咷���X�^�[�g
	m_isBlocked = false;

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_BLOCK3 && block->GetType() != TYPE_BLOCK2)
		{
			continue;
		}


		D3DXVECTOR3 blockPos = block->GetPos();
		D3DXVECTOR3 modelSize = block->GetModelSize();// ���̃T�C�Y
		D3DXVECTOR3 scale = GetSize();// �g�嗦

		// �g�嗦��K�p����
		D3DXVECTOR3 blockSize;
		blockSize.x = modelSize.x * scale.x;
		blockSize.y = modelSize.y * scale.y;
		blockSize.z = modelSize.z * scale.z;

		D3DXVECTOR3 aabbMin = blockPos - blockSize * 0.5f;
		D3DXVECTOR3 aabbMax = blockPos + blockSize * 0.5f;

		// �����ŃV�����_�[�i�ő咷���j��AABB�̓����蔻������A
		// �������Ă����瓖����ʂ܂ł̋������v�Z
		float hitDistance = 0.0f;
		if (CCollision::CheckCylinderAABBCollisionWithHitDistance(cylinderCenter, cylinderRadius, maxCylinderHeight, forward, aabbMin, aabbMax, &hitDistance))
		{
			m_isBlocked = true;
			if (hitDistance < minHitDistance)
			{
				minHitDistance = hitDistance;
			}
		}
	}

	// �V�����_�[�̒������k�߂�i�ŏ��̓����苗���j
	float cylinderHeight = m_isBlocked ? minHitDistance : maxCylinderHeight;

	// ���S�ʒu������
	offsetDistance = cylinderHeight * 0.5f;
	cylinderCenter = worldPosBack - forward * offsetDistance;

	// �p�[�e�B�N�������i�k�߂������ɍ��킹�āj
	pParticle = CParticle::Create(-forward, worldPosBack, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 40, CParticle::TYPE_FLAMETHROWER, 5);

	// �p�[�e�B�N���̑��x�{���Z�b�g�i�k�߂������ő��x�𒲐��j
	if (pParticle)
	{
		auto flamethrowerParticle = dynamic_cast<CFlamethrowerParticle*>(pParticle);
		if (flamethrowerParticle)
		{
			// ���x�X�P�[���𔭐��ʒu��̋��������ŒP���v�Z
			float speedScale = clamp(minHitDistance / maxCylinderHeight, 0.0f, 1.0f);
			flamethrowerParticle->SetSpeedScale(speedScale);
		}
	}

	// �v���C���[�̓����蔻����k�߂��͈͂�
	CPlayer* pPlayer = CGame::GetPlayer();
	D3DXVECTOR3 playerPos = pPlayer->GetColliderPos();
	float playerRadius = pPlayer->GetRadius();
	float playerHeight = pPlayer->GetHeight();

	if (CCollision::CheckCapsuleCylinderCollision_Dir(playerPos, playerRadius, playerHeight,
		cylinderCenter, cylinderRadius, cylinderHeight, forward, m_isBlocked))
	{
		// ���X�|�[������
		pPlayer->RespawnToCheckpoint();
	}
}


//=============================================================================
// �Ή����ˑ�(����)�u���b�N�̃R���X�g���N�^
//=============================================================================
CMoveFireStatueBlock::CMoveFireStatueBlock()
{
	// �l�̃N���A
	m_playedSoundID = -1;
}
//=============================================================================
// �Ή����ˑ�(����)�u���b�N�̃f�X�g���N�^
//=============================================================================
CMoveFireStatueBlock::~CMoveFireStatueBlock()
{
	// �Ȃ�
}
//=============================================================================
// �Ή����ˑ�(����)�u���b�N�̍X�V����
//=============================================================================
void CMoveFireStatueBlock::Update(void)
{
	CBlock::Update(); // ���ʏ���

	CPlayer* pPlayer = CGame::GetPlayer();

	if (!pPlayer)
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();
	float distance = D3DXVec3Length(&disPos);
	const float kTriggerDistance = 880.0f; // ��������

	// �u���b�N�̃��[���h�}�g���b�N�X
	D3DXMATRIX worldMtx = GetWorldMatrix();

	// ==== �����ʒu====
	D3DXVECTOR3 localOffset(0.0f, -30.0f, -40.0f); // Z�������ɃI�t�Z�b�g
	D3DXVECTOR3 worldPos;
	D3DXVec3TransformCoord(&worldPos, &localOffset, &worldMtx);

	if (distance < kTriggerDistance)
	{
		// �p�[�e�B�N���̐ݒ�
		SetParticle();

		if (m_playedSoundID == -1) // �Đ����Ă��Ȃ���΍Đ��J�n
		{
			// �O�̉����~�߂�i�O�̂��߁j
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3D�T�E���h�Đ�����ID��ێ�
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
		}
	}
	else
	{
		// ���ꂽ�特��~����ID���Z�b�g
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	// �����̈ʒu�X�V��ID���g��
	if (m_playedSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, worldPos);
	}
}
//=============================================================================
// �Ή����ˑ�(����)�u���b�N�̃p�[�e�B�N���ݒ菈��
//=============================================================================
void CMoveFireStatueBlock::SetParticle(void)
{
	CParticle* pParticle = NULL;

	// �u���b�N�̃��[���h�}�g���b�N�X
	D3DXMATRIX worldMtx = GetWorldMatrix();

	// ==== �O�����x�N�g���iZ���j====
	D3DXVECTOR3 localForward(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 forward;
	D3DXVec3TransformNormal(&forward, &localForward, &worldMtx);
	D3DXVec3Normalize(&forward, &forward);

	// ==== ���ˈʒu�i�㑤�j====
	D3DXVECTOR3 localOffsetBack(0.0f, -30.0f, -40.0f); // Z�������ɃI�t�Z�b�g
	D3DXVECTOR3 worldPosBack;
	D3DXVec3TransformCoord(&worldPosBack, &localOffsetBack, &worldMtx);

	// ���̃V�����_�[�̔���T�C�Y
	float cylinderRadius = 60.0f;
	float maxCylinderHeight = 220.0f;

	// ����p�̒��S�ʒu�͔��ˈʒu���玲�����ɔ����ړ������Ƃ���i�V�����_�[�̒��S�j
	float offsetDistance = maxCylinderHeight * 0.5f;
	D3DXVECTOR3 cylinderCenter = worldPosBack - forward * offsetDistance;

	// �u���^����(�����Ȃ�)�Ƃ̓����蔻��
	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_TORCH3)
		{
			continue;
		}

		CTorch3Block* pTorch3 = dynamic_cast<CTorch3Block*>(block);

		D3DXVECTOR3 blockPos = block->GetPos();
		D3DXVECTOR3 blockSize = block->GetModelSize();
		D3DXVECTOR3 aabbMin = blockPos - blockSize * 0.5f;
		D3DXVECTOR3 aabbMax = blockPos + blockSize * 0.5f;

		// �������Ă�����t���O��true�ɂ���
		if (CCollision::CheckCylinderAABBCollisionWithHitDistance(cylinderCenter, cylinderRadius, maxCylinderHeight, forward, aabbMin, aabbMax, NULL))
		{
			pTorch3->SetHit(true);
		}
	}

	// �p�[�e�B�N������
	pParticle = CParticle::Create(-forward, worldPosBack, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 40, CParticle::TYPE_FLAMETHROWER, 5);

	CPlayer* pPlayer = CGame::GetPlayer();
	D3DXVECTOR3 playerPos = pPlayer->GetColliderPos();  // �J�v�Z�����S�ʒu
	float playerRadius = pPlayer->GetRadius();
	float playerHeight = pPlayer->GetHeight();

	// �v���C���[�Ƃ̓����蔻��
	if (CCollision::CheckCapsuleCylinderCollision_Dir(playerPos, playerRadius, playerHeight,
		cylinderCenter, cylinderRadius, maxCylinderHeight, forward, false))
	{
		// ���X�|�[������
		pPlayer->RespawnToCheckpoint(D3DXVECTOR3(-1132.0f, 299.5f, 724.5f));
	}
}


//=============================================================================
// �Ή����ˑ�(��])�u���b�N�̃R���X�g���N�^
//=============================================================================
CTurnFireStatueBlock::CTurnFireStatueBlock()
{
	// �l�̃N���A
	m_playedSoundID = -1;
}
//=============================================================================
// �Ή����ˑ�(��])�u���b�N�̃f�X�g���N�^
//=============================================================================
CTurnFireStatueBlock::~CTurnFireStatueBlock()
{
	// �Ȃ�
}
//=============================================================================
// �Ή����ˑ�(��])�u���b�N�̍X�V����
//=============================================================================
void CTurnFireStatueBlock::Update(void)
{
	CBlock::Update(); // ���ʏ���

	CPlayer* pPlayer = CGame::GetPlayer();

	if (!pPlayer)
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();
	float distance = D3DXVec3Length(&disPos);
	const float kTriggerDistance = 720.0f; // ��������

	if (distance < kTriggerDistance)
	{
		// �p�[�e�B�N���̐ݒ�
		SetParticle();

		if (m_playedSoundID == -1) // �Đ����Ă��Ȃ���΍Đ��J�n
		{
			// �O�̉����~�߂�i�O�̂��߁j
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3D�T�E���h�Đ�����ID��ێ�
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
		}

		D3DXVECTOR3 rot = GetRot();

		rot.y += 0.01f;

		if (rot.y > D3DX_PI * 2.0f)
		{
			rot.y -= D3DX_PI * 2.0f;
		}

		SetRot(rot);
	}
	else
	{
		// ���ꂽ�特��~����ID���Z�b�g
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	// �����̈ʒu�X�V��ID���g��
	if (m_playedSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
	}
}
//=============================================================================
// �Ή����ˑ�(��])�u���b�N�̃p�[�e�B�N���ݒ菈��
//=============================================================================
void CTurnFireStatueBlock::SetParticle(void)
{
	CParticle* pParticle = NULL;

	// �u���b�N�̃��[���h�}�g���b�N�X
	D3DXMATRIX worldMtx = GetWorldMatrix();

	// ==== �O�����x�N�g���iZ���j====
	D3DXVECTOR3 localForward(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 forward;
	D3DXVec3TransformNormal(&forward, &localForward, &worldMtx);
	D3DXVec3Normalize(&forward, &forward);

	// ==== ���ˈʒu�i�O���j====
	D3DXVECTOR3 localOffsetFront(0.0f, -30.0f, 40.0f); // Z�������ɃI�t�Z�b�g
	D3DXVECTOR3 worldPosFront;
	D3DXVec3TransformCoord(&worldPosFront, &localOffsetFront, &worldMtx);

	// ==== ���ˈʒu�i�㑤�j====
	D3DXVECTOR3 localOffsetBack(0.0f, -30.0f, -40.0f); // Z�������ɃI�t�Z�b�g
	D3DXVECTOR3 worldPosBack;
	D3DXVec3TransformCoord(&worldPosBack, &localOffsetBack, &worldMtx);

	// ���̃V�����_�[�̔���T�C�Y
	float cylinderRadius = 50.0f;
	float maxCylinderHeight = 200.0f;

	// ����p�̒��S�ʒu�͔��ˈʒu���玲�����ɔ����ړ������Ƃ���i�V�����_�[�̒��S�j
	float offsetDistanceFront = maxCylinderHeight * 0.5f;
	float offsetDistanceBack = maxCylinderHeight * 0.5f;
	D3DXVECTOR3 cylinderCenterFront = worldPosFront + forward * offsetDistanceFront;
	D3DXVECTOR3 cylinderCenterBack = worldPosBack - forward * offsetDistanceBack;

	// �p�[�e�B�N������
	pParticle = CParticle::Create(forward, worldPosFront, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 40, CParticle::TYPE_FLAMETHROWER, 5);
	pParticle = CParticle::Create(-forward, worldPosBack, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 40, CParticle::TYPE_FLAMETHROWER, 5);

	CPlayer* pPlayer = CGame::GetPlayer();
	D3DXVECTOR3 playerPos = pPlayer->GetColliderPos();  // �J�v�Z�����S�ʒu
	float playerRadius = pPlayer->GetRadius();
	float playerHeight = pPlayer->GetHeight();

	// �v���C���[�Ƃ̓����蔻��(�O��)
	if (CCollision::CheckCapsuleCylinderCollision_Dir(playerPos, playerRadius, playerHeight,
		cylinderCenterFront, cylinderRadius, maxCylinderHeight, forward, false))
	{
		pPlayer->RespawnToCheckpoint();
	}

	// �v���C���[�Ƃ̓����蔻��(�㑤)
	if (CCollision::CheckCapsuleCylinderCollision_Dir(playerPos, playerRadius, playerHeight,
		cylinderCenterBack, cylinderRadius, maxCylinderHeight, forward, false))
	{
		pPlayer->RespawnToCheckpoint();
	}
}


//=============================================================================
// ���̍�u���b�N�̃R���X�g���N�^
//=============================================================================
CKeyFenceBlock::CKeyFenceBlock()
{
	// �l�̃N���A
	m_closedPos = INIT_VEC3;
	m_prevDown = false;
}
//=============================================================================
// ���̍�u���b�N�̃f�X�g���N�^
//=============================================================================
CKeyFenceBlock::~CKeyFenceBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���̍�u���b�N�̏���������
//=============================================================================
HRESULT CKeyFenceBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	m_closedPos = GetPos();

	return S_OK;
}
//=============================================================================
// ���̍�u���b�N�̍X�V����
//=============================================================================
void CKeyFenceBlock::Update()
{
	CBlock::Update(); // ���ʏ���

	if (CBlockManager::CheckAllTorches())
	{// �S�Ă̏����ɉ΂��t������

		const float kDownRange = 190.0f; // ������[��

		float targetY = m_closedPos.y - kDownRange;

		D3DXVECTOR3 pos = GetPos();
		if (pos.y > targetY)
		{
			pos.y = max(pos.y - 1.0f, targetY);
			SetPos(pos);
		}

		bool n = CBlockManager::CheckAllTorches();

		if (n && !m_prevDown)
		{
			// ���o�J�����ɂ���
			CManager::GetCamera()->SetCamMode(5, D3DXVECTOR3(-1571.0f, 644.5f, 820.5f),
				D3DXVECTOR3(-2180.5f, 181.0f, 589.0f),
				D3DXVECTOR3(0.62f, 1.21f, 0.0f));
		}

		// �t���O���X�V���Ď��̃t���[���ɔ�����
		m_prevDown = n;
	}
}


//=============================================================================
// ���u���b�N�̃R���X�g���N�^
//=============================================================================
CKeyBlock::CKeyBlock()
{
	// �l�̃N���A
	m_ResPos = INIT_VEC3;
}
//=============================================================================
// ���u���b�N�̃f�X�g���N�^
//=============================================================================
CKeyBlock::~CKeyBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���u���b�N�̏���������
//=============================================================================
HRESULT CKeyBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	// �ŏ��̈ʒu�����X�|�[���ʒu�ɐݒ�
	m_ResPos = GetPos();

	// ���I�ɖ߂�
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// ���u���b�N�̍X�V����
//=============================================================================
void CKeyBlock::Update()
{
	CBlock::Update(); // ���ʏ���
}
//=============================================================================
// �Z�b�g����
//=============================================================================
void CKeyBlock::Set(D3DXVECTOR3 pos)
{
	// ���������߂ɃL�l�}�e�B�b�N�ɂ���
	SetEditMode(true);

	// ���u���b�N�̈ʒu���擾
	D3DXVECTOR3 keyPos = GetPos();
	D3DXVECTOR3 keyRot = GetRot();

	D3DXVECTOR3 targetPos(pos);			// �ݒu����ڕW�ʒu
	D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);	// ���������Z�b�g

	keyPos = targetPos;
	keyRot = rot;

	SetPos(keyPos);
	SetRot(keyRot);

	// �R���C�_�[�̍X�V
	UpdateCollider();
}


//=============================================================================
// ���̑���u���b�N�̃R���X�g���N�^
//=============================================================================
CKeyPedestalBlock::CKeyPedestalBlock()
{
	// �l�̃N���A
	m_Pos = INIT_VEC3;
	m_isSet = false;
	m_prevIsSet = false;
}
//=============================================================================
// ���̑���u���b�N�̃f�X�g���N�^
//=============================================================================
CKeyPedestalBlock::~CKeyPedestalBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���̑���u���b�N�̏���������
//=============================================================================
HRESULT CKeyPedestalBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	// �ŏ��̈ʒu�����X�|�[���ʒu�ɐݒ�
	m_Pos = GetPos();

	return S_OK;
}
//=============================================================================
// ���̑���u���b�N�̍X�V����
//=============================================================================
void CKeyPedestalBlock::Update()
{
	CBlock::Update(); // ���ʏ���

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_KEY)
		{
			continue;
		}

		CKeyBlock* pKey = dynamic_cast<CKeyBlock*>(block);

		D3DXVECTOR3 keyPos = block->GetPos();
		D3DXVECTOR3 disPos = keyPos - GetPos();
		float distance = D3DXVec3Length(&disPos);
		const float kTriggerDistance = 130.0f; // ��������

		if (distance < kTriggerDistance)
		{// �������ɂ͂߂�
			D3DXVECTOR3 targetPos(GetPos().x, GetPos().y + 40.0f, GetPos().z);
			pKey->Set(targetPos);
			m_isSet = true;
		}

		bool n = m_isSet;

		if (n && !m_prevIsSet)
		{
			// ����̃T�E���h�Đ�
			CManager::GetSound()->Play(CSound::SOUND_LABEL_SHINE);
		}

		m_prevIsSet = n;
	}
}


//=============================================================================
// ���h�A�u���b�N�̃R���X�g���N�^
//=============================================================================
CKeyDoorBlock::CKeyDoorBlock()
{
	// �l�̃N���A
	m_openPos = INIT_VEC3;
	m_isGoal = false;	// �ڕW�ʒu�Ɉړ�������
	m_isSet = false;	// �ݒu���ꂽ��
	m_deleyTime = 120;	// �J���܂ł̒x������
	prevIsSet = false;	// ���O�ɐݒu������
}
//=============================================================================
// ���h�A�u���b�N�̃f�X�g���N�^
//=============================================================================
CKeyDoorBlock::~CKeyDoorBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���h�A�u���b�N�̏���������
//=============================================================================
HRESULT CKeyDoorBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	// �ŏ��̈ʒu��ۑ�
	m_openPos = GetPos();

	return S_OK;
}
//=============================================================================
// ���h�A�u���b�N�̍X�V����
//=============================================================================
void CKeyDoorBlock::Update()
{
	CBlock::Update(); // ���ʏ���

	if (m_isGoal)
	{// ������ʂ��Ȃ�
		return;
	}

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_KEY_PEDESTAL)
		{// �������Ȃ�������
			continue;
		}

		CKeyPedestalBlock* pPedestal = dynamic_cast<CKeyPedestalBlock*>(block);

		if (pPedestal->IsSet())
		{// ����ɐݒu����Ă�����
			m_isSet = true;
		}
	}

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_DOOR_TRIGGER)
		{// �g���K�[�u���b�N����Ȃ�������
			continue;
		}

		CDoorTriggerBlock* pTrigger = dynamic_cast<CDoorTriggerBlock*>(block);

		if (pTrigger->IsSet())
		{// �ݒu����Ă�����
			m_isSet = true;
		}
	}

	if (m_isSet)
	{
		m_deleyTime--;

		if (m_deleyTime > 0)
		{
			return;
		}

		bool n = m_isSet;

		if (n && !prevIsSet)
		{// ��񂾂��ʂ�
			// �h�ASE�̍Đ�
			CManager::GetSound()->Play(CSound::SOUND_LABEL_DOOR);
		}

		prevIsSet = n;

		// �h�A���J����
		const float kOpenRange = 210.0f; // �J������

		float targetY = m_openPos.y + kOpenRange;

		D3DXVECTOR3 pos = GetPos();
		if (pos.y < targetY)
		{
			pos.y += 1.0f;
			SetPos(pos);
		}
		else
		{// �ڕW�ʒu�ɓ��B����
			m_isGoal = true;
		}
	}
}


//=============================================================================
// ���u���b�N�̃R���X�g���N�^
//=============================================================================
CShieldBlock::CShieldBlock()
{
	// �l�̃N���A
	m_isEnd = false;
	m_playedSoundID = -1;
}
//=============================================================================
// ���u���b�N�̃f�X�g���N�^
//=============================================================================
CShieldBlock::~CShieldBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���u���b�N�̍X�V����
//=============================================================================
void CShieldBlock::Update(void)
{
	CBlock::Update(); // ���ʏ���

	if (!CGame::GetPlayer())
	{
		return;
	}

	CParticle* pParticle = NULL;

	if (CManager::GetMode() == MODE_GAME)
	{
		D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
		D3DXVECTOR3 disPos = playerPos - GetPos();

		float distance = D3DXVec3Length(&disPos);

		const float kTriggerDistance = 780.0f; // ��������

		if (distance < kTriggerDistance)
		{
			// �I�t�Z�b�g
			D3DXVECTOR3 localOffset(0.0f, -60.0f, 0.0f);
			D3DXVECTOR3 worldOffset;

			// �u���b�N�̃��[���h�}�g���b�N�X���擾
			D3DXMATRIX worldMtx = GetWorldMatrix();

			D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

			// �p�[�e�B�N������
			pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.6f, 0.6f, 0.0f, 0.3f), 50, CParticle::TYPE_AURA, 1);

			if (m_playedSoundID == -1) // �Đ����Ă��Ȃ���΍Đ��J�n
			{
				// �O�̉����~�߂�i�O�̂��߁j
				CManager::GetSound()->Stop(m_playedSoundID);

				// 3D�T�E���h�Đ�����ID��ێ�
				m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_TREASURE, GetPos(), 250.0f, kTriggerDistance);
			}

			// �����̈ʒu�X�V��ID���g��
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
			}
		}
		else
		{
			// ���ꂽ�特��~����ID���Z�b�g
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->Stop(m_playedSoundID);
				m_playedSoundID = -1;
			}
		}

		const float getDistance = 250.0f; // ��������

		if (distance < getDistance)
		{// ��ɓ��ꂽ
			m_isEnd = true;
		}
	}
}


//=============================================================================
// �Α��u���b�N�̃R���X�g���N�^
//=============================================================================
CStatueBlock::CStatueBlock()
{
	// �l�̃N���A
	m_playedSoundID = -1;
}
//=============================================================================
// �Α��u���b�N�̃f�X�g���N�^
//=============================================================================
CStatueBlock::~CStatueBlock()
{
	// �Ȃ�
}
//=============================================================================
// �Α��u���b�N�̍X�V����
//=============================================================================
void CStatueBlock::Update(void)
{
	CBlock::Update(); // ���ʏ���

	if (!CGame::GetPlayer())
	{
		return;
	}

	CParticle* pParticle = NULL;

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 780.0f; // ��������

	if (distance < kTriggerDistance)
	{
		// �I�t�Z�b�g
		D3DXVECTOR3 localOffset(0.0f, -140.0f, 0.0f);
		D3DXVECTOR3 worldOffset;

		// �u���b�N�̃��[���h�}�g���b�N�X���擾
		D3DXMATRIX worldMtx = GetWorldMatrix();

		D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

		// �p�[�e�B�N������
		pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_STATUE_FIRE, 1);
		pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_STATUE_FIRE, 1);

		if (m_playedSoundID == -1) // �Đ����Ă��Ȃ���΍Đ��J�n
		{
			// �O�̉����~�߂�i�O�̂��߁j
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3D�T�E���h�Đ�����ID��ێ�
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 250.0f, kTriggerDistance);
		}

		// �����̈ʒu�X�V��ID���g��
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
		}
	}
	else
	{
		// ���ꂽ�特��~����ID���Z�b�g
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}
}


//=============================================================================
// �΂�����Ɠ����Α��u���b�N�̃R���X�g���N�^
//=============================================================================
CStatueBlock2::CStatueBlock2()
{
	// �l�̃N���A
	m_startPos = INIT_VEC3;
	m_playedSoundID = -1;
	m_triggerDis = false;
	m_isMoving = false;
	m_hasTriggered = false;
}
//=============================================================================
// �΂�����Ɠ����Α��u���b�N�̃f�X�g���N�^
//=============================================================================
CStatueBlock2::~CStatueBlock2()
{
	// �Ȃ�
}
//=============================================================================
// �΂�����Ɠ����Α��u���b�N�̏���������
//=============================================================================
HRESULT CStatueBlock2::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	// �ŏ��̈ʒu��ۑ�
	m_startPos = GetPos();

	return S_OK;
}
//=============================================================================
// �΂�����Ɠ����Α��u���b�N�̍X�V����
//=============================================================================
void CStatueBlock2::Update(void)
{
	CBlock::Update(); // ���ʏ���

	if (!CGame::GetPlayer())
	{
		return;
	}

	CParticle* pParticle = NULL;

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 680.0f; // ��������

	if (distance < kTriggerDistance)
	{
		// �I�t�Z�b�g
		D3DXVECTOR3 localOffset1(90.0f, -200.0f, 0.0f);
		D3DXVECTOR3 localOffset2(-90.0f, -170.0f, 0.0f);
		D3DXVECTOR3 worldOffset1;
		D3DXVECTOR3 worldOffset2;

		// �u���b�N�̃��[���h�}�g���b�N�X���擾
		D3DXMATRIX worldMtx = GetWorldMatrix();

		D3DXVec3TransformCoord(&worldOffset1, &localOffset1, &worldMtx);
		D3DXVec3TransformCoord(&worldOffset2, &localOffset2, &worldMtx);

		// �p�[�e�B�N������
		pParticle = CParticle::Create(INIT_VEC3, worldOffset1, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_STATUE_FIRE, 1);
		pParticle = CParticle::Create(INIT_VEC3, worldOffset1, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_STATUE_FIRE, 1);

		if (m_triggerDis)
		{
			pParticle = CParticle::Create(INIT_VEC3, worldOffset2, D3DXCOLOR(0.8f, 0.3f, 0.1f, 0.8f), 8, CParticle::TYPE_STATUE_FIRE, 1);
			pParticle = CParticle::Create(INIT_VEC3, worldOffset2, D3DXCOLOR(1.0f, 0.5f, 0.0f, 0.8f), 15, CParticle::TYPE_STATUE_FIRE, 1);
		}

		if (m_playedSoundID == -1) // �Đ����Ă��Ȃ���΍Đ��J�n
		{
			// �O�̉����~�߂�i�O�̂��߁j
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3D�T�E���h�Đ�����ID��ێ�
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 250.0f, kTriggerDistance);
		}

		// �����̈ʒu�X�V��ID���g��
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
		}
	}
	else
	{
		// ���ꂽ�特��~����ID���Z�b�g
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	// �ړ�����
	Move();
}
//=============================================================================
// �΂�����Ɠ����Α��u���b�N�̈ړ�����
//=============================================================================
void CStatueBlock2::Move(void)
{
	D3DXVECTOR3 pos = GetPos();

	if (!m_hasTriggered) // �܂��g���K�[���ĂȂ�
	{
		// --- �����Ƃ̋����`�F�b�N ---
		float minDistance = FLT_MAX;
		bool torchFound = false;

		for (CBlock* block : CBlockManager::GetAllBlocks())
		{
			if (block->GetType() != TYPE_TORCH2)
			{// �u���^��������Ȃ�������
				continue;
			}

			D3DXVECTOR3 torchPos = block->GetPos();
			D3DXVECTOR3 disPos = torchPos - pos;

			float distance = D3DXVec3Length(&disPos);
			if (distance < minDistance)
			{
				minDistance = distance;
				torchFound = true;
			}


		}

		// ��苗���ȓ��ɏ������������瓮����
		const float kTriggerDistance = 210.0f; // ��������

		// �������Ȃ�g���K�[ON
		if (torchFound && minDistance < kTriggerDistance)
		{
			// �Ђ�߂�SE
			CManager::GetSound()->Play(CSound::SOUND_LABEL_INSPIRATION);

			m_triggerDis = true;
			m_isMoving = true;
			m_hasTriggered = true;
		}
	}

	// --- �ړ����� ---
	if (m_isMoving)
	{
		// �ړ����x
		const float speed = 1.0f;

		// ������
		const float Range = 180.0f; // ��������

		float targetX = m_startPos.x + Range;

		if (pos.x < targetX)
		{
			pos.x += speed;
			SetPos(pos);
		}
	}
}


//=============================================================================
// ���u���b�N�̃R���X�g���N�^
//=============================================================================
CEggBlock::CEggBlock()
{
	// �l�̃N���A
	m_isGet = false;
	m_playedSoundID = -1;
}
//=============================================================================
// ���u���b�N�̃f�X�g���N�^
//=============================================================================
CEggBlock::~CEggBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���u���b�N�̍X�V����
//=============================================================================
void CEggBlock::Update(void)
{
	CBlock::Update(); // ���ʏ���

	if (!CGame::GetPlayer())
	{
		return;
	}

	CParticle* pParticle = NULL;

	if (CManager::GetMode() == MODE_GAME)
	{
		D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
		D3DXVECTOR3 disPos = playerPos - GetPos();

		float distance = D3DXVec3Length(&disPos);

		const float kTriggerDistance = 1100.0f; // ��������

		if (distance < kTriggerDistance && !m_isGet)
		{
			// �I�t�Z�b�g
			D3DXVECTOR3 localOffset(0.0f, 15.0f, 0.0f);
			D3DXVECTOR3 worldOffset;

			// �u���b�N�̃��[���h�}�g���b�N�X���擾
			D3DXMATRIX worldMtx = GetWorldMatrix();

			D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

			// �p�[�e�B�N������
			pParticle = CParticle::Create(INIT_VEC3, worldOffset, D3DXCOLOR(0.8f, 0.2f, 0.2f, 0.3f), 50, CParticle::TYPE_AURA2, 1);

			if (m_playedSoundID == -1) // �Đ����Ă��Ȃ���΍Đ��J�n
			{
				// �O�̉����~�߂�i�O�̂��߁j
				CManager::GetSound()->Stop(m_playedSoundID);

				// 3D�T�E���h�Đ�����ID��ێ�
				m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_MASK, GetPos(), 250.0f, kTriggerDistance);
			}

			// �����̈ʒu�X�V��ID���g��
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
			}
		}
		else
		{
			// ���ꂽ�特��~����ID���Z�b�g
			if (m_playedSoundID != -1)
			{
				CManager::GetSound()->Stop(m_playedSoundID);
				m_playedSoundID = -1;
			}
		}

		const float getDistance = 250.0f; // ��������

		if (distance < getDistance)
		{
			if (!m_isGet)
			{
				// ���擾UI�̐���
				CUi::Create(CUi::TYPE_GET, "data/TEXTURE/ui_egg.png", D3DXVECTOR3(900.0f, 220.0f, 0.0f), 320.0f, 140.0f);
			}

			m_isGet = true;
		}
	}
}


//=============================================================================
// �h�A�g���K�[�u���b�N�̃R���X�g���N�^
//=============================================================================
CDoorTriggerBlock::CDoorTriggerBlock()
{
	// �l�̃N���A
	m_Pos = INIT_VEC3;
	m_isSet = false;
	m_prevIsSet = false;
	m_isSetCam = false;
	m_nDeleyTime = 120;// �J�����ڍs�܂ł̒x������
}
//=============================================================================
// �h�A�g���K�[�u���b�N�̃f�X�g���N�^
//=============================================================================
CDoorTriggerBlock::~CDoorTriggerBlock()
{
	// �Ȃ�
}
//=============================================================================
// �h�A�g���K�[�u���b�N�̏���������
//=============================================================================
HRESULT CDoorTriggerBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	// �ŏ��̈ʒu�����X�|�[���ʒu�ɐݒ�
	m_Pos = GetPos();

	return S_OK;
}
//=============================================================================
// �h�A�g���K�[�u���b�N�̍X�V����
//=============================================================================
void CDoorTriggerBlock::Update()
{
	CBlock::Update(); // ���ʏ���

	if (m_isSet && !m_isSetCam)
	{
		m_nDeleyTime--;
	}

	if (m_nDeleyTime <= 0 && !m_isSetCam)
	{
		// ���o�J�����ɂ���
		CManager::GetCamera()->SetCamMode(3, D3DXVECTOR3(1405.0f, 170.0f, 300.0f),
			D3DXVECTOR3(1894.0f, 93.0f, 297.0f),
			D3DXVECTOR3(0.16f, -1.57f, 0.0f));

		m_nDeleyTime = 0;
		m_isSetCam = true;
	}

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_WOODBOX || m_isSet)
		{
			continue;
		}

		CWoodBoxBlock* pWoodBox = dynamic_cast<CWoodBoxBlock*>(block);

		D3DXVECTOR3 woodBoxPos = block->GetPos();
		D3DXVECTOR3 disPos = woodBoxPos - GetPos();
		float distance = D3DXVec3Length(&disPos);
		const float kTriggerDistance = 130.0f; // ��������

		if (distance < kTriggerDistance)
		{// �ؔ������ɂ͂߂�
			// �I�t�Z�b�g
			D3DXVECTOR3 localOffset(0.0f, -25.0f, -5.0f);
			D3DXVECTOR3 worldOffset;

			// �u���b�N�̃��[���h�}�g���b�N�X���擾
			D3DXMATRIX worldMtx = GetWorldMatrix();

			D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

			pWoodBox->Set(worldOffset);
			m_isSet = true;
		}

		bool n = m_isSet;

		if (n && !m_prevIsSet)
		{
			// ����̃T�E���h�Đ�
			CManager::GetSound()->Play(CSound::SOUND_LABEL_SHINE);
		}

		m_prevIsSet = n;
	}
}


//=============================================================================
// ���X�|�[���u���b�N�̃R���X�g���N�^
//=============================================================================
CRespawnBlock::CRespawnBlock()
{
	// �l�̃N���A
}
//=============================================================================
// ���X�|�[���u���b�N�̃f�X�g���N�^
//=============================================================================
CRespawnBlock::~CRespawnBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���X�|�[���u���b�N�̍X�V����
//=============================================================================
void CRespawnBlock::Update(void)
{
	// �u���b�N�̍X�V����
	CBlock::Update();

	// �u���b�N�� AABB ���擾
	D3DXVECTOR3 blockPos = GetPos();		// �u���b�N�̈ʒu
	D3DXVECTOR3 modelSize = GetModelSize(); // �X�C�b�`�̌��̃T�C�Y�i���S���_�j
	D3DXVECTOR3 scale = GetSize();			// �g�嗦

	// �g�嗦��K�p����
	D3DXVECTOR3 blockSize;
	blockSize.x = modelSize.x * scale.x;
	blockSize.y = modelSize.y * scale.y;
	blockSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 blockMin = blockPos - blockSize * 0.5f;
	D3DXVECTOR3 blockMax = blockPos + blockSize * 0.5f;

	// --- �v���C���[�ڐG���� ---
	CPlayer* pPlayer = CGame::GetPlayer();

	if (pPlayer)
	{
		D3DXVECTOR3 pPos = pPlayer->GetColliderPos(); // �J�v�Z���R���C�_�[���S�ʒu

		// �J�v�Z���R���C�_�[�̃T�C�Y����AABB�T�C�Y���v�Z
		float radius = pPlayer->GetRadius();
		float height = pPlayer->GetHeight();

		D3DXVECTOR3 pSize;
		pSize.x = radius * 2.0f;
		pSize.z = radius * 2.0f;
		pSize.y = height + radius * 2.0f;

		// AABB�v�Z
		D3DXVECTOR3 pMin = pPos - pSize * 0.5f;
		D3DXVECTOR3 pMax = pPos + pSize * 0.5f;

		bool isOverlap =
			blockMin.x <= pMax.x && blockMax.x >= pMin.x &&
			blockMin.y <= pMax.y && blockMax.y >= pMin.y &&
			blockMin.z <= pMax.z && blockMax.z >= pMin.z;

		// �������Ă���
		if (isOverlap)
		{
			// �v���C���[�̃��X�|�[������
			CGame::GetPlayer()->RespawnToCheckpoint(D3DXVECTOR3(2810.0f, 100.0f, -1518.0f));
		}
	}
}


//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̃R���X�g���N�^
//=============================================================================
CRedMassBlock::CRedMassBlock()
{
	// �l�̃N���A
	m_ResPos = INIT_VEC3;
}
//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̃f�X�g���N�^
//=============================================================================
CRedMassBlock::~CRedMassBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̏���������
//=============================================================================
HRESULT CRedMassBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	m_ResPos = GetPos();

	// ���I�ɖ߂�
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̍X�V����
//=============================================================================
void CRedMassBlock::Update(void)
{
	// �u���b�N�̍X�V����
	CBlock::Update();

	if (GetPos().y <= -410.0f)
	{
		// ���X�|�[������
		Respawn(m_ResPos);
	}
}

//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̃R���X�g���N�^
//=============================================================================
CBlueMassBlock::CBlueMassBlock()
{
	// �l�̃N���A
	m_ResPos = INIT_VEC3;
}
//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̃f�X�g���N�^
//=============================================================================
CBlueMassBlock::~CBlueMassBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̏���������
//=============================================================================
HRESULT CBlueMassBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	m_ResPos = GetPos();

	// ���I�ɖ߂�
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̍X�V����
//=============================================================================
void CBlueMassBlock::Update(void)
{
	// �u���b�N�̍X�V����
	CBlock::Update();

	if (GetPos().y <= -410.0f)
	{
		// ���X�|�[������
		Respawn(m_ResPos);
	}
}


//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̃R���X�g���N�^
//=============================================================================
CYellowMassBlock::CYellowMassBlock()
{
	// �l�̃N���A
	m_ResPos = INIT_VEC3;
}
//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̃f�X�g���N�^
//=============================================================================
CYellowMassBlock::~CYellowMassBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̏���������
//=============================================================================
HRESULT CYellowMassBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	m_ResPos = GetPos();

	// ���I�ɖ߂�
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̍X�V����
//=============================================================================
void CYellowMassBlock::Update(void)
{
	// �u���b�N�̍X�V����
	CBlock::Update();

	if (GetPos().y <= -410.0f)
	{
		// ���X�|�[������
		Respawn(m_ResPos);
	}
}


//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̃R���X�g���N�^
//=============================================================================
CGreenMassBlock::CGreenMassBlock()
{
	// �l�̃N���A
	m_ResPos = INIT_VEC3;
}
//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̃f�X�g���N�^
//=============================================================================
CGreenMassBlock::~CGreenMassBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̏���������
//=============================================================================
HRESULT CGreenMassBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	m_ResPos = GetPos();

	// ���I�ɖ߂�
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// ���ʃu���b�N(��)�u���b�N�̍X�V����
//=============================================================================
void CGreenMassBlock::Update(void)
{
	// �u���b�N�̍X�V����
	CBlock::Update();

	if (GetPos().y <= -410.0f)
	{
		// ���X�|�[������
		Respawn(m_ResPos);
	}
}


//=============================================================================
// ���ԃu���b�N�̃R���X�g���N�^
//=============================================================================
CWaterWheelBlock::CWaterWheelBlock()
{
	// �l�̃N���A
	m_playedSoundID = -1;
	m_isRotation = false;
}
//=============================================================================
// ���ԃu���b�N�̃f�X�g���N�^
//=============================================================================
CWaterWheelBlock::~CWaterWheelBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���ԃu���b�N�̍X�V����
//=============================================================================
void CWaterWheelBlock::Update(void)
{
	CBlock::Update();// ���ʏ���

	Rotation();
}
//=============================================================================
// ���ԃu���b�N�̉�]����
//=============================================================================
void CWaterWheelBlock::Rotation(void)
{
	// ����X�C�b�`�����݂��邩�m�F
	std::vector<CBlock*> blocks = CBlockManager::GetAllBlocks();

	for (CBlock* block : blocks)
	{
		if (block->GetType() != TYPE_SWITCH2)
		{
			continue;
		}

		CBridgeSwitchBlock* pSwitch = dynamic_cast<CBridgeSwitchBlock*>(block);

		if (!pSwitch || !pSwitch->IsSinkOn())
		{
			continue;
		}

		m_isRotation = true;

		// ��]
		D3DXVECTOR3 rot = GetRot();

		rot.z += 0.02f;

		// �����̐ݒ�
		SetRot(rot);
	}

	if (!m_isRotation)
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();
	float distance = D3DXVec3Length(&disPos);
	const float kTriggerDistance = 880.0f; // ��������

	if (distance < kTriggerDistance)
	{
		if (m_playedSoundID == -1) // �Đ����Ă��Ȃ���΍Đ��J�n
		{
			// �O�̉����~�߂�i�O�̂��߁j
			CManager::GetSound()->Stop(m_playedSoundID);

			// 3D�T�E���h�Đ�����ID��ێ�
			m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_WATERWHEEL, GetPos(), 150.0f, kTriggerDistance);
		}

	}
	else
	{
		// ���ꂽ�特��~����ID���Z�b�g
		if (m_playedSoundID != -1)
		{
			CManager::GetSound()->Stop(m_playedSoundID);
			m_playedSoundID = -1;
		}
	}

	// �����̈ʒu�X�V��ID���g��
	if (m_playedSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, GetPos());
	}
}


//=============================================================================
// �p�C�v�u���b�N�̃R���X�g���N�^
//=============================================================================
CPipeBlock::CPipeBlock()
{
	// �l�̃N���A
	m_playedSoundID = -1;
	m_isOn = false;
}
//=============================================================================
// �p�C�v�u���b�N�̃f�X�g���N�^
//=============================================================================
CPipeBlock::~CPipeBlock()
{
	// �Ȃ�
}
//=============================================================================
// �p�C�v�u���b�N�̍X�V����
//=============================================================================
void CPipeBlock::Update(void)
{
	// �u���b�N�̍X�V����
	CBlock::Update();

	// ����X�C�b�`�����݂��邩�m�F
	std::vector<CBlock*> blocks = CBlockManager::GetAllBlocks();

	for (CBlock* block : blocks)
	{
		if (block->GetType() != TYPE_SWITCH2)
		{
			continue;
		}

		CBridgeSwitchBlock* pSwitch = dynamic_cast<CBridgeSwitchBlock*>(block);

		if (!pSwitch || !pSwitch->IsSinkOn())
		{
			continue;
		}

		m_isOn = true;
	}

	if (!m_isOn)
	{
		return;
	}

	CParticle* pParticle = NULL;
	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	// �I�t�Z�b�g
	D3DXVECTOR3 localOffset(0.0f, 140.0f, 50.0f); // �����̐�[�i���[�J���j
	D3DXVECTOR3 worldOffset;

	// �u���b�N�̃��[���h�}�g���b�N�X���擾
	D3DXMATRIX worldMtx = GetWorldMatrix();

	D3DXVec3TransformCoord(&worldOffset, &localOffset, &worldMtx);

	// ==== �O�����x�N�g���iZ���j====
	D3DXVECTOR3 localForward(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 forward;
	D3DXVec3TransformNormal(&forward, &localForward, &worldMtx);
	D3DXVec3Normalize(&forward, &forward);

	// �p�[�e�B�N������
	pParticle = CParticle::Create(forward, worldOffset, D3DXCOLOR(0.3f, 0.6f, 1.0f, 0.8f), 50, CParticle::TYPE_WATERFLOW, 10);

	const float kTriggerDistance = 1180.0f; // ��������

	if (distance < kTriggerDistance)
	{

		//if (m_playedSoundID == -1) // �Đ����Ă��Ȃ���΍Đ��J�n
		//{
		//	// �O�̉����~�߂�i�O�̂��߁j
		//	CManager::GetSound()->Stop(m_playedSoundID);

		//	// 3D�T�E���h�Đ�����ID��ێ�
		//	m_playedSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_FIRE, GetPos(), 150.0f, kTriggerDistance);
		//}
	}
	//else
	//{
	//	// ���ꂽ�特��~����ID���Z�b�g
	//	if (m_playedSoundID != -1)
	//	{
	//		CManager::GetSound()->Stop(m_playedSoundID);
	//		m_playedSoundID = -1;
	//	}
	//}

	//// �����̈ʒu�X�V��ID���g��
	//if (m_playedSoundID != -1)
	//{
	//	CManager::GetSound()->UpdateSoundPosition(m_playedSoundID, worldOffset);
	//}
}