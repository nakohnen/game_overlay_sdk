#include <Wbemidl.h>
#include <accctrl.h>
#include <aclapi.h>
#include <atlcomcli.h>
#include <atlstr.h>
#include <comdef.h>
#include <string.h>

#include "MonitorProcessCreation.h"

#include "DLLInjection.h"
#include "EventSink.h"
#include "FileUtils.h"
#include "Monitor.h"
#include "StringUtils.h"
#include "SuspendThreads.h"
#include "Win32Handle.h"

#pragma comment(lib, "wbemuuid.lib")

#define MMAPSIZE 4096
#define MMAPSIZE_SCREENSHOT 1048576

std::shared_ptr<spdlog::logger> Monitor::monitorLogger = spdlog::stderr_logger_mt ("monitorLogger");

Monitor::Monitor ()
{
    this->thread = NULL;
    this->createEvent = NULL;
    this->stopEvent = NULL;

    this->mapFile = NULL;
    this->mapImageFile = NULL;
    this->mapCommandFile = NULL;

    this->pid = 0;
    this->processHandle = NULL;
}

Monitor::~Monitor ()
{
    Monitor::monitorLogger->error ("Destructor called. grml...");
    this->ReleaseResources ();
}

void Monitor::SetLogLevel (int level)
{
    int log_level = level;
    if (level > 6)
        log_level = 6;
    if (level < 0)
        log_level = 0;
    Monitor::monitorLogger->set_level (spdlog::level::level_enum (log_level));
    Monitor::monitorLogger->flush_on (spdlog::level::level_enum (log_level));
    DLLInjection::SetLogLevel (level);
}

int Monitor::RunProcess (char *exePath, char *args, char *dllLoc)
{
    strcpy_s ((char *)this->dllLoc, 1023, dllLoc);
    strcpy_s ((char *)this->exePath, 1023, exePath);
    int res = CreateDesktopProcess (exePath, args);
    if (res != STATUS_OK)
    {
        return res;
    }
    res = CreateFileMap (pid);
    if (res != STATUS_OK)
    {
        return res;
    }
    int architecture = GetArchitecture (pid);
    DLLInjection dllInjection (this->pid, architecture, dllLoc);
    bool is_injected = dllInjection.InjectDLL ();
    ResumeThread (this->thread);
    if (!is_injected)
    {
        Monitor::monitorLogger->error ("Failed to inject dll");
        return GENERAL_ERROR;
    }
    this->processHandle = dllInjection.GetTargetProcessHandle ();
    return STATUS_OK;
}

int Monitor::StartMonitor (char *processName, char *dllLoc)
{
    strcpy_s ((char *)this->processName, 1023, processName);
    strcpy_s ((char *)this->dllLoc, 1023, dllLoc);
    Monitor::monitorLogger->trace ("start monitoring");
    int res = CreateFileMap (0);
    if (res != STATUS_OK)
    {
        return res;
    }
    this->createEvent = CreateEvent (NULL, TRUE, FALSE, TEXT ("createEvent"));
    this->stopEvent = CreateEvent (NULL, TRUE, FALSE, TEXT ("stopEvent"));
    if ((!this->createEvent) && (!this->stopEvent))
    {
        Monitor::monitorLogger->error ("Failed to create event");
        return GENERAL_ERROR;
    }
    this->thread = CreateThread (NULL, 0, Monitor::ThreadProc, this, 0, NULL);
    DWORD dwWait = WaitForSingleObject (createEvent, 3000);
    if (dwWait == WAIT_TIMEOUT)
    {
        Monitor::monitorLogger->error ("Failed to create monitoring thread");
        return GENERAL_ERROR;
    }
    return STATUS_OK;
}

HANDLE Monitor::CreateFileMapHelper(std::string name, int pid, int mapsize) {
    // Create mmp where the overlay text gets written to
    std::string fileMapName = std::string(name);
    if (pid > 0){
        fileMapName.append("_") ;
        fileMapName.append(std::to_string(pid)) ;
    }
    Monitor::monitorLogger->info ("mmapped file created: {}", fileMapName);
    return CreateFileMapping (INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, mapsize, fileMapName.c_str());
}

