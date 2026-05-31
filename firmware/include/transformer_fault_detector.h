#ifndef TRANSFORMER_FAULT_DETECTOR_H
#define TRANSFORMER_FAULT_DETECTOR_H

/* ============================================================
 * Transformer Fault Detection — Inference Pipeline
 * Target:    EFR32xG24 (production) / STM32F4 (prototype)
 * Author:    Shubh Verma <shubh.verma_ec23@gla.ac.in>
 * License:   Apache 2.0
 * ============================================================ */

#include <stdint.h>
#include <stdbool.h>

/* ---- Audio configuration ---------------------------------- */
#define SAMPLE_RATE_HZ          16000
#define FRAME_SIZE_SAMPLES      512       /* 32ms frame         */
#define MFCC_NUM_COEFFICIENTS   13
#define MFCC_NUM_FILTERS        26
#define MFCC_FFT_SIZE           512
#define INFERENCE_WINDOW_MS     1000      /* 1 second window    */

/* ---- Classification labels -------------------------------- */
typedef enum {
    FAULT_CLASS_HEALTHY           = 0,
    FAULT_CLASS_CORONA_DISCHARGE  = 1,
    FAULT_CLASS_WINDING_FAULT     = 2,
    FAULT_CLASS_PARTIAL_DISCHARGE = 3,
    FAULT_CLASS_UNKNOWN           = 4
} fault_class_t;

/* ---- Inference result ------------------------------------- */
typedef struct {
    fault_class_t   label;
    float           confidence;     /* 0.0 – 1.0              */
    bool            alert_required; /* true if not HEALTHY    */
} inference_result_t;

/* ---- Public API ------------------------------------------- */

/**
 * @brief  Initialise audio capture (I2S DMA) and inference engine.
 * @return true on success
 */
bool tfd_init(void);

/**
 * @brief  Run one inference cycle on the latest audio frame.
 *         Call this from your main loop or a FreeRTOS task.
 * @param  result  Output: classification label + confidence
 * @return true if inference completed successfully
 */
bool tfd_run_inference(inference_result_t *result);

/**
 * @brief  Transmit a BLE fault alert advertisement.
 *         Only called when result.alert_required == true.
 * @param  result  The inference result to advertise
 */
void tfd_ble_advertise_fault(const inference_result_t *result);

/**
 * @brief  Enter low-power sleep between inference cycles.
 *         Wakes on DMA buffer-full interrupt.
 */
void tfd_sleep_until_next_frame(void);

/**
 * @brief  Return human-readable label string.
 */
const char *tfd_label_str(fault_class_t label);

#endif /* TRANSFORMER_FAULT_DETECTOR_H */
