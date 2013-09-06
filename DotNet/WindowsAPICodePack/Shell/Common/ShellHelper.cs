//Copyright (c) Microsoft Corporation.  All rights reserved.

using System;
using System.IO;
using System.Runtime.InteropServices;
using Microsoft.WindowsAPICodePack.Shell.PropertySystem;
using Microsoft.WindowsAPICodePack.Shell.Resources;
using MS.WindowsAPICodePack.Internal;

namespace Microsoft.WindowsAPICodePack.Shell
{
    /// <summary>
    /// A helper class for Shell Objects
    /// </summary>
    public static class ShellHelper
    {
        public static string GetParsingName(IShellItem shellItem)
        {
            if (shellItem == null) { return null; }

            string path = null;

            IntPtr pszPath = IntPtr.Zero;
            HResult hr = shellItem.GetDisplayName(ShellNativeMethods.ShellItemDesignNameOptions.DesktopAbsoluteParsing, out pszPath);

            if (hr != HResult.Ok && hr != HResult.InvalidArguments)
            {
                throw new ShellException(LocalizedMessages.ShellHelperGetParsingNameFailed, hr);
            }

            if (pszPath != IntPtr.Zero)
            {
                path = Marshal.PtrToStringAuto(pszPath);
                Marshal.FreeCoTaskMem(pszPath);
                pszPath = IntPtr.Zero;
            }

            return path;

        }

        public static string GetAbsolutePath(string path)
        {
            if (Uri.IsWellFormedUriString(path, UriKind.Absolute))
            {
                return path;
            }
            return Path.GetFullPath((path));
        }

        public static PropertyKey ItemTypePropertyKey = new PropertyKey(new Guid("28636AA6-953D-11D2-B5D6-00C04FD918D0"), 11);

        public static string GetItemType(IShellItem2 shellItem)
        {
            if (shellItem != null)
            {
                string itemType = null;                
                HResult hr = shellItem.GetString(ref ItemTypePropertyKey, out itemType);
                if (hr == HResult.Ok) { return itemType; }
            }

            return null;
        }

        public static IntPtr PidlFromParsingName(string name)
        {
            IntPtr pidl;

            ShellNativeMethods.ShellFileGetAttributesOptions sfgao;
            int retCode = ShellNativeMethods.SHParseDisplayName(
                name, IntPtr.Zero, out pidl, (ShellNativeMethods.ShellFileGetAttributesOptions)0,
                out sfgao);

            return (CoreErrorHelper.Succeeded(retCode) ? pidl : IntPtr.Zero);
        }

        public static IntPtr PidlFromShellItem(IShellItem nativeShellItem)
        {
            IntPtr unknown = Marshal.GetIUnknownForObject(nativeShellItem);
            return PidlFromUnknown(unknown);
        }

        public static IntPtr PidlFromUnknown(IntPtr unknown)
        {
            IntPtr pidl;
            int retCode = ShellNativeMethods.SHGetIDListFromObject(unknown, out pidl);
            return (CoreErrorHelper.Succeeded(retCode) ? pidl : IntPtr.Zero);
        }

    }
}
