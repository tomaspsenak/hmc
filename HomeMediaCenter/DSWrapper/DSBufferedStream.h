#pragma once

#if !defined(DSBUFFEREDSTREAM_DSWRAPPER_INCLUDED)
#define DSBUFFEREDSTREAM_DSWRAPPER_INCLUDED

//Trieda DSBufferedStream umoznuje prepojenie seekovatelneho streamu do neseekovatelneho (napr. network stream)
//Data su ukladane do buffera v ktorom je mozne seekovat. Po jeho naplneni sa odosle. Pri seekovani a zapise
//pred buffer sa poziadavka nevykona. Pri seekovani za buffer sa do streamu poslu nuly.

namespace DSWrapper 
{
	public ref class DSBufferedStream : public System::IO::Stream
	{
		public:		DSBufferedStream(System::IO::Stream ^ innerStream, System::Int32 length);

					property System::Boolean CanRead
					{
						virtual System::Boolean get() override { return this->m_innerStream->CanRead; }
					}

					property System::Boolean CanSeek
					{
						virtual System::Boolean get() override { return this->m_innerStream->CanSeek; }
					}

					property System::Boolean CanWrite
					{
						virtual System::Boolean get() override { return this->m_innerStream->CanWrite; }
					}

					property System::Int64 Position
					{
						virtual System::Int64 get() override;
						virtual void set(System::Int64 value) override;
					}

					property System::Int64 Length
					{
						virtual System::Int64 get() override;
					}

					virtual System::Int64 Seek(System::Int64 offset, System::IO::SeekOrigin origin) override;

					virtual void SetLength(System::Int64 value) override;

					virtual System::Int32 Read(array<System::Byte> ^ buffer, System::Int32 offset, System::Int32 count) override;

					virtual void Write(array<System::Byte> ^ buffer, System::Int32 offset, System::Int32 count) override;

					virtual void Flush(void) override;

		private:	void SendBuffer(void);

					System::IO::Stream ^ m_innerStream;
					array<System::Byte> ^ m_buffer;

					System::Int32 m_bufferPos;
					System::Int32 m_bufferLength;
					System::Int64 m_streamPosBuffer;
					System::Int64 m_streamLength;
	};
}

#endif //DSBUFFEREDSTREAM_DSWRAPPER_INCLUDED