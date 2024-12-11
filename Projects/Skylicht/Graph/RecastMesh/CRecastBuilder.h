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

#include "CRecastMesh.h"
#include "ObstacleAvoidance/CObstacleAvoidance.h"
#include "RenderMesh/CMesh.h"

namespace Skylicht
{
	namespace Graph
	{
		struct SBuilderConfig
		{
			float CellSize;
			float CellHeight;
			float AgentHeight;
			float AgentRadius;
			float AgentMaxClimb;
			float AgentMaxSlope;
			float RegionMinSize;
			float RegionMergeSize;
			float EdgeMaxLen;
			float EdgeMaxError;
			int VertsPerPoly;
			float DetailSampleDist;
			float DetailSampleMaxError;

			SBuilderConfig()
			{
				CellSize = 0.3f;
				CellHeight = 0.2f;
				AgentHeight = 2.0f;
				AgentRadius = 0.6f;
				AgentMaxClimb = 0.9f;
				AgentMaxSlope = 45.0f;
				RegionMinSize = 8;
				RegionMergeSize = 20;
				EdgeMaxLen = 12.0f;
				EdgeMaxError = 1.3f;
				VertsPerPoly = 6;
				DetailSampleDist = 6.0f;
				DetailSampleMaxError = 1.0f;
			}
		};

		class CRecastBuilder
		{
		protected:
			SBuilderConfig m_config;

		public:
			CRecastBuilder();

			virtual ~CRecastBuilder();

			bool build(CRecastMesh* mesh, CMesh* output, CObstacleAvoidance* obstacle);

			bool load(CEntityPrefab* prefab, const core::matrix4& transform, CMesh* output, CObstacleAvoidance* obstacle);

			inline const SBuilderConfig& getConfig()
			{
				return m_config;
			}

			inline void setConfig(const SBuilderConfig& config)
			{
				m_config = config;
			}

		protected:

			void addMesh(CMesh* inputMesh, const core::matrix4& transform, CMesh* output);

			void loadObstacle(CMesh* navMesh, CObstacleAvoidance* obstacle);
		};
	}
}