int Monitor::CreateFileMap (int pid)
{
    this->mapFile = CreateFileMapHelper("Global\\GameOverlayMap", pid, MMAPSIZE) ;
    this->mapImageFile = CreateFileMapHelper("Global\\GameOverlayImageMap", pid, MMAPSIZE_SCREENSHOT) ;
    this->mapCommandFile = CreateFileMapHelper("Global\\GameOverlayCommandMap", pid, MMAPSIZE) ;

    if ((this->mapFile == NULL) || (this->mapImageFile == NULL) || (this->mapCommandFile == NULL))
    {
        Monitor::monitorLogger->error ("failed to create mmapped file : Error {}", GetLastError ());
        return GENERAL_ERROR;
    } else {
        Monitor::monitorLogger->info ("mmapped files successfully created");
    }
    SetSecurityInfo (this->mapFile, SE_KERNEL_OBJECT,
        DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION, NULL, NULL, NULL, NULL);

    SetSecurityInfo (this->mapImageFile, SE_KERNEL_OBJECT,
        DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION, NULL, NULL, NULL, NULL);
    return STATUS_OK;
}

int Monitor::CreateFileMap (){
    return this->CreateFileMap(0);
}

int Monitor::ReleaseResources ()
{
    int result = STATUS_OK;
    if (this->CheckTargetProcessAlive ())
    {
        // this code looks good but there is no DLL_PROCESS_DETACH call in target process
        int architecture = GetArchitecture (this->pid);
        DLLInjection dllInjection (this->pid, architecture, (char *)this->dllLoc);
        SuspendAllThreads (pid);
        bool res = dllInjection.FreeDLL ();
        if (!res)
        {
            Monitor::monitorLogger->error ("failed to free library");
            result = GENERAL_ERROR;
        }
        ResumeAllThreads (pid);
    }
    Monitor::monitorLogger->trace ("releasing");
    if (this->thread)
    {
        if (this->stopEvent)
        {
            SetEvent (this->stopEvent);
            WaitForSingleObject (this->thread, INFINITE);
        }
    }
    if (this->thread)
    {
        CloseHandle (this->thread);
        this->thread = NULL;
    }
    if (this->createEvent)
    {
        CloseHandle (this->createEvent);
        this->createEvent = NULL;
    }
    if (this->stopEvent)
    {
        CloseHandle (this->stopEvent);
        this->stopEvent = NULL;
    }
    if (this->mapFile)
    {
        CloseHandle (this->mapFile);
        this->mapFile = NULL;
    }

    this->pid = 0;
    this->processHandle = NULL;
    return result;
}

int Monitor::GetPid ()
{
    return this->pid;
}

int Monitor::SendMessageToOverlay (char *message)
{
    //Monitor::monitorLogger->info ("received message to send to overlay");
    if ((this->mapFile == NULL) || (this->pid == 0))
    {
        Monitor::monitorLogger->error ("No process to send message to");
        return TARGET_PROCESS_IS_NOT_CREATED_ERROR;
    }
    if (!CheckTargetProcessAlive ())
    {
        Monitor::monitorLogger->error ("target process was terminated, don't send message");
        this->pid = 0;
        this->processHandle = NULL;
        return TARGET_PROCESS_WAS_TERMINATED_ERROR;
    }
    //Monitor::monitorLogger->info ("sending message to {}", this->pid);
    Monitor::monitorLogger->trace ("sending message '{}' to {}", message, this->pid);
    char *buf = (char *)MapViewOfFile (this->mapFile, FILE_MAP_WRITE, 0, 0, MMAPSIZE);
    if (buf == NULL)
    {
        Monitor::monitorLogger->error ("failed to create MapViewOfFile {}", GetLastError ());
        return GENERAL_ERROR;
    }
    CopyMemory ((PVOID)buf, message, (strlen (message) + 1) * sizeof (char));
    UnmapViewOfFile (buf);
    //Monitor::monitorLogger->info ("data copied and finished");
    return STATUS_OK;
}

