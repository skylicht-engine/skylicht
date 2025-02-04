/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
#include "CGCapsule.h"

#include "Primitive/CCapsule.h"

#include "Transform/CWorldInverseTransformData.h"

namespace Skylicht
{
	namespace Editor
	{
		ACTIVATOR_REGISTER(CGCapsule);

		DEPENDENT_COMPONENT(CCapsule, CGCapsule);

		CGCapsule::CGCapsule() :
			m_capsule(NULL)
		{

		}

		CGCapsule::~CGCapsule()
		{

		}

		void CGCapsule::initComponent()
		{
			m_capsule = m_gameObject->getComponent<CCapsule>();
		}

		void CGCapsule::updateComponent()
		{
			updateSelectBBox();
		}

		void CGCapsule::updateSelectBBox()
		{
			CEntity* entity = m_gameObject->getEntity();

			CSelectObjectData* selectObjectData = GET_ENTITY_DATA(entity, CSelectObjectData);
			if (selectObjectData == NULL)
				selectObjectData = entity->addData<CSelectObjectData>();

			CWorldInverseTransformData* worldInv = GET_ENTITY_DATA(entity, CWorldInverseTransformData);
			if (worldInv == NULL)
				worldInv = entity->addData<CWorldInverseTransformData>();

			// select bbox data
			selectObjectData->GameObject = m_gameObject;
			selectObjectData->Entity = NULL;

			float height = m_capsule->getHeight();
			float radius = m_capsule->getRadius();

			float h = height * 0.5f + radius;

			selectObjectData->BBox.MinEdge = core::vector3df(-radius, -h, -radius);
			selectObjectData->BBox.MaxEdge = core::vector3df(radius, h, radius);
		}
	}
}