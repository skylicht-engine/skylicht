#pragma once

#include "ViewManager/CView.h"
#include "CIKHand.h"
#include "Animation/CGradientBandInterpolation.h"
#include "Animation/CAnimationController.h"

class CViewDemo : public CView
{
protected:
	CGradientBandInterpolation m_movementBlending;
	CGradientBandInterpolation m_aimBlending;

	CAnimationController* m_animController;
	CIKHand* m_handIK;

	float m_movementSpeed;
	float m_movementAngle;
	float m_aim;
	float m_aimUpDown;
	float m_aimLeftRight;
	bool m_modify;

	core::vector3df m_aimPosition;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	void onGUI();
};