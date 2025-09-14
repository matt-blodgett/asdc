# asdc

A third party desktop client for Arctic Spa brand hot tubs. Uses a reverse-engineered TCP/protobuf interface for controlling and viewing the status of your hot tub temperature, jets, lights, and much more.

## Overview

Arctic Spa hot tubs communicate over TCP using protobuf messages. By writing a specifically crafted packet that includes a message type integer to the connection with the device, the onboard controller in the hot tub will respond with a corresponding protobuf message.

## License

Licensed under the [GPL-3.0](./LICENSE) license.

## Disclaimer

This project is not affiliated in any way with the Arctic Spas company or brand.

A hot tub is a significant financial investment and there is inherent risk in using unauthorized third party tools to interact with your device.

By choosing to use software, you assume full responsibility for any damage, loss, or consequences that could potentially result.  
