// WIN32/C++17: DX11 DXGI Screen capture sample
//
// References:
//  - https://github.com/microsoftarchive/msdn-code-gallery-microsoft/tree/master/Official%20Windows%20Platform%20Sample/DXGI%20desktop%20duplication%20sample
//  - https://github.com/microsoft/DirectXTex/blob/master/ScreenGrab/ScreenGrab11.cpp
//  - https://docs.microsoft.com/en-us/windows/win32/direct3ddxgi/desktop-dup-api
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atlbase.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <vector>
#include <iostream>
#include <cstdint>
#include "../capturescreen.hpp"
#include <algorithm>
#include <dxgi.h>
#include <cmath>

#pragma comment(lib, "D3D11.lib")

Dev dev;
ID3D11Device* device = dev.device;
ID3D11DeviceContext* deviceContext = dev.deviceContext;
DXGI_OUTDUPL_DESC duplDesc;





#define Report(x) { printf("Report: %s(%d), hr=0x%08x\n", __FILE__, __LINE__, (x)); }



std::vector<RGB> getPixelsAboveCenter(const Image& image, int xoffset) {

    std::vector<RGB> pixelsStraightUp;
    pixelsStraightUp.clear();

    double centerX = image.width / 2;
    double centerY = image.height / 2;
    centerX = round(centerX) + xoffset;
    centerY = round(centerY);

    int startY = 0; 
    int endY = centerY; 

   
    startY = max(0, startY);
    endY = min(endY, image.height - 1);

    for (int y = startY; y <= endY; y++) {
        int rowOffset = y * image.rowPitch;
        int offset = rowOffset + (centerX * 4);

        uint8_t blue = image.bytes[offset];
        uint8_t green = image.bytes[offset + 1];
        uint8_t red = image.bytes[offset + 2];

        pixelsStraightUp.push_back(RGB{ red, green, blue });
    }
    
    return pixelsStraightUp;

    
    
}

std::vector<RGB> getPixelsBelowCenter(const Image& image, int xoffset) {
    
    std::vector<RGB> pixelsStraightdown;
    pixelsStraightdown.clear();
    double centerX = image.width / 2;
    double centerY = image.height / 2;
    centerX = round(centerX) + xoffset;
    centerY = round(centerY);

    int startY = image.height - 1; 
    int endY = centerY; 

   


    for (int y = startY; y >= endY; y--) {
        int rowOffset = y * image.rowPitch;
        int offset = rowOffset + (centerX * 4);

        uint8_t blue = image.bytes[offset];
        uint8_t green = image.bytes[offset + 1];
        uint8_t red = image.bytes[offset + 2];

        pixelsStraightdown.push_back(RGB{ red, green, blue });

    }
    int size = pixelsStraightdown.size();
    
    return pixelsStraightdown;
 
    
}



Image captureDesktop() {

    if (device == nullptr) { Report(E_FAIL); return {}; }

    // Create tex2dStaging which represents duplication image of desktop.
    CComPtr<ID3D11Texture2D> tex2dStaging;
    {
        OutputDuplication od(device);
        IDXGIOutputDuplication* outputDuplication = od.outputDuplication;
        if (outputDuplication == nullptr) { Report(E_FAIL); return {}; }

        AcquiredDesktopImage adi(outputDuplication);
        ID3D11Texture2D* acquiredDesktopImage = adi.acquiredDesktopImage;

        if (acquiredDesktopImage == nullptr) { Report(E_FAIL); return {}; }

        outputDuplication->GetDesc(&duplDesc);

        const auto f = static_cast<int>(duplDesc.ModeDesc.Format);
        const auto goodFormat = f == DXGI_FORMAT_B8G8R8A8_UNORM
            || f == DXGI_FORMAT_B8G8R8X8_UNORM
            || f == DXGI_FORMAT_B8G8R8A8_TYPELESS
            || f == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
            || f == DXGI_FORMAT_B8G8R8X8_TYPELESS
            || f == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
        if (!goodFormat) { Report(E_FAIL); return {}; }

        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = duplDesc.ModeDesc.Width;
        desc.Height = duplDesc.ModeDesc.Height;
        desc.Format = duplDesc.ModeDesc.Format;
        desc.ArraySize = 1;
        desc.BindFlags = 0;
        desc.MiscFlags = 0;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.MipLevels = 1;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.Usage = D3D11_USAGE_STAGING;
        const auto hr = device->CreateTexture2D(&desc, nullptr, &tex2dStaging);
        if (FAILED(hr)) { Report(hr); return {}; }
        if (tex2dStaging == nullptr) { Report(E_FAIL); return {}; }

        deviceContext->CopyResource(tex2dStaging, acquiredDesktopImage);
    }

    // Lock tex2dStaging and copy its content from GPU to CPU memory.
    Image image;

    D3D11_TEXTURE2D_DESC desc;
    tex2dStaging->GetDesc(&desc);

    D3D11_MAPPED_SUBRESOURCE res;
    const auto hr = deviceContext->Map(
        tex2dStaging,
        D3D11CalcSubresource(0, 0, 0),
        D3D11_MAP_READ,
        0,
        &res
    );
    if (FAILED(hr)) { Report(hr); return {}; }
    image.width = static_cast<int>(desc.Width);
    image.height = static_cast<int>(desc.Height);
    image.rowPitch = res.RowPitch;
    image.bytes.resize(image.rowPitch * image.height);
    memcpy(image.bytes.data(), res.pData, image.bytes.size());
    deviceContext->Unmap(tex2dStaging, 0);
    return image;
}



