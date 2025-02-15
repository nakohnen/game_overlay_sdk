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

#pragma once

#include <chrono>
#include <string>

enum class TextureState
{
    Default,
    Start,
    Stop
};

class RecordingState final
{
public:
    static RecordingState &GetInstance ();
    RecordingState (RecordingState const &) = delete;
    void operator= (RecordingState const &) = delete;

    bool Started ();
    bool Stopped ();
    bool IsOverlayShowing ();
    void Start ();
    void Stop ();

    TextureState Update ();
    void SetDisplayTimes (float start, float end);
    void SetRecordingTime (float time);
    void HideOverlay ();
    void ShowOverlay ();

    char *GetOverlayMessage ();
    void SetOverlayMessage (char *message);

    void SetScreenshotCommand (bool new_state);
    bool GetScreenshotCommand ();

    void SetScreenshotFilename (char *filename);
    char *GetScreenshotFilename ();

    void SetScreenshotReady (bool new_state);
    bool GetScreenshotReady ();


private:
    RecordingState ();

    bool recording_ = false;
    bool stateChanged_ = false;
    bool showOverlay_ = true;

    bool doScreenshot_ = false ;
    bool screenshotReady_ = false ;

    float startDisplayTime_ = 1.0f;
    float endDisplayTime_ = 1.0f;
    float recordingTime_ = 0.0f;

    volatile char overlayMessage_[2048];
    volatile char screenshotFilename_[2048];

    TextureState currentTextureState_ = TextureState::Default;
    std::chrono::high_resolution_clock::time_point currentStateStart_;
};
