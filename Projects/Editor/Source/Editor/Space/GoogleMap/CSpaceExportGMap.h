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

#include "SkylichtEngine.h"

#include "Editor/Space/CSpaceLoading.h"
#include "AssetManager/CAssetImporter.h"

#include "CDownloadGMapThread.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpaceExportGMap : public CSpaceLoading
		{
		protected:
			enum EImportState
			{
				None = 0,
				Init,
				Draw,
				Write,
				Finish,
			};

		protected:
			GUI::CProgressBar* m_progressBar;

			GUI::CLabel* m_statusText;

			EImportState m_state;

			CDownloadGMapThread* m_downloadThread;

			long m_x;
			long m_y;
			int m_z;
			int m_type;

			long m_x1;
			long m_y1;
			long m_x2;
			long m_y2;
			int m_gridSize;
			long m_drawElement;
			int m_retryDownload;

			std::string m_path;

			IImage* m_image;
		public:
			CSpaceExportGMap(GUI::CWindow* window, CEditor* editor);

			virtual ~CSpaceExportGMap();

			virtual void update();

			virtual void onDestroy(GUI::CBase* base);

			void exportMap(const char* path, long x1, long y1, long x2, long y2, int zoom, int type, int gridSize);
		};
	}
}