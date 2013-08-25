﻿//Copyright (c) Microsoft Corporation.  All rights reserved.

using Microsoft.WindowsAPICodePack.Resources;
namespace Microsoft.WindowsAPICodePack.Dialogs
{
    public static class DialogsDefaults
    {
        public static string Caption { get { return LocalizedMessages.DialogDefaultCaption; } }
        public static string MainInstruction { get { return LocalizedMessages.DialogDefaultMainInstruction; } }
        public static string Content { get { return LocalizedMessages.DialogDefaultContent; } }

        public const int ProgressBarStartingValue = 0;
        public const int ProgressBarMinimumValue = 0;
        public const int ProgressBarMaximumValue = 100;

        public const int IdealWidth = 0;

        // For generating control ID numbers that won't 
        // collide with the standard button return IDs.
        public const int MinimumDialogControlId =
            (int)TaskDialogNativeMethods.TaskDialogCommonButtonReturnIds.Close + 1;
    }
}
