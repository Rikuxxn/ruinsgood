//=============================================================================
//
// �}�l�[�W���[���� [manager.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "manager.h"
#include "renderer.h"
#include "sound.h"
#include "game.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
CRenderer* CManager::m_pRenderer = NULL;
CInputKeyboard* CManager::m_pInputKeyboard = NULL;
CInputJoypad* CManager::m_pInputJoypad = NULL;
CInputMouse* CManager::m_pInputMouse = NULL;
CSound* CManager::m_pSound = NULL;
CTexture* CManager::m_pTexture = NULL;
CCamera* CManager::m_pCamera = NULL;
CLight* CManager::m_pLight = NULL;

CScene* CManager::m_pScene = NULL;
CFade* CManager::m_pFade = NULL;

btDiscreteDynamicsWorld* CManager::m_pDynamicsWorld = NULL;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CManager::CManager()
{
	// �l�̃N���A
	m_fps					  = 0;
	m_pBroadphase			  = NULL;	// �Փ˔���̃N���X�ւ̃|�C���^
	m_pCollisionConfiguration = NULL;	// �Փˌ��o�̐ݒ���Ǘ�����N���X�ւ̃|�C���^
	m_pDispatcher			  = NULL;	// ���ۂɏՓ˔��菈�������s����N���X�ւ̃|�C���^
	m_pSolver				  = NULL;	// �����V�~�����[�V�����̐���\���o�[�ւ̃|�C���^
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CManager::~CManager()
{

}
//=============================================================================
// ����������
//=============================================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd)
{
	// �����_���[�̐���
	m_pRenderer = new CRenderer;

	// �����_���[�̏���������
	if (FAILED(m_pRenderer->Init(hWnd, TRUE)))
	{
		return -1;
	}

	// �L�[�{�[�h�̐���
	m_pInputKeyboard = new CInputKeyboard;

	// �L�[�{�[�h�̏���������
	if (FAILED(m_pInputKeyboard->Init(hInstance, hWnd)))
	{
		return -1;
	}

	// �W���C�p�b�h�̐���
	m_pInputJoypad = new CInputJoypad;

	// �W���C�p�b�h�̏���������
	if (FAILED(m_pInputJoypad->Init()))
	{
		return E_FAIL;
	}

	// �}�E�X�̐���
	m_pInputMouse = new CInputMouse;

	// �}�E�X�̏���������
	if (FAILED(m_pInputMouse->Init(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	// �T�E���h�̐���
	m_pSound = new CSound;

	// �T�E���h�̏���������
	if (FAILED(m_pSound->Init(hWnd)))
	{
		return E_FAIL;
	}


	// Bullet�������[���h�̐���
	m_pBroadphase = new btDbvtBroadphase();
	m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);
	m_pSolver = new btSequentialImpulseConstraintSolver();

	m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);

	// �d�͂�ݒ�
	m_pDynamicsWorld->setGravity(btVector3(0, -659.8f, 0));

	// �J�����̐���
	m_pCamera = new CCamera;

	// �J�����̏���������
	m_pCamera->Init();

	// ���C�g�̐���
	m_pLight = new CLight;

	// ���C�g�̏���������
	m_pLight->Init();

	// �e�N�X�`���̐���
	m_pTexture = new CTexture;

	// �e�N�X�`���̓ǂݍ���
	m_pTexture->Load();

	// �^�C�g�����
	m_pFade = CFade::Create(CScene::MODE_TITLE);

	// �^�C�g�����
	m_pScene = CScene::Create(CScene::MODE_TITLE);

	// �^�C�g����ʂ�������
	if (m_pScene->GetMode() == MODE_TITLE)
	{// �J�����̈ʒu�̐ݒ�
		D3DXVECTOR3 posV(D3DXVECTOR3(-1235.5f, 292.0f, -2220.2f));
		D3DXVECTOR3 posR(D3DXVECTOR3(-1524.1f, 142.5f, -1489.2f));

		m_pCamera->SetPosV(posV);
		m_pCamera->SetPosR(posR);
		m_pCamera->SetRot(D3DXVECTOR3(0.19f, 2.77f, 0.0f));
		m_pCamera->SetDis(sqrtf(
			((posV.x - posR.x) * (posV.x - posR.x)) +
			((posV.y - posR.y) * (posV.y - posR.y)) +
			((posV.z - posR.z) * (posV.z - posR.z))));
	}

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CManager::Uninit(void)
{
	// ���ׂẴI�u�W�F�N�g�̔j��
	CObject::ReleaseAll();

	// �e�N�X�`���̔j��
	if (m_pTexture != NULL)
	{
		// �S�Ẵe�N�X�`���̔j��
		m_pTexture->Unload();

		delete m_pTexture;
		m_pTexture = NULL;
	}

	// �L�[�{�[�h�̏I������
	m_pInputKeyboard->Uninit();

	// �W���C�p�b�h�̏I������
	m_pInputJoypad->Uninit();

	// �}�E�X�̏I������
	m_pInputMouse->Uninit();

	// �T�E���h�̏I������
	m_pSound->Uninit();

	// �L�[�{�[�h�̔j��
	if (m_pInputKeyboard != NULL)
	{
		// �����_���[�̏I������
		m_pInputKeyboard->Uninit();

		delete m_pInputKeyboard;
		m_pInputKeyboard = NULL;
	}

	// �W���C�p�b�h�̔j��
	if (m_pInputJoypad != NULL)
	{
		// �W���C�p�b�h�̏I������
		m_pInputJoypad->Uninit();

		delete m_pInputJoypad;
		m_pInputJoypad = NULL;
	}

	// �}�E�X�̔j��
	if (m_pInputMouse != NULL)
	{
		// �}�E�X�̏I������
		m_pInputMouse->Uninit();

		delete m_pInputMouse;
		m_pInputMouse = NULL;
	}

	// �T�E���h�̔j��
	if (m_pSound != NULL)
	{
		// �}�E�X�̏I������
		m_pSound->Uninit();

		delete m_pSound;
		m_pSound = NULL;
	}

	// �J�����̔j��
	if (m_pCamera != NULL)
	{
		delete m_pCamera;
		m_pCamera = NULL;
	}

	// ���C�g�̔j��
	if (m_pLight != NULL)
	{
		delete m_pLight;
		m_pLight = NULL;
	}

	if (m_pDynamicsWorld)
	{
		// ��Ƀ��[���h����SRigidBody���O����delete
		int num = m_pDynamicsWorld->getNumCollisionObjects();
		for (int i = num - 1; i >= 0; i--)
		{
			btCollisionObject* obj = m_pDynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);

			if (body && body->getMotionState())
			{
				delete body->getMotionState();
				m_pDynamicsWorld->removeCollisionObject(obj);
				delete obj;
			}
		}
		delete m_pDynamicsWorld;  delete m_pSolver;
		delete m_pDispatcher;     delete m_pCollisionConfiguration;
		delete m_pBroadphase;
	}

	// �t�F�[�h�̔j��
	if (m_pFade != NULL)
	{
		// �t�F�[�h�̏I������
		m_pFade->Uninit();

		delete m_pFade;
		m_pFade = NULL;
	}

	// �����_���[�̔j��
	if (m_pRenderer != NULL)
	{
		// �����_���[�̏I������
		m_pRenderer->Uninit();

		delete m_pRenderer;
		m_pRenderer = NULL;
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void CManager::Update(void)
{
	// �L�[�{�[�h�̍X�V
	m_pInputKeyboard->Update();

	// �t�F�[�h�̍X�V����
	if (m_pFade != NULL)
	{
		// �t�F�[�h�̍X�V����
		m_pFade->Update();
	}

	m_pDynamicsWorld->stepSimulation((btScalar)m_fps);

	// �W���C�p�b�h�̍X�V
	m_pInputJoypad->Update();

	// �}�E�X�̍X�V
	m_pInputMouse->Update();

	// �J�����̍X�V
	m_pCamera->Update();

	// ���C�g�̍X�V
	m_pLight->Update();

	// �����_���[�̍X�V
	m_pRenderer->Update();
}
//=============================================================================
// �`�揈��
//=============================================================================
void CManager::Draw(void)
{
	// �����_���[�̕`��
	m_pRenderer->Draw(m_fps);
}
//=============================================================================
// ���[�h�̐ݒ�
//=============================================================================
void CManager::SetMode(CScene::MODE mode)
{
	// �J�����̏���������
	m_pCamera->Init();

	// �T�E���h�̒�~
	m_pSound->Stop();

	if (m_pScene != NULL)
	{	// ���݂̃��[�h�j��
		m_pScene->Uninit();
	}

	// �S�ẴI�u�W�F�N�g��j��
	CObject::ReleaseAll();

	// �V�������[�h�̐���
	m_pScene = CScene::Create(mode);
}
//=============================================================================
// ���݂̃��[�h�̎擾
//=============================================================================
CScene::MODE CManager::GetMode(void)
{
	return m_pScene->GetMode();
}
