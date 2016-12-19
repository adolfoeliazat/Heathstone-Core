/**
   \file initG3D.cpp
 */
#include <stdlib.h>
#include "G3D/platform.h"
#include "G3D/System.h"

namespace G3D {

namespace _internal {

    G3DSpecification& g3dInitializationSpecification() {
        static G3DSpecification s;
        return s;
    }

#if 0 /*disable G3D networking*/
    void initializeNetwork();
    void cleanupNetwork();
#endif
}

static void G3DCleanupHook() {
#if 0 /*disable G3D networking*/
    _internal::cleanupNetwork();
#endif
    System::cleanup();
}

void initG3D(const G3DSpecification& spec) {
    static bool initialized = false;
    
    if (! initialized) {
        initialized = true;
        _internal::g3dInitializationSpecification() = spec;
        atexit(&G3DCleanupHook);
        
        // Trigger a call to System::init
        System::cpuSpeedMHz();

#if 0 /*disable G3D networking*/
        _internal::initializeNetwork();
#endif
    }
}

} // namespace
