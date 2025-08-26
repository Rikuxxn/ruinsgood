//=============================================================================
//
// UI���� [ui.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _UI_H_// ���̃}�N����`������Ă��Ȃ�������
#define _UI_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object2D.h"
#include <unordered_map>
#include <functional>

class CUi;

using UiCreateFunc = std::function<CUi* ()>;

//*****************************************************************************
// UI�N���X
//*****************************************************************************
class CUi : public CObject2D
{
public:
	CUi(int nPriority = 6);
	~CUi();

	// UI�̎��
	typedef enum
	{
		TYPE_PAUSE = 0,
		TYPE_GET,
		TYPE_RESULT_UI,
		TYPE_RESULT_RANK,
		TYPE_STAGE_NAME,
		TYPE_MAX
	}TYPE;

	static CUi* Create(TYPE type, const char* path,D3DXVECTOR3 pos, float fWidth, float fHeight);
	static void InitFactory(void);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }
	void SetUV(float u, float v, float w, float h);  // UV�ݒ�

private:
	int m_nIdxTexture;
	char m_szPath[MAX_PATH];			// �t�@�C���p�X
	TYPE m_type;
	float m_uvLeft;
	float m_uvTop;
	float m_uvWidth;
	float m_uvHeight;
	bool m_isUVDirty;
	static std::unordered_map<TYPE, UiCreateFunc> m_UiFactoryMap;
};

//*****************************************************************************
// �|�[�YUI�N���X
//*****************************************************************************
class CPauseUi : public CUi
{
public:
	CPauseUi();
	~CPauseUi();

	HRESULT Init(void);
	void Update(void);

};

//*****************************************************************************
// ���擾UI�N���X
//*****************************************************************************
class CGetUi : public CUi
{
public:
	CGetUi();
	~CGetUi();

	HRESULT Init(void);
	void Update(void);

private:
	float m_fTimer;// �o�ߎ���(�b)
	float m_fAlpha;// �A���t�@�l
	bool m_bFading;// �t�F�[�h�J�n�t���O
};

//*****************************************************************************
// ���U���gUI�N���X
//*****************************************************************************
class CResultUi : public CUi
{
public:
	CResultUi();
	~CResultUi();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// ���U���gUI(�����N)�N���X
//*****************************************************************************
class CResultRankUi : public CUi
{
public:
	CResultRankUi();
	~CResultRankUi();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// �X�e�[�W���\���N���X
//*****************************************************************************
class CStageUi : public CUi
{
public:
	CStageUi();
	~CStageUi();

	// ���
	typedef enum
	{
		STATE_FADEIN = 0,
		STATE_SHOW,
		STATE_FADEOUT,
		STATE_END,
		STATE_MAX
	}STATE;

	HRESULT Init(void);
	void Update(void);

private:
	float m_fTimer;	// �o�ߎ���(�b)
	float m_fAlpha;	// �A���t�@�l
	STATE m_state;	// ���
};

#endif
