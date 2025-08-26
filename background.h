//=============================================================================
//
// �w�i���� [background.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BACKGROUND_H_// ���̃}�N����`������Ă��Ȃ�������
#define _BACKGROUND_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object2D.h"

//*****************************************************************************
// �w�i�N���X
//*****************************************************************************
class CBackground : public CObject2D
{
public:
	CBackground();
	~CBackground();

	static CBackground* Create(D3DXVECTOR3 pos, float fWidth, float fHeight, const char* path);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }

private:
	int m_nIdxTexture;
	char m_szPath[MAX_PATH];			// �t�@�C���p�X

};

#endif
