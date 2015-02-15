using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO.Pipes;
using System.IO;
using System.Globalization;

namespace HomeMediaCenter
{
    public class HlsItem
    {
        private readonly string sessionId;
        private readonly HlsManager manager;
        private readonly EncoderBuilder encoder;
        private readonly object objLock = new object();
        private readonly object filePipeLock = new object();

        private bool stopped;
        private uint segmentTime;
        private uint nextFilePipeIdx;
        private uint mediaSequence;
        private DateTime lastRequest;
        private uint lastPlaylistIdx;

        private NamedPipeServerStream filePipe;
        private readonly NamedPipeServerStream playlistPipe;

        private readonly Thread encoderThread;
        private readonly Thread filePipeThread;
        private readonly Thread playlistPipeThread;

        private readonly Dictionary<uint, StreamItem> streamItems = new Dictionary<uint, StreamItem>();
        private class StreamItem
        {
            public double? Duration
            {
                get; set;
            }

            public byte[] FileData
            {
                get; set;
            }
        }

        private const uint KeepUsedFiles = 1;

        public HlsItem(HlsManager manager, string sessionId, Dictionary<string, string> parameters, CultureInfo cultureInfo)
        {
            this.sessionId = sessionId;
            this.manager = manager;

            this.segmentTime = uint.Parse(parameters["hlsSegmentTime"]);

            this.streamItems.Add(0, new StreamItem() { Duration = this.segmentTime });

            string playlistPipeName = "HMC_" + sessionId;
            parameters["hlsPlaylistUrl"] = "\\\\.\\pipe\\" + playlistPipeName;
            this.playlistPipe = CreatePipe(playlistPipeName);

            this.filePipe = CreatePipe("HMC_" + (this.nextFilePipeIdx++) + "_" + this.sessionId);
            parameters["hlsFileUrl"] = "\\\\.\\pipe\\HMC_%d_" + sessionId;

            this.encoder = EncoderBuilder.GetEncoder(parameters);

            this.playlistPipeThread = new Thread(new ThreadStart(ListenPlaylistPipe));
            this.filePipeThread = new Thread(new ThreadStart(ListenFilePipe));
            this.encoderThread = new Thread(new ThreadStart(Encoder));
            if (cultureInfo != null)
            {
                this.playlistPipeThread.CurrentCulture = this.playlistPipeThread.CurrentUICulture = cultureInfo;
                this.filePipeThread.CurrentCulture = this.filePipeThread.CurrentUICulture = cultureInfo;
                this.encoderThread.CurrentCulture = this.encoderThread.CurrentUICulture = cultureInfo;
            }

            this.lastRequest = DateTime.Now;
        }

        public string SessionId
        {
            get { return this.sessionId; }
        }

        public DateTime LastRequest
        {
            get { return this.lastRequest; }
        }

        public Dictionary<string, string> GetParamCopy()
        {
            return this.encoder.GetParamCopy();
        }

        public void StartEncode()
        {
            this.playlistPipeThread.Start();
            this.filePipeThread.Start();
            this.encoderThread.Start();
        }

        public void StopEncode()
        {
            lock (this.objLock)
            {
                this.stopped = true;
                Monitor.PulseAll(this.objLock);
            }

            this.encoder.StopEncode();
        }

        public void JoinStopped()
        {
            this.encoderThread.Join();

            this.playlistPipe.Dispose();

            lock (this.filePipeLock)
            {
                this.filePipe.Dispose();
                //filePipe nastavit na null aby sa v ListenFilePipe nevytvorila dalsia pipe
                this.filePipe = null;
            }

            this.playlistPipeThread.Join();
            this.filePipeThread.Join();
        }

        public void WritePlaylist(StreamWriter sw)
        {
            this.lastRequest = DateTime.Now;

            sw.WriteLine("#EXTM3U");
            sw.WriteLine("#EXT-X-VERSION:3");
            sw.WriteLine("#EXT-X-ALLOW-CACHE:NO");
            sw.WriteLine("#EXT-X-TARGETDURATION:" + this.segmentTime);
            sw.Flush();

            lock (this.objLock)
            {
                while (this.stopped == false && this.lastPlaylistIdx == this.streamItems.Keys.Max())
                    Monitor.Wait(this.objLock);

                sw.WriteLine("#EXT-X-MEDIA-SEQUENCE:" + this.mediaSequence);

                foreach (uint key in this.streamItems.Keys.OrderBy(a => a))
                {
                    StreamItem item = this.streamItems[key];
                    if (!item.Duration.HasValue || (item.FileData == null && key != 0))
                        break;

                    sw.WriteLine(string.Format(System.Globalization.CultureInfo.InvariantCulture, "#EXTINF:{0:0.000000},", item.Duration));
                    sw.WriteLine(string.Format("{0}_{1}.ts", key, this.sessionId));

                    this.lastPlaylistIdx = Math.Max(this.lastPlaylistIdx, key);
                }

                if (this.stopped)
                    sw.WriteLine("#EXT-X-ENDLIST");
            }
        }

