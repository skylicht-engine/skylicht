#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"
#include "Transform/CWorldTransformSystem.h"

#include "Primitive/CCube.h"
#include "SkySun/CSkySun.h"

#include "CBoldSystem.h"
#include "CBoldAnimationSystem.h"

#include "Material/Shader/Instancing/CTBNSGInstancing.h"

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL),
	m_downloaded(0),
	m_bakeSHLighting(true)
{

}

CViewInit::~CViewInit()
{

}

io::path CViewInit::getBuiltInPath(const char* name)
{
	return getApplication()->getBuiltInPath(name);
}

void CViewInit::onInit()
{
	CBaseApp* app = getApplication();
	app->showDebugConsole();
	app->getFileSystem()->addFileArchive(getBuiltInPath("BuiltIn.zip"), false, false);

	CShaderManager* shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();
	shaderMgr->initSGForwarderShader();
	shaderMgr->initSGDeferredShader();

	CGlyphFreetype* freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");

	// init basic gui
	CZone* zone = CContext::getInstance()->initScene()->createZone();
	m_guiObject = zone->createEmptyObject();
	CCanvas* canvas = m_guiObject->addComponent<CCanvas>();

	// load font
	m_font = new CGlyphFont();
	m_font->setFont("Segoe UI Light", 25);

	// create text
	m_textInfo = canvas->createText(m_font);
	m_textInfo->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	m_textInfo->setText(L"Init assets");

	// create gui camera
	CGameObject* guiCameraObject = zone->createEmptyObject();
	CCamera* guiCamera = guiCameraObject->addComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);
	CContext::getInstance()->setGUICamera(guiCamera);
}

void CViewInit::initScene()
{
	CBaseApp* app = getApplication();

	// create a scene
	CScene* scene = CContext::getInstance()->getScene();
	CZone* zone = scene->getZone(0);

	// gui camera
	CGameObject* guiCameraObject = zone->createEmptyObject();
	CCamera* guiCamera = guiCameraObject->addComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);

	// sky
	CSkySun* skySun = zone->createEmptyObject()->addComponent<CSkySun>();

	// reflection probe
	CGameObject* reflectionProbeObj = zone->createEmptyObject();
	CReflectionProbe* reflection = reflectionProbeObj->addComponent<CReflectionProbe>();
	reflection->loadStaticTexture("Common/Textures/Sky/PaperMill");

	// lighting
	CGameObject* lightObj = zone->createEmptyObject();
	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();
	SColor c(255, 255, 244, 214);
	directionalLight->setColor(SColorf(c));

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(4.0f, -6.0f, -4.5f);
	lightTransform->setOrientation(direction, Transform::Oy);


	// load toon instancing shader
	CShaderManager* shaderManager = CShaderManager::getInstance();

	shaderManager->loadShader("BuiltIn/Shader/ShadowDepthWrite/SDWSkinInstancing2.xml");
	shaderManager->loadShader("BuiltIn/Shader/Toon/ToonShadowInstancing.xml");
	shaderManager->loadShader("BuiltIn/Shader/Toon/ToonShadow.xml", new CTBNSGInstancing());

	float envMin = -25.0f;
	float envMax = 25.0f;

	// init environment
	initEnviroment(zone, envMin, envMax);

	// load animation
	CAnimationManager* animManager = CAnimationManager::getInstance();
	CAnimationClip* animIdle = animManager->loadAnimation("SampleBoids/RifleMan/A_RifleMan_Idle.fbx");
	CAnimationClip* animWalk = animManager->loadAnimation("SampleBoids/RifleMan/A_RifleMan_Walk.fbx");
	CAnimationClip* animRun = animManager->loadAnimation("SampleBoids/RifleMan/A_RifleMan_Running.fbx");

	std::vector<CAnimationClip*> clips;
	clips.push_back(animIdle);
	clips.push_back(animWalk);
	clips.push_back(animRun);

	// init crowd
	int fps = 60;
	initCrowdByVertexTexture(zone, envMin, envMax, clips, fps);

	// init bold system
	CEntityManager* entityManager = scene->getEntityManager();

	// add bold system (that will update moving for CBoldData)
	float wallDepth = 0.5f;
	CBoldSystem* boldSystem = entityManager->addSystem<CBoldSystem>();
	boldSystem->setBounds(
		envMin + wallDepth, // min x
		envMax - wallDepth, // max x
		envMin + wallDepth, // min z
		envMax - wallDepth, // max z
		2.5f // distance near wall, that bold will begin turn
	);

	// that let CBoldSystem will update before CWorldTransformSystem
	int worldTransformOrder = entityManager->getSystem<CWorldTransformSystem>()->getSystemOrder();
	boldSystem->setSystemOrder(worldTransformOrder - 1);
	entityManager->notifySystemOrderChanged();

	// add bold system (that will update animation clip)
	CBoldAnimationSystem* animSystem = entityManager->addSystem<CBoldAnimationSystem>();
	animSystem->addClip(animIdle, 0, fps, 0.0f);
	animSystem->addClip(animWalk, 1, fps, 0.01f);
	animSystem->addClip(animRun, 2, fps, 0.03f);

	// Rendering
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	CContext* context = CContext::getInstance();

	context->initShadowForwarderPipeline(w, h);
	context->setActiveZone(zone);
	context->setGUICamera(guiCamera);
	context->setDirectionalLight(directionalLight);

	bool use2Cascade = false;
	bool noCascade = false;

	if (use2Cascade)
	{
		// Test use 2 cascade shadow
		context->getShadowMapRenderPipeline()->setShadowCascade(2);
	}
	else if (noCascade)
	{
		// Test no shadow cascade (60m)
		context->getShadowMapRenderPipeline()->setNoShadowCascade(2048, 60.0f);
	}

	CPostProcessorRP* postProcess = context->getPostProcessorPipeline();
	postProcess->enableAutoExposure(false);
	postProcess->enableBloomEffect(false);
}

