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

#pragma once

#include "CGUIElement.h"
#include "Graphics2D/SpriteFrame/IFont.h"

#define MAX_FORMATCOLOR 32

#define HAVE_CARET

namespace Skylicht
{
	class CFontSource;

	class SKYLICHT_API CGUIText : public CGUIElement
	{
		friend class CCanvas;
	public:
		typedef std::vector<int> ArrayInt;
		typedef std::vector<SModuleOffset*> ArrayModuleOffset;

	protected:
		IFont* m_font;
		IFont* m_customfont;

		int m_charPadding;
		int m_charSpacePadding;
		int m_linePadding;

		std::string m_text;
		std::wstring m_textw;
		ArrayInt m_textFormat;

		bool m_enableTextFormat;

		std::string m_textId;
		std::wstring m_textwId;

		EGUIVerticalAlign TextVertical;
		EGUIHorizontalAlign TextHorizontal;

		int m_textHeight;
		int m_textOffsetY;
		int m_textSpaceWidth;

		bool m_multiLine;
		bool m_centerRotate;

		SColor m_colorFormat[MAX_FORMATCOLOR];

		bool m_updateTextRender;

		std::vector<ArrayModuleOffset> m_arrayCharRender;
		std::vector<ArrayInt> m_arrayCharFormat;
		std::vector<ArrayInt> m_arrayCharId;

		std::string m_fontSource;
		std::string m_fontGUID;

		CFontSource* m_fontData;
		int m_fontChanged;

		float m_lastWidth;
		float m_lastHeight;

#ifdef HAVE_CARET
		bool m_showCaret;
		int m_caretShader;
		int m_setCaret;
		float m_caretBlink;
		float m_caretBlinkSpeed;
		core::vector2di m_caret;
#endif

	protected:
		CGUIText(CCanvas* canvas, CGUIElement* parent, IFont* font);
		CGUIText(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect, IFont* font);

		virtual void renderText(ArrayModuleOffset& string, ArrayInt& format, int posX, int posY, int line);

		void init();

	public:
		virtual ~CGUIText();

		virtual void render(CCamera* camera);

		void setTextAlign(EGUIHorizontalAlign h, EGUIVerticalAlign v)
		{
			TextVertical = v;
			TextHorizontal = h;
		}

		/*
		* Example:
		* <1>PLAYERA <0>gives <2>a heart <0>to <1>PLAYERB
		* With format:
		* <1> -> RED
		* <0> -> WHITE
		* <2> -> YELLOW
		*/
		void setFormatText(const char* formatText);

		// setColorFormat
		// set color for function setFormatText
		void setColorFormat(int id, const SColor& c)
		{
			if (id < MAX_FORMATCOLOR)
				m_colorFormat[id] = c;
		}

		void setText(const char* text);
		void setTextStrim(const char* text);
		void setText(const wchar_t* text);
		void setTextStrim(const wchar_t* text);

		void setTextId(const char* textId);
		void setTextId(const wchar_t* textId);

		int getStringWidth(const char* text);
		int getCharWidth(wchar_t c);

		const char* getText()
		{
			return m_text.c_str();
		}

		const wchar_t* getTextW()
		{
			return m_textw.c_str();
		}

		const char* getTextId()
		{
			return m_textId.c_str();
		}

		const wchar_t* getTextwId()
		{
			return m_textwId.c_str();
		}

		inline int getTextLength()
		{
			return (int)m_textw.size();
		}

		inline void setEnableTextFormnat(bool b)
		{
			m_enableTextFormat = b;
		}

		inline bool isEnableTextFormat()
		{
			return m_enableTextFormat;
		}

		inline void setCharPadding(int charPadding)
		{
			m_charPadding = charPadding;
			m_charSpacePadding = charPadding;
		}

		inline void setLinePadding(int linePadding)
		{
			m_linePadding = linePadding;
		}

		inline void setMultiLine(bool b)
		{
			m_multiLine = b;
		}

		inline void setCenterRotate(bool b)
		{
			m_centerRotate = b;
		}

		void setFontSource(const char* fontSource);

		DECLARE_GETTYPENAME(CGUIText)

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		void getClosestCharacter(float posX, float posY, int& line, int& character);

#ifdef HAVE_CARET
		inline void showCaret(bool b)
		{
			m_showCaret = b;
		}

		inline void setCaret(int line, int character)
		{
			m_caret.set(character, line);
			m_caretBlink = 0.0f;
		}

		void setCaret(int charPos);

		inline void getCaretPosition(int& line, int& character)
		{
			character = m_caret.X;
			line = m_caret.Y;
		}

		inline int getNumLine()
		{
			return (int)m_arrayCharRender.size();
		}

		int getNumCharacter(int line);

		bool getWordAtPosition(int line, int charPosition, int& from, int& to);

		bool isCharacter(wchar_t c);

		void doBackspace();

		void doDelete();

		void insert(wchar_t c);
#endif

	protected:

#ifdef HAVE_CARET
		void updateCaret();

		void updateSetCaret();
#endif

		void updateSplitText();

		void splitText(std::vector<ArrayModuleOffset>& split, std::vector<ArrayInt>& format, std::vector<ArrayInt>& id, int width);

		void getClosestCharacter(float posX, int x, int y, int line, const core::matrix4& world, int& character);

		void initFont(IFont* font);

		IFont* getCurrentFont();
	};
}
