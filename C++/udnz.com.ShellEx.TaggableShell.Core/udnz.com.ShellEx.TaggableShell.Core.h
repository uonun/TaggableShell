// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� UDNZCOMSHELLEXTAGGABLESHELLCORE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// UDNZCOMSHELLEXTAGGABLESHELLCORE_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef UDNZCOMSHELLEXTAGGABLESHELLCORE_EXPORTS
#define UDNZCOMSHELLEXTAGGABLESHELLCORE_API __declspec(dllexport)
#else
#define UDNZCOMSHELLEXTAGGABLESHELLCORE_API __declspec(dllimport)
#endif

// �����Ǵ� udnz.com.ShellEx.TaggableShell.Core.dll ������
class UDNZCOMSHELLEXTAGGABLESHELLCORE_API CudnzcomShellExTaggableShellCore {
public:
	CudnzcomShellExTaggableShellCore(void);
	// TODO: �ڴ�������ķ�����
};

extern UDNZCOMSHELLEXTAGGABLESHELLCORE_API int nudnzcomShellExTaggableShellCore;

UDNZCOMSHELLEXTAGGABLESHELLCORE_API int fnudnzcomShellExTaggableShellCore(void);
