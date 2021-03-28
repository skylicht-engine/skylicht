/*
!@
MIT License

CopyRight (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CSpaceAssets.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceAssets::CSpaceAssets(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor)
		{
			GUI::CToolbar* toolbar = new GUI::CToolbar(window);

			m_btnAdd = toolbar->addButton(L"Add", GUI::ESystemIcon::Plus);

			m_inputSearch = new GUI::CTextBox(toolbar);
			m_inputSearch->setWidth(200.0f);
			m_inputSearch->showIcon();
			m_inputSearch->setStringHint(L"Search");
			toolbar->addControl(m_inputSearch, true);

			GUI::CSplitter* spliter = new GUI::CSplitter(window);
			spliter->dock(GUI::EPosition::Fill);
			spliter->setNumberRowCol(1, 2);

			m_treeFS = new GUI::CTreeControl(spliter);
			m_treeFSController = new CTreeFSController(window->getCanvas(), m_treeFS);

			m_listFS = new GUI::CListBox(spliter);
			m_listFSController = new CListFSController(window->getCanvas(), m_listFS);

			m_listFSController->setTreeController(m_treeFSController);
			m_treeFSController->setListController(m_listFSController);

			spliter->setControl(m_treeFS, 0, 0);
			spliter->setControl(m_listFS, 0, 1);

			spliter->setColWidth(0, 300.0f);
			spliter->setWeakCol(1);

			m_contextMenuFS = new CContextMenuFS(window->getCanvas(), m_treeFS, m_treeFSController, m_listFS, m_listFSController);
			m_contextMenuAdd = new CContextMenuAdd(window->getCanvas(), m_listFSController);

			m_btnAdd->OnDown = [contextMenu = m_contextMenuAdd](GUI::CBase* sender)
			{
				contextMenu->popupMenu((GUI::CButton*)sender);
			};
		}

		CSpaceAssets::~CSpaceAssets()
		{
			delete m_treeFSController;
			delete m_listFSController;
			delete m_contextMenuFS;
			delete m_contextMenuAdd;
		}

		void CSpaceAssets::refresh()
		{
			m_listFSController->refresh();
			m_treeFSController->refresh();
		}
	}
}