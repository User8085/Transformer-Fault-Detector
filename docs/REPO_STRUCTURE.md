# Repository Structure

```
transformer-fault-detector/
│
├── README.md                          ← Main project documentation (Silicon Labs template)
├── LICENSE                            ← Apache 2.0
│
├── firmware/
│   ├── include/
│   │   └── transformer_fault_detector.h   ← Public API header
│   └── src/
│       └── transformer_fault_detector.c   ← Inference loop + BLE alert
│
├── edge-impulse/
│   ├── collect_audio.py               ← Training data collection script
│   └── (ei-model-export/)             ← Edge Impulse exported C++ library
│                                        (added after model training)
│
├── docs/
│   ├── HARDWARE_SETUP.md              ← Wiring guide, I2S pinout
│   └── EDGE_IMPULSE_GUIDE.md          ← Step-by-step model training walkthrough
│
└── diagrams/
    └── (architecture diagrams)        ← Exported from README Mermaid blocks
```

## GitHub Topics to Add to Your Repo

When creating your GitHub repository, add these topics
(Settings → Topics in your repo):

```
silicon-labs
efr32xg24
tinyml
iot
predictive-maintenance
acoustic-anomaly-detection
edge-impulse
ble
transformer-fault-detection
centre-of-innovation-in-iot
```

## How to Submit to Silicon Labs Community Creations

1. Fork https://github.com/SiliconLabsSoftware/community-creations
2. Navigate to `projects/` → find the `bluetooth/` or `machine-learning/` category
3. Edit the README.md table in that folder, add a new row:
   | [Transformer Fault Detection using TinyML](https://github.com/User8085/transformer-fault-detector) | On-device acoustic anomaly detection for power transformer health monitoring using EFR32xG24 AI/ML accelerator and BLE alerting — no cloud required. | [User8085](https://github.com/User8085) |
4. Submit a Pull Request with title:
   `[CoI IoT] Transformer Fault Detection — Acoustic Anomaly Detection on EFR32xG24`
