//=============================================================================
//
// �T�E���h���� [sound.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "sound.h"
using namespace std;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CSound::CSound()
{
	// �l�̃N���A
	m_pXAudio2			= NULL;
	m_pMasteringVoice	= NULL;
	m_Listener			= {};					// ���X�i�[�̈ʒu

	for (int nCnt = 0; nCnt < SOUND_LABEL_MAX; nCnt++)
	{
		m_apSourceVoice[nCnt] = {};		// �\�[�X�{�C�X
		m_apDataAudio[nCnt] = {};		// �I�[�f�B�I�f�[�^
		m_aSizeAudio[nCnt] = {};		// �I�[�f�B�I�f�[�^�T�C�Y
		m_Emitters[nCnt] = {};			// �e�T�E���h�̉���(3D�p)
	}
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CSound::~CSound()
{
	// �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CSound::Init(HWND hWnd)
{
	HRESULT hr;

	// COM���C�u�����̏�����
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio2�I�u�W�F�N�g�̍쐬
	hr = XAudio2Create(&m_pXAudio2, 0);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2�I�u�W�F�N�g�̍쐬�Ɏ��s�I", "�x���I", MB_ICONWARNING);

		// COM���C�u�����̏I������
		CoUninitialize();

		return E_FAIL;
	}

	// �}�X�^�[�{�C�X�̐���
	hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice, 4, 48000, 0, NULL, NULL);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�}�X�^�[�{�C�X�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);

		if (m_pXAudio2 != NULL)
		{
			// XAudio2�I�u�W�F�N�g�̊J��
			m_pXAudio2->Release();
			m_pXAudio2 = NULL;
		}

		// COM���C�u�����̏I������
		CoUninitialize();

		return E_FAIL;
	}

	//// �X�s�[�J�[�ݒ���擾
	//XAUDIO2_VOICE_DETAILS details;
	//m_pMasteringVoice->GetVoiceDetails(&details);

	//UINT32 speakerConfig = details.InputChannels; // ���݂̃X�s�[�J�[�ݒ�
	//X3DAudioInitialize(speakerConfig, X3DAUDIO_SPEED_OF_SOUND, m_X3DInstance);

	//// ���X�i�[�̏����ʒu
	//m_Listener.Position = { 0.0f, 0.0f, 0.0f };   // ����
	//m_Listener.OrientFront = { 0.0f, 0.0f, 0.0f }; // �O����
	//m_Listener.OrientTop = { 0.0f, 1.0f, 0.0f };   // �����
	//m_Listener.Velocity = { 0.0f, 0.0f, 0.0f };

	// �T�E���h�f�[�^�̏�����
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// �o�b�t�@�̃N���A
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// �T�E���h�f�[�^�t�@�C���̐���
		hFile = CreateFile(m_aSoundInfo[nCntSound].pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, "�T�E���h�f�[�^�t�@�C���̐����Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
			return HRESULT_FROM_WIN32(GetLastError());
		}
		if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{// �t�@�C���|�C���^��擪�Ɉړ�
			MessageBox(hWnd, "�T�E���h�f�[�^�t�@�C���̐����Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
			return HRESULT_FROM_WIN32(GetLastError());
		}

		// WAVE�t�@�C���̃`�F�b�N
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
			return S_FALSE;
		}
		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
			return S_FALSE;
		}
		if (dwFiletype != 'EVAW')
		{
			MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(3)", "�x���I", MB_ICONWARNING);
			return S_FALSE;
		}

		// �t�H�[�}�b�g�`�F�b�N
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "�t�H�[�}�b�g�`�F�b�N�Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
			return S_FALSE;
		}
		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "�t�H�[�}�b�g�`�F�b�N�Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
			return S_FALSE;
		}

		// �I�[�f�B�I�f�[�^�ǂݍ���
		hr = CheckChunk(hFile, 'atad', &m_aSizeAudio[nCntSound], &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "�I�[�f�B�I�f�[�^�ǂݍ��݂Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
			return S_FALSE;
		}
		m_apDataAudio[nCntSound] = (BYTE*)malloc(m_aSizeAudio[nCntSound]);
		hr = ReadChunkData(hFile, m_apDataAudio[nCntSound], m_aSizeAudio[nCntSound], dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "�I�[�f�B�I�f�[�^�ǂݍ��݂Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
			return S_FALSE;
		}

		// �\�[�X�{�C�X�̐���
		hr = m_pXAudio2->CreateSourceVoice(&m_apSourceVoice[nCntSound], &(wfx.Format));
		if (FAILED(hr))
		{
			MessageBox(hWnd, "�\�[�X�{�C�X�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);
			return S_FALSE;
		}

		// �o�b�t�@�̒l�ݒ�
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		buffer.AudioBytes = m_aSizeAudio[nCntSound];
		buffer.pAudioData = m_apDataAudio[nCntSound];
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount = m_aSoundInfo[nCntSound].nCntLoop;

		// �I�[�f�B�I�o�b�t�@�̓o�^
		m_apSourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);

		// �t�@�C�����N���[�Y
		CloseHandle(hFile);
	}

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CSound::Uninit(void)
{
	// �ꎞ��~
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (m_apSourceVoice[nCntSound] != NULL)
		{
			// �ꎞ��~
			m_apSourceVoice[nCntSound]->Stop(0);

			// �\�[�X�{�C�X�̔j��
			m_apSourceVoice[nCntSound]->DestroyVoice();
			m_apSourceVoice[nCntSound] = NULL;

			// �I�[�f�B�I�f�[�^�̊J��
			free(m_apDataAudio[nCntSound]);
			m_apDataAudio[nCntSound] = NULL;
		}
	}


	if (m_pXAudio2 != NULL)
	{
		// �}�X�^�[�{�C�X�̔j��
		m_pMasteringVoice->DestroyVoice();
		m_pMasteringVoice = NULL;

		// XAudio2�I�u�W�F�N�g�̊J��
		m_pXAudio2->Release();
		m_pXAudio2 = NULL;
	}

	// COM���C�u�����̏I������
	CoUninitialize();
}
//=============================================================================
// 2D�Z�O�����g�Đ�(�Đ����Ȃ��~)
//=============================================================================
HRESULT CSound::Play(SOUND_LABEL label)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_aSizeAudio[label];
	buffer.pAudioData = m_apDataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = m_aSoundInfo[label].nCntLoop;

	// ��Ԏ擾
	m_apSourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		m_apSourceVoice[label]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		m_apSourceVoice[label]->FlushSourceBuffers();
	}

	// �I�[�f�B�I�o�b�t�@�̓o�^
	m_apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// �Đ�
	m_apSourceVoice[label]->Start(0);

	return S_OK;
}
//=============================================================================
// 3D�Z�O�����g�Đ�(�Đ����Ȃ��~)
//=============================================================================
HRESULT CSound::Play3D(SOUND_LABEL label, D3DXVECTOR3 soundPos, float minDistance, float maxDistance)
{
	if (!m_apSourceVoice[label])
	{
		return E_FAIL;
	}

	//// ����(�G�~�b�^�[)�ɐݒ肵�Ĉʒu���X�V
	//UpdateSoundPosition(label, soundPos);

	m_Emitters[label].Velocity = { 0.0f, 0.0f, 0.0f };
	m_Emitters[label].ChannelCount = 1;
	m_Emitters[label].CurveDistanceScaler = 80.0f;  // ����������ݒ�

	// 3D�I�[�f�B�I�v�Z�p�̃o�b�t�@
	X3DAUDIO_DSP_SETTINGS dspSettings = {};
	FLOAT32 matrix[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	dspSettings.SrcChannelCount = 1;
	dspSettings.DstChannelCount = 4;
	dspSettings.pMatrixCoefficients = matrix;

	// �J�X�^���p���j���O�v�Z
	CalculateCustomPanning(label, matrix, minDistance, maxDistance);

	// �p���j���O�l���N���b�v�i0.0 �` 0.5 �͈̔͂Ɏ��߂�j
	matrix[0] = max(0.0f, min(0.5f, matrix[0]));
	matrix[1] = max(0.0f, min(0.5f, matrix[1]));

	// �p���j���O�l���N���b�v
	for (int i = 0; i < 2; i++)
	{
		if (matrix[i] > 1.0f)
		{
			matrix[i] = 1.0f;
		}
		if (matrix[i] < 0.0f)
		{
			matrix[i] = 0.0f;
		}
	}

	// �v�Z���ʂ�K�p
	m_apSourceVoice[label]->SetOutputMatrix(NULL, 1, 4, matrix);

	// �o�b�t�@�ݒ�
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = m_aSizeAudio[label];
	buffer.pAudioData = m_apDataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = 0;

	// ��Ԏ擾
	m_apSourceVoice[label]->GetState(&xa2state);

	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		m_apSourceVoice[label]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		m_apSourceVoice[label]->FlushSourceBuffers();
	}

	// �I�[�f�B�I�o�b�t�@�̓o�^
	m_apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// �Đ�
	m_apSourceVoice[label]->Start(0);

	return S_OK;
}
//=============================================================================
// �Z�O�����g��~(���x���w��)
//=============================================================================
void CSound::Stop(SOUND_LABEL label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// ��Ԏ擾
	m_apSourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		m_apSourceVoice[label]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		m_apSourceVoice[label]->FlushSourceBuffers();
	}
}
//=============================================================================
// �Z�O�����g��~(�S��)
//=============================================================================
void CSound::Stop(void)
{
	// �ꎞ��~
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (m_apSourceVoice[nCntSound] != NULL)
		{
			// �ꎞ��~
			m_apSourceVoice[nCntSound]->Stop(0);
		}
	}
}
//=============================================================================
// �J�X�^���p���j���O
//=============================================================================
void CSound::CalculateCustomPanning(SOUND_LABEL label, FLOAT32* matrix, float minDis, float maxDis)
{
	// �v���C���[�̌����i�O�����x�N�g���j
	D3DXVECTOR3 front = m_Listener.OrientFront;
	D3DXVec3Normalize(&front, &front);

	// ������x�N�g���i�ʏ�� (0, 1, 0)�j
	D3DXVECTOR3 up = m_Listener.OrientTop;
	D3DXVec3Normalize(&up, &up);

	// �E�����x�N�g�� = front �~ up�i�O�ςŋ��߂�j
	D3DXVECTOR3 right;
	D3DXVec3Cross(&right, &up, &front);
	D3DXVec3Normalize(&right, &right);

	// ���X�i�[ �� �����̃x�N�g��
	D3DXVECTOR3 toEmitter =
	{
		m_Emitters[label].Position.x - m_Listener.Position.x,
		m_Emitters[label].Position.y - m_Listener.Position.y,
		m_Emitters[label].Position.z - m_Listener.Position.z
	};

	// �������v�Z
	float distance = D3DXVec3Length(&toEmitter);

	// �ŏ������ƍő勗���i���������͈̔́j
	float minDistance = minDis;
	float maxDistance = maxDis;

	// ���������v�Z
	float volumeScale = 1.0f - ((distance - minDistance) / (maxDistance - minDistance));
	volumeScale = max(0.0f, min(1.0f, volumeScale));

	// �����x�N�g���𐳋K��
	D3DXVec3Normalize(&toEmitter, &toEmitter);

	// ���E�p���j���O
	FLOAT32 panFactorLR = D3DXVec3Dot(&right, &toEmitter);
	FLOAT32 panCurveLR = sinf(panFactorLR * D3DX_PI * 0.5f); // -1.0 �` 1.0 �͈̔�

	// �O��p���j���O
	FLOAT32 panFactorFB = D3DXVec3Dot(&front, &toEmitter);
	FLOAT32 panCurveFB = sinf(panFactorFB * D3DX_PI * 0.5f); // -1.0 �` 1.0 �͈̔�

	// �`�����l�����Ƃ̉��ʌv�Z
	FLOAT32 frontLeft = (0.8f - max(0.0f, panCurveLR)) * (1.0f + panCurveFB) * 0.5f * volumeScale;
	FLOAT32 frontRight = (0.8f - max(0.0f, -panCurveLR)) * (1.0f + panCurveFB) * 0.5f * volumeScale;
	FLOAT32 rearLeft = (0.8f - max(0.0f, panCurveLR)) * (1.0f - panCurveFB) * 0.5f * volumeScale;
	FLOAT32 rearRight = (0.8f - max(0.0f, -panCurveLR)) * (1.0f - panCurveFB) * 0.5f * volumeScale;

	// ���ʃ}�g���b�N�X�ɃZ�b�g�i4ch: FL, FR, RL, RR�j
	matrix[0] = frontLeft;
	matrix[1] = frontRight;
	matrix[2] = rearLeft;
	matrix[3] = rearRight;
}
////=============================================================================
//// �����̈ʒu�X�V����
////=============================================================================
//void CSound::UpdateSoundPosition(SOUND_LABEL label, D3DXVECTOR3 pos)
//{
//	if (label < 0 || label >= SOUND_LABEL_MAX)
//	{
//		return;
//	}
//
//	// �����̌��݈ʒu��K�p
//	m_Emitters[label].Position = { pos.x, pos.y, pos.z };
//
//	// �����Đ�����Ă��Ȃ��Ƃ��̂ݍX�V
//	XAUDIO2_VOICE_STATE xa2state;
//	m_apSourceVoice[label]->GetState(&xa2state);
//
//	if (xa2state.BuffersQueued == 0)
//	{
//		// 3D�I�[�f�B�I�v�Z
//		X3DAUDIO_DSP_SETTINGS dspSettings = {};
//		FLOAT32 matrix[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
//		dspSettings.SrcChannelCount = 1;
//		dspSettings.DstChannelCount = 4;
//		dspSettings.pMatrixCoefficients = matrix;
//
//		X3DAudioCalculate(
//			m_X3DInstance,
//			&m_Listener,
//			&m_Emitters[label],
//			X3DAUDIO_CALCULATE_MATRIX,
//			&dspSettings
//		);
//
//		// �p���j���O�l���N���b�v
//		for (int nCnt = 0; nCnt < 4; nCnt++)
//		{
//			matrix[nCnt] = max(0.0f, min(1.0f, matrix[nCnt]));
//		}
//
//		// ���̃p���j���O���X�V�i�Đ����͕ύX���Ȃ��j
//		m_apSourceVoice[label]->SetOutputMatrix(NULL, 1, 4, matrix);
//	}
//}
//=============================================================================
// �`�����N�̃`�F�b�N
//=============================================================================
HRESULT CSound::CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;

	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^��擪�Ɉړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// �`�����N�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// �`�����N�f�[�^�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// �t�@�C���^�C�v�̓ǂݍ���
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// �t�@�C���|�C���^���`�����N�f�[�^���ړ�
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == format)
		{
			*pChunkSize = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if (dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}

//=============================================================================
// �`�����N�f�[�^�̓ǂݍ���
//=============================================================================
HRESULT CSound::ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^���w��ʒu�܂ňړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// �f�[�^�̓ǂݍ���
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}