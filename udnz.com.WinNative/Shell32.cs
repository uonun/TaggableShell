using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using udnz.com.WinNative.Enums;
using udnz.com.WinNative.Structs;

namespace udnz.com.WinNative
{
    public class Shell32
    {
        [DllImport("Shell32.dll", CharSet = CharSet.Auto)]
        public static extern Int32 SHAppBarMessage(int dwMessage, IntPtr pData);

        [DllImport("Shell32.dll", CharSet = CharSet.Auto)]
        public static extern Int32 SHGetDesktopFolder(out IntPtr ppshf);

        [DllImport("Shell32.dll", CharSet = CharSet.Auto)]
        public static extern int SHGetSpecialFolderLocation(IntPtr handle, CSIDL nFolder, out IntPtr ppidl);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="ppidl"></param>
        /// <param name="dwFileAttributes"></param>
        /// <param name="sfi"></param>
        /// <param name="cbFileInfo"></param>
        /// <param name="uFlags"></param>
        /// <returns></returns>
        /// <remarks>x86 ONLY.</remarks>
        [DllImport("Shell32.dll")]
        public static extern IntPtr SHGetFileInfo(IntPtr ppidl, FILE_ATTRIBUTE dwFileAttributes, ref SHFILEINFO sfi, int cbFileInfo, SHGFI uFlags);

        /// <summary>   
        /// 返回系统设置的图标   
        /// </summary>   
        /// <param name="pszPath">文件路径 如果为""返回文件夹的</param>   
        /// <param name="dwFileAttributes">0</param>   
        /// <param name="sfi">结构体</param>   
        /// <param name="cbFileInfo">结构体大小</param>   
        /// <param name="uFlags">枚举类型</param>   
        /// <returns>-1失败</returns>   
        [DllImport("Shell32.dll")]
        public static extern IntPtr SHGetFileInfo(string pszPath, FILE_ATTRIBUTE dwFileAttributes, ref SHFILEINFO sfi, int cbFileInfo, SHGFI uFlags);


    }
}
