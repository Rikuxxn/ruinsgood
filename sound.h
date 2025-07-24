//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _SOUND_H_// ���̃}�N����`������Ă��Ȃ�������
#define _SOUND_H_// 2�d�C���N���[�h�h�~�̃}�N����`

// �T�E���h�N���X
class CSound
{
public:
	CSound();
	~CSound();

	//*************************************************************************
	// �T�E���h�ꗗ
	//*************************************************************************
	typedef enum
	{
		SOUND_LABEL_GAMEBGM = 0,	// �Q�[��BGM
		SOUND_LABEL_WATER,
		SOUND_LABEL_WATERRISE,
		SOUND_LABEL_HIT,
		SOUND_LABEL_FIRE,
		SOUND_LABEL_ROCKHIT,
		SOUND_LABEL_SWITCH,
		SOUND_LABEL_PAUSE,
		SOUND_LABEL_SELECT,
		SOUND_LABEL_ENTER,
		SOUND_LABEL_ROLL,
		SOUND_LABEL_MAX,
	} SOUND_LABEL;

	HRESULT Init(HWND hWnd);
	void Uninit(void);
	HRESULT Play(SOUND_LABEL label);
	HRESULT Play3D(SOUND_LABEL label,D3DXVECTOR3 soundPos,float minDistance,float maxDistance);
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);
	void Stop(SOUND_LABEL label);
	void Stop(void);
	void CalculateCustomPanning(SOUND_LABEL label, FLOAT32* matrix, float minDistance, float maxDistance);
	void UpdateListener(D3DXVECTOR3 pos);
	void UpdateSoundPosition(SOUND_LABEL label, D3DXVECTOR3 pos);

private:
	IXAudio2* m_pXAudio2;									// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
	IXAudio2MasteringVoice* m_pMasteringVoice;				// �}�X�^�[�{�C�X
	IXAudio2SourceVoice* m_apSourceVoice[SOUND_LABEL_MAX];	// �\�[�X�{�C�X
	BYTE* m_apDataAudio[SOUND_LABEL_MAX];					// �I�[�f�B�I�f�[�^
	DWORD m_aSizeAudio[SOUND_LABEL_MAX];					// �I�[�f�B�I�f�[�^�T�C�Y

	// 3D�I�[�f�B�I�p�̕ϐ�
	X3DAUDIO_HANDLE m_X3DInstance;       // X3DAudio �C���X�^���X
	X3DAUDIO_LISTENER m_Listener;   // ���X�i�[�i�v���C���[�̈ʒu�j
	X3DAUDIO_EMITTER m_Emitters[SOUND_LABEL_MAX]; // �e�T�E���h�̉���

	typedef struct
	{
		const char* pFilename;	// �t�@�C����
		int nCntLoop;			// ���[�v�J�E���g
	} SOUNDINFO;

	// �T�E���h�̏��
	SOUNDINFO m_aSoundInfo[SOUND_LABEL_MAX] =
	{
		{"data/BGM/titleBGM.wav", -1},			// �Q�[��BGM
		{"data/SE/water.wav", 0},				// ����SE
		{"data/SE/waterrise.wav", 0},			// ���ʏ㏸SE
		{"data/SE/hit.wav", 0},					// �v���C���[�q�b�gSE
		{"data/SE/fire.wav", -1},				// ��SE(���[�v)
		{"data/SE/rock_hit.wav", 0},			// ��Փ�SE
		{"data/SE/switch.wav", 0},				// �X�C�b�`SE
		{"data/SE/menu.wav", 0},				// �|�[�YSE
		{"data/SE/select.wav", 0},				// �I��SE
		{"data/SE/enter.wav", 0},				// ����SE
		{"data/SE/roll.wav", -1},				// ��]����SE(���[�v)
	};

};


#endif