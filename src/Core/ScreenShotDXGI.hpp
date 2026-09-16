/**
 * @file ScreenShotDXGI.hpp
 * @brief DXGI Desktop Duplication 屏幕采集：设备初始化、取帧与 CPU 可读拷贝。
 *
 * 注意：staging 纹理 CPUAccessFlags 只能是 READ 或 WRITE 之一；
 * 拷贝时必须按 RowPitch 逐行 memcpy，不能整块线性拷贝。
 */

#pragma once

#include <iostream>

#include <d3d11.h>
#include <dxgi1_2.h>

#pragma comment(lib, "d3d11.lib")

void qrLog(const std::string& msg);

class ScreenShotDXGI
{
public:
    ScreenShotDXGI() :
        m_Device(nullptr),
        m_Context(nullptr),
        m_DeskDupl(nullptr),
        m_AcquiredDesktopImage(nullptr),
        m_AcquiredDesktopImage_copy(nullptr),
        m_monitorIdx(0),
        m_stagingWidth(0),
        m_stagingHeight(0),
        selectedAdapter(nullptr)
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

        if (m_Context)
        {
            m_Context->Release();
            m_Context = nullptr;
        }

        if (m_Device)
        {
            m_Device->Release();
            m_Device = nullptr;
        }

        if (selectedAdapter)
        {
            selectedAdapter->Release();
            selectedAdapter = nullptr;
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
            // 缓存 ImmediateContext，避免每帧 GetImmediateContext 增减引用计数
            m_Device->GetImmediateContext(&m_Context);
            return true;
        }
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
        if (!m_AcquiredDesktopImage || !buffer || !*buffer)
        {
            return false;
        }

        HRESULT hr;
        ID3D11DeviceContext* context = m_Context;
        if (!context)
        {
            return false;
        }

        if (!m_AcquiredDesktopImage_copy)
        {
            D3D11_TEXTURE2D_DESC desc;
            m_AcquiredDesktopImage->GetDesc(&desc);
            qrLog("staging desc: " + std::to_string(desc.Width) + "x" + std::to_string(desc.Height) + " fmt=" + std::to_string((int)desc.Format));
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
            // Staging 纹理的 CPUAccessFlags 只能是 READ 或 WRITE 之一。
            // 同时置 READ|WRITE 会让 CreateTexture2D 直接失败，调用方若忽略返回值，
            // 大块分配的零页缓冲区会被当成「画面」写出全透明 PNG。
            copyImageDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            copyImageDesc.Usage = D3D11_USAGE_STAGING;
            hr = m_Device->CreateTexture2D(&copyImageDesc, NULL, &m_AcquiredDesktopImage_copy);

            if (FAILED(hr) || !m_AcquiredDesktopImage_copy)
            {
                qrLog("CreateTexture2D failed hr=" + std::to_string((long)hr));
                return false;
            }
            m_stagingWidth = desc.Width;
            m_stagingHeight = desc.Height;
        }

        context->CopyResource(m_AcquiredDesktopImage_copy, m_AcquiredDesktopImage); //源Texture2D和目的Texture2D需要有相同的多重采样计数和质量时

        D3D11_MAPPED_SUBRESOURCE mapRes{};
        UINT subresource = D3D11CalcSubresource(0, 0, 0);

        hr = context->Map(m_AcquiredDesktopImage_copy, subresource, D3D11_MAP_READ, 0, &mapRes);
        if (FAILED(hr))
        {
            qrLog("Map failed hr=" + std::to_string((long)hr));
            return false;
        }
        {
            std::string first;
            for (int i = 0; i < 8; i++)
            {
                first += std::to_string((int)((const BYTE*)mapRes.pData)[i]) + " ";
            }
            qrLog("map rowPitch=" + std::to_string((long)mapRes.RowPitch) + " first8=" + first);
        }

        // RowPitch 通常大于 width*4（行对齐填充），必须按行拷贝。
        BYTE* dptr = *buffer;
        const UINT rowBytes = m_stagingWidth * 4;
        const size_t required = static_cast<size_t>(rowBytes) * m_stagingHeight;
        if (mapRes.RowPitch < rowBytes || static_cast<size_t>(bufferSize) < required)
        {
            qrLog("copy size mismatch rowPitch=" + std::to_string((long)mapRes.RowPitch) +
                  " rowBytes=" + std::to_string(rowBytes) +
                  " bufferSize=" + std::to_string(bufferSize));
            context->Unmap(m_AcquiredDesktopImage_copy, subresource);
            return false;
        }
        const BYTE* sptr = static_cast<const BYTE*>(mapRes.pData);
        for (UINT y = 0; y < m_stagingHeight; ++y)
        {
            memcpy_s(dptr + static_cast<size_t>(y) * rowBytes, rowBytes,
                     sptr + static_cast<size_t>(y) * mapRes.RowPitch, rowBytes);
        }
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
        if (selectedAdapter)
        {
            selectedAdapter->Release();
            selectedAdapter = nullptr;
        }

        IDXGIFactory1* dxgiFactory{ nullptr };
        HRESULT hr{ CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&dxgiFactory) };

        if (FAILED(hr))
        {
            return;
        }

        // 优先选「真正挂着显示器输出」的硬件适配器；多显卡/核显+独显机器上
        // 第一个非软件适配器可能并不驱动当前屏幕，复制出来会是空帧。
        IDXGIAdapter1* fallback = nullptr;
        for (UINT i = 0;; ++i)
        {
            IDXGIAdapter1* adapter = nullptr;
            if (dxgiFactory->EnumAdapters1(i, &adapter) == DXGI_ERROR_NOT_FOUND)
            {
                break;
            }

            DXGI_ADAPTER_DESC1 desc{};
            adapter->GetDesc1(&desc);
            if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0)
            {
                adapter->Release();
                continue;
            }

            IDXGIOutput* output = nullptr;
            const bool hasOutput = SUCCEEDED(adapter->EnumOutputs(0, &output));
            if (output)
            {
                output->Release();
            }
            if (hasOutput)
            {
                selectedAdapter = adapter;
                break;
            }
            if (!fallback)
            {
                fallback = adapter;
            }
            else
            {
                adapter->Release();
            }
        }

        if (!selectedAdapter)
        {
            selectedAdapter = fallback;
            fallback = nullptr;
        }
        if (fallback)
        {
            fallback->Release();
        }
        dxgiFactory->Release();
    }

private:
    ID3D11Device* m_Device;
    ID3D11DeviceContext* m_Context;
    IDXGIOutputDuplication* m_DeskDupl;
    bool m_DeskDupl_state = false;

    UINT m_monitorIdx;
    ID3D11Texture2D* m_AcquiredDesktopImage;
    ID3D11Texture2D* m_AcquiredDesktopImage_copy;
    UINT m_stagingWidth;
    UINT m_stagingHeight;

    IDXGIAdapter1* selectedAdapter;
};