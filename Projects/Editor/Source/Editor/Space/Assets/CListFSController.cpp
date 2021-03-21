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
#include "CListFSController.h"
#include "CTreeFSController.h"
#include "Utils/CPath.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Editor
	{
		CListFSController::CListFSController(GUI::CCanvas* canvas, GUI::CListBox* list) :
			m_renameItem(NULL),
			m_treeController(NULL),
			m_canvas(canvas),
			m_msgBox(NULL)
		{
			m_assetManager = CAssetManager::getInstance();

			m_listFS = list;
			m_listFS->OnKeyPress = std::bind(
				&CListFSController::OnKeyPress,
				this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3
			);

			std::vector<SFileInfo> files;
			m_assetManager->getRoot(files);
			add("", files);
		}

		CListFSController::~CListFSController()
		{

		}

		void CListFSController::OnKeyPress(GUI::CBase* control, int key, bool press)
		{
			GUI::CListBox* list = dynamic_cast<GUI::CListBox*>(control);
			if (list == NULL)
				return;

			if (key == GUI::KEY_F2)
			{
				rename(list->getSelected());
			}
		}

		void CListFSController::rename(GUI::CListRowItem* node)
		{
			if (node != NULL)
			{
				m_renameItem = node;
				m_renameRevert = node->getLabel();

				node->getTextEditHelper()->beginEdit(
					BIND_LISTENER(&CListFSController::OnRename, this),
					BIND_LISTENER(&CListFSController::OnCancelRename, this)
				);
			}
		}

		void CListFSController::removePath(const char* path)
		{
			std::list<GUI::CListRowItem*> items = m_listFS->getAllItems();
			for (GUI::CListRowItem* item : items)
			{
				const std::string& tagPath = item->getTagString();
				if (tagPath == path)
				{
					item->remove();
					return;
				}
			}
		}

		void CListFSController::scrollAndSelectPath(const char* path)
		{
			std::list<GUI::CListRowItem*> items = m_listFS->getAllItems();
			for (GUI::CListRowItem* item : items)
			{
				const std::string& tagPath = item->getTagString();
				if (tagPath == path)
				{
					item->setToggle(true);

					m_listFS->invalidate();
					m_listFS->recurseLayout();
					m_listFS->scrollToItem(item);
					return;
				}
			}
		}

		void CListFSController::OnRename(GUI::CBase* control)
		{
			GUI::CTextBox* textbox = dynamic_cast<GUI::CTextBox*>(control);

			std::wstring newNameW = textbox->getString();
			std::string newName = CStringImp::convertUnicodeToUTF8(newNameW.c_str());

			const std::string& path = m_renameItem->getTagString();
			std::string newPath = CPath::getFolderPath(path);
			newPath += "/";
			newPath += newName;

			if (m_assetManager->isExist(newPath.c_str()))
			{
				m_renameItem->setLabel(m_renameRevert);
				m_msgBox = new GUI::CMessageBox(m_canvas, GUI::CMessageBox::OK);
				m_msgBox->setMessage("File or folder with the new name already exists!", newName.c_str());
				m_msgBox->getMessageIcon()->setIcon(GUI::ESystemIcon::Alert);
				return;
			}

			if (m_assetManager->renameAsset(path.c_str(), newName.c_str()))
			{
				refresh();
				scrollAndSelectPath(newPath.c_str());
			}

			m_listFS->focus();
		}

		void CListFSController::OnCancelRename(GUI::CBase* control)
		{
			m_listFS->focus();
		}

		void CListFSController::add(const std::string& currentFolder, std::vector<SFileInfo>& files)
		{
			m_listFS->removeAllItem();

			if (currentFolder.size() > 0 &&
				currentFolder != m_assetManager->getAssetFolder())
			{
				GUI::CListRowItem* item = m_listFS->addItem(L"..", GUI::ESystemIcon::Folder);

				std::string parent = CPath::getFolderPath(currentFolder);
				item->tagString(parent);
				item->tagBool(true);
				item->OnDoubleLeftMouseClick = BIND_LISTENER(&CListFSController::OnFileOpen, this);
			}

			for (SFileInfo& f : files)
			{
				GUI::CListRowItem* item;

				if (f.IsFolder)
					item = m_listFS->addItem(f.NameW.c_str(), GUI::ESystemIcon::Folder);
				else
					item = m_listFS->addItem(f.NameW.c_str(), GUI::ESystemIcon::File);

				item->tagString(f.FullPath);
				item->tagBool(f.IsFolder);
				item->OnDoubleLeftMouseClick = BIND_LISTENER(&CListFSController::OnFileOpen, this);
			}

			m_listFS->setScrollVertical(0.0f);

			m_currentFolder = currentFolder;
		}

		void CListFSController::OnFileOpen(GUI::CBase* node)
		{
			GUI::CListRowItem* rowNode = dynamic_cast<GUI::CListRowItem*>(node);
			if (rowNode != NULL)
			{
				const std::string& fullPath = rowNode->getTagString();

				bool isFolder = rowNode->getTagBool();
				if (isFolder == true)
				{
					// browse folder
					std::vector<SFileInfo> files;
					m_assetManager->getFolder(fullPath.c_str(), files);

					add(fullPath, files);

					if (m_treeController != NULL)
						m_treeController->expand(fullPath);
				}
				else
				{
					// shell open the file
#if WIN32
					char path[512] = { 0 };
					CStringImp::replaceText(path, fullPath.c_str(), "/", "\\");
					ShellExecuteA(NULL, "open", path, NULL, NULL, SW_SHOWDEFAULT);
#endif
				}
			}
		}

		void CListFSController::refresh()
		{
			std::vector<SFileInfo> files;

			if (m_currentFolder.empty())
				m_assetManager->getRoot(files);
			else
				m_assetManager->getFolder(m_currentFolder.c_str(), files);

			add(m_currentFolder, files);
		}
	}
}