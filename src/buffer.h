#ifndef BUFFER_H
#define BUFFER_H

#include <atomic>
#include <bitset>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

#include <dsound.h>
#include <mmreg.h>

#include "AL/al.h"
#include "comptr.h"
#include "dsoal.h"
#include "dsoundoal.h"
#include "expected.h"
#include "vmanager.h"


class DSound8OAL;


class SharedBuffer {
    std::atomic<ULONG> mRef{1u};

    auto dispose() noexcept -> void;

public:
    SharedBuffer() = default;
    ~SharedBuffer();

    auto AddRef() noexcept -> ULONG { return mRef.fetch_add(1u, std::memory_order_relaxed)+1; }
    auto Release() noexcept -> ULONG
    {
        const auto ret = mRef.fetch_sub(1u, std::memory_order_relaxed)-1;
        if(ret == 0) dispose();
        return ret;
    }

    char *mData;
    DWORD mDataSize{0};
    DWORD mFlags{};

    WAVEFORMATEXTENSIBLE mWfxFormat{};
    ALenum mAlFormat{AL_NONE};
    ALuint mAlBuffer{0};
    VmMode mVoiceMode{DSPROPERTY_VMANAGER_MODE_DEFAULT};

    static auto Create(const DSBUFFERDESC &bufferDesc, const std::bitset<ExtensionCount> exts) noexcept
        -> ds::expected<ComPtr<SharedBuffer>,HRESULT>;
};

class Buffer final : IDirectSoundBuffer8 {
    class UnknownImpl final : IUnknown {
        Buffer *impl_from_base() noexcept
        {
#ifdef __GNUC__
    _Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wcast-align\"")
#endif
            return CONTAINING_RECORD(this, Buffer, mUnknownIface);
#ifdef __GNUC__
    _Pragma("GCC diagnostic pop")
#endif
        }

