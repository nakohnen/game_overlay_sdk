#include <mutex>
#include <string.h>
#include <vector>

#include "Monitor.h"
#include "MonitorProcessCreation.h"

Monitor *last_monitor;
std::vector<Monitor> monitors;
//monitors.reserve(5);
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

    monitors.push_back(*monitor) ;
    last_monitor = &(monitors.back()) ;
    return monitor->StartMonitor (processName, dllLoc);
}

int RunProcess (char *exePath, char *args, char *dllLoc)
{
    std::lock_guard<std::mutex> lock (mutex);

    last_monitor = new Monitor();
    monitors.emplace_back() ;
    Monitor *monitor = &(monitors.back());
    if (!monitor)
    {
        Monitor::monitorLogger->error ("failed to create monitor");
        return GENERAL_ERROR;
    }
    last_monitor = monitor ;
    //return last_monitor->RunProcess(exePath, args, dllLoc);
    return monitors.back().RunProcess (exePath, args, dllLoc);
    //return 1;
}

int ReleaseResources ()
{
    std::lock_guard<std::mutex> lock (mutex);

    if (monitors.size() == 0)
    {
        Monitor::monitorLogger->error ("no process monitor is not running");
        return PROCESS_MONITOR_IS_NOT_RUNNING_ERROR;
    }

    int res = 0;

    // using begin() to print vector
    for(auto monitor: monitors)
    {
        res = monitor.ReleaseResources ();
        //delete monitor;
        //monitor = NULL;
    }
    monitors.clear();
    return res;
}

int GetPid (int *pid)
{
    std::lock_guard<std::mutex> lock (mutex);
    if (last_monitor != nullptr)
    {
        Monitor::monitorLogger->error ("process monitor is not running");
        return PROCESS_MONITOR_IS_NOT_RUNNING_ERROR;
    }
    return last_monitor->GetPid();
}

int SendMessageToOverlay (char *message)
{
    std::lock_guard<std::mutex> lock (mutex);
    if (last_monitor == nullptr)
    {
        Monitor::monitorLogger->error ("process monitor is not running");
        return PROCESS_MONITOR_IS_NOT_RUNNING_ERROR;
    }
    return last_monitor->SendMessageToOverlay (message);
}

int SendMessageToOverlayWithPid (int pid, char *message)
{
    std::lock_guard<std::mutex> lock (mutex);
    for(auto it_monitor: monitors)
    {
        if (it_monitor.GetPid() == pid) {
            return it_monitor->SendMessageToOverlay (message);
        }
    }
    Monitor::monitorLogger->error ("process monitor is not running");
    return PROCESS_MONITOR_IS_NOT_RUNNING_ERROR;
}

BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {

        case DLL_PROCESS_DETACH:
        {
            Monitor::monitorLogger->error ("DLL_PROCESS_DETACH called");
            int to_del = -1;
            for (int i=0; i < monitors.size(); i++)
            {
                if (monitors.at(i).GetPid() == last_monitor->GetPid()) {
                    to_del = i;
                    break;
                }
            }

            if (to_del >= 0)
                monitors.erase(monitors.begin() + to_del) ;
            last_monitor = nullptr;

            //ReleaseResources() ;
            //last_monitor = NULL;
            //monitors.clear() ;
            break;
        }
        default:
            break;
    }
    return TRUE;
}
