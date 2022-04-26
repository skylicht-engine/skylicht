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

#include "pch.h"
#include "CHistory.h"

namespace Skylicht
{
	namespace Editor
	{
		CHistory::CHistory()
		{

		}

		CHistory::~CHistory()
		{
			clearHistory();
		}

		void CHistory::clearHistory()
		{
			for (SHistoryData* history : m_history)
			{
				for (CObjectSerializable* data : history->Data)
					delete data;
				for (CObjectSerializable* data : history->DataModified)
					delete data;
				delete history;
			}
			m_history.clear();
		}

		void CHistory::addHistory(EHistory history, const std::string& containerID, std::vector<CObjectSerializable*>& dataModified, std::vector<CObjectSerializable*>& data)
		{
			SHistoryData* historyData = new SHistoryData();
			historyData->History = history;
			historyData->ContainerID = containerID;
			historyData->DataModified = dataModified;
			historyData->Data = data;
			m_history.push_back(historyData);
		}

		void CHistory::addHistory(EHistory history, const std::string& containerID, CObjectSerializable* dataModified, CObjectSerializable* data)
		{
			SHistoryData* historyData = new SHistoryData();
			historyData->History = history;
			historyData->ContainerID = containerID;
			historyData->DataModified.push_back(dataModified);
			historyData->Data.push_back(data);
			m_history.push_back(historyData);
		}
	}
}