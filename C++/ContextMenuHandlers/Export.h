// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 UDNZCOMSHELLEXTAGGABLESHELLCORE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// UDNZSHELLAPI 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef UDNZCOMSHELLEXTAGGABLESHELLCORE_EXPORTS
#define UDNZSHELLAPI __declspec(dllexport)
#else
#define UDNZSHELLAPI __declspec(dllimport)
#endif

//extern "C" UDNZSHELLAPI BOOL WINAPI DllMain(
//  _In_  HINSTANCE hinstDLL,
//  _In_  DWORD fdwReason,
//  _In_  LPVOID lpvReserved
//);

//extern "C" HRESULT CALLBACK DllGetClassObject 
//    (REFCLSID rclsid, REFIID riid, LPVOID * ppvObj) ;
//
//extern "C" UDNZSHELLAPI HRESULT __stdcall DllCanUnloadNow(void);

/*
// 此类是从 udnz.com.ShellEx.TaggableShell.Core.dll 导出的
class UDNZSHELLAPI CudnzcomShellExTaggableShellCore {
public:
	CudnzcomShellExTaggableShellCore(void);
	// TODO: 在此添加您的方法。
};

extern UDNZSHELLAPI int nudnzcomShellExTaggableShellCore;

UDNZSHELLAPI int fnudnzcomShellExTaggableShellCore(void);

*/
