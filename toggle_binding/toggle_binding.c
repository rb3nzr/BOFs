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

typedef struct _INetCfgLock INetCfgLock;
typedef struct INetCfgLockVtbl 
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(INetCfgLock*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(INetCfgLock*);
    ULONG   (STDMETHODCALLTYPE *Release)(INetCfgLock*);
    HRESULT (STDMETHODCALLTYPE *AcquireWriteLock)(INetCfgLock*, DWORD cmsTimeout, LPCWSTR pszClientDesc, LPWSTR *ppszClientDesc);
    HRESULT (STDMETHODCALLTYPE *ReleaseWriteLock)(INetCfgLock*);
} INetCfgLockVtbl;

typedef struct _INetCfgLock 
{ 
    struct INetCfgLockVtbl *lpVtbl; 
} INetCfgLock;

DECLSPEC_IMPORT int MSVCRT$strcmp(const char *str1, const char *str2);
DECLSPEC_IMPORT HRESULT WINAPI OLE32$CoInitialize(LPVOID pvReserved);
DECLSPEC_IMPORT void WINAPI OLE32$CoUninitialize();
DECLSPEC_IMPORT void WINAPI OLE32$CoTaskMemFree(LPVOID pv);
DECLSPEC_IMPORT HRESULT WINAPI OLE32$CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv);
DECLSPEC_IMPORT int WINAPI KERNEL32$MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
DECLSPEC_IMPORT int WINAPI KERNEL32$WideCharToMultiByte(UINT, DWORD, LPCWCH, int, LPSTR, int, LPCCH, LPBOOL);

static const GUID _IID_INetCfgLock = { 0xC0E8AE9F,0x306E,0x11D1,{0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E} }; 
static const GUID _CLSID_CNetCfg = { 0x5B035261, 0x40F9, 0x11D1, {0xAA,0xEC,0x00,0x80,0x5F,0xC1,0x27,0x0E} }; 
static const GUID _IID_INetCfg = { 0xC0E8AE93, 0x306E, 0x11D1, {0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E} };
static const GUID _IID_INetCfgClass = { 0xC0E8AE97, 0x306E, 0x11D1, {0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E} }; 
static const GUID _GUID_DEVCLASS_NET = { 0x4D36E972, 0xE325, 0x11CE, {0xBF,0xC1,0x08,0x00,0x2B,0xE1,0x03,0x18} };
static const GUID _IID_INetCfgComponentBindings = { 0xC0E8AE9E,0x306E,0x11D1,{0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E} };

#define CHECK_HRES(x) do { hr = (x); if (FAILED(hr)) { BeaconPrintf(CALLBACK_ERROR, #x " -> 0x%08X\n", hr); goto cleanup; } } while (0)

// case-insensitive compare - LPWSTR vs UTF-8/ASCII 
int w_equals_ci(LPCWSTR w, const char *s)
{
    if (!w || !s) return 0;
    char buf[256];
    int n = KERNEL32$WideCharToMultiByte(CP_UTF8, 0, w, -1, buf, sizeof(buf), NULL, NULL);
    if (n <= 0) return 0;

    for (int i=0; buf[i] && s[i]; ++i) {
        char a = buf[i]; if (a >= 'A' && a <= 'Z') a += 32;
        char b = s[i];  if (b >= 'A' && b <= 'Z') b += 32;
        if (a != b) return 0;
        if (!buf[i+1] && !s[i+1]) return 1;
    }
    return (buf[0] == 0 && s[0] == 0);
}


