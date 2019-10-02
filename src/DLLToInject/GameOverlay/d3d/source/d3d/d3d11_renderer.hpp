// Copyright 2016 Patrick Mours.All rights reserved.
//
// https://github.com/crosire/GameOverlay
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met :
//
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and / or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDER ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT
// SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once


#include <d3d11.h>
#include <vector>
#include <wrl.h>
#include <functional>

#include "Rendering/OverlayBitmap.h"

using Microsoft::WRL::ComPtr;

namespace GameOverlay
{
    enum class InitializationStatus
    {
        DEFERRED_CONTEXT_INITIALIZED,
        IMMEDIATE_CONTEXT_INITIALIZED,
        UNINITIALIZED
    };

    inline DXGI_FORMAT EnsureNotTypeless(DXGI_FORMAT fmt)
    {
        // Assumes UNORM or FLOAT; doesn't use UINT or SINT
        switch (fmt)
        {
            case DXGI_FORMAT_R32G32B32A32_TYPELESS: return DXGI_FORMAT_R32G32B32A32_FLOAT;
            case DXGI_FORMAT_R32G32B32_TYPELESS:    return DXGI_FORMAT_R32G32B32_FLOAT;
            case DXGI_FORMAT_R16G16B16A16_TYPELESS: return DXGI_FORMAT_R16G16B16A16_UNORM;
            case DXGI_FORMAT_R32G32_TYPELESS:       return DXGI_FORMAT_R32G32_FLOAT;
            case DXGI_FORMAT_R10G10B10A2_TYPELESS:  return DXGI_FORMAT_R10G10B10A2_UNORM;
            case DXGI_FORMAT_R8G8B8A8_TYPELESS:     return DXGI_FORMAT_R8G8B8A8_UNORM;
            case DXGI_FORMAT_R16G16_TYPELESS:       return DXGI_FORMAT_R16G16_UNORM;
            case DXGI_FORMAT_R32_TYPELESS:          return DXGI_FORMAT_R32_FLOAT;
            case DXGI_FORMAT_R8G8_TYPELESS:         return DXGI_FORMAT_R8G8_UNORM;
            case DXGI_FORMAT_R16_TYPELESS:          return DXGI_FORMAT_R16_UNORM;
            case DXGI_FORMAT_R8_TYPELESS:           return DXGI_FORMAT_R8_UNORM;
            case DXGI_FORMAT_BC1_TYPELESS:          return DXGI_FORMAT_BC1_UNORM;
            case DXGI_FORMAT_BC2_TYPELESS:          return DXGI_FORMAT_BC2_UNORM;
            case DXGI_FORMAT_BC3_TYPELESS:          return DXGI_FORMAT_BC3_UNORM;
            case DXGI_FORMAT_BC4_TYPELESS:          return DXGI_FORMAT_BC4_UNORM;
            case DXGI_FORMAT_BC5_TYPELESS:          return DXGI_FORMAT_BC5_UNORM;
            case DXGI_FORMAT_B8G8R8A8_TYPELESS:     return DXGI_FORMAT_B8G8R8A8_UNORM;
            case DXGI_FORMAT_B8G8R8X8_TYPELESS:     return DXGI_FORMAT_B8G8R8X8_UNORM;
            case DXGI_FORMAT_BC7_TYPELESS:          return DXGI_FORMAT_BC7_UNORM;
            default:                                return fmt;
        }
    }


    class d3d11_renderer final
    {
    public:
        d3d11_renderer (ID3D11Device *device, IDXGISwapChain *swapchain);
        d3d11_renderer (ID3D11Device *device,
            std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> renderTargets_,
            int backBufferWidth, int backBufferHeight);
        ~d3d11_renderer ();


        bool on_present ();
        bool on_present (int backBufferIndex);

        D3D11_VIEWPORT GetViewport ()
        {
            return viewPort_;
        }
        void HandleScreenshot();

    private:
        bool CreateOverlayRenderTarget ();
        bool CreateOverlayTexture ();
        bool CreateOverlayResources (int backBufferWidth, int backBufferHeight);
        bool RecordOverlayCommandList ();

        void CopyOverlayTexture ();
        bool UpdateOverlayPosition ();
        void UpdateOverlayTexture ();

        Microsoft::WRL::ComPtr<ID3D11Device> device_;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
        Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain_;

        Microsoft::WRL::ComPtr<ID3D11VertexShader> overlayVS_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> overlayPS_;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingTexture_;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> displayTexture_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> displaySRV_;
        Microsoft::WRL::ComPtr<ID3D11Buffer> viewportOffsetCB_;
        std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> renderTargets_;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_;
        Microsoft::WRL::ComPtr<ID3D11BlendState> blendState_;
        std::vector<Microsoft::WRL::ComPtr<ID3D11CommandList>> overlayCommandList_;
        D3D11_VIEWPORT viewPort_;
        std::unique_ptr<OverlayBitmap> overlayBitmap_;

        InitializationStatus status = InitializationStatus::UNINITIALIZED;

        HRESULT d3d11_renderer::SaveWICTextureToFile(ID3D11DeviceContext* pContext,
                                                     ID3D11Resource* pSource,
                                                     REFGUID guidContainerFormat,
                                                     const wchar_t* fileName,
                                                     const GUID* targetFormat,
                                                     std::function<void(IPropertyBag2*)> setCustomProps,
                                                     bool forceSRGB);
        HRESULT d3d11_renderer::CaptureTexture( _In_ ID3D11DeviceContext* pContext,
                                                _In_ ID3D11Resource* pSource,
                                                D3D11_TEXTURE2D_DESC& desc,
                                                ComPtr<ID3D11Texture2D>& pStaging);
        bool _IsWIC2();
        IWICImagingFactory* _GetWIC();
        // Also used by ScreenGrab

    };

    class auto_delete_file_wic
    {
    public:
        auto_delete_file_wic(Microsoft::WRL::ComPtr<IWICStream>& hFile, LPCWSTR szFile) : m_filename(szFile), m_handle(hFile) {}

        auto_delete_file_wic(const auto_delete_file_wic&) = delete;
        auto_delete_file_wic& operator=(const auto_delete_file_wic&) = delete;

        ~auto_delete_file_wic()
        {
            if (m_filename)
            {
                m_handle.Reset();
                DeleteFileW(m_filename);
            }
        }

        void clear() { m_filename = nullptr; }

    private:
        LPCWSTR m_filename;
        Microsoft::WRL::ComPtr<IWICStream>& m_handle;
    };
}
