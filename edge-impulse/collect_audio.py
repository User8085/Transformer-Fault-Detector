"""
Transformer Fault Detection — Training Data Collection Script
Author:  Shubh Verma <shubh.verma_ec23@gla.ac.in>
License: Apache 2.0

Usage:
    python collect_audio.py --label healthy --duration 60
    python collect_audio.py --label corona_discharge --duration 30
    python collect_audio.py --label winding_fault --duration 30
    python collect_audio.py --label partial_discharge --duration 30

Requires:
    pip install pyaudio numpy soundfile edge-impulse-cli
"""

import argparse
import os
import time
import numpy as np
import soundfile as sf
import pyaudio

SAMPLE_RATE   = 16000   # Hz  — matches firmware configuration
FRAME_SECONDS = 1       # Each saved clip = 1 second
CHANNELS      = 1       # Mono
FORMAT        = pyaudio.paInt16

LABELS = ["healthy", "corona_discharge", "winding_fault", "partial_discharge"]


def record_samples(label: str, total_seconds: int, output_dir: str):
    if label not in LABELS:
        raise ValueError(f"Label must be one of: {LABELS}")

    os.makedirs(f"{output_dir}/{label}", exist_ok=True)

    pa     = pyaudio.PyAudio()
    stream = pa.open(
        format=FORMAT,
        channels=CHANNELS,
        rate=SAMPLE_RATE,
        input=True,
        frames_per_buffer=SAMPLE_RATE  # 1 second buffer
    )

    num_clips = total_seconds // FRAME_SECONDS
    print(f"\nRecording {num_clips} clips of class '{label}'...")
    print("Press Ctrl+C to stop early.\n")

    try:
        for i in range(num_clips):
            print(f"  Clip {i+1}/{num_clips}...", end=" ", flush=True)
            raw   = stream.read(SAMPLE_RATE, exception_on_overflow=False)
            audio = np.frombuffer(raw, dtype=np.int16).astype(np.float32)
            audio = audio / 32768.0  # normalise to -1.0 .. 1.0

            filename = f"{output_dir}/{label}/{label}_{int(time.time()*1000)}.wav"
            sf.write(filename, audio, SAMPLE_RATE)
            print(f"saved → {filename}")
            time.sleep(0.05)

    except KeyboardInterrupt:
        print("\nRecording stopped by user.")
    finally:
        stream.stop_stream()
        stream.close()
        pa.terminate()

    print(f"\nDone. {i+1} clips saved to {output_dir}/{label}/")
    print("Next step: upload the folder to Edge Impulse Studio → Data Acquisition")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Collect transformer audio samples")
    parser.add_argument("--label",    required=True, choices=LABELS)
    parser.add_argument("--duration", type=int, default=60, help="Total seconds to record")
    parser.add_argument("--output",   default="./dataset", help="Output directory")
    args = parser.parse_args()

    record_samples(args.label, args.duration, args.output)
