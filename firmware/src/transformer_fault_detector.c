/* ============================================================
 * Transformer Fault Detection — Main Inference Loop
 * Target:    EFR32xG24 (production) / STM32F4 (prototype)
 * Author:    Shubh Verma <shubh.verma_ec23@gla.ac.in>
 * License:   Apache 2.0
 *
 * NOTE: Edge Impulse SDK (ei_run_classifier) is auto-generated
 * after model training on Edge Impulse Studio / Silicon Labs MLTK.
 * Drop the exported library into firmware/edge-impulse-sdk/
 * and this file will compile against it.
 * ============================================================ */

#include "transformer_fault_detector.h"

/* Edge Impulse auto-generated header (added after model export) */
/* #include "edge-impulse-sdk/classifier/ei_run_classifier.h"   */

#include <stdio.h>
#include <string.h>

/* ---- Internal audio ring buffer --------------------------- */
static int16_t  audio_buffer[FRAME_SIZE_SAMPLES];
static volatile bool frame_ready = false;

/* ---- I2S DMA callback ------------------------------------- */
/* Called by HAL/DMADRV when one full frame is captured        */
void i2s_dma_complete_callback(void) {
    frame_ready = true;
}

/* ---- Init ------------------------------------------------- */
bool tfd_init(void) {
    /* 1. Configure I2S peripheral
     *    EFR32xG24: USART0 in I2S mode, PC3=WS, PC4=SCK, PC5=SD
     *    STM32F4  : SPI2 in I2S mode,  PB12=WS, PB13=SCK, PB15=SD
     *    Baud: 16000 Hz * 32 bits * 2 channels = 1.024 MHz SCK  */

    /* 2. Start DMA circular transfer into audio_buffer         */

    /* 3. Initialise Edge Impulse classifier                    */
    /* ei_sleep = tfd_sleep_until_next_frame; (hook EI sleep)   */

    printf("[TFD] Init complete. Sample rate: %d Hz\n", SAMPLE_RATE_HZ);
    return true;
}

/* ---- Main inference loop ---------------------------------- */
bool tfd_run_inference(inference_result_t *result) {
    /* Wait for DMA to fill one audio frame */
    while (!frame_ready) {
        tfd_sleep_until_next_frame();
    }
    frame_ready = false;

    /* ---- Step 1: Pre-emphasis filter ---------------------- */
    /* y[n] = x[n] - 0.97 * x[n-1]  boosts high frequencies  */
    int16_t filtered[FRAME_SIZE_SAMPLES];
    filtered[0] = audio_buffer[0];
    for (int i = 1; i < FRAME_SIZE_SAMPLES; i++) {
        filtered[i] = (int16_t)(audio_buffer[i] - 0.97f * audio_buffer[i-1]);
    }

    /* ---- Step 2: Edge Impulse inference ------------------- */
    /* Uncomment after dropping in Edge Impulse exported SDK:

    signal_t signal;
    numpy::signal_from_buffer(
        (float*)filtered,
        FRAME_SIZE_SAMPLES,
        &signal
    );

    ei_impulse_result_t ei_result = {0};
    EI_IMPULSE_ERROR err = run_classifier(&signal, &ei_result, false);
    if (err != EI_IMPULSE_OK) return false;

    // Find highest confidence class
    float max_conf = 0.0f;
    int   max_idx  = 0;
    for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (ei_result.classification[i].value > max_conf) {
            max_conf = ei_result.classification[i].value;
            max_idx  = i;
        }
    }
    result->label          = (fault_class_t)max_idx;
    result->confidence     = max_conf;
    result->alert_required = (result->label != FAULT_CLASS_HEALTHY)
                             && (max_conf > 0.85f);

    */

    /* ---- Placeholder result (remove after EI integration) - */
    result->label          = FAULT_CLASS_HEALTHY;
    result->confidence     = 0.99f;
    result->alert_required = false;

    printf("[TFD] Class: %-20s Confidence: %.1f%%\n",
           tfd_label_str(result->label),
           result->confidence * 100.0f);

    return true;
}

/* ---- BLE alert -------------------------------------------- */
void tfd_ble_advertise_fault(const inference_result_t *result) {
    /* Build custom BLE advertisement payload:
     * Byte 0:   Fault class (0-3)
     * Byte 1:   Confidence * 100 (integer %)
     * Bytes 2-3: Node ID (16-bit, configurable per device)    */
    uint8_t adv_data[4];
    adv_data[0] = (uint8_t)result->label;
    adv_data[1] = (uint8_t)(result->confidence * 100);
    adv_data[2] = 0x00; /* Node ID high byte                   */
    adv_data[3] = 0x01; /* Node ID low byte                    */

    /* sl_bt_advertiser_set_data(adv_handle, 0, 4, adv_data);  */
    /* sl_bt_advertiser_start(adv_handle, ...);                 */

    printf("[BLE] Fault alert advertised: %s (%.0f%%)\n",
           tfd_label_str(result->label),
           result->confidence * 100.0f);
}

/* ---- Power management ------------------------------------- */
void tfd_sleep_until_next_frame(void) {
    /* EFR32xG24: EMU_EnterEM1() — I2S DMA runs in EM1         */
    /* STM32F4:   HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON,
                                          PWR_SLEEPENTRY_WFI)   */
    __WFI(); /* Wait For Interrupt — wakes on DMA complete      */
}

/* ---- Utility ---------------------------------------------- */
const char *tfd_label_str(fault_class_t label) {
    switch (label) {
        case FAULT_CLASS_HEALTHY:           return "HEALTHY";
        case FAULT_CLASS_CORONA_DISCHARGE:  return "CORONA_DISCHARGE";
        case FAULT_CLASS_WINDING_FAULT:     return "WINDING_FAULT";
        case FAULT_CLASS_PARTIAL_DISCHARGE: return "PARTIAL_DISCHARGE";
        default:                            return "UNKNOWN";
    }
}
