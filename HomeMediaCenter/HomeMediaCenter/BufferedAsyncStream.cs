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
        private readonly object syncObj = new object();

        private bool stopped;
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
            lock (this.syncObj)
            {
                if (this.isThreadStarted)
                    throw new Exception("BufferedAsyncStream - thread already started");

                this.stopped = false;
                this.isThreadStarted = true;

                try
                {
                    this.writerThread.Start();
                }
                catch
                {
                    this.isThreadStarted = false;
                    throw;
                }
            }
        }

        public void Stop()
        {
            lock (this.syncObj)
            {
                while (this.isThreadStarted && this.linkedData.Count > 0)
                    Monitor.Wait(this.syncObj);

                this.stopped = true;
                Monitor.PulseAll(this.syncObj);

                while (this.isThreadStarted)
                    Monitor.Wait(this.syncObj);
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
            lock (this.syncObj)
            {
                if (!this.isThreadStarted)
                    throw new Exception("BufferedAsyncStream - writer thread closed");

                //Pole o velkosti 0 znamena Flush
                this.linkedData.AddLast(new byte[0]);

                Monitor.PulseAll(this.syncObj);
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

            lock (this.syncObj)
            {
                while (this.isThreadStarted && this.linkedDataSize > this.maxDataSize)
                    Monitor.Wait(this.syncObj);

                if (!this.isThreadStarted)
                    throw new Exception("BufferedAsyncStream - writer thread closed");

                this.linkedData.AddLast(buffer);
                this.linkedDataSize += count;

                Monitor.PulseAll(this.syncObj);
            }
        }

        private void WriterLoop()
        {
            while (true)
            {
                byte[] data;

                lock (this.syncObj)
                {
                    while (this.stopped == false && this.linkedData.Count < 1)
                        Monitor.Wait(this.syncObj);

                    Monitor.PulseAll(this.syncObj);

                    if (this.stopped)
                    {
                        this.isThreadStarted = false;
                        return;
                    }

                    data = this.linkedData.First.Value;
                    this.linkedData.RemoveFirst();
                    this.linkedDataSize -= data.Length;
                }

                try
                {
                    //Pole o velkosti 0 znamena Flush inak zapis
                    if (data.Length == 0)
                        this.innerStream.Flush();
                    else
                        this.innerStream.Write(data, 0, data.Length);
                }
                catch
                {
                    lock (this.syncObj)
                    {
                        this.isThreadStarted = false;
                        Monitor.PulseAll(this.syncObj);
                    }

                    return;
                }
            }
        }

        protected override void Dispose(bool disposing)
        {
            lock (this.syncObj)
            {
                this.stopped = true;
                Monitor.PulseAll(this.syncObj);
            }

            base.Dispose(disposing);
        }
    }
}
