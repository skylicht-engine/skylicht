/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
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

#pragma once

#include "pch.h"
#include "CParticleBufferData.h"
#include "CParticleRenderer.h"
#include "CParticleComponent.h"

#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	namespace Particle
	{
		CParticleComponent::CParticleComponent() :
			m_data(NULL)
		{

		}

		CParticleComponent::~CParticleComponent()
		{

		}

		void CParticleComponent::initComponent()
		{
			// add particle buffer data
			m_data = m_gameObject->getEntity()->addData<CParticleBufferData>();

			// add renderer system
			m_gameObject->getEntityManager()->addRenderSystem<CParticleRenderer>();
		}

		void CParticleComponent::updateComponent()
		{
			for (u32 i = 0, n = m_data->Groups.size(); i < n; i++)
			{
				m_data->Groups[i]->update();
			}
		}

		CGroup* CParticleComponent::createParticleGroup()
		{
			return m_data->createGroup();
		}

		void CParticleComponent::removeParticleGroup(CGroup* group)
		{
			m_data->removeGroup(group);
		}
	}
}