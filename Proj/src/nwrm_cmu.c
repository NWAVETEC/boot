#include "nwrm_module.h"
#include "em_cmu.h"
#include "nwrm_cmu.h"


void NWRM_CMU_Init(bool external)
{
    if (external) {

        // Start LFXO, and use LFXO for low-energy modules
        CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
        CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
    } else {
        CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
    }

	// Enable the clock to the interface of the low energy modules
	CMU_ClockEnable(cmuClock_CORELE, true);
}

