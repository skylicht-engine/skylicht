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

#include "pch.h"
#include "CShaderCamera.h"
#include "Camera/CCamera.h"
#include "GameObject/CGameObject.h"

namespace Skylicht
{
	CCamera* CShaderCamera::s_camera = NULL;

	void CShaderCamera::setCamera(CCamera* camera)
	{
		s_camera = camera;
	}

	CShaderCamera::CShaderCamera()
	{

	}

	CShaderCamera::~CShaderCamera()
	{

	}

	void CShaderCamera::OnSetConstants(CShader* shader, SUniform* uniform, IMaterialRenderer* matRender, bool vertexShader)
	{
		switch (uniform->Type)
		{
		case WORLD_CAMERA_POSITION:
		{
			core::vector3df position;
			if (s_camera != NULL)
				position = s_camera->getGameObject()->getPosition();
			shader->setWorldPosition(matRender, uniform->UniformShaderID, position, vertexShader);
		}
		break;
		case CAMERA_POSITION:
		{
			core::vector3df position;
			if (s_camera != NULL)
				position = s_camera->getGameObject()->getPosition();
			shader->setPosition(matRender, uniform->UniformShaderID, position, vertexShader);
		}
		break;
		default:
			break;
		}
	}
}