# 🚗 GPS Vehicle Tracking System — Maduino Zero 4G LTE

<p align="center">
  <strong>TRACK • LOCATE • ALERT • MONITOR</strong><br>
  An embedded vehicle-tracking prototype combining a Maduino/SIM7600 modem, GPS positioning, MQTT messaging, and a web dashboard.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Embedded-Maduino%20%2F%20SIM7600-blue" alt="Maduino SIM7600">
  <img src="https://img.shields.io/badge/Connectivity-4G%20LTE-green" alt="4G LTE">
  <img src="https://img.shields.io/badge/Messaging-MQTT-orange" alt="MQTT">
  <img src="https://img.shields.io/badge/Frontend-Next.js-black" alt="Next.js">
  <img src="https://img.shields.io/badge/Backend-Node.js-brightgreen" alt="Node.js">
</p>

---

## Overview

This repository contains the firmware sketches, Node.js backend and Next.js frontend supplied for an academic GPS vehicle-tracking project. The firmware variants show GPS acquisition using SIM7600 AT commands, coordinate conversion, cellular data connectivity, MQTT publishing, and (in the relevant sketch) SOS/OK alert topics. The web application contains a dashboard component, map-related assets and a Supabase client configured through environment variables.

The repository preserves the supplied implementation files while separating firmware, backend and frontend components. It is a project prototype, not a hardened production fleet-tracking service.

## Features represented in the supplied source

- GPS position acquisition through SIM7600 commands such as `AT+CGPSINFO`.
- Conversion of GPS coordinates from degrees/minutes format to decimal degrees.
- Cellular packet-data setup using a configured APN.
- MQTT publishing/subscribing for tracking, SOS and OK message topics.
- Node.js services for MQTT, UDP, HTTP and socket-based data handling.
- Next.js dashboard interface with map-related assets.
- Optional MongoDB persistence in the relay server.

> Exact end-to-end behavior depends on which firmware sketch and backend service are deployed together. See the source files before wiring or running the system.

## System architecture

```text
GPS satellites
      |
      v
SIM7600 GNSS / Maduino controller
      |
      +---- GPS position (NMEA / CGPSINFO-derived data)
      +---- SOS / OK status inputs (firmware variant dependent)
      |
      v
4G LTE data connection
      |
      v
MQTT broker / Node.js backend services
      |
      +---- Optional MongoDB relay service
      +---- Socket / UDP / HTTP services
      |
      v
Next.js dashboard
```

## Repository structure

```text
gps-vehicle-tracking-system/
├── firmware/
│   ├── Blueband_final/
│   ├── Blueband_with_serial/
│   └── MADUINO_GPS/
├── backend/
│   ├── car44.js
│   ├── relay_server.js
│   ├── udp_server.js
│   ├── package.json
│   ├── .env.example
│   └── ...
├── frontend/
│   ├── src/
│   ├── public/
│   ├── package.json
│   └── .env.example
├── .gitignore
└── README.md
```

## Hardware

- Maduino Zero 4G LTE / SIM7600-based board (as indicated by the supplied sketches).
- Compatible SIM card and cellular antenna.
- GPS/GNSS antenna where required by the board/module.
- SOS/OK switches for the firmware variant that defines those inputs.
- Suitable regulated power supply.

Confirm the exact board revision, antenna connections, pin mapping and supply requirements against your hardware documentation before powering the prototype.

## Software requirements

- Arduino IDE and the appropriate Maduino/SIM7600 board support.
- Arduino libraries required by the selected sketch (for example TinyGSM, PubSubClient, and, for the OLED sketch, Adafruit GFX and Adafruit SSD1306).
- Node.js and npm for backend/frontend components.
- A reachable MQTT broker.
- Optional MongoDB instance for the relay service.
- Supabase project configuration for the frontend client, if using that integration.

## Setup

### 1. Clone the repository

```bash
git clone <YOUR_GITHUB_REPOSITORY_URL>
cd gps-vehicle-tracking-system
```

### 2. Configure the backend

```bash
cd backend
cp .env.example .env
npm install
```

Set `MQTT_BROKER_URL` to a broker you control or are authorized to use. Set `MONGODB_URI` only if running the MongoDB relay service. Do not commit `.env`.

The backend `car44.js` defaults to `mqtt://localhost:1883` when `MQTT_BROKER_URL` is not set. Start only the service script appropriate to your deployment; inspect its port and topic configuration first.

### 3. Configure the frontend

```bash
cd ../frontend
cp .env.example .env.local
npm install
npm run dev
```

Fill in `NEXT_PUBLIC_SUPABASE_URL` and `NEXT_PUBLIC_SUPABASE_ANON_KEY` with the values for your own Supabase project if the dashboard uses Supabase. Configure Supabase Row Level Security (RLS) policies for the tables accessed by the frontend. Never place a Supabase service-role key in browser code.

### 4. Upload firmware

Open the selected `.ino` file under `firmware/` in Arduino IDE, select the correct Maduino board and port, install its required libraries, and upload. Configure the APN, MQTT broker and alert destination for your own test setup. The public copy of `MADUINO_GPS.ino` uses a masked phone-number placeholder; replace it locally with a number you control before testing SMS.

## Firmware variants

| Sketch | Supplied functionality indicated by source |
|---|---|
| `Blueband_final/Blueband_final.ino` | SIM7600/GPRS setup, MQTT client, OLED display, tracking and SOS/OK topic handling |
| `Blueband_with_serial/Blueband_with_serial.ino` | Serial-debug version with GPS coordinate parsing and MQTT tracking publish |
| `MADUINO_GPS/MADUINO_GPS.ino` | GPS coordinate parsing and SMS transmission |

These are separate sketches/iterations. They are not intended to be compiled together.

## Security and privacy notes

- `.env.local` and other local environment files from the supplied frontend archive have been excluded. Use the provided `.env.example` templates.
- The phone number hardcoded in the supplied SMS sketch has been replaced with `+91XXXXXXXXXX` in this public-ready copy.
- A one-off Pipedream request-bin test script was excluded.
- `node_modules/` and Next.js `.next/` build output were excluded; install dependencies locally using `npm install`.
- Two local coordinate-trace CSV files were excluded to avoid publishing potentially private journey data. Public/sample track CSVs from the supplied backend archive remain.
- The source sketches reference an unauthenticated MQTT setup/topic pattern. Public MQTT brokers and predictable topics are not appropriate for private vehicle tracking. Use an authenticated broker, TLS, access controls and non-guessable per-device topics for any real deployment.
- If any real token, phone number, password or private endpoint was previously committed to a public repository, rotate/revoke it. Deleting it in a later commit does not erase it from Git history.

## Limitations

This package is a cleaned organization of the files supplied for the project. It has not been hardware-tested as a complete integrated deployment in this packaging step. Pin assignments, backend service selection, broker configuration, Supabase schema/RLS and live map behavior must be verified against the actual working setup.

## Author

**Kousik Jayanthi**  
Electronics and Communication Engineering  
Interests: Embedded Systems • IoT • GPS & Cellular Communication

---

If you find this project useful, consider starring the repository.