        public byte[] GetFileData(uint index)
        {
            this.lastRequest = DateTime.Now;

            lock (this.objLock)
            {
                if (!this.streamItems.ContainsKey(index))
                    throw new HttpException(404, "HLS file number " + index + " not found");

                StreamItem item = this.streamItems[index];
                if (item.FileData == null)
                    throw new HttpException(404, "No data in HLS file number " + index);

                if (index > KeepUsedFiles)
                {
                    //Vymaze subory starsie ako je aktualny index, ich pocet udava KeepUsedFiles, nezaratava sa aktualny
                    uint[] toRemove = this.streamItems.Keys.Where(a => a < (index - KeepUsedFiles)).ToArray();
                    if (toRemove.Length > 0)
                    {
                        foreach (uint key in toRemove)
                            this.streamItems.Remove(key);
                        this.mediaSequence += (uint)toRemove.Length;
                        Monitor.PulseAll(this.objLock);
                    }
                }

                return item.FileData;
            }
        }

        private void Encoder()
        {
            try
            {
                this.encoder.StartEncode(null);
            }
            catch
            { }

            lock (this.objLock)
            {
                //Netreba volat pulse - neskor sa zavola StopEncode
                this.stopped = true;
            }
        }

        private void ListenPlaylistPipe()
        {
            string prevMessage = string.Empty;
            byte[] buffer = new byte[2048];
            int readed;

            try
            {
                this.playlistPipe.WaitForConnection();

                while ((readed = this.playlistPipe.Read(buffer, 0, buffer.Length)) > 0)
                {
                    string message = Encoding.ASCII.GetString(buffer, 0, readed) + prevMessage;

                    string[] lines = message.Split(new char[] { '\n' }, StringSplitOptions.None);

                    //Ak nie je dokonceny dalsi riadok, spracuj ho pri dalsej iteracii
                    if (lines[lines.Length - 1] == string.Empty)
                        prevMessage = string.Empty;
                    else
                        prevMessage = lines[lines.Length - 1];

                    if (lines.Length < 2)
                        continue;

                    foreach (string line in lines)
                    {
                        string[] values = line.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                        if (values.Length < 3)
                            continue;

                        double start;
                        if (!double.TryParse(values[1], System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture, out start))
                            continue;

                        double end;
                        if (!double.TryParse(values[2], System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture, out end))
                            continue;

                        string[] nameSplit = values[0].Split(new char[] { '_' }, 3, StringSplitOptions.RemoveEmptyEntries);
                        if (nameSplit.Length != 3)
                            continue;

                        uint nameIdx;
                        if (!uint.TryParse(nameSplit[1], out nameIdx))
                            continue;

                        AddStreamItem(nameIdx, end - start, null);
                    }
                }
            }
            catch (ObjectDisposedException)
            { }
            catch (IOException)
            { }

            //Netreba nastavovat this.stopped = true lebo encoder.StartEncode skonci a vtedy sa nastavi
        }

        private void ListenFilePipe()
        {
            NamedPipeServerStream nextFilePipe = this.filePipe, mainFilePipe;
            byte[] buffer = new byte[8192];
            int readed;
            uint index;

            while (true)
            {
                index = this.nextFilePipeIdx++;

                lock (this.filePipeLock)
                {
                    //Ak je filePipe null, nevytvarat dalsiu a ukoncit vlakno
                    if (this.filePipe == null)
                        break;
                    this.filePipe = nextFilePipe;
                    //Spravit lokalny pointer, this.filePipe moze byt null a napr. WaitForConnection padne, disposed moze byt
                    mainFilePipe = nextFilePipe;
                }

                lock (this.objLock)
                {
                    while (this.stopped == false && this.streamItems.Count > this.manager.MaxPlaylistItems)
                        Monitor.Wait(this.objLock);
                }

                try
                {
                    //Vytvorit dopredu pipe aby bola k dispozicii hned po skonceni nextFilePipe
                    nextFilePipe = CreatePipe("HMC_" + index + "_" + this.sessionId);

                    mainFilePipe.WaitForConnection();

                    using (MemoryStream memStream = new MemoryStream())
                    {
                        while ((readed = mainFilePipe.Read(buffer, 0, buffer.Length)) > 0)
                        {
                            memStream.Write(buffer, 0, readed);
                        }

                        AddStreamItem(index - 1, null, memStream.ToArray());
                    }

                    mainFilePipe.Dispose();
                }
                catch (ObjectDisposedException)
                {
                    break;
                }
                catch (IOException)
                {
                    break;
                }
            }

            try
            {
                //Moze byt aj null, napr. ak je na zaciatku inicializovane na null
                nextFilePipe.Dispose();
            }
            catch { }
        }

        private void AddStreamItem(uint index, double? duration, byte[] fileData)
        {
            //Cislovanie zacina od 1, pre nulty prvok je vyhradeny staticky obraz
            index++;

            lock (this.objLock)
            {
                StreamItem item;
                if (this.streamItems.ContainsKey(index))
                {
                    item = this.streamItems[index];
                    if (duration.HasValue)
                        item.Duration = duration;
                    if (fileData != null)
                        item.FileData = fileData;
                }
                else
                {
                    item = new StreamItem() { Duration = duration, FileData = fileData };
                    this.streamItems[index] = item;
                }

                if (item.Duration.HasValue && item.FileData != null)
                    Monitor.PulseAll(this.objLock);
            }
        }

        private static NamedPipeServerStream CreatePipe(string name)
        {
            //Nastavit PipeOptions.Asynchronous, inak WaitForConnection sa neukonci pocas Dispose
            return new NamedPipeServerStream(name, PipeDirection.In, 1, PipeTransmissionMode.Byte, PipeOptions.Asynchronous, 0, 0);
        }
    }
}
