//Copyright (c) Microsoft Corporation.  All rights reserved.

using MS.WindowsAPICodePack.Internal;

namespace Microsoft.WindowsAPICodePack.Shell
{
    /// <summary>
    /// Represents a saved search
    /// </summary>
    public class ShellSavedSearchCollection : ShellSearchCollection
    {
        public ShellSavedSearchCollection(IShellItem2 shellItem)
            : base(shellItem)
        {
            CoreHelpers.ThrowIfNotVista();
        }
    }
}
