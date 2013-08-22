using System;
using System.Collections.Generic;
using System.Text;

namespace udnz.com.WinNative.Enums
{
    /// <summary>
    /// HRESULT Wrapper    
    /// </summary>    
    public enum HResult
    {
        /// <summary>     
        /// S_OK          
        /// </summary>    
        Ok = 0x0000,

        /// <summary>
        /// S_FALSE
        /// </summary>        
        False = 0x0001,

        /// <summary>
        /// E_INVALIDARG
        /// </summary>
        InvalidArguments = unchecked((int)0x80070057),

        /// <summary>
        /// E_OUTOFMEMORY
        /// </summary>
        OutOfMemory = unchecked((int)0x8007000E),

        /// <summary>
        /// E_NOINTERFACE
        /// </summary>
        NoInterface = unchecked((int)0x80004002),

        /// <summary>
        /// E_FAIL
        /// </summary>
        Fail = unchecked((int)0x80004005),

        /// <summary>
        /// E_ELEMENTNOTFOUND
        /// </summary>
        ElementNotFound = unchecked((int)0x80070490),

        /// <summary>
        /// TYPE_E_ELEMENTNOTFOUND
        /// </summary>
        TypeElementNotFound = unchecked((int)0x8002802B),

        /// <summary>
        /// NO_OBJECT
        /// </summary>
        NoObject = unchecked((int)0x800401E5),

        /// <summary>
        /// Win32 Error code: ERROR_CANCELLED
        /// </summary>
        Win32ErrorCanceled = 1223,

        /// <summary>
        /// ERROR_CANCELLED
        /// </summary>
        Canceled = unchecked((int)0x800704C7),

        /// <summary>
        /// The requested resource is in use
        /// </summary>
        ResourceInUse = unchecked((int)0x800700AA),

        /// <summary>
        /// The requested resources is read-only.
        /// </summary>
        AccessDenied = unchecked((int)0x80030005)
    }

    /// <summary>
    /// 
    /// </summary>
    /// <seealso cref="APPBARDATA"/>
    public enum ABMsg
    {
        ABM_NEW = 0,
        ABM_REMOVE = 1,
        ABM_QUERYPOS = 2,
        ABM_SETPOS = 3,
        ABM_GETSTATE = 4,
        ABM_GETTASKBARPOS = 5,
        ABM_ACTIVATE = 6,
        ABM_GETAUTOHIDEBAR = 7,
        ABM_SETAUTOHIDEBAR = 8,
        ABM_WINDOWPOSCHANGED = 9,
        ABM_SETSTATE = 10
    }

    /// <summary>
    /// 
    /// </summary>
    /// <seealso cref="APPBARDATA"/>
    public enum ABEdge
    {
        ABE_LEFT = 0,
        ABE_TOP = 1,
        ABE_RIGHT = 2,
        ABE_BOTTOM = 3
    }

