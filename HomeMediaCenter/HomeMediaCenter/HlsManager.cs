using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;
using System.Xml;

namespace HomeMediaCenter
{
    public class HlsManager
    {
        private bool started;
        private Thread collectorThread;
        private readonly object stateLock = new object();

        private int itemTimeout = 40000;
        private int timerInterval = 10000;
        private int maxPlaylistItems = 10;
        private uint defaultSegmentTime = 8;

        private readonly MediaServerDevice device;

        private readonly ReaderWriterLockSlim itemsLock = new ReaderWriterLockSlim();
        private readonly Dictionary<string, HlsItem> items = new Dictionary<string, HlsItem>(StringComparer.OrdinalIgnoreCase);

        public HlsManager(MediaServerDevice device)
        {
            this.device = device;

            device.Server.HttpServer.AddRoute("GET", "/Hls/playlist.m3u8", new HttpRouteDelegate(GetPlaylist));
            device.Server.HttpServer.AddRoute("GET", "/Hls/*", new HttpRouteDelegate(GetEncode));
        }

        public int MaxPlaylistItems
        {
            get { return this.maxPlaylistItems; }
        }

        public uint DefaultSegmentTime
        {
            get { return this.defaultSegmentTime; }
        }

        public void Start()
        {
            if (this.collectorThread == null)
            {
                lock (this.stateLock)
                {
                    this.started = true;
                }

                this.collectorThread = new Thread(new ThreadStart(ItemCollector)) { IsBackground = true };
                if (this.device.CultureInfo != null)
                    this.collectorThread.CurrentCulture = this.collectorThread.CurrentUICulture = this.device.CultureInfo;
                this.collectorThread.Start();
            }
        }

        public void Stop()
        {
            if (this.collectorThread != null)
            {
                lock (this.stateLock)
                {
                    this.started = false;
                    Monitor.Pulse(this.stateLock);
                }

                //Pocka na ukoncenie vlakna kde sa odstrania vsetky items
                this.collectorThread.Join();
                this.collectorThread = null;
            }
        }

        internal void LoadSettings(XmlDocument xmlReader)
        {
            int intVal;
            uint uintVal;

            XmlNode node = xmlReader.SelectSingleNode("/HomeMediaCenter/HlsManager/ItemTimeout");
            if (node != null && int.TryParse(node.InnerText, out intVal))
                this.itemTimeout = intVal;

            node = xmlReader.SelectSingleNode("/HomeMediaCenter/HlsManager/TimerInterval");
            if (node != null && int.TryParse(node.InnerText, out intVal))
                this.timerInterval = intVal;

            node = xmlReader.SelectSingleNode("/HomeMediaCenter/HlsManager/MaxPlaylistItems");
            if (node != null && int.TryParse(node.InnerText, out intVal))
                this.maxPlaylistItems = intVal;

            node = xmlReader.SelectSingleNode("/HomeMediaCenter/HlsManager/DefaultSegmentTime");
            if (node != null && uint.TryParse(node.InnerText, out uintVal))
                this.defaultSegmentTime = uintVal;
        }

        internal void SaveSettings(XmlWriter xmlWriter)
        {
            xmlWriter.WriteStartElement("HlsManager");

            xmlWriter.WriteElementString("ItemTimeout", this.itemTimeout.ToString());
            xmlWriter.WriteElementString("TimerInterval", this.timerInterval.ToString());
            xmlWriter.WriteElementString("MaxPlaylistItems", this.maxPlaylistItems.ToString());
            xmlWriter.WriteElementString("DefaultSegmentTime", this.defaultSegmentTime.ToString());

            xmlWriter.WriteEndElement();
        }

