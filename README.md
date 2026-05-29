# ⌨️ CapacitiveSliderKeyboard
> **2026 Hardware Side Project**

A custom HID keyboard integration project featuring an embedded **capacitive touch slider**, a **rotary encoder**, an **analog joystick**, and an **integrated USB hub** built on top of a modified Lenovo GK10 (98-key) substrate. This repository includes the custom FPCB designs and control firmware.

---

## 🚀 Features

* **Dual Capacitive Touch Sliders:** Positioned at both edges for smooth, native horizontal and vertical scrolling.
* **Custom Rotary Encoder:** A high-precision mechanical scroll wheel.
* **Analog Thumbstick:** Mapable to mouse pointer control or arrow keys.
* **Mode Switching Button & LED:** Dedicated tactile button for quick mode toggling with instant LED feedback.
* **Integrated USB Hub:** Combines the core keyboard and the custom controller down to a single USB cable output.

---

## 🛠️ Hardware Specifications

### 1. Housing & Mechanical Design
* **Enclosure:** The custom housing was 3D printed using a **Bambu Lab P1S**. Due to the build plate size limitations, it was split into a **3-piece modular design**.
* **Top Plate:** A laser-cut **2T acrylic plate**.
* **Scroll Wheel Mechanics:** Built using two precision bearings and a $\Phi3\text{mm}$-10mm shaft to ensure zero-play, smooth rotation. The magnetic disc is press-fitted directly onto the custom wheel.

### 2. Core Components
| Part | Description | Usage / Purpose |
| :--- | :--- | :--- |
| **Arduino Leonardo ETH** | ATmega32u4 MCU | Acts as the main HID controller. |
| **Adafruit CH334F** | 2-Port USB Hub | Merges the stock keyboard matrix and Arduino into one cable. |
| **MPR121** | 12-Ch Capacitive Sensor | Drives the custom touch sliders via I2C. |
| **Pololu Magnetic Encoder** | Side-entry pair kit | Detects wheel rotation via the press-fitted magnetic disc. |
| **Thumbstick** | Analog Joystick | Used for directional navigation or mouse movement. |

> 💡 **Firmware Solution for Hardware Limitations:**
> Due to the 3D-printed wall thickness (1mm PLA) and the relatively small surface area of the FPCB electrodes, the raw capacitance data from the MPR121 suffered from a low signal-to-noise ratio and noticeable flickering. To achieve smooth and reliable tracking, a custom **weight-based Center of Mass (COM) algorithm** was implemented in the firmware to calculate precise touch positions.

---

## 🔌 Pin Mapping

| Component | Arduino Leonardo Pin | Function |
| :--- | :--- | :--- |
| **LED** | `D10` | Mode Status Feedback |
| **Button** | `D14` (MISO) | Tactile Mode Switching |
| **Encoder A** | `D5` | Quadrature Encoder Phase A |
| **Encoder B** | `D6` | Quadrature Encoder Phase B |
| **MPR121 INT** | `D4` | Capacitive Touch Interrupt |
| **Thumbstick X** | `A0` | Analog X-Axis Input |
| **Thumbstick Y** | `A1` | Analog Y-Axis Input |
| **MPR121 I2C** | `D2` (SDA) / `D3` (SCL) | Sensor Data Communication |

---

## 📝 Technical Notes

* **Native Horizontal Scrolling:** Rather than mapping the slider to repetitive keystrokes, this project utilizes a **Custom HID Report Descriptor** to send true, native horizontal scroll data recognized directly by the OS.
* **Single Cable Setup:** Thanks to the onboard Adafruit CH334F hub controller, the entire setup enumerates as a single composite device on the host PC.

---

## 📂 Code & Design Availability

* **What's Included:** Control firmware code and custom slider FPCB design files.
* **What's Omitted:** The main keyboard enclosure and control panel PCB designs. The panel schematic includes modified layouts of commercial parts, and the housing geometry is tied directly to the proprietary Lenovo GK10 substrate. 
* *If you need these source files for personal reference or derivative builds, please feel free to reach out.*

---

## 📸 Gallery
<p align="center">
  <img width="922" height="388" alt="Screenshot 2026-05-30 003803" src="https://github.com/user-attachments/assets/4eda720f-edae-4137-a11d-84196f1a5ed8" />
  <img width="908" height="382" alt="Screenshot 2026-05-30 003905" src="https://github.com/user-attachments/assets/d90b4d0d-9b02-484d-9d02-96bf17f346a3" />
</p>


<img width="5712" height="4284" alt="KakaoTalk_20260530_000324815_05" src="https://github.com/user-attachments/assets/a6d177b1-313c-46d3-8c52-12d82c22921a" />


<p align="center">
  <img width="48%" alt="CapacitiveSliderKeyboard Top View" src="https://github.com/user-attachments/assets/15f3f3a7-09c0-46bd-9a4a-6d534472f29d" />
  <img width="48%" alt="CapacitiveSliderKeyboard Angle View" src="https://github.com/user-attachments/assets/acb821fd-5371-4143-8f4a-f6f58bc55164" />
</p>
