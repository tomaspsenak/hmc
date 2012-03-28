using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HomeMediaCenter
{
    public class MediaCenterException :Exception
    {
        public MediaCenterException(string message) : base(message) { }
    }
}