    public:
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) noexcept override;
        ULONG STDMETHODCALLTYPE AddRef() noexcept override;
        ULONG STDMETHODCALLTYPE Release() noexcept override;

        template<typename T>
        T as() noexcept { return static_cast<T>(this); }
    };
    UnknownImpl mUnknownIface;

    class Buffer3D final : IDirectSound3DBuffer {
        auto impl_from_base() noexcept
        {
#ifdef __GNUC__
    _Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wcast-align\"")
#endif
            return CONTAINING_RECORD(this, Buffer, mBuffer3D);
#ifdef __GNUC__
    _Pragma("GCC diagnostic pop")
#endif
        }

    public:
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) noexcept override;
        ULONG STDMETHODCALLTYPE AddRef() noexcept override;
        ULONG STDMETHODCALLTYPE Release() noexcept override;
        HRESULT STDMETHODCALLTYPE GetAllParameters(DS3DBUFFER *ds3dBuffer) noexcept override;
        HRESULT STDMETHODCALLTYPE GetConeAngles(DWORD *insideConeAngle, DWORD *outsideConeAngle) noexcept override;
        HRESULT STDMETHODCALLTYPE GetConeOrientation(D3DVECTOR *orientation) noexcept override;
        HRESULT STDMETHODCALLTYPE GetConeOutsideVolume(LONG *coneOutsideVolume) noexcept override;
        HRESULT STDMETHODCALLTYPE GetMaxDistance(D3DVALUE *maxDistance) noexcept override;
        HRESULT STDMETHODCALLTYPE GetMinDistance(D3DVALUE *minDistance) noexcept override;
        HRESULT STDMETHODCALLTYPE GetMode(DWORD *mode) noexcept override;
        HRESULT STDMETHODCALLTYPE GetPosition(D3DVECTOR *position) noexcept override;
        HRESULT STDMETHODCALLTYPE GetVelocity(D3DVECTOR *velocity) noexcept override;
        HRESULT STDMETHODCALLTYPE SetAllParameters(const DS3DBUFFER *ds3dBuffer, DWORD apply) noexcept override;
        HRESULT STDMETHODCALLTYPE SetConeAngles(DWORD insideConeAngle, DWORD outsideConeAngle, DWORD apply) noexcept override;
        HRESULT STDMETHODCALLTYPE SetConeOrientation(D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD apply) noexcept override;
        HRESULT STDMETHODCALLTYPE SetConeOutsideVolume(LONG coneOutsideVolume, DWORD apply) noexcept override;
        HRESULT STDMETHODCALLTYPE SetMaxDistance(D3DVALUE maxDistance, DWORD apply) noexcept override;
        HRESULT STDMETHODCALLTYPE SetMinDistance(D3DVALUE minDistance, DWORD apply) noexcept override;
        HRESULT STDMETHODCALLTYPE SetMode(DWORD mode, DWORD apply) noexcept override;
        HRESULT STDMETHODCALLTYPE SetPosition(D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD apply) noexcept override;
        HRESULT STDMETHODCALLTYPE SetVelocity(D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD apply) noexcept override;

        template<typename T>
        T as() noexcept { return static_cast<T>(this); }
    };
    Buffer3D mBuffer3D;

    class Prop final : IKsPropertySet {
        auto impl_from_base() noexcept
        {
#ifdef __GNUC__
    _Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wcast-align\"")
#endif
            return CONTAINING_RECORD(this, Buffer, mProp);
#ifdef __GNUC__
    _Pragma("GCC diagnostic pop")
#endif
        }

    public:
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) noexcept override;
        ULONG STDMETHODCALLTYPE AddRef() noexcept override;
        ULONG STDMETHODCALLTYPE Release() noexcept override;
        HRESULT STDMETHODCALLTYPE Get(REFGUID guidPropSet, ULONG dwPropID, void *pInstanceData, ULONG cbInstanceData, void *pPropData, ULONG cbPropData, ULONG *pcbReturned) noexcept override;
        HRESULT STDMETHODCALLTYPE Set(REFGUID guidPropSet, ULONG dwPropID, void *pInstanceData, ULONG cbInstanceData, void *pPropData, ULONG cbPropData) noexcept override;
        HRESULT STDMETHODCALLTYPE QuerySupport(REFGUID guidPropSet, ULONG dwPropID, ULONG *pTypeSupport) noexcept override;

        template<typename T>
        T as() noexcept { return static_cast<T>(this); }
    };
    Prop mProp;

    std::atomic<ULONG> mTotalRef{1u}, mDsRef{1u}, mDs3dRef{0u}, mPropRef{0u}, mUnkRef{0u};

    DSound8OAL &mParent;
    ALCcontext *mContext{};

    std::mutex &mMutex;
    ComPtr<SharedBuffer> mBuffer;
    ALuint mSource{};
    DWORD mLastPos{0};
    DWORD mVmPriority{0};
    std::atomic<bool> mLocked{false};
    bool mBufferLost{false};

    LONG mVolume{};
    LONG mPan{};
    DWORD mFrequency{};

    DS3DBUFFER mImmediate{};
    DS3DBUFFER mDeferred{};
    enum DirtyFlags {
        Position,
        Velocity,
        ConeAngles,
        ConeOrientation,
        ConeVolume,
        MinDistance,
        MaxDistance,
        Mode,

        FlagCount
    };
    std::bitset<FlagCount> mDirty;

    void setParams(const DS3DBUFFER &params, const std::bitset<FlagCount> flags);

    enum class LocStatus : uint8_t {
        None, Any=None,
        Hardware = DSBSTATUS_LOCHARDWARE,
        Software = DSBSTATUS_LOCSOFTWARE,
    };
    LocStatus mLocStatus{};

    bool mIs8{};
    bool mIsInitialized{false};

    HRESULT setLocation(LocStatus locStatus) noexcept;

public:
    Buffer(DSound8OAL &parent, bool is8, IDirectSoundBuffer *original) noexcept;
    ~Buffer();

    /*** IUnknown methods ***/
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) noexcept override;
    ULONG STDMETHODCALLTYPE AddRef() noexcept override;
    ULONG STDMETHODCALLTYPE Release() noexcept override;
    /*** IDirectSoundBuffer8 methods ***/
    HRESULT STDMETHODCALLTYPE GetCaps(DSBCAPS *bufferCaps) noexcept override;
    HRESULT STDMETHODCALLTYPE GetCurrentPosition(DWORD *playCursor, DWORD *writeCursor) noexcept override;
    HRESULT STDMETHODCALLTYPE GetFormat(WAVEFORMATEX *wfx, DWORD sizeAllocated, DWORD *sizeWritten) noexcept override;
    HRESULT STDMETHODCALLTYPE GetVolume(LONG *volume) noexcept override;
    HRESULT STDMETHODCALLTYPE GetPan(LONG *pan) noexcept override;
    HRESULT STDMETHODCALLTYPE GetFrequency(DWORD *frequency) noexcept override;
    HRESULT STDMETHODCALLTYPE GetStatus(DWORD *status) noexcept override;
    HRESULT STDMETHODCALLTYPE Initialize(IDirectSound *directSound, const DSBUFFERDESC *dsBufferDesc) noexcept override;
    HRESULT STDMETHODCALLTYPE Lock(DWORD offset, DWORD bytes, void **audioPtr1, DWORD *audioBytes1, void **audioPtr2, DWORD *audioBytes2, DWORD flags) noexcept override;
    HRESULT STDMETHODCALLTYPE Play(DWORD reserved1, DWORD reserved2, DWORD flags) noexcept override;
    HRESULT STDMETHODCALLTYPE SetCurrentPosition(DWORD newPosition) noexcept override;
    HRESULT STDMETHODCALLTYPE SetFormat(const WAVEFORMATEX *wfx) noexcept override;
    HRESULT STDMETHODCALLTYPE SetVolume(LONG volume) noexcept override;
    HRESULT STDMETHODCALLTYPE SetPan(LONG pan) noexcept override;
    HRESULT STDMETHODCALLTYPE SetFrequency(DWORD frequency) noexcept override;
    HRESULT STDMETHODCALLTYPE Stop() noexcept override;
    HRESULT STDMETHODCALLTYPE Unlock(void *audioPtr1, DWORD audioBytes1, void *audioPtr2, DWORD audioBytes2) noexcept override;
    HRESULT STDMETHODCALLTYPE Restore() noexcept override;
    HRESULT STDMETHODCALLTYPE SetFX(DWORD dwEffectsCount, DSEFFECTDESC *dsFXDesc, DWORD *resultCodes) noexcept override;
    HRESULT STDMETHODCALLTYPE AcquireResources(DWORD flags, DWORD effectsCount, DWORD *resultCodes) noexcept override;
    HRESULT STDMETHODCALLTYPE GetObjectInPath(REFGUID objectId, DWORD index, REFGUID interfaceId, void **ppObject) noexcept override;

    void commit()
    {
        if(auto flags{std::exchange(mDirty, 0ull)}; flags.any())
            setParams(mDeferred, flags);
    }

    [[nodiscard]]
    ALuint getSource() const noexcept { return mSource; }

    [[nodiscard]]
    DWORD getCurrentMode() const noexcept { return mImmediate.dwMode; }

    template<typename T>
    T as() noexcept
    {
        /* MinGW headers do not have IDirectSoundBuffer8 inherit from
         * IDirectSoundBuffer, which MSVC apparently does. IDirectSoundBuffer
         * is a strict subset of IDirectSoundBuffer8, so the interface is ABI
         * compatible.
         */
        if constexpr(std::is_same_v<T,IDirectSoundBuffer*>
            && !std::is_base_of_v<IDirectSoundBuffer,Buffer>)
            return ds::bit_cast<T>(static_cast<IDirectSoundBuffer8*>(this));
        else
            return static_cast<T>(this);
    }
};

#endif // BUFFER_H