        private void GetPlaylist(HttpRequest request)
        {
            if (!request.UrlParams.ContainsKey("id"))
                throw new HttpException(404, "Unknown source");

            string sessionId;
            if (!request.Headers.ContainsKey("X-Playback-Session-Id"))
                throw new HttpException(403, "Unknown session id");
            sessionId = request.Headers["X-Playback-Session-Id"];
            //sessionId = Guid.Empty.ToString();

            HlsItem item;
            this.itemsLock.EnterUpgradeableReadLock();
            try
            {
                if (this.items.ContainsKey(sessionId))
                {
                    //Session id polozka uz existuje
                    item = this.items[sessionId];
                }
                else
                {
                    //Vytvorenie novej session id polozky a jej enkodera
                    TimeSpan? duration;
                    string path, subtitlesPath, encodeFeature;
                    if (!this.device.ItemManager.GetEncode(request.UrlParams["id"], out path, out subtitlesPath, out encodeFeature, out duration))
                        throw new HttpException(404, "Bad parameter");

                    request.UrlParams["codec"] = "hls";
                    request.UrlParams["source"] = path;
                    if (subtitlesPath != null)
                        request.UrlParams["subtitles"] = subtitlesPath;

                    string prefDsDemuxStr = this.device.GetPrefDsDemux();
                    if (prefDsDemuxStr != null)
                        request.UrlParams["prefDsDemux"] = prefDsDemuxStr;

                    uint segmentTime;
                    if (!request.UrlParams.ContainsKey("hlsSegmentTime") || !uint.TryParse(request.UrlParams["hlsSegmentTime"], out segmentTime))
                        request.UrlParams["hlsSegmentTime"] = this.defaultSegmentTime.ToString();

                    item = new HlsItem(this, sessionId, request.UrlParams, this.device.CultureInfo);
                    item.StartEncode();

                    this.device.OnLogEvent("Starting HLS encode (id: " + item.SessionId + ")");

                    this.itemsLock.EnterWriteLock();
                    try
                    {
                        this.items[item.SessionId] = item;
                    }
                    finally
                    {
                        this.itemsLock.ExitWriteLock();
                    }
                }
            }
            finally
            {
                this.itemsLock.ExitUpgradeableReadLock();
            }

            HttpResponse response = request.GetResponse();
            response.AddHreader(HttpHeader.ContentType, "application/x-mpegURL");
            response.SendHeaders();

            using (StreamWriter sw = new StreamWriter(response.GetStream(), Encoding.ASCII))
            {
                item.WritePlaylist(sw);
            }
        }

        private void GetEncode(HttpRequest request)
        {
            uint index;
            string[] values = request.GetFileName().Split(new char[] { '_' }, 2);
            if (values.Length < 2 || !uint.TryParse(values[0], out index))
                throw new HttpException(404, "HLS file not found");

            string sessionId;
            int extIndex = values[1].ToLower().LastIndexOf(".ts");
            if (extIndex < 1)
                throw new HttpException(404, "Unknown session id");
            sessionId = values[1].Substring(0, extIndex);

            HlsItem item;
            this.itemsLock.EnterReadLock();
            try
            {
                if (!this.items.ContainsKey(sessionId))
                    throw new HttpException(404, "Session id not found");
                item = this.items[sessionId];
            }
            finally
            {
                this.itemsLock.ExitReadLock();
            }

            HttpResponse response = request.GetResponse();
            response.AddHreader(HttpHeader.ContentType, "video/MP2T");

            if (index == 0)
            {
                //Poslanie nulteho suboru so statickym obrazom, je pripraveny hned na stiahnutie
                //Jeho dlzka je rovnaka ako dlzka segmentu
                response.SendHeaders();

                Dictionary<string, string> parameters = item.GetParamCopy();
                parameters.Remove("starttime");
                parameters.Remove("subtitles");
                parameters.Remove("id");
                parameters["source"] = "static";
                parameters["codec"] = "mpeg2_ts_h264";
                parameters["endtime"] = parameters["hlsSegmentTime"];

                EncoderBuilder encoder = EncoderBuilder.GetEncoder(parameters);
                encoder.StartEncode(response.GetStream());
            }
            else
            {
                //Posielanie segmentovanych suborov pozadovaneho zdroja
                byte[] fileData = item.GetFileData(index);

                response.AddHreader(HttpHeader.ContentLength, fileData.Length.ToString());
                response.SendHeaders();
                response.GetStream().Write(fileData, 0, fileData.Length);
            }
        }

        private void ItemCollector()
        {
            while (true)
            {
                lock (this.stateLock)
                {
                    Monitor.Wait(this.stateLock, this.timerInterval);
                    if (!this.started)
                        break;
                }

                HlsItem[] toRemove;
                this.itemsLock.EnterReadLock();
                try
                {
                    DateTime now = DateTime.Now;

                    toRemove = this.items.Values.Where(a => (now - a.LastRequest).TotalMilliseconds > this.itemTimeout).ToArray();
                    if (toRemove.Length < 1)
                        continue;
                }
                finally
                {
                    this.itemsLock.ExitReadLock();
                }

                RemoveItems(toRemove);
            }

            //Pri ukonceni vlakna odstranit vsetky items
            this.itemsLock.EnterUpgradeableReadLock();
            try
            {
                RemoveItems(this.items.Values.ToArray());
            }
            finally
            {
                this.itemsLock.ExitUpgradeableReadLock();
            }
        }

        private void RemoveItems(HlsItem[] items)
        {
            foreach (HlsItem item in items)
            {
                item.StopEncode();
            }

            foreach (HlsItem item in items)
            {
                item.JoinStopped();
                this.device.OnLogEvent("HLS encode stopped (id: " + item.SessionId + ")");
            }

            this.itemsLock.EnterWriteLock();
            try
            {
                foreach (HlsItem item in items)
                    this.items.Remove(item.SessionId);
            }
            finally
            {
                this.itemsLock.ExitWriteLock();
            }
        }
    }
}