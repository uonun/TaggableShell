using System;
using System.Collections.Generic;
using Drawing = System.Drawing;
using System.Runtime.InteropServices;
using System.Text;
using udnz.com.WinNative.Enums;
using udnz.com.WinNative.Structs;

namespace udnz.com.WinNative
{
    public class User32
    {
        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr SendMessage(IntPtr hWnd, UInt32 Msg, IntPtr wParam, IntPtr lParam);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern int ReleaseDC(IntPtr hWnd, IntPtr hDC);

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool RedrawWindow(IntPtr hWnd, IntPtr lprcUpdate, IntPtr hrgnUpdate, uint flags);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern int GetWindowLong(IntPtr hWnd, int nIndex);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr GetWindowLongPtr(IntPtr hWnd, int nIndex);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern int SetWindowLong(IntPtr hWnd, SetWindowLongIndex nIndex, int dwNewLong);

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int x, int y, int cx, int cy, SWPFlags uFlags);

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool GetWindowRect(IntPtr hWnd, ref RECT lpRect);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr GetDCEx(IntPtr hWnd, IntPtr hrgnClip, DCXFlags flags);

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool UpdateLayeredWindow(IntPtr hwnd, IntPtr hdcDst, ref Point pptDst, ref Size psize, IntPtr hdcSrc, ref Point pprSrc, Int32 crKey, ref BLENDFUNCTION pblend, UpdateLayeredWindowFlag dwFlags);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr GetDC(IntPtr hWnd);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern bool ShowWindow(HandleRef hWnd, int nCmdShow);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr GetWindow(IntPtr hwnd, GetWindowCmd uCmd);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern int GetClassName(IntPtr pWnd, char[] className, int nMaxCount);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern int GetClientRect(IntPtr hwnd, ref RECT lpRect);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern int GetClientRect(IntPtr hwnd, [In, Out] ref Drawing.Rectangle rect);

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool InvalidateRect(IntPtr hwnd, ref Drawing.Rectangle rect, bool bErase);

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool ValidateRect(IntPtr hwnd, ref Drawing.Rectangle rect);

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool IsWindowVisible(IntPtr hwnd);

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool SystemParametersInfo(SystemParametersInfoAction uiAction, uint uiParam, IntPtr pvParam, uint fWinIni);

        /// <summary>
        /// uiAction = SystemParametersInfoAction.SPI_GETANIMATION 时的重载
        /// </summary>
        /// <param name="uiAction">必须为 SystemParametersInfoAction.SPI_GETANIMATION，否则请使用其他重载</param>
        /// <param name="uiParam"></param>
        /// <param name="pvParam"></param>
        /// <param name="fWinIni"></param>
        /// <returns></returns>
        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool SystemParametersInfo(SystemParametersInfoAction uiAction, uint uiParam, ref ANIMATIONINFO pvParam, uint fWinIni);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr CreatePopupMenu();

        [DllImport("user32.dll", SetLastError = true)]
        public static extern uint TrackPopupMenuEx(IntPtr hmenu, TPM flags, int x, int y, IntPtr hwnd, IntPtr lptpm);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern MessageBoxCommandID MessageBox(IntPtr hwnd, string lpText, string lpCaption, MessageBoxFlag uType);
    }
}
