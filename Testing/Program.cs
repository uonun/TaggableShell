using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using Microsoft.WindowsAPICodePack.Shell;
using udnz.com.ShellEx.TaggableShell;
using udnz.com.WinNative;
using udnz.com.WinNative.Enums;
//using udnz.com.WinNative.Interfaces;
using udnz.com.WinNative.Structs;

namespace Testing
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.BufferWidth = 120;
            Console.WindowWidth = 120;

            //IntPtr desktopP;
            //var desktop = ShellExtension.GetDesktopFolder(out desktopP);

            //GetChilds(desktop);

            ShellLibrary sl = new ShellLibrary("MyShellLib2", Microsoft.WindowsAPICodePack.Shell.KnownFolders.DeviceMetadataStore, true);
            sl.LibraryType = LibraryFolderType.Music;

            ListFolders();

            string extName = "我的扩展";
            var guid = new Guid("00000ABC-0000-0000-0000-5D45B06CF3C9");
            var computerGuid = new Guid("20D04FE0-3AEA-1069-A2D8-08002B30309D");
            var parsingName = string.Format("::{{{0}}}\\::{{{1}}}", computerGuid, guid);

            Console.WriteLine("===============ADD================");
            KnownFoldersSafeNativeMethods.NativeFolderDefinition d = new KnownFoldersSafeNativeMethods.NativeFolderDefinition();
            d.category = FolderCategory.Common;
            d.localizedName = Marshal.StringToBSTR(extName);
            d.definitionOptions = DefinitionOptions.LocalRedirectOnly;
            d.name = Marshal.StringToBSTR(extName);
            d.description = Marshal.StringToBSTR("我的扩展描述");
            d.localizedName = Marshal.StringToBSTR("MyExtension");
            //var parent = KnownFolderHelper.FromKnownFolderId(new Guid("20D04FE0-3AEA-1069-A2D8-08002B30309D"));
            d.parentId = computerGuid;
            d.folderTypeId = FolderTypes.Documents;
            d.relativePath = Marshal.StringToBSTR("MyExtension");
            d.parsingName = Marshal.StringToBSTR("::{" + guid + "}");
            d.attributes = 1;

            KnownFolderManagerClass m = new KnownFolderManagerClass();
            m.RegisterFolder(guid, ref d);

            Console.WriteLine("Extension added: {0}", extName);

            Console.WriteLine("===============List again================");


            ListFolders();


            Console.WriteLine("===============GET================");
            IKnownFolderNative f;
            m.GetFolder(guid, out f);
            if (f != null)
            {
                //var sss = f.GetPath(0);
                //Console.WriteLine(sss);

                //f.SetPath((int)KNOWN_FOLDER_FLAG.KF_FLAG_DONT_UNEXPAND, @"d:\temp");
                //sss = f.GetPath(0);
                //Console.WriteLine(sss);

                KnownFoldersSafeNativeMethods.NativeFolderDefinition d2;
                f.GetFolderDefinition(out d2);

                Console.WriteLine("Extension found: {0}, {1}", Marshal.PtrToStringAuto(d2.name), Marshal.PtrToStringAuto(d2.relativePath));
                Console.WriteLine("Extension: ::{{{0}}}\\::{{{1}}}", d2.parentId, f.GetId());

                Guid ishellItem = new Guid(ShellIIDGuid.IShellItem2);
                IShellItem2 s;
                f.GetShellItem((int)(KNOWN_FOLDER_FLAG.KF_FLAG_CREATE
                    | KNOWN_FOLDER_FLAG.KF_FLAG_DEFAULT_PATH
                    | KNOWN_FOLDER_FLAG.KF_FLAG_INIT
                    | KNOWN_FOLDER_FLAG.KF_FLAG_SIMPLE_IDLIST), ref ishellItem, out s);


                //Console.WriteLine("===============DELETE================");
                //m.UnregisterFolder(guid);
                //Console.WriteLine("Extension deleted.");

                Console.WriteLine("===============GET================");
                Console.WriteLine("Try to get extension again:");

                IKnownFolderNative f2;
                m.GetFolder(guid, out f2);
                if (f2 != null)
                {
                    KnownFoldersSafeNativeMethods.NativeFolderDefinition d3;
                    f2.GetFolderDefinition(out d3);
                    Console.WriteLine("Extension found: {0}, {1}", Marshal.PtrToStringAuto(d3.name), Marshal.PtrToStringAuto(d3.relativePath));
                }
                else
                {
                    Console.WriteLine("Got nothing");
                }
            }


            Console.ReadKey();
        }

        private static void GetChilds(IShellFolder desktop)
        {
            Console.WriteLine("===============GetChilds================");

            //循环查找子项
            Microsoft.WindowsAPICodePack.Shell.IEnumIDList Enum = null;
            IEnumIDList EnumPtr;
            IntPtr pidlSub;
            uint celtFetched;

            if (desktop.EnumObjects(IntPtr.Zero, ShellNativeMethods.ShellFolderEnumerationOptions.Folders | ShellNativeMethods.ShellFolderEnumerationOptions.IncludeHidden | ShellNativeMethods.ShellFolderEnumerationOptions.Storage, out EnumPtr) == CONST.S_OK)
            {
                while (EnumPtr.Next(1, out pidlSub, out celtFetched) == 0 && celtFetched == CONST.S_FALSE)
                {
                    IntPtr strr;
                    desktop.GetDisplayNameOf(ref pidlSub, (uint)SHGNO.INFOLDER, out strr);
                    string name = Marshal.PtrToStringAuto(strr);

                    //SHFILEINFO info = new SHFILEINFO();
                    //Shell32.SHGetFileInfo(pidlSub, 0, ref info, Marshal.SizeOf(typeof(SHFILEINFO)), SHGFI.PIDL | SHGFI.DISPLAYNAME | SHGFI.ICONLOCATION);

                    //IShellFolder iSub;
                    //desktop.BindToObject(pidlSub, IntPtr.Zero, ref Guids.IID_IShellFolder, out iSub);

                    Console.WriteLine(name);
                }
            }
        }

        private static void ListFolders()
        {
            var ie = KnownFolders.Computer.GetEnumerator();
            while (ie.MoveNext())
            {
                var s = ie.Current;
                Console.WriteLine(string.Format("{0}\r\n\t{1}", s.Name, s.ParsingName));
            }
        }
    }
}
