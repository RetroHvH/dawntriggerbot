#ifndef SCREEN_CAPTURE_HPP
#define SCREEN_CAPTURE_HPP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atlbase.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <vector>
#include <iostream>
#include <cstdint>

#pragma comment(lib, "D3D11.lib")

#define Report(x)
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define max(a,b)            (((a) > (b)) ? (a) : (b))

struct Dev
{
    CComPtr<ID3D11Device> device;
    CComPtr<ID3D11DeviceContext> deviceContext;
    D3D_FEATURE_LEVEL featureLevel;


    Dev()
    {
        static const D3D_DRIVER_TYPE driverTypes[] = {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE
        };

        static const D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_1
        };

        for (const auto& driverType : driverTypes)
        {
            const auto hr = D3D11CreateDevice(
                nullptr,
                driverType,
                nullptr,
                0,
                featureLevels,
                static_cast<UINT>(std::size(featureLevels)),
                D3D11_SDK_VERSION,
                &device,
                &featureLevel,
                &deviceContext
            );
            if (SUCCEEDED(hr))
            {
                break;
            }
            device.Release();
            deviceContext.Release();
        }
    }
};

struct OutputDuplication
{
    CComPtr<IDXGIOutputDuplication> outputDuplication;

    OutputDuplication(ID3D11Device* device)
    {
        HRESULT hr;

        CComPtr<IDXGIDevice> dxgiDevice;
        hr = device->QueryInterface(
            __uuidof(dxgiDevice),
            reinterpret_cast<void**>(&dxgiDevice)
        );
        if (FAILED(hr))
        {
            Report(hr);
            return;
        }

        CComPtr<IDXGIAdapter> dxgiAdapter;
        hr = dxgiDevice->GetParent(
            __uuidof(dxgiAdapter),
            reinterpret_cast<void**>(&dxgiAdapter)
        );
        if (FAILED(hr))
        {
            Report(hr);
            return;
        }

        CComPtr<IDXGIOutput> dxgiOutput;
        hr = dxgiAdapter->EnumOutputs(0, &dxgiOutput);
        if (FAILED(hr))
        {
            Report(hr);
            return;
        }

        CComPtr<IDXGIOutput1> dxgiOutput1;
        hr = dxgiOutput->QueryInterface(
            __uuidof(IDXGIOutput1),
            reinterpret_cast<void**>(&dxgiOutput1)
        );
        if (FAILED(hr))
        {
            Report(hr);
            return;
        }

        hr = dxgiOutput1->DuplicateOutput(device, &outputDuplication);
        if (FAILED(hr))
        {
            Report(hr);
            return;
        }
    }
};

struct AcquiredDesktopImage
{
    CComPtr<ID3D11Texture2D> acquiredDesktopImage;

    AcquiredDesktopImage(IDXGIOutputDuplication* outputDuplication)
    {
        CComPtr<IDXGIResource> desktopResource;
        HRESULT hr = E_FAIL;
        for (int i = 0; i < 10; ++i)
        {
            DXGI_OUTDUPL_FRAME_INFO fi{};
            const int timeoutMsec = 500; // milliseconds
            hr = outputDuplication->AcquireNextFrame(timeoutMsec, &fi, &desktopResource);
            if (SUCCEEDED(hr) && (fi.LastPresentTime.QuadPart == 0))
            {
                // If AcquireNextFrame() returns S_OK and
                // fi.LastPresentTime.QuadPart == 0, it means
                // AcquireNextFrame() didn't acquire next frame yet.
                // We must wait next frame sync timing to retrieve
                // actual frame data.
                //
                // Since method is successfully completed,
                // we need to release the resource and frame explicitly.
                desktopResource.Release();
                outputDuplication->ReleaseFrame();
                Sleep(1);
                continue;
            }
            else
            {
                break;
            }
        }
        if (FAILED(hr))
        {
            Report(hr);
            return;
        }
        try
        {
            hr = desktopResource->QueryInterface(
                __uuidof(ID3D11Texture2D),
                reinterpret_cast<void**>(&acquiredDesktopImage)
            );
        }
        catch (...)
        {
        }
        if (FAILED(hr))
        {
            Report(hr);
            return;
        }
    }
};

struct Image
{
    std::vector<byte> bytes;
    int width = 0;
    int height = 0;
    int rowPitch = 0;
};

struct RGB
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

std::vector<RGB> getPixelsAboveCenter(const Image& image, int);

std::vector<RGB> getPixelsBelowCenter(const Image& image, int);


Image captureDesktop();


#endif // SCREEN_CAPTURE_HPP
