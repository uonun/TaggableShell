using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace udnz.com.WinNative
{
    public class Shlwapi
    {
        [DllImport("Shlwapi.Dll", CharSet = CharSet.Auto)]
        public static extern Int32 StrRetToBuf(IntPtr pstr, IntPtr pidl, StringBuilder pszBuf, int cchBuf);
    }
}
