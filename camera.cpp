//=============================================================================
//
// �J�������� [camera.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "camera.h"
#include "renderer.h"
#include "manager.h"
#include "game.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CCamera::CCamera()
{
	// �l�̃N���A
	m_posV				= D3DXVECTOR3(0.0f, 0.0f, 0.0f);// ���_
	m_posVDest			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);// �ړI�̎��_
	m_posR				= D3DXVECTOR3(0.0f, 0.0f, 0.0f);// �����_
	m_posRDest			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);// �ړI�̒����_
	m_vecU				= D3DXVECTOR3(0.0f, 0.0f, 0.0f);// ������x�N�g��
	m_mtxProjection		= {};							// �v���W�F�N�V�����}�g���b�N�X
	m_mtxView			= {};							// �r���[�}�g���b�N�X
	m_rot				= D3DXVECTOR3(0.0f, 0.0f, 0.0f);// ����
	m_fDistance			= 0.0f;							// ���_���璍���_�̋���
#ifdef _DEBUG
	m_Mode = MODE_EDIT;									// �J�����̃��[�h
#else
	m_Mode = MODE_GAME;
#endif
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CCamera::~CCamera()
{
	// �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CCamera::Init(void)
{	
	m_posV = D3DXVECTOR3(0.0f, 80.0f, -540.0f);
	m_posR = D3DXVECTOR3(0.0f, 80.0f, 0.0f);
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);// �Œ�ł���
	m_rot = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);
	m_fDistance = sqrtf(
		((m_posV.x - m_posR.x) * (m_posV.x - m_posR.x)) +
		((m_posV.y - m_posR.y) * (m_posV.y - m_posR.y)) +
		((m_posV.z - m_posR.z) * (m_posV.z - m_posR.z)));
	
#ifdef _DEBUG
	m_Mode = MODE_EDIT;									// �J�����̃��[�h
#else
	m_Mode = MODE_GAME;
#endif

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CCamera::Uninit(void)
{


}
//=============================================================================
// �X�V����
//=============================================================================
void CCamera::Update(void)
{
	// �^�C�g����ʂ�������
	if (CManager::GetMode() == MODE_TITLE)
	{// �J�����̈ʒu�̐ݒ�
		m_posV = D3DXVECTOR3(-1235.5f, 292.0f, -2220.2f);
		m_posR = D3DXVECTOR3(-1524.1f, 142.5f, -1489.2f);
		m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);// �Œ�ł���
		m_rot = D3DXVECTOR3(0.19f, 2.77f, 0.0f);
		m_fDistance = sqrtf(
			((m_posV.x - m_posR.x) * (m_posV.x - m_posR.x)) +
			((m_posV.y - m_posR.y) * (m_posV.y - m_posR.y)) +
			((m_posV.z - m_posR.z) * (m_posV.z - m_posR.z)));
	}

#ifdef _DEBUG
	// �L�[�{�[�h�̎擾
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	if (m_Mode == MODE_EDIT && pInputKeyboard->GetTrigger(DIK_C) == true)
	{
		// �Q�[���J����
		m_Mode = MODE_GAME;
	}
	else if (m_Mode == MODE_GAME && pInputKeyboard->GetTrigger(DIK_C) == true)
	{
		// �G�f�B�^�[�J����
		m_Mode = MODE_EDIT;
	}
