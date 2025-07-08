//=============================================================================
//
// �X�R�A���� [score.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _SCORE_H_// ���̃}�N����`������Ă��Ȃ�������
#define _SCORE_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "number.h"
#include "object.h"
#include "number.h"


//*****************************************************************************
// �X�R�A�N���X
//*****************************************************************************
class CScore : public CObject
{
public:
	CScore(int nPriority = 7);
	~CScore();

	static CScore* Create(float baseX, float baseY, float digitWidth, float digitHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);
	void SetScore(int nScore);
	static void AddScore(int nValue);
	int DigitNum(int nScore);

private:
	static const int MAX_DIGITS = 8;		// ����
	CNumber* m_apNumber[MAX_DIGITS];		// �e���̐����\���p
	static int m_nScore;					// �X�R�A
	float m_digitWidth;						// ����1��������̕�
	float m_digitHeight;					// ����1��������̍���
	D3DXVECTOR3 m_basePos;					// �\���̊J�n�ʒu
	int m_nIdxTexture;

};

#endif
