//=============================================================================
//
// �u���b�N���� [block.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "block.h"
#include "manager.h"
#include <algorithm>

using namespace std;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CBlock::CBlock()
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
// �f�X�g���N�^
//=============================================================================
CBlock::~CBlock()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CBlock* CBlock::Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size, TYPE type)
{
	CBlock* pBlock = NULL;

	// �^�C�v���Ƃɔh���N���X�֕���
	switch (type)
	{
	case TYPE_WATER:
		pBlock = new CWaterBlock();
		break;
	case TYPE_DOOR:
		pBlock = new CDoorBlock();
		break;
	case TYPE_SWITCH:
		pBlock = new CSwitchBlock();
		break;
	default:
		pBlock = new CBlock();
		break;
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
// �^�C�v�ł̐�������
//=============================================================================
CBlock* CBlock::CreateFromType(TYPE type, D3DXVECTOR3 pos)
{
	const char* path = "";
	D3DXVECTOR3 size = { 1.0f, 1.0f, 1.0f };
	D3DXVECTOR3 rot = { 0.0f, 0.0f, 0.0f };

	switch (type)
	{
	case TYPE_WOODBOX:
		path = "data/MODELS/woodbox_001.x";
		size = { 1.5f, 1.5f, 1.5f };
		break;

	case TYPE_WALL:
		path = "data/MODELS/wall_01.x";
		size = { 1.0f, 1.0f, 1.0f };
		break;

	case TYPE_WALL2:
		path = "data/MODELS/wall_02.x";
		size = { 1.0f, 1.0f, 1.0f };
		break;

	case TYPE_WALL3:
		path = "data/MODELS/wall_03.x";
		size = { 1.0f, 1.0f, 1.0f };
		break;

	case TYPE_WALL4:
		path = "data/MODELS/wall_04.x";
		size = { 1.0f, 1.0f, 1.0f };
		break;

	case TYPE_AXE:
		path = "data/MODELS/Axe_01.x";
		size = { 2.0f, 2.0f, 2.0f };
		break;

	case TYPE_RAFT:
		path = "data/MODELS/ikada.x";
		size = { 1.0f, 1.0f, 1.0f };
		break;

	case TYPE_ROCK:
		path = "data/MODELS/Rock_001.x";
		size = { 1.0f, 1.0f, 1.0f };
		break;

	case TYPE_DOOR:
		path = "data/MODELS/door_01.x";
		size = { 1.0f, 1.0f, 1.0f };
		break;

	default:
		return NULL;
	}

	CBlock* block = CBlock::Create(path, pos, rot, size, type);

	return block;
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
	if (IsStaticBlock() || IsEditMode())
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

		m_pRigidBody->setWorldTransform(trans);
		m_pRigidBody->getMotionState()->setWorldTransform(trans);

	}
	else
	{
		// ���I�u���b�N�͕����̕ϊ����擾���Ĕ��f

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

		// cos(pitch) ��0�ɋ߂���gimbal lock�Ȃ̂ŁA���̉������
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

	if (m_pRigidBody)
	{
		// �R���C�_�[�̕`��
		m_pDebug3D->DrawBlockCollider(m_pRigidBody, D3DXCOLOR(0.0f, 1.0f, 0.3f, 1.0f));
	}

#endif

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
	switch (type)
	{
	case TYPE_WOODBOX: 
		return "data/TEXTURE/woodbox.png";

	case TYPE_WALL: 
		return "data/TEXTURE/wall1.png";

	case TYPE_WALL2:
		return "data/TEXTURE/wall2.png";

	case TYPE_WALL3:
		return "data/TEXTURE/wall3.png";

	case TYPE_WALL4:
		return "data/TEXTURE/wall4.png";

	case TYPE_AXE: 
		return "data/TEXTURE/Axe.png";

	case TYPE_RAFT: 
		return "data/TEXTURE/ikada.png";

	case TYPE_ROCK:
		return "data/TEXTURE/rock.png";

	case TYPE_TORCH:
		return "data/TEXTURE/torch1.png";

	case TYPE_TORCH2:
		return "data/TEXTURE/torch2.png";

	case TYPE_FLOOR:
		return "data/TEXTURE/floor1.png";

	case TYPE_FLOOR2:
		return "data/TEXTURE/floor2.png";

	case TYPE_DOOR:
		return "data/TEXTURE/door1.png";

	case TYPE_CEILING:
		return "data/TEXTURE/ceiling1.png";

	case TYPE_CEILING2:
		return "data/TEXTURE/ceiling2.png";

	case TYPE_WATER:
		return "data/TEXTURE/water.png";

	case TYPE_SWITCH:
		return "data/TEXTURE/switch.png";

	case TYPE_SWITCH_BODY:
		return "data/TEXTURE/switch_body.png";

	case TYPE_BRIDGE:
		return "data/TEXTURE/bridge.png";

	case TYPE_DOOR_TOP:
		return "data/TEXTURE/door_top.png";

	case TYPE_DOOR_SIDE:
		return "data/TEXTURE/door_left.png";

	case TYPE_PILLAR:
		return "data/TEXTURE/pillar.png";

	case TYPE_BLOCK:
		return "data/TEXTURE/block.png";

	case TYPE_FENCE:
		return "data/TEXTURE/fence.png";

	case TYPE_FENCE_PART:
		return "data/TEXTURE/fence_part.png";

	default: 
		return "";
	}
}
//=============================================================================
// �����蔻��̐�������
//=============================================================================
void CBlock::CreatePhysics(const D3DXVECTOR3& pos, const D3DXVECTOR3& size)
{
	ReleasePhysics();

	m_colliderSize = size;

	if (m_Type == TYPE_ROCK)
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
	btScalar mass = m_isEditMode ? GetMassByType(m_Type) : (IsStaticBlock() ? 0.0f : GetMassByType(m_Type));
	btVector3 inertia(0, 0, 0);

	if (mass != 0.0f)
	{
		m_pShape->calculateLocalInertia(mass, inertia);
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, m_pShape, inertia);

	m_pRigidBody = new btRigidBody(rbInfo);
	m_pRigidBody->setUserPointer(this);

	int flags = m_pRigidBody->getCollisionFlags();

	if (m_Type == TYPE_WATER)
	{
		flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
		m_pRigidBody->setCollisionFlags(flags);
	}

	if (m_Type == TYPE_PILLAR)
	{
		m_pRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 1.0f));
	}
	else
	{
		m_pRigidBody->setFriction(1.0f);		// ���C
		m_pRigidBody->setRollingFriction(0.7f);	// �]���薀�C
		m_pRigidBody->setDamping(0.1f, 0.5f);	// (linearDamping, angularDamping)
	}


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

	btScalar mass = IsStaticBlock() ? 0.0f : 3.0f;
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
// �R���C�_�[�̃I�t�Z�b�g�̐ݒ�
//=============================================================================
void CBlock::SetColliderOffset(const D3DXVECTOR3& offset)
{
	m_colliderOffset = offset;
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
// �ÓI�ȃu���b�N�����肷�鏈��
//=============================================================================
bool CBlock::IsStaticBlock(void) const
{
	switch (m_Type)
	{
	case TYPE_WALL:
	case TYPE_WALL2:
	case TYPE_WALL3:
	case TYPE_WALL4:
	case TYPE_TORCH:
	case TYPE_FLOOR:
	case TYPE_FLOOR2:
	case TYPE_CEILING:
	case TYPE_CEILING2:
	case TYPE_DOOR:
	case TYPE_SWITCH_BODY:
	case TYPE_DOOR_TOP:
	case TYPE_DOOR_SIDE:
	case TYPE_WATER:
	case TYPE_AXE:
	case TYPE_SWITCH:
	case TYPE_BLOCK:
	case TYPE_FENCE:
	case TYPE_FENCE_PART:
		return true; // �ÓI�i�����Ȃ��j

	default:
		return false; // �����\������
	}
}
//=============================================================================
// �R���C�_�[�T�C�Y�̐ݒ菈��
//=============================================================================
void CBlock::SetColliderSize(const D3DXVECTOR3& size)
{
	m_colliderSize = size;
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
// �u���b�N�^�C�v���Ƃ̎��ʂ�Ԃ�
//=============================================================================
btScalar CBlock::GetMassByType(TYPE type)
{
	switch (type)
	{
	case TYPE_WOODBOX:	return 4.0f;	// �ؔ�
	case TYPE_TORCH2:	return 5.0f;	// �u���^�g�[�`
	case TYPE_PILLAR:	return 55.0f;	// ��
	case TYPE_ROCK:		return 10.0f;	// ��
	case TYPE_BRIDGE:	return 8.0f;	// ��
	case TYPE_RAFT:		return 6.0f;	// �C�J�_
	case TYPE_AXE:		return 80.0f;	// ��
	default:			return 2.0f;	// �f�t�H���g����
	}
}


//=============================================================================
// ���u���b�N�̃R���X�g���N�^
//=============================================================================
CWaterBlock::CWaterBlock()
{
	SetType(TYPE_WATER);
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
	const float B_maxLiftSpeed = 20.0f;

	// ------------------------
	// ���̃u���b�N�𕂂�����
	// ------------------------
	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block == this || block->IsStaticBlock())
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

		btRigidBody* pRigid = block->GetRigidBody();

		if (pRigid && !block->IsStaticBlock())
		{
			btVector3 velocity = pRigid->getLinearVelocity();

			if (velocity.getY() < B_maxLiftSpeed)
			{
				// �����΂���ڕW��������x
				const float targetUpSpeed = 130.0f; // ����X�s�[�h
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
}
//=============================================================================
// ���u���b�N�̃v���C���[���V����
//============================================================================
void CWaterBlock::ApplyToPlayer(void)
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

	// �v���C���[�̕��͐ݒ�
	const float P_waterLiftPower = 18.0f;
	const float P_maxLiftSpeed = 53.0f;

	// ------------------------
	// �v���C���[�𕂂�����
	// ------------------------
	CPlayer* pPlayer = CManager::GetPlayer();

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
			pPlayer->SetInWater(true); // �t���O�p�֐��i���L�ǉ��j

			// �^�C�}�[�X�V
			pPlayer->AddWaterStayTime(1.0f / 60.0f); // ���t���[�����Z
		}
		else
		{
			pPlayer->SetInWater(false);
			pPlayer->ResetWaterStayTime();
		}
	}
}


//=============================================================================
// �h�A�u���b�N�̃R���X�g���N�^
//=============================================================================
CDoorBlock::CDoorBlock()
{
	SetType(TYPE_DOOR);

	// �l�̃N���A
	m_lerpStartPos	= INIT_VEC3;
	m_lerpTargetPos = INIT_VEC3;
	m_lerpTimer		= 0.0f;;
	m_lerpDuration	= 0.0f;
	m_isDoorOpened	= false;
}
//=============================================================================
// �h�A�u���b�N�̃f�X�g���N�^
//=============================================================================
CDoorBlock::~CDoorBlock()
{
	// �Ȃ�
}
//=============================================================================
// �h�A�u���b�N�̍X�V����
//=============================================================================
void CDoorBlock::Update(void)
{
	D3DXVECTOR3 playerPos = CManager::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();

	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 280.0f;// �������鋗��

	if (distance < kTriggerDistance && !m_isDoorOpened)
	{
		m_isDoorOpened = true;	// �t���OON
	}

	if (m_isDoorOpened)
	{
		// �h�A���J��
		D3DXVECTOR3 newPos = GetPos();

		if (newPos.y <= 300.0f)
		{
			newPos.y += 1.0f;
			SetPos(newPos);
		}
		else
		{
			m_isDoorOpened = false;
		}
	}
	else
	{
		// �h�A��߂�
		D3DXVECTOR3 newPos = GetPos();

		if (newPos.y > 90.0f && !m_isDoorOpened)
		{
			newPos.y -= 1.0f;
			SetPos(newPos);
		}
	}

	CBlock::Update(); // ���ʏ���
}


//=============================================================================
// �X�C�b�`�u���b�N�̃R���X�g���N�^
//=============================================================================
CSwitchBlock::CSwitchBlock()
{
	SetType(TYPE_SWITCH);

	// �l�̃N���A
	m_closedPos = INIT_VEC3;
	m_isSwitchOn = false;
}
//=============================================================================
// �X�C�b�`�u���b�N�̃f�X�g���N�^
//=============================================================================
CSwitchBlock::~CSwitchBlock()
{
	// �Ȃ�
}
//=============================================================================
// �X�C�b�`�u���b�N�̍X�V����
//=============================================================================
void CSwitchBlock::Update(void)
{
	m_closedPos = GetPos();

	// �X�C�b�`�� AABB ���擾
	D3DXVECTOR3 swPos = GetPos();
	D3DXVECTOR3 modelSize = GetModelSize(); // �X�C�b�`�̃T�C�Y�i���S���_�j
	D3DXVECTOR3 scale = GetSize();

	D3DXVECTOR3 swSize;

	// �g�嗦��K�p����
	swSize.x = modelSize.x * scale.x;
	swSize.y = modelSize.y * scale.y;
	swSize.z = modelSize.z * scale.z;

	D3DXVECTOR3 swMin = swPos - swSize * 0.5f;
	D3DXVECTOR3 swMax = swPos + swSize * 0.5f;

	float totalMass = 0.0f;

	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block == this || block->IsStaticBlock())
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
		D3DXVECTOR3 pos = swPos;

		// ������Ă���i���ɏ������ށj
		pos.y -= 1.0f; // ���ɒ��߂�

		if (pos.y > 12.0f)// TODO : �����ꉺ����͈͂����߂Ĕ��肷��悤�ɂ���
		{
			SetPos(pos);
		}

		SetEditMode(true); // ���������߂ɃL�l�}�e�B�b�N

		m_isSwitchOn = true;
	}

	if (!m_isSwitchOn)
	{
		return;
	}

	// ���ʏ㏸����
	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() != TYPE_WATER)
		{
			continue;
		}

		D3DXVECTOR3 waterPos = block->GetPos();

		if (waterPos.y < -280.0f)
		{
			// ���X�ɐ��ʂ��グ��
			waterPos.y += 0.5f;
			block->SetPos(waterPos);
		}
	}

	CBlock::Update(); // ���ʏ���
}