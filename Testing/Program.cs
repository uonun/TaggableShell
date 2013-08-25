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
        [DllImport("shell32.dll")]
        private static extern int SHILCreateFromPath([MarshalAs(UnmanagedType.LPWStr)] string pszPath, out IntPtr ppIdl, ref uint rgflnOut);

        static string extName = "我的扩展";
        static Guid guid = new Guid("00000ABC-0000-0000-0000-5D45B06CF3C9");
        static Guid computerGuid = new Guid("20D04FE0-3AEA-1069-A2D8-08002B30309D");
        static string parsingName = string.Format("::{{{0}}}\\::{{{1}}}", computerGuid, guid);
        static KnownFolderManagerClass m = new KnownFolderManagerClass();

        static void Main(string[] args)
        {
            Console.BufferWidth = 120;
            Console.WindowWidth = 120;
            Console.WindowHeight = 50;

            //IntPtr desktopP;
            //var desktop = ShellExtension.GetDesktopFolder(out desktopP);
            //ShellNativeMethods.SHGetDesktopFolder
            //GetChilds(desktop);
            //---------------

            CreateLib();

            ListFolders();

            RegisterFolder();


            //--------------------
            IShellItem s;
            NonFileSystemKnownFolder sf = (NonFileSystemKnownFolder)ShellFolder.FromParsingName(KnownFolders.Computer.ParsingName);
            IntPtr ppidl;
            uint flag = 0;
            SHILCreateFromPath(@"d:\temp", out ppidl, ref flag);
            var tmp = ShellNativeMethods.SHCreateShellItem(IntPtr.Zero, sf.NativeShellFolder, ppidl, out s);
            


            GetFolder();

            //DeleteFolder();

            Console.WriteLine("Try to get extension again:");

            GetFolder();


            Console.ReadKey();
        }

        private static void DeleteFolder()
        {
            Console.WriteLine("===============DELETE================");
            m.UnregisterFolder(guid);
            Console.WriteLine("Extension deleted.");
            Console.WriteLine();
            Console.WriteLine();
        }

        private static void GetFolder()
        {
            Console.WriteLine("===============GET================");
            IKnownFolderNative f;
            m.GetFolder(guid, out f);
            if (f != null)
            {
                KnownFoldersSafeNativeMethods.NativeFolderDefinition d2;
                f.GetFolderDefinition(out d2);

                Console.WriteLine("Extension found: {0}, {1}", Marshal.PtrToStringAuto(d2.name), Marshal.PtrToStringAuto(d2.relativePath));
                Console.WriteLine("Extension: ::{{{0}}}\\::{{{1}}}", d2.parentId, f.GetId());

                //Guid ishellItem = new Guid(ShellIIDGuid.IShellItem2);
                //IShellItem2 s;
                //f.GetShellItem((int)(KNOWN_FOLDER_FLAG.KF_FLAG_CREATE
                //    | KNOWN_FOLDER_FLAG.KF_FLAG_DEFAULT_PATH
                //    | KNOWN_FOLDER_FLAG.KF_FLAG_INIT
                //    | KNOWN_FOLDER_FLAG.KF_FLAG_SIMPLE_IDLIST), ref ishellItem, out s);
            }
            else
            {
                Console.WriteLine("Got nothing");
            }
            Console.WriteLine();
            Console.WriteLine();
        }

        private static void RegisterFolder()
        {
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

            m.RegisterFolder(guid, ref d);

            Console.WriteLine("Extension added: {0}", extName);
            Console.WriteLine();
            Console.WriteLine();
        }

        private static void CreateLib()
        {
            ShellLibrary sl = new ShellLibrary("MyShellLib2", Microsoft.WindowsAPICodePack.Shell.KnownFolders.Libraries, true);
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
            Console.WriteLine();
            Console.WriteLine();
        }

        private static void ListFolders()
        {
            var ie = KnownFolders.Computer.GetEnumerator();
            while (ie.MoveNext())
            {
                var s = ie.Current;
                Console.WriteLine(string.Format("{0,-25}\t{1}", s.Name, s.ParsingName));
            }
            Console.WriteLine();
            Console.WriteLine();
        }
    }
}
