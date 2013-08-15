using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HomeMediaCenter
{
    public class ExceptionEventArgs : EventArgs
    {
        private Exception exception;

        public ExceptionEventArgs(Exception exception)
        {
            this.exception = exception;
        }

        public Exception Exception
        {
            get { return this.exception; }
        }
    }
}
