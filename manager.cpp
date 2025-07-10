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
#include "object2D.h"
#include "sound.h"
#include "object3D.h"
#include "objectX.h"
#include "objectBillboard.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
CRenderer* CManager::m_pRenderer = NULL;
CObject2D* CManager::m_pObject2D = NULL;
CInputKeyboard* CManager::m_pInputKeyboard = NULL;
CInputJoypad* CManager::m_pInputJoypad = NULL;
CInputMouse* CManager::m_pInputMouse = NULL;
CSound* CManager::m_pSound = NULL;
CPlayer* CManager::m_pPlayer = NULL;
CEffect* CManager::m_pEffect = NULL;
CScore* CManager::m_pScore = NULL;
CTime* CManager::m_pTime = NULL;
CColon* CManager::m_pColon = NULL;
CPause* CManager::m_pPause = NULL;
CTexture* CManager::m_pTexture = NULL;
CCamera* CManager::m_pCamera = NULL;
CLight* CManager::m_pLight = NULL;
CModel* CManager::m_pModel = NULL;
CMotion* CManager::m_pMotion = NULL;
CObjectX* CManager::m_pObjectX = NULL;
CBlockManager* CManager::m_pBlockManager = NULL;
CBlock* CManager::m_pBlock = NULL;
CImGuiManager* CManager::m_pImGuiManager = NULL;

bool CManager::m_isPaused = false;
btDiscreteDynamicsWorld* CManager::m_pDynamicsWorld = NULL;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CManager::CManager()
{
	// �l�̃N���A
	m_fps					  = 0;
	m_isPaused				  = false;
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

	// �u���b�N�}�l�[�W���[�̐���
	m_pBlockManager = new CBlockManager;

	// �u���b�N�}�l�[�W���[�̏�����
	m_pBlockManager->Init();

	// �v���C���[�̐���
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 100.0f, -300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// JSON�̓ǂݍ���
	m_pBlockManager->LoadFromJson("data/block_info.json");

	//// �r���{�[�h�̐���
	//CObjectBillboard::Create(CObjectBillboard::TYPE_ONE, D3DXVECTOR3(0.0f, 20.0f, -350.0f), 120.0f, 60.0f);

	//// �X�R�A�̐���
	//m_pScore = CScore::Create(920.0f, 10.0f, 42.0f, 58.0f);

	float fTimePosX = 760.0f;
	float fTimeWidth = 42.0f;
	float fTimeHeight = 58.0f;

	// �^�C���̐���
	m_pTime = CTime::Create(12, 15, fTimePosX, 10.0f, fTimeWidth, fTimeHeight);

	// �R�����̐���
	m_pColon = CColon::Create(D3DXVECTOR3(fTimePosX + 2 * fTimeWidth, 10.0f, 0.0f), fTimeWidth / 2, fTimeHeight);

	// �|�[�Y�̐���
	m_pPause = CPause::Create(D3DXVECTOR3(490.0f, 340.0f, 0.0f), 400.0f, 90.0f);

	//m_pSound->Play(CSound::SOUND_LABEL_GAMEBGM);

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

	// �u���b�N�}�l�[�W���[�̔j��
	if (m_pBlockManager != NULL)
	{
		m_pBlockManager->Uninit();

		delete m_pBlockManager;
		m_pBlockManager = NULL;
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

	// P�L�[�Ń|�[�YON/OFF
	if (m_pInputKeyboard->GetTrigger(DIK_P))
	{
		m_isPaused = !m_isPaused;
	}

	// �|�[�Y���̓Q�[���X�V���Ȃ�
	if (m_isPaused == true)
	{
		// �|�[�Y�̍X�V����
		m_pPause->Update();

		return;
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

#ifdef _DEBUG
	// �u���b�N�}�l�[�W���[�̏��̍X�V
	m_pBlockManager->UpdateInfo();

	// �u���b�N�}�l�[�W���[�̃h���b�O�����̍X�V
	m_pBlockManager->UpdateDraggingBlock();
#endif
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

	// �|�[�Y����������
	if (m_isPaused)
	{
		// �|�[�Y�̕`�揈��
		m_pPause->Draw();
	}
}
//=============================================================================
// �|�[�Y�����ǂ����̎擾
//=============================================================================
bool CManager::GetisPaused(void)
{
	return m_isPaused;
}