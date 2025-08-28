//=============================================================================
//
// �u���b�N���� [block.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "block.h"
#include "manager.h"
#include "particle.h"
#include "game.h"
#include "result.h"
#include "blocklist.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
std::unordered_map<CBlock::TYPE, BlockCreateFunc> CBlock::m_BlockFactoryMap = {};
std::unordered_map<CBlock::TYPE, const char*> s_TexturePathMap = {};

using namespace std;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CBlock::CBlock(int nPriority) : CObjectX(nPriority)
{
	// �l�̃N���A
	for (int nCnt = 0; nCnt < MAX_PATH; nCnt++)
	{
		m_szPath[nCnt] = NULL;					// �t�@�C���p�X
	}

	m_col			 = INIT_XCOL;				// �F
	m_baseCol		 = INIT_XCOL;				// �x�[�X�̐F
	m_bSelected		 = false;					// �I���t���O
	m_nIdxTexture	 = 0;						// �e�N�X�`���C���f�b�N�X
	m_pDebug3D		 = NULL;					// 3D�f�o�b�O�\���ւ̃|�C���^
	m_prevSize		 = INIT_VEC3;				// �O��̃T�C�Y
	m_colliderSize	 = INIT_VEC3;				// �R���C�_�[�T�C�Y
	m_colliderOffset = INIT_VEC3;				// �R���C�_�[�̃I�t�Z�b�g
	m_isEditMode	 = false;					// �ҏW�����ǂ���
}
//=============================================================================
// ��������
//=============================================================================
CBlock* CBlock::Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size, TYPE type)
{
	if (m_BlockFactoryMap.empty())
	{
		// �t�@�N�g���[
		InitFactory();
	}

	CBlock* pBlock = nullptr;

	auto it = m_BlockFactoryMap.find(type);
	if (it != m_BlockFactoryMap.end())
	{
		pBlock = it->second();
	}
	else
	{
		pBlock = new CBlock(); // �f�t�H���g���N���X
	}

	if (!pBlock)
	{
		return NULL;
	}

	pBlock->SetPos(pos);
	pBlock->SetRot(rot);
	pBlock->SetSize(size);
	pBlock->SetType(type);
	pBlock->SetPath(pFilepath);

	pBlock->Init();
	pBlock->CreatePhysicsFromScale(size);

	return pBlock;
}
//=============================================================================
// �t�@�N�g���[
//=============================================================================
void CBlock::InitFactory(void)
{
	// ���X�g����ɂ���
	m_BlockFactoryMap.clear();

	m_BlockFactoryMap[CBlock::TYPE_WOODBOX]				= []() -> CBlock* { return new CWoodBoxBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_PILLAR]				= []() -> CBlock* { return new CPillarBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_BRIDGE]				= []() -> CBlock* { return new CWoodBridgeBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_RAFT]				= []() -> CBlock* { return new CRaftBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_WATER]				= []() -> CBlock* { return new CWaterBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_DOOR]				= []() -> CBlock* { return new CDoorBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_DOOR2]				= []() -> CBlock* { return new CBigDoorBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_SWITCH]				= []() -> CBlock* { return new CSwitchBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_SWITCH2]				= []() -> CBlock* { return new CBridgeSwitchBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_AXE]					= []() -> CBlock* { return new CAxeBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_BRIDGE2]				= []() -> CBlock* { return new CBridgeBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_TARGET]				= []() -> CBlock* { return new CTargetBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_TORCH]				= []() -> CBlock* { return new CTorchBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_TORCH2]				= []() -> CBlock* { return new CTorch2Block(); };
	m_BlockFactoryMap[CBlock::TYPE_MASK]				= []() -> CBlock* { return new CMaskBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_SWORD]				= []() -> CBlock* { return new CSwordBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_SWITCH3]				= []() -> CBlock* { return new CBarSwitchBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_BAR]					= []() -> CBlock* { return new CBarBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_BRIDGE3]				= []() -> CBlock* { return new CFootingBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_FIRE_STATUE]			= []() -> CBlock* { return new CFireStatueBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_TURN_FIRE_STATUE]	= []() -> CBlock* { return new CTurnFireStatueBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_BLOCK3]				= []() -> CBlock* { return new CBoxRockBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_FLOOR4]				= []() -> CBlock* { return new CLavaBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_MOVE_FIRE_STATUE]	= []() -> CBlock* { return new CMoveFireStatueBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_TORCH3]				= []() -> CBlock* { return new CTorch3Block(); };
	m_BlockFactoryMap[CBlock::TYPE_KEYFENCE]			= []() -> CBlock* { return new CKeyFenceBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_KEY]					= []() -> CBlock* { return new CKeyBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_KEY_PEDESTAL]		= []() -> CBlock* { return new CKeyPedestalBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_KEY_DOOR]			= []() -> CBlock* { return new CKeyDoorBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_SHIELD]				= []() -> CBlock* { return new CShieldBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_STATUE]				= []() -> CBlock* { return new CStatueBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_STATUE2]				= []() -> CBlock* { return new CStatueBlock2(); };
	m_BlockFactoryMap[CBlock::TYPE_EGG]					= []() -> CBlock* { return new CEggBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_DOOR_TRIGGER]		= []() -> CBlock* { return new CDoorTriggerBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_ROCK]				= []() -> CBlock*
	{
		CRockBlock* pRock = new CRockBlock();

		// �`�F�b�N�|�C���g�ǉ�
		pRock->AddPathPoint(D3DXVECTOR3(2812.5f, 217.0f, -1989.0f));
		pRock->AddPathPoint(D3DXVECTOR3(2810.0f, 217.0f, -2821.5f));
		pRock->AddPathPoint(D3DXVECTOR3(2718.0f, 217.0f, -3045.0f));
		pRock->AddPathPoint(D3DXVECTOR3(1958.5f, 217.0f, -3815.0f));
		pRock->AddPathPoint(D3DXVECTOR3(1746.0f, 217.0f, -3898.0f));
		pRock->AddPathPoint(D3DXVECTOR3(343.0f, 217.0f, -3898.0f));
		pRock->AddPathPoint(D3DXVECTOR3(-660.0f, 217.0f, -3898.0f));
		pRock->AddPathPoint(D3DXVECTOR3(-1230.5f, 217.0f, -3898.0f));
		pRock->AddPathPoint(D3DXVECTOR3(-1430.5f, 217.0f, -3898.0f));
		return pRock;
	};
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CBlock::Init(void)
{
	// �I�u�W�F�N�gX�̏���������
	CObjectX::Init();

	// �}�e���A���F���u���b�N�̐F�ɐݒ�
	m_col = GetMaterialColor();
	m_col = m_baseCol;              // ���݂̐F�ɂ���x����Ă���

	// ====�_�̏����R���C�_�[====
	m_colliderHandle.size = D3DXVECTOR3(21.0f, 248.5f, 21.5f);
	m_colliderHandle.offset = D3DXVECTOR3(0.5f, -118.0f, 0.0f);

	// ====�n�̏����R���C�_�[====
	m_colliderBlade.size = D3DXVECTOR3(304.0f, 127.0f, 24.5f);
	m_colliderBlade.offset = D3DXVECTOR3(0.0f, -307.0f, 0.0f);

	// �e�N�X�`�����蓖��
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register(GetTexPathFromType(m_Type));

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CBlock::Uninit(void)
{
	// �T�E���h���~
	CManager::GetSound()->Stop();

	ReleasePhysics();

	// �I�u�W�F�N�gX�̏I������
	CObjectX::Uninit();
}
//=============================================================================
// �X�V����
//=============================================================================
void CBlock::Update(void)
{
	// �ÓI�u���b�N�� Transform ���蓮�ōX�V
	if (!IsDynamicBlock() || IsEditMode())
	{
		D3DXVECTOR3 Pos = GetPos() + m_colliderOffset;
		D3DXVECTOR3 Rot = GetRot();

		btTransform trans;
		trans.setIdentity();

		D3DXMATRIX matRot;
		D3DXMatrixRotationYawPitchRoll(&matRot, Rot.y, Rot.x, Rot.z);

		D3DXQUATERNION dq;
		D3DXQuaternionRotationMatrix(&dq, &matRot);

		btQuaternion q(dq.x, dq.y, dq.z, dq.w);
		trans.setOrigin(btVector3(Pos.x, Pos.y, Pos.z));
		trans.setRotation(q);

		if (m_pRigidBody && m_pRigidBody->getMotionState())
		{
			m_pRigidBody->setWorldTransform(trans);
			m_pRigidBody->getMotionState()->setWorldTransform(trans);
		}
	}
	else
	{
		// ���I�u���b�N�͕����̕ϊ����擾���Ĕ��f

		if (!m_pRigidBody || !m_pRigidBody->getMotionState())
		{
			return;
		}

		btTransform trans;

		m_pRigidBody->getMotionState()->getWorldTransform(trans);

		btVector3 pos = trans.getOrigin();

		// �ʒu�Z�b�g�i�I�t�Z�b�g���������j
		D3DXVECTOR3 newPos(pos.x(), pos.y(), pos.z());
		SetPos(newPos - m_colliderOffset);

		btQuaternion rot = m_pRigidBody->getOrientation();
		rot.normalize();

		// Z�����]�ō��W�n�␳
		D3DXQUATERNION dq(rot.x(), rot.y(), rot.z(), rot.w());

		// ��]�s��ɕϊ�
		D3DXMATRIX matRot;
		D3DXMatrixRotationQuaternion(&matRot, &dq);

		// �s�� -> �I�C���[�p�iXYZ���j
		D3DXVECTOR3 euler;
		float sy = -matRot._32; // �E����W

		// Clamp�͈͂������ɂ��Ĉُ�p���
		sy = std::max(-1.0f, std::min(1.0f, sy));
		euler.x = asinf(sy);  // pitch (X)

		// cos(pitch) ��0�ɋ߂��ƃW���o�����b�N�Ȃ̂ŁA���̉������
		if (fabsf(cosf(euler.x)) > 1e-4f)
		{
			euler.y = atan2f(matRot._31, matRot._33);  // yaw (Y)
			euler.z = atan2f(matRot._12, matRot._22);  // roll (Z)
		}
		else
		{
			euler.y = 0.0f;
			euler.z = atan2f(-matRot._21, matRot._11); // ��֒l�iRoll�������o�j
		}

		// �A���␳
		static D3DXVECTOR3 prevEuler(0, 0, 0);
		auto FixAngleJump = [](float prev, float current) -> float

		{
			if (_isnan(current))
			{
				return prev;
			}

			float diff = current - prev;

			if (diff > D3DX_PI)
			{
				current -= 2 * D3DX_PI;
			}
			else if (diff < -D3DX_PI)
			{
				current += 2 * D3DX_PI;
			}

			return current;
		};

		euler.x = FixAngleJump(prevEuler.x, euler.x);
		euler.y = FixAngleJump(prevEuler.y, euler.y);
		euler.z = FixAngleJump(prevEuler.z, euler.z);

		prevEuler = euler;

		// �Z�b�g
		SetRot(euler);
	}
}
//=============================================================================
// �R���C�_�[�̍X�V����
//=============================================================================
void CBlock::UpdateCollider(void)
{
	if (!m_pRigidBody)
	{
		return;
	}

	// �ʒu�̎擾
	D3DXVECTOR3 Pos = GetPos();

	// �폜���čĐ���
	ReleasePhysics();

	CreatePhysics(Pos, m_colliderSize);
}
//=============================================================================
// �`�揈��
//=============================================================================
void CBlock::Draw(void)
{
	// �I�u�W�F�N�gX�̕`�揈��
	CObjectX::Draw();

#ifdef _DEBUG

	////// �����蔻��̕`�揈��
	//DrawCollider();

#endif

}
//=============================================================================
// �����蔻��`�揈��
//=============================================================================
void CBlock::DrawCollider(void)
{
	if (m_pRigidBody)
	{
		// �R���C�_�[�̕`��
		m_pDebug3D->DrawBlockCollider(m_pRigidBody, D3DXCOLOR(0.0f, 1.0f, 0.3f, 1.0f));
	}
}
//=============================================================================
// �F�̎擾
//=============================================================================
D3DXCOLOR CBlock::GetCol(void) const
{
	if (m_bSelected)
	{// �Ԃ�����
		return D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.6f);
	}
	else
	{// ���␳
		return D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); 
	}
}
//=============================================================================
// �摜�\���p�e�N�X�`���p�X�̎擾
//=============================================================================
const char* CBlock::GetTexPathFromType(TYPE type)
{
	auto it = s_TexturePathMap.find(type);
	if (it != s_TexturePathMap.end())
	{
		return it->second;
	}

	return "";
}
//=============================================================================
// �摜�\���p�e�N�X�`���p�X
//=============================================================================
const std::unordered_map<CBlock::TYPE, const char*> CBlock::s_TexturePathMap = 
{
	{ TYPE_WOODBOX,			"data/TEXTURE/woodbox.png" },
	{ TYPE_WALL,			"data/TEXTURE/wall1.png" },
	{ TYPE_WALL2,			"data/TEXTURE/wall2.png" },
	{ TYPE_WALL3,			"data/TEXTURE/wall3.png" },
	{ TYPE_WALL4,			"data/TEXTURE/wall4.png" },
	{ TYPE_AXE,				"data/TEXTURE/Axe.png" },
	{ TYPE_RAFT,			"data/TEXTURE/ikada.png" },
	{ TYPE_ROCK,			"data/TEXTURE/rock.png" },
	{ TYPE_TORCH,			"data/TEXTURE/torch1.png" },
	{ TYPE_TORCH2,			"data/TEXTURE/torch2.png" },
	{ TYPE_FLOOR,			"data/TEXTURE/floor1.png" },
	{ TYPE_FLOOR2,			"data/TEXTURE/floor2.png" },
	{ TYPE_DOOR,			"data/TEXTURE/door1.png" },
	{ TYPE_CEILING,			"data/TEXTURE/ceiling1.png" },
	{ TYPE_CEILING2,		"data/TEXTURE/ceiling2.png" },
	{ TYPE_WATER,			"data/TEXTURE/water.png" },
	{ TYPE_SWITCH,			"data/TEXTURE/switch.png" },
	{ TYPE_SWITCH_BODY,		"data/TEXTURE/switch_body.png" },
	{ TYPE_BRIDGE,			"data/TEXTURE/bridge.png" },
	{ TYPE_DOOR_TOP,		"data/TEXTURE/door_top.png" },
	{ TYPE_DOOR_SIDE,		"data/TEXTURE/door_left.png" },
	{ TYPE_PILLAR,			"data/TEXTURE/pillar.png" },
	{ TYPE_BLOCK,			"data/TEXTURE/block.png" },
	{ TYPE_FENCE,			"data/TEXTURE/fence.png" },
	{ TYPE_FENCE_PART,		"data/TEXTURE/fence_part.png" },
	{ TYPE_BRIDGE2,			"data/TEXTURE/bridge2.png" },
	{ TYPE_TARGET,			"data/TEXTURE/target.png" },
	{ TYPE_SWITCH2,			"data/TEXTURE/controlswitch.png" },
	{ TYPE_DOOR2,			"data/TEXTURE/door2.png" },
	{ TYPE_MASK,			"data/TEXTURE/mask.png" },
	{ TYPE_SWORD,			"data/TEXTURE/sword.png" },
	{ TYPE_SWITCH3,			"data/TEXTURE/controlswitch2.png" },
	{ TYPE_BAR,				"data/TEXTURE/bar.png" },
	{ TYPE_BRIDGE3,			"data/TEXTURE/bridge3.png" },
	{ TYPE_FIRE_STATUE,		"data/TEXTURE/fire_statue.png" },
	{ TYPE_WALL5,			"data/TEXTURE/wall5.png" },
	{ TYPE_FLOOR3,			"data/TEXTURE/floor3.png" },
	{ TYPE_TURN_FIRE_STATUE,"data/TEXTURE/turn_fire_statue.png" },
	{ TYPE_BLOCK2,			"data/TEXTURE/block2.png" },
	{ TYPE_STAIRS,			"data/TEXTURE/stairs.png" },
	{ TYPE_PILLAR2,			"data/TEXTURE/pillar2.png" },
	{ TYPE_BLOCK3,			"data/TEXTURE/block3.png" },
	{ TYPE_FLOOR4,			"data/TEXTURE/floor4.png" },
	{ TYPE_MOVE_FIRE_STATUE,"data/TEXTURE/fire_statue.png" },
	{ TYPE_TORCH3,			"data/TEXTURE/torch2.png" },
	{ TYPE_NETFLOOR,		"data/TEXTURE/netfloor.png" },
	{ TYPE_KEYFENCE,		"data/TEXTURE/keyfence.png" },
	{ TYPE_KEY,				"data/TEXTURE/key.png" },
	{ TYPE_KEY_PEDESTAL,	"data/TEXTURE/key_pedestal.png" },
	{ TYPE_KEY_DOOR,		"data/TEXTURE/keydoor.png" },
	{ TYPE_SHIELD,			"data/TEXTURE/shield.png" },
	{ TYPE_STATUE,			"data/TEXTURE/statue.png" },
	{ TYPE_STATUE2,			"data/TEXTURE/statue2.png" },
	{ TYPE_EGG,				"data/TEXTURE/egg.png" },
	{ TYPE_DOOR_TRIGGER,	"data/TEXTURE/doorTriggerBlock.png" },
};
//=============================================================================
// �����蔻��̐�������
//=============================================================================
void CBlock::CreatePhysics(const D3DXVECTOR3& pos, const D3DXVECTOR3& size)
{
	ReleasePhysics();

	m_colliderSize = size;

	if (m_Type == TYPE_ROCK || m_Type == TYPE_KEY)
	{
		float radius = std::max(std::max(size.x, size.y), size.z) * 0.5f;
		m_pShape = new btSphereShape(radius);
	}
	else if (m_Type == TYPE_AXE)
	{
		btCompoundShape* compound = new btCompoundShape();

		// �O�̎q�V�F�C�v���N���A
		m_childShapes.clear();

		// Handle
		btBoxShape* boxHandle = new btBoxShape(btVector3(
			m_colliderHandle.size.x * 0.5f,
			m_colliderHandle.size.y * 0.5f,
			m_colliderHandle.size.z * 0.5f));
		btTransform transHandle;

		transHandle.setIdentity();
		transHandle.setOrigin(btVector3(
			m_colliderHandle.offset.x,
			m_colliderHandle.offset.y,
			m_colliderHandle.offset.z));

		compound->addChildShape(transHandle, boxHandle);
		m_childShapes.push_back(boxHandle); // ��� delete ����p

		// Blade
		btBoxShape* boxBlade = new btBoxShape(btVector3(
			m_colliderBlade.size.x * 0.5f,
			m_colliderBlade.size.y * 0.5f,
			m_colliderBlade.size.z * 0.5f));
		btTransform transBlade;

		transBlade.setIdentity();
		transBlade.setOrigin(btVector3(
			m_colliderBlade.offset.x,
			m_colliderBlade.offset.y,
			m_colliderBlade.offset.z));

		compound->addChildShape(transBlade, boxBlade);
		m_childShapes.push_back(boxBlade); // ��� delete ����p

		m_pShape = compound;
	}
	else
	{
		btVector3 halfExtents(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
		m_pShape = new btBoxShape(halfExtents);
	}

	btTransform transform;
	transform.setIdentity();
	btVector3 origin(pos.x + m_colliderOffset.x, pos.y + m_colliderOffset.y, pos.z + m_colliderOffset.z);
	transform.setOrigin(origin);

	// ��]�𔽉f
	D3DXVECTOR3 euler = GetRot();
	D3DXMATRIX matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot, euler.y, euler.x, euler.z);

	D3DXQUATERNION dq;
	D3DXQuaternionRotationMatrix(&dq, &matRot);
	btQuaternion quat(dq.x, dq.y, dq.z, dq.w);
	transform.setRotation(quat);

	// �ҏW���͋����I�ɓ��I�i�L�l�}�e�B�b�N�p�j
	btScalar mass = m_isEditMode ? GetMass() : (!IsDynamicBlock() ? 0.0f : GetMass());
	btVector3 inertia(0, 0, 0);

	if (mass != 0.0f)
	{
		m_pShape->calculateLocalInertia(mass, inertia);
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, m_pShape, inertia);

	m_pRigidBody = new btRigidBody(rbInfo);
	m_pRigidBody->setUserPointer(this);
	m_pRigidBody->setActivationState(DISABLE_DEACTIVATION);// �X���[�v��Ԃɂ��Ȃ�

	int flags = m_pRigidBody->getCollisionFlags();

	if (m_Type == TYPE_WATER)
	{
		flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
		m_pRigidBody->setCollisionFlags(flags);
	}

	m_pRigidBody->setAngularFactor(GetAngularFactor());
	m_pRigidBody->setFriction(1.0f);		// ���C
	m_pRigidBody->setRollingFriction(GetRollingFriction());	// �]���薀�C
	m_pRigidBody->setDamping(0.1f, 0.5f);	// linearDamping, angularDamping

	btDiscreteDynamicsWorld* pWorld = CManager::GetPhysicsWorld();

	if (pWorld != NULL)
	{
		pWorld->addRigidBody(m_pRigidBody);
	}
}
//=============================================================================
// �����R���C�_�[�̐�������
//=============================================================================
void CBlock::CreatePhysicsFromParts(void)
{
	ReleasePhysics();

	btCompoundShape* compound = new btCompoundShape();

	// ���S�̂��ߑO�̎q�V�F�C�v���X�g���N���A
	m_childShapes.clear();

	// �_�iHandle�j
	btBoxShape* boxHandle = new btBoxShape(btVector3(
		m_colliderHandle.size.x * 0.5f,
		m_colliderHandle.size.y * 0.5f,
		m_colliderHandle.size.z * 0.5f));

	btTransform transHandle;
	transHandle.setIdentity();
	transHandle.setOrigin(btVector3(
		m_colliderHandle.offset.x,
		m_colliderHandle.offset.y,
		m_colliderHandle.offset.z));

	compound->addChildShape(transHandle, boxHandle);
	m_childShapes.push_back(boxHandle); // ��� delete ���邽�ߋL�^

	// �n�iBlade�j
	btBoxShape* boxBlade = new btBoxShape(btVector3(
		m_colliderBlade.size.x * 0.5f,
		m_colliderBlade.size.y * 0.5f,
		m_colliderBlade.size.z * 0.5f));

	btTransform transBlade;
	transBlade.setIdentity();
	transBlade.setOrigin(btVector3(
		m_colliderBlade.offset.x,
		m_colliderBlade.offset.y,
		m_colliderBlade.offset.z));

	compound->addChildShape(transBlade, boxBlade);
	m_childShapes.push_back(boxBlade); // ��� delete ���邽�ߋL�^

	m_pShape = compound;

	// ���[���h�ϊ��i�ʒu�j
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(
		GetPos().x + m_colliderOffset.x,
		GetPos().y + m_colliderOffset.y,
		GetPos().z + m_colliderOffset.z));

	btScalar mass = !IsDynamicBlock() ? 0.0f : 3.0f;
	btVector3 inertia(0, 0, 0);

	if (mass != 0.0f)
	{
		m_pShape->calculateLocalInertia(mass, inertia);
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, m_pShape, inertia);

	m_pRigidBody = new btRigidBody(rbInfo);
	m_pRigidBody->setUserPointer(this);

	m_pRigidBody->setLinearFactor(btVector3(1, 1, 1));     // ��������
	m_pRigidBody->setAngularFactor(btVector3(1, 1, 1));    // ��]�����鎲
	m_pRigidBody->setFriction(0.5f);
	m_pRigidBody->setDamping(0.05f, 0.05f);
	m_pRigidBody->setGravity(btVector3(0, -59.8f, 0));
	m_pRigidBody->setActivationState(DISABLE_DEACTIVATION);

	btDiscreteDynamicsWorld* pWorld = CManager::GetPhysicsWorld();
	if (pWorld != NULL)
	{
		pWorld->addRigidBody(m_pRigidBody);
	}
}
//=============================================================================
// �X�P�[���ɂ��R���C�_�[�̐�������
//=============================================================================
void CBlock::CreatePhysicsFromScale(const D3DXVECTOR3& scale)
{
	D3DXVECTOR3 modelSize = GetModelSize();

	D3DXVECTOR3 newColliderSize =
	{
		modelSize.x * scale.x,
		modelSize.y * scale.y,
		modelSize.z * scale.z
	};

	CreatePhysics(GetPos(), newColliderSize); // �Đ���
}
//=============================================================================
// �����蔻��̎蓮�ݒ�p
//=============================================================================
void CBlock::SetColliderManual(const D3DXVECTOR3& newSize)
{
	m_colliderSize = newSize;

	ReleasePhysics();							// �ȑO�̍��̂��폜

	CreatePhysics(GetPos(), m_colliderSize);	// �Đ���
}
//=============================================================================
// Physics�̔j��
//=============================================================================
void CBlock::ReleasePhysics(void)
{
	auto world = CManager::GetPhysicsWorld();

	if (m_pRigidBody)
	{
		if (world)
			world->removeRigidBody(m_pRigidBody);

		delete m_pRigidBody->getMotionState();
		delete m_pRigidBody;
		m_pRigidBody = NULL;
	}

	// �qShape�iAXE�Ȃǁj�𖾎��I�ɍ폜
	for (btCollisionShape* shape : m_childShapes)
	{
		delete shape;
	}
	m_childShapes.clear();

	if (m_pShape)
	{
		delete m_pShape;
		m_pShape = NULL;
	}
}
//=============================================================================
// �����R���C�_�[�̔��菈��
//=============================================================================
bool CBlock::IsCompoundCollider(void) const
{
	if (!m_pRigidBody)
	{
		return false;
	}

	btCollisionShape* shape = m_pRigidBody->getCollisionShape();

	if (!shape)
	{
		return false;
	}

	return (shape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE);
}
//=============================================================================
// �G�f�B�^�[�����ǂ����ŃL�l�}�e�B�b�N�ɂ��邩���肷�鏈��
//=============================================================================
void CBlock::SetEditMode(bool enable)
{
	m_isEditMode = enable;

	if (!m_pRigidBody)
	{
		return;
	}

	if (enable)
	{
		m_pRigidBody->setCollisionFlags(m_pRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		m_pRigidBody->setActivationState(DISABLE_DEACTIVATION);
	}
	else
	{
		m_pRigidBody->setCollisionFlags(m_pRigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		m_pRigidBody->setActivationState(ACTIVE_TAG);
	}
}
//=============================================================================
// ���[���h�}�g���b�N�X�̎擾
//=============================================================================
D3DXMATRIX CBlock::GetWorldMatrix(void)
{
	D3DXMATRIX matScale, matRot, matTrans;

	// �X�P�[���s��
	D3DXVECTOR3 scale = GetSize(); // �g�嗦
	D3DXMatrixScaling(&matScale, scale.x, scale.y, scale.z);

	// ��]�s��
	D3DXVECTOR3 rot = GetRot(); // ���W�A���p
	D3DXMatrixRotationYawPitchRoll(&matRot, rot.y, rot.x, rot.z);

	// ���s�ړ��s��
	D3DXVECTOR3 pos = GetPos();
	D3DXMatrixTranslation(&matTrans, pos.x, pos.y, pos.z);

	// �����FS * R * T
	D3DXMATRIX world = matScale * matRot * matTrans;

	return world;
}