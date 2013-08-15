using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Net.Sockets;

namespace HomeMediaCenter
{
    public class ChunkedStream : Stream
    {
        private NetworkStream innerStream;
        private int chunkSize;
        private bool canRead;

        public ChunkedStream(NetworkStream innerStream, bool canRead)
        {
            this.innerStream = innerStream;
            this.canRead = canRead;
        }

        public override bool CanRead
        {
            get { return this.canRead; }
        }

        public override bool CanSeek
        {
            get { return false; }
        }

        public override bool CanWrite
        {
            get { return !this.canRead; }
        }

        public override bool CanTimeout
        {
            get { return this.innerStream.CanTimeout; }
        }

        public override void Flush()
        {
            this.innerStream.Flush();
        }

        public override long Length
        {
            get { throw new NotSupportedException(); }
        }

        public override long Position
        {
            get { throw new NotSupportedException(); }
            set { throw new NotSupportedException(); }
        }

        public override int Read(byte[] buffer, int offset, int count)
        {
            if (!this.canRead)
                throw new NotSupportedException();

            if (chunkSize > 0)
            {
                //Hlavicka s informaciou o velkosti casti bola prijata
                int minCount = Math.Min(count, this.chunkSize);
                int readed = this.innerStream.Read(buffer, offset, minCount);
                this.chunkSize -= readed;

                if (this.chunkSize == 0)
                {
                    //Vynechaj konecne \r\n casti
                    this.innerStream.ReadByte();
                    this.innerStream.ReadByte();
                }

                return readed;
            }
            else
            {
                //Hlavicka casti este nebola prijata
                StringBuilder sb = new StringBuilder(128);
                bool isLastR = false;
                int iByte;

                while ((iByte = this.innerStream.ReadByte()) >= 0)
                {
                    if (iByte == '\n' && isLastR)
                    {
                        //Z hlavicky casti zisti velkost v hexadecimalnom tvare
                        int index;
                        string line = sb.ToString(0, sb.Length - 1);
                        
                        //V hlavicke mozu but aj volitelne parametre oddelene ; ale ignoruju sa
                        if ((index = line.IndexOf(';')) > 0)
                            line = line.Substring(0, index).Trim();

                        this.chunkSize = int.Parse(line, System.Globalization.NumberStyles.HexNumber);

                        if (this.chunkSize > 0)
                        {
                            return Read(buffer, offset, count);
                        }
                        else
                        {
                            this.chunkSize = 0;
                            return 0;
                        }
                    }
                    else if (iByte == '\r')
                    {
                        sb.Append((char)iByte);
                        isLastR = true;
                    }
                    else
                    {
                        sb.Append((char)iByte);
                        isLastR = false;
                    }
                }

                throw new HttpException(400, "Bad chunk header");
            }
        }

        public override long Seek(long offset, SeekOrigin origin)
        {
            throw new NotSupportedException();
        }

        public override void SetLength(long value)
        {
            throw new NotSupportedException();
        }

        public override void Write(byte[] buffer, int offset, int count)
        {
            if (this.canRead)
                throw new NotSupportedException();

            byte[] header = Encoding.ASCII.GetBytes(string.Format("{0:X}\r\n", count));

            this.innerStream.Write(header, 0, header.Length);
            this.innerStream.Write(buffer, offset, count);
            this.innerStream.Write(new byte[] { 13, 10 }, 0, 2);
            this.innerStream.Flush();
        }

        public override void Close()
        {
            if (!this.canRead)
                this.innerStream.Write(new byte[] { 0, 13, 10 }, 0, 3);

            this.innerStream.Close();
        }
    }
}