HRESULT ToggleBindingOnAdapter(const char *adapterIdentifier, const char *bindingId, BOOL enable)
{
    HRESULT hr = E_FAIL;
    INetCfg *pnc = NULL;
    INetCfgLock *pLock = NULL;
    INetCfgComponent *pAdapter = NULL;
    INetCfgClass *pClass = NULL;
    IEnumNetCfgComponent *pEnum = NULL;
    BOOL lockAcquired = FALSE;
    BOOL adapterFound = FALSE;
    BOOL bindingFound = FALSE;

    CHECK_HRES(OLE32$CoInitialize(NULL));
    CHECK_HRES(OLE32$CoCreateInstance(&_CLSID_CNetCfg, NULL, CLSCTX_INPROC_SERVER, &_IID_INetCfg, &pnc));
    
    // get the lock interface
    hr = pnc->lpVtbl->QueryInterface(pnc, &_IID_INetCfgLock, (void**)&pLock);
    if (FAILED(hr)) {
        BeaconPrintf(CALLBACK_ERROR, "Failed to get INetCfgLock interface: 0x%08X\n", hr);
        goto cleanup;
    }
    
    // request write lock
    LPWSTR ppszwClientDescription = NULL; 
    hr = pLock->lpVtbl->AcquireWriteLock(pLock, 5000, L"ToggleBinding BOF", &ppszwClientDescription);
    if (FAILED(hr)) {
        if (ppszwClientDescription) OLE32$CoTaskMemFree(ppszwClientDescription);
        BeaconPrintf(CALLBACK_ERROR, "Failed to acquire write lock: 0x%08X\n", hr);
        goto cleanup;
    }
    lockAcquired = TRUE;
    if (ppszwClientDescription) OLE32$CoTaskMemFree(ppszwClientDescription);
    
    CHECK_HRES(pnc->lpVtbl->Initialize(pnc, NULL));
    CHECK_HRES(pnc->lpVtbl->QueryNetCfgClass(pnc, &_GUID_DEVCLASS_NET, &_IID_INetCfgClass, (void**)&pClass));
    CHECK_HRES(pClass->lpVtbl->EnumComponents(pClass, (void**)&pEnum));

    // gind the specific adapter
    BeaconPrintf(CALLBACK_OUTPUT, "[*] Looking for adapter: %s\n", adapterIdentifier);
    
    while (!adapterFound) {
        ULONG fetched = 0;
        pAdapter = NULL;
        hr = pEnum->lpVtbl->Next(pEnum, 1, (void**)&pAdapter, &fetched);
        if (hr != S_OK || !pAdapter) break;

        // get adapter ID and display name
        LPWSTR adapterId = NULL;
        LPWSTR adapterName = NULL;
        BOOL idMatch = FALSE;
        BOOL nameMatch = FALSE;
        
        if (SUCCEEDED(pAdapter->lpVtbl->GetId(pAdapter, &adapterId))) {
            idMatch = w_equals_ci(adapterId, adapterIdentifier);
        }
        
        if (SUCCEEDED(pAdapter->lpVtbl->GetDisplayName(pAdapter, &adapterName))) {
            nameMatch = w_equals_ci(adapterName, adapterIdentifier);
        }
        
        if (idMatch || nameMatch) {
            adapterFound = TRUE;
            BeaconPrintf(CALLBACK_OUTPUT, "[+] Found adapter: %ls (ID: %ls)\n", 
                         adapterName ? adapterName : L"Unknown",
                         adapterId ? adapterId : L"Unknown");
            
            // look for the specific binding on this adapter
            BeaconPrintf(CALLBACK_OUTPUT, "[*] Looking for binding '%s'\n", bindingId);
            
            INetCfgComponentBindings *pBind = NULL;
            hr = pAdapter->lpVtbl->QueryInterface(pAdapter, &_IID_INetCfgComponentBindings, (void**)&pBind);
            if (SUCCEEDED(hr) && pBind) {
                IEnumNetCfgBindingPath *pBindingPath = NULL;
                hr = pBind->lpVtbl->EnumBindingPaths(pBind, EBP_ABOVE, (void**)&pBindingPath);
                if (SUCCEEDED(hr) && pBindingPath) {
                    while (!bindingFound) {
                        ULONG fp = 0;
                        INetCfgBindingPath *pPath = NULL;
                        hr = pBindingPath->lpVtbl->Next(pBindingPath, 1, (void**)&pPath, &fp);
                        if (hr != S_OK || !pPath) break;

                        IEnumNetCfgBindingInterface *pEnumIf = NULL;
                        hr = pPath->lpVtbl->EnumBindingInterfaces(pPath, (void**)&pEnumIf);
                        if (SUCCEEDED(hr) && pEnumIf) {
                            while (!bindingFound) {
                                ULONG fbi = 0;
                                INetCfgBindingInterface *pBindingInterface = NULL;
                                hr = pEnumIf->lpVtbl->Next(pEnumIf, 1, (void**)&pBindingInterface, &fbi);
                                if (hr != S_OK || !pBindingInterface) break;

                                INetCfgComponent *pUpper = NULL, *pLower = NULL;
                                if (SUCCEEDED(pBindingInterface->lpVtbl->GetUpperComponent(pBindingInterface, &pUpper)) &&
                                    SUCCEEDED(pBindingInterface->lpVtbl->GetLowerComponent(pBindingInterface, &pLower))) {
                                    
                                    // verify this is our target adapter
                                    if (pLower == pAdapter) {
                                        LPWSTR upperId = NULL;
                                        if (SUCCEEDED(pUpper->lpVtbl->GetId(pUpper, &upperId))) {
                                            if (w_equals_ci(upperId, bindingId)) {
                                                bindingFound = TRUE;
                                                BeaconPrintf(CALLBACK_OUTPUT, "[+] Found binding '%s' on adapter\n", bindingId);
                                                
                                                // get current state
                                                HRESULT isEnabledResult = pPath->lpVtbl->IsEnabled(pPath);
                                                BOOL currentlyEnabled = (isEnabledResult == S_OK);
                                                BOOL currentlyDisabled = (isEnabledResult == S_FALSE);
                                                
                                                BeaconPrintf(CALLBACK_OUTPUT, "[*] Current state: %s\n", 
                                                             currentlyEnabled ? "Enabled" : 
                                                             currentlyDisabled ? "Disabled" : "Unknown");
                                                
                                                // check if already in desired state
                                                if ((enable && currentlyEnabled) || (!enable && currentlyDisabled)) {
                                                    BeaconPrintf(CALLBACK_OUTPUT, "[!] Binding already %s\n", enable ? "enabled" : "disabled");
                                                    hr = S_FALSE; // already in desired state
                                                } else {
                                                    // toggle it
                                                    hr = pPath->lpVtbl->Enable(pPath, enable);
                                                    if (SUCCEEDED(hr)) {
                                                        BeaconPrintf(CALLBACK_OUTPUT, "[*] Successfully %s %s on adapter\n", enable ? "enabled" : "disabled", bindingId);
                                                        
                                                        // apply changes
                                                        hr = pnc->lpVtbl->Apply(pnc);
                                                        if (SUCCEEDED(hr)) {
                                                            BeaconPrintf(CALLBACK_OUTPUT, "[+] Changes applied successfully\n");
                                                        } else {
                                                            BeaconPrintf(CALLBACK_ERROR, "[X] Apply failed: 0x%08X\n", hr);
                                                        }
                                                    } else {
                                                        BeaconPrintf(CALLBACK_ERROR, "[X] Enable/Disable failed: 0x%08X\n", hr);
                                                    }
                                                }
                                                
                                                OLE32$CoTaskMemFree(upperId);
                                            } else {
                                                OLE32$CoTaskMemFree(upperId);
                                            }
                                        }
                                    }
                                }
                                
                                if (pUpper) pUpper->lpVtbl->Release(pUpper);
                                if (pLower) pLower->lpVtbl->Release(pLower);
                                pBindingInterface->lpVtbl->Release(pBindingInterface);
                            }
                            pEnumIf->lpVtbl->Release(pEnumIf);
                        }
                        pPath->lpVtbl->Release(pPath);
                    }
                    pBindingPath->lpVtbl->Release(pBindingPath);
                }
                pBind->lpVtbl->Release(pBind);
            }
        }
        
        if (adapterId) OLE32$CoTaskMemFree(adapterId);
        if (adapterName) OLE32$CoTaskMemFree(adapterName);
        
        if (!adapterFound) {
            // release this adapter and continue searching
            pAdapter->lpVtbl->Release(pAdapter);
            pAdapter = NULL;
        } else {
            // found adapter; break the loop
            break;
        }
    }

    if (!adapterFound) {
        BeaconPrintf(CALLBACK_ERROR, "[X] Adapter '%s' not found\n", adapterIdentifier);
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    } else if (!bindingFound) {
        BeaconPrintf(CALLBACK_ERROR, "[X] Binding '%s' not found on adapter '%s'\n", bindingId, adapterIdentifier);
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

cleanup:
    if (pAdapter) pAdapter->lpVtbl->Release(pAdapter);
    if (pEnum) pEnum->lpVtbl->Release(pEnum);
    if (pClass) pClass->lpVtbl->Release(pClass);
    
    if (pnc) {
        pnc->lpVtbl->Uninitialize(pnc);
        pnc->lpVtbl->Release(pnc);
    }
    
    // release lock only if acquired
    if (pLock) {
        if (lockAcquired) {
            pLock->lpVtbl->ReleaseWriteLock(pLock);
        }
        pLock->lpVtbl->Release(pLock);
    }
    
    OLE32$CoUninitialize();
    return hr;
}

void go (IN PCHAR Buffer, IN ULONG Length) 
{
    datap parser = {0};
    BeaconDataParse(&parser, Buffer, Length);
    char *adapterId = BeaconDataExtract(&parser, NULL);
    char *bindingId = BeaconDataExtract(&parser, NULL);
    char *action = BeaconDataExtract(&parser, NULL);

    if (!adapterId || !bindingId || !action) {
        BeaconPrintf(CALLBACK_ERROR, 
            "[X] Usage: toggle_binding <adapter_id_or_name> <binding_id> <enable|disable>\n"
            "    Example: toggle_binding ms_ndiswanbh insecure_npcap disable\n"
            "    Example: toggle_binding \"WAN Miniport (Network Monitor)\" ms_pacer enable\n");
        return;
    }

    if (MSVCRT$strcmp(action, "enable") != 0 && MSVCRT$strcmp(action, "disable") != 0) {
        BeaconPrintf(CALLBACK_ERROR, "[X] Invalid action '%s'. Use 'enable' or 'disable'\n", action);
        return;
    }

    BOOL enable = (MSVCRT$strcmp(action, "enable") == 0);
    BeaconPrintf(CALLBACK_OUTPUT, "[*] Attempting to %s binding '%s' on adapter '%s'\n", enable ? "enable" : "disable", bindingId, adapterId);
    
    HRESULT hr = ToggleBindingOnAdapter(adapterId, bindingId, enable);
    if (SUCCEEDED(hr)) {
        if (hr == S_FALSE) {
            BeaconPrintf(CALLBACK_OUTPUT, "[!] No change needed (already in desired state)\n");
        } else {
            BeaconPrintf(CALLBACK_OUTPUT, "[+] Operation completed successfully\n");
        }
    } else {
        BeaconPrintf(CALLBACK_ERROR, "[X] Operation failed: 0x%08X\n", hr);
    }
}

