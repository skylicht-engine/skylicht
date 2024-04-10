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

#include "pch.h"
#include "CSkinnedMeshRendererInstancing.h"
#include "SkinnedInstancing/CSkinnedInstanceData.h"

#include "Culling/CCullingData.h"
#include "Entity/CEntityManager.h"

#include "MeshManager/CMeshManager.h"

#include "Material/Shader/ShaderCallback/CShaderSH.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"
#include "Material/Shader/ShaderCallback/CShaderTransformTexture.h"

namespace Skylicht
{
	CSkinnedMeshRendererInstancing::CSkinnedMeshRendererInstancing() :
		m_group(NULL)
	{
		m_pipelineType = IRenderPipeline::Forwarder;
	}

	CSkinnedMeshRendererInstancing::~CSkinnedMeshRendererInstancing()
	{

	}

	void CSkinnedMeshRendererInstancing::beginQuery(CEntityManager* entityManager)
	{
		if (m_group == NULL)
		{
			const u32 skinnedInstance[] = GET_LIST_ENTITY_DATA(CSkinnedInstanceData);
			m_group = entityManager->createGroupFromVisible(skinnedInstance, 1);
		}
	}

	void CSkinnedMeshRendererInstancing::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		entities = m_group->getEntities();
		numEntity = m_group->getEntityCount();

		CMeshManager* meshManager = CMeshManager::getInstance();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CSkinnedInstanceData* skinnedIntance = GET_ENTITY_DATA(entity, CSkinnedInstanceData);

			// skip no transform textures
			if (skinnedIntance->TransformTextures.size() == 0)
				continue;

			for (u32 j = 0, n = skinnedIntance->RenderData.size(); j < n; j++)
			{
				CRenderMeshData* meshData = skinnedIntance->RenderData[j];

				// skip transform texture, that null
				if (skinnedIntance->TransformTextures[j] == NULL)
					continue;

				SMeshInstancing* data = meshData->getMeshInstancing();
				if (data)
				{
					SMeshInstancingGroup* group = m_groups[data];
					if (group == NULL)
					{
						group = new SMeshInstancingGroup();
						group->TransformTexture = skinnedIntance->TransformTextures[j];
						group->RenderMesh = meshData;

						m_groups[data] = group;
					}

					group->Entities.push(entity);
				}
			}
		}
	}

	void CSkinnedMeshRendererInstancing::init(CEntityManager* entityManager)
	{

	}

	void CSkinnedMeshRendererInstancing::update(CEntityManager* entityManager)
	{
		for (auto it : m_groups)
		{
			SMeshInstancing* data = it.first;
			SMeshInstancingGroup* group = it.second;

			u32 count = group->Entities.count();
			if (count == 0)
				continue;

			CMesh* mesh = group->RenderMesh->getMesh();

			for (u32 i = 0, n = data->RenderMeshBuffers.size(); i < n; i++)
			{
				group->Materials.reset();

				for (u32 j = 0; j < count; j++)
					group->Materials.push(mesh->Materials[i]);

				// batching transform & material data to buffer
				data->InstancingShader[i]->batchIntancing(
					data->InstancingBuffer[i],
					data->TransformBuffer[i],
					data->IndirectLightingBuffer[i],
					group->Materials.pointer(),
					group->Entities.pointer(),
					count
				);
			}
		}
	}

	void CSkinnedMeshRendererInstancing::render(CEntityManager* entityManager)
	{
		IVideoDriver* driver = getVideoDriver();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		CEntity** allEntities = entityManager->getEntities();

		for (auto& it : m_groups)
		{
			SMeshInstancing* data = it.first;
			SMeshInstancingGroup* group = it.second;

			int count = group->Entities.count();
			if (count == 0)
				continue;

			CMaterial** materials = group->Materials.pointer();

			CShaderTransformTexture::setTexture(group->TransformTexture);

			u32 numMeshBuffer = data->RenderMeshBuffers.size();
			for (u32 i = 0; i < numMeshBuffer; i++)
			{
				if (materials[i] == NULL)
					continue;

				CShader* shader = materials[i]->getShader();
				if (!rp->canRenderShader(shader))
					continue;

				// apply material
				CShaderMaterial::setMaterial(materials[i]);

				int materialType = shader->getMaterialRenderID();

				/*
				rp->drawInstancingMeshBuffer(
					(CMesh*)data->InstancingMesh,
					i,
					materialType,
					entityManager,
					false
				);
				*/
			}
		}

		/*
		IVideoDriver* driver = getVideoDriver();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		CEntity** allEntities = entityManager->getEntities();

		u32 numEntity = m_meshs.size();
		CRenderMeshData** meshDatas = m_meshs.pointer();
		CTransformTextureData** textureDatas = m_textures.pointer();

		for (u32 i = 0, n = m_meshs.size(); i < n; i++)
		{
			CRenderMeshData* meshData = meshDatas[i];
			CTransformTextureData* textureData = textureDatas[i];

			CEntity* entity = allEntities[meshData->EntityIndex];

			CMesh* mesh = meshData->getMesh();

			CIndirectLightingData* lightingData = GET_ENTITY_DATA(entity, CIndirectLightingData);
			if (lightingData != NULL)
			{
				if (lightingData->Type == CIndirectLightingData::SH9)
					CShaderSH::setSH9(lightingData->SH);
				else if (lightingData->Type == CIndirectLightingData::AmbientColor)
					CShaderLighting::setLightAmbient(lightingData->Color);
			}

			CShaderTransformTexture::setTexture(textureData->TransformTexture);

			CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			driver->setTransform(video::ETS_WORLD, transform->World);

			for (u32 j = 0, m = mesh->getMeshBufferCount(); j < m; j++)
				rp->drawMeshBuffer(mesh, j, entityManager, meshData->EntityIndex, false);
		}
		*/
	}
}