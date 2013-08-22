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
using udnz.com.WinNative.Interfaces;
using udnz.com.WinNative.Structs;

namespace Testing
{
    class Program
    {
        static void Main(string[] args)
        {
            /* IntPtr desktopP;
             var desktop = ShellExtension.GetDesktopFolder(out desktopP);

             //循环查找子项
             IEnumIDList Enum = null;
             IntPtr EnumPtr = IntPtr.Zero;
             IntPtr pidlSub;
             int celtFetched;

             if (desktop.EnumObjects(IntPtr.Zero, SHCONTF.FOLDERS | SHCONTF.INCLUDEHIDDEN | SHCONTF.STORAGE | SHCONTF.SHAREABLE, out EnumPtr) == CONST.S_OK)
             {
                 Enum = (IEnumIDList)Marshal.GetObjectForIUnknown(EnumPtr);
                 while (Enum.Next(1, out pidlSub, out celtFetched) == 0 && celtFetched == CONST.S_FALSE)
                 {
                     string name = ShellExtension.GetNameByIShell(desktop, pidlSub);

                     SHFILEINFO info = new SHFILEINFO();
                     Shell32.SHGetFileInfo(pidlSub, 0, ref info, Marshal.SizeOf(typeof(SHFILEINFO)), SHGFI.PIDL | SHGFI.DISPLAYNAME | SHGFI.ICONLOCATION);

                     //IShellFolder iSub;
                     //desktop.BindToObject(pidlSub, IntPtr.Zero, ref Guids.IID_IShellFolder, out iSub);

                     Console.WriteLine(name);
                 }
             }
             */
            //ShellLibrary sl = new ShellLibrary("MyShellLib2", Microsoft.WindowsAPICodePack.Shell.KnownFolders.Libraries, true);

            var ie = Microsoft.WindowsAPICodePack.Shell.KnownFolders.Computer.GetEnumerator();
            while (ie.MoveNext())
            {
                var s = ie.Current;
                Console.WriteLine(s.Name);
            }

            string extName = "我的扩展";
            var guid = new Guid("EADFDCB5-0253-4EEF-881D-5D45B06CF3C9");
            KnownFolderManagerClass m = new KnownFolderManagerClass();

            Console.WriteLine("===============ADD================");
            KnownFoldersSafeNativeMethods.NativeFolderDefinition d = new KnownFoldersSafeNativeMethods.NativeFolderDefinition();
            d.category = FolderCategory.PerUser;
            d.localizedName = Marshal.StringToBSTR(extName);
            d.definitionOptions = DefinitionOptions.LocalRedirectOnly;
            d.name = Marshal.StringToBSTR(extName);
            d.description = Marshal.StringToBSTR("我的扩展描述");
            d.localizedName = Marshal.StringToBSTR("MyExtension");
            d.parentId = KnownFolders.Computer.FolderId;
            d.folderTypeId = FolderTypes.Documents;
            d.relativePath = Marshal.StringToBSTR("\\MyExtension");
            d.parsingName = Marshal.StringToBSTR("MyExtension");
            m.RegisterFolder(guid, ref d);

            Console.WriteLine("Extension added: {0}", extName);

            Console.WriteLine("===============List again================");


            ie = Microsoft.WindowsAPICodePack.Shell.KnownFolders.Computer.GetEnumerator();
            while (ie.MoveNext())
            {
                var s = ie.Current;
                Console.WriteLine(s.Name);
            }


            Console.WriteLine("===============GET================");
            IKnownFolderNative f;
            m.GetFolder(guid, out f);
            if (f != null)
            {
                KnownFoldersSafeNativeMethods.NativeFolderDefinition d2;
                f.GetFolderDefinition(out d2);

                Console.WriteLine("Extension found: {0}, {1}", Marshal.PtrToStringAuto(d2.name), Marshal.PtrToStringAuto(d2.relativePath));

                Console.WriteLine("===============DELETE================");
                m.UnregisterFolder(guid);
                Console.WriteLine("Extension deleted.");

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
                else {
                    Console.WriteLine("Got nothing");
                }
            }


            Console.ReadKey();
        }
    }
}
