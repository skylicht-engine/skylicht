/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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

#include "CBaseRP.h"

namespace Skylicht
{
	class CDeferredRP : public CBaseRP
	{
	protected:
		static bool s_enableRenderIndirect;
		static bool s_enableRenderTestIndirect;

	protected:
		ITexture *m_target;

		ITexture *m_albedo;
		ITexture *m_position;
		ITexture *m_normal;
		ITexture *m_data;
		ITexture *m_indirect;

		ITexture *m_lightBuffer;

		core::dimension2du m_size;

		core::array<irr::video::IRenderTarget> m_multiRenderTarget;

		core::matrix4 m_viewMatrix;
		core::matrix4 m_projectionMatrix;

		bool m_isIndirectPass;
		int m_vertexColorShader;
		int m_textureColorShader;

		SMaterial m_pointLightPass;
		SMaterial m_directionalLightPass;
		SMaterial m_finalPass;

		int m_pointLightShader;
		int m_pointLightShadowShader;

		float m_indirectMultipler;
		float m_directMultipler;
	protected:

		void initDefferredMaterial();
		void initPointLightMaterial();

	public:
		CDeferredRP();

		virtual ~CDeferredRP();

		virtual bool canRenderMaterial(CMaterial *material);

		virtual void initRender(int w, int h);

		virtual void render(ITexture *target, CCamera *camera, CEntityManager *entityManager, const core::recti& viewport);

		virtual void drawMeshBuffer(CMesh *mesh, int bufferID, CEntityManager* entity, int entityID);

		inline void setIndirectMultipler(float f)
		{
			m_indirectMultipler = f;
		}

		inline void setDirectMultipler(float f)
		{
			m_directMultipler = f;
		}

		static void enableRenderIndirect(bool b);

		static bool isEnableRenderIndirect();

		static void enableTestIndirect(bool b);
	};
}