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

#ifndef _TOUCHMANAGER_H_
#define _TOUCHMANAGER_H_

#include "Thread/IMutex.h"
#include "Utils/CGameSingleton.h"
#include "CTouchIdentify.h"

namespace Skylicht
{

	class CTouchManager : public CGameSingleton<CTouchManager>
	{
	public:
		enum ETouchEvent
		{
			TouchNone = 0,
			TouchDown,
			TouchMove,
			TouchUp,
		};

		struct STouchStatus
		{
			ETouchEvent	touchEvent;
			int			x;
			int			y;
			long		touchID;
		};

	protected:
		core::array<STouchStatus>	m_touchQueue;

		STouchStatus	m_touch[MAX_MULTITOUCH];
		CTouchIdentify	m_touchIdentify;

		SkylichtSystem::IMutex	*m_mutex;

	public:
		CTouchManager();
		virtual ~CTouchManager();

		void resetTouch();

		void update();

		void touchEvent(ETouchEvent touchEvent, int x, int y, long id);

		inline CTouchIdentify* getTouchIdentify()
		{
			return &m_touchIdentify;
		}

		inline void setTouchIdentify(int touchID, CTouchIdentify::ETouchIdentify identify, void *data = NULL)
		{
			m_touchIdentify.setTouchIdentify(touchID, identify, data);
		}

		inline CTouchIdentify::ETouchIdentify getTouchIdentify(int touchID)
		{
			return m_touchIdentify.getTouchIdentify(touchID);
		}

		void updateTouch(ETouchEvent touchEvent, int x, int y, long id);

		void updateEvent();

	};

}

#endif