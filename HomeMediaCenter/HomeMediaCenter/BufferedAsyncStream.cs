using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;

namespace HomeMediaCenter
{
    public class BufferedAsyncStream : Stream
    {
        private readonly int maxDataSize;
        private readonly Stream innerStream;
        private readonly Thread writerThread;

        private int linkedDataSize;
        private bool isThreadStarted = false;
        private LinkedList<byte[]> linkedData = new LinkedList<byte[]>();

        public BufferedAsyncStream(Stream innerStream, int size)
        {
            this.innerStream = innerStream;
            this.maxDataSize = size;
            this.writerThread = new Thread(new ThreadStart(WriterLoop)) { IsBackground = true };
        }

        ~BufferedAsyncStream()
        {
            Dispose(false);
        }

        public void Start()
        {
            lock (this.linkedData)
            {
                if (this.isThreadStarted)
                    throw new Exception("BufferedAsyncStream - thread already started");

                this.writerThread.Start();
                this.isThreadStarted = true;
            }
        }

        public override bool CanRead
        {
            get { return false; }
        }

        public override bool CanSeek
        {
            get { return false; }
        }

        public override bool CanWrite
        {
            get { return this.innerStream.CanWrite; }
        }

        public override void Flush()
        {
            while (true)
            {
                lock (this.linkedData)
                {
                    if (!this.isThreadStarted || this.linkedData.Count == 0)
                    {
                        this.innerStream.Flush();
                        return;
                    }

                    Monitor.Wait(this.linkedData);
                }
            }
        }

        public override long Length
        {
            get { throw new NotImplementedException(); }
        }

        public override long Position
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }

        public override int Read(byte[] buffer, int offset, int count)
        {
            throw new NotImplementedException();
        }

        public override long Seek(long offset, SeekOrigin origin)
        {
            throw new NotImplementedException();
        }

        public override void SetLength(long value)
        {
            throw new NotImplementedException();
        }

        public override void Write(byte[] buffer, int offset, int count)
        {
            if (offset != 0 || count != buffer.Length)
            {
                byte[] tmpBuffer = new byte[count];

                Array.Copy(buffer, offset, tmpBuffer, 0, count);
                buffer = tmpBuffer;
            }

            lock (this.linkedData)
            {
                if (this.linkedDataSize > this.maxDataSize)
                    Monitor.Wait(this.linkedData);

                if (!this.isThreadStarted)
                    throw new Exception("BufferedAsyncStream - writer thread closed");

                this.linkedData.AddLast(buffer);
                this.linkedDataSize += count;

                Monitor.Pulse(this.linkedData);
            }
        }

        private void WriterLoop()
        {
            while (true)
            {
                byte[] data;

                lock (this.linkedData)
                {
                    if (this.linkedData.Count < 1)
                        Monitor.Wait(this.linkedData);

                    if (this.linkedData.Count < 1)
                    {
                        this.isThreadStarted = false;
                        Monitor.Pulse(this.linkedData);

                        return;
                    }

                    data = this.linkedData.First.Value;
                    this.linkedData.RemoveFirst();
                    this.linkedDataSize -= data.Length;
                }

                try
                {
                    this.innerStream.Write(data, 0, data.Length);
                }
                catch
                {
                    lock (this.linkedData)
                    {
                        this.isThreadStarted = false;
                        Monitor.Pulse(this.linkedData);
                    }

                    return;
                }

                lock (this.linkedData)
                {
                    Monitor.Pulse(this.linkedData);
                }
            }
        }

        protected override void Dispose(bool disposing)
        {
            lock (this.linkedData)
            {
                this.linkedData.Clear();
                Monitor.Pulse(this.linkedData);
            }

            base.Dispose(disposing);
        }
    }
}
