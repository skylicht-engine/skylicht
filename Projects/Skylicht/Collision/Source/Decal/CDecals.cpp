/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CDecals.h"
#include "CDecalsRenderer.h"
#include "CDecalData.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformData.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CDecals);

	CATEGORY_COMPONENT(CDecals, "Renderer", "Decals");

	CDecals::CDecals()
	{

	}

	CDecals::~CDecals()
	{

	}

	void CDecals::initComponent()
	{
		m_gameObject->getEntityManager()->addRenderSystem<CDecalsRenderer>();
	}

	void CDecals::updateComponent()
	{

	}

	CObjectSerializable* CDecals::createSerializable()
	{
		return CComponentSystem::createSerializable();
	}

	void CDecals::loadSerializable(CObjectSerializable* object)
	{
		return CComponentSystem::loadSerializable(object);
	}

	CEntity* CDecals::addDecal(
		const core::vector3df& position,
		const core::vector3df& dimension,
		const core::vector3df& normal,
		float textureRotation,
		float lifeTime,
		float distance)
	{
		// References
		// https://sourceforge.net/p/irrext/code/HEAD/tree/trunk/extensions/scene/ISceneNode/DecalSystem

		CEntity* entity = createEntity();

		CDecalData* decalData = entity->addData<CDecalData>();
		decalData->Dimension = dimension;
		decalData->Normal = normal;
		decalData->TextureRotation = textureRotation;
		decalData->LifeTime = lifeTime;
		decalData->Distance = distance;

		CWorldTransformData* transform = (CWorldTransformData*)entity->getDataByIndex(CWorldTransformData::DataTypeIndex);
		transform->Relative.setTranslation(position);

		return entity;
	}

	void CDecals::bake()
	{

	}
}