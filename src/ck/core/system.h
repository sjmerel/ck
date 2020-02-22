#pragma once

#include "ck/core/platform.h"
#include "ck/core/funcallocator.h"
#include "ck/config.h"

namespace Cki
{


class System 
{
public:
    static bool init(CkConfig&, bool toolMode = false);
    static System* get();
    static void shutdown();

    CkConfig& getConfig();
    bool isToolMode() const { return m_toolMode; }
    CkSampleType getSampleType() { return m_sampleType; }
    bool isSimdSupported() const { return m_simd; }

    virtual void update();
    virtual void suspend();
    virtual void resume();

protected:
    CkConfig m_config;
    FuncAllocator m_alloc;
    bool m_suspended;
    bool m_simd;

    System(const CkConfig&);
    virtual ~System();

private:
    void printSysInfo();
    bool m_toolMode;
    CkSampleType m_sampleType;
};


} 

