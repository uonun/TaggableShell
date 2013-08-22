using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace udnz.com.WinNative.Interfaces
{
    [ComImport(),
    Guid("000214F2-0000-0000-C000-000000000046"),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IEnumIDList
    {
        [PreserveSig()]
        uint Next(
            uint celt,
            out IntPtr rgelt,
            out int pceltFetched);

        void Skip(
            uint celt);

        void Reset();

        IEnumIDList Clone();
    }
}
