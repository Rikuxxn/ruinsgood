//=============================================================================
//
// �J�������� [camera.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _CAMERA_H_// ���̃}�N����`������Ă��Ȃ�������
#define _CAMERA_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �J�����N���X
//*****************************************************************************
class CCamera
{
public:
	CCamera();
	~CCamera();

	// �J�����̎��
	typedef enum
	{
		MODE_EDIT = 0,	// �G�f�B�^�[�J����
		MODE_GAME,		// �Q�[���J����
		MODE_DIRECTION,	// ���o�J����
		MODE_MAX
	}MODE;

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void UpdateInfo(void);
	void SetCamera(void);
	void EditCamera(void);
	void GameCamera(void);
	void CameraWithGamepad(float stickX, float stickY);
	void AdjustCameraPosition(const D3DXVECTOR3& playerPos);
	void DirectionCamera(int nTimer);

	void SetPosV(D3DXVECTOR3 posV) { m_posV = posV; }
	void SetPosR(D3DXVECTOR3 posR) { m_posR = posR; }
	void SetRot(D3DXVECTOR3 rot) { m_rot = rot; }
	void SetDis(float fDistance) { m_fDistance = fDistance; }
	int  SetTimer(int nTime) { return m_nTimer = nTime * 60; }
	void SetCamMode(int nTimer, D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 rot);

	//*****************************************************************************
	// getter�֐�
	//*****************************************************************************
	D3DXVECTOR3 GetRot(void) { return m_rot; }			// �J�����̊p�x�̎擾
	MODE GetMode(void) { return m_Mode; }

private:
	D3DXVECTOR3 m_posV;			// ���_
	D3DXVECTOR3 m_posVDest;		// �ړI�̎��_
	D3DXVECTOR3 m_posR;			// �����_
	D3DXVECTOR3 m_posRDest;		// �ړI�̒����_
	D3DXVECTOR3 m_vecU;			// ������x�N�g��
	D3DXMATRIX m_mtxProjection;	// �v���W�F�N�V�����}�g���b�N�X
	D3DXMATRIX m_mtxView;		// �r���[�}�g���b�N�X
	D3DXVECTOR3 m_rot;			// ����
	float m_fDistance;			// ���_���璍���_�̋���
	MODE m_Mode;				// �J�����̃��[�h
	int m_nDirectionCamTimer;
	int m_nTimer;
};

#endif