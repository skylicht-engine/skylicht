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
#include "CDefaultEditor.h"

#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/SpaceController/CSceneController.h"

#include "GameObject/CGameObject.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Editor
	{
		CDefaultEditor::CDefaultEditor() :
			m_data(NULL)
		{

		}

		CDefaultEditor::~CDefaultEditor()
		{
			closeGUI();
		}

		void CDefaultEditor::closeGUI()
		{
			for (size_t i = 0, n = m_subjects.size(); i < n; i++)
				delete m_subjects[i];
			m_subjects.clear();

			if (m_data != NULL)
				delete m_data;

			m_data = NULL;
			m_component = NULL;
			m_gameObject = NULL;
		}

		void CDefaultEditor::initGUI(CComponentSystem* target, CSpaceProperty* ui)
		{
			m_component = target;
			m_gameObject = target->getGameObject();
			m_data = target->createSerializable();
			m_changed = false;

			if (m_gameObject->isEnableEditorChange() && m_component != NULL)
			{
				// setup gui
				std::string name = m_component->getTypeName().c_str();
				if (!m_name.empty())
					name = m_name;

				GUI::CCollapsibleGroup* group = ui->addGroup(name.c_str(), this);

				GUI::CBoxLayout* layout = ui->createBoxLayout(group);

				initDataGUI(m_data, layout, ui);

				initCustomGUI(layout, ui);

				group->setExpand(true);
			}
		}

		void CDefaultEditor::initDataGUI(CObjectSerializable* data, GUI::CBoxLayout* layout, CSpaceProperty* ui)
		{
			// add serializable data control
			for (u32 i = 0, n = data->getNumProperty(); i < n; i++)
			{
				CValueProperty* valueProperty = data->getPropertyID(i);
				if (valueProperty->isHidden())
					continue;

				// add ui space
				if (valueProperty->getUISpace() > 0)
				{
					float space = valueProperty->getUISpace();
					if (space > 100.0f)
						space = 100.0f;
					layout->addSpace(space);
				}

				if (valueProperty->getUIHeader().size() > 0)
				{
					std::wstring label = CStringImp::convertUTF8ToUnicode(valueProperty->getUIHeader().c_str());
					ui->addLabel(layout, label.c_str(), GUI::TextLeft);
				}

				if (valueProperty->getType() == EPropertyDataType::Bool)
				{
					CBoolProperty* value = dynamic_cast<CBoolProperty*>(valueProperty);
					CSubject<bool>* subject = new CSubject<bool>(value->get());
					subject->addObserver(new CObserver([&, value, s = subject](ISubject* subject, IObserver* from)
						{
							value->set(s->get());
							updateData();
						}), true);

					ui->addCheckBox(layout, ui->getPrettyName(value->Name), subject);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::Float)
				{
					CFloatProperty* value = dynamic_cast<CFloatProperty*>(valueProperty);
					if (value->ClampMin && value->ClampMax)
					{
						// add slider control on Limit Float
						CSubject<float>* subject = new CSubject<float>(value->get());
						CObserver* observer = new CObserver();
						observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
							{
								if (from != o)
								{
									float v = s->get();
									value->set(v);
									s->set(v);
									updateData();
								}
							};
						subject->addObserver(observer, true);
						ui->addSlider(layout, ui->getPrettyName(value->Name), subject, value->Min, value->Max);
					}
					else
					{
						CSubject<float>* subject = new CSubject<float>(value->get());
						CObserver* observer = new CObserver();
						observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
							{
								if (from != o)
								{
									float v = s->get();
									bool notifyUI = false;

									if (value->ClampMin && v < value->Min)
									{
										v = value->Min;
										notifyUI = true;
									}
									if (value->ClampMax && v > value->Max)
									{
										v = value->Max;
										notifyUI = true;
									}

									value->set(v);

									if (notifyUI)
									{
										s->set(v);
										s->notify(o);
									}

									updateData();
								}
							};
						subject->addObserver(observer, true);
						ui->addNumberInput(layout, ui->getPrettyName(value->Name), subject, 0.01f);
						m_subjects.push_back(subject);
					}
				}
				else if (valueProperty->getType() == EPropertyDataType::Integer)
				{
					CIntProperty* value = dynamic_cast<CIntProperty*>(valueProperty);
					CSubject<int>* subject = new CSubject<int>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								int v = s->get();
								bool notifyUI = false;

								if (value->ClampMin && v < value->Min)
								{
									v = value->Min;
									notifyUI = true;
								}
								if (value->ClampMax && v > value->Max)
								{
									v = value->Max;
									notifyUI = true;
								}

								value->set(v);

								if (notifyUI)
								{
									s->set(v);
									s->notify(o);
								}

								updateData();
							}
						};
					subject->addObserver(observer, true);
					ui->addNumberInput(layout, ui->getPrettyName(value->Name), subject);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::Vector3)
				{
					CVector3Property* value = dynamic_cast<CVector3Property*>(valueProperty);
					const core::vector3df& v = value->get();

					CSubject<float>* X = new CSubject<float>(v.X);
					CSubject<float>* Y = new CSubject<float>(v.Y);
					CSubject<float>* Z = new CSubject<float>(v.Z);

					CObserver* observer = new CObserver();
					observer->Notify = [&, value, X, Y, Z, observer](ISubject* subject, IObserver* from)
						{
							if (from != observer)
							{
								core::vector3df v = value->get();
								v.X = X->get();
								v.Y = Y->get();
								v.Z = Z->get();
								value->set(v);
								updateData();
							}
						};

					X->addObserver(observer);
					Y->addObserver(observer);
					Z->addObserver(observer, true);

					valueProperty->OnSetHidden = [X, Y, Z, o = observer](bool hide)
						{
							X->setEnable(!hide);
							X->notify(o);

							Y->setEnable(!hide);
							Y->notify(o);

							Z->setEnable(!hide);
							Z->notify(o);
						};

					valueProperty->OnChanged = [value, X, Y, Z, observer]()
						{
							X->set(value->get().X);
							X->notify(observer);

							Y->set(value->get().Y);
							Y->notify(observer);

							Z->set(value->get().Z);
							Z->notify(observer);
						};

					std::wstring name = ui->getPrettyName(value->Name);
					name += L" X";

					ui->addNumberInput(layout, name.c_str(), X, 0.1f);
					ui->addNumberInput(layout, L"Y", Y, 0.1f);
					ui->addNumberInput(layout, L"Z", Z, 0.1f);
					layout->addSpace(5.0f);

					m_subjects.push_back(X);
					m_subjects.push_back(Y);
					m_subjects.push_back(Z);
				}
				else if (valueProperty->getType() == EPropertyDataType::UInteger)
				{
					CUIntProperty* value = dynamic_cast<CUIntProperty*>(valueProperty);
					CSubject<u32>* subject = new CSubject<u32>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								u32 v = s->get();
								bool notifyUI = false;

								if (value->ClampMax && v > value->Max)
								{
									v = value->Max;
									notifyUI = true;
								}

								value->set(v);

								if (notifyUI)
								{
									s->set(v);
									s->notify(o);
								}

								updateData();
							}
						};
					subject->addObserver(observer, true);
					ui->addNumberInput(layout, ui->getPrettyName(value->Name), subject);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::String)
				{
					CStringProperty* value = dynamic_cast<CStringProperty*>(valueProperty);
					std::wstring stringValue = CStringImp::convertUTF8ToUnicode(value->get().c_str());

					CSubject<std::wstring>* subject = new CSubject<std::wstring>(stringValue);

					CObserver* observer = new CObserver();
					observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								const std::wstring& stringValue = s->get();
								std::string stringValueA = CStringImp::convertUnicodeToUTF8(stringValue.c_str());
								value->set(stringValueA);
								updateData();
							}
						};

					subject->addObserver(observer, true);
					ui->addTextBox(layout, ui->getPrettyName(value->Name), subject);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::Color)
				{
					CColorProperty* value = dynamic_cast<CColorProperty*>(valueProperty);
					CSubject<SColor>* subject = new CSubject<SColor>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								const SColor& color = s->get();
								value->set(color);
								updateData();
							}
						};

					subject->addObserver(observer, true);
					ui->addColorPicker(layout, ui->getPrettyName(value->Name), subject);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::FilePath)
				{
					CFilePathProperty* value = dynamic_cast<CFilePathProperty*>(valueProperty);
					CSubject<std::string>* subject = new CSubject<std::string>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								const std::string& path = s->get();
								value->set(path);
								updateData();
							}
						};

					subject->addObserver(observer, true);
					ui->addInputFile(layout, ui->getPrettyName(value->Name), subject, value->Exts);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::FolderPath)
				{
					CFolderPathProperty* value = dynamic_cast<CFolderPathProperty*>(valueProperty);
					CSubject<std::string>* subject = new CSubject<std::string>(value->get());
					CObserver* observer = new CObserver();
					observer->Notify = [&, value, s = subject, o = observer](ISubject* subject, IObserver* from)
						{
							if (from != o)
							{
								const std::string& path = s->get();
								value->set(path);
								updateData();
							}
						};

					subject->addObserver(observer, true);
					ui->addInputFolder(layout, ui->getPrettyName(value->Name), subject);
					m_subjects.push_back(subject);
				}
				else if (valueProperty->getType() == EPropertyDataType::Enum)
				{
					CEnumPropertyData* enumValue = dynamic_cast<CEnumPropertyData*>(valueProperty);

					GUI::CDropdownBox* dropBox = ui->addDropBox(layout, ui->getPrettyName(valueProperty->Name), L"");
					GUI::CMenu* menu = dropBox->getMenu();

					int currentValue = enumValue->getIntValue();

					int enumCount = enumValue->getEnumCount();
					for (int i = 0; i < enumCount; i++)
					{
						const CEnumPropertyData::SEnumString& enumData = enumValue->getEnum(i);

						std::wstring enumName = CStringImp::convertUTF8ToUnicode(enumData.Name.c_str());

						GUI::CMenuItem* item = menu->addItem(enumName);
						item->tagInt(enumData.Value);

						if (enumData.Value == currentValue)
							dropBox->setLabel(enumName);

						item->OnPress = [&, item, enumValue, dropBox, ui](GUI::CBase* base)
							{
								enumValue->setIntValue(item->getTagInt());
								updateData();
								dropBox->setLabel(item->getLabel());
								ui->getWindow()->getCanvas()->closeMenu();
							};
					}
				}
				else if (valueProperty->getType() == EPropertyDataType::Object)
				{
					CObjectSerializable* object = (CObjectSerializable*)valueProperty;
					CSubject<CObjectSerializable*>* subject = new CSubject<CObjectSerializable*>(object);

					GUI::CCollapsibleGroup* group = NULL;
					GUI::CBoxLayout* objectLayout = NULL;

					bool haveChildGroup = true;

					if (object->getObjectType() == ObjectInterpolate)
						haveChildGroup = false;

					if (haveChildGroup)
					{
						// header
						group = ui->addSubGroup(layout);
						group->getHeader()->setLabel(ui->getPrettyName(valueProperty->Name));
						objectLayout = ui->createBoxLayout(group);

						valueProperty->OnSetHidden = [group](bool hide)
							{
								group->setHidden(hide);
							};
					}

					if (object->getObjectType() == ObjectArray)
					{
						CArraySerializable* arrayObject = (CArraySerializable*)object;

						if (arrayObject->haveCreateElementFunction())
						{
							// add input to add elements
							CSubject<int>* count = new CSubject<int>(arrayObject->getElementCount());
							m_subjects.push_back(count);

							ui->addNumberTextBox(objectLayout, L"Count", count);

							objectLayout = ui->createBoxLayout(group);

							CObserver* observer = new CObserver();
							observer->Notify = [&, arrayObject, count, o = observer, objectLayout, group, ui](ISubject* subject, IObserver* from)
								{
									if (from != o)
									{
										int numElement = count->get();
										if (arrayObject->resize(numElement))
										{
											// remove old ui
											objectLayout->getChild(0)->releaseChildren();

											// re-init ui
											initDataGUI(arrayObject, objectLayout, ui);

											// update object data
											m_component->loadSerializable(m_data);
										}
									}
								};
							count->addObserver(observer, true);
						}
					}
					else if (object->getObjectType() == ObjectInterpolate)
					{
						CInterpolateFloatSerializable* value = (CInterpolateFloatSerializable*)valueProperty;

						CInterpolator interpolator;
						value->getInterpolator(&interpolator);

						CSubject<CInterpolator>* subject = new CSubject<CInterpolator>(interpolator);
						CObserver* observer = new CObserver();
						observer->Notify = [&, object, value, s = subject, o = observer](ISubject* subject, IObserver* from)
							{
								if (from != o)
								{
									value->setInterpolator(&s->get());
									updateData();
								}
							};
						subject->addObserver(observer, true);

						valueProperty->OnSetHidden = [s = subject, o = observer](bool hide)
							{
								s->setEnable(!hide);
								s->notify(o);
							};

						ui->addInterpolateCurves(layout, ui->getPrettyName(value->Name), subject);

						valueProperty->OnChanged = [value, subject, observer]()
							{
								CInterpolator v;
								value->getInterpolator(&v);
								subject->set(v);
								subject->notify(observer);
							};

						m_subjects.push_back(subject);
					}

					// show child data
					if (haveChildGroup)
						initDataGUI(object, objectLayout, ui);

					m_subjects.push_back(subject);
				}

				initCustomDataGUI(data, valueProperty, layout, ui);
			}
		}

		void CDefaultEditor::updateData()
		{
			m_changed = true;
			m_component->loadSerializable(m_data);
			CSceneController::getInstance()->updateTreeNode(m_component->getGameObject());
		}

		void CDefaultEditor::initCustomGUI(GUI::CBoxLayout* layout, CSpaceProperty* ui)
		{
		}

		void CDefaultEditor::initCustomDataGUI(CObjectSerializable* obj, CValueProperty* data, GUI::CBoxLayout* layout, CSpaceProperty* ui)
		{

		}

		void CDefaultEditor::update()
		{

		}
	}
}