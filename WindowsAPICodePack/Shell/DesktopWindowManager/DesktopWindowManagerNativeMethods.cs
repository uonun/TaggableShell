//Copyright (c) Microsoft Corporation.  All rights reserved.

using System;
using System.Runtime.InteropServices;
using System.Security;
using Microsoft.WindowsAPICodePack.Shell.Interop;
using Microsoft.WindowsAPICodePack.Shell;

namespace MS.WindowsAPICodePack.Internal
{
    public static class DWMMessages
    {
        public const int WM_DWMCOMPOSITIONCHANGED = 0x031E;
        public const int WM_DWMNCRENDERINGCHANGED = 0x031F;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Margins
    {
        public int LeftWidth;      // width of left border that retains its size
        public int RightWidth;     // width of right border that retains its size
        public int TopHeight;      // height of top border that retains its size
        public int BottomHeight;   // height of bottom border that retains its size

        public Margins(bool fullWindow)
        {
            LeftWidth = RightWidth = TopHeight = BottomHeight = (fullWindow ? -1 : 0);
        }
    };
    
    public enum CompositionEnable
    {
        Disable = 0,
        Enable = 1
    }

    /// <summary>
    /// Internal class that contains interop declarations for 
    /// functions that are not benign and are performance critical. 
    /// </summary>
    [SuppressUnmanagedCodeSecurity]
    public static class DesktopWindowManagerNativeMethods
    {
        [DllImport("DwmApi.dll")]
        public static extern int DwmExtendFrameIntoClientArea(
            IntPtr hwnd,
            ref Margins m);

        [DllImport("DwmApi.dll", PreserveSig = false)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool DwmIsCompositionEnabled();

        [DllImport("DwmApi.dll")]
        public static extern int DwmEnableComposition(
            CompositionEnable compositionAction);

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool GetWindowRect(IntPtr hwnd, [Out] out NativeRect rect);
        
        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool GetClientRect(IntPtr hwnd, [Out] out NativeRect rect);
    }
}
