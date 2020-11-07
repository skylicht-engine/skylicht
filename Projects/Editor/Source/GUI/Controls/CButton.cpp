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
#include "CButton.h"
#include "GUI/CGUIContext.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CButton::CButton(CBase *parent) :
				CBase(parent),
				m_color(CThemeConfig::ButtonColor),
				m_hoverColor(CThemeConfig::ButtonHoverColor),
				m_pressColor(CThemeConfig::ButtonPressColor),
				m_focusColor(CThemeConfig::ButtonFocusColor),
				m_pressed(false),
				m_drawBackground(true)
			{
				setPadding(SPadding(8.0f, 0.0f, 8.0f, 0.0f));
				m_icon = new CIcon(this, ESystemIcon::None);
				m_icon->dock(EPosition::Left);

				m_label = new CTextContainer(this);
				m_label->setMargin(SMargin(0.0f, 2.0f, 0.0f, 0.0f));
				m_label->dock(EPosition::Fill);
				m_label->setColor(CThemeConfig::ButtonTextColor);

				setSize(80.0f, 20.0f);
				m_icon->setHidden(true);

				m_iconColor = m_icon->getColor();
				m_labelColor = m_label->getColor();
			}

			CButton::~CButton()
			{

			}

			void CButton::renderUnder()
			{
				if (!m_drawBackground)
					return;

				CTheme::getTheme()->drawButtonShadow(getRenderBounds());

				SGUIColor c = m_color;

				if (isHovered())
				{
					if (m_pressed == true)
						c = m_pressColor;
					else
						c = m_hoverColor;
				}

				CTheme::getTheme()->drawButton(getRenderBounds(), c);
			}

			void CButton::onBoundsChanged(const SRect& oldBounds)
			{
				CBase::onBoundsChanged(oldBounds);

				m_label->sizeToContents();
				invalidate();
			}

			void CButton::setLabel(const std::wstring& text)
			{
				m_label->setString(text);

				if (OnTextChanged != nullptr)
					OnTextChanged(this);
			}

			void CButton::onMouseClickLeft(float x, float y, bool down)
			{
				if (isDisabled())
					return;

				if (down)
				{
					m_pressed = true;
					m_icon->setColor(CThemeConfig::IconPressColor);
					m_label->setColor(CThemeConfig::TextPressColor);

					if (OnDown != nullptr)
						OnDown(this);
				}
				else
				{
					if (isHovered() && OnPress != nullptr)
					{
						OnPress(this);
					}

					m_pressed = false;
					m_icon->setColor(m_iconColor);
					m_label->setColor(m_labelColor);
				}
			}

			void CButton::onMouseClickRight(float x, float y, bool down)
			{
				if (isDisabled())
					return;

				if (down)
				{
					m_pressed = true;
					m_icon->setColor(CThemeConfig::IconPressColor);
					m_label->setColor(CThemeConfig::TextPressColor);
				}
				else
				{
					if (isHovered() && OnPress != nullptr)
					{
						OnPress(this);
					}

					m_pressed = false;
					m_icon->setColor(m_iconColor);
					m_label->setColor(m_labelColor);
				}
			}

			void CButton::onMouseEnter()
			{
				if (OnHover != nullptr)
					OnHover(this);
			}

			void CButton::onMouseLeave()
			{
				m_icon->setColor(m_iconColor);
				m_label->setColor(m_labelColor);
			}

			void CButton::sizeToContents()
			{
				float h = height();

				m_label->sizeToContents();

				float w = m_padding.Left + m_padding.Right + m_label->width();
				if (m_icon->isHidden() == false)
					w = w + m_icon->width();

				setSize(w, h);
			}
		}
	}
}