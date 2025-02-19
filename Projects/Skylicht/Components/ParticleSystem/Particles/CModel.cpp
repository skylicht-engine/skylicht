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

#include "pch.h"
#include "CModel.h"
#include "ParticleSystem/Particles/Zones/CZone.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Particle
	{
		const wchar_t* g_modelName[] = {
			L"Scale",
			L"ScaleX",
			L"ScaleY",
			L"ScaleZ",
			L"RotateX",
			L"RotateY",
			L"RotateZ",
			L"ColorR",
			L"ColorG",
			L"ColorB",
			L"ColorA",
			L"Mass",
			L"FrameIndex",
			L"RotateSpeedX",
			L"RotateSpeedY",
			L"RotateSpeedZ",
			L"NumParams"
		};

		CModel::CModel(EParticleParams type) :
			m_type(type),
			m_start1(0),
			m_start2(0),
			m_end1(0),
			m_end2(0),
			m_haveStart(false),
			m_haveEnd(false),
			m_interpolator(NULL)
		{

		}

		CModel::~CModel()
		{
		}

		CObjectSerializable* CModel::createSerializable()
		{
			CObjectSerializable* object = CParticleSerializable::createSerializable();

			CStringProperty* name = new CStringProperty(object, "name", CStringImp::convertUnicodeToUTF8(getName()).c_str());
			name->setHidden(true);
			object->autoRelease(name);

			CFloatProperty* randomStart = new CFloatProperty(object, "start1", m_start1);
			randomStart->setUIHeader("Random start");
			object->autoRelease(randomStart);
			object->autoRelease(new CFloatProperty(object, "start2", m_start2));

			CFloatProperty* randomEnd = new CFloatProperty(object, "end1", m_end1);
			randomEnd->setUIHeader("Random end");
			object->autoRelease(randomEnd);
			object->autoRelease(new CFloatProperty(object, "end2", m_end2));

			return object;
		}

		void CModel::loadSerializable(CObjectSerializable* object)
		{
			CParticleSerializable::loadSerializable(object);

			m_start1 = object->get<float>("start1", 0.0f);
			m_start2 = object->get<float>("start2", 0.0f);
			if (m_start1 != 0 || m_start2 != 0)
				m_haveStart = true;

			m_end1 = object->get<float>("end1", 0.0f);
			m_end2 = object->get<float>("end2", 0.0f);
			if (m_end1 != 0 || m_end2 != 0)
				m_haveEnd = true;
		}

		const wchar_t* CModel::getName()
		{
			return g_modelName[(int)m_type];
		}

		float CModel::getRandomStart()
		{
			return random(m_start1, m_start2);
		}

		float CModel::getRandomEnd()
		{
			return random(m_end1, m_end2);
		}
	}
}