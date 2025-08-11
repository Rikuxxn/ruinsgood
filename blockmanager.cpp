//=============================================================================
//
// ブロックマネージャー処理 [blockmanager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "blockmanager.h"
#include "json.hpp"
#include "FileDialogUtils.h"
#include "manager.h"
#include "imgui_internal.h"
#include "raycast.h"
#include "game.h"

using json = nlohmann::json;

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
std::vector<CBlock*> CBlockManager::m_blocks = {};	// ブロックの情報
int CBlockManager::m_nNumAll = 0;				// ブロックの総数
int CBlockManager::m_selectedIdx = 0;			// 選択中のインデックス
CBlock* CBlockManager::m_draggingBlock = {};
std::unordered_map<CBlock::TYPE, const char*> s_FilePathMap = {};

//=============================================================================
// コンストラクタ
//=============================================================================
CBlockManager::CBlockManager()
{
	// 値のクリア
	m_prevSelectedIdx = -1;
	m_hasConsumedPayload = false;
	m_pDebug3D = NULL;							// 3Dデバッグ表示へのポインタ
	m_autoUpdateColliderSize = true;
}
//=============================================================================
// デストラクタ
//=============================================================================
CBlockManager::~CBlockManager()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CBlock* CBlockManager::CreateBlock(CBlock::TYPE type, D3DXVECTOR3 pos)
{
	const char* path = CBlockManager::GetFilePathFromType(type);

	CBlock* newBlock = CBlock::Create(path, pos, D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1), type);

	if (newBlock)
	{
		m_blocks.push_back(newBlock);
		m_nNumAll++;			// 総数のカウントアップ
	}

	return newBlock;
}
//=============================================================================
// 初期化処理
//=============================================================================
void CBlockManager::Init(void)
{
	// 動的配列を空にする (サイズを0にする)
	m_blocks.clear();
}
//=============================================================================
// 終了処理
//=============================================================================
void CBlockManager::Uninit(void)
{
	m_nNumAll = 0;

	// 動的配列を空にする (サイズを0にする)
	m_blocks.clear();
}
//=============================================================================
// 更新処理
//=============================================================================
void CBlockManager::Update(void)
{
#ifdef _DEBUG

	// 情報の更新
	UpdateInfo();

	// ドラッグ処理の更新
	UpdateDraggingBlock();

#endif
}
//=============================================================================
// 情報の更新処理
//=============================================================================
void CBlockManager::UpdateInfo(void)
{
	// ImGuiマネージャーの取得
	CImGuiManager* pImGuiManager = CGame::GetImGuiManager();

	// GUIスタイルの取得
	ImGuiStyle& style = ImGui::GetStyle();

	style.GrabRounding		= 10.0f;		// スライダーのつまみを丸く
	style.ScrollbarRounding = 10.0f;		// スクロールバーの角
	style.ChildRounding		= 10.0f;		// 子ウィンドウの角
	style.WindowRounding	= 10.0f;		// ウィンドウ全体の角

	// 場所
	pImGuiManager->SetPosImgui(ImVec2(1900.0f, 20.0f));

	// サイズ
	pImGuiManager->SetSizeImgui(ImVec2(420.0f, 500.0f));

	// 最初のGUI
	pImGuiManager->StartImgui(u8"BlockInfo", CImGuiManager::IMGUITYPE_DEFOULT);

	// ブロックがない場合
	if (m_blocks.empty())
	{
		ImGui::Text("No blocks placed yet.");
	}
	else
	{
		// ブロックの総数
		ImGui::Text("Block Num %d", m_nNumAll);

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// インデックス選択
		ImGui::SliderInt("Block Index", &m_selectedIdx, 0, (int)m_blocks.size() - 1);

		// 範囲外対策
		if (m_selectedIdx >= (int)m_blocks.size())
		{
			m_selectedIdx = (int)m_blocks.size() - 1;
		}

		// 前回選んでたブロックを解除
		if (m_prevSelectedIdx != -1 && m_prevSelectedIdx != m_selectedIdx)
		{
			m_blocks[m_prevSelectedIdx]->SetSelected(false);
		}

		// 対象ブロックの取得
		CBlock* selectedBlock = m_blocks[m_selectedIdx];

		// ブロック情報の調整処理
		UpdateTransform(selectedBlock);

		if (GetUpdateCollider() == false)
		{
			// コライダーの調整処理
			UpdateCollider(selectedBlock);
		}
		else
		{
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける
			ImGui::Text("Auto Update Collider Size is Active");
		}
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

	// ブロックタイプ一覧
	if (ImGui::TreeNode("Block Types"))
	{
		for (int typeInt = 0; typeInt < (int)CBlock::TYPE_MAX; typeInt++)
		{
			CBlock::TYPE type = static_cast<CBlock::TYPE>(typeInt);

			// テクスチャ取得
			const char* texPath = CBlock::GetTexPathFromType(type);
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

				// 4つごとに改行
				if (typeInt % 4 != 3)
				{
					ImGui::SameLine();
				}
			}
		}

		ImGui::NewLine(); // 次の行へ
		ImGui::TreePop(); // ツリー閉じる
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

	if (ImGui::Button("Save"))
	{
		// ダイアログを開いてファイルに保存
		std::string path = OpenWindowsSaveFileDialog();

		if (!path.empty())
		{
			// データの保存
			CBlockManager::SaveToJson(path.c_str());
		}
	}

	ImGui::SameLine(0);

	if (ImGui::Button("Load"))
	{
		// ダイアログを開いてファイルを開く
		std::string path = OpenWindowsOpenFileDialog();

		if (!path.empty())
		{
			// データの読み込み
			CBlockManager::LoadFromJson(path.c_str());
		}
	}

	ImGui::End();

	if (CManager::GetMode() == MODE_TITLE)
	{
		return;
	}

	// マウス選択処理
	PickBlockFromMouseClick();
}
//=============================================================================
// ブロック情報の調整処理
//=============================================================================
void CBlockManager::UpdateTransform(CBlock* selectedBlock)
{
	if (selectedBlock)
	{
		// 選択中のブロックの色を変える
		selectedBlock->SetSelected(true);

		D3DXVECTOR3 pos = selectedBlock->GetPos();	// 選択中のブロックの位置の取得
		D3DXVECTOR3 rot = selectedBlock->GetRot();	// 選択中のブロックの向きの取得
		D3DXVECTOR3 size = selectedBlock->GetSize();// 選択中のブロックのサイズの取得

		// ラジアン→角度に一時変換（静的変数で保持し操作中のみ更新）
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
			selectedBlock->SetEditMode(true);  // 中でKinematic化
		}
		else
		{
			selectedBlock->SetEditMode(false); // 通常に戻す
		}

		//*********************************************************************
		// POS の調整
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// ラベル
		ImGui::Text("POS"); ImGui::SameLine(60); // ラベルの位置ちょっと調整

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
		// ROT の調整
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
		// SIZE の調整
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// ラベル
		ImGui::Text("SIZE"); ImGui::SameLine(60); // ラベルの位置ちょっと調整

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

		// チェックボックス：拡大率に応じて自動更新するか
		ImGui::Checkbox("Auto Update Collider Size", &m_autoUpdateColliderSize);

		// 前回のサイズを保持
		static D3DXVECTOR3 prevSize = selectedBlock->GetSize();

		// サイズ変更チェック
		bool isSizeChanged = (size != prevSize);

		// 角度→ラジアンに戻す
		D3DXVECTOR3 rotRad = D3DXToRadian(degRot);

		// 位置の設定
		selectedBlock->SetPos(pos);

		// サイズの設定
		selectedBlock->SetSize(size);

		// サイズ(拡大率)が変わったときだけ呼ぶ
		if (m_autoUpdateColliderSize == true && isSizeChanged)
		{
			// ブロックサイズによるコライダーの生成
			selectedBlock->CreatePhysicsFromScale(size);
			prevSize = size; // 更新
		}

		if (isRotChanged)
		{
			// 回転が変わった時だけセット
			selectedBlock->SetRot(rotRad);

			// 編集モードなら即物理Transformも更新して同期
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
			// 編集していない時はdegRotをselectedBlockの値に同期しておく
			degRot = D3DXToDegree(selectedBlock->GetRot());
		}

		//*********************************************************************
		// ブロックの削除
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));

		if (ImGui::Button("Delete"))
		{
			if (m_autoUpdateColliderSize)
			{
				m_nNumAll--;		// 総数のカウントダウン

				if (m_blocks[m_selectedIdx])
				{
					// 選択中のブロックを削除
					m_blocks[m_selectedIdx]->Uninit();
				}

				m_blocks.erase(m_blocks.begin() + m_selectedIdx);

				// 選択インデックスを調整
				if (m_selectedIdx >= (int)m_blocks.size())
				{
					m_selectedIdx = (int)m_blocks.size() - 1;
				}

				m_prevSelectedIdx = -1;
			}
			else
			{// m_autoUpdateColliderSizeがfalseの時は何もしない
				
			}
		}

		ImGui::PopStyleColor(3);
	}

	// 最後に保存
	m_prevSelectedIdx = m_selectedIdx;
}
//=============================================================================
// ブロックのドラッグ処理
//=============================================================================
void CBlockManager::UpdateDraggingBlock(void)
{
	// マウスの取得
	CInputMouse* pMouse = CManager::GetInputMouse();

	// 現在のImGuiの内部状態（コンテキスト）へのポインターを取得
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	if (!m_draggingBlock && !m_hasConsumedPayload)
	{
		// ドラッグ＆ドロップ中なら即生成して追従開始
		if (ctx->DragDropActive && ctx->DragDropPayload.Data && ctx->DragDropPayload.DataSize == sizeof(CBlock::TYPE))
		{
			if (ctx->DragDropPayload.IsDataType("BLOCK_TYPE"))
			{
				CBlock::TYPE draggedType = *(CBlock::TYPE*)ctx->DragDropPayload.Data;

				D3DXVECTOR3 pos = pMouse->GetGroundHitPosition();
				pos.y = 30.0f;

				// ブロックの生成
				m_draggingBlock = CreateBlock(draggedType, pos);

				// 生成時はキネマティック化（EditMode ON）
				m_draggingBlock->SetEditMode(true);

				m_hasConsumedPayload = true;
			}
		}
	}
	else
	{
		// ドラッグ中はマウスに追従させる
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			D3DXVECTOR3 pos = pMouse->GetGroundHitPosition();
			pos.y = 30.0f;
			m_draggingBlock->SetPos(pos);
		}
		else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			// 離したら通常物理に戻す（キネマティック解除）
			m_draggingBlock->SetEditMode(false);

			// ドラッグ終了
			m_draggingBlock = NULL;
		}

		// ImGuiのドラッグドロップが完全に終わったらフラグリセット
		if (!ctx->DragDropActive)
		{
			m_hasConsumedPayload = false;
		}
	}
}
//=============================================================================
// ブロック選択処理
//=============================================================================
void CBlockManager::PickBlockFromMouseClick(void)
{
	// ImGuiがマウスを使ってるなら選択処理をキャンセル
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

	// 左クリックのみ
	if (!CManager::GetInputMouse()->GetTrigger(0))
	{
		return;
	}

	// レイ取得（CRayCastを使用）
	D3DXVECTOR3 rayOrigin, rayDir;
	CRayCast::GetMouseRay(rayOrigin, rayDir);

	float minDist = FLT_MAX;
	int hitIndex = -1;

	for (size_t i = 0; i < m_blocks.size(); ++i)
	{
		CBlock* block = m_blocks[i];

		// ワールド行列の取得（位置・回転・拡大を含む）
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

	// 選択状態を反映
	if (hitIndex >= 0)
	{
		// 以前選ばれていたブロックを非選択に
		if (m_prevSelectedIdx != -1 && m_prevSelectedIdx != hitIndex)
		{
			m_blocks[m_prevSelectedIdx]->SetSelected(false);
		}

		// 新しく選択
		m_selectedIdx = hitIndex;
		m_blocks[m_selectedIdx]->SetSelected(true);
		m_prevSelectedIdx = hitIndex;
	}
}
//=============================================================================
// コライダーの調整処理
//=============================================================================
void CBlockManager::UpdateCollider(CBlock* selectedBlock)
{
	if (selectedBlock->IsCompoundCollider()) // ここは複合コライダーか判定する関数
	{
		// 複合コライダー用UI
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("COMPOUND COLLIDER PARTS");

		// 棒パーツ
		{
			ColliderPart handle = selectedBlock->GetColliderHandle();
			static ColliderPart prevHandle = handle;

			ImGui::Text("Handle Size");
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を入れて調整
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

		// 刃パーツ
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
		// 単一コライダー用
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
// Xファイルパスの設定
//=============================================================================
const std::unordered_map<CBlock::TYPE, const char*> CBlockManager::s_FilePathMap =
{
	{ CBlock::TYPE_WOODBOX,			"data/MODELS/woodbox_003.x" },
	{ CBlock::TYPE_WALL,			"data/MODELS/wall_01.x" },
	{ CBlock::TYPE_WALL2,			"data/MODELS/wall_02.x" },
	{ CBlock::TYPE_WALL3,			"data/MODELS/wall_03.x" },
	{ CBlock::TYPE_WALL4,			"data/MODELS/wall_04.x" },
	{ CBlock::TYPE_AXE,				"data/MODELS/Axe_01.x" },
	{ CBlock::TYPE_RAFT,			"data/MODELS/ikada.x" },
	{ CBlock::TYPE_ROCK,			"data/MODELS/Rock_001.x" },
	{ CBlock::TYPE_TORCH,			"data/MODELS/torch_01.x" },
	{ CBlock::TYPE_TORCH2,			"data/MODELS/torch_02.x" },
	{ CBlock::TYPE_FLOOR,			"data/MODELS/floor_01.x" },
	{ CBlock::TYPE_FLOOR2,			"data/MODELS/floor_02.x" },
	{ CBlock::TYPE_DOOR,			"data/MODELS/door_01.x" },
	{ CBlock::TYPE_CEILING,			"data/MODELS/ceiling_01.x" },
	{ CBlock::TYPE_CEILING2,		"data/MODELS/ceiling_02.x" },
	{ CBlock::TYPE_WATER,			"data/MODELS/water.x" },
	{ CBlock::TYPE_SWITCH,			"data/MODELS/switch.x" },
	{ CBlock::TYPE_SWITCH_BODY,		"data/MODELS/switch_body.x" },
	{ CBlock::TYPE_BRIDGE,			"data/MODELS/bridge_01.x" },
	{ CBlock::TYPE_DOOR_TOP,		"data/MODELS/wall_door_parttop.x" },
	{ CBlock::TYPE_DOOR_SIDE,		"data/MODELS/wall_door_partleft.x" },
	{ CBlock::TYPE_PILLAR,			"data/MODELS/pillar_001.x" },
	{ CBlock::TYPE_BLOCK,			"data/MODELS/block.x" },
	{ CBlock::TYPE_FENCE,			"data/MODELS/fence.x" },
	{ CBlock::TYPE_FENCE_PART,		"data/MODELS/fence_part.x" },
	{ CBlock::TYPE_BRIDGE2,			"data/MODELS/bridge_02.x" },
	{ CBlock::TYPE_TARGET,			"data/MODELS/target.x" },
	{ CBlock::TYPE_SWITCH2,			"data/MODELS/switch2.x" },
	{ CBlock::TYPE_DOOR2,			"data/MODELS/door_02.x" },
	{ CBlock::TYPE_MASK,			"data/MODELS/mask.x" },
	{ CBlock::TYPE_SWORD,			"data/MODELS/sword.x" },
	{ CBlock::TYPE_SWITCH3,			"data/MODELS/switch3.x" },
	{ CBlock::TYPE_BAR,				"data/MODELS/bar.x" },
	{ CBlock::TYPE_BRIDGE3,			"data/MODELS/bridge_03.x" },
	{ CBlock::TYPE_FIRE_STATUE,		"data/MODELS/fire_statue.x" },
	{ CBlock::TYPE_WALL5,			"data/MODELS/wall_05.x" },
	{ CBlock::TYPE_FLOOR3,			"data/MODELS/floor_03.x" },
	{ CBlock::TYPE_TURN_FIRE_STATUE,"data/MODELS/turn_fire_statue.x" },
	{ CBlock::TYPE_BLOCK2,			"data/MODELS/block_01.x" },
	{ CBlock::TYPE_STAIRS,			"data/MODELS/stairs.x" },
	{ CBlock::TYPE_PILLAR2,			"data/MODELS/pillar_002.x" },
	{ CBlock::TYPE_BLOCK3,			"data/MODELS/block_02.x" },
	{ CBlock::TYPE_FLOOR4,			"data/MODELS/floor_04.x" },
	{ CBlock::TYPE_MOVE_FIRE_STATUE,"data/MODELS/fire_statue.x" },
	{ CBlock::TYPE_TORCH3,			"data/MODELS/torch_02.x" },
	{ CBlock::TYPE_NETFLOOR,		"data/MODELS/net.x" },
};
//=============================================================================
// タイプからXファイルパスを取得
//=============================================================================
const char* CBlockManager::GetFilePathFromType(CBlock::TYPE type)
{
	auto it = s_FilePathMap.find(type);
	return (it != s_FilePathMap.end()) ? it->second : "";
}
//=============================================================================
// ブロック情報の保存処理
//=============================================================================
void CBlockManager::SaveToJson(const char* filename)
{
	// JSONオブジェクト
	json j;

	// 1つづつJSON化
	for (const auto& block : m_blocks)
	{
		// ラジアン→角度に一時変換
		D3DXVECTOR3 degRot = D3DXToDegree(block->GetRot());
		
		json b;
		b["type"] = block->GetType();												// ブロックのタイプ
		b["pos"] = { block->GetPos().x, block->GetPos().y, block->GetPos().z };		// 位置
		b["rot"] = { degRot.x, degRot.y, degRot.z };								// 向き
		b["size"] = { block->GetSize().x, block->GetSize().y, block->GetSize().z };	// サイズ

		// 複合コライダーなら、それぞれ保存
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

		// 追加
		j.push_back(b);
	}

	// 出力ファイルストリーム
	std::ofstream file(filename);

	if (file.is_open())
	{
		file << std::setw(4) << j;

		// ファイルを閉じる
		file.close();
	}
}
//=============================================================================
// ブロック情報の読み込み処理
//=============================================================================
void CBlockManager::LoadFromJson(const char* filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{// 開けなかった
		return;
	}

	json j;
	file >> j;

	// ファイルを閉じる
	file.close();

	// 既存のブロックを消す
	for (auto block : m_blocks)
	{
		if (block != NULL)
		{
			// ブロックの終了処理
			block->Uninit();
		}
	}

	// 動的配列を空にする (サイズを0にする)
	m_blocks.clear();

	// 新たに生成
	for (const auto& b : j)
	{
		// タイプ情報を取得して列挙型にキャスト
		int typeInt = b["type"];
		CBlock::TYPE type = static_cast<CBlock::TYPE>(typeInt);

		D3DXVECTOR3 pos(b["pos"][0], b["pos"][1], b["pos"][2]);
		D3DXVECTOR3 degRot(b["rot"][0], b["rot"][1], b["rot"][2]);
		D3DXVECTOR3 size(b["size"][0], b["size"][1], b["size"][2]);

		D3DXVECTOR3 rot = D3DXToRadian(degRot); // 度→ラジアンに変換

		// タイプからブロック生成
		CBlock* block = CreateBlock(type, pos);

		if (block)
		{
			block->SetRot(rot);
			block->SetSize(size);

			// 複合コライダーか判定して処理を分ける
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
				block->CreatePhysicsFromParts(); // 複合用再生成
			}
			else if (b.contains("collider_size"))
			{
				D3DXVECTOR3 colliderSize(
					b["collider_size"][0],
					b["collider_size"][1],
					b["collider_size"][2]);

				block->SetColliderSize(colliderSize);
				block->UpdateCollider(); // 単一用再生成
			}
		}
	}
}
//=============================================================================
// 置き型松明の着火状態チェック
//=============================================================================
bool CBlockManager::CheckAllTorches(void)
{
	for (CBlock* block : CBlockManager::GetAllBlocks())
	{
		if (block->GetType() == CBlock::TYPE_TORCH3)
		{
			CTorch3Block* pTorch3 = dynamic_cast<CTorch3Block*>(block);
			if (!pTorch3->IsHit())  // 火がついてない松明があれば
			{
				return false;  // まだ全部ついてない
			}
		}
	}

	return true; // すべて火がついている
}
//=============================================================================
// 全ブロックの取得
//=============================================================================
const std::vector<CBlock*>& CBlockManager::GetAllBlocks(void)
{
	return m_blocks;
}