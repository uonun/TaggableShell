//Copyright (c) Microsoft Corporation.  All rights reserved.

namespace Microsoft.WindowsAPICodePack.Controls
{
    public static class ExplorerBrowserIIDGuid
    {
        // IID GUID strings for relevant Shell COM interfaces.
        public const string IExplorerBrowser = "DFD3B6B5-C10C-4BE9-85F6-A66969F402F6";
        public const string IKnownFolderManager = "8BE2D872-86AA-4d47-B776-32CCA40C7018";
        public const string IFolderView = "cde725b0-ccc9-4519-917e-325d72fab4ce";
        public const string IFolderView2 = "1af3a467-214f-4298-908e-06b03e0b39f9";
        public const string IServiceProvider = "6d5140c1-7436-11ce-8034-00aa006009fa";
        public const string IExplorerPaneVisibility = "e07010ec-bc17-44c0-97b0-46c7c95b9edc";
        public const string IExplorerBrowserEvents = "361bbdc7-e6ee-4e13-be58-58e2240c810f";
        public const string IInputObject = "68284fAA-6A48-11D0-8c78-00C04fd918b4";
        public const string IShellView = "000214E3-0000-0000-C000-000000000046";
        public const string IDispatch = "00020400-0000-0000-C000-000000000046";
        public const string DShellFolderViewEvents = "62112AA2-EBE4-11cf-A5FB-0020AFE7292D";

        public const string ICommDlgBrowser = "000214F1-0000-0000-C000-000000000046";
        public const string ICommDlgBrowser2 = "10339516-2894-11d2-9039-00C04F8EEB3E";
        public const string ICommDlgBrowser3 = "c8ad25a1-3294-41ee-8165-71174bd01c57";

    }

    public static class ExplorerBrowserViewPanes
    {
        public const string Navigation = "cb316b22-25f7-42b8-8a09-540d23a43c2f";
        public const string Commands = "d9745868-ca5f-4a76-91cd-f5a129fbb076";
        public const string CommandsOrganize = "72e81700-e3ec-4660-bf24-3c3b7b648806";
        public const string CommandsView = "21f7c32d-eeaa-439b-bb51-37b96fd6a943";
        public const string Details = "43abf98b-89b8-472d-b9ce-e69b8229f019";
        public const string Preview = "893c63d1-45c8-4d17-be19-223be71be365";
        public const string Query = "65bcde4f-4f07-4f27-83a7-1afca4df7ddd";
        public const string AdvancedQuery = "b4e9db8b-34ba-4c39-b5cc-16a1bd2c411c";
    }

    public static class ExplorerBrowserCLSIDGuid
    {
        // CLSID GUID strings for relevant coclasses.
        public const string ExplorerBrowser = "71F96385-DDD6-48D3-A0C1-AE06E8B055FB";
    }

    public static class ExplorerBrowserViewDispatchIds
    {
        public const int SelectionChanged = 200;
        public const int ContentsChanged = 207;
        public const int FileListEnumDone = 201;
        public const int SelectedItemChanged = 220;
    }
}
