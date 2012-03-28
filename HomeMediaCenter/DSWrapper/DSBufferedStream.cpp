#include "StdAfx.h"
#include "DSBufferedStream.h"

namespace DSWrapper 
{
	DSBufferedStream::DSBufferedStream(System::IO::Stream ^ innerStream, System::Int32 length) : m_innerStream(innerStream), m_bufferPos(0),
		m_bufferLength(0)
	{
		this->m_buffer = gcnew array<System::Byte>(length);

		try
		{
			this->m_streamPosBuffer = innerStream->Position;
		}
		catch (System::Exception^)
		{
			this->m_streamPosBuffer = 0;
		}

		try
		{
			this->m_streamLength = innerStream->Length;
		}
		catch (System::Exception^)
		{
			this->m_streamLength = 0;
		}
	}

	System::Int64 DSBufferedStream::Position::get()
	{
		return this->m_streamPosBuffer + this->m_bufferPos;
	}

	void DSBufferedStream::Position::set(System::Int64 value)
	{
		Seek(value, System::IO::SeekOrigin::Begin);
	}

	System::Int64 DSBufferedStream::Length::get()
	{
		return this->m_streamLength + this->m_bufferLength;
	}

	System::Int64 DSBufferedStream::Seek(System::Int64 offset, System::IO::SeekOrigin origin)
	{
		switch (origin)
		{
			case System::IO::SeekOrigin::Begin:
				if (offset <= this->m_streamLength)
				{
					//Nastavenie pozicie pred buffer (pred uz poslane data)
					//Musi byt aj rovna sa - po navrate zo seeku vzadu na koniec streamu
					//sa musi nastavit aj pozicia buffera
					if (this->m_bufferLength > 0)
							SendBuffer();

					this->m_streamPosBuffer = offset;
				}
				else
				{
					if (offset >= this->m_streamPosBuffer && offset < (this->m_streamPosBuffer + this->m_buffer->Length))
					{
						//Pozadovana pozicia sa nachadza v ramci buffera (este neodoslene data)
						this->m_bufferPos = (System::Int32)(offset - this->m_streamPosBuffer);
					}
					else
					{
						//Pozicia je za bufferom, posle sa buffer a do pozadovanej pozicie sa posielaju nuly
						if (this->m_bufferLength > 0)
							SendBuffer();

						SetLength(offset);
					}
				}
				break;
			case System::IO::SeekOrigin::Current:
				return Seek(Position + offset, System::IO::SeekOrigin::Begin);
			case System::IO::SeekOrigin::End:
				return Seek(Length + offset, System::IO::SeekOrigin::Begin);
		}

		return Position;
	}

	void DSBufferedStream::SetLength(System::Int64 value)
	{
		if (this->m_bufferLength > 0)
			SendBuffer();

		if (value < this->m_streamLength)
		{
			//Stream sa zmensuje
			this->m_innerStream->SetLength(value);
			this->m_innerStream->Seek(value, System::IO::SeekOrigin::Begin);
			this->m_streamLength = value;
		}
		else
		{
			//Do pozadovanej pozicie sa posielaju nuly
			for (; this->m_streamLength < value; this->m_streamLength++)
				this->m_innerStream->WriteByte(0);
		}
		this->m_streamPosBuffer = value;
	}

	System::Int32 DSBufferedStream::Read(array<System::Byte> ^ buffer, System::Int32 offset, System::Int32 count)
	{
		//Ak sa da nacitat z buffera, nacita data
		System::Int32 countRead = System::Math::Min(count, this->m_bufferLength - this->m_bufferPos);
		for (System::Int32 i = 0; i < countRead; i++, offset++, this->m_bufferPos++)
			buffer[offset] = this->m_buffer[this->m_bufferPos];
		count -= countRead;

		if (countRead > 0)
			return countRead;

		//Ak sa neda nacitat, vrati 0
		return 0;
	}

	void DSBufferedStream::Write(array<System::Byte> ^ buffer, System::Int32 offset, System::Int32 count)
	{
		if (this->m_streamPosBuffer < this->m_streamLength)
		{
			//Po seekovani dozadu sa data nezapisu
			this->m_streamPosBuffer += count;
			return;
		}

		//Zapise do buffera kolko sa da
		System::Int32 countWrite = System::Math::Min(count, this->m_buffer->Length - this->m_bufferPos);
		for (System::Int32 i = 0; i < countWrite; i++, offset++, this->m_bufferPos++)
			this->m_buffer[this->m_bufferPos] = buffer[offset];
		count -= countWrite;
		if (this->m_bufferPos > this->m_bufferLength)
			this->m_bufferLength = this->m_bufferPos;

		//Ak je plny buffer, posle ho
		if (this->m_bufferLength >= this->m_buffer->Length)
			SendBuffer();

		//Ak je velkost dat na zapisanie vacsia, poslu sa priamo na stream
		countWrite = (count / this->m_buffer->Length) * this->m_buffer->Length;
		if (countWrite > 0)
		{
			this->m_innerStream->Write(buffer, offset, countWrite);
			this->m_streamPosBuffer += countWrite;
			offset += countWrite;
			count -= countWrite;

			if (this->m_streamPosBuffer > this->m_streamLength)
				this->m_streamLength = this->m_streamPosBuffer;
		}

		//Zvysok sa zapise do buffera
		for (System::Int32 i = 0; i < count; i++, offset++, this->m_bufferPos++)
			this->m_buffer[this->m_bufferPos] = buffer[offset];
		if (this->m_bufferPos > this->m_bufferLength)
			this->m_bufferLength = this->m_bufferPos;
	}

	void DSBufferedStream::Flush(void)
	{
		if (this->m_bufferLength > 0)
			SendBuffer();
		this->m_innerStream->Flush();
	}

	void DSBufferedStream::SendBuffer(void)
	{
		this->m_innerStream->Write(this->m_buffer, 0, this->m_bufferLength);
		System::Array::Clear(this->m_buffer, 0, this->m_bufferLength);
		this->m_streamPosBuffer += this->m_bufferLength;
		this->m_bufferPos = 0;
		this->m_bufferLength = 0;

		if (this->m_streamPosBuffer > this->m_streamLength)
			this->m_streamLength = this->m_streamPosBuffer;
	}
}