#endif

	switch (m_Mode)
	{
	case MODE_EDIT:

#ifdef _DEBUG
		if (CManager::GetMode() == MODE_TITLE)
		{
			return;
		}

		// �G�f�B�^�[�J�����̏���
		EditCamera();
#endif
		break;
	case MODE_GAME:
		// �Q�[���̃J��������
		GameCamera();

		break;
	}

}
//=============================================================================
// �J�������̍X�V����
//=============================================================================
void CCamera::UpdateInfo(void)
{
	if (ImGui::TreeNode("Camera Info"))
	{
		ImGui::Text("Change Camera Mode : [Press C]");

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		ImGui::Text("PosV Move : [Press L ALT & MouseButton L]");
		ImGui::Text("PosR Move : [Press MouseButton R]");
		ImGui::Text("Zoom : [MouseWheel]");

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���_�̈ʒu
		ImGui::Text("PosV [X:%.1f Y:%.1f Z:%.1f]", m_posV.x, m_posV.y, m_posV.z);

		// �����_�̈ʒu
		ImGui::Text("PosR [X:%.1f Y:%.1f Z:%.1f]", m_posR.x, m_posR.y, m_posR.z);

		// �p�x
		ImGui::Text("Rot  [X:%.2f Y:%.2f Z:%.2f]", m_rot.x, m_rot.y, m_rot.z);

		ImGui::TreePop(); // ����
	}
}
//=============================================================================
// �J�����̐ݒ菈��
//=============================================================================
void CCamera::SetCamera(void)
{
	// �f�o�C�X�̎擾
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	//�r���[�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxView);

	//�r���[�}�g���b�N�X�̍쐬
	D3DXMatrixLookAtLH(&m_mtxView,
		&m_posV,
		&m_posR,
		&m_vecU);

	//�r���[�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);

	//�v���W�F�N�V�����}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxProjection);

	//�v���W�F�N�V�����}�g���b�N�X�̍쐬
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
		D3DXToRadian(80.0f),						// ����p
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, // �A�X�y�N�g��
		1.0f,										// �߃N���b�v��
		2500.0f);									// ���N���b�v��

	//�v���W�F�N�V�����}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);
}
//=============================================================================
// �G�f�B�^�[�J�����̏���
//=============================================================================
void CCamera::EditCamera(void)
{
	// �L�[�{�[�h�̎擾
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	// �}�E�X�̎擾
	CInputMouse* pInputMouse = CManager::GetInputMouse();

	// �}�E�X�J�[�\����\������
	pInputMouse->SetCursorVisibility(true);

	// ���݂̃J�[�\���ʒu���擾
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	// �O�t���[������̃}�E�X�ړ��ʂ��擾
	static POINT prevCursorPos = { cursorPos.x, cursorPos.y };
	float deltaX = (float)(cursorPos.x - prevCursorPos.x);
	float deltaY = (float)(cursorPos.y - prevCursorPos.y);

	// ���݂̃J�[�\���ʒu��ۑ��i���̃t���[���ł̔�r�p�j
	prevCursorPos = cursorPos;

	// �}�E�X���x
	const float mouseSensitivity = 0.004f;

	deltaX *= mouseSensitivity;
	deltaY *= mouseSensitivity;

	//====================================
	// �}�E�X�z�C�[���ŃY�[���C���E�A�E�g
	//====================================
	int wheel = pInputMouse->GetWheel();
	const float zoomSpeed = 15.0f;

	if (wheel != 0)
	{
		m_fDistance -= wheel * zoomSpeed;

		// �J������������
		if (m_fDistance < 100.0f)
		{
			m_fDistance = 100.0f;
		}
		if (m_fDistance > 800.0f)
		{
			m_fDistance = 800.0f;
		}
	}

	if (pInputKeyboard->GetPress(DIK_LALT) && pInputMouse->GetPress(0)) // ���N���b�N�����Ȃ���}�E�X�ړ� �� ���_��]
	{
		m_rot.y += deltaX; // ������]
		m_rot.x += deltaY; // ������]

		//�p�x�̐��K��
		if (m_rot.y > D3DX_PI)
		{
			m_rot.y -= D3DX_PI * 2.0f;
		}
		else if (m_rot.y < -D3DX_PI)
		{
			m_rot.y += D3DX_PI * 2.0f;
		}

		// ������]�̐���
		if (m_rot.x > 1.57f)
		{
			m_rot.x = 1.57f;
		}

		if (m_rot.x < -1.57f)
		{
			m_rot.x = -1.57f;
		}

		// ���_�̍X�V�i�J�����̕�����K�p�j
		m_posV.x = m_posR.x + sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posV.y = m_posR.y + sinf(m_rot.x) * m_fDistance;
		m_posV.z = m_posR.z + cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;

	}
	else if (pInputMouse->GetPress(1)) // �E�N���b�N�����Ȃ���}�E�X�ړ� �� �����_��]
	{
		m_rot.y += deltaX; // ������]
		m_rot.x += deltaY; // ������]

		//�p�x�̐��K��
		if (m_rot.y > D3DX_PI)
		{
			m_rot.y -= D3DX_PI * 2.0f;
		}
		else if (m_rot.y < -D3DX_PI)
		{
			m_rot.y += D3DX_PI * 2.0f;
		}

		// ������]�̐���
		if (m_rot.x > 1.57f)
		{
			m_rot.x = 1.57f;
		}
		if (m_rot.x < -1.57f)
		{
			m_rot.x = -1.57f;
		}

		// �����_�̍X�V
		m_posR.x = m_posV.x - sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posR.y = m_posV.y - sinf(m_rot.x) * m_fDistance;
		m_posR.z = m_posV.z - cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	}
	else
	{
		// ���͂��Ȃ��ꍇ�ł��Y�[�����f�̂��߂Ɏ��_���X�V
		m_posV.x = m_posR.x + sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posV.y = m_posR.y + sinf(m_rot.x) * m_fDistance;
		m_posV.z = m_posR.z + cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	}

	// �����_�̍X�V
	m_posR.x = m_posV.x - sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	m_posR.y = m_posV.y - sinf(m_rot.x) * m_fDistance;
	m_posR.z = m_posV.z - cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
}
//=============================================================================
// �Q�[���J�����̏���
//=============================================================================
void CCamera::GameCamera(void)
{
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	// ����������Z������
	static bool firstTime = true;

	if (firstTime)
	{
		m_fDistance = 220.0f;
		firstTime = false;
	}

	// �}�E�X�̎擾
	CInputMouse* pInputMouse = CManager::GetInputMouse();

	// �X�e�B�b�N�̎擾
	XINPUT_STATE* pStick = CInputJoypad::GetStickAngle();

	// �v���C���[�̎擾
	CPlayer* pPlayer = CGame::GetPlayer();

	if (pPlayer != NULL)
	{
		// �v���C���[�̈ʒu�̎擾
		D3DXVECTOR3 playerPos = pPlayer->GetPos();

		// �}�E�X�̏�Ԃ��擾
		DIMOUSESTATE mouseState;

		// �}�E�X�J�[�\�����\���ɂ���
		pInputMouse->SetCursorVisibility(false);

		// �Q�[���p�b�h�E�X�e�B�b�N�J��������
		if (pStick != NULL)
		{
			// �E�X�e�B�b�N�̒l���擾
			float stickX = pStick->Gamepad.sThumbRX;
			float stickY = pStick->Gamepad.sThumbRY;

			// �f�b�h�]�[������
			const float DEADZONE = 10922.0f;
			if (fabsf(stickX) < DEADZONE)
			{
				stickX = 0.0f;
			}
			if (fabsf(stickY) < DEADZONE)
			{
				stickY = 0.0f;
			}

			// ���K��
			stickX /= 32768.0f;
			stickY /= 32768.0f;

			// �J������]�̍X�V
			CameraWithGamepad(stickX, stickY);
		}
		if (pInputMouse->GetMouseState(&mouseState))
		{
			// �O�t���[���̃J�[�\���ʒu���L�^����ÓI�ϐ�
			static POINT prevCursorPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

			// ���݂̃}�E�X�̈ʒu�̎擾
			POINT cursorPos;
			GetCursorPos(&cursorPos);

			// �}�E�X���x
			const float mouseSensitivity = 0.003f;

			float deltaX = (float)(cursorPos.x - prevCursorPos.x) * mouseSensitivity;
			float deltaY = (float)(cursorPos.y - prevCursorPos.y) * mouseSensitivity;

			prevCursorPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
			SetCursorPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

			m_rot.y += deltaX;
			m_rot.x += deltaY;

			// �㉺��]����
			if (m_rot.x < -1.3f)
			{
				m_rot.x = -1.3f;
			}
			if (m_rot.x > 1.3f)
			{
				m_rot.x = 1.3f;
			}

			//====================================
			// �}�E�X�z�C�[���ŃY�[���C���E�A�E�g
			//====================================
			int wheel = pInputMouse->GetWheel();
			const float zoomSpeed = 2.0f;

			if (wheel != 0)
			{
				m_fDistance -= wheel * zoomSpeed;

				// �J������������
				if (m_fDistance < 100.0f)
				{
					m_fDistance = 100.0f;
				}
				if (m_fDistance > 200.0f)
				{
					m_fDistance = 200.0f;
				}
			}
		}

		// �J�����ʒu�v�Z
		m_posV.x = playerPos.x + sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posV.y = playerPos.y + sinf(m_rot.x) * m_fDistance + 80.0f;
		m_posV.z = playerPos.z + cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;

		// �����_
		m_posR = playerPos;
		m_posR.y += 60.0f;

		// �J�����̈ʒu�␳����
		AdjustCameraPosition(playerPos);
	}
}
//=============================================================================
// �Q�[���p�b�h�̃J������]����
//=============================================================================
void CCamera::CameraWithGamepad(float stickX, float stickY)
{
	const float rotationSpeed = 0.07f; // ��]���x

	 // ���������̉�]�iY���j
	m_rot.y += stickX * rotationSpeed;

	// ���������̉�]�iX���j
	m_rot.x -= stickY * rotationSpeed;

	// ���������̉�]�𐧌� (-90�x �` 90�x)
	if (m_rot.x > D3DX_PI / 2)
	{
		m_rot.x = D3DX_PI / 2;
	}
	if (m_rot.x < -D3DX_PI / 2)
	{
		m_rot.x = -D3DX_PI / 2;
	}

	// ���������̉�]�𐳋K�� (-�� �` ��)
	if (m_rot.y > D3DX_PI)
	{
		m_rot.y -= D3DX_PI * 2.0f;
	}
	else if (m_rot.y < -D3DX_PI)
	{
		m_rot.y += D3DX_PI * 2.0f;
	}
}
//=============================================================================
// �J�����̈ʒu�␳(�Ǌђʂ��Ȃ���)����
//=============================================================================
void CCamera::AdjustCameraPosition(const D3DXVECTOR3& playerPos)
{
	// �v���C���[�̓��ʒu�𒍎��_�Ƃ���
	D3DXVECTOR3 playerEye = playerPos;
	playerEye.y += 60.0f; // �v���C���[�̓��̍���
	m_posR = playerEye;

	// �J�����̗��z�ʒu
	D3DXVECTOR3 offsetFromPlayer;
	offsetFromPlayer.x = sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	offsetFromPlayer.y = sinf(m_rot.x) * m_fDistance + 20.0f;
	offsetFromPlayer.z = cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;

	D3DXVECTOR3 idealCamPos = playerEye + offsetFromPlayer;

	// Bullet Physics �Ń��C�L���X�g
	btVector3 from(playerEye.x, playerEye.y, playerEye.z);
	btVector3 to(idealCamPos.x, idealCamPos.y, idealCamPos.z);

	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
	CManager::GetPhysicsWorld()->rayTest(from, to, rayCallback);

	if (rayCallback.hasHit())
	{
		// �ՓˑΏۂ̃��[�U�[�|�C���^����u���b�N�����
		const btCollisionObject* hitObj = rayCallback.m_collisionObject;
		void* userPtr = hitObj->getUserPointer();

		if (userPtr != NULL)
		{
			CBlock* hitBlock = static_cast<CBlock*>(userPtr);

			// TYPE_WATER �Ȃ�␳���X�L�b�v
			if (hitBlock->GetType() == CBlock::TYPE_WATER)
			{
				m_posV = idealCamPos;
			}
			else
			{
				// �Փ˓_�̏�����O�ɃJ������z�u
				btVector3 hitPoint = rayCallback.m_hitPointWorld;
				btVector3 camDir = (from - hitPoint).normalized();
				hitPoint += camDir * 10.0f;

				m_posV = D3DXVECTOR3(hitPoint.x(), hitPoint.y(), hitPoint.z());
			}
		}
		else
		{// ���[�U�[�|�C���^�� null �̏ꍇ

			// �␳���Ă���
			btVector3 hitPoint = rayCallback.m_hitPointWorld;
			btVector3 camDir = (from - hitPoint).normalized();
			hitPoint += camDir * 10.0f;

			m_posV = D3DXVECTOR3(hitPoint.x(), hitPoint.y(), hitPoint.z());
		}
	}
	else
	{// �Փ˂Ȃ�
		// ���z�ʒu���̂܂�
		m_posV = idealCamPos;
	}

	// �����_�̓v���C���[�̓�
	m_posR = playerEye;
}