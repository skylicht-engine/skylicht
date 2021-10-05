/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "CEditor.h"
#include "CWindowConfig.h"
#include "Utils/CStringImp.h"

#include "Space/Import/CSpaceImport.h"
#include "Space/Scene/CSpaceScene.h"
#include "Space/Assets/CSpaceAssets.h"
#include "Space/Console/CSpaceConsole.h"
#include "Space/Property/CSpaceProperty.h"
#include "Space/Hierarchy/CSpaceHierarchy.h"
#include "Space/LoadScene/CSpaceLoadScene.h"
#include "Space/ProjectSettings/CSpaceProjectSettings.h"

#include "SpaceController/CSceneController.h"
#include "SpaceController/CPropertyController.h"
#include "SpaceController/CCollisionController.h"

#include "AssetManager/CAssetManager.h"
#include "Selection/CSelection.h"
#include "Activator/CEditorActivator.h"
#include "ProjectSettings/CProjectSettings.h"

namespace Skylicht
{
	namespace Editor
	{
		CEditor::CEditor() :
			m_menuBar(NULL),
			m_canvas(NULL),
			m_dockPanel(NULL),
			m_statusInfo(NULL),
			m_status(NULL),
			m_importDialog(NULL),
			m_loadSceneDialog(NULL),
			m_uiInitiate(false),
			m_confirmQuit(false)
		{
			// init canvas
			m_canvas = GUI::CGUIContext::getRoot();

			// file watcher
			m_assetWatcher = new CAssetWatcher();

			// scene icon
			m_spriteIcon = new CSpriteAtlas(video::ECF_A8R8G8B8, 256, 256);
			m_spriteIcon->addFrame("light", "Editor/Icon/Objects/Light.png");
			m_spriteIcon->addFrame("camera", "Editor/Icon/Objects/Camera.png");
			m_spriteIcon->updateTexture();

			// init controller
			CCollisionController::createGetInstance();
			CSceneController::createGetInstance()->initContextMenu(m_canvas);
			CPropertyController::createGetInstance();
			CSelection::createGetInstance();
			CEditorActivator::createGetInstance();
			CProjectSettings::createGetInstance();
		}

		CEditor::~CEditor()
		{
			CEditorActivator::releaseInstance();
			CPropertyController::releaseInstance();
			CSceneController::releaseInstance();
			CCollisionController::releaseInstance();
			CSelection::releaseInstance();
			CProjectSettings::releaseInstance();

			delete m_spriteIcon;
			delete m_assetWatcher;
		}

		void CEditor::update()
		{
			m_assetWatcher->update();

			Editor::CAssetManager::getInstance()->update();

			for (CSpace* s : m_workspaces)
				s->update();

			if (m_loadSceneDialog != NULL)
			{
				CSpace* space = getWorkspace(m_loadSceneDialog);
				if (space != NULL)
				{
					CSpaceLoadScene* spaceLoadScene = dynamic_cast<CSpaceLoadScene*>(space);
					if (spaceLoadScene != NULL && spaceLoadScene->isFinish())
					{
						closeLoadSceneDialog();
					}
				}
			}
		}

		void CEditor::pause()
		{
			m_assetWatcher->beginWatch();

			for (CSpace* s : m_workspaces)
			{
				s->pause();
			}
		}

		void CEditor::resume()
		{
			m_assetWatcher->endWatch();

			if (m_assetWatcher->needReImport())
			{
				m_assetWatcher->lock();
				initImportGUI(true);
			}

			for (CSpace* s : m_workspaces)
			{
				s->resume();
			}
		}

		void CEditor::refresh()
		{
			for (CSpace* s : m_workspaces)
			{
				s->refresh();
			}

			m_canvas->invalidate();
			m_canvas->recurseLayout();
		}

		bool CEditor::needReImport()
		{
			return m_assetWatcher->needReImport();
		}

		bool CEditor::isImportFinish()
		{
			CSpace* space = getWorkspace(m_importDialog);
			if (space != NULL)
			{
				CSpaceImport* spaceImport = dynamic_cast<CSpaceImport*>(space);
				if (spaceImport != NULL)
					return spaceImport->isFinish();
			}

			return false;
		}

