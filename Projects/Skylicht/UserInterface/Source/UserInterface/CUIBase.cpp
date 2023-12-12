/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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
#include "CUIBase.h"

namespace Skylicht
{
	namespace UI
	{
		CUIBase::CUIBase(CUIContainer* container, CGUIElement* element) :
			m_container(container),
			m_element(element)
		{

		}

		CUIBase::~CUIBase()
		{

		}

		core::vector3df* CUIBase::getRectTransform()
		{
			// get current canvas
			CCanvas* canvas = m_element->getCanvas();

			// get last camera, that render this button
			CCamera* camera = canvas->getRenderCamera();
			if (camera == NULL)
				return false;

			const core::matrix4& world = canvas->getRenderWorldTransform();
			const core::matrix4& elementTranform = m_element->getAbsoluteTransform();

			// real world transform
			core::matrix4 worldElementTransform = world * elementTranform;

			core::rectf r = m_element->getRect();

			m_rectTransform[0].set(r.UpperLeftCorner.X, r.UpperLeftCorner.Y, 0.0f);
			m_rectTransform[1].set(r.LowerRightCorner.X, r.UpperLeftCorner.Y, 0.0f);
			m_rectTransform[2].set(r.UpperLeftCorner.X, r.LowerRightCorner.Y, 0.0f);
			m_rectTransform[3].set(r.LowerRightCorner.X, r.LowerRightCorner.Y, 0.0f);

			for (int i = 0; i < 4; i++)
			{
				// get real 3d position
				worldElementTransform.transformVect(m_rectTransform[i]);
			}

			return m_rectTransform;
		}

		void CUIBase::onPointerHover(float pointerX, float pointerY)
		{
			if (OnPointerHover != nullptr)
				OnPointerHover(pointerX, pointerY);
		}

		void CUIBase::onPointerOut(float pointerX, float pointerY)
		{
			if (OnPointerOut != nullptr)
				OnPointerOut(pointerX, pointerY);
		}

		void CUIBase::onPointerDown(float pointerX, float pointerY)
		{
			if (OnPointerDown != nullptr)
				OnPointerDown(pointerX, pointerY);
		}

		void CUIBase::onPointerUp(float pointerX, float pointerY)
		{
			if (OnPointerUp != nullptr)
				OnPointerUp(pointerX, pointerY);
		}

		void CUIBase::onPressed()
		{
			if (OnPressed != nullptr)
				OnPressed();
		}
	}
}