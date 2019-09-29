//
// Copyright(c) 2016 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "OverlayThread.h"
#include "RecordingState.h"
#include "Utility/Constants.h"
#include "Utility/MessageLog.h"
#include "Utility/ProcessHelper.h"
#include "Utility/StringUtils.h"

#include <processthreadsapi.h>
#include <string>

#define MMAPSIZE 4096
#define MMAPSIZE_SCREENSHOT 1048576

namespace GameOverlay
{

    HWND g_windowHandle = NULL;

    OverlayThread::~OverlayThread ()
    {
        Stop ();
    }

    void OverlayThread::Stop ()
    {
        HANDLE thread = reinterpret_cast<HANDLE> (overlayThread_.native_handle ());
        if (thread)
        {
            const auto threadID = GetThreadId (thread);
            if (overlayThread_.joinable ())
            {
                this->quit_ = true;
                overlayThread_.join ();
            }
        }
    }

    void OverlayThread::Start ()
    {
        g_messageLog.LogInfo ("OverlayThread", "Start overlay thread ");
        quit_ = false;
        overlayThread_ = std::thread ([this] { this->ThreadProc (); });
    }

    void OverlayThread::ThreadProc ()
    {
        RecordingState::GetInstance ().Start ();

        int pid = GetCurrentProcessId();

        std::string fileMapName = std::string();
        char fileName_array[64];

        fileMapName = "Global\\GameOverlayMap";
        if (pid > 0){
            fileMapName.append("_") ;
            fileMapName.append(std::to_string(pid)) ;
        }

        std::wstring stemp = std::wstring(fileMapName.begin(), fileMapName.end());
        LPCWSTR sw = stemp.c_str();


        CopyMemory (fileName_array, fileMapName.c_str(), (fileMapName.size() + 1) * sizeof (char));
        RecordingState::GetInstance ().SetOverlayMessage (fileName_array);

        HANDLE mapFile =
            OpenFileMapping (FILE_MAP_ALL_ACCESS, FALSE, sw);

        // *******************************************************************

        fileMapName = "Global\\GameOverlayImageMap";
        if (pid > 0){
            fileMapName.append("_") ;
            fileMapName.append(std::to_string(pid)) ;
        }

        stemp = std::wstring(fileMapName.begin(), fileMapName.end());
        sw = stemp.c_str();

        CopyMemory (fileName_array, fileMapName.c_str(), (fileMapName.size() + 1) * sizeof (char));
        RecordingState::GetInstance ().SetOverlayMessage (fileName_array);

        HANDLE mapImageFile =
            OpenFileMapping (FILE_MAP_ALL_ACCESS, FALSE, sw);

        // *******************************************************************

        fileMapName = "Global\\GameOverlayCommandMap";
        if (pid > 0){
            fileMapName.append("_") ;
            fileMapName.append(std::to_string(pid)) ;
        }

        stemp = std::wstring(fileMapName.begin(), fileMapName.end());
        sw = stemp.c_str();

        CopyMemory (fileName_array, fileMapName.c_str(), (fileMapName.size() + 1) * sizeof (char));
        RecordingState::GetInstance ().SetOverlayMessage (fileName_array);

        HANDLE mapCommandFile =
            OpenFileMapping (FILE_MAP_ALL_ACCESS, FALSE, sw);

        // *******************************************************************


        if ((mapFile == NULL) || (mapImageFile == NULL) || (mapCommandFile == NULL))
        {
            g_messageLog.LogError ("OverlayThread", "Failed to open image file mapping", GetLastError ());
        }
        g_messageLog.LogInfo ("OverlayThread", "Opened mapped files");


        // declaring character array
        char fileMapName_c[64];
        strcpy_s(fileMapName_c, fileMapName.c_str());

        // Make the unique filename available for the screenshot hooks.
        RecordingState::GetInstance ().SetScreenshotFilename( fileMapName_c);


        while (!this->quit_)
        {
            if (mapFile && mapImageFile && mapCommandFile)
            {
                char *buf_command = (char *)MapViewOfFile (mapCommandFile, FILE_MAP_ALL_ACCESS, 0, 0, MMAPSIZE);

                if (buf_command)
                {
                    // If we read screenshot in the message queue then we should do a screenshot
                    if (strcmp(buf_command, "SCREENSHOT")==0){
                        // do screenshot
                        RecordingState::GetInstance ().SetScreenshotCommand(true);

                        // Tell caller that the image is ready
                        char message[5] = "WAIT" ;
                        CopyMemory ((PVOID)buf_command, message, (strlen (message) + 1) * sizeof (char));

                    // We wait until the screenshot is ready
                    } else if (strcmp(buf_command, "WAIT") == 0)
                    {
                        if (RecordingState::GetInstance ().GetScreenshotReady())
                        {
                            // Tell caller that the image is ready
                            char message[5] = "READ" ;
                            CopyMemory ((PVOID)buf_command, message, (strlen (message) + 1) * sizeof (char));
                        }
                    }

                    UnmapViewOfFile (buf_command);

                }

                // No command is given so print the message on the screen
                char *buf_text = (char *)MapViewOfFile (mapFile, FILE_MAP_ALL_ACCESS, 0, 0, MMAPSIZE);

                if (buf_text)
                {
                    RecordingState::GetInstance ().SetOverlayMessage (buf_text);
                    UnmapViewOfFile (buf_text);
                }


            }
            else
            {
                g_messageLog.LogError ("OverlayThread", "Failed to read from mapped file");
            }
        }
        Sleep (200);
    }
}
