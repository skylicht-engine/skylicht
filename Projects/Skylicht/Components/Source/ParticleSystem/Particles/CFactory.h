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

namespace Skylicht
{
	namespace Particle
	{
		class CEmitter;
		class CZone;
		class IRenderer;

		enum EZone
		{
			Sphere,
			Point,
			AABox,
			Plane,
			Line,
			Ring,
			Cylinder
		};

		enum EEmitter
		{
			Random,
		};

		enum ERenderer
		{
			Quad,
			Mesh,
		};

		class CFactory
		{
		protected:
			std::vector<IRenderer*> m_renderers;
			std::vector<CZone*> m_zones;
			std::vector<CEmitter*> m_emitters;

		public:
			CFactory();

			virtual ~CFactory();

			CEmitter* createEmitter(EEmitter type = Random);

			void deleteEmitter(CEmitter *e);

			IRenderer* createRenderer(ERenderer type = Quad);

			void deleteRenderer(IRenderer* r);

			CZone* createZone(EZone type = Sphere);

			void deleteZone(CZone *z);
		};
	}
}