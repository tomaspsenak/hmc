using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HomeMediaCenter
{
    public enum LogEventType { Message, RequestCount };

    public class LogEventArgs : EventArgs
    {
        private string message;
        private int requestCount;
        private LogEventType type;

        public LogEventArgs(string message)
        {
            this.message = message;
            this.type = LogEventType.Message;
        }

        public LogEventArgs(int requestCount)
        {
            this.requestCount = requestCount;
            this.type = LogEventType.RequestCount;
        }

        public string Message
        {
            get
            {
                if (this.type == LogEventType.RequestCount)
                    return this.requestCount.ToString();
                else
                    return this.message;
            }
        }

        public int RequestCount
        {
            get { return this.requestCount; }
        }

        public LogEventType Type
        {
            get { return this.type; }
        }
    }
}
