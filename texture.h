//=============================================================================
//
// テクスチャ処理 [texture.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TEXTURE_H_// このマクロ定義がされていなかったら
#define _TEXTURE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_TEXTURE (256)


//*****************************************************************************
// テクスチャクラス
//*****************************************************************************
class CTexture
{
public:
	CTexture();
	~CTexture();

	HRESULT Load(void);
	void Unload(void);
	int Register(const char* pFilename);
	int RegisterX(const char* pFilename);
	LPDIRECT3DTEXTURE9 GetAddress(int nIdx);

private:
	const char* TEXTURE[MAX_TEXTURE] =
	{
		"data/TEXTURE/num001.png",
		"data/TEXTURE/pause.png",
		"data/TEXTURE/time.png",
		"data/TEXTURE/colon.png",
		"data/TEXTURE/ground.png",
		"data/TEXTURE/wall001.jpg",
		"data/TEXTURE/shadow000.jpg",
		"data/TEXTURE/eye.png",
		"data/TEXTURE/rock.png",
		"data/TEXTURE/woodbox.png",
		"data/TEXTURE/Axe.png",
		"data/TEXTURE/ikada.png",
		"data/TEXTURE/wall1.png",
		"data/TEXTURE/wall2.png",
		"data/TEXTURE/wall3.png",
		"data/TEXTURE/wall4.png",
		"data/TEXTURE/torch1.png",
		"data/TEXTURE/torch2.png",
		"data/TEXTURE/floor1.png",
		"data/TEXTURE/floor2.png",
		"data/TEXTURE/wall_door1.png",
		"data/TEXTURE/ceiling1.png",
		"data/TEXTURE/ceiling2.png",
		"data/TEXTURE/door1.png",
		"data/TEXTURE/water.png",
		"data/TEXTURE/switch.png",
		"data/TEXTURE/switch_body.png",
		"data/TEXTURE/bridge.png",
		"data/TEXTURE/door_top.png",
		"data/TEXTURE/door_left.png",
		"data/TEXTURE/door_right.png",
		"data/TEXTURE/pillar.png",
		"data/TEXTURE/switch_tex1.png",
		"data/TEXTURE/block.png",
		"data/TEXTURE/fence.png",
		"data/TEXTURE/fence_part.png",
		"data/TEXTURE/bridge2.png",
		"data/TEXTURE/target.png",
		"data/TEXTURE/switch3.png",
		"data/TEXTURE/switch_tex2.png",
		"data/TEXTURE/door2.png",
		"data/TEXTURE/effect000.jpg",
		"data/TEXTURE/treasure_effect.png",
		"data/TEXTURE/smoke.jpg",
		"data/TEXTURE/mask.png",
		"data/TEXTURE/sword.png",
		"data/TEXTURE/golden.jpg",
		"data/TEXTURE/title.png",
		"data/TEXTURE/press.png",
		"data/TEXTURE/continue.png",
		"data/TEXTURE/retry.png",
		"data/TEXTURE/quit.png",
		"data/TEXTURE/ui_move.png",
		"data/TEXTURE/ui_jump.png",
		"data/TEXTURE/ui_pick.png",
		"data/TEXTURE/ui_pause.png",
		"data/TEXTURE/ui_mask.png",
		"data/TEXTURE/ui_result001.png",
		"data/TEXTURE/ui_result002.png",
		"data/TEXTURE/ui_masknotfound.png",
		"data/TEXTURE/ui_maskfound.png",
		"data/TEXTURE/ui_result005.png",
		"data/TEXTURE/ui_result006.png",
		"data/TEXTURE/resultBG.png",
		"data/TEXTURE/switch2.png",
		"data/TEXTURE/switch_tex3.png",
		"data/TEXTURE/bridge3.png",
		"data/TEXTURE/bar.png",
		"data/TEXTURE/stage_01.png",
		"data/TEXTURE/stage_none.png",
		"data/TEXTURE/back.png",
		"data/TEXTURE/fire_statue.png",
		"data/TEXTURE/wall.jpg",
		"data/TEXTURE/wall5.png",
		"data/TEXTURE/floor3.png",
		"data/TEXTURE/ground.jpg",
		"data/TEXTURE/turn_fire_statue.png",
		"data/TEXTURE/block2.png",
		"data/TEXTURE/stairs.png",
		"data/TEXTURE/block3.png",
		"data/TEXTURE/magma.jpg",
		"data/TEXTURE/floor4.png",
		"data/TEXTURE/net.png",
		"data/TEXTURE/netfloor.png",
		"data/TEXTURE/keyfence.png",
		"data/TEXTURE/key.png",
		"data/TEXTURE/key_pedestal.png",
		"data/TEXTURE/keydoor_tex.png",
		"data/TEXTURE/keydoor.png",
		"data/TEXTURE/shield.png",
		"data/TEXTURE/stage_02.png",
		"data/TEXTURE/statue.png",
		"data/TEXTURE/statue2.png",
		"data/TEXTURE/egg.png",
		"data/TEXTURE/ui_egg.png",
		"data/TEXTURE/ui_eggfound.png",
		"data/TEXTURE/ui_eggnotfound.png",
		"data/TEXTURE/collected.png",
		"data/TEXTURE/notfound.png",
		"data/TEXTURE/hint.png",
		"data/TEXTURE/doorTriggerBlock.png",
		"data/TEXTURE/text.png",
		"data/TEXTURE/ui_interact.png",
		"data/TEXTURE/hint2.png",
		"data/TEXTURE/hint3.png",
		"data/TEXTURE/hint4.png",
		"data/TEXTURE/hint5.png",
		"data/TEXTURE/pillar2.png",
		"data/TEXTURE/hint6.png",
		"data/TEXTURE/massBlock_circle.png",
		"data/TEXTURE/massBlock_triangle.png",
		"data/TEXTURE/massBlock_square.png",
		"data/TEXTURE/massBlock_star.png",
		"data/TEXTURE/hint7.png",
		"data/TEXTURE/hint8.png",
		"data/TEXTURE/hint9.png",
		"data/TEXTURE/hint10.png",
		"data/TEXTURE/hint11.png",
		"data/TEXTURE/hint12.png",
		"data/TEXTURE/push.png",
		"data/TEXTURE/respawnBlock.png",
		"data/TEXTURE/waterWheel.png",
		"data/TEXTURE/pipe.png",
		"data/TEXTURE/circle.png",
		"data/TEXTURE/triangle.png",
		"data/TEXTURE/square.png",
		"data/TEXTURE/star.png",
	};

	LPDIRECT3DTEXTURE9 m_apTexture[MAX_TEXTURE];
	static int m_nNumAll;
};

#endif