#pragma once

class DeviceManager
{
public:
    static DeviceManager& GetInstance();

    DeviceManager(const DeviceManager&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;
    DeviceManager(DeviceManager&&) = delete;
    DeviceManager& operator=(DeviceManager&&) = delete;

private:
    DeviceManager() = default;
    ~DeviceManager() = default;

public:
    void Initialize();
    ID3D12Device* GetDevice() const;
    IDXGIFactory6* GetFactory() const;
    IDXGIAdapter4* GetAdapter() const;

private:
    void CreateFactory();
    void CreateAdapter();
    void CreateDevice();

private:
    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<IDXGIFactory6> factory_;
    Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter_;
};