void CViewInit::initEnviroment(CZone* zone, float& envMin, float& envMax)
{
	int minBound = -2;
	int maxBound = 2;

	float space = 8.0f;
	float halfSpace = space * 0.5f;

	float scale = 0.16f;
	core::vector3df scaleVector(scale, scale, scale);

	envMin = minBound * space - halfSpace;
	envMax = maxBound * space + halfSpace;

	CMeshManager* meshManager = CMeshManager::getInstance();

	std::vector<std::string> searchTextureFolders;
	ArrayMaterial envMaterials = CMaterialManager::getInstance()->loadMaterial("SampleBoids/Environment/Environment.mat", true, searchTextureFolders);

	CEntityPrefab* groundPrefab = meshManager->loadModel("SampleBoids/Environment/1_Ground_1.fbx", NULL, true);
	if (groundPrefab)
	{
		CGameObject* plane = zone->createEmptyObject();
		CRenderMeshInstancing* meshInstancing = plane->addComponent<CRenderMeshInstancing>();
		meshInstancing->initFromPrefab(groundPrefab);
		meshInstancing->initMaterial(envMaterials);

		for (int i = minBound; i <= maxBound; i++)
		{
			for (int j = minBound; j <= maxBound; j++)
			{
				CEntity* entity = meshInstancing->spawn();

				CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				transform->Relative.setTranslation(core::vector3df(i * space, 0.0f, j * space));
				transform->Relative.setScale(scaleVector);
			}
		}

		meshInstancing->applyShareTransformBuffer();
		meshInstancing->applyShareMaterialBuffer();
	}

	std::vector<std::string> randomWallMesh;
	randomWallMesh.push_back("SampleBoids/Environment/1_Border_1.fbx");
	randomWallMesh.push_back("SampleBoids/Environment/1_Border_2.fbx");
	randomWallMesh.push_back("SampleBoids/Environment/1_Border_3.fbx");
	randomWallMesh.push_back("SampleBoids/Environment/1_Border_4.fbx");

	std::vector<CRenderMeshInstancing*> meshInstancings;

	for (std::string& s : randomWallMesh)
	{
		CEntityPrefab* prefab = meshManager->loadModel(s.c_str(), NULL, true);
		if (prefab)
		{
			CGameObject* border = zone->createEmptyObject();
			CRenderMeshInstancing* meshInstancing = border->addComponent<CRenderMeshInstancing>();
			meshInstancing->initFromPrefab(prefab);
			meshInstancing->initMaterial(envMaterials);

			meshInstancings.push_back(meshInstancing);
		}
	}

	core::matrix4 rot180, rot90, rot270, scaleMat;
	rot90.setRotationDegrees(core::vector3df(0.0f, 90.0f, 0.0f));
	rot180.setRotationDegrees(core::vector3df(0.0f, 180.0f, 0.0f));
	rot270.setRotationDegrees(core::vector3df(0.0f, 270, 0.0f));
	scaleMat.setScale(scaleVector);

	{
		CEntity* entity;
		CWorldTransformData* transform;

		int random = 0;
		int count = (int)meshInstancings.size();
		if (count > 0)
		{
			for (int i = minBound; i <= maxBound; i++)
			{
				random = os::Randomizer::rand() % count;
				entity = meshInstancings[random]->spawn();
				transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				transform->Relative.setTranslation(core::vector3df(i * space, 0.0f, -maxBound * space - halfSpace));
				transform->Relative.setScale(scaleVector);

				random = os::Randomizer::rand() % count;
				entity = meshInstancings[random]->spawn();
				transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				transform->Relative = rot180 * scaleMat;
				transform->Relative.setTranslation(core::vector3df(i * space, 0.0f, maxBound * space + halfSpace));

				random = os::Randomizer::rand() % count;
				entity = meshInstancings[random]->spawn();
				transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				transform->Relative = rot90 * scaleMat;
				transform->Relative.setTranslation(core::vector3df(-maxBound * space - halfSpace, 0.0f, i * space));

				random = os::Randomizer::rand() % count;
				entity = meshInstancings[random]->spawn();
				transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				transform->Relative = rot270 * scaleMat;
				transform->Relative.setTranslation(core::vector3df(maxBound * space + halfSpace, 0.0f, i * space));
			}
		}
	}

	{
		CEntityPrefab* prefab = meshManager->loadModel("SampleBoids/Environment/1_Border_Corner.fbx", NULL, true);
		if (prefab)
		{
			CGameObject* corner = zone->createEmptyObject();
			CRenderMeshInstancing* meshInstancing = corner->addComponent<CRenderMeshInstancing>();
			meshInstancing->initFromPrefab(prefab);
			meshInstancing->initMaterial(envMaterials);

			CEntity* entity;
			CWorldTransformData* transform;

			entity = meshInstancing->spawn();
			transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			transform->Relative.setTranslation(core::vector3df(-maxBound * space - halfSpace, 0.0f, -maxBound * space - halfSpace));
			transform->Relative.setScale(scaleVector);

			entity = meshInstancing->spawn();
			transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			transform->Relative = rot90 * scaleMat;
			transform->Relative.setTranslation(core::vector3df(-maxBound * space - halfSpace, 0.0f, maxBound * space + halfSpace));

			entity = meshInstancing->spawn();
			transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			transform->Relative = rot180 * scaleMat;
			transform->Relative.setTranslation(core::vector3df(maxBound * space + halfSpace, 0.0f, maxBound * space + halfSpace));

			entity = meshInstancing->spawn();
			transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			transform->Relative = rot270 * scaleMat;
			transform->Relative.setTranslation(core::vector3df(maxBound * space + halfSpace, 0.0f, -maxBound * space - halfSpace));
		}
	}
}

