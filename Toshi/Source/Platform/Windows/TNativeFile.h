#pragma once
#include "Toshi/Typedefs.h"
#include "Toshi/File/TFile.h"
#include "TNativeFileSystem.h"

namespace Toshi
{
	class TNativeFile : public TFile
	{
	public:
		TNativeFile(const TNativeFile&);

		virtual size_t Read(LPVOID, size_t) override;
		virtual int Write(LPVOID, size_t) { return 0; }
		virtual bool Seek(int, TFile::TSEEK) override;
		virtual uint32_t Tell() override;
		virtual int GetSize() override;
		virtual _FILETIME GetDate() override;
		virtual char GetCChar() override;
		virtual wchar_t GetWChar() { return 0; }
		virtual int PutCChar(char character) { return 0; }
		virtual int PutWChar(wchar_t character) { return 0; }
		virtual int VCPrintf(const char* format, ...) { return 0; }
		virtual int VWPrintf(const wchar_t* format, ...) { return 0; }
		
		bool LoadBuffer(DWORD curBufferOverflow);
		int FlushWriteBuffer();
		int ReadUnbuffered(LPVOID, int);

	protected:
		TNativeFile(class TNativeFileSystem*);

		bool Open(const TCString& a_FileName, uint32_t a_Flags);
		void Close();

		friend TNativeFileSystem;

	private:
		HANDLE hnd; // 0x8
		uint32_t m_Position; // 0xC
		DWORD unk2; //0x10
		uint32_t m_PrevBufferPos; // 0x14
		uint32_t m_LastBufferSize; // 0x18
		char* m_pBuffer1; // 0x1C
		char* m_pBuffer2; // 0x20
		uint32_t m_iWriteBufferUsed; // 0x24
		bool m_bWriteBuffered; // 0x28
	};
}


