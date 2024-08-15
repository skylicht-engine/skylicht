#pragma once

#include "ViewManager/CView.h"
#include "GameObject/CGameObject.h"

#include "UserInterface/CUIContainer.h"
#include "UserInterface/CUIListView.h"
#include "UserInterface/CUIGridView.h"
#include "UserInterface/CUIButton.h"

class CViewDemo : public CView
{
protected:
	UI::CUIContainer* m_uiContainer;
	UI::CUIListView* m_listUniform;
	std::vector<UI::CUIButton*> m_listUniformBtn;
	CCanvas* m_canvas;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void onGUI();

	void addListIconItem(SFrame* frame);

	void onChangeUniform(UI::CUIBase* btn);
};
