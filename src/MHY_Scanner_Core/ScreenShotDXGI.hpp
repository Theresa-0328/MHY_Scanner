#pragma once

#include <iostream>

#include <d3d11.h>
#include <dxgi1_2.h>
#include <trrlog.hpp>

#pragma comment(lib, "d3d11.lib")

class ScreenShotDXGI
{
public:
    ScreenShotDXGI() :
        m_Device(nullptr),
        m_DeskDupl(nullptr),
        m_AcquiredDesktopImage(nullptr),
        m_AcquiredDesktopImage_copy(nullptr),
        m_monitorIdx(0)
    {
    }
    ~ScreenShotDXGI()
    {
        if (m_AcquiredDesktopImage)
        {
            m_AcquiredDesktopImage->Release();
            m_AcquiredDesktopImage = nullptr;
        }
        if (m_AcquiredDesktopImage_copy)
        {
            m_AcquiredDesktopImage_copy->Release();
            m_AcquiredDesktopImage_copy = nullptr;
        }

        if (m_DeskDupl)
        {
            m_DeskDupl->Release();
            m_DeskDupl = nullptr;
        }

        if (m_Device)
        {
            m_Device->Release();
            m_Device = nullptr;
        }
    }
    /*
	 * @brief InitDevic
	 * @param 2
	 * @param 3
	 * @return Initialization Result
	 */
    bool InitDevice()
    {
        ChooseAdapter();

        HRESULT hr{ S_OK };
        // Feature levels supported
        D3D_FEATURE_LEVEL FeatureLevels[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_1
        };
        UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);

        D3D_FEATURE_LEVEL FeatureLevel;

        // Create device

