using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace udnz.com.WinNative
{
    public class Kernel32
    {
        /// <summary>
        /// ms-help://MS.MSDNQTR.v90.chs/debug/base/system_error_codes.htm
        /// </summary>
        /// <returns></returns>
        [DllImport("Kernel32.dll")]
        public extern static uint GetLastError();
        // 1008 - An attempt was made to reference a token that does not exist
    }
}