		void CEditor::closeImportDialog()
		{
			m_importDialog->remove();
			m_importDialog = NULL;

			if (m_assetWatcher->needReImport())
				m_assetWatcher->unlock();
		}

		void CEditor::closeLoadSceneDialog()
		{
			m_loadSceneDialog->remove();
			m_loadSceneDialog = NULL;

			refresh();
		}

		void CEditor::initImportGUI(bool fromWatcher)
		{
			m_importDialog = new GUI::CDialogWindow(m_canvas, 0.0f, 0.0f, 600.0f, 120.0f);
			m_importDialog->setCaption(L"Import Assets");
			m_importDialog->showCloseButton(false);
			m_importDialog->setCenterPosition();
			m_importDialog->bringToFront();

			initWorkspace(m_importDialog, m_importDialog->getCaption());

			CSpace* space = getWorkspace(m_importDialog);
			CSpaceImport* spaceImport = dynamic_cast<CSpaceImport*>(space);

			if (fromWatcher == true)
				spaceImport->initImportFiles(m_assetWatcher->getFiles(), m_assetWatcher->getDeletedFiles());
			else
				spaceImport->initImportAll();
		}

		void CEditor::initLoadSceneGUI(const char* path)
		{
			m_loadSceneDialog = new GUI::CDialogWindow(m_canvas, 0.0f, 0.0f, 600.0f, 120.0f);
			m_loadSceneDialog->setCaption(L"Load Scene");
			m_loadSceneDialog->showCloseButton(false);
			m_loadSceneDialog->setCenterPosition();
			m_loadSceneDialog->bringToFront();

			initWorkspace(m_loadSceneDialog, m_loadSceneDialog->getCaption());

			CSpace* space = getWorkspace(m_loadSceneDialog);
			CSpaceLoadScene* spaceLoadScene = dynamic_cast<CSpaceLoadScene*>(space);

			spaceLoadScene->loadScene(path);
		}

		void CEditor::initEditorGUI()
		{
			// bind save layout function
			m_canvas->OnSaveDockLayout = std::bind(&CEditor::saveLayout, this, std::placeholders::_1);

			// init menu bar
			m_menuBar = new GUI::CMenuBar(m_canvas);
			initMenuBar();

			// init status bar
			GUI::CToolbar* statusBar = new GUI::CToolbar(m_canvas);
			statusBar->dock(GUI::EPosition::Bottom);

			m_statusInfo = new GUI::CTableRow(statusBar);
			m_statusInfo->setWidth(450.0f);
			m_statusInfo->setColumnCount(4);

			float colWidth = round(m_statusInfo->width() / 4.0f);
			for (int i = 0; i < 4; i++)
				m_statusInfo->setColumnWidth(i, colWidth);

			m_statusInfo->setCellText(1, L"");
			m_statusInfo->setCellText(2, L"");
			m_statusInfo->setCellText(3, L"");
			m_statusInfo->dock(GUI::EPosition::Right);

			m_status = new GUI::CIconTextItem(statusBar);
			m_status->setLabel(L"");
			m_status->setIcon(GUI::ESystemIcon::None);
			m_status->dock(GUI::EPosition::Fill);

			// init dock panel
			m_dockPanel = new GUI::CDockPanel(m_canvas);
			m_dockPanel->dock(GUI::EPosition::Fill);

			// calculate layout
			m_canvas->recurseLayout();

			u32 x, y, w, h;
			bool maximize = false;
			std::string data;
			CWindowConfig::loadConfigAndExtraData(x, y, w, h, maximize, data);

			if (data.length() > 0)
			{
				initSessionLayout(data);
			}
			else
			{
				initDefaultLayout();
			}

			m_uiInitiate = true;
		}

		void CEditor::initMenuBar()
		{
			GUI::CMenu* submenu, * temp;

			GUI::CMenuItem* logo = m_menuBar->addItem(GUI::ESystemIcon::Windows);
			submenu = logo->getMenu();
			submenu->addItem(L"About");
			submenu->addItem(L"Development funding", GUI::ESystemIcon::Web);
			submenu->addSeparator();
			submenu->addItem(L"Close", GUI::ESystemIcon::Quit, L"Ctrl + Q");
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandLogo, this);

