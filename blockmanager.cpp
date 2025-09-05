//=============================================================================
//
// �u���b�N�}�l�[�W���[���� [blockmanager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "blockmanager.h"
#include "json.hpp"
#include "FileDialogUtils.h"
#include "manager.h"
#include "imgui_internal.h"
#include "raycast.h"
#include "game.h"
#include "blocklist.h"

using json = nlohmann::json;

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
std::vector<CBlock*> CBlockManager::m_blocks = {};	// �u���b�N�̏��
int CBlockManager::m_nNumAll = 0;				// �u���b�N�̑���
int CBlockManager::m_selectedIdx = 0;			// �I�𒆂̃C���f�b�N�X
CBlock* CBlockManager::m_draggingBlock = {};
std::unordered_map<CBlock::TYPE, std::string> CBlockManager::s_FilePathMap; 
std::unordered_map<CBlock::TYPE, std::string> CBlockManager::s_texFilePathMap;
CBlock* CBlockManager::m_selectedBlock = {};

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CBlockManager::CBlockManager()
{
	// �l�̃N���A
	m_selectedBlock = NULL;
	m_prevSelectedIdx = -1;
	m_hasConsumedPayload = false;
	m_pDebug3D = NULL;							// 3D�f�o�b�O�\���ւ̃|�C���^
	m_autoUpdateColliderSize = true;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CBlockManager::~CBlockManager()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CBlock* CBlockManager::CreateBlock(CBlock::TYPE type, D3DXVECTOR3 pos)
{
	const char* path = CBlockManager::GetFilePathFromType(type);

	CBlock* newBlock = CBlock::Create(path, pos, D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1), type);

	if (newBlock)
	{
		m_blocks.push_back(newBlock);
		m_nNumAll++;			// �����̃J�E���g�A�b�v
	}

	return newBlock;
}
//=============================================================================
// ����������
//=============================================================================
void CBlockManager::Init(void)
{
	LoadConfig("data/block_payload.json");

	// ���I�z�����ɂ��� (�T�C�Y��0�ɂ���)
	m_blocks.clear();
}
//=============================================================================
// �I������
//=============================================================================
void CBlockManager::Uninit(void)
{
	m_nNumAll = 0;

	// ���I�z�����ɂ��� (�T�C�Y��0�ɂ���)
	m_blocks.clear();
}
//=============================================================================
// �X�V����
//=============================================================================
void CBlockManager::Update(void)
{
#ifdef _DEBUG

	// ���̍X�V
	UpdateInfo();

	// �h���b�O�����̍X�V
	UpdateDraggingBlock();

#endif
}
//=============================================================================
// �`�揈��
//=============================================================================
void CBlockManager::Draw(void)
{
#ifdef _DEBUG
	//// �I�𒆂̃u���b�N�����R���C�_�[�`��
	//if (CBlock* pSelectBlock = GetSelectedBlock())
	//{
	//	pSelectBlock->DrawCollider();
	//}
#endif
}
//=============================================================================
// ���̍X�V����
//=============================================================================
void CBlockManager::UpdateInfo(void)
{
	// ImGui�}�l�[�W���[�̎擾
	CImGuiManager* pImGuiManager = CGame::GetImGuiManager();

	// GUI�X�^�C���̎擾
	ImGuiStyle& style = ImGui::GetStyle();

	style.GrabRounding		= 10.0f;		// �X���C�_�[�̂܂݂��ۂ�
	style.ScrollbarRounding = 10.0f;		// �X�N���[���o�[�̊p
	style.ChildRounding		= 10.0f;		// �q�E�B���h�E�̊p
	style.WindowRounding	= 10.0f;		// �E�B���h�E�S�̂̊p

	// �ꏊ
	pImGuiManager->SetPosImgui(ImVec2(1900.0f, 20.0f));

	// �T�C�Y
	pImGuiManager->SetSizeImgui(ImVec2(420.0f, 500.0f));

	// �ŏ���GUI
	pImGuiManager->StartImgui(u8"BlockInfo", CImGuiManager::IMGUITYPE_DEFOULT);

	// �u���b�N���Ȃ��ꍇ
	if (m_blocks.empty())
	{
		ImGui::Text("No blocks placed yet.");
	}
	else
	{
		// �u���b�N�̑���
		ImGui::Text("Block Num %d", m_nNumAll);

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// �C���f�b�N�X�I��
		ImGui::SliderInt("Block Index", &m_selectedIdx, 0, (int)m_blocks.size() - 1);

		// �͈͊O�΍�
		if (m_selectedIdx >= (int)m_blocks.size())
		{
			m_selectedIdx = (int)m_blocks.size() - 1;
		}

		// �O��I��ł��u���b�N������
		if (m_prevSelectedIdx != -1 && m_prevSelectedIdx != m_selectedIdx)
		{
			m_blocks[m_prevSelectedIdx]->SetSelected(false);
		}

		// �Ώۃu���b�N�̎擾
		m_selectedBlock = m_blocks[m_selectedIdx];

		// �u���b�N���̒�������
		UpdateTransform(m_selectedBlock);

		if (GetUpdateCollider() == false)
		{
			// �R���C�_�[�̒�������
			UpdateCollider(m_selectedBlock);
		}
		else
		{
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�
			ImGui::Text("Auto Update Collider Size is Active");
		}
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

	// �u���b�N�^�C�v�ꗗ
	if (ImGui::TreeNode("Block Types"))
	{
		for (int typeInt = 0; typeInt < (int)CBlock::TYPE_MAX; typeInt++)
		{
			CBlock::TYPE type = static_cast<CBlock::TYPE>(typeInt);

			// �e�N�X�`���擾
			const char* texPath = CBlockManager::GetTexPathFromType(type);
			int texIdx = CManager::GetTexture()->Register(texPath);

			LPDIRECT3DTEXTURE9 tex = CManager::GetTexture()->GetAddress(texIdx);

			if (tex)
			{
				ImGui::PushID(typeInt);
				ImGui::Image((ImTextureID)tex, ImVec2(74, 74));

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					ImGui::SetDragDropPayload("BLOCK_TYPE", &type, sizeof(type));
					ImGui::Text("Block Type %d", typeInt);
					ImGui::Image((ImTextureID)tex, ImVec2(64, 64));
					ImGui::EndDragDropSource();
				}

				ImGui::PopID();

				// 4���Ƃɉ��s
				if (typeInt % 4 != 3)
				{
					ImGui::SameLine();
				}
			}
		}

		ImGui::NewLine(); // ���̍s��
		ImGui::TreePop(); // �c���[����
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

	if (ImGui::Button("Save"))
	{
		// �_�C�A���O���J���ăt�@�C���ɕۑ�
		std::string path = OpenWindowsSaveFileDialog();

		if (!path.empty())
		{
			// �f�[�^�̕ۑ�
			CBlockManager::SaveToJson(path.c_str());
		}
	}

	ImGui::SameLine(0);

	if (ImGui::Button("Load"))
	{
		// �_�C�A���O���J���ăt�@�C�����J��
		std::string path = OpenWindowsOpenFileDialog();

		if (!path.empty())
		{
			// �f�[�^�̓ǂݍ���
			CBlockManager::LoadFromJson(path.c_str());
		}
	}

	ImGui::End();

	if (CManager::GetMode() == MODE_TITLE)
	{
		return;
	}

	// �}�E�X�I������
	PickBlockFromMouseClick();
}
//=============================================================================
// �u���b�N���̒�������
//=============================================================================
void CBlockManager::UpdateTransform(CBlock* selectedBlock)
{
	if (selectedBlock)
	{
		// �I�𒆂̃u���b�N�̐F��ς���
		selectedBlock->SetSelected(true);

		D3DXVECTOR3 pos = selectedBlock->GetPos();	// �I�𒆂̃u���b�N�̈ʒu�̎擾
		D3DXVECTOR3 rot = selectedBlock->GetRot();	// �I�𒆂̃u���b�N�̌����̎擾
		D3DXVECTOR3 size = selectedBlock->GetSize();// �I�𒆂̃u���b�N�̃T�C�Y�̎擾
		btScalar mass = selectedBlock->GetMass();	// �I�𒆂̃u���b�N�̎��ʂ̎擾

		// ���W�A�����p�x�Ɉꎞ�ϊ��i�ÓI�ϐ��ŕێ������쒆�̂ݍX�V�j
		static D3DXVECTOR3 degRot = D3DXToDegree(rot);
		static bool m_initializedDegRot = false;


		if (!m_initializedDegRot)
		{
			degRot = D3DXToDegree(rot);
			m_initializedDegRot = true;
		}

		bool isEditMode = selectedBlock->IsEditMode();

		ImGui::Checkbox("Edit Block", &isEditMode);

		if (isEditMode)
		{
			selectedBlock->SetEditMode(true);  // �`�F�b�N��Kinematic��
		}
		else
		{
			selectedBlock->SetEditMode(false); // �ʏ�ɖ߂�
		}

		//*********************************************************************
		// POS �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���x��
		ImGui::Text("POS"); ImGui::SameLine(60); // ���x���̈ʒu������ƒ���

		// X
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_pos_x", &pos.x, 1.0f, -9000.0f, 9000.0f, "%.1f");

		// Y
		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_pos_y", &pos.y, 1.0f, -9000.0f, 9000.0f, "%.1f");

		// Z
		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_pos_z", &pos.z, 1.0f, -9000.0f, 9000.0f, "%.1f");

		//*********************************************************************
		// ROT �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::Text("ROT"); ImGui::SameLine(60);

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedX = ImGui::DragFloat("##Block_rot_x", &degRot.x, 0.1f, -180.0f, 180.0f, "%.2f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedY = ImGui::DragFloat("##Block_rot_y", &degRot.y, 0.1f, -180.0f, 180.0f, "%.2f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedZ = ImGui::DragFloat("##Block_rot_z", &degRot.z, 0.1f, -180.0f, 180.0f, "%.2f");

		bool isRotChanged = changedX || changedY || changedZ;
		
		//*********************************************************************
		// SIZE �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���x��
		ImGui::Text("SIZE"); ImGui::SameLine(60); // ���x���̈ʒu������ƒ���

		// X
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_size_x", &size.x, 0.1f, -100.0f, 100.0f, "%.1f");

		// Y
		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_size_y", &size.y, 0.1f, -100.0f, 100.0f, "%.1f");

		// Z
		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_size_z", &size.z, 0.1f, -100.0f, 100.0f, "%.1f");

		// �`�F�b�N�{�b�N�X�F�g�嗦�ɉ����Ď����X�V���邩
		ImGui::Checkbox("Auto Update Collider Size", &m_autoUpdateColliderSize);

		// �O��̃T�C�Y��ێ�
		static D3DXVECTOR3 prevSize = selectedBlock->GetSize();

		// �T�C�Y�ύX�`�F�b�N
		bool isSizeChanged = (size != prevSize);

		//*********************************************************************
		// ���� �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���x��
		ImGui::Text("MASS"); ImGui::SameLine(60); // ���x���̈ʒu������ƒ���

		// �X���C�_�[�i�͈�: 0.0f �` 80.0f�j
		ImGui::SliderFloat("##MassSlider", &mass, 0.0f, 80.0f, "%.2f");


		// �p�x�����W�A���ɖ߂�
		D3DXVECTOR3 rotRad = D3DXToRadian(degRot);

		// �ʒu�̐ݒ�
		selectedBlock->SetPos(pos);

		// �T�C�Y�̐ݒ�
		selectedBlock->SetSize(size);

		//// ���ʂ̐ݒ�
		//selectedBlock->SetMass(mass);

		// �T�C�Y(�g�嗦)���ς�����Ƃ������Ă�
		if (m_autoUpdateColliderSize == true && isSizeChanged)
		{
			// �u���b�N�T�C�Y�ɂ��R���C�_�[�̐���
			selectedBlock->CreatePhysicsFromScale(size);
			prevSize = size; // �X�V
		}

		if (isRotChanged)
		{
			// ��]���ς�����������Z�b�g
			selectedBlock->SetRot(rotRad);

			// �ҏW���[�h�Ȃ瑦����Transform���X�V���ē���
			if (isEditMode)
			{
				btTransform transform;
				transform.setIdentity();

				btVector3 btPos(pos.x + selectedBlock->GetColliderOffset().x,
					pos.y + selectedBlock->GetColliderOffset().y,
					pos.z + selectedBlock->GetColliderOffset().z);
				transform.setOrigin(btPos);

				D3DXMATRIX matRot;
				D3DXMatrixRotationYawPitchRoll(&matRot, rotRad.y, rotRad.x, rotRad.z);

				D3DXQUATERNION dq;
				D3DXQuaternionRotationMatrix(&dq, &matRot);

				btQuaternion btRot(dq.x, dq.y, dq.z, dq.w);
				transform.setRotation(btRot);

				if (!selectedBlock->GetRigidBody())
				{
					return;
				}
				selectedBlock->GetRigidBody()->setWorldTransform(transform);
				selectedBlock->GetRigidBody()->getMotionState()->setWorldTransform(transform);
			}
		}
		else
		{
			// �ҏW���Ă��Ȃ�����degRot��selectedBlock�̒l�ɓ������Ă���
			degRot = D3DXToDegree(selectedBlock->GetRot());
		}

		//*********************************************************************
		// �u���b�N�̍폜
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));

		if (ImGui::Button("Delete"))
		{
			if (m_autoUpdateColliderSize)
			{
				m_nNumAll--;		// �����̃J�E���g�_�E��

				if (m_blocks[m_selectedIdx])
				{
					// �I�𒆂̃u���b�N���폜
					m_blocks[m_selectedIdx]->Uninit();
				}

				m_blocks.erase(m_blocks.begin() + m_selectedIdx);

				// �I���C���f�b�N�X�𒲐�
				if (m_selectedIdx >= (int)m_blocks.size())
				{
					m_selectedIdx = (int)m_blocks.size() - 1;
				}

				m_prevSelectedIdx = -1;
			}
			else
			{// m_autoUpdateColliderSize��false�̎��͉������Ȃ�
				
			}
		}

		ImGui::PopStyleColor(3);
	}

	// �Ō�ɕۑ�
	m_prevSelectedIdx = m_selectedIdx;
}
//=============================================================================
// �u���b�N�̃h���b�O����
//=============================================================================
void CBlockManager::UpdateDraggingBlock(void)
{
	// �}�E�X�̎擾
	CInputMouse* pMouse = CManager::GetInputMouse();

	// ���݂�ImGui�̓�����ԁi�R���e�L�X�g�j�ւ̃|�C���^�[���擾
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	if (!m_draggingBlock && !m_hasConsumedPayload)
	{
		// �h���b�O���h���b�v���Ȃ瑦�������ĒǏ]�J�n
		if (ctx->DragDropActive && ctx->DragDropPayload.Data && ctx->DragDropPayload.DataSize == sizeof(CBlock::TYPE))
		{
			if (ctx->DragDropPayload.IsDataType("BLOCK_TYPE"))
			{
				CBlock::TYPE draggedType = *(CBlock::TYPE*)ctx->DragDropPayload.Data;

				D3DXVECTOR3 pos = pMouse->GetGroundHitPosition();
				pos.y = 30.0f;

				// �u���b�N�̐���
				m_draggingBlock = CreateBlock(draggedType, pos);

				// �������̓L�l�}�e�B�b�N���iEditMode ON�j
				m_draggingBlock->SetEditMode(true);

				m_hasConsumedPayload = true;
			}
		}
	}
	else
	{
		// �h���b�O���̓}�E�X�ɒǏ]������
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			D3DXVECTOR3 pos = pMouse->GetGroundHitPosition();
			pos.y = 30.0f;
			m_draggingBlock->SetPos(pos);
		}
		else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			// ��������ʏ함���ɖ߂��i�L�l�}�e�B�b�N�����j
			m_draggingBlock->SetEditMode(false);

			// �h���b�O�I��
			m_draggingBlock = NULL;
		}

		// ImGui�̃h���b�O�h���b�v�����S�ɏI�������t���O���Z�b�g
		if (!ctx->DragDropActive)
		{
			m_hasConsumedPayload = false;
		}
	}
}
//=============================================================================
// �u���b�N�I������
//=============================================================================
void CBlockManager::PickBlockFromMouseClick(void)
{
	// ImGui���}�E�X���g���Ă�Ȃ�I���������L�����Z��
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

	// ���N���b�N�̂�
	if (!CManager::GetInputMouse()->GetTrigger(0))
	{
		return;
	}

	// ���C�擾�iCRayCast���g�p�j
	D3DXVECTOR3 rayOrigin, rayDir;
	CRayCast::GetMouseRay(rayOrigin, rayDir);

	float minDist = FLT_MAX;
	int hitIndex = -1;

	for (size_t i = 0; i < m_blocks.size(); ++i)
	{
		CBlock* block = m_blocks[i];

		// ���[���h�s��̎擾�i�ʒu�E��]�E�g����܂ށj
		D3DXMATRIX world = block->GetWorldMatrix();

		D3DXVECTOR3 modelSize = block->GetModelSize();
		D3DXVECTOR3 scale = block->GetSize();

		D3DXVECTOR3 halfSize;
		halfSize.x = modelSize.x * 0.5f;
		halfSize.y = modelSize.y * 0.5f;
		halfSize.z = modelSize.z * 0.5f;

		float dist = 0.0f;
		if (CRayCast::IntersectOBB(rayOrigin, rayDir, world, halfSize, dist))
		{
			if (dist < minDist)
			{
				minDist = dist;
				hitIndex = static_cast<int>(i);
			}
		}
	}

	// �I����Ԃ𔽉f
	if (hitIndex >= 0)
	{
		// �ȑO�I�΂�Ă����u���b�N���I����
		if (m_prevSelectedIdx != -1 && m_prevSelectedIdx != hitIndex)
		{
			m_blocks[m_prevSelectedIdx]->SetSelected(false);
		}

		// �V�����I��
		m_selectedIdx = hitIndex;
		m_blocks[m_selectedIdx]->SetSelected(true);
		m_prevSelectedIdx = hitIndex;
	}
}
//=============================================================================
// �R���C�_�[�̒�������
//=============================================================================
void CBlockManager::UpdateCollider(CBlock* selectedBlock)
{
	if (selectedBlock->IsCompoundCollider()) // �����͕����R���C�_�[�����肷��֐�
	{
		// �����R���C�_�[�pUI
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("COMPOUND COLLIDER PARTS");

		// �_�p�[�c
		{
			ColliderPart handle = selectedBlock->GetColliderHandle();
			static ColliderPart prevHandle = handle;

			ImGui::Text("Handle Size");
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂����Ē���
			bool changedSize = false;

			ImGui::Text("X:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80);
			changedSize |= ImGui::DragFloat("##handle_size_x", &handle.size.x, 0.1f, 0.1f, 800.0f, "%.1f");

			ImGui::SameLine();
			ImGui::Text("Y:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80);
			changedSize |= ImGui::DragFloat("##handle_size_y", &handle.size.y, 0.1f, 0.1f, 800.0f, "%.1f");

			ImGui::SameLine();
			ImGui::Text("Z:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80);
			changedSize |= ImGui::DragFloat("##handle_size_z", &handle.size.z, 0.1f, 0.1f, 800.0f, "%.1f");

			ImGui::Text("Handle Offset");
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			bool changedOffset = false;

			ImGui::Text("X:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80);
			changedOffset |= ImGui::DragFloat("##handle_offset_x", &handle.offset.x, 0.1f, -800.0f, 800.0f, "%.1f");

			ImGui::SameLine();
			ImGui::Text("Y:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80);
			changedOffset |= ImGui::DragFloat("##handle_offset_y", &handle.offset.y, 0.1f, -800.0f, 800.0f, "%.1f");

			ImGui::SameLine();
			ImGui::Text("Z:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80);
			changedOffset |= ImGui::DragFloat("##handle_offset_z", &handle.offset.z, 0.1f, -800.0f, 800.0f, "%.1f");

			if (changedSize || changedOffset)
			{
				selectedBlock->SetColliderHandle(handle);
				selectedBlock->CreatePhysicsFromParts();
				prevHandle = handle;
			}
		}

		// �n�p�[�c
		{
			ColliderPart blade = selectedBlock->GetColliderBlade();
			static ColliderPart prevBlade = blade;

			ImGui::Text("Blade Size");
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			bool changedSize = false;

			ImGui::Text("X:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80);
			changedSize |= ImGui::DragFloat("##blade_size_x", &blade.size.x, 0.1f, 0.1f, 800.0f, "%.1f");

			ImGui::SameLine();
			ImGui::Text("Y:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80);
			changedSize |= ImGui::DragFloat("##blade_size_y", &blade.size.y, 0.1f, 0.1f, 800.0f, "%.1f");

			ImGui::SameLine();
			ImGui::Text("Z:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80);
			changedSize |= ImGui::DragFloat("##blade_size_z", &blade.size.z, 0.1f, 0.1f, 800.0f, "%.1f");

			ImGui::Text("Blade Offset");
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			bool changedOffset = false;

			ImGui::Text("X:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80);
			changedOffset |= ImGui::DragFloat("##blade_offset_x", &blade.offset.x, 0.1f, -800.0f, 800.0f, "%.1f");

			ImGui::SameLine();
			ImGui::Text("Y:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80);
			changedOffset |= ImGui::DragFloat("##blade_offset_y", &blade.offset.y, 0.1f, -800.0f, 800.0f, "%.1f");

			ImGui::SameLine();
			ImGui::Text("Z:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80);
			changedOffset |= ImGui::DragFloat("##blade_offset_z", &blade.offset.z, 0.1f, -800.0f, 800.0f, "%.1f");

			if (changedSize || changedOffset)
			{
				selectedBlock->SetColliderBlade(blade);
				selectedBlock->CreatePhysicsFromParts();
				prevBlade = blade;
			}
		}
	}
	else
	{
		// �P��R���C�_�[�p
		D3DXVECTOR3 colliderSize = selectedBlock->GetColliderSize();
		static D3DXVECTOR3 prevSize = colliderSize;

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("COLLIDER SIZE");

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		bool changed = false;

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		changed |= ImGui::DragFloat("##collider_size_x", &colliderSize.x, 0.1f, 0.1f, 800.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		changed |= ImGui::DragFloat("##collider_size_y", &colliderSize.y, 0.1f, 0.1f, 800.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		changed |= ImGui::DragFloat("##collider_size_z", &colliderSize.z, 0.1f, 0.1f, 800.0f, "%.1f");

		if (changed && colliderSize != prevSize)
		{
			colliderSize.x = std::max(colliderSize.x, 0.01f);
			colliderSize.y = std::max(colliderSize.y, 0.01f);
			colliderSize.z = std::max(colliderSize.z, 0.01f);

			selectedBlock->SetColliderManual(colliderSize);

			prevSize = colliderSize;
		}

		D3DXVECTOR3 offset = selectedBlock->GetColliderOffset();
		static D3DXVECTOR3 prevOffset = offset;

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("COLLIDER OFFSET");

		bool offsetChanged = false;

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		offsetChanged |= ImGui::DragFloat("##collider_offset_x", &offset.x, 0.1f, -800.0f, 800.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		offsetChanged |= ImGui::DragFloat("##collider_offset_y", &offset.y, 0.1f, -800.0f, 800.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		offsetChanged |= ImGui::DragFloat("##collider_offset_z", &offset.z, 0.1f, -800.0f, 800.0f, "%.1f");

		if (offsetChanged && offset != prevOffset)
		{
			selectedBlock->SetColliderOffset(offset);
			selectedBlock->CreatePhysics(selectedBlock->GetPos(), selectedBlock->GetColliderSize());

			prevOffset = offset;
		}
	}
}
//=============================================================================
// X�t�@�C���p�X�̓ǂݍ���
//=============================================================================
void CBlockManager::LoadConfig(const std::string& filename)
{
	std::ifstream ifs(filename);
	if (!ifs) return;

	json j;
	ifs >> j;

	// j �͔z��ɂȂ��Ă�̂Ń��[�v����
	for (auto& block : j)
	{
		int typeInt = block["type"];
		std::string filepath = block["filepath"];
		std::string texFilepath = block["GUItexFilepath"];

		s_FilePathMap[(CBlock::TYPE)typeInt] = filepath;
		s_texFilePathMap[(CBlock::TYPE)typeInt] = texFilepath;
	}
}
//=============================================================================
// �^�C�v����X�t�@�C���p�X���擾
//=============================================================================
const char* CBlockManager::GetFilePathFromType(CBlock::TYPE type)
{
	auto it = s_FilePathMap.find(type);
	return (it != s_FilePathMap.end()) ? it->second.c_str() : "";
}
//=============================================================================
// �^�C�v����GUI�e�N�X�`���t�@�C���p�X���擾
//=============================================================================
const char* CBlockManager::GetTexPathFromType(CBlock::TYPE type)
{
	auto it = s_texFilePathMap.find(type);
	return (it != s_texFilePathMap.end()) ? it->second.c_str() : "";
}
//=============================================================================
// �u���b�N���̕ۑ�����
//=============================================================================
void CBlockManager::SaveToJson(const char* filename)
{
	// JSON�I�u�W�F�N�g
	json j;

	// 1�Â�JSON��
	for (const auto& block : m_blocks)
	{
		// ���W�A�����p�x�Ɉꎞ�ϊ�
		D3DXVECTOR3 degRot = D3DXToDegree(block->GetRot());
		
		json b;
		b["type"] = block->GetType();												// �u���b�N�̃^�C�v
		b["pos"] = { block->GetPos().x, block->GetPos().y, block->GetPos().z };		// �ʒu
		b["rot"] = { degRot.x, degRot.y, degRot.z };								// ����
		b["size"] = { block->GetSize().x, block->GetSize().y, block->GetSize().z };	// �T�C�Y
		//b["mass"] = { block->GetMass() };											// ����

		// �����R���C�_�[�Ȃ�A���ꂼ��ۑ�
		if (block->IsCompoundCollider())
		{
			ColliderPart handle = block->GetColliderHandle();
			ColliderPart blade = block->GetColliderBlade();

			b["collider_handle_size"] = { handle.size.x, handle.size.y, handle.size.z };
			b["collider_handle_offset"] = { handle.offset.x, handle.offset.y, handle.offset.z };

			b["collider_blade_size"] = { blade.size.x, blade.size.y, blade.size.z };
			b["collider_blade_offset"] = { blade.offset.x, blade.offset.y, blade.offset.z };
		}
		else
		{
			b["collider_size"] =
			{
				block->GetColliderSize().x,
				block->GetColliderSize().y,
				block->GetColliderSize().z
			};
		}

		// �ǉ�
		j.push_back(b);
	}

	// �o�̓t�@�C���X�g���[��
	std::ofstream file(filename);

	if (file.is_open())
	{
		file << std::setw(4) << j;

		// �t�@�C�������
		file.close();
	}
}
//=============================================================================
// �u���b�N���̓ǂݍ��ݏ���
//=============================================================================
void CBlockManager::LoadFromJson(const char* filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{// �J���Ȃ�����
		return;
	}

	json j;
	file >> j;

	// �t�@�C�������
	file.close();

	// �����̃u���b�N������
	for (auto block : m_blocks)
	{
		if (block != NULL)
		{
			// �u���b�N�̏I������
			block->Uninit();
		}
	}

	// ���I�z�����ɂ��� (�T�C�Y��0�ɂ���)
	m_blocks.clear();

	// �V���ɐ���
	for (const auto& b : j)
	{
		// �^�C�v�����擾���ė񋓌^�ɃL���X�g
		int typeInt = b["type"];
		CBlock::TYPE type = static_cast<CBlock::TYPE>(typeInt);

		D3DXVECTOR3 pos(b["pos"][0], b["pos"][1], b["pos"][2]);
		D3DXVECTOR3 degRot(b["rot"][0], b["rot"][1], b["rot"][2]);
		D3DXVECTOR3 size(b["size"][0], b["size"][1], b["size"][2]);

		//btScalar mass(b["mass"]);
		//if (b.contains("mass"))
		//{
		//	if (b["mass"].is_string())
		//	{
		//		mass = std::stof(b["mass"].get<std::string>());
		//	}
		//	else if (b["mass"].is_number_float() || b["mass"].is_number_integer())
		//	{
		//		mass = b["mass"].get<btScalar>();
		//	}
		//}

		D3DXVECTOR3 rot = D3DXToRadian(degRot); // �x�����W�A���ɕϊ�

		// �^�C�v����u���b�N����
		CBlock* block = CreateBlock(type, pos);

		if (!block)
		{
			continue;
		}

		block->SetRot(rot);
		block->SetSize(size);
		//block->SetMass(mass);

		// �����R���C�_�[�����肵�ď����𕪂���
		if (b.contains("collider_handle_size") && b.contains("collider_blade_size"))
		{
			ColliderPart handle, blade;

			handle.size = D3DXVECTOR3(
				b["collider_handle_size"][0],
				b["collider_handle_size"][1],
				b["collider_handle_size"][2]);

			handle.offset = D3DXVECTOR3(
				b["collider_handle_offset"][0],
				b["collider_handle_offset"][1],
				b["collider_handle_offset"][2]);

			blade.size = D3DXVECTOR3(
				b["collider_blade_size"][0],
				b["collider_blade_size"][1],
				b["collider_blade_size"][2]);

			blade.offset = D3DXVECTOR3(
				b["collider_blade_offset"][0],
				b["collider_blade_offset"][1],
				b["collider_blade_offset"][2]);

			block->SetColliderHandle(handle);
			block->SetColliderBlade(blade);
			block->CreatePhysicsFromParts(); // �����p�Đ���
		}
		else if (b.contains("collider_size"))
		{
			D3DXVECTOR3 colliderSize(
				b["collider_size"][0],
				b["collider_size"][1],
				b["collider_size"][2]);

			block->SetColliderSize(colliderSize);
			block->UpdateCollider(); // �P��p�Đ���
		}
	}
}
//=============================================================================
// �u���^�����̒��Ώ�ԃ`�F�b�N
//=============================================================================
bool CBlockManager::CheckAllTorches(void)
{
	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() == CBlock::TYPE_TORCH3)
		{
			CTorch3Block* pTorch3 = dynamic_cast<CTorch3Block*>(block);
			if (!pTorch3->IsHit())  // �΂����ĂȂ������������
			{
				return false;  // �܂��S�����ĂȂ�
			}
		}
	}

	return true; // ���ׂĉ΂����Ă���
}
//=============================================================================
// ������X�C�b�`�̉����`�F�b�N
//=============================================================================
bool CBlockManager::CheckAllSwitch(void)
{
	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() == CBlock::TYPE_BRIDGESWITCH)
		{
			CBridgeSwitchBlock* pTSwitch = dynamic_cast<CBridgeSwitchBlock*>(block);
			if (!pTSwitch->IsSwitchOn())  // ������Ă��Ȃ��X�C�b�`�������
			{
				return false;  // �܂��S��������Ă��Ȃ�
			}
		}
	}

	return true; // ���ׂĉ�����Ă���
}
//=============================================================================
// �S�u���b�N�̎擾
//=============================================================================
const std::vector<CBlock*>& CBlockManager::GetAllBlocks(void)
{
	return m_blocks;
}