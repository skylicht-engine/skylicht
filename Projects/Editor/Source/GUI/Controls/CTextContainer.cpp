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

#include "pch.h"
#include "CTextContainer.h"
#include "GUI/CGUIContext.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Renderer/CRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTextContainer::CTextContainer(CBase *parent) :
				CBase(parent),
				m_wrapMultiLine(false),
				m_textChange(true),
				m_fontSize(EFontSize::SizeNormal),
				m_color(CThemeConfig::DefaultTextColor),
				m_paddingRight(0.0f),
				m_showCaret(false),
				m_caretBeginLine(0),
				m_caretBeginPosition(0),
				m_caretEndLine(0),
				m_caretEndPosition(0),
				m_caretBlinkSpeed(500.0f),
				m_caretBlink(0.0f)
			{
				setMouseInputEnabled(false);
				enableClip(true);
			}

			CTextContainer::~CTextContainer()
			{

			}

			void CTextContainer::render()
			{
				if (m_showCaret == false)
					return;

				m_caretBlink = m_caretBlink + CGUIContext::getDeltaTime();
				if (m_caretBlink > 2.0f * m_caretBlinkSpeed)
					m_caretBlink = 0.0f;

				u32 lineID = 0;
				for (CText *line : m_lines)
				{
					if (lineID == m_caretBeginLine)
					{
						SDimension caretPosition = line->getCharacterPosition(m_caretBeginPosition);
						m_caretRect.X = line->X() + caretPosition.Width;
						m_caretRect.Y = line->Y() - 1.0f;
						m_caretRect.Width = 1.0f;
						m_caretRect.Height = caretPosition.Height + 1.0f;

						if (m_caretBlink < m_caretBlinkSpeed)
							CRenderer::getRenderer()->drawFillRect(m_caretRect, CThemeConfig::DefaultTextColor);
					}
					lineID++;
				}
			}

			void CTextContainer::setWrapMultiline(bool b)
			{
				if (m_wrapMultiLine == b)
					return;

				m_wrapMultiLine = b;
				m_textChange = true;
				invalidate();
			}

			void CTextContainer::setFontSize(EFontSize size)
			{
				if (m_fontSize == size)
					return;

				m_fontSize = size;
				m_textChange = true;
				invalidate();
			}

			void CTextContainer::setString(const std::wstring& string)
			{
				if (m_string == string)
					return;

				m_string = string;
				m_textChange = true;
				invalidate();
			}

			void CTextContainer::setCaretBegin(u32 line, u32 c)
			{
				if (line > m_lines.size())
					line = m_lines.size() - 1;

				m_caretBeginLine = line;
				m_caretBeginPosition = c;
				m_caretBlink = 0.0f;
			}

			void CTextContainer::setCaretEnd(u32 line, u32 c)
			{
				if (line > m_lines.size())
					line = m_lines.size() - 1;

				m_caretEndLine = line;
				m_caretEndPosition = c;
				m_caretBlink = 0.0f;
			}

			u32 CTextContainer::getClosestCharacter(const SPoint& point, u32& outLine, u32& outChar)
			{
				outLine = 0;
				outChar = 0;

				u32 lineOffset = 0;
				CText *foundLine = NULL;

				for (CText *line : m_lines)
				{
					lineOffset += line->getLength();
					outLine++;

					if (point.Y < line->Y())
						continue;

					if (point.Y > line->bottom())
						continue;

					if (point.Y < line->bottom())
					{
						foundLine = line;
						break;
					}
				}

				if (foundLine != NULL)
				{
					lineOffset -= foundLine->getLength();
					outLine--;
					outChar = foundLine->getClosestCharacter(SPoint(point.X - foundLine->X(), point.Y - foundLine->Y()));

					return lineOffset + outChar;
				}

				return 0;
			}

			void CTextContainer::layout()
			{
				if (m_textChange == true)
				{
					sizeToContents();
					m_textChange = false;
				}
			}

			void CTextContainer::setColor(const SGUIColor& color)
			{
				m_color = color;

				ListTextControl::iterator i = m_lines.begin(), end = m_lines.end();
				while (i != end)
				{
					(*i)->setColor(color);
					++i;
				}
			}

			void CTextContainer::sizeToContents()
			{
				removeAllLines();

				// this is not yet layout, so calc from parent (Label, Edit,...)
				float w = m_parent->width() - m_parent->getPadding().Left - m_parent->getPadding().Right - m_paddingRight;

				float x = 0.0f;
				float y = 0.0f;
				std::wstring strLine;

				if (m_wrapMultiLine == true)
				{
					// multi line
					std::vector<std::wstring> words;

					if (splitWords(m_string, words, w) == false)
						return;

					words.push_back(L"");

					for (auto&& it = words.begin(); it != words.end(); ++it)
					{
						bool finishLine = false;
						bool wrapped = false;

						// If this word is a newline - make a newline (we still add it to the
						// text)
						if ((*it).c_str()[0] == '\n')
							finishLine = true;

						// Does adding this word drive us over the width?
						strLine += *it;
						SDimension p = CRenderer::getRenderer()->measureText(m_fontSize, strLine);

						if (p.Width > w)
						{
							finishLine = true;
							wrapped = true;
						}

						// If this is the last word then finish the line
						if (--words.end() == it)
							finishLine = true;

						if (finishLine)
						{
							CText* t = new CText(this);
							t->setFontSize(m_fontSize);
							t->setColor(m_color);

							if (wrapped)
							{
								t->setString(strLine.substr(0, strLine.length() - (*it).length()));
								// newline should start with the word that was too big
								strLine = *it;
							}
							else
							{
								t->setString(strLine.substr(0, strLine.length()));
								//new line is empty
								strLine.clear();
							}
							t->setBounds(x, y, w, p.Height);
							m_lines.push_back(t);

							// Position the newline
							y += p.Height;
							x = 0.0f;
						}
					}
				}
				else
				{
					// compute size of text
					SDimension p = CRenderer::getRenderer()->measureText(m_fontSize, m_string);

					// single line
					CText* t = new CText(this);
					t->setFontSize(m_fontSize);
					t->setColor(m_color);
					t->setBounds(x, y, p.Width, p.Height);
					t->setString(m_string);
					m_lines.push_back(t);
				}

				if (m_lines.size() > 0)
				{
					u32 caretLineID = m_lines.size() - 1;
					u32 caretCharID = m_lines.back()->getLength();

					setCaretBegin(caretLineID, caretCharID);
					setCaretEnd(caretLineID, caretCharID);
				}

				// Size to children height and parent width
				{
					SDimension childsize = getChildrenSize();
					setSize(childsize.Width, childsize.Height);
				}

				invalidateParent();
				invalidate();
			}

			bool CTextContainer::splitWords(std::wstring s, std::vector<std::wstring>& lines, float lineWidth)
			{
				std::wstring str;

				for (size_t i = 0; i < s.length(); i++)
				{
					if (s[i] == '\n')
					{
						if (!str.empty())
							lines.push_back(str);

						lines.push_back(L"\n");
						str.clear();
						continue;
					}

					if (s[i] == ' ')
					{
						str += s[i];
						lines.push_back(str);
						str.clear();
						continue;
					}

					str += s[i];

					// if adding character makes the word bigger than the textbox size
					SDimension p = CRenderer::getRenderer()->measureText(m_fontSize, str);
					if (p.Width > lineWidth)
					{
						//split words
						str.pop_back();

						if (str.empty())
							return false;

						lines.push_back(str);
						str.clear();
						--i;
						continue;
					}
				}

				if (!str.empty())
					lines.push_back(str);

				return true;
			}

			void CTextContainer::removeAllLines()
			{
				ListTextControl::iterator i = m_lines.begin(), end = m_lines.end();
				while (i != end)
				{
					CText *t = (*i);
					removeChild(t);
					delete t;
					++i;
				}
				m_lines.clear();
			}
		}
	}
}