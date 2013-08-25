//Copyright (c) Microsoft Corporation.  All rights reserved.

namespace Microsoft.WindowsAPICodePack.Shell
{
    /// <summary>
    /// Represents a non filesystem item (e.g. virtual items inside Control Panel)
    /// </summary>
    public class ShellNonFileSystemItem : ShellObject
    {
        public ShellNonFileSystemItem(IShellItem2 shellItem)
        {
            nativeShellItem = shellItem;
        }
    }
}
