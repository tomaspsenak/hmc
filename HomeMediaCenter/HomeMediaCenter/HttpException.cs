using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HomeMediaCenter
{
    public class HttpException : Exception
    {
        private int code;

        public HttpException(int code, string message) : base(message)
        {
            this.code = code;
        }

        public int Code
        {
            get { return this.code; }
        }
    }
}
