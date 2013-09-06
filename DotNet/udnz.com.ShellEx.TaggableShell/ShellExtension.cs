using System;
using System.Collections.Generic;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Text;
using udnz.com.WinNative;
using udnz.com.WinNative.Enums;
using udnz.com.WinNative.Interfaces;
using udnz.com.WinNative.Structs;

namespace udnz.com.ShellEx.TaggableShell
{
    public class ShellExtension
    {
        public const int MAX_PATH = 260;

        /// <summary>
        /// 获取桌面 Shell
        /// </summary>
        /// <param name="ppshf"></param>
        /// <returns></returns>
        public static IShellFolder GetDesktopFolder(out IntPtr ppshf)
        {
            Shell32.SHGetDesktopFolder(out ppshf);
            Object obj = Marshal.GetObjectForIUnknown(ppshf);
            return (IShellFolder)obj;
        }

        /// <summary>
        /// 获取显示名称
        /// </summary>
        public static string GetNameByIShell(IShellFolder Root, IntPtr pidlSub)
        {
            IntPtr strr = Marshal.AllocCoTaskMem(MAX_PATH * 2 + 4);
            Marshal.WriteInt32(strr, 0, 0);
            StringBuilder buf = new StringBuilder(MAX_PATH);
            Root.GetDisplayNameOf(pidlSub, SHGNO.INFOLDER, strr);
            Shlwapi.StrRetToBuf(strr, pidlSub, buf, MAX_PATH);
            return buf.ToString();
        }

        /// <summary>
        /// 根据 PIDL 获取显示名称.
        /// x86 ONLY.
        /// </summary>
        public static string GetNameByPIDL(IntPtr pidl)
        {
            SHFILEINFO info = new SHFILEINFO();
            Shell32.SHGetFileInfo(pidl, 0, ref info, Marshal.SizeOf(typeof(SHFILEINFO)), SHGFI.PIDL | SHGFI.DISPLAYNAME | SHGFI.ICONLOCATION);
            return info.szDisplayName;
        }

        /// <summary>   
        /// 获取文件图标
        /// </summary>   
        /// <param name="p_Path">文件全路径</param>   
        /// <returns>图标</returns>   
        public static Icon GetFileIcon(string p_Path)
        {
            SHFILEINFO _shfileinfo = new SHFILEINFO();
            IntPtr _iconIntPtr = Shell32.SHGetFileInfo(p_Path, 0, ref _shfileinfo, Marshal.SizeOf(_shfileinfo), SHGFI.ICON | SHGFI.LARGEICON | SHGFI.USEFILEATTRIBUTES);
            if (_iconIntPtr.Equals(IntPtr.Zero)) return null;
            Icon _icon = System.Drawing.Icon.FromHandle(_shfileinfo.hIcon);
            return _icon;
        }

        /// <summary>   
        /// 获取文件夹图标
        /// </summary>   
        /// <returns>图标</returns>   
        public static Icon GetDirectoryIcon()
        {
            SHFILEINFO _shfileinfo = new SHFILEINFO();
            IntPtr _iconIntPtr = Shell32.SHGetFileInfo(@"", 0, ref _shfileinfo, Marshal.SizeOf(_shfileinfo), SHGFI.ICON | SHGFI.LARGEICON);
            if (_iconIntPtr.Equals(IntPtr.Zero)) return null;
            Icon _icon = System.Drawing.Icon.FromHandle(_shfileinfo.hIcon);
            return _icon;
        }
    }
}
