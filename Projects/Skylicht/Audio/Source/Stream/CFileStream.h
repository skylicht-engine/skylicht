/*
!@
MIT License

Copyright (c) 2012 - 2019 Skylicht Technology CO., LTD

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

#ifndef _SKYLICHTAUDIO_FILESTREAM_H_
#define _SKYLICHTAUDIO_FILESTREAM_H_

#include "stdafx.h"
#include "IStream.h"

namespace Skylicht
{
	namespace Audio
	{
		class CFileStreamCursor : public IStreamCursor
		{
		protected:
			FILE *m_file;
			int m_size;
			int m_pos;
			
			unsigned char* m_buffer;
			int m_bufferSize;
			int m_bufferAvailableBytes;
			int m_bufferPos;
			int m_bufferOffset;
		public:
			CFileStreamCursor(FILE *file);
			virtual ~CFileStreamCursor();
			
			virtual int seek(int pos, EOrigin origin);
			virtual int tell();
			virtual int read(unsigned char* buff, int len);
			virtual bool endOfStream();
			virtual int size();
			virtual bool readyReadData(int len);
		};
		
		class CFileStream : public IStream
		{
		protected:
			std::string	m_fileName;
			FILE *m_file;
			int m_size;
			
		public:
			CFileStream(const char *fileName);
			virtual ~CFileStream();
			
			bool isError();
			
			virtual IStreamCursor* createCursor();
		};
	}
}

#endif