void CViewInit::initCrowdByVertexTexture(CZone* zone, float envMin, float envMax, std::vector<CAnimationClip*>& clips, int fps)
{
	// bake clip animation	
	float maxDuration = 0.0f;
	for (CAnimationClip* clip : clips)
	{
		if (clip->Duration > maxDuration)
			maxDuration = clip->Duration;
	}

	std::vector<std::string> searchTextureFolders;

	CMaterialManager* materialManager = CMaterialManager::getInstance();
	CMeshManager* meshManager = CMeshManager::getInstance();

	CEntityPrefab* modelPrefab1 = meshManager->loadModel("SampleBoids/RifleMan/RifleMan.fbx", NULL, true);
	CEntityPrefab* modelPrefab2 = meshManager->loadModel("SampleBoids/RifleMan/RifleMan_ELITE.fbx", NULL, true);

	if (modelPrefab1 != NULL && modelPrefab2 != NULL)
	{
		// create gpu anim character
		CGameObject* crowd1 = zone->createEmptyObject();
		crowd1->setName("Crowd1");

		initVATCrowd(crowd1, modelPrefab1, clips, fps);
	}
}

void CViewInit::initVATCrowd(CGameObject* crowd, CEntityPrefab* modelPrefab, std::vector<CAnimationClip*>& clips, int fps)
{
	CRenderMeshInstancingVAT* crowdMesh = crowd->addComponent<CRenderMeshInstancingVAT>();
	crowdMesh->initFromPrefab(modelPrefab);

	CAnimationController* animController = crowd->addComponent<CAnimationController>();
	animController->setEnable(false);

	CSkeleton* skeleton = animController->createSkeleton();

	float maxDuration = 0.0f;
	for (CAnimationClip* clip : clips)
	{
		if (clip->Duration > maxDuration)
			maxDuration = clip->Duration;
	}

	int clipId = 0;
	int totalFrames = (int)(maxDuration * fps);
	int numClip = (int)clips.size();

	std::map<std::string, int> boneMap;
	skeleton->getBoneIdMap(boneMap);
	int numBones = (int)boneMap.size();

	core::matrix4* transforms = new core::matrix4[numBones];

	for (CAnimationClip* clip : clips)
	{
		skeleton->setAnimation(clip, true);

		int clipFrames = (int)(clip->Duration * fps);
		int clipOffset = clipId * numBones * totalFrames;

		for (int i = 0; i < clipFrames; i++)
		{
			float t = i / (float)clipFrames;
			skeleton->simulateTransform(t * clip->Duration, core::IdentityMatrix, transforms, numBones);
			crowdMesh->updateSkinnedMesh();
		}

		clipId++;
	}

	delete[]transforms;

	// test spawn 1 entity
	crowdMesh->spawn();
}