        hr = D3D11CreateDevice(
            selectedAdapter,
            D3D_DRIVER_TYPE_UNKNOWN, // D3D_DRIVER_TYPE_UNKNOWN
            nullptr,
            /* D3D11_CREATE_DEVICE_BGRA_SUPPORT
		* This flag adds support for surfaces with a different
		* color channel ordering than the API default.
		* You need it for compatibility with Direct2D. */
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            FeatureLevels,
            NumFeatureLevels,
            D3D11_SDK_VERSION,
            &m_Device,
            &FeatureLevel,
            nullptr);
        if (SUCCEEDED(hr))
        {
            trrlog::Log_debug("InitDevice success");
            return true;
        }
        trrlog::Log_debug("InitDevice error");
        return false;
    }

    bool InitDupl(UINT monitorIdx, int& duplWidth, int& duplHeight)
    {
        m_monitorIdx = monitorIdx;
        HRESULT hr = S_FALSE;
        // Get DXGI device
        IDXGIDevice* DxgiDevice = nullptr;
        hr = m_Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice));
        if (FAILED(hr))
        {
            return false;
        }

        // Get DXGI adapter
        IDXGIAdapter* DxgiAdapter = nullptr;
        hr = DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter));
        DxgiDevice->Release();
        DxgiDevice = nullptr;
        if (FAILED(hr))
        {
            return false;
        }

        // Get output
        IDXGIOutput* DxgiOutput = nullptr;
        hr = DxgiAdapter->EnumOutputs(m_monitorIdx, &DxgiOutput);
        DxgiAdapter->Release();
        DxgiAdapter = nullptr;
        if (FAILED(hr))
        {
            return false;
        }

        // QI for Output 1
        IDXGIOutput1* DxgiOutput1 = nullptr;
        hr = DxgiOutput->QueryInterface(__uuidof(DxgiOutput1), reinterpret_cast<void**>(&DxgiOutput1));

        DxgiOutput->Release();
        DxgiOutput = nullptr;

        if (FAILED(hr))
        {
            return false;
        }

        // Create desktop duplication
        hr = DxgiOutput1->DuplicateOutput(m_Device, &m_DeskDupl);
        DxgiOutput1->Release();
        DxgiOutput1 = nullptr;

        if (FAILED(hr))
        {
            if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
            {
                //WRITELOG("DXGI_ERROR_NOT_CURRENTLY_AVAILABLE");
            }
            //切换屏幕创建duplication失败时触发的错误
            //char log[100];
            //sprintf_s(log, "[0x%08X]: DxgiOutput1->DuplicateOutput failed.", hr);
            //WRITELOG(log);
            //LOGE("[0x%08X]: DxgiOutput1->DuplicateOutput failed.", hr);

            return false;
        }

        DXGI_OUTDUPL_DESC outDuplDesc;
        m_DeskDupl->GetDesc(&outDuplDesc);

        duplWidth = outDuplDesc.ModeDesc.Width;
        duplHeight = outDuplDesc.ModeDesc.Height;

        m_DeskDupl_state = true;

        //this->showMonitorInfos();
        return true;
    }

    /*
	 * @brief Get
	 * @param timeout
	 * @return 0
	 * @return 1
	 * @return 2
	 */
    int getFrame(int timeout = 100)
    {
        if (!m_DeskDupl_state)
        {
            //LOGE("Duplication Abnormal, unable to getFrame");
            //return GETFRAME_DUPLICATION_ERROR;
            return 1;
        }
        // If still holding old frame, destroy it
        if (m_AcquiredDesktopImage)
        {
            m_AcquiredDesktopImage->Release();
            m_AcquiredDesktopImage = nullptr;
        }

        IDXGIResource* DesktopResource = nullptr;
        DXGI_OUTDUPL_FRAME_INFO FrameInfo;

        // Get new frame
        HRESULT hr = m_DeskDupl->AcquireNextFrame(timeout, &FrameInfo, &DesktopResource);

        if (FAILED(hr))
        {
            if (hr == DXGI_ERROR_WAIT_TIMEOUT)
            {
                //屏幕无变化可能引起该错误
                //LOGE("[0x%08X]: AcquireNextFrame failed.(May be caused by timeout)", hr);

                if (DesktopResource)
                {
                    DesktopResource->Release();
                    DesktopResource = nullptr;
                }

                //return GETFRAME_ERROR;
                return 2;
            }
            else
            {
                //切换屏幕，锁屏可能引起该错误，一旦进入该错误，需要重启Duplication才能重新使用
                //char log[100];
                //sprintf_s(log, "[0x%08X]: AcquireNextFrame failed.(May be caused by screen switching)", hr);
                //WRITELOG(log);

                if (DesktopResource)
                {
                    DesktopResource->Release();
                    DesktopResource = nullptr;
                }
                m_DeskDupl_state = false;
                //return GETFRAME_DUPLICATION_ERROR;
                return 1;
            }
        }

        // QI for IDXGIResource
        hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_AcquiredDesktopImage));

        DesktopResource->Release();
        DesktopResource = nullptr;

        if (FAILED(hr))
        {
            return 2;
        }
        //return GETFRAME_SUCCESS;
        return 0;
    }

    bool copyFrameToBuffer(BYTE** buffer, long bufferSize)
    {
        HRESULT hr;
        ID3D11DeviceContext* context;
        m_Device->GetImmediateContext(&context);

        if (!m_AcquiredDesktopImage_copy)
        {
            D3D11_TEXTURE2D_DESC desc;
            m_AcquiredDesktopImage->GetDesc(&desc);
            // Create CPU access texture m_AcquiredDesktopImage_copy
            D3D11_TEXTURE2D_DESC copyImageDesc{};
            copyImageDesc.Width = desc.Width;
            copyImageDesc.Height = desc.Height;
            copyImageDesc.Format = desc.Format;
            copyImageDesc.ArraySize = 1;
            copyImageDesc.BindFlags = 0;
            copyImageDesc.MiscFlags = 0;
            copyImageDesc.SampleDesc.Count = 1;
            copyImageDesc.SampleDesc.Quality = 0;
            copyImageDesc.MipLevels = 1;
            copyImageDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
            copyImageDesc.Usage = D3D11_USAGE_STAGING;
            hr = m_Device->CreateTexture2D(&copyImageDesc, NULL, &m_AcquiredDesktopImage_copy);

            if (FAILED(hr))
            {
                //LOGE("[0x%08X]: CreateTexture2D failed.", hr);
                return false;
            }
        }

        context->CopyResource(m_AcquiredDesktopImage_copy, m_AcquiredDesktopImage); //源Texture2D和目的Texture2D需要有相同的多重采样计数和质量时

        D3D11_MAPPED_SUBRESOURCE mapRes;
        UINT subresource = D3D11CalcSubresource(0, 0, 0);

        hr = context->Map(m_AcquiredDesktopImage_copy, subresource, D3D11_MAP_READ, 0, &mapRes);
        if (FAILED(hr))
        {
            //LOGE("[0x%08X]: context->Map failed.", hr);
            return false;
        }
        BYTE* dptr = *buffer;
        memcpy_s(dptr, bufferSize, mapRes.pData, bufferSize);
        context->Unmap(m_AcquiredDesktopImage_copy, subresource);
        return true;
    }

    bool doneWithFrame()
    {
        HRESULT hr = m_DeskDupl->ReleaseFrame();
        if (FAILED(hr))
        {
            return false;
        }

        if (m_AcquiredDesktopImage)
        {
            m_AcquiredDesktopImage->Release();
            m_AcquiredDesktopImage = nullptr;
        }

        return true;
    }

private:
    void ChooseAdapter()
    {
        IDXGIFactory1* dxgiFactory{ nullptr };
        HRESULT hr{ CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&dxgiFactory) };

        if (FAILED(hr))
        {
            return;
        }
        for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &selectedAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            DXGI_ADAPTER_DESC1 desc;
            hr = selectedAdapter->GetDesc1(&desc);
            char narrowString[100];
            WideCharToMultiByte(CP_UTF8, 0, desc.Description, -1, narrowString, sizeof(narrowString), NULL, NULL);
            trrlog::Log_debug("{}", narrowString);
            if (desc.Flags != DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                trrlog::Log_debug("used display adapter{}", narrowString);
                break;
            }
        }
        dxgiFactory->Release();
    }

private:
    ID3D11Device* m_Device;
    IDXGIOutputDuplication* m_DeskDupl;
    bool m_DeskDupl_state = false;

    UINT m_monitorIdx;
    ID3D11Texture2D* m_AcquiredDesktopImage;
    ID3D11Texture2D* m_AcquiredDesktopImage_copy;

    IDXGIAdapter1* selectedAdapter;
};