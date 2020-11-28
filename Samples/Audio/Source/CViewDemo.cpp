#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"

CViewDemo::CViewDemo()
{
	// add to support read resource on ZIP Bundle
	m_streamFactory = new CZipAudioStreamFactory();
	SkylichtAudio::CAudioEngine::getSoundEngine()->registerStreamFactory(m_streamFactory);
}

CViewDemo::~CViewDemo()
{
	SkylichtAudio::CAudioEngine::getSoundEngine()->unRegisterStreamFactory(m_streamFactory);
	delete m_streamFactory;
}

void CViewDemo::onInit()
{
	CContext *context = CContext::getInstance();
	CCamera *camera = context->getActiveCamera();

	os::Printer::log("Init sound emitter");

	// test sound
	SkylichtAudio::CAudioEngine *engine = SkylichtAudio::CAudioEngine::getSoundEngine();
	SkylichtAudio::CAudioEmitter *emitter = engine->createAudioEmitter("SampleAudio/file_example_MP3_5MG.mp3", false);
	if (emitter != NULL)
	{
		emitter->setLoop(true);
		emitter->play();
	}
	else
	{
		os::Printer::log("Init sound emitter FAILED!");
	}
}

void CViewDemo::onDestroy()
{

}

void CViewDemo::onUpdate()
{
	CContext *context = CContext::getInstance();
	CScene *scene = context->getScene();
	if (scene != NULL)
		scene->update();
}

void CViewDemo::onRender()
{
	CContext *context = CContext::getInstance();

	CCamera *camera = context->getActiveCamera();
	CCamera *guiCamera = context->getGUICamera();

	CScene *scene = context->getScene();

	// render scene
	if (camera != NULL && scene != NULL)
	{
		context->updateDirectionLight();

		context->getRenderPipeline()->render(NULL, camera, scene->getEntityManager(), core::recti());
	}

	// render GUI
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}
}

void CViewDemo::onPostRender()
{

}
