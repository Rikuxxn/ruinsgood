//=============================================================================
//
// �}�l�[�W���[���� [manager.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _MANAGER_H_// ���̃}�N����`������Ă��Ȃ�������
#define _MANAGER_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"
#include "renderer.h"
#include "object2D.h"
#include "input.h"
#include "sound.h"
#include "player.h"
#include "effect.h"
#include "score.h"
#include "time.h"
#include "pause.h"
#include "texture.h"
#include "camera.h"
#include "light.h"
#include "motion.h"
#include "block.h"
#include "blockmanager.h"

//*****************************************************************************
// �}�l�[�W���[�N���X
//*****************************************************************************
class CManager
{
public:
	CManager();
	~CManager();

	HRESULT Init(HINSTANCE HInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CRenderer* GetRenderer(void) { return m_pRenderer; }
	static CObject2D* GetObject2D(void) { return m_pObject2D; }
	static CInputKeyboard* GetInputKeyboard(void) { return m_pInputKeyboard; }
	static CInputJoypad* GetInputJoypad(void) { return m_pInputJoypad; }
	static CInputMouse* GetInputMouse(void) { return m_pInputMouse; }
	static CSound* GetSound(void) { return m_pSound; }
	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CEffect* GetEffect(void) { return m_pEffect; }
	static CScore* GetScore(void) { return m_pScore; }
	static CTime* GetTime(void) { return m_pTime; }
	static CColon* GetColon(void) { return m_pColon; }
	static CPause* GetPause(void) { return m_pPause; }
	static CTexture* GetTexture(void) { return m_pTexture; }
	static CCamera* GetCamera(void) { return m_pCamera; }
	static CLight* GetLight(void) { return m_pLight; }
	static CModel* GetModel(void) { return m_pModel; }
	static CMotion* GetMotion(void) { return m_pMotion; }
	static CObjectX* GetObjectX(void) { return m_pObjectX; }
	static CBlock* GetBlock(void) { return m_pBlock; }
	static CBlockManager* GetBlockManager(void) { return m_pBlockManager; }
	static CImGuiManager* GetImGuiManager(void) { return m_pImGuiManager; }

	int GetFPS(int fps) { return m_fps = fps; };
	int GetFPSCnt(void) { return m_fps; }
	static bool GetisPaused(void);
	// ���N���X���畨�����[���h��G�肽�����p
	static btDiscreteDynamicsWorld* GetPhysicsWorld(void) { return m_pDynamicsWorld; }
	static void CheckCollisions(void);

private:
	static CRenderer* m_pRenderer;				// �����_���[�ւ̃|�C���^
	static CObject2D* m_pObject2D;				// 2D�I�u�W�F�N�g�ւ̃|�C���^
	static CInputKeyboard* m_pInputKeyboard;	// �L�[�{�[�h�ւ̃|�C���^
	static CInputJoypad* m_pInputJoypad;		// �W���C�p�b�h�ւ̃|�C���^
	static CInputMouse* m_pInputMouse;			// �}�E�X�ւ̃|�C���^
	static CSound* m_pSound;					// �}�E�X�ւ̃|�C���^
	static CPlayer* m_pPlayer;					// �v���C���[�ւ̃|�C���^
	static CEffect* m_pEffect;					// �G�t�F�N�g�ւ̃|�C���^
	static CScore* m_pScore;					// �X�R�A�ւ̃|�C���^
	static CTime* m_pTime;						// �^�C���ւ̃|�C���^
	static CColon* m_pColon;					// �R�����ւ̃|�C���^
	static CPause* m_pPause;					// �|�[�Y�ւ̃|�C���^
	static CTexture* m_pTexture;				// �e�N�X�`���ւ̃|�C���^
	static CCamera* m_pCamera;					// �J�����ւ̃|�C���^
	static CLight* m_pLight;					// ���C�g�ւ̃|�C���^
	static CModel* m_pModel;					// ���f���ւ̃|�C���^
	static CMotion* m_pMotion;					// ���[�V�����ւ̃|�C���^
	static CBlock* m_pBlock;					// �u���b�N�ւ̃|�C���^
	static CObjectX* m_pObjectX;				// X�t�@�C���I�u�W�F�N�g�ւ̃|�C���^
	static CBlockManager* m_pBlockManager;		// �u���b�N�}�l�[�W���[�ւ̃|�C���^
	static CImGuiManager* m_pImGuiManager;		// ImGui�}�l�[�W���[�ւ̃|�C���^

	static std::vector<CBlock*> m_blocks;  // �����̃u���b�N��ێ�
	int m_fps;
	static bool m_isPaused; // true�Ȃ�|�[�Y��

	btBroadphaseInterface* m_pBroadphase;						// �����܂��Փ˔���̃N���X�ւ̃|�C���^
	btDefaultCollisionConfiguration* m_pCollisionConfiguration ;// �Փˌ��o�̐ݒ���Ǘ�����N���X�ւ̃|�C���^
	btCollisionDispatcher* m_pDispatcher;						// ���ۂɏՓ˔��菈�������s����N���X�ւ̃|�C���^
	btSequentialImpulseConstraintSolver* m_pSolver;				// �����V�~�����[�V�����̐���\���o�[�ւ̃|�C���^
	static btDiscreteDynamicsWorld* m_pDynamicsWorld;			// �������E���Ǘ�����N���X�ւ̃|�C���^
};

#endif