void CViewInit::onDestroy()
{
	m_guiObject->remove();
	delete m_font;
}

void CViewInit::onUpdate()
{
	CContext* context = CContext::getInstance();

	switch (m_initState)
	{
	case CViewInit::DownloadBundles:
	{
		io::IFileSystem* fileSystem = getApplication()->getFileSystem();

		std::vector<std::string> listBundles;
		listBundles.push_back("SampleBoids.zip");
		listBundles.push_back("SampleBoidsResource.zip");

#ifdef __EMSCRIPTEN__
		const char* filename = listBundles[m_downloaded].c_str();

		if (m_getFile == NULL)
		{
			m_getFile = new CGetFileURL(filename, filename);
			m_getFile->download(CGetFileURL::Get);

			char log[512];
			sprintf(log, "Download asset: %s", filename);
			os::Printer::log(log);
		}
		else
		{
			char log[512];
			sprintf(log, "Download asset: %s - %d%%", filename, m_getFile->getPercent());
			m_textInfo->setText(log);

			if (m_getFile->getState() == CGetFileURL::Finish)
			{
				// [bundles].zip
				fileSystem->addFileArchive(filename, false, false);

				if (++m_downloaded >= listBundles.size())
					m_initState = CViewInit::InitScene;
				else
				{
					delete m_getFile;
					m_getFile = NULL;
				}
			}
			else if (m_getFile->getState() == CGetFileURL::Error)
			{
				// retry download
				delete m_getFile;
				m_getFile = NULL;
			}
		}
#else

		for (std::string& bundle : listBundles)
		{
			const char* r = bundle.c_str();
			fileSystem->addFileArchive(getBuiltInPath(r), false, false);
		}

		m_initState = CViewInit::InitScene;
#endif
	}
	break;
	case CViewInit::InitScene:
	{
		initScene();
		m_initState = CViewInit::Finished;
	}
	break;
	case CViewInit::Error:
	{
		// todo nothing with black screen
	}
	break;
	default:
	{
		CScene* scene = context->getScene();
		if (scene != NULL)
			scene->update();

		CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
	}
	break;
	}
}

void CViewInit::onRender()
{
	if (m_initState == CViewInit::Finished)
	{
		CContext* context = CContext::getInstance();
		CScene* scene = CContext::getInstance()->getScene();
		CBaseRP* rp = CContext::getInstance()->getRenderPipeline();

		if (m_bakeSHLighting == true)
		{
			m_bakeSHLighting = false;

			CZone* zone = scene->getZone(0);

			// light probe
			CGameObject* lightProbeObj = zone->createEmptyObject();
			CLightProbe* lightProbe = lightProbeObj->addComponent<CLightProbe>();
			lightProbeObj->getTransformEuler()->setPosition(core::vector3df(0.0f, 1.0f, 0.0f));

			CGameObject* bakeCameraObj = scene->getZone(0)->createEmptyObject();
			CCamera* bakeCamera = bakeCameraObj->addComponent<CCamera>();
			scene->updateAddRemoveObject();

			// bake light probe
			Lightmapper::CLightmapper* lm = Lightmapper::CLightmapper::getInstance();
			lm->initBaker(64);

			std::vector<CLightProbe*> probes;
			probes.push_back(lightProbe);

			lm->bakeProbes(probes, bakeCamera, rp, scene->getEntityManager());
		}
	}
	else
	{
		CCamera* guiCamera = CContext::getInstance()->getGUICamera();
		CGraphics2D::getInstance()->render(guiCamera);
	}
}
