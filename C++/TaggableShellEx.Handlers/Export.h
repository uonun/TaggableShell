// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� UDNZCOMSHELLEXTAGGABLESHELLCORE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// UDNZSHELLAPI ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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
// �����Ǵ� udnz.com.ShellEx.TaggableShell.Core.dll ������
class UDNZSHELLAPI CudnzcomShellExTaggableShellCore {
public:
	CudnzcomShellExTaggableShellCore(void);
	// TODO: �ڴ�������ķ�����
};

extern UDNZSHELLAPI int nudnzcomShellExTaggableShellCore;

UDNZSHELLAPI int fnudnzcomShellExTaggableShellCore(void);

*/
