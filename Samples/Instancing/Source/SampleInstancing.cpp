#include "pch.h"
#include "SampleInstancing.h"

#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "CViewInit.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleInstancing* app = new SampleInstancing();
	getApplication()->registerAppEvent("SampleInstancing", app);
}

SampleInstancing::SampleInstancing()
{
	CContext::createGetInstance();
	CViewManager::createGetInstance()->initViewLayer(1);
	CLightmapper::createGetInstance();
}

SampleInstancing::~SampleInstancing()
{
	CViewManager::releaseInstance();
	CContext::releaseInstance();
	CLightmapper::releaseInstance();
}

void SampleInstancing::onInitApp()
{
	CViewManager::getInstance()->getLayer(0)->pushView<CViewInit>();
}

void SampleInstancing::onUpdate()
{
	CViewManager::getInstance()->update();
}

void SampleInstancing::onRender()
{
	CViewManager::getInstance()->render();
}

void SampleInstancing::onPostRender()
{
	// post render application
	CViewManager::getInstance()->postRender();
}

bool SampleInstancing::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)	
	return CViewManager::getInstance()->onBack();
}

void SampleInstancing::onResize(int w, int h)
{
	if (CContext::getInstance() != NULL)
		CContext::getInstance()->resize(w, h);
}

void SampleInstancing::onResume()
{
	// resume application
	CViewManager::getInstance()->onResume();
}

void SampleInstancing::onPause()
{
	// pause application
	CViewManager::getInstance()->onPause();
}

void SampleInstancing::onQuitApp()
{
	// end application
	delete this;
}