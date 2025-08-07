//=============================================================================
//
// �X�e�[�W���� [stage.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _STAGE_H_// ���̃}�N����`������Ă��Ȃ�������
#define _STAGE_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"


//*****************************************************************************
// �X�e�[�W�N���X
//*****************************************************************************
class CStage : public CObject
{
public:
	CStage(int nPriority = 7);
	~CStage();

	// �I�����ڂ̎��
	typedef enum
	{
		STAGE_ID_1 = 0,		
		STAGE_ID_2,
		STAGE_ID_3,
		STAGE_ID_NONE,
		STAGE_MAX
	}STAGE;

	static CStage* Create(STAGE type, D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	D3DXVECTOR3 GetPos(void);
	bool IsMouseOver(void);
	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }
	void SetCol(D3DXCOLOR col) { m_col = col; }
	virtual void Execute(void) {};

	// �I����Ԑݒ�E�擾
	void SetSelected(bool selected) { m_isSelected = selected; }
	bool IsSelected(void) const { return m_isSelected; }
	void SetTargetPos(const D3DXVECTOR3& targetPos) { m_targetPos = targetPos; }
	void StartSlideIn(bool flag) { m_isSlidingIn = flag; }
	void StartSlideOut(bool flag) { m_isSlidingOut = flag; }
	bool IsSlideIn(void) { return m_isSlidingIn; }
	bool IsSlideOut(void) { return m_isSlidingOut; }
	bool IsSlideInFinished(void) { return m_isSlideInFinished; }
	bool IsSlideOutFinished(void) { return m_isSlideOutFinished; }
	void SlideIn(void);
	void SlideOut(void);
	STAGE GetStageId(void) const;

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;						// �ʒu
	D3DCOLOR m_col;							// �F
	float m_fWidth, m_fHeight;				// �T�C�Y
	int m_nIdxTexture;						// �e�N�X�`���C���f�b�N�X
	char m_szPath[MAX_PATH];				// �t�@�C���p�X
	bool m_isSelected;						// �I��������
	D3DXVECTOR3 m_startPos;					// �X���C�h�C���J�n�ʒu�i��ʉE�O�j
	D3DXVECTOR3 m_targetPos;				// �X���C�h�C�������ʒu�i�ړI�ʒu�j
	bool m_isSlidingIn ;					// �X���C�h���t���O
	bool m_isSlidingOut;					// �X���C�h�A�E�g�t���O
	bool m_isSlideInFinished;
	bool m_isSlideOutFinished;
};

//*****************************************************************************
// �X�e�[�W1�N���X
//*****************************************************************************
class CStage1 : public CStage
{
public:
	CStage1();
	~CStage1();

	void Execute(void) override;

private:

};

//*****************************************************************************
// �X�e�[�W2�N���X
//*****************************************************************************
class CStage2 : public CStage
{
public:
	CStage2();
	~CStage2();

	void Execute(void) override;

private:

};

//*****************************************************************************
// �X�e�[�W3�N���X
//*****************************************************************************
class CStage3 : public CStage
{
public:
	CStage3();
	~CStage3();

	void Execute(void) override;

private:

};

//*****************************************************************************
// �߂鍀�ڃN���X
//*****************************************************************************
class CBack : public CStage
{
public:
	CBack();
	~CBack();

	void Execute(void) override;

private:

};

#endif