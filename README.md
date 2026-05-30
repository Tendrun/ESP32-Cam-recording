# ESP32-CAM Dashcam & Web Recorder 🚗📷

This was one of my favorite university projects. It turns an **ESP32-CAM** module into a fully functional dashcam. It hosts a web server where you can view a live camera stream and control recording directly to an SD card.

Originally built to serve as a vehicle dashcam for recording traffic and potential collisions, it provides a simple web interface to monitor the feed and manage the camera remotely.

## ✨ Features
* **Live Web Stream:** Hosts a local web server that provides a real-time video feed from the camera.
* **SD Card Recording:** Records and saves video footage directly to an onboard MicroSD card.
* **Remote Controls:** Use the web interface to easily toggle video recording (Start/Stop) or snap still photos.
* **Dashcam Utility:** Designed to be mounted in a car to document drives and capture any incidents on the road.

## 🛠️ Hardware Requirements
* **ESP32-CAM** module (e.g., with an OV2640 camera)
* **MicroSD Card** (formatted to FAT32)
* Power supply (e.g., car USB adapter, power bank, or standard 5V supply)

## 🚀 Usage
1. Flash the code to your ESP32-CAM.
2. Insert a MicroSD card into the module.
3. Power on the device and connect to its IP address via a web browser.
4. From the web panel, you can watch the live stream, press **Record** to save video to the SD card, or click **Take Photo** to capture an image.
