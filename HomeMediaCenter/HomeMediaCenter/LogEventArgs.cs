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
        private LogEventType type;

        public LogEventArgs(string message)
        {
            this.message = message;
            this.type = LogEventType.Message;
        }

        public LogEventArgs(int requestCount)
        {
            this.message = requestCount.ToString();
            this.type = LogEventType.RequestCount;
        }

        public string Message
        {
            get { return this.message; }
        }

        public LogEventType Type
        {
            get { return this.type; }
        }
    }
}
