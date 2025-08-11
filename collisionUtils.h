//=============================================================================
//
// 当たり判定処理 [collisionUtils.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _COLLISIONUTILS_H_// このマクロ定義がされていなかったら
#define _COLLISIONUTILS_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************

//*****************************************************************************
// 当たり判定クラス
//*****************************************************************************
class CCollision
{
public:

	// シリンダーとAABBの当たり判定(その距離を返す)
	static bool CheckCylinderAABBCollisionWithHitDistance(
		const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight, const D3DXVECTOR3& cylinderDir,
		const D3DXVECTOR3& aabbMin, const D3DXVECTOR3& aabbMax,
		float* outHitDistance);

	// シリンダーとカプセルの当たり判定
	static bool CheckCapsuleCylinderCollision_Dir(
		const D3DXVECTOR3& capsuleCenter, float capsuleRadius, float capsuleHeight,
		const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight,
		const D3DXVECTOR3& cylinderDir, bool flag);

};


#endif