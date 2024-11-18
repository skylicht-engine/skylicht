#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"
#include "CImguiManager.h"
#include "imgui.h"

#include "CDemoObstacleAvoidance.h"

#include "Projective/CProjective.h"

CViewDemo::CViewDemo() :
	m_demoId(0)
{
	m_demo[0] = NULL;
	m_demo[1] = NULL;
}

CViewDemo::~CViewDemo()
{
	delete m_demo[0];

	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	scene->unRegisterEvent(this);
}

void CViewDemo::onInit()
{
	CContext* context = CContext::getInstance();
	CCamera* camera = context->getActiveCamera();

	CScene* scene = context->getScene();
	scene->updateIndexSearchObject();
	scene->registerEvent("Demo", this);

	CZone* zone = context->getActiveZone();

	m_demo[0] = new CDemoObstacleAvoidance(zone);
	m_demo[1] = NULL;

	initDemo();
}

void CViewDemo::onDestroy()
{

}

void CViewDemo::onUpdate()
{
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	if (scene != NULL)
		scene->update();

	// update demo
	if (m_demo[m_demoId])
		m_demo[m_demoId]->update();

	// imgui update
	CImguiManager::getInstance()->onNewFrame();
}

void CViewDemo::onRender()
{
	CContext* context = CContext::getInstance();

	CCamera* camera = context->getActiveCamera();
	CCamera* guiCamera = context->getGUICamera();

	CScene* scene = context->getScene();

	// render scene
	if (camera != NULL && scene != NULL)
	{
		context->getRenderPipeline()->render(NULL, camera, scene->getEntityManager(), core::recti());
	}

	// render GUI
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}

	// imgui render
	onGUI();
	CImguiManager::getInstance()->onRender();
}

void CViewDemo::onPostRender()
{

}

void CViewDemo::onGUI()
{
	bool open = true;

	ImGuiWindowFlags window_flags = 0;

	// We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
	ImGui::SetNextWindowPos(ImVec2(850, 15), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(280, 270), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Graph Test", &open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// BEGIN WINDOW
	{
		const char* demoName[] = {
			"Obstacle Avoidance",
			"Recast mesh",
		};

		if (ImGui::Combo("Demo", &m_demoId, demoName, IM_ARRAYSIZE(demoName)))
		{
			initDemo();
		}

		if (ImGui::CollapsingHeader("Demo", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (m_demo[m_demoId])
				m_demo[m_demoId]->onGUI();
		}

		if (ImGui::Button("Reset"))
		{
			initDemo();
		}

		ImGui::End();
	}
}

void CViewDemo::initDemo()
{
	if (m_demo[m_demoId])
		m_demo[m_demoId]->init();
}

bool CViewDemo::OnEvent(const SEvent& event)
{
	if (event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		float mouseX = (float)event.MouseInput.X;
		float mouseY = (float)event.MouseInput.Y;

		if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
		{
			CCamera* camera = CContext::getInstance()->getActiveCamera();

			core::vector3df projectilePosition(0.0f, 1.0f, 0.0f);
			core::vector3df collide;

			const core::recti& vp = getVideoDriver()->getViewPort();

			core::line3df viewRay = CProjective::getViewRay(camera, mouseX, mouseY, vp.getWidth(), vp.getHeight());

			// plane test
			core::plane3df p(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
			bool hitCollide = p.getIntersectionWithLimitedLine(viewRay.start, viewRay.end, collide);

			if (hitCollide == true)
			{
				if (m_demo[m_demoId])
					m_demo[m_demoId]->onLeftClickPosition(collide);
			}

			return true;
		}
		else if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
		{
			return true;
		}
	}

	return false;
}