    /// <summary>
    /// Window Styles
    /// </summary>
    /// <remarks>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/ms632600(v=vs.85).aspx
    /// </remarks>
    public enum WindowStyles : uint
    {
        WS_OVERLAPPED = 0x00000000,
        WS_POPUP = 0x80000000,
        WS_CHILD = 0x40000000,
        WS_MINIMIZE = 0x20000000,
        WS_VISIBLE = 0x10000000,
        WS_DISABLED = 0x08000000,
        WS_CLIPSIBLINGS = 0x04000000,
        WS_CLIPCHILDREN = 0x02000000,
        WS_MAXIMIZE = 0x01000000,
        WS_CAPTION = 0x00C00000,
        WS_BORDER = 0x00800000,
        WS_DLGFRAME = 0x00400000,
        WS_VSCROLL = 0x00200000,
        WS_HSCROLL = 0x00100000,
        WS_SYSMENU = 0x00080000,
        WS_THICKFRAME = 0x00040000,
        WS_GROUP = 0x00020000,
        WS_TABSTOP = 0x00010000,
        WS_MINIMIZEBOX = 0x00020000,
        WS_MAXIMIZEBOX = 0x00010000,
        WS_TILED = 0x00000000,
        WS_ICONIC = 0x20000000,
        WS_SIZEBOX = 0x00040000,
        WS_OVERLAPPEDWINDOW = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX),
        WS_POPUPWINDOW = (WS_POPUP | WS_BORDER | WS_SYSMENU),
        WS_CHILDWINDOW = (WS_CHILD),
        WS_EX_DLGMODALFRAME = 0x00000001,
        WS_EX_NOPARENTNOTIFY = 0x00000004,
        WS_EX_TOPMOST = 0x00000008,
        WS_EX_ACCEPTFILES = 0x00000010,
        WS_EX_TRANSPARENT = 0x00000020,
        WS_EX_MDICHILD = 0x00000040,
        WS_EX_TOOLWINDOW = 0x00000080,
        WS_EX_WINDOWEDGE = 0x00000100,
        WS_EX_CLIENTEDGE = 0x00000200,
        WS_EX_CONTEXTHELP = 0x00000400,
        WS_EX_RIGHT = 0x00001000,
        WS_EX_LEFT = 0x00000000,
        WS_EX_RTLREADING = 0x00002000,
        WS_EX_LTRREADING = 0x00000000,
        WS_EX_LEFTSCROLLBAR = 0x00004000,
        WS_EX_RIGHTSCROLLBAR = 0x00000000,
        WS_EX_CONTROLPARENT = 0x00010000,
        WS_EX_STATICEDGE = 0x00020000,
        WS_EX_APPWINDOW = 0x00040000,
        WS_EX_OVERLAPPEDWINDOW = (0x00000100 | 0x00000200),
        WS_EX_PALETTEWINDOW = (0x00000100 | 0x00000080 | 0x00000008),
        WS_EX_LAYERED = 0x00080000,
        WS_EX_NOINHERITLAYOUT = 0x00100000,
        WS_EX_LAYOUTRTL = 0x00400000,
        WS_EX_NOACTIVATE = 0x08000000,
    }

    /// <summary>
    /// Window Class styles
    /// </summary>
    /// <remarks>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/ff729176(v=vs.85).aspx
    /// </remarks>
    public enum WindowClassStyles
    {
        CS_VREDRAW = 0x0001,
        CS_HREDRAW = 0x0002,
        CS_DBLCLKS = 0x0008,
        CS_OWNDC = 0x0020,
        CS_CLASSDC = 0x0040,
        CS_PARENTDC = 0x0080,
        CS_NOCLOSE = 0x0200,
        CS_SAVEBITS = 0x0800,
        CS_BYTEALIGNCLIENT = 0x1000,
        CS_BYTEALIGNWINDOW = 0x2000,
        CS_GLOBALCLASS = 0x4000,
        CS_DROPSHADOW = 0x00020000,
    }

    [Flags]
    public enum DCXFlags : long
    {
        DCX_WINDOW = 0x00000001L,
        DCX_CACHE = 0x00000002L,
        DCX_CLIPSIBLINGS = 0x00000010L,
    }

    public enum HitTest
    {
        HTNOWHERE = 0,
        HTCLIENT = 1,
        HTCAPTION = 2,
        HTSYSMENU = 3,
        HTMINBUTTON = 8,
        HTMAXBUTTON = 9,
        HTCLOSE = 20,
    }

    public enum RedrawWindowFlags
    {
        RDW_INVALIDATE = 0x0001,
        RDW_ALLCHILDREN = 0x0080,
        RDW_UPDATENOW = 0x0100,
        RDW_ERASENOW = 0x0200,
        RDW_FRAME = 0x0400,
    }

    public enum WAFlags
    {
        WA_ACTIVE = 1,
        WA_CLICKACTIVE = 2,
    }

    /// <summary>
    /// SetWindowPos function
    /// </summary>
    /// <remarks>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/ms633545(v=vs.85).aspx
    /// </remarks>
    [Flags]
    public enum SWPFlags
    {
        SWP_NOSIZE = 0x0001,
        SWP_NOMOVE = 0x0002,
        SWP_NOZORDER = 0x0004,
        SWP_NOREDRAW = 0x0008,
        SWP_NOACTIVATE = 0x0010,
        /// <summary>
        /// The frame changed: send WM_NCCALCSIZE
        /// </summary>
        SWP_FRAMECHANGED = 0x0020,
        SWP_SHOWWINDOW = 0x0040,
        SWP_HIDEWINDOW = 0x0080,
        SWP_NOCOPYBITS = 0x0100,
        /// <summary>
        /// Don't do owner Z ordering
        /// </summary>
        SWP_NOOWNERZORDER = 0x0200,
        /// <summary>
        /// Don't send WM_WINDOWPOSCHANGING
        /// </summary>
        SWP_NOSENDCHANGING = 0x0400,

        SWP_DRAWFRAME = SWP_FRAMECHANGED,
        SWP_NOREPOSITION = SWP_NOOWNERZORDER,

        SWP_DEFERERASE = 0x2000,
        SWP_ASYNCWINDOWPOS = 0x4000,
    }

    /// <summary>
    /// Windows Messages
    /// Defined in winuser.h from Windows SDK v6.1
    /// Documentation pulled from MSDN.
    /// </summary>
    public enum WM_Messages : uint
    {
        /// <summary>
        /// The WM_SIZE message is sent to a window after its size has changed.
        /// </summary>
        WM_SIZE = 0x0005,
        /// <summary>
        /// The WM_ACTIVATE message is sent to both the window being activated and the window being deactivated. If the windows use the same input queue, the message is sent synchronously, first to the window procedure of the top-level window being deactivated, then to the window procedure of the top-level window being activated. If the windows use different input queues, the message is sent asynchronously, so the window is activated immediately. 
        /// </summary>
        WM_ACTIVATE = 0x0006,
        /// <summary>
        /// The WM_SHOWWINDOW message is sent to a window when the window is about to be hidden or shown.
        /// </summary>
        WM_SHOWWINDOW = 0x0018,
        /// <summary>
        /// The WM_ACTIVATEAPP message is sent when a window belonging to a different application than the active window is about to be activated. The message is sent to the application whose window is being activated and to the application whose window is being deactivated.
        /// </summary>
        WM_ACTIVATEAPP = 0x001C,
        WM_GETMINMAXINFO = 0x0024,
        /// <summary>
        /// The WM_WINDOWPOSCHANGING message is sent to a window whose size, position, or place in the Z order is about to change as a result of a call to the SetWindowPos function or another window-management function.
        /// </summary>
        WM_WINDOWPOSCHANGING = 0x0046,
        /// <summary>
        /// The WM_WINDOWPOSCHANGED message is sent to a window whose size, position, or place in the Z order has changed as a result of a call to the SetWindowPos function or another window-management function.
        /// </summary>
        WM_WINDOWPOSCHANGED = 0x0047,
        /// <summary>
        /// The WM_STYLECHANGED message is sent to a window after the SetWindowLong function has changed one or more of the window's styles
        /// </summary>
        WM_STYLECHANGED = 0x007D,
        /// <summary>
        /// The WM_NCCALCSIZE message is sent when the size and position of a window's client area must be calculated. By processing this message, an application can control the content of the window's client area when the size or position of the window changes.
        /// </summary>
        WM_NCCALCSIZE = 0x0083,
        /// <summary>
        /// The WM_NCHITTEST message is sent to a window when the cursor moves, or when a mouse button is pressed or released. If the mouse is not captured, the message is sent to the window beneath the cursor. Otherwise, the message is sent to the window that has captured the mouse.
        /// </summary>
        WM_NCHITTEST = 0x0084,
        /// <summary>
        /// The WM_NCPAINT message is sent to a window when its frame must be painted. 
        /// </summary>
        WM_NCPAINT = 0x0085,
        /// <summary>
        /// The WM_NCACTIVATE message is sent to a window when its nonclient area needs to be changed to indicate an active or inactive state.
        /// </summary>
        WM_NCACTIVATE = 0x0086,
        /// <summary>
        /// The WM_NCMOUSEMOVE message is posted to a window when the cursor is moved within the nonclient area of the window. This message is posted to the window that contains the cursor. If a window has captured the mouse, this message is not posted.
        /// </summary>
        WM_NCMOUSEMOVE = 0x00A0,
        /// <summary>
        /// The WM_NCLBUTTONDOWN message is posted when the user presses the left mouse button while the cursor is within the nonclient area of a window. This message is posted to the window that contains the cursor. If a window has captured the mouse, this message is not posted.
        /// </summary>
        WM_NCLBUTTONDOWN = 0x00A1,
        /// <summary>
        /// The WM_NCLBUTTONUP message is posted when the user releases the left mouse button while the cursor is within the nonclient area of a window. This message is posted to the window that contains the cursor. If a window has captured the mouse, this message is not posted.
        /// </summary>
        WM_NCLBUTTONUP = 0x00A2,
        /// <summary>
        /// A window receives this message when the user chooses a command from the Window menu, clicks the maximize button, minimize button, restore button, close button, or moves the form. You can stop the form from moving by filtering this out.
        /// </summary>
        WM_SYSCOMMAND = 0x0112,
        /// <summary>
        /// The WM_LBUTTONDOWN message is posted when the user presses the left mouse button while the cursor is in the client area of a window. If the mouse is not captured, the message is posted to the window beneath the cursor. Otherwise, the message is posted to the window that has captured the mouse.
        /// </summary>
        WM_LBUTTONDOWN = 0x0201,
        /// <summary>
        /// An application sends the WM_MDIACTIVATE message to a multiple-document interface (MDI) client window to instruct the client window to activate a different MDI child window. 
        /// </summary>
        WM_MDIACTIVATE = 0x0222,
        /// <summary>
        /// The WM_MOUSELEAVE message is posted to a window when the cursor leaves the client area of the window specified in a prior call to TrackMouseEvent.
        /// </summary>
        WM_MOUSELEAVE = 0x02A3,
        /// <summary>
        /// The WM_NCMOUSELEAVE message is posted to a window when the cursor leaves the nonclient area of the window specified in a prior call to TrackMouseEvent.
        /// </summary>
        WM_NCMOUSELEAVE = 0x02A2,

        WM_MOUSEHOVER = 0x2A1,

        WM_NULL = 0x0000,
        WM_CREATE = 0x0001,
        WM_DESTROY = 0x0002,
        WM_MOVE = 0x0003,
        WM_SETFOCUS = 0x0007,
        WM_KILLFOCUS = 0x0008,
        WM_ENABLE = 0x000A,
        WM_SETREDRAW = 0x000B,
        WM_SETTEXT = 0x000C,
        WM_GETTEXT = 0x000D,
        WM_GETTEXTLENGTH = 0x000E,
        WM_PAINT = 0x000F,
        WM_CLOSE = 0x0010,
        WM_QUERYENDSESSION = 0x0011,
        WM_QUIT = 0x0012,
        WM_QUERYOPEN = 0x0013,
        WM_ERASEBKGND = 0x0014,
        WM_SYSCOLORCHANGE = 0x0015,
        WM_ENDSESSION = 0x0016,
        WM_CTLCOLOR = 0x0019,
        WM_WININICHANGE = 0x001A,
        WM_SETTINGCHANGE = 0x001A,
        WM_DEVMODECHANGE = 0x001B,
        WM_FONTCHANGE = 0x001D,
        WM_TIMECHANGE = 0x001E,
        WM_CANCELMODE = 0x001F,
        WM_SETCURSOR = 0x0020,
        WM_MOUSEACTIVATE = 0x0021,
        WM_CHILDACTIVATE = 0x0022,
        WM_QUEUESYNC = 0x0023,
        WM_PAINTICON = 0x0026,
        WM_ICONERASEBKGND = 0x0027,
        WM_NEXTDLGCTL = 0x0028,
        WM_SPOOLERSTATUS = 0x002A,
        WM_DRAWITEM = 0x002B,
        WM_MEASUREITEM = 0x002C,
        WM_DELETEITEM = 0x002D,
        WM_VKEYTOITEM = 0x002E,
        WM_CHARTOITEM = 0x002F,
        WM_SETFONT = 0x0030,
        WM_GETFONT = 0x0031,
        WM_SETHOTKEY = 0x0032,
        WM_GETHOTKEY = 0x0033,
        WM_QUERYDRAGICON = 0x0037,
        WM_COMPAREITEM = 0x0039,
        WM_GETOBJECT = 0x003D,
        WM_COMPACTING = 0x0041,
        WM_COMMNOTIFY = 0x0044,
        WM_POWER = 0x0048,
        WM_COPYDATA = 0x004A,
        WM_CANCELJOURNAL = 0x004B,
        WM_NOTIFY = 0x004E,
        WM_INPUTLANGCHANGEREQUEST = 0x0050,
        WM_INPUTLANGCHANGE = 0x0051,
        WM_TCARD = 0x0052,
        WM_HELP = 0x0053,
        WM_USERCHANGED = 0x0054,
        WM_NOTIFYFORMAT = 0x0055,
        WM_CONTEXTMENU = 0x007B,
        WM_STYLECHANGING = 0x007C,
        WM_DISPLAYCHANGE = 0x007E,
        WM_GETICON = 0x007F,
        WM_SETICON = 0x0080,
        WM_NCCREATE = 0x0081,
        WM_NCDESTROY = 0x0082,
        WM_GETDLGCODE = 0x0087,
        WM_SYNCPAINT = 0x0088,
        WM_NCLBUTTONDBLCLK = 0x00A3,
        WM_NCRBUTTONDOWN = 0x00A4,
        WM_NCRBUTTONUP = 0x00A5,
        WM_NCRBUTTONDBLCLK = 0x00A6,
        WM_NCMBUTTONDOWN = 0x00A7,
        WM_NCMBUTTONUP = 0x00A8,
        WM_NCMBUTTONDBLCLK = 0x00A9,
        WM_KEYDOWN = 0x0100,
        WM_KEYUP = 0x0101,
        WM_CHAR = 0x0102,
        WM_DEADCHAR = 0x0103,
        WM_SYSKEYDOWN = 0x0104,
        WM_SYSKEYUP = 0x0105,
        WM_SYSCHAR = 0x0106,
        WM_SYSDEADCHAR = 0x0107,
        WM_KEYLAST = 0x0108,
        WM_IME_STARTCOMPOSITION = 0x010D,
        WM_IME_ENDCOMPOSITION = 0x010E,
        WM_IME_COMPOSITION = 0x010F,
        WM_IME_KEYLAST = 0x010F,
        WM_INITDIALOG = 0x0110,
        WM_COMMAND = 0x0111,
        WM_TIMER = 0x0113,
        WM_HSCROLL = 0x0114,
        WM_VSCROLL = 0x0115,
        WM_INITMENU = 0x0116,
        WM_INITMENUPOPUP = 0x0117,
        WM_MENUSELECT = 0x011F,
        WM_MENUCHAR = 0x0120,
        WM_ENTERIDLE = 0x0121,
        WM_MENURBUTTONUP = 0x0122,
        WM_MENUDRAG = 0x0123,
        WM_MENUGETOBJECT = 0x0124,
        WM_UNINITMENUPOPUP = 0x0125,
        WM_MENUCOMMAND = 0x0126,
        WM_CTLCOLORWinMsgBOX = 0x0132,
        WM_CTLCOLOREDIT = 0x0133,
        WM_CTLCOLORLISTBOX = 0x0134,
        WM_CTLCOLORBTN = 0x0135,
        WM_CTLCOLORDLG = 0x0136,
        WM_CTLCOLORSCROLLBAR = 0x0137,
        WM_CTLCOLORSTATIC = 0x0138,
        WM_MOUSEMOVE = 0x0200,
        WM_LBUTTONUP = 0x0202,
        WM_LBUTTONDBLCLK = 0x0203,
        WM_RBUTTONDOWN = 0x0204,
        WM_RBUTTONUP = 0x0205,
        WM_RBUTTONDBLCLK = 0x0206,
        WM_MBUTTONDOWN = 0x0207,
        WM_MBUTTONUP = 0x0208,
        WM_MBUTTONDBLCLK = 0x0209,
        WM_MOUSEWHEEL = 0x020A,
        WM_PARENTNOTIFY = 0x0210,
        WM_ENTERMENULOOP = 0x0211,
        WM_EXITMENULOOP = 0x0212,
        WM_NEXTMENU = 0x0213,
        WM_SIZING = 0x0214,
        WM_CAPTURECHANGED = 0x0215,
        WM_MOVING = 0x0216,
        WM_DEVICECHANGE = 0x0219,
        WM_MDICREATE = 0x0220,
        WM_MDIDESTROY = 0x0221,
        WM_MDIRESTORE = 0x0223,
        WM_MDINEXT = 0x0224,
        WM_MDIMAXIMIZE = 0x0225,
        WM_MDITILE = 0x0226,
        WM_MDICASCADE = 0x0227,
        WM_MDIICONARRANGE = 0x0228,
        WM_MDIGETACTIVE = 0x0229,
        WM_MDISETMENU = 0x0230,
        WM_ENTERSIZEMOVE = 0x0231,
        WM_EXITSIZEMOVE = 0x0232,
        WM_DROPFILES = 0x0233,
        WM_MDIREFRESHMENU = 0x0234,
        WM_IME_SETCONTEXT = 0x0281,
        WM_IME_NOTIFY = 0x0282,
        WM_IME_CONTROL = 0x0283,
        WM_IME_COMPOSITIONFULL = 0x0284,
        WM_IME_SELECT = 0x0285,
        WM_IME_CHAR = 0x0286,
        WM_IME_REQUEST = 0x0288,
        WM_IME_KEYDOWN = 0x0290,
        WM_IME_KEYUP = 0x0291,
        WM_CUT = 0x0300,
        WM_COPY = 0x0301,
        WM_PASTE = 0x0302,
        WM_CLEAR = 0x0303,
        WM_UNDO = 0x0304,
        WM_RENDERFORMAT = 0x0305,
        WM_RENDERALLFORMATS = 0x0306,
        WM_DESTROYCLIPBOARD = 0x0307,
        WM_DRAWCLIPBOARD = 0x0308,
        WM_PAINTCLIPBOARD = 0x0309,
        WM_VSCROLLCLIPBOARD = 0x030A,
        WM_SIZECLIPBOARD = 0x030B,
        WM_ASKCBFORMATNAME = 0x030C,
        WM_CHANGECBCHAIN = 0x030D,
        WM_HSCROLLCLIPBOARD = 0x030E,
        WM_QUERYNEWPALETTE = 0x030F,
        WM_PALETTEISCHANGING = 0x0310,
        WM_PALETTECHANGED = 0x0311,
        WM_HOTKEY = 0x0312,
        WM_PRINT = 0x0317,
        WM_PRINTCLIENT = 0x0318,
        WM_HANDHELDFIRST = 0x0358,
        WM_HANDHELDLAST = 0x035F,
        WM_AFXFIRST = 0x0360,
        WM_AFXLAST = 0x037F,
        WM_PENWINFIRST = 0x0380,
        WM_PENWINLAST = 0x038F,
        WM_APP = 0x8000,
        WM_USER = 0x0400,
        WM_REFLECT = WM_USER + 0x1c00
    }

    /// <summary>
    /// The SysCommands which can be executed by a form button.
    /// </summary>
    /// <see cref="WM_Messages.WM_SYSCOMMAND"/>
    /// <remarks>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646360(v=vs.85).aspx
    /// </remarks>
    public enum WM_SYSCOMMAND_MSG
    {
        /// <summary>
        /// If no SysCommands should be executed
        /// </summary>
        SC_NONE = 0x0,
        SC_SIZE = 0xF000,
        SC_MOVE = 0xF010,
        /// <summary>
        /// If the Form should be minimized
        /// </summary>
        SC_MINIMIZE = 0xf020,
        /// <summary>
        /// If the Form should be maximized
        /// </summary>
        SC_MAXIMIZE = 0xf030,
        SC_NEXTWINDOW = 0xF040,
        SC_PREVWINDOW = 0xF050,
        /// <summary>
        /// If the Form should be closed.
        /// </summary>
        SC_CLOSE = 0xf060,
        SC_VSCROLL = 0xF070,
        SC_HSCROLL = 0xF080,
        SC_MOUSEMENU = 0xF090,
        SC_KEYMENU = 0xF100,
        SC_ARRANGE = 0xF110,
        /// <summary>
        /// If the form should be restored from the maximize mode.
        /// </summary>
        SC_RESTORE = 0xf120,
        /// <summary>
        /// Execute or activate the Windows Task Manager application
        /// </summary>
        SC_TASKLIST = 0xF130,
        SC_SCREENSAVE = 0xF140,
        SC_HOTKEY = 0xF150,
        SC_DEFAULT = 0xF160,
        SC_MONITORPOWER = 0xF170,
        SC_CONTEXTHELP = 0xF180,
        SC_SEPARATOR = 0xF00F,
    }

    /// <summary>
    /// MOUSEHOOKSTRUCT Mouse Position Codes
    /// </summary>
    /// <see cref="WM_Messages.WM_NCHITTEST"/>
    /// <remarks>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/ms645618(v=vs.85).aspx
    /// </remarks>
    public enum WM_NCHITTEST_MSG
    {
        HTERROR = (-2),
        HTTRANSPARENT = (-1),
        HTNOWHERE = 0,
        HTCLIENT = 1,
        HTCAPTION = 2,
        HTSYSMENU = 3,
        HTGROWBOX = 4,
        HTSIZE = HTGROWBOX,
        HTMENU = 5,
        HTHSCROLL = 6,
        HTVSCROLL = 7,
        HTMINBUTTON = 8,
        HTMAXBUTTON = 9,
        HTLEFT = 10,
        HTRIGHT = 11,
        HTTOP = 12,
        HTTOPLEFT = 13,
        HTTOPRIGHT = 14,
        HTBOTTOM = 15,
        HTBOTTOMLEFT = 16,
        HTBOTTOMRIGHT = 17,
        HTBORDER = 18,
        HTREDUCE = HTMINBUTTON,
        HTZOOM = HTMAXBUTTON,
        HTSIZEFIRST = HTLEFT,
        HTSIZELAST = HTBOTTOMRIGHT,
        HTOBJECT = 19,
        HTCLOSE = 20,
        HTHELP = 21,
    }

    /// <summary>
    /// 
    /// </summary>
    /// <see cref="User32.GetWindow"/>
    /// <remarks>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/ms633515(v=vs.85).aspx
    /// </remarks>
    public enum GetWindowCmd
    {
        GW_HWNDFIRST = 0,
        GW_HWNDLAST = 1,
        GW_HWNDNEXT = 2,
        GW_HWNDPREV = 3,
        GW_OWNER = 4,
        GW_CHILD = 5,
        GW_ENABLEDPOPUP = 6,
        GW_MAX = 6,
    }

    /// <summary>
    /// 
    /// </summary>
    /// <see cref="User32.SetWindowLong"/>
    /// <remarks>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/ms633591(v=vs.85).aspx
    /// </remarks>
    public enum SetWindowLongIndex
    {
        GWL_EXSTYLE = (-20),
        GWL_HINSTANCE = (-6),
        GWL_ID = (-12),
        GWL_STYLE = (-16),
        GWL_USERDATA = (-21),
        GWL_WNDPROC = (-4),
    }

    /// <summary>
    /// 
    /// </summary>
    /// <see cref="User32.UpdateLayeredWindow"/>
    /// <remarks>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/ms633556(v=vs.85).aspx
    /// </remarks>
    [Flags]
    public enum UpdateLayeredWindowFlag
    {
        ULW_COLORKEY = 0x00000001,
        ULW_ALPHA = 0x00000002,
        ULW_OPAQUE = 0x00000004,
    }

    /// <summary>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/gg258117(v=vs.85).aspx
    /// </summary>
    [Flags]
    public enum FILE_ATTRIBUTE : uint
    {
        READONLY = 0x00000001,
        HIDDEN = 0x00000002,
        SYSTEM = 0x00000004,
        DIRECTORY = 0x00000010,
        ARCHIVE = 0x00000020,
        DEVICE = 0x00000040,
        NORMAL = 0x00000080,
        TEMPORARY = 0x00000100,
        SPARSE_FILE = 0x00000200,
        REPARSE_POINT = 0x00000400,
        COMPRESSED = 0x00000800,
        OFFLINE = 0x00001000,
        NOT_CONTENT_INDEXED = 0x00002000,
        ENCRYPTED = 0x00004000,
        INTEGRITY_STREAM = 0x00008000,
        VIRTUAL = 0x00010000,
        NO_SCRUB_DATA = 0x00020000,
    }

    [Flags()]
    public enum CSIDL
    {
        ADMINTOOLS = 0x30,
        ALTSTARTUP = 0x1d,
        APPDATA = 0x1a,
        BITBUCKET = 10,
        CDBURN_AREA = 0x3b,
        COMMON_ADMINTOOLS = 0x2f,
        COMMON_ALTSTARTUP = 30,
        COMMON_APPDATA = 0x23,
        COMMON_DESKTOPDIRECTORY = 0x19,
        COMMON_DOCUMENTS = 0x2e,
        COMMON_FAVORITES = 0x1f,
        COMMON_MUSIC = 0x35,
        COMMON_PICTURES = 0x36,
        COMMON_PROGRAMS = 0x17,
        COMMON_STARTMENU = 0x16,
        COMMON_STARTUP = 0x18,
        COMMON_TEMPLATES = 0x2d,
        COMMON_VIDEO = 0x37,
        CONTROLS = 3,
        COOKIES = 0x21,
        DESKTOP = 0,
        DESKTOPDIRECTORY = 0x10,
        DRIVES = 0x11,
        FAVORITES = 6,
        FLAG_CREATE = 0x8000,
        FONTS = 20,
        HISTORY = 0x22,
        INTERNET = 1,
        INTERNET_CACHE = 0x20,
        LOCAL_APPDATA = 0x1c,
        MYDOCUMENTS = 12,
        MYMUSIC = 13,
        MYPICTURES = 0x27,
        MYVIDEO = 14,
        NETHOOD = 0x13,
        NETWORK = 0x12,
        PERSONAL = 5,
        PRINTERS = 4,
        PRINTHOOD = 0x1b,
        PROFILE = 40,
        PROFILES = 0x3e,
        PROGRAM_FILES = 0x26,
        PROGRAM_FILES_COMMON = 0x2b,
        PROGRAMS = 2,
        RECENT = 8,
        SENDTO = 9,
        STARTMENU = 11,
        STARTUP = 7,
        SYSTEM = 0x25,
        TEMPLATES = 0x15,
        WINDOWS = 0x24
    }

    [Flags]
    public enum TPM : uint
    {
        LEFTBUTTON = 0x0000,
        RIGHTBUTTON = 0x0002,
        LEFTALIGN = 0x0000,
        CENTERALIGN = 0x0004,
        RIGHTALIGN = 0x0008,
        TOPALIGN = 0x0000,
        VCENTERALIGN = 0x0010,
        BOTTOMALIGN = 0x0020,
        HORIZONTAL = 0x0000,
        VERTICAL = 0x0040,
        NONOTIFY = 0x0080,
        RETURNCMD = 0x0100,
        RECURSE = 0x0001,
        HORPOSANIMATION = 0x0400,
        HORNEGANIMATION = 0x0800,
        VERPOSANIMATION = 0x1000,
        VERNEGANIMATION = 0x2000,
        NOANIMATION = 0x4000,
        LAYOUTRTL = 0x8000
    }

    /// <summary>
    /// 
    /// </summary>
    /// <see cref="User32.SystemParametersInfo"/>
    /// <remarks>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/ms724947(v=vs.85).aspx
    /// </remarks>
    public enum SystemParametersInfoAction : uint
    {
        SPI_GETBEEP = 0x0001,
        SPI_SETBEEP = 0x0002,
        SPI_GETMOUSE = 0x0003,
        SPI_SETMOUSE = 0x0004,
        SPI_GETBORDER = 0x0005,
        SPI_SETBORDER = 0x0006,
        SPI_GETKEYBOARDSPEED = 0x000A,
        SPI_SETKEYBOARDSPEED = 0x000B,
        SPI_LANGDRIVER = 0x000C,
        SPI_ICONHORIZONTALSPACING = 0x000D,
        SPI_GETSCREENSAVETIMEOUT = 0x000E,
        SPI_SETSCREENSAVETIMEOUT = 0x000F,
        SPI_GETSCREENSAVEACTIVE = 0x0010,
        SPI_SETSCREENSAVEACTIVE = 0x0011,
        SPI_GETGRIDGRANULARITY = 0x0012,
        SPI_SETGRIDGRANULARITY = 0x0013,
        SPI_SETDESKWALLPAPER = 0x0014,
        SPI_SETDESKPATTERN = 0x0015,
        SPI_GETKEYBOARDDELAY = 0x0016,
        SPI_SETKEYBOARDDELAY = 0x0017,
        SPI_ICONVERTICALSPACING = 0x0018,
        SPI_GETICONTITLEWRAP = 0x0019,
        SPI_SETICONTITLEWRAP = 0x001A,
        SPI_GETMENUDROPALIGNMENT = 0x001B,
        SPI_SETMENUDROPALIGNMENT = 0x001C,
        SPI_SETDOUBLECLKWIDTH = 0x001D,
        SPI_SETDOUBLECLKHEIGHT = 0x001E,
        SPI_GETICONTITLELOGFONT = 0x001F,
        SPI_SETDOUBLECLICKTIME = 0x0020,
        SPI_SETMOUSEBUTTONSWAP = 0x0021,
        SPI_SETICONTITLELOGFONT = 0x0022,
        SPI_GETFASTTASKSWITCH = 0x0023,
        SPI_SETFASTTASKSWITCH = 0x0024,
        SPI_SETDRAGFULLWINDOWS = 0x0025,
        SPI_GETDRAGFULLWINDOWS = 0x0026,
        SPI_GETNONCLIENTMETRICS = 0x0029,
        SPI_SETNONCLIENTMETRICS = 0x002A,
        SPI_GETMINIMIZEDMETRICS = 0x002B,
        SPI_SETMINIMIZEDMETRICS = 0x002C,
        SPI_GETICONMETRICS = 0x002D,
        SPI_SETICONMETRICS = 0x002E,
        SPI_SETWORKAREA = 0x002F,
        SPI_GETWORKAREA = 0x0030,
        SPI_SETPENWINDOWS = 0x0031,
        SPI_GETHIGHCONTRAST = 0x0042,
        SPI_SETHIGHCONTRAST = 0x0043,
        SPI_GETKEYBOARDPREF = 0x0044,
        SPI_SETKEYBOARDPREF = 0x0045,
        SPI_GETSCREENREADER = 0x0046,
        SPI_SETSCREENREADER = 0x0047,
        SPI_GETANIMATION = 0x0048,
        SPI_SETANIMATION = 0x0049,
        SPI_GETFONTSMOOTHING = 0x004A,
        SPI_SETFONTSMOOTHING = 0x004B,
        SPI_SETDRAGWIDTH = 0x004C,
        SPI_SETDRAGHEIGHT = 0x004D,
        SPI_SETHANDHELD = 0x004E,
        SPI_GETLOWPOWERTIMEOUT = 0x004F,
        SPI_GETPOWEROFFTIMEOUT = 0x0050,
        SPI_SETLOWPOWERTIMEOUT = 0x0051,
        SPI_SETPOWEROFFTIMEOUT = 0x0052,
        SPI_GETLOWPOWERACTIVE = 0x0053,
        SPI_GETPOWEROFFACTIVE = 0x0054,
        SPI_SETLOWPOWERACTIVE = 0x0055,
        SPI_SETPOWEROFFACTIVE = 0x0056,
        SPI_SETCURSORS = 0x0057,
        SPI_SETICONS = 0x0058,
        SPI_GETDEFAULTINPUTLANG = 0x0059,
        SPI_SETDEFAULTINPUTLANG = 0x005A,
        SPI_SETLANGTOGGLE = 0x005B,
        SPI_GETWINDOWSEXTENSION = 0x005C,
        SPI_SETMOUSETRAILS = 0x005D,
        SPI_GETMOUSETRAILS = 0x005E,
        SPI_SETSCREENSAVERRUNNING = 0x0061,
        SPI_SCREENSAVERRUNNING = SPI_SETSCREENSAVERRUNNING,
        SPI_GETFILTERKEYS = 0x0032,
        SPI_SETFILTERKEYS = 0x0033,
        SPI_GETTOGGLEKEYS = 0x0034,
        SPI_SETTOGGLEKEYS = 0x0035,
        SPI_GETMOUSEKEYS = 0x0036,
        SPI_SETMOUSEKEYS = 0x0037,
        SPI_GETSHOWSOUNDS = 0x0038,
        SPI_SETSHOWSOUNDS = 0x0039,
        SPI_GETSTICKYKEYS = 0x003A,
        SPI_SETSTICKYKEYS = 0x003B,
        SPI_GETACCESSTIMEOUT = 0x003C,
        SPI_SETACCESSTIMEOUT = 0x003D,
        SPI_GETSERIALKEYS = 0x003E,
        SPI_SETSERIALKEYS = 0x003F,
        SPI_GETSOUNDSENTRY = 0x0040,
        SPI_SETSOUNDSENTRY = 0x0041,
        SPI_GETSNAPTODEFBUTTON = 0x005F,
        SPI_SETSNAPTODEFBUTTON = 0x0060,
        SPI_GETMOUSEHOVERWIDTH = 0x0062,
        SPI_SETMOUSEHOVERWIDTH = 0x0063,
        SPI_GETMOUSEHOVERHEIGHT = 0x0064,
        SPI_SETMOUSEHOVERHEIGHT = 0x0065,
        SPI_GETMOUSEHOVERTIME = 0x0066,
        SPI_SETMOUSEHOVERTIME = 0x0067,
        SPI_GETWHEELSCROLLLINES = 0x0068,
        SPI_SETWHEELSCROLLLINES = 0x0069,
        SPI_GETMENUSHOWDELAY = 0x006A,
        SPI_SETMENUSHOWDELAY = 0x006B,
        SPI_GETWHEELSCROLLCHARS = 0x006C,
        SPI_SETWHEELSCROLLCHARS = 0x006D,
        SPI_GETSHOWIMEUI = 0x006E,
        SPI_SETSHOWIMEUI = 0x006F,
        SPI_GETMOUSESPEED = 0x0070,
        SPI_SETMOUSESPEED = 0x0071,
        SPI_GETSCREENSAVERRUNNING = 0x0072,
        SPI_GETDESKWALLPAPER = 0x0073,
        SPI_GETAUDIODESCRIPTION = 0x0074,
        SPI_SETAUDIODESCRIPTION = 0x0075,
        SPI_GETSCREENSAVESECURE = 0x0076,
        SPI_SETSCREENSAVESECURE = 0x0077,
        SPI_GETHUNGAPPTIMEOUT = 0x0078,
        SPI_SETHUNGAPPTIMEOUT = 0x0079,
        SPI_GETWAITTOKILLTIMEOUT = 0x007A,
        SPI_SETWAITTOKILLTIMEOUT = 0x007B,
        SPI_GETWAITTOKILLSERVICETIMEOUT = 0x007C,
        SPI_SETWAITTOKILLSERVICETIMEOUT = 0x007D,
        SPI_GETMOUSEDOCKTHRESHOLD = 0x007E,
        SPI_SETMOUSEDOCKTHRESHOLD = 0x007F,
        SPI_GETPENDOCKTHRESHOLD = 0x0080,
        SPI_SETPENDOCKTHRESHOLD = 0x0081,
        SPI_GETWINARRANGING = 0x0082,
        SPI_SETWINARRANGING = 0x0083,
        SPI_GETMOUSEDRAGOUTTHRESHOLD = 0x0084,
        SPI_SETMOUSEDRAGOUTTHRESHOLD = 0x0085,
        SPI_GETPENDRAGOUTTHRESHOLD = 0x0086,
        SPI_SETPENDRAGOUTTHRESHOLD = 0x0087,
        SPI_GETMOUSESIDEMOVETHRESHOLD = 0x0088,
        SPI_SETMOUSESIDEMOVETHRESHOLD = 0x0089,
        SPI_GETPENSIDEMOVETHRESHOLD = 0x008A,
        SPI_SETPENSIDEMOVETHRESHOLD = 0x008B,
        SPI_GETDRAGFROMMAXIMIZE = 0x008C,
        SPI_SETDRAGFROMMAXIMIZE = 0x008D,
        SPI_GETSNAPSIZING = 0x008E,
        SPI_SETSNAPSIZING = 0x008F,
        SPI_GETDOCKMOVING = 0x0090,
        SPI_SETDOCKMOVING = 0x0091,
        SPI_GETACTIVEWINDOWTRACKING = 0x1000,
        SPI_SETACTIVEWINDOWTRACKING = 0x1001,
        SPI_GETMENUANIMATION = 0x1002,
        SPI_SETMENUANIMATION = 0x1003,
        SPI_GETCOMBOBOXANIMATION = 0x1004,
        SPI_SETCOMBOBOXANIMATION = 0x1005,
        SPI_GETLISTBOXSMOOTHSCROLLING = 0x1006,
        SPI_SETLISTBOXSMOOTHSCROLLING = 0x1007,
        SPI_GETGRADIENTCAPTIONS = 0x1008,
        SPI_SETGRADIENTCAPTIONS = 0x1009,
        SPI_GETKEYBOARDCUES = 0x100A,
        SPI_SETKEYBOARDCUES = 0x100B,
        SPI_GETMENUUNDERLINES = SPI_GETKEYBOARDCUES,
        SPI_SETMENUUNDERLINES = SPI_SETKEYBOARDCUES,
        SPI_GETACTIVEWNDTRKZORDER = 0x100C,
        SPI_SETACTIVEWNDTRKZORDER = 0x100D,
        SPI_GETHOTTRACKING = 0x100E,
        SPI_SETHOTTRACKING = 0x100F,
        SPI_GETMENUFADE = 0x1012,
        SPI_SETMENUFADE = 0x1013,
        SPI_GETSELECTIONFADE = 0x1014,
        SPI_SETSELECTIONFADE = 0x1015,
        SPI_GETTOOLTIPANIMATION = 0x1016,
        SPI_SETTOOLTIPANIMATION = 0x1017,
        SPI_GETTOOLTIPFADE = 0x1018,
        SPI_SETTOOLTIPFADE = 0x1019,
        SPI_GETCURSORSHADOW = 0x101A,
        SPI_SETCURSORSHADOW = 0x101B,
        SPI_GETMOUSESONAR = 0x101C,
        SPI_SETMOUSESONAR = 0x101D,
        SPI_GETMOUSECLICKLOCK = 0x101E,
        SPI_SETMOUSECLICKLOCK = 0x101F,
        SPI_GETMOUSEVANISH = 0x1020,
        SPI_SETMOUSEVANISH = 0x1021,
        SPI_GETFLATMENU = 0x1022,
        SPI_SETFLATMENU = 0x1023,
        SPI_GETDROPSHADOW = 0x1024,
        SPI_SETDROPSHADOW = 0x1025,
        SPI_GETBLOCKSENDINPUTRESETS = 0x1026,
        SPI_SETBLOCKSENDINPUTRESETS = 0x1027,
        SPI_GETUIEFFECTS = 0x103E,
        SPI_SETUIEFFECTS = 0x103F,
        SPI_GETDISABLEOVERLAPPEDCONTENT = 0x1040,
        SPI_SETDISABLEOVERLAPPEDCONTENT = 0x1041,
        SPI_GETCLIENTAREAANIMATION = 0x1042,
        SPI_SETCLIENTAREAANIMATION = 0x1043,
        SPI_GETCLEARTYPE = 0x1048,
        SPI_SETCLEARTYPE = 0x1049,
        SPI_GETSPEECHRECOGNITION = 0x104A,
        SPI_SETSPEECHRECOGNITION = 0x104B,
        SPI_GETFOREGROUNDLOCKTIMEOUT = 0x2000,
        SPI_SETFOREGROUNDLOCKTIMEOUT = 0x2001,
        SPI_GETACTIVEWNDTRKTIMEOUT = 0x2002,
        SPI_SETACTIVEWNDTRKTIMEOUT = 0x2003,
        SPI_GETFOREGROUNDFLASHCOUNT = 0x2004,
        SPI_SETFOREGROUNDFLASHCOUNT = 0x2005,
        SPI_GETCARETWIDTH = 0x2006,
        SPI_SETCARETWIDTH = 0x2007,
        SPI_GETMOUSECLICKLOCKTIME = 0x2008,
        SPI_SETMOUSECLICKLOCKTIME = 0x2009,
        SPI_GETFONTSMOOTHINGTYPE = 0x200A,
        SPI_SETFONTSMOOTHINGTYPE = 0x200B,
        SPI_GETFONTSMOOTHINGCONTRAST = 0x200C,
        SPI_SETFONTSMOOTHINGCONTRAST = 0x200D,
        SPI_GETFOCUSBORDERWIDTH = 0x200E,
        SPI_SETFOCUSBORDERWIDTH = 0x200F,
        SPI_GETFOCUSBORDERHEIGHT = 0x2010,
        SPI_SETFOCUSBORDERHEIGHT = 0x2011,
        SPI_GETFONTSMOOTHINGORIENTATION = 0x2012,
        SPI_SETFONTSMOOTHINGORIENTATION = 0x2013,
        SPI_GETMINIMUMHITRADIUS = 0x2014,
        SPI_SETMINIMUMHITRADIUS = 0x2015,
        SPI_GETMESSAGEDURATION = 0x2016,
        SPI_SETMESSAGEDURATION = 0x2017,
    }

    #region MessageBox function
    /// <summary>
    /// 
    /// </summary>
    /// <see cref="User32.MessageBox"/>
    /// <remarks>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/ms645505(v=vs.85).aspx
    /// </remarks>
    public enum MessageBoxFlag : uint
    {
        MB_OK = 0x00000000,
        MB_OKCANCEL = 0x00000001,
        MB_ABORTRETRYIGNORE = 0x00000002,
        MB_YESNOCANCEL = 0x00000003,
        MB_YESNO = 0x00000004,
        MB_RETRYCANCEL = 0x00000005,
        MB_CANCELTRYCONTINUE = 0x00000006,

        MB_ICONHAND = 0x00000010,
        MB_ICONQUESTION = 0x00000020,
        MB_ICONEXCLAMATION = 0x00000030,
        MB_ICONASTERISK = 0x00000040,

        MB_USERICON = 0x00000080,
        MB_ICONWARNING = MB_ICONEXCLAMATION,
        MB_ICONERROR = MB_ICONHAND,

        MB_ICONINFORMATION = MB_ICONASTERISK,
        MB_ICONSTOP = MB_ICONHAND,

        MB_DEFBUTTON1 = 0x00000000,
        MB_DEFBUTTON2 = 0x00000100,
        MB_DEFBUTTON3 = 0x00000200,
        MB_DEFBUTTON4 = 0x00000300,

        MB_APPLMODAL = 0x00000000,
        MB_SYSTEMMODAL = 0x00001000,
        MB_TASKMODAL = 0x00002000,
        MB_HELP = 0x00004000,// Help Button       

        MB_NOFOCUS = 0x00008000,
        MB_SETFOREGROUND = 0x00010000,
        MB_DEFAULT_DESKTOP_ONLY = 0x00020000,

        MB_TOPMOST = 0x00040000,
        MB_RIGHT = 0x00080000,
        MB_RTLREADING = 0x00100000,

        MB_SERVICE_NOTIFICATION = 0x00200000,
        MB_SERVICE_NOTIFICATION_NT3X = 0x00040000,

        MB_TYPEMASK = 0x0000000F,
        MB_ICONMASK = 0x000000F0,
        MB_DEFMASK = 0x00000F00,
        MB_MODEMASK = 0x00003000,
        MB_MISCMASK = 0x0000C000,
    }

    /// <summary>
    /// 
    /// </summary>
    /// <see cref="User32.MessageBox"/>
    /// <remarks>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/ms645505(v=vs.85).aspx
    /// </remarks>
    public enum MessageBoxCommandID
    {
        IDOK = 1,
        IDCANCEL = 2,
        IDABORT = 3,
        IDRETRY = 4,
        IDIGNORE = 5,
        IDYES = 6,
        IDNO = 7,
        IDCLOSE = 8,
        IDHELP = 9,
        IDTRYAGAIN = 10,
        IDCONTINUE = 11,
        IDTIMEOUT = 32000,
    }
    #endregion

    /// <summary>
    /// 
    /// </summary>
    /// <see cref="Shell32.SHGetFileInfo"/>
    [Flags()]
    public enum SHGFI : uint
    {
        ADDOVERLAYS = 0x20,
        ATTR_SPECIFIED = 0x20000,
        ATTRIBUTES = 0x800,
        DISPLAYNAME = 0x200,
        EXETYPE = 0x2000,
        ICON = 0x100,
        ICONLOCATION = 0x1000,
        LARGEICON = 0,
        LINKOVERLAY = 0x8000,
        OPENICON = 2,
        OVERLAYINDEX = 0x40,
        PIDL = 8,
        SELECTED = 0x10000,
        SHELLICONSIZE = 4,
        SMALLICON = 1,
        SYSICONINDEX = 0x4000,
        TYPENAME = 0x400,
        USEFILEATTRIBUTES = 0x10
    }

    /// <summary>
    /// 
    /// </summary>
    /// <see cref="udnz.com.WinNative.Shell.IShellFolder.EnumObjects"/>
    [Flags()]
    public enum SHCONTF
    {
        FOLDERS = 0x20,
        NONFOLDERS = 0x40,
        INCLUDEHIDDEN = 0x80,
        INIT_ON_FIRST_NEXT = 0x100,
        NETPRINTERSRCH = 0x200,
        SHAREABLE = 0x400,
        STORAGE = 0x800
    }

    /// <summary>
    /// 
    /// </summary>
    /// <see cref="udnz.com.WinNative.Shell.IShellFolder.GetAttributesOf"/>
    [Flags()]
    public enum SFGAO
    {
        CANCOPY = 0x1,
        CANMOVE = 0x2,
        CANLINK = 0x4,
        STORAGE = 0x8,
        CANRENAME = 0x10,
        CANDELETE = 0x20,
        HASPROPSHEET = 0x40,
        DROPTARGET = 0x100,
        CAPABILITYMASK = 0x177,
        ENCRYPTED = 0x2000,
        ISSLOW = 0x4000,
        GHOSTED = 0x8000,
        LINK = 0x10000,
        SHARE = 0x20000,
        READONLY = 0x40000,
        HIDDEN = 0x80000,
        DISPLAYATTRMASK = 0xFC000,
        FILESYSANCESTOR = 0x10000000,
        FOLDER = 0x20000000,
        FILESYSTEM = 0x40000000,
        HASSUBFOLDER = unchecked((int)0x80000000),
        CONTENTSMASK = unchecked((int)0x80000000),
        VALIDATE = 0x1000000,
        REMOVABLE = 0x2000000,
        COMPRESSED = 0x4000000,
        BROWSABLE = 0x8000000,
        NONENUMERATED = 0x100000,
        NEWCONTENT = 0x200000,
        CANMONIKER = 0x400000,
        HASSTORAGE = 0x400000,
        STREAM = 0x400000,
        STORAGEANCESTOR = 0x800000,
        STORAGECAPMASK = 0x70C50008
    }

    /// <summary>
    /// 
    /// </summary>
    /// <see cref="udnz.com.WinNative.Shell.IShellFolder.GetDisplayNameOf"/>
    /// <see cref="udnz.com.WinNative.Shell.IShellFolder.SetNameOf"/>
    [Flags()]
    public enum SHGNO
    {
        NORMAL = 0x0,
        INFOLDER = 0x1,
        FOREDITING = 0x1000,
        FORADDRESSBAR = 0x4000,
        FORPARSING = 0x8000,
    }

}
