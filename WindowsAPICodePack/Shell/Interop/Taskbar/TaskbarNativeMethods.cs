﻿//Copyright (c) Microsoft Corporation.  All rights reserved.

using System;
using System.Runtime.InteropServices;
using Microsoft.WindowsAPICodePack.Shell;
using Microsoft.WindowsAPICodePack.Shell.PropertySystem;
using MS.WindowsAPICodePack.Internal;

namespace Microsoft.WindowsAPICodePack.Taskbar
{
    #region Enums
    public enum KnownDestinationCategory
    {
        Frequent = 1,
        Recent
    }

    public enum ShellAddToRecentDocs
    {
        Pidl = 0x1,
        PathA = 0x2,
        PathW = 0x3,
        AppIdInfo = 0x4,       // indicates the data type is a pointer to a SHARDAPPIDINFO structure
        AppIdInfoIdList = 0x5, // indicates the data type is a pointer to a SHARDAPPIDINFOIDLIST structure
        Link = 0x6,            // indicates the data type is a pointer to an IShellLink instance
        AppIdInfoLink = 0x7,   // indicates the data type is a pointer to a SHARDAPPIDINFOLINK structure 
    }

    public enum TaskbarProgressBarStatus
    {
        NoProgress = 0,
        Indeterminate = 0x1,
        Normal = 0x2,
        Error = 0x4,
        Paused = 0x8
    }

    public enum TaskbarActiveTabSetting
    {
        UseMdiThumbnail = 0x1,
        UseMdiLivePreview = 0x2
    }

    public enum ThumbButtonMask
    {
        Bitmap = 0x1,
        Icon = 0x2,
        Tooltip = 0x4,
        THB_FLAGS = 0x8
    }

    [Flags]
    public enum ThumbButtonOptions
    {
        Enabled = 0x00000000,
        Disabled = 0x00000001,
        DismissOnClick = 0x00000002,
        NoBackground = 0x00000004,
        Hidden = 0x00000008,
        NonInteractive = 0x00000010
    }

    public enum SetTabPropertiesOption
    {
        None = 0x0,
        UseAppThumbnailAlways = 0x1,
        UseAppThumbnailWhenActive = 0x2,
        UseAppPeekAlways = 0x4,
        UseAppPeekWhenActive = 0x8
    }

    #endregion

    #region Structs

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
    public struct ThumbButton
    {
        /// <summary>
        /// WPARAM value for a THUMBBUTTON being clicked.
        /// </summary>
        public const int Clicked = 0x1800;

        [MarshalAs(UnmanagedType.U4)]
        public ThumbButtonMask Mask;
        public uint Id;
        public uint Bitmap;
        public IntPtr Icon;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
        public string Tip;
        [MarshalAs(UnmanagedType.U4)]
        public ThumbButtonOptions Flags;
    }
    #endregion;

    public static class TaskbarNativeMethods
    {
        public static class TaskbarGuids
        {
            public static Guid IObjectArray = new Guid("92CA9DCD-5622-4BBA-A805-5E9F541BD8C9");
            public static Guid IUnknown = new Guid("00000000-0000-0000-C000-000000000046");
        }

        public const int WmCommand = 0x0111;

        // Register Window Message used by Shell to notify that the corresponding taskbar button has been added to the taskbar.
        public static readonly uint WmTaskbarButtonCreated = RegisterWindowMessage("TaskbarButtonCreated");

        public const uint WmDwmSendIconThumbnail = 0x0323;
        public const uint WmDwmSendIconicLivePreviewBitmap = 0x0326;

        #region Methods

        [DllImport("shell32.dll")]
        public static extern void SetCurrentProcessExplicitAppUserModelID(
            [MarshalAs(UnmanagedType.LPWStr)] string AppID);

        [DllImport("shell32.dll")]
        public static extern void GetCurrentProcessExplicitAppUserModelID(
            [Out(), MarshalAs(UnmanagedType.LPWStr)] out string AppID);

        [DllImport("shell32.dll")]
        public static extern void SHAddToRecentDocs(
            ShellAddToRecentDocs flags,
            [MarshalAs(UnmanagedType.LPWStr)] string path);

        public static void SHAddToRecentDocs(string path)
        {
            SHAddToRecentDocs(ShellAddToRecentDocs.PathW, path);
        }

        [DllImport("user32.dll", EntryPoint = "RegisterWindowMessage", SetLastError = true, CharSet = CharSet.Unicode)]
        public static extern uint RegisterWindowMessage([MarshalAs(UnmanagedType.LPWStr)] string lpString);


        [DllImport("shell32.dll")]
        public static extern int SHGetPropertyStoreForWindow(
            IntPtr hwnd,
            ref Guid iid /*IID_IPropertyStore*/,
            [Out(), MarshalAs(UnmanagedType.Interface)]out IPropertyStore propertyStore);

        /// <summary>
        /// Sets the window's application id by its window handle.
        /// </summary>
        /// <param name="hwnd">The window handle.</param>
        /// <param name="appId">The application id.</param>
        public static void SetWindowAppId(IntPtr hwnd, string appId)
        {
            SetWindowProperty(hwnd, SystemProperties.System.AppUserModel.ID, appId);
        }

        public static void SetWindowProperty(IntPtr hwnd, PropertyKey propkey, string value)
        {
            // Get the IPropertyStore for the given window handle
            IPropertyStore propStore = GetWindowPropertyStore(hwnd);

            // Set the value
            using (PropVariant pv = new PropVariant(value))
            {
                HResult result = propStore.SetValue(ref propkey, pv);
                if (!CoreErrorHelper.Succeeded(result))
                {
                    throw new ShellException(result);
                }
            }


            // Dispose the IPropertyStore and PropVariant
            Marshal.ReleaseComObject(propStore);
        }

        public static IPropertyStore GetWindowPropertyStore(IntPtr hwnd)
        {
            IPropertyStore propStore;
            Guid guid = new Guid(ShellIIDGuid.IPropertyStore);
            int rc = SHGetPropertyStoreForWindow(
                hwnd,
                ref guid,
                out propStore);
            if (rc != 0)
            {
                throw Marshal.GetExceptionForHR(rc);
            }
            return propStore;
        }

        #endregion
    }

    /// <summary>
    /// Thumbnail Alpha Types
    /// </summary>
    public enum ThumbnailAlphaType
    {
        /// <summary>
        /// Let the system decide.
        /// </summary>
        Unknown = 0,

        /// <summary>
        /// No transparency
        /// </summary>
        NoAlphaChannel = 1,

        /// <summary>
        /// Has transparency
        /// </summary>
        HasAlphaChannel = 2,
    }
}
