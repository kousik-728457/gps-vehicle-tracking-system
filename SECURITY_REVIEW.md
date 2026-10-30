# Packaging security review

This copy was prepared for public repository upload. The following items were handled:

- Excluded frontend `.env.local`; supplied `.env.example` contains placeholders only.
- Replaced the hardcoded SMS destination in `firmware/MADUINO_GPS/MADUINO_GPS.ino` with `+91XXXXXXXXXX`.
- Changed the backend MQTT client to read `MQTT_BROKER_URL` from the environment, defaulting to localhost.
- Excluded a one-off request-bin test script containing a unique external endpoint.
- Excluded dependency/build directories (`node_modules/`, `.next/`).
- Excluded two local GPS coordinate-trace CSVs from the backend archive.

The supplied firmware still contains public/test broker and topic configuration in its sketches. Those are not passwords, but they are not private or access-controlled. Configure an authenticated broker and unique topics before real vehicle use.

This is a source-file hygiene pass, not a full penetration test or hardware validation. Review the final repository and rotate any credentials that may have been exposed in earlier commits.
