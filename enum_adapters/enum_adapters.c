#define COBJMACROS
#include <windows.h>
#include "beacon.h"

#define EBP_ABOVE 0x01 // specifies that the binding paths contain a network component - can start at component or contain component in path
#define EBP_BELOW 0x02 // specifies that the binding paths start at the network component

typedef struct _INetCfg INetCfg;

typedef struct INetCfgVtbl 
{
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(INetCfg *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(INetCfg *This);
    ULONG (STDMETHODCALLTYPE *Release)(INetCfg *This);
    HRESULT (STDMETHODCALLTYPE *Initialize)(INetCfg *This, PVOID pvReserved);
    HRESULT (STDMETHODCALLTYPE *Uninitialize)(INetCfg *This);
    HRESULT (STDMETHODCALLTYPE *Apply)(INetCfg *This);
    HRESULT (STDMETHODCALLTYPE *Cancel)(INetCfg *This);
    HRESULT (STDMETHODCALLTYPE *EnumComponents)(INetCfg *This, const GUID *pguidClass, void **ppIEnum);
    HRESULT (STDMETHODCALLTYPE *FindComponent)(INetCfg *This, LPCWSTR pszwInfId, void **ppncc);
    HRESULT (STDMETHODCALLTYPE *QueryNetCfgClass)(INetCfg *This, const GUID *pguidClass, REFIID riid, void **ppvObject);
} INetCfgVtbl;

typedef struct _INetCfg 
{
    struct INetCfgVtbl *lpVtbl;
} INetCfg;

typedef struct _IEnumNetCfgComponent IEnumNetCfgComponent;

typedef struct IEnumNetCfgComponentVtbl 
{
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IEnumNetCfgComponent*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(IEnumNetCfgComponent*);
    ULONG   (STDMETHODCALLTYPE *Release)(IEnumNetCfgComponent*);
    HRESULT (STDMETHODCALLTYPE *Next)(IEnumNetCfgComponent*, ULONG, void** /*INetCfgComponent** */, ULONG*);
    HRESULT (STDMETHODCALLTYPE *Skip)(IEnumNetCfgComponent*, ULONG);
    HRESULT (STDMETHODCALLTYPE *Reset)(IEnumNetCfgComponent*);
    HRESULT (STDMETHODCALLTYPE *Clone)(IEnumNetCfgComponent*, IEnumNetCfgComponent**);
    END_INTERFACE
} IEnumNetCfgComponentVtbl;

typedef struct _IEnumNetCfgComponent 
{ 
    struct IEnumNetCfgComponentVtbl *lpVtbl;
} IEnumNetCfgComponent;

typedef struct _INetCfgComponent INetCfgComponent;

typedef struct INetCfgComponentVtbl 
{
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(INetCfgComponent*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(INetCfgComponent*);
    ULONG   (STDMETHODCALLTYPE *Release)(INetCfgComponent*);
    HRESULT (STDMETHODCALLTYPE *GetDisplayName)(INetCfgComponent*, LPWSTR*);
    HRESULT (STDMETHODCALLTYPE *SetDisplayName)(INetCfgComponent*, LPCWSTR);
    HRESULT (STDMETHODCALLTYPE *GetHelpText)(INetCfgComponent*, LPWSTR*);
    HRESULT (STDMETHODCALLTYPE *GetId)(INetCfgComponent*, LPWSTR*);
    HRESULT (STDMETHODCALLTYPE *GetCharacteristics)(INetCfgComponent*, DWORD*);
    HRESULT (STDMETHODCALLTYPE *GetInstanceGuid)(INetCfgComponent*, GUID*);
    HRESULT (STDMETHODCALLTYPE *GetPnpDevNodeId)(INetCfgComponent*, LPWSTR*);
    HRESULT (STDMETHODCALLTYPE *GetClassGuid)(INetCfgComponent*, GUID*);
    HRESULT (STDMETHODCALLTYPE *GetBindName)(INetCfgComponent*, LPWSTR*);
    HRESULT (STDMETHODCALLTYPE *GetDeviceStatus)(INetCfgComponent*, DWORD*);
    HRESULT (STDMETHODCALLTYPE *OpenParamKey)(INetCfgComponent*, HKEY*);
    HRESULT (STDMETHODCALLTYPE *RaisePropertyUi)(INetCfgComponent*, HWND, DWORD, const WCHAR*);
    END_INTERFACE
} INetCfgComponentVtbl;

typedef struct _INetCfgComponent 
{ 
    struct INetCfgComponentVtbl *lpVtbl; 
} INetCfgComponent;

typedef struct _INetCfgClass INetCfgClass;

typedef struct INetCfgClassVtbl 
{
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(INetCfgClass*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(INetCfgClass*);
    ULONG   (STDMETHODCALLTYPE *Release)(INetCfgClass*);
    HRESULT (STDMETHODCALLTYPE *FindComponent)(INetCfgClass*, LPCWSTR, INetCfgComponent**);
    HRESULT (STDMETHODCALLTYPE *EnumComponents)(INetCfgClass*, IEnumNetCfgComponent**);
    END_INTERFACE
} INetCfgClassVtbl;

typedef struct _INetCfgClass 
{ 
    struct INetCfgClassVtbl *lpVtbl; 
} INetCfgClass;

/* --- Bindings layer --- */
typedef struct _INetCfgComponentBindings INetCfgComponentBindings;

typedef struct INetCfgComponentBindingsVtbl 
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(INetCfgComponentBindings*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(INetCfgComponentBindings*);
    ULONG   (STDMETHODCALLTYPE *Release)(INetCfgComponentBindings*);
    HRESULT (STDMETHODCALLTYPE *BindTo)(INetCfgComponentBindings*, INetCfgComponent*); /* not used here */
    HRESULT (STDMETHODCALLTYPE *UnbindFrom)(INetCfgComponentBindings*, INetCfgComponent*);
    HRESULT (STDMETHODCALLTYPE *SupportsBindingInterface)(INetCfgComponentBindings*, DWORD, LPCWSTR);
    HRESULT (STDMETHODCALLTYPE *IsBoundTo)(INetCfgComponentBindings*, INetCfgComponent*);
    HRESULT (STDMETHODCALLTYPE *IsBindableTo)(INetCfgComponentBindings*, INetCfgComponent*);
    HRESULT (STDMETHODCALLTYPE *EnumBindingPaths)(INetCfgComponentBindings*, DWORD, void** /*IEnumNetCfgBindingPath**/);
    HRESULT (STDMETHODCALLTYPE *MoveBefore)(INetCfgComponentBindings*, DWORD, void*, void*);
    HRESULT (STDMETHODCALLTYPE *MoveAfter)(INetCfgComponentBindings*, DWORD, void*, void*);
} INetCfgComponentBindingsVtbl;

typedef struct _INetCfgComponentBindings 
{ 
    struct INetCfgComponentBindingsVtbl *lpVtbl; 
} INetCfgComponentBindings;

typedef struct _IEnumNetCfgBindingPath IEnumNetCfgBindingPath;

typedef struct IEnumNetCfgBindingPathVtbl 
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IEnumNetCfgBindingPath*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(IEnumNetCfgBindingPath*);
    ULONG   (STDMETHODCALLTYPE *Release)(IEnumNetCfgBindingPath*);
    HRESULT (STDMETHODCALLTYPE *Next)(IEnumNetCfgBindingPath*, ULONG, void** /*INetCfgBindingPath**/, ULONG*);
    HRESULT (STDMETHODCALLTYPE *Skip)(IEnumNetCfgBindingPath*, ULONG);
    HRESULT (STDMETHODCALLTYPE *Reset)(IEnumNetCfgBindingPath*);
    HRESULT (STDMETHODCALLTYPE *Clone)(IEnumNetCfgBindingPath*, IEnumNetCfgBindingPath**);
} IEnumNetCfgBindingPathVtbl;

typedef struct _IEnumNetCfgBindingPath 
{ 
    struct IEnumNetCfgBindingPathVtbl *lpVtbl; 
} IEnumNetCfgBindingPath;

typedef struct _INetCfgBindingPath INetCfgBindingPath;

typedef struct INetCfgBindingPathVtbl 
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(INetCfgBindingPath*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(INetCfgBindingPath*);
    ULONG   (STDMETHODCALLTYPE *Release)(INetCfgBindingPath*);
    HRESULT (STDMETHODCALLTYPE *IsSamePathAs)(INetCfgBindingPath*, INetCfgBindingPath*);
    HRESULT (STDMETHODCALLTYPE *IsSubPathOf)(INetCfgBindingPath*, INetCfgBindingPath*);
    HRESULT (STDMETHODCALLTYPE *IsEnabled)(INetCfgBindingPath*);           /* returns S_OK if enabled, S_FALSE if disabled */
    HRESULT (STDMETHODCALLTYPE *Enable)(INetCfgBindingPath*, BOOL);
    HRESULT (STDMETHODCALLTYPE *GetPathToken)(INetCfgBindingPath*, LPWSTR*);
    HRESULT (STDMETHODCALLTYPE *GetOwner)(INetCfgBindingPath*, INetCfgComponent**);
    HRESULT (STDMETHODCALLTYPE *GetDepth)(INetCfgBindingPath*, ULONG*);
    HRESULT (STDMETHODCALLTYPE *EnumBindingInterfaces)(INetCfgBindingPath*, void** /*IEnumNetCfgBindingInterface**/);
} INetCfgBindingPathVtbl;

typedef struct _INetCfgBindingPath 
{ 
    struct INetCfgBindingPathVtbl *lpVtbl; 
} INetCfgBindingPath;

typedef struct _IEnumNetCfgBindingInterface IEnumNetCfgBindingInterface;

typedef struct IEnumNetCfgBindingInterfaceVtbl 
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IEnumNetCfgBindingInterface*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(IEnumNetCfgBindingInterface*);
    ULONG   (STDMETHODCALLTYPE *Release)(IEnumNetCfgBindingInterface*);
    HRESULT (STDMETHODCALLTYPE *Next)(IEnumNetCfgBindingInterface*, ULONG, void** /*INetCfgBindingInterface**/, ULONG*);
    HRESULT (STDMETHODCALLTYPE *Skip)(IEnumNetCfgBindingInterface*, ULONG);
    HRESULT (STDMETHODCALLTYPE *Reset)(IEnumNetCfgBindingInterface*);
    HRESULT (STDMETHODCALLTYPE *Clone)(IEnumNetCfgBindingInterface*, IEnumNetCfgBindingInterface**);
} IEnumNetCfgBindingInterfaceVtbl;

typedef struct _IEnumNetCfgBindingInterface 
{ 
    struct IEnumNetCfgBindingInterfaceVtbl *lpVtbl; 
} IEnumNetCfgBindingInterface;

typedef struct _INetCfgBindingInterface INetCfgBindingInterface;

typedef struct INetCfgBindingInterfaceVtbl 
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(INetCfgBindingInterface*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(INetCfgBindingInterface*);
    ULONG   (STDMETHODCALLTYPE *Release)(INetCfgBindingInterface*);
    HRESULT (STDMETHODCALLTYPE *GetName)(INetCfgBindingInterface*, LPWSTR*);
    HRESULT (STDMETHODCALLTYPE *GetUpperComponent)(INetCfgBindingInterface*, INetCfgComponent**);
    HRESULT (STDMETHODCALLTYPE *GetLowerComponent)(INetCfgBindingInterface*, INetCfgComponent**);
} INetCfgBindingInterfaceVtbl;

typedef struct _INetCfgBindingInterface 
{ 
    struct INetCfgBindingInterfaceVtbl *lpVtbl; 
} INetCfgBindingInterface;

DECLSPEC_IMPORT HRESULT WINAPI OLE32$CoInitialize(LPVOID pvReserved);
DECLSPEC_IMPORT void WINAPI OLE32$CoUninitialize();
DECLSPEC_IMPORT void WINAPI OLE32$CoTaskMemFree(LPVOID pv);
DECLSPEC_IMPORT HRESULT WINAPI OLE32$CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv);
DECLSPEC_IMPORT int WINAPI KERNEL32$MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
DECLSPEC_IMPORT int WINAPI KERNEL32$WideCharToMultiByte(UINT, DWORD, LPCWCH, int, LPSTR, int, LPCCH, LPBOOL);

