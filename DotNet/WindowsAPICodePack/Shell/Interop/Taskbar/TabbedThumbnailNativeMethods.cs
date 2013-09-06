//Copyright (c) Microsoft Corporation.  All rights reserved.

using System;
using System.Runtime.InteropServices;
using MS.WindowsAPICodePack.Internal;
using Microsoft.WindowsAPICodePack.Shell.Interop;
using Microsoft.WindowsAPICodePack.Shell;

namespace Microsoft.WindowsAPICodePack.Taskbar
{
    public static class TabbedThumbnailNativeMethods
    {
        public const int DisplayFrame = 0x00000001;

        public const int ForceIconicRepresentation = 7;
        public const int HasIconicBitmap = 10;
        
        public const uint WmDwmSendIconicThumbnail = 0x0323;
        public const uint WmDwmSendIconicLivePreviewBitmap = 0x0326;

        public const uint WaActive = 1;
        public const uint WaClickActive = 2;

        public const int ScClose = 0xF060;
        public const int ScMaximize = 0xF030;
        public const int ScMinimize = 0xF020;

        public const uint MsgfltAdd = 1;
        public const uint MsgfltRemove = 2;

        [DllImport("dwmapi.dll")]
        public static extern int DwmSetIconicThumbnail(
            IntPtr hwnd, IntPtr hbitmap, uint flags);

        [DllImport("dwmapi.dll")]
        public static extern int DwmInvalidateIconicBitmaps(IntPtr hwnd);

        [DllImport("dwmapi.dll")]
        public static extern int DwmSetIconicLivePreviewBitmap(
            IntPtr hwnd,
            IntPtr hbitmap,
            ref NativePoint ptClient,
            uint flags);

        [DllImport("dwmapi.dll")]
        public static extern int DwmSetIconicLivePreviewBitmap(
            IntPtr hwnd, IntPtr hbitmap, IntPtr ptClient, uint flags);

        [DllImport("dwmapi.dll", PreserveSig = true)]
        public static extern int DwmSetWindowAttribute(
            IntPtr hwnd,
            //DWMWA_* values.
            uint dwAttributeToSet,
            IntPtr pvAttributeValue,
            uint cbAttribute);

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool GetWindowRect(IntPtr hwnd, ref NativeRect rect);

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool GetClientRect(IntPtr hwnd, ref NativeRect rect);

        public static bool GetClientSize(IntPtr hwnd, out System.Drawing.Size size)
        {
            NativeRect rect = new NativeRect();
            if (!GetClientRect(hwnd, ref rect))
            {
                size = new System.Drawing.Size(-1, -1);
                return false;
            }
            size = new System.Drawing.Size(rect.Right, rect.Bottom);
            return true;
        }

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool ClientToScreen(
            IntPtr hwnd,
            ref NativePoint point);


        [DllImport("gdi32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool StretchBlt(
            IntPtr hDestDC, int destX, int destY, int destWidth, int destHeight,
            IntPtr hSrcDC, int srcX, int srcY, int srcWidth, int srcHeight,
            uint operation);

        [DllImport("user32.dll")]
        public static extern IntPtr GetWindowDC(IntPtr hwnd);

        [DllImport("user32.dll")]
        public static extern int ReleaseDC(IntPtr hwnd, IntPtr hdc);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr ChangeWindowMessageFilter(uint message, uint dwFlag);

        /// <summary>
        /// Sets the specified iconic thumbnail for the specified window.
        /// This is typically done in response to a DWM message.
        /// </summary>
        /// <param name="hwnd">The window handle.</param>
        /// <param name="hBitmap">The thumbnail bitmap.</param>
        public static void SetIconicThumbnail(IntPtr hwnd, IntPtr hBitmap)
        {
            int rc = DwmSetIconicThumbnail(
                hwnd,
                hBitmap,
                DisplayFrame);
            if (rc != 0)
            {
                throw Marshal.GetExceptionForHR(rc);
            }
        }

        /// <summary>
        /// Sets the specified peek (live preview) bitmap for the specified
        /// window.  This is typically done in response to a DWM message.
        /// </summary>
        /// <param name="hwnd">The window handle.</param>
        /// <param name="bitmap">The thumbnail bitmap.</param>
        /// <param name="displayFrame">Whether to display a standard window
        /// frame around the bitmap.</param>
        public static void SetPeekBitmap(IntPtr hwnd, IntPtr bitmap, bool displayFrame)
        {
            int rc = DwmSetIconicLivePreviewBitmap(
                hwnd,
                bitmap,
                IntPtr.Zero,
                displayFrame ? DisplayFrame : (uint)0);
            if (rc != 0)
            {
                throw Marshal.GetExceptionForHR(rc);
            }
        }

        /// <summary>
        /// Sets the specified peek (live preview) bitmap for the specified
        /// window.  This is typically done in response to a DWM message.
        /// </summary>
        /// <param name="hwnd">The window handle.</param>
        /// <param name="bitmap">The thumbnail bitmap.</param>
        /// <param name="offset">The client area offset at which to display
        /// the specified bitmap.  The rest of the parent window will be
        /// displayed as "remembered" by the DWM.</param>
        /// <param name="displayFrame">Whether to display a standard window
        /// frame around the bitmap.</param>
        public static void SetPeekBitmap(IntPtr hwnd, IntPtr bitmap, System.Drawing.Point offset, bool displayFrame)
        {
            var nativePoint = new NativePoint(offset.X, offset.Y);
            int rc = DwmSetIconicLivePreviewBitmap(
                hwnd,
                bitmap,
                ref nativePoint,
                displayFrame ? DisplayFrame : (uint)0);

            if (rc != 0)
            {
                Exception e = Marshal.GetExceptionForHR(rc);

                if (e is ArgumentException)
                {
                    // Ignore argument exception as it's not really recommended to be throwing
                    // exception when rendering the peek bitmap. If it's some other kind of exception,
                    // then throw it.
                }
                else
                {
                    throw e;
                }
            }
        }

        /// <summary>
        /// Call this method to either enable custom previews on the taskbar (second argument as true)
        /// or to disable (second argument as false). If called with True, the method will call DwmSetWindowAttribute
        /// for the specific window handle and let DWM know that we will be providing a custom bitmap for the thumbnail
        /// as well as Aero peek.
        /// </summary>
        /// <param name="hwnd"></param>
        /// <param name="enable"></param>
        public static void EnableCustomWindowPreview(IntPtr hwnd, bool enable)
        {
            IntPtr t = Marshal.AllocHGlobal(4);
            Marshal.WriteInt32(t, enable ? 1 : 0);

            try
            {
                int rc = DwmSetWindowAttribute(hwnd, HasIconicBitmap, t, 4);
                if (rc != 0)
                {
                    throw Marshal.GetExceptionForHR(rc);
                }

                rc = DwmSetWindowAttribute(hwnd, ForceIconicRepresentation, t, 4);
                if (rc != 0)
                {
                    throw Marshal.GetExceptionForHR(rc);
                }
            }
            finally
            {
                Marshal.FreeHGlobal(t);
            }
        }

    }
}
