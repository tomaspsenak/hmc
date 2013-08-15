using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HomeMediaCenter
{
    public class SoapException : Exception
    {
        private int code;

        public SoapException(int code, string message) : base(message)
        {
            this.code = code;
        }

        public int Code
        {
            get { return this.code; }
        }
    }
}
