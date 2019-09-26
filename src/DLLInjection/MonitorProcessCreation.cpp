#include <mutex>
#include <string.h>
#include <unordered_map>

#include "Monitor.h"
#include "MonitorProcessCreation.h"

Monitor *last_monitor = NULL;
std::unordered_map<int, Monitor> monitors*;
std::mutex mutex;

int SetLogLevel (int level)
{
    std::lock_guard<std::mutex> lock (mutex);
    Monitor::SetLogLevel (level);
    return STATUS_OK;
}

int StartMonitor (char *processName, char *dllLoc)
{
    std::lock_guard<std::mutex> lock (mutex);
    Monitor *monitor = NULL;
    monitor = new Monitor ();
    if (!monitor)
    {
        Monitor::monitorLogger->error ("failed to create monitor");
        return GENERAL_ERROR;
    }

    int monitor_pid = monitor->GetPid() ;
    monitors.insert( std::make_pair<int, Monitor>(monitor_pid, monitor) ) ;
    last_monitor = monitor ;

    return monitor->StartMonitor (processName, dllLoc);
}

int RunProcess (char *exePath, char *args, char *dllLoc)
{
    std::lock_guard<std::mutex> lock (mutex);
    Monitor *monitor = NULL;
    monitor = new Monitor ();
    if (!monitor)
    {
        Monitor::monitorLogger->error ("failed to create monitor");
        return GENERAL_ERROR;
    }

    int monitor_pid = monitor->GetPid() ;
    monitors.insert( std::make_pair<int, Monitor>(monitor_pid, monitor) ) ;
    last_monitor = monitor ;

    return monitor->RunProcess (exePath, args, dllLoc);
}

int ReleaseResources ()
{
    std::lock_guard<std::mutex> lock (mutex);

    if (!monitors)
    {
        Monitor::monitorLogger->error ("process monitor is not running");
        return PROCESS_MONITOR_IS_NOT_RUNNING_ERROR;
    }

    int res = 0;

    for (auto x : monitors)
    {
        Monitor *monitor = &x.second;
        res = monitor->ReleaseResources ();
        delete monitor;
        monitor = NULL;
        # return res;
    }

    monitors.clear()
    return res
}

int GetPid (int *pid)
{
    std::lock_guard<std::mutex> lock (mutex);
    if (!last_monitor)
    {
        Monitor::monitorLogger->error ("process monitor is not running");
        return PROCESS_MONITOR_IS_NOT_RUNNING_ERROR;
    }
    *pid = last_monitor->GetPid ();
    if (*pid == 0)
    {
        return TARGET_PROCESS_IS_NOT_CREATED_ERROR;
    }
    return STATUS_OK;
}

int SendMessageToOverlay (char *message)
{
    std::lock_guard<std::mutex> lock (mutex);
    if (!last_monitor)
    {
        Monitor::monitorLogger->error ("process monitor is not running");
        return PROCESS_MONITOR_IS_NOT_RUNNING_ERROR;
    }
    return last_monitor->SendMessageToOverlay (message);
}

int SendMessageToOverlayWithPid (int pid, char *message)
{
    std::lock_guard<std::mutex> lock (mutex);
    Monitor *monitor = monitors.at(pid) ;
    if (!monitor)
    {
        Monitor::monitorLogger->error ("process monitor is not running");
        return PROCESS_MONITOR_IS_NOT_RUNNING_ERROR;
    }
    last_monitor = monitor ;
    return monitor->SendMessageToOverlay (message);
}

BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {

        case DLL_PROCESS_DETACH:
        {
            if (last_monitor) {
                last_monitor->ReleaseResources ();
                delete last_monitor;
                last_monitor = NULL;

                monitors.clear() ;
                delete monitors;

            }
            /*if (last_monitor)
            {
                int pid = last_monitor.GetPid();

                monitors.erase(pid);

                last_monitor->ReleaseResources ();

                delete monitors[pid].second;
                for (auto monitor : monitors) {
                    last_monitor = monitor.second ;
                    break;
                }

            }*/
            break;
        }
        default:
            break;
    }
    return TRUE;
}