static const GUID _CLSID_CNetCfg = { 0x5B035261, 0x40F9, 0x11D1, {0xAA,0xEC,0x00,0x80,0x5F,0xC1,0x27,0x0E} }; 
static const GUID _IID_INetCfg = { 0xC0E8AE93, 0x306E, 0x11D1, {0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E} };  
static const GUID _GUID_DEVCLASS_NET = { 0x4D36E972, 0xE325, 0x11CE, {0xBF,0xC1,0x08,0x00,0x2B,0xE1,0x03,0x18} }; 
static const GUID _IID_INetCfgClass = { 0xC0E8AE97, 0x306E, 0x11D1, {0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E} };  
static const GUID _IID_INetCfgComponentBindings = { 0xC0E8AE9E, 0x306E, 0x11D1, {0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E} }; 

#define CHECK_HRES(x) do { hr = (x); if (FAILED(hr)) { BeaconPrintf(CALLBACK_ERROR, #x " -> 0x%08X\n", hr); goto end; } } while (0)

void PrintUTF8(const char *prefix, LPWSTR w)
{
    char buf[1024];
    int n = 0;
    if (w) {
        n = KERNEL32$WideCharToMultiByte(CP_UTF8, 0, w, -1, buf, sizeof(buf), NULL, NULL);
    }
    BeaconPrintf(CALLBACK_OUTPUT, "%s%s\n", prefix, (n > 0) ? buf : "[null]");
}

HRESULT EnumAdapters(void)
{
    HRESULT hr = E_FAIL;
    INetCfg *pnc = NULL;
    INetCfgClass *pClass = NULL;
    IEnumNetCfgComponent *pEnum = NULL;
    INetCfgComponent *pAdapter = NULL;

    CHECK_HRES(OLE32$CoInitialize(NULL));
    CHECK_HRES(OLE32$CoCreateInstance(&_CLSID_CNetCfg, NULL, CLSCTX_INPROC_SERVER, &_IID_INetCfg, &pnc));
    CHECK_HRES(pnc->lpVtbl->Initialize(pnc, NULL));
    CHECK_HRES(pnc->lpVtbl->QueryNetCfgClass(pnc, &_GUID_DEVCLASS_NET, &_IID_INetCfgClass, &pClass));
    CHECK_HRES(pClass->lpVtbl->EnumComponents(pClass, &pEnum));

    for (;;) {
        ULONG fa = 0;
        pAdapter = NULL;

        hr = pEnum->lpVtbl->Next(pEnum, 1, &pAdapter, &fa);
        if (hr != S_OK || !pAdapter) { hr = S_OK; break; }

        LPWSTR ppszwDisplayName = NULL, ppszwId = NULL;
        (void)pAdapter->lpVtbl->GetDisplayName(pAdapter, &ppszwDisplayName);
        (void)pAdapter->lpVtbl->GetId(pAdapter, &ppszwId);

        if (ppszwDisplayName && ppszwId) {
            BeaconPrintf(CALLBACK_OUTPUT, "\n======================================================\n");
            BeaconPrintf(CALLBACK_OUTPUT, "> Adapter: %ls\n", ppszwDisplayName);
            BeaconPrintf(CALLBACK_OUTPUT, "> ID: %ls\n\n", ppszwId);
            BeaconPrintf(CALLBACK_OUTPUT, "%-8s %-14s %s", "Enabled", "ID", "Name\n");
            BeaconPrintf(CALLBACK_OUTPUT, "-------- -------------- ----------------------\n");
        }

        INetCfgComponentBindings *pBindings = NULL;
        CHECK_HRES(pAdapter->lpVtbl->QueryInterface(pAdapter, &_IID_INetCfgComponentBindings, &pBindings));

        IEnumNetCfgBindingPath *pBindingPath = NULL;
        CHECK_HRES(pBindings->lpVtbl->EnumBindingPaths(pBindings, EBP_ABOVE, &pBindingPath));

        for (;;) {
            ULONG fp = 0;
            INetCfgBindingPath *pPath = NULL;

            hr = pBindingPath->lpVtbl->Next(pBindingPath, 1, &pPath, &fp);
            if (hr != S_OK || !pPath) { hr = S_OK; break; }

            // if S_OK -> enabled; if S_FALSE -> disabled
            BOOL enabled = (pPath->lpVtbl->IsEnabled(pPath) == S_OK);

            // find the hop where lower == this adapter
            IEnumNetCfgBindingInterface *pEnumIf = NULL;
            CHECK_HRES(pPath->lpVtbl->EnumBindingInterfaces(pPath, &pEnumIf) && pEnumIf);

            for (;;) {
                ULONG fbi = 0;
                INetCfgBindingInterface *pBindingInterface = NULL;

                hr = pEnumIf->lpVtbl->Next(pEnumIf, 1, &pBindingInterface, &fbi);
                if (hr != S_OK || !pBindingInterface) { hr = S_OK; break; }

                INetCfgComponent *pUpper=NULL, *pLower=NULL;
                if (SUCCEEDED(pBindingInterface->lpVtbl->GetUpperComponent(pBindingInterface, &pUpper)) && 
                    SUCCEEDED(pBindingInterface->lpVtbl->GetLowerComponent(pBindingInterface, &pLower))) {

                    if (pLower == pAdapter) {
                        LPWSTR upName=NULL, upId=NULL;
                        (void)pUpper->lpVtbl->GetDisplayName(pUpper, &upName);
                        (void)pUpper->lpVtbl->GetId(pUpper, &upId);
                        BeaconPrintf(CALLBACK_OUTPUT, "> Bound: (enabled=%s)\n", enabled ? "True" : "False");
                        PrintUTF8("     Name: ", upName);
                        PrintUTF8("     Id:   ", upId);
                        //BeaconPrintf(CALLBACK_OUTPUT, "%-8s %-14ls %ls (%ls)\n",
                                    //enabled ? "True" : "False",
                                    //upId,
                                    //upName);
                        
                        if (upName) OLE32$CoTaskMemFree(upName);
                        if (upId) OLE32$CoTaskMemFree(upId);
                    }
                }

                if (pUpper) pUpper->lpVtbl->Release(pUpper);
                if (pLower) pLower->lpVtbl->Release(pLower);
                pBindingInterface->lpVtbl->Release(pBindingInterface);
            }

            pEnumIf->lpVtbl->Release(pEnumIf);
            pPath->lpVtbl->Release(pPath);

        }

        pBindingPath->lpVtbl->Release(pBindingPath);
        pBindings->lpVtbl->Release(pBindings);
        if (ppszwDisplayName) OLE32$CoTaskMemFree(ppszwDisplayName);
        if (ppszwId)   OLE32$CoTaskMemFree(ppszwId);
        pAdapter->lpVtbl->Release(pAdapter);
    } 

end:
    if (pEnum) pEnum->lpVtbl->Release(pEnum);
    if (pClass) pClass->lpVtbl->Release(pClass);
    if (pnc) 
        (void)pnc->lpVtbl->Uninitialize(pnc);
        pnc->lpVtbl->Release(pnc);
    OLE32$CoUninitialize();
    return hr;
}

void go (IN PCHAR Buffer, IN ULONG Length)
{
    HRESULT hr = EnumAdapters();
    if (SUCCEEDED(hr)) {
        BeaconPrintf(CALLBACK_OUTPUT, "\n===== [DONE] =====\n");
    } else {
        BeaconPrintf(CALLBACK_ERROR, "[X] Something failed: 0x%08X\n", hr);
    }
}

