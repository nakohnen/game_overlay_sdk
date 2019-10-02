#ifndef MONITOR
#define MONITOR

#define _WIN32_DCOM

#include <logger\spdlog.h>
#include <windows.h>
#include <string>

class Monitor
{
public:
    static std::shared_ptr<spdlog::logger> monitorLogger;
    static void SetLogLevel (int level);

    Monitor ();
    ~Monitor ();

    int StartMonitor (char *processName, char *dllLoc);
    int RunProcess (char *exePath, char *args, char *dllLoc);
    int ReleaseResources ();
    void Callback (int pid, char *pName);
    int GetPid ();
    int SendMessageToOverlay (char *message);

    int GetCommandFromOverlay(char *retArray) ;
    int SendCommandToOverlay (char *message) ;
    int RequestScreenshot() ;

private:
    volatile HANDLE thread;
    volatile HANDLE createEvent;
    volatile HANDLE stopEvent;
    volatile HANDLE mapFile;
    volatile HANDLE mapImageFile;
    volatile HANDLE mapCommandFile;
    volatile char processName[1024];
    volatile char dllLoc[1024];
    volatile char exePath[1024];
    volatile int pid;
    volatile HANDLE processHandle;

    bool RegisterCreationCallback ();
    static DWORD WINAPI ThreadProc (LPVOID lpParameter);
    void WorkerThread ();
    int GetArchitecture (int pid);
    HANDLE GetProcessHandleFromID (DWORD id, DWORD access);
    int CreateDesktopProcess (char *path, char *cmdArgs);
    int CreateFileMap ();
    int CreateFileMap (int pid) ;
    bool CheckTargetProcessAlive ();

    HANDLE CreateFileMapHelper (std::string name, int pid, int map_size) ;
};

#endif
