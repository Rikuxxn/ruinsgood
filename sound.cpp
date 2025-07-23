//=============================================================================
//
// サウンド処理 [sound.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "sound.h"
using namespace std;

//=============================================================================
// コンストラクタ
//=============================================================================
CSound::CSound()
{
	// 値のクリア
	m_pXAudio2			= NULL;
	m_pMasteringVoice	= NULL;
	m_Listener			= {};					// リスナーの位置

	for (int nCnt = 0; nCnt < SOUND_LABEL_MAX; nCnt++)
	{
		m_apSourceVoice[nCnt] = {};		// ソースボイス
		m_apDataAudio[nCnt] = {};		// オーディオデータ
		m_aSizeAudio[nCnt] = {};		// オーディオデータサイズ
		m_Emitters[nCnt] = {};			// 各サウンドの音源(3D用)
	}
}
//=============================================================================
// デストラクタ
//=============================================================================
CSound::~CSound()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CSound::Init(HWND hWnd)
{
	HRESULT hr;

	// COMライブラリの初期化
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio2オブジェクトの作成
	hr = XAudio2Create(&m_pXAudio2, 0);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2オブジェクトの作成に失敗！", "警告！", MB_ICONWARNING);

		// COMライブラリの終了処理
		CoUninitialize();

		return E_FAIL;
	}

	// マスターボイスの生成
	hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice, 4, 48000, 0, NULL, NULL);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "マスターボイスの生成に失敗！", "警告！", MB_ICONWARNING);

		if (m_pXAudio2 != NULL)
		{
			// XAudio2オブジェクトの開放
			m_pXAudio2->Release();
			m_pXAudio2 = NULL;
		}

		// COMライブラリの終了処理
		CoUninitialize();

		return E_FAIL;
	}

	//// スピーカー設定を取得
	//XAUDIO2_VOICE_DETAILS details;
	//m_pMasteringVoice->GetVoiceDetails(&details);

	//UINT32 speakerConfig = details.InputChannels; // 現在のスピーカー設定
	//X3DAudioInitialize(speakerConfig, X3DAUDIO_SPEED_OF_SOUND, m_X3DInstance);

	//// リスナーの初期位置
	//m_Listener.Position = { 0.0f, 0.0f, 0.0f };   // 中央
	//m_Listener.OrientFront = { 0.0f, 0.0f, 0.0f }; // 前方向
	//m_Listener.OrientTop = { 0.0f, 1.0f, 0.0f };   // 上方向
	//m_Listener.Velocity = { 0.0f, 0.0f, 0.0f };

	// サウンドデータの初期化
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// バッファのクリア
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// サウンドデータファイルの生成
		hFile = CreateFile(m_aSoundInfo[nCntSound].pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(1)", "警告！", MB_ICONWARNING);
			return HRESULT_FROM_WIN32(GetLastError());
		}
		if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{// ファイルポインタを先頭に移動
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(2)", "警告！", MB_ICONWARNING);
			return HRESULT_FROM_WIN32(GetLastError());
		}

		// WAVEファイルのチェック
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(1)", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}
		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(2)", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}
		if (dwFiletype != 'EVAW')
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(3)", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}

		// フォーマットチェック
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(1)", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}
		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(2)", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}

		// オーディオデータ読み込み
		hr = CheckChunk(hFile, 'atad', &m_aSizeAudio[nCntSound], &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(1)", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}
		m_apDataAudio[nCntSound] = (BYTE*)malloc(m_aSizeAudio[nCntSound]);
		hr = ReadChunkData(hFile, m_apDataAudio[nCntSound], m_aSizeAudio[nCntSound], dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(2)", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}

		// ソースボイスの生成
		hr = m_pXAudio2->CreateSourceVoice(&m_apSourceVoice[nCntSound], &(wfx.Format));
		if (FAILED(hr))
		{
			MessageBox(hWnd, "ソースボイスの生成に失敗！", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}

		// バッファの値設定
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		buffer.AudioBytes = m_aSizeAudio[nCntSound];
		buffer.pAudioData = m_apDataAudio[nCntSound];
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount = m_aSoundInfo[nCntSound].nCntLoop;

		// オーディオバッファの登録
		m_apSourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);

		// ファイルをクローズ
		CloseHandle(hFile);
	}

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CSound::Uninit(void)
{
	// 一時停止
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (m_apSourceVoice[nCntSound] != NULL)
		{
			// 一時停止
			m_apSourceVoice[nCntSound]->Stop(0);

			// ソースボイスの破棄
			m_apSourceVoice[nCntSound]->DestroyVoice();
			m_apSourceVoice[nCntSound] = NULL;

			// オーディオデータの開放
			free(m_apDataAudio[nCntSound]);
			m_apDataAudio[nCntSound] = NULL;
		}
	}


	if (m_pXAudio2 != NULL)
	{
		// マスターボイスの破棄
		m_pMasteringVoice->DestroyVoice();
		m_pMasteringVoice = NULL;

		// XAudio2オブジェクトの開放
		m_pXAudio2->Release();
		m_pXAudio2 = NULL;
	}

	// COMライブラリの終了処理
	CoUninitialize();
}
//=============================================================================
// 2Dセグメント再生(再生中なら停止)
//=============================================================================
HRESULT CSound::Play(SOUND_LABEL label)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_aSizeAudio[label];
	buffer.pAudioData = m_apDataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = m_aSoundInfo[label].nCntLoop;

	// 状態取得
	m_apSourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		m_apSourceVoice[label]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	m_apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	m_apSourceVoice[label]->Start(0);

	return S_OK;
}
//=============================================================================
// 3Dセグメント再生(再生中なら停止)
//=============================================================================
HRESULT CSound::Play3D(SOUND_LABEL label, D3DXVECTOR3 soundPos, float minDistance, float maxDistance)
{
	if (!m_apSourceVoice[label])
	{
		return E_FAIL;
	}

	//// 音源(エミッター)に設定して位置を更新
	//UpdateSoundPosition(label, soundPos);

	m_Emitters[label].Velocity = { 0.0f, 0.0f, 0.0f };
	m_Emitters[label].ChannelCount = 1;
	m_Emitters[label].CurveDistanceScaler = 80.0f;  // 距離減衰を設定

	// 3Dオーディオ計算用のバッファ
	X3DAUDIO_DSP_SETTINGS dspSettings = {};
	FLOAT32 matrix[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	dspSettings.SrcChannelCount = 1;
	dspSettings.DstChannelCount = 4;
	dspSettings.pMatrixCoefficients = matrix;

	// カスタムパンニング計算
	CalculateCustomPanning(label, matrix, minDistance, maxDistance);

	// パンニング値をクリップ（0.0 ～ 0.5 の範囲に収める）
	matrix[0] = max(0.0f, min(0.5f, matrix[0]));
	matrix[1] = max(0.0f, min(0.5f, matrix[1]));

	// パンニング値をクリップ
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

	// 計算結果を適用
	m_apSourceVoice[label]->SetOutputMatrix(NULL, 1, 4, matrix);

	// バッファ設定
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = m_aSizeAudio[label];
	buffer.pAudioData = m_apDataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = 0;

	// 状態取得
	m_apSourceVoice[label]->GetState(&xa2state);

	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		m_apSourceVoice[label]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	m_apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	m_apSourceVoice[label]->Start(0);

	return S_OK;
}
//=============================================================================
// セグメント停止(ラベル指定)
//=============================================================================
void CSound::Stop(SOUND_LABEL label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// 状態取得
	m_apSourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		m_apSourceVoice[label]->FlushSourceBuffers();
	}
}
//=============================================================================
// セグメント停止(全て)
//=============================================================================
void CSound::Stop(void)
{
	// 一時停止
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (m_apSourceVoice[nCntSound] != NULL)
		{
			// 一時停止
			m_apSourceVoice[nCntSound]->Stop(0);
		}
	}
}
//=============================================================================
// カスタムパンニング
//=============================================================================
void CSound::CalculateCustomPanning(SOUND_LABEL label, FLOAT32* matrix, float minDis, float maxDis)
{
	// プレイヤーの向き（前方向ベクトル）
	D3DXVECTOR3 front = m_Listener.OrientFront;
	D3DXVec3Normalize(&front, &front);

	// 上方向ベクトル（通常は (0, 1, 0)）
	D3DXVECTOR3 up = m_Listener.OrientTop;
	D3DXVec3Normalize(&up, &up);

	// 右方向ベクトル = front × up（外積で求める）
	D3DXVECTOR3 right;
	D3DXVec3Cross(&right, &up, &front);
	D3DXVec3Normalize(&right, &right);

	// リスナー → 音源のベクトル
	D3DXVECTOR3 toEmitter =
	{
		m_Emitters[label].Position.x - m_Listener.Position.x,
		m_Emitters[label].Position.y - m_Listener.Position.y,
		m_Emitters[label].Position.z - m_Listener.Position.z
	};

	// 距離を計算
	float distance = D3DXVec3Length(&toEmitter);

	// 最小距離と最大距離（距離減衰の範囲）
	float minDistance = minDis;
	float maxDistance = maxDis;

	// 距離減衰計算
	float volumeScale = 1.0f - ((distance - minDistance) / (maxDistance - minDistance));
	volumeScale = max(0.0f, min(1.0f, volumeScale));

	// 方向ベクトルを正規化
	D3DXVec3Normalize(&toEmitter, &toEmitter);

	// 左右パンニング
	FLOAT32 panFactorLR = D3DXVec3Dot(&right, &toEmitter);
	FLOAT32 panCurveLR = sinf(panFactorLR * D3DX_PI * 0.5f); // -1.0 ～ 1.0 の範囲

	// 前後パンニング
	FLOAT32 panFactorFB = D3DXVec3Dot(&front, &toEmitter);
	FLOAT32 panCurveFB = sinf(panFactorFB * D3DX_PI * 0.5f); // -1.0 ～ 1.0 の範囲

	// チャンネルごとの音量計算
	FLOAT32 frontLeft = (0.8f - max(0.0f, panCurveLR)) * (1.0f + panCurveFB) * 0.5f * volumeScale;
	FLOAT32 frontRight = (0.8f - max(0.0f, -panCurveLR)) * (1.0f + panCurveFB) * 0.5f * volumeScale;
	FLOAT32 rearLeft = (0.8f - max(0.0f, panCurveLR)) * (1.0f - panCurveFB) * 0.5f * volumeScale;
	FLOAT32 rearRight = (0.8f - max(0.0f, -panCurveLR)) * (1.0f - panCurveFB) * 0.5f * volumeScale;

	// 音量マトリックスにセット（4ch: FL, FR, RL, RR）
	matrix[0] = frontLeft;
	matrix[1] = frontRight;
	matrix[2] = rearLeft;
	matrix[3] = rearRight;
}
////=============================================================================
//// 音源の位置更新処理
////=============================================================================
//void CSound::UpdateSoundPosition(SOUND_LABEL label, D3DXVECTOR3 pos)
//{
//	if (label < 0 || label >= SOUND_LABEL_MAX)
//	{
//		return;
//	}
//
//	// 音源の現在位置を適用
//	m_Emitters[label].Position = { pos.x, pos.y, pos.z };
//
//	// 音が再生されていないときのみ更新
//	XAUDIO2_VOICE_STATE xa2state;
//	m_apSourceVoice[label]->GetState(&xa2state);
//
//	if (xa2state.BuffersQueued == 0)
//	{
//		// 3Dオーディオ計算
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
//		// パンニング値をクリップ
//		for (int nCnt = 0; nCnt < 4; nCnt++)
//		{
//			matrix[nCnt] = max(0.0f, min(1.0f, matrix[nCnt]));
//		}
//
//		// 音のパンニングを更新（再生中は変更しない）
//		m_apSourceVoice[label]->SetOutputMatrix(NULL, 1, 4, matrix);
//	}
//}
//=============================================================================
// チャンクのチェック
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
	{// ファイルポインタを先頭に移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクデータの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// ファイルタイプの読み込み
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// ファイルポインタをチャンクデータ分移動
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
// チャンクデータの読み込み
//=============================================================================
HRESULT CSound::ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを指定位置まで移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// データの読み込み
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}