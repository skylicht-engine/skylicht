/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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

#pragma once

#include "Entity/IRenderSystem.h"
#include "CGizmosData.h"

namespace Skylicht
{
	namespace Editor
	{
		struct SLineArrow
		{
			core::vector3df Position;
			core::vector3df Vector;
			float Length;
			float ArrowSize;
			SColor Color;
		};

		struct SRectBillboard
		{
			core::vector3df Position;
			float Width;
			float Height;
			SColor Color;
		};

		class CGizmosRenderer : public IRenderSystem
		{
		protected:
			CGizmosData* m_data;

			bool m_enable;

			core::array<SLineArrow> m_arrows;
			core::array<SRectBillboard> m_rectBillboard;

		public:
			CGizmosRenderer();

			virtual ~CGizmosRenderer();

			virtual void init(CEntityManager* entityManager);

			virtual void beginQuery();

			virtual void onQuery(CEntityManager* entityManager, CEntity* entity);

			virtual void update(CEntityManager* entityManager);

			virtual void render(CEntityManager* entityManager);

			inline void setEnable(bool b)
			{
				m_enable = b;
			}

			inline bool isEnable()
			{
				return m_enable;
			}

			inline CLineDrawData* getLineData()
			{
				return (CLineDrawData*)m_data;
			}

			inline CPolygonDrawData* getPolyData()
			{
				return (CPolygonDrawData*)m_data;
			}

			void drawRectBillboard(const core::vector3df& pos, float width, float height, const SColor& color);

			void drawArrowInViewSpace(const core::vector3df& pos, const core::vector3df& v, float length, float arrowSize, const SColor& color);
		};
	}
}