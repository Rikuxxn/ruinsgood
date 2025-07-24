//=============================================================================
//
// サウンド処理 [sound.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _SOUND_H_// このマクロ定義がされていなかったら
#define _SOUND_H_// 2重インクルード防止のマクロ定義

// サウンドクラス
class CSound
{
public:
	CSound();
	~CSound();

	//*************************************************************************
	// サウンド一覧
	//*************************************************************************
	typedef enum
	{
		SOUND_LABEL_GAMEBGM = 0,	// ゲームBGM
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
	IXAudio2* m_pXAudio2;									// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice* m_pMasteringVoice;				// マスターボイス
	IXAudio2SourceVoice* m_apSourceVoice[SOUND_LABEL_MAX];	// ソースボイス
	BYTE* m_apDataAudio[SOUND_LABEL_MAX];					// オーディオデータ
	DWORD m_aSizeAudio[SOUND_LABEL_MAX];					// オーディオデータサイズ

	// 3Dオーディオ用の変数
	X3DAUDIO_HANDLE m_X3DInstance;       // X3DAudio インスタンス
	X3DAUDIO_LISTENER m_Listener;   // リスナー（プレイヤーの位置）
	X3DAUDIO_EMITTER m_Emitters[SOUND_LABEL_MAX]; // 各サウンドの音源

	typedef struct
	{
		const char* pFilename;	// ファイル名
		int nCntLoop;			// ループカウント
	} SOUNDINFO;

	// サウンドの情報
	SOUNDINFO m_aSoundInfo[SOUND_LABEL_MAX] =
	{
		{"data/BGM/titleBGM.wav", -1},			// ゲームBGM
		{"data/SE/water.wav", 0},				// 入水SE
		{"data/SE/waterrise.wav", 0},			// 水位上昇SE
		{"data/SE/hit.wav", 0},					// プレイヤーヒットSE
		{"data/SE/fire.wav", -1},				// 炎SE(ループ)
		{"data/SE/rock_hit.wav", 0},			// 岩衝突SE
		{"data/SE/switch.wav", 0},				// スイッチSE
		{"data/SE/menu.wav", 0},				// ポーズSE
		{"data/SE/select.wav", 0},				// 選択SE
		{"data/SE/enter.wav", 0},				// 決定SE
		{"data/SE/roll.wav", -1},				// 岩転がるSE(ループ)
	};

};


#endif