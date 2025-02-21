#include "targets.h"

#ifdef TARGET_TX

#include "CRSF.h"
#include "CRSFHandset.h"
#include "NullHandset.h"
#include "POWERMGNT.h"
#include "devHandset.h"

#if defined(PLATFORM_ESP32)
#include "AutoDetect.h"
#endif

Handset *handset;

static bool initialize()
{
#if defined(PLATFORM_ESP32)
    if (GPIO_PIN_RCSIGNAL_RX == GPIO_PIN_RCSIGNAL_TX)
    {
        handset = new AutoDetect();
        return true;
    }
#elif defined(PLATFORM_ESP8266)
    // The 8266 only has one UART, so we can't use it for handset and airport
    if (!firmwareOptions.is_airport) {
        handset = new CRSFHandset();
        return true;
    }
    handset = new NullHandset();
    return true;
#else
    handset = new CRSFHandset();
    return true;
#endif
}

static int start()
{
    handset->Begin();
#if defined(DEBUG_TX_FREERUN)
    handset->forceConnection();
#endif
    return DURATION_IMMEDIATELY;
}

static int timeout()
{
    handset->handleInput();
    return DURATION_IMMEDIATELY;
}

static int event()
{
    // An event should be generated every time the TX power changes
    CRSF::LinkStatistics.uplink_TX_Power = powerToCrsfPower(PowerLevelContainer::currPower());
    return DURATION_IGNORE;
}

device_t Handset_device = {
    .initialize = initialize,
    .start = start,
    .event = event,
    .timeout = timeout,
    .subscribe = EVENT_POWER_CHANGED
};
#endif