int Monitor::SendCommandToOverlay (char *message)
{
    if ((this->mapCommandFile == NULL) || (this->pid == 0))
    {
        Monitor::monitorLogger->error ("No process to send command to");
        return TARGET_PROCESS_IS_NOT_CREATED_ERROR;
    }
    if (!CheckTargetProcessAlive ())
    {
        Monitor::monitorLogger->error ("target process was terminated, don't send message");
        this->pid = 0;
        this->processHandle = NULL;
        return TARGET_PROCESS_WAS_TERMINATED_ERROR;
    }
    monitorLogger->trace ("sending message '{}' to {}", message, this->pid);
    char *buf = (char *)MapViewOfFile (this->mapCommandFile, FILE_MAP_WRITE, 0, 0, MMAPSIZE);
    if (buf == NULL)
    {
        monitorLogger->error ("failed to create MapViewOfFile {}", GetLastError ());
        return GENERAL_ERROR;
    }
    CopyMemory ((PVOID)buf, message, (strlen (message) + 1) * sizeof (char));
    UnmapViewOfFile (buf);
    return STATUS_OK;
}

int Monitor::RequestScreenshot(){
    char message[64] = "SCREENSHOT" ;
    return SendCommandToOverlay(message);
}

int Monitor::GetCommandFromOverlay(char * retArray){
    if ((this->mapCommandFile == NULL) || (this->pid == 0))
    {
        Monitor::monitorLogger->error ("No process to send command to");
        return 1;
    }
    if (!CheckTargetProcessAlive ())
    {
        Monitor::monitorLogger->error ("target process was terminated, don't send message");
        this->pid = 0;
        this->processHandle = NULL;
        return 1;
    }

    monitorLogger->trace ("reading command from {}", this->pid);

    char *buf = (char *)MapViewOfFile (this->mapCommandFile, FILE_MAP_WRITE, 0, 0, MMAPSIZE);

    if (buf == NULL)
    {
        monitorLogger->error ("failed to create MapViewOfFile {}", GetLastError ());
        return 1;
    }
    CopyMemory (retArray, (PVOID)buf, 64 * sizeof (char));
    UnmapViewOfFile (buf);

    return 0;
}

void Monitor::Callback (int pid, char *pName)
{
    if (strcmp (pName, (char *)this->processName) == 0)
    {
        if (this->pid != 0)
        {
            if (!this->CheckTargetProcessAlive ())
            {
                Monitor::monitorLogger->info (
                    "Previous process was terminated running callback for the new one");
            }
            else
            {
                Monitor::monitorLogger->warn (
                    "Only single process is allowed, dont injectdll to new process");
                return;
            }
        }
        int architecture = GetArchitecture (pid);
        Monitor::monitorLogger->info (
            "Target Process created, name {}, pid {}, architecture {}", pName, pid, architecture);
        DLLInjection dllInjection (pid, architecture, (char *)this->dllLoc);
        SuspendAllThreads (pid);
        bool res = dllInjection.InjectDLL ();
        ResumeAllThreads (pid);
        if (!res)
        {
            Monitor::monitorLogger->error ("Failed to inject dll to {}", pName);
        }
        else
        {
            this->processHandle = dllInjection.GetTargetProcessHandle ();
            this->pid = pid;
        }
    }
    else
    {
        monitorLogger->trace ("Non Target Process created, name {}, pid {}", pName, pid);
    }
}

DWORD WINAPI Monitor::ThreadProc (LPVOID pMonitor)
{
    ((Monitor *)pMonitor)->WorkerThread ();
    return 0;
}

void Monitor::WorkerThread ()
{
    this->RegisterCreationCallback ();
    if (!SetEvent (this->createEvent))
    {
        Monitor::monitorLogger->error ("unable to set event");
        return;
    }
    WaitForSingleObject (this->stopEvent, INFINITE);
    Monitor::monitorLogger->trace ("monitoring thread was stopped");
}

bool Monitor::RegisterCreationCallback ()
{
    CComPtr<IWbemLocator> pLoc;
    CoInitializeEx (0, COINIT_MULTITHREADED);
    HRESULT hres = CoCreateInstance (
        CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);

    if (FAILED (hres))
    {
        Monitor::monitorLogger->error ("Failed to create IWbemLocator object error code {}", hres);
        return false;
    }
    CComPtr<EventSink> pSink (new EventSink (this));
    hres = pLoc->ConnectServer (_bstr_t (L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSink->pSvc);
    if (FAILED (hres))
    {
        Monitor::monitorLogger->error ("Failed to ConnectServer error code {}", hres);
        return false;
    }
    hres = CoSetProxyBlanket (pSink->pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
    if (FAILED (hres))
    {
        Monitor::monitorLogger->error ("Failed to CoSetProxyBlanket error code {}", hres);
        return false;
    }

    CComPtr<IUnsecuredApartment> pUnsecApp;
    hres = CoCreateInstance (CLSID_UnsecuredApartment, NULL, CLSCTX_LOCAL_SERVER,
        IID_IUnsecuredApartment, (void **)&pUnsecApp);
    CComPtr<IUnknown> pStubUnk;
    pUnsecApp->CreateObjectStub (pSink, &pStubUnk);
    pStubUnk->QueryInterface (IID_IWbemObjectSink, (void **)&pSink->pStubSink);

    char buffer[512];
    sprintf_s (buffer,
        "SELECT * FROM __InstanceCreationEvent WITHIN 0.001 WHERE TargetInstance ISA "
        "'Win32_Process'");

    hres = pSink->pSvc->ExecNotificationQueryAsync (
        _bstr_t ("WQL"), _bstr_t (buffer), WBEM_FLAG_SEND_STATUS, NULL, pSink->pStubSink);

    if (FAILED (hres))
    {
        Monitor::monitorLogger->error ("Failed to ExecNotificationQueryAsync error code {}", hres);
        return false;
    }
    return true;
}

// if failed 64bit is assumed!
int Monitor::GetArchitecture (int pid)
{
    const auto wow64FunctionAdress =
        GetProcAddress (GetModuleHandle ("kernel32"), "IsWow64Process");
    if (!wow64FunctionAdress)
    {
        Monitor::monitorLogger->error ("IsWow64Process function not found in kernel32");
        return X64;
    }

    BOOL wow64Process = true;
    Win32Handle processHandle = GetProcessHandleFromID (pid, PROCESS_QUERY_INFORMATION);
    if (!processHandle.Get ())
    {
        Monitor::monitorLogger->error ("can not get handle from pid");
        return X64;
    }
    if (!IsWow64Process (processHandle.Get (), &wow64Process))
    {
        Monitor::monitorLogger->error ("IsWow64Process failed, error {}", GetLastError ());
        return X64;
    }

    if (wow64Process)
        return X86;
    else
        return X64;
}

HANDLE Monitor::GetProcessHandleFromID (DWORD id, DWORD access)
{
    HANDLE handle = OpenProcess (access, FALSE, id);
    if (!handle)
    {
        return NULL;
    }
    return handle;
}

int Monitor::CreateDesktopProcess (char *path, char *cmdArgs)
{
    Monitor::monitorLogger->info ("Trying to start executable {} with args: {}", path, cmdArgs);
    std::wstring wargs = ConvertUTF8StringToUTF16String (std::string (cmdArgs));
    std::wstring wpath = ConvertUTF8StringToUTF16String (std::string (path));
    const auto directory = GetDirFromPathSlashes (wpath);
    if (directory.empty ())
    {
        return PATH_NOT_FOUND_ERROR;
    }

    std::wstring wcommandLine = L"\"" + wpath + L"\" " + wargs;
    std::string commandLine = ConvertUTF16StringToUTF8String (wcommandLine);
    STARTUPINFO startupInfo {};
    PROCESS_INFORMATION processInfo_ {};
    startupInfo.cb = sizeof (startupInfo);

    if (!CreateProcess (NULL, &commandLine[0], NULL, NULL, FALSE, CREATE_SUSPENDED, NULL,
            ConvertUTF16StringToUTF8String (directory).c_str (), &startupInfo, &processInfo_))
    {
        Monitor::monitorLogger->error (
            "Failed to create process, error code is {}", GetLastError ());
        return GENERAL_ERROR;
    }

    // runprocess and monitor mode should not be used together, so it's okay to use this->thread for
    // both modes
    this->pid = processInfo_.dwProcessId;
    this->thread = processInfo_.hThread;
    return STATUS_OK;
}

bool Monitor::CheckTargetProcessAlive ()
{
    if (this->processHandle == NULL)
    {
        return false;
    }
    DWORD exitCode = 0;
    bool status = GetExitCodeProcess (this->processHandle, &exitCode);
    if (exitCode == STILL_ACTIVE)
        return true;
    else
        return false;
}
