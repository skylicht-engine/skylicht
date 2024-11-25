/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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

#include "Entity/CEntityPrefab.h"
#include "RenderMesh/CRenderMeshData.h"
#include "ObstacleAvoidance/CObstacleAvoidance.h"

namespace Skylicht
{
	namespace Graph
	{
		struct STile
		{
			int Id;
			int X;
			int Y;
			int Z;
			core::vector3df Position;
			core::array<SObstacleSegment> Cols;
			core::array<core::triangle3df> Tris;

			STile* Neighbour[8];

			STile()
			{
				Id = 0;
				X = 0;
				Y = 0;
				Z = 0;
				for (int i = 0; i < 8; i++)
					Neighbour[i] = NULL;
			}
		};

		class CTileMap
		{
		protected:
			core::array<STile*> m_tiles;
			float m_tileWidth;
			float m_tileHeight;

		public:
			CTileMap();

			virtual ~CTileMap();

			void generate(float tileWidth, float tileHeight, const core::aabbox3df& bbox);

			void release();

			inline float getTileWidth()
			{
				return m_tileWidth;
			}

			inline float getTileHeight()
			{
				return m_tileHeight;
			}

			inline core::array<STile*>& getTiles()
			{
				return m_tiles;
			}
		};
	}
}