			GUI::CMenuItem* file = m_menuBar->addItem(L"File");
			submenu = file->getMenu();
			submenu->addItem(L"Save", GUI::ESystemIcon::Save, L"Ctrl + S");
			submenu->addItem(L"Save As");
			submenu->OnOpen = BIND_LISTENER(&CEditor::OnOpenMenuFile, this);
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandFile, this);

			GUI::CMenuItem* edit = m_menuBar->addItem(L"Edit");
			submenu = edit->getMenu();
			submenu->addItem(L"Search", GUI::ESystemIcon::Search, L"Ctrl + F");
			submenu->addSeparator();
			submenu->addItem(L"Undo", L"Ctrl + Z");
			submenu->addItem(L"Redo", L"Ctrl + Y");
			submenu->addSeparator();
			submenu->addItem(L"Copy", GUI::ESystemIcon::Copy, L"Ctrl + C");
			submenu->addItem(L"Paste", GUI::ESystemIcon::Paste, L"Ctrl + V");
			submenu->addItem(L"Duplicate", GUI::ESystemIcon::Duplicate, L"Ctrl + D");
			submenu->addItem(L"Cut", GUI::ESystemIcon::None, L"Ctrl + X");
			submenu->addSeparator();
			submenu->addItem(L"Delete");

			GUI::CMenuItem* asset = m_menuBar->addItem(L"Assets");
			submenu = asset->getMenu();
			temp = submenu;

			GUI::CMenuItem* create = submenu->addItem(L"Create");
			submenu = create->getMenu();
			submenu->addItem(L"Scene");
			submenu->addItem(L"Material");
			submenu->addItem(L"Shader");
			submenu->addItem(L"GUI");
			submenu->addItem(L"Sprite");
			submenu->addItem(L"Animation");

			submenu = temp;
			submenu->addSeparator();
			submenu->addItem(L"Refresh", L"Ctrl + R");

			GUI::CMenuItem* gameObject = m_menuBar->addItem(L"GameObject");
			submenu = gameObject->getMenu();
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandGameObject, this);
			submenu->addItem(L"Zone", GUI::ESystemIcon::Collection);
			submenu->addItem(L"Container Object", GUI::ESystemIcon::Folder);
			submenu->addItem(L"Empty Object", GUI::ESystemIcon::Res3D);
			submenu->addSeparator();
			temp = submenu;

			GUI::CMenuItem* object = submenu->addItem(L"Object");
			submenu = object->getMenu();
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandGameObject, this);

			submenu->addItem(L"Cube");
			submenu->addItem(L"Sphere");
			submenu->addItem(L"Capsule");
			submenu->addItem(L"Cylinder");
			submenu->addItem(L"Plane");
			submenu->addSeparator();
			submenu->addItem(L"Mesh");
			submenu->addSeparator();
			submenu->addItem(L"Terrain");
			submenu->addSeparator();
			submenu->addItem(L"Skydome");
			submenu->addItem(L"Skybox");
			submenu->addItem(L"Sky");

			submenu = temp;
			GUI::CMenuItem* effect = submenu->addItem(L"Effect");
			submenu = effect->getMenu();
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandGameObject, this);

			submenu->addItem(L"Particle System");
			submenu->addItem(L"Line");
			submenu->addItem(L"Trail");

			submenu = temp;
			GUI::CMenuItem* lighting = submenu->addItem(L"Lighting");
			submenu = lighting->getMenu();
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandGameObject, this);

			submenu->addItem(L"Direction Light");
			submenu->addItem(L"Point Light");
			submenu->addItem(L"Spot Light");
			submenu->addSeparator();
			submenu->addItem(L"Reflection Probe");
			submenu->addItem(L"Light Probe");

			submenu = temp;
			submenu->addSeparator();
			submenu->addItem(L"Camera");
			submenu->addSeparator();
			submenu->addItem(L"Trigger");

			GUI::CMenuItem* window = m_menuBar->addItem(L"Window");
			submenu = window->getMenu();
			submenu->addItem(L"Project Setting", GUI::ESystemIcon::Setting);
			submenu->addSeparator();
			m_menuWindowItems.clear();
			m_menuWindowItems.push_back(submenu->addItem(L"Assets"));
			m_menuWindowItems.push_back(submenu->addItem(L"Property"));
			m_menuWindowItems.push_back(submenu->addItem(L"Scene"));
			m_menuWindowItems.push_back(submenu->addItem(L"Hierarchy"));
			m_menuWindowItems.push_back(submenu->addItem(L"GUI Design"));
			m_menuWindowItems.push_back(submenu->addItem(L"Animation"));
			m_menuWindowItems.push_back(submenu->addItem(L"Console"));
			submenu->addSeparator();
			submenu->addItem(L"Reset layout");
			submenu->OnOpen = BIND_LISTENER(&CEditor::OnCommandWindowOpen, this);
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandWindow, this);

			GUI::CMenuItem* help = m_menuBar->addItem(L"Help");
			submenu = help->getMenu();
			submenu->addItem(L"Tutorial", GUI::ESystemIcon::Guide);
			submenu->addItem(L"Report a bug", GUI::ESystemIcon::Web);
			submenu->addSeparator();
			submenu->addItem(L"Development funding", GUI::ESystemIcon::Web);

			m_canvas->addAccelerator("Ctrl + Q", BIND_LISTENER(&CEditor::OnMenuQuit, this));
			m_canvas->addAccelerator("Ctrl + S", BIND_LISTENER(&CEditor::OnMenuSave, this));
		}

		void CEditor::saveLayout(const std::string& data)
		{
			CWindowConfig::saveExtraData(data.c_str());
		}

		void CEditor::initDefaultLayout()
		{
			float width = m_dockPanel->width();
			float height = m_dockPanel->height();
			float w, h;

			w = width * 0.2f;
			h = round(height * 0.8f);
			GUI::CDockableWindow* hierarchy = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			hierarchy->setCaption(L"Hierarchy");
			m_dockPanel->dockChildWindow(hierarchy, NULL, GUI::CDockPanel::DockLeft);
			m_dockPanel->recurseLayout();

			w = width;
			h = round(height * 0.8f);
			GUI::CDockableWindow* scene = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			scene->setCaption(L"Scene");
			m_dockPanel->dockChildWindow(scene, NULL, GUI::CDockPanel::DockCenter);
			m_dockPanel->recurseLayout();

			w = round(width * 0.7f);
			h = round(height * 0.3f);
			GUI::CDockableWindow* asset = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			asset->setCaption(L"Assets");
			m_dockPanel->dockChildWindow(asset, NULL, GUI::CDockPanel::DockBottom);
			m_dockPanel->recurseLayout();

			w = round(width * 0.35f);
			h = round(height * 0.3f);
			GUI::CDockableWindow* console = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			console->setCaption(L"Console");
			m_dockPanel->dockChildWindow(console, asset->getCurrentDockTab(), GUI::CDockPanel::DockTargetRight);
			m_dockPanel->recurseLayout();

			w = width * 0.3f;
			h = height;
			GUI::CDockableWindow* property = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			property->setCaption(L"Property");
			m_dockPanel->dockChildWindow(property, NULL, GUI::CDockPanel::DockRight);
			m_dockPanel->recurseLayout();

			initWorkspace(scene, scene->getCaption());
			initWorkspace(asset, asset->getCaption());
			initWorkspace(console, console->getCaption());
			initWorkspace(property, property->getCaption());
			initWorkspace(hierarchy, hierarchy->getCaption());
		}

		void CEditor::initWorkspace(GUI::CWindow* window, const std::wstring& workspace)
		{
			// note:
			// Space will delete when window closed see CSpace::onDestroy

			if (workspace == L"Scene")
			{
				m_workspaces.push_back(new CSpaceScene(window, this));
			}
			else if (workspace == L"Import Assets")
			{
				m_workspaces.push_back(new CSpaceImport(window, this));
			}
			else if (workspace == L"GUI Design")
			{

			}
			else if (workspace == L"Animation")
			{

			}
			else if (workspace == L"Assets")
			{
				m_workspaces.push_back(new CSpaceAssets(window, this));
			}
			else if (workspace == L"Console")
			{
				m_workspaces.push_back(new CSpaceConsole(window, this));
			}
			else if (workspace == L"Property")
			{
				m_workspaces.push_back(new CSpaceProperty(window, this));
			}
			else if (workspace == L"Hierarchy")
			{
				m_workspaces.push_back(new CSpaceHierarchy(window, this));
			}
			else if (workspace == L"Project Setting")
			{
				m_workspaces.push_back(new CSpaceProjectSettings(window, this));
			}
			else if (workspace == L"Load Scene")
			{
				m_workspaces.push_back(new CSpaceLoadScene(window, this));
			}
		}

		void CEditor::removeWorkspace(CSpace* space)
		{
			std::list<CSpace*>::iterator i = std::find(m_workspaces.begin(), m_workspaces.end(), space);
			if (i != m_workspaces.end())
			{
				delete (*i);
				m_workspaces.erase(i);
			}
		}

		CSpace* CEditor::getWorkspace(GUI::CWindow* window)
		{
			for (CSpace* s : m_workspaces)
			{
				if (s->getWindow() == window)
					return s;
			}
			return NULL;
		}

		CSpace* CEditor::getWorkspaceByName(const std::wstring& name)
		{
			for (CSpace* s : m_workspaces)
			{
				if (s->getWindow()->getCaption() == name)
					return s;
			}
			return NULL;
		}

		void CEditor::initSessionLayout(const std::string& data)
		{
			io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();
			io::IReadFile* file = fs->createMemoryReadFile(data.c_str(), (s32)data.length(), "data");
			io::IXMLReader* xmlRead = fs->createXMLReader(file);
			if (xmlRead == NULL)
				return;

			while (xmlRead->read())
			{
				switch (xmlRead->getNodeType())
				{
				case io::EXN_ELEMENT:
				{
					core::stringw nodeName = xmlRead->getNodeName();

					if (core::stringw(L"canvas") == nodeName)
					{
						readBound(xmlRead, m_canvas);
					}
					else if (core::stringw(L"dock-panel") == nodeName)
					{
						// dock layout
						readBound(xmlRead, m_dockPanel);
						readDockLayout(xmlRead, m_dockPanel);
					}
				}
				break;
				case io::EXN_ELEMENT_END:
				{
				}
				break;
				case io::EXN_TEXT:
				{
					// std::wstring text = xmlRead->getNodeData();
				}
				break;
				}
			}

			xmlRead->drop();
			file->drop();
		}

		void CEditor::readBound(io::IXMLReader* xml, GUI::CBase* base)
		{
			char ansiData[64];
			const wchar_t* valueX = xml->getAttributeValue(L"x");
			const wchar_t* valueY = xml->getAttributeValue(L"y");
			const wchar_t* valueW = xml->getAttributeValue(L"w");
			const wchar_t* valueH = xml->getAttributeValue(L"h");

			if (valueX == NULL || valueY == NULL || valueW == NULL || valueH == NULL)
				return;

			float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;

			if (valueX != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueX, ansiData);
				x = (float)atof(ansiData);
			}

			if (valueY != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueY, ansiData);
				y = (float)atof(ansiData);
			}

			if (valueW != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueW, ansiData);
				w = (float)atof(ansiData);
			}

			if (valueH != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueH, ansiData);
				h = (float)atof(ansiData);
			}

			base->setBounds(x, y, w, h);
		}

		void CEditor::readDockLayout(io::IXMLReader* xmlRead, GUI::CDockPanel* panel)
		{
			while (xmlRead->read())
			{
				switch (xmlRead->getNodeType())
				{
				case io::EXN_ELEMENT:
				{
					core::stringw nodeName = xmlRead->getNodeName();

					if (nodeName == L"horizontal")
					{
						readSpliterLayout(xmlRead, panel, panel->getMainSpliter(), true);
					}
					else if (nodeName == L"vertical")
					{
						readSpliterLayout(xmlRead, panel, panel->getMainSpliter(), false);
					}
					else if (nodeName == L"window")
					{
						const wchar_t* name;
						name = xmlRead->getAttributeValue(L"name");
						if (name != NULL)
						{
							GUI::CDockableWindow* win = new GUI::CDockableWindow(panel, 0, 0, 1, 1);
							readBound(xmlRead, win);
							win->setCaption(name);

							initWorkspace(win, name);
						}
					}
				}
				break;
				case io::EXN_ELEMENT_END:
				{
					core::stringw nodeName = xmlRead->getNodeName();

					if (core::stringw(L"dock-panel") == nodeName)
					{
						return;
					}
				}
				break;
				}
			}
		}

		void CEditor::readSpliterLayout(io::IXMLReader* xmlRead, GUI::CDockPanel* panel, GUI::CSplitter* spliter, bool isHorizontal)
		{
			u32 count = 0;
			u32 weakRow = 0;
			u32 weakCol = 0;

			char ansiData[64];
			const wchar_t* valueCount;

			readBound(xmlRead, spliter);

			valueCount = xmlRead->getAttributeValue(L"count");
			if (valueCount != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
				count = (u32)atoi(ansiData);
			}

			valueCount = xmlRead->getAttributeValue(L"weak-row");
			if (valueCount != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
				weakRow = (u32)atoi(ansiData);
			}

			valueCount = xmlRead->getAttributeValue(L"weak-col");
			if (valueCount != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
				weakCol = (u32)atoi(ansiData);
			}

			if (isHorizontal)
				spliter->setNumberRowCol(1, count);
			else
				spliter->setNumberRowCol(count, 1);

			spliter->setWeakRow(weakRow);
			spliter->setWeakCol(weakCol);

			u32 col = 0;
			u32 row = 0;

			while (xmlRead->read())
			{
				switch (xmlRead->getNodeType())
				{
				case io::EXN_ELEMENT:
				{
					core::stringw nodeName = xmlRead->getNodeName();
					if (nodeName == L"col")
					{
						float width = 0;
						valueCount = xmlRead->getAttributeValue(L"width");
						if (valueCount != NULL)
						{
							CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
							width = (float)atof(ansiData);
						}

						spliter->setColWidth(col, width);
					}
					if (nodeName == L"row")
					{
						float height = 0;
						valueCount = xmlRead->getAttributeValue(L"height");
						if (valueCount != NULL)
						{
							CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
							height = (float)atof(ansiData);
						}

						spliter->setRowHeight(row, height);
					}
					if (nodeName == L"horizontal")
					{
						GUI::CSplitter* newSpliter = new GUI::CSplitter(spliter);
						spliter->setControl(newSpliter, row, col);
						readSpliterLayout(xmlRead, panel, newSpliter, true);
					}
					else if (nodeName == L"vertical")
					{
						GUI::CSplitter* newSpliter = new GUI::CSplitter(spliter);
						spliter->setControl(newSpliter, row, col);
						readSpliterLayout(xmlRead, panel, newSpliter, false);
					}
					if (nodeName == L"docktab")
					{
						GUI::CDockTabControl* docktab = new GUI::CDockTabControl(spliter, panel);
						readBound(xmlRead, docktab);

						spliter->setControl(docktab, row, col);
						readDockTab(xmlRead, docktab);
					}
				}
				break;
				case io::EXN_ELEMENT_END:
				{
					core::stringw nodeName = xmlRead->getNodeName();

					if (core::stringw(L"horizontal") == nodeName && isHorizontal)
					{
						spliter->saveUserExpectedSize();
						return;
					}
					else if (core::stringw(L"vertical") == nodeName && !isHorizontal)
					{
						spliter->saveUserExpectedSize();
						return;
					}
					else if (nodeName == L"col" && isHorizontal)
					{
						col++;
					}
					else if (nodeName == L"row" && !isHorizontal)
					{
						row++;
					}
				}
				break;
				}
			}
		}

		void CEditor::readDockTab(io::IXMLReader* xmlRead, GUI::CDockTabControl* tabcontrol)
		{
			const wchar_t* currentTab = xmlRead->getAttributeValue(L"current");

			std::wstring current;
			GUI::CDockableWindow* currentWin = NULL;
			if (currentTab != NULL)
				current = currentTab;

			while (xmlRead->read())
			{
				switch (xmlRead->getNodeType())
				{
				case io::EXN_ELEMENT:
				{
					core::stringw nodeName = xmlRead->getNodeName();
					if (nodeName == L"window")
					{
						const wchar_t* name;
						name = xmlRead->getAttributeValue(L"name");
						if (name != NULL)
						{
							GUI::CDockableWindow* win = new GUI::CDockableWindow(m_dockPanel, 0, 0, 1, 1);
							win->setCaption(name);
							tabcontrol->dockWindow(win);

							initWorkspace(win, name);

							if (CStringImp::comp(name, current.c_str()) == 0)
								currentWin = win;
						}
					}
				}
				case io::EXN_ELEMENT_END:
				{
					core::stringw nodeName = xmlRead->getNodeName();
					if (core::stringw(L"docktab") == nodeName)
					{
						if (currentWin != NULL)
							tabcontrol->setCurrentWindow(currentWin);
						return;
					}
				}
				break;
				}
			}
		}

		bool CEditor::onClose()
		{
			if (m_confirmQuit == false)
			{
				CSceneController* sceneController = CSceneController::getInstance();
				if (sceneController->needSave())
				{
					const std::string& path = sceneController->getScenePath();
					std::string name = CPath::getFileName(path);
					if (name.empty())
						name = "UntitledScene.scene";

					GUI::CMessageBox* msgBox = new GUI::CMessageBox(m_canvas, GUI::CMessageBox::YesNoCancel);
					msgBox->setMessage("Save changes before close the Editor?", name);
					msgBox->OnYes = [&, &p = path, controller = sceneController](GUI::CBase* button) {
						if (p.empty())
						{
							std::string assetFolder = CAssetManager::getInstance()->getAssetFolder();
							GUI::COpenSaveDialog* dialog = new GUI::COpenSaveDialog(m_canvas, GUI::COpenSaveDialog::Save, assetFolder.c_str(), assetFolder.c_str(), "scene;*");
							dialog->OnSave = [&, controller = controller](std::string path)
							{
								controller->save(path.c_str());
							};
						}
						else
						{
							controller->save(path.c_str());
							m_confirmQuit = true;
							getIrrlichtDevice()->closeDevice();
						}
					};
					msgBox->OnNo = [&](GUI::CBase* button) {
						m_confirmQuit = true;
						getIrrlichtDevice()->closeDevice();
					};
					return false;
				}
				else
				{
					m_confirmQuit = true;
					return true;
				}
			}
			else
			{
				m_confirmQuit = true;
				return true;
			}
		}

		void CEditor::OnMenuQuit(GUI::CBase* item)
		{
			if (onClose() == true)
			{
				getIrrlichtDevice()->closeDevice();
			}
		}

		void CEditor::OnMenuSave(GUI::CBase* item)
		{
			CSceneController* sceneController = CSceneController::getInstance();
			const std::string& fileName = sceneController->getScenePath();

			if (fileName.empty())
			{
				std::string assetFolder = CAssetManager::getInstance()->getAssetFolder();

				GUI::COpenSaveDialog* dialog = new GUI::COpenSaveDialog(m_canvas, GUI::COpenSaveDialog::Save, assetFolder.c_str(), assetFolder.c_str(), "scene;*");
				dialog->OnSave = [&, controller = sceneController](std::string path)
				{
					controller->save(path.c_str());
				};
			}
			else
			{
				sceneController->save(fileName.c_str());
			}
		}

		void CEditor::OnCommandLogo(GUI::CBase* item)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(item);
			const std::wstring& label = menuItem->getLabel();

			if (label == L"Close")
			{
				OnMenuQuit(item);
			}
		}

		void CEditor::OnOpenMenuFile(GUI::CBase* item)
		{
			GUI::CMenu* file = dynamic_cast<GUI::CMenu*>(item);
			GUI::CMenuItem* save = file->getItemByLabel(L"Save");
			GUI::CMenuItem* saveAs = file->getItemByLabel(L"Save As");

			CSceneController* sceneController = CSceneController::getInstance();
			if (sceneController->needSave())
				save->setDisabled(false);
			else
				save->setDisabled(true);

			saveAs->setDisabled(false);
		}

		void CEditor::OnCommandFile(GUI::CBase* item)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(item);
			const std::wstring& label = menuItem->getLabel();

			if (label == L"Save")
			{
				OnMenuSave(item);
			}
			else if (label == L"Save As")
			{
				std::string assetFolder = CAssetManager::getInstance()->getAssetFolder();

				GUI::COpenSaveDialog* dialog = new GUI::COpenSaveDialog(m_canvas, GUI::COpenSaveDialog::SaveAs, assetFolder.c_str(), assetFolder.c_str(), "scene;*");
				dialog->OnSave = [&, controller = CSceneController::getInstance()](std::string path)
				{
					controller->save(path.c_str());
				};
			}
		}

		void CEditor::OnCommandWindowOpen(GUI::CBase* item)
		{
			GUI::CMenu* menu = dynamic_cast<GUI::CMenu*>(item);
			if (menu == NULL)
				return;

			for (GUI::CMenuItem* item : m_menuWindowItems)
			{
				setCheckIcon(item);
			}
		}

		void CEditor::setCheckIcon(GUI::CMenuItem* item)
		{
			if (item == NULL)
				return;

			item->showIcon(true);

			CSpace* space = getWorkspaceByName(item->getLabel());
			if (space != NULL)
				item->setIcon(GUI::ESystemIcon::Check);
			else
				item->setIcon(GUI::ESystemIcon::None);
		}

		void CEditor::closeOpenWorkspace(GUI::CMenuItem* item)
		{
			CSpace* space = getWorkspaceByName(item->getLabel());
			if (space != NULL)
			{
				// Space is open -> close
				GUI::CDockableWindow* dockWindow = dynamic_cast<GUI::CDockableWindow*>(space->getWindow());
				if (dockWindow != NULL)
				{
					GUI::CDockTabControl* dockTab = dockWindow->getCurrentDockTab();
					if (dockTab != NULL)
					{
						// if window is docked on TabControl
						GUI::CTabButton* tabButton = dockTab->getTabButtonByPage(dockWindow);
						if (tabButton != NULL)
							dockTab->doTabClose(tabButton);
					}
					else
					{
						// call close on float window
						space->getWindow()->onCloseWindow();
					}
				}
				else
				{
					// call close non DockableWindow
					space->getWindow()->onCloseWindow();
				}
			}
			else
			{
				// Space is hide -> open
				float w = 680.0f;
				float h = 480.0f;
				GUI::CDockableWindow* window = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
				window->setCaption(item->getLabel());
				window->setCenterPosition();
				initWorkspace(window, window->getCaption());
			}
		}

		void CEditor::OnCommandWindow(GUI::CBase* item)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(item);
			const std::wstring& label = menuItem->getLabel();

			if (label == L"Reset layout")
			{
				// delete old control
				m_dockPanel->remove();
				m_canvas->processDelayedDeletes();

				// init dock panel
				m_dockPanel = new GUI::CDockPanel(m_canvas);
				m_dockPanel->dock(GUI::EPosition::Fill);

				// calculate layout
				m_canvas->recurseLayout();

				// reset layout
				initDefaultLayout();

				m_canvas->invalidate();
			}
			else if (label == L"Project Setting")
			{
				showProjectSetting();
			}
			else
			{
				// do close/open the window by item label
				for (GUI::CMenuItem* item : m_menuWindowItems)
				{
					if (item == menuItem)
					{
						closeOpenWorkspace(menuItem);
						break;
					}
				}
			}
		}

		void CEditor::showProjectSetting()
		{
			float w = 680.0f;
			float h = 480.0f;
			GUI::CDialogWindow* window = new GUI::CDialogWindow(m_canvas, 0.0f, 0.0f, w, h);
			window->setCaption(L"Project Setting");
			window->setCenterPosition();
			window->setResizable(true);
			initWorkspace(window, window->getCaption());
		}

		void CEditor::OnCommandGameObject(GUI::CBase* item)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(item);
			const std::wstring& label = menuItem->getLabel();

			CSceneController::getInstance()->onCommand(label);
		}
	}
}