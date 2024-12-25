#include "esp_camera.h"
#include "SD_MMC.h"
#include <WiFi.h>
#include <time.h> // For time functionality


// Select camera model
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM

#include "camera_pins.h"

const char * photoPrefix = "/photo_";
int photoNumber = 0;




// ===========================
// Enter your WiFi credentials
// ===========================
const char *ssid = "Tendrun";
const char *password = "niepodam123#";

extern bool isRecording;
extern bool reqStartRecording;
extern bool reqStopRecording;

void startCameraServer();
void setupLedFlash(int pin);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;


  isRecording = false;


if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;

  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }



  #if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  #endif


#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }

  s->set_framesize(s, FRAMESIZE_QVGA);

  #if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s -> set_vflip(s, 1);
  s -> set_hmirror(s, 1);
  #endif


 Serial.println("Initialising SD card");

  if (!SD_MMC.begin()) {
    Serial.println("Failed to initialise SD card!");
    return;
  }



  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");


/*
    xTaskCreate(recordVideoTask,     // Task function
                  "RecordVideoTask",   // Task name (for debugging)
                  8192,                // Stack size (adjust if needed)
                  NULL,                // Parameters to pass
                  1,                   // Task priority
                  NULL); */
}

/*
void recordVideoTask(void *param) {
    Serial.println("Initializing video recording task...");

    Serial.println("Ready for recording...");
    camera_fb_t *fb = NULL;

    reqStopRecording = false;
    isRecording = false;

    while (true) {
        if (isRecording) {
            // Check if a stop request is made
            if (reqStopRecording) {
                reqStopRecording = false;
                isRecording = false; // Stop recording
                Serial.println("Video recording stopped");
            } else {
                // Capture a frame
                fb = esp_camera_fb_get();
                if (fb) {
                    // capture camera frame
                    fb = esp_camera_fb_get();
                    writeRecordingFile(SD_MMC, "/video.mjpeg", fb->buf, fb->len);
                    esp_camera_fb_return(fb);         // Return the frame buffer
                    Serial.println("Recording...");
                    delay(1000);

                } else {
                    Serial.println("Failed to capture frame");
                }
            }
        } else {
            // Check if a start request is made
            if (reqStartRecording) {
                reqStartRecording = false;
                fb = esp_camera_fb_get();
                if (fb) {
                    isRecording = true; // Start recording
                    esp_camera_fb_return(fb); // Return the frame buffer (not used here)
                    Serial.println("Recording started...");
                } else {
                    Serial.println("Failed to start recording - no frame available");
                }
            } else {
                Serial.println("Idle...");
                delay(1000);
            }
        }
        delay(100); // Small delay to prevent CPU overload
    }
}
*/
void CreateFile() {
    // Open the file to check if it exists
    File file = SD_MMC.open("/data.txt");
    if (!file) {
        Serial.println("File doesn't exist");
        Serial.println("Creating file...");
        writeFile(SD_MMC, "/data.txt", "Epoch Time, Temperature, Humidity, Pressure \r\n");
    } else {
        Serial.println("File already exists");
    }
    file.close();
}



void writeRecordingFile(fs::FS &fs, const char *path, const uint8_t *data, size_t length) {
    Serial.printf("Writing file: %s\n", path);
    Serial.println();

    time_t now = time(nullptr); // Get the current time
    struct tm *timeinfo = localtime(&now); // Convert to local time

    // Format folder name as "YYYY-MM-DD_HH-MM-SS"
    char FileName[64];
    strftime(FileName, sizeof(FileName), "%Y-%m-%d_%H-%M-%S", timeinfo);

    // Create the final file name as a const char
    static char FinalfileName[128]; // Ensure enough space for concatenation
    snprintf(FinalfileName, sizeof(FinalfileName), "%s%s.jpg", path, FileName);

    Serial.printf("Writing file: %s\n", FinalfileName);

    File file = fs.open(FinalfileName, FILE_WRITE);
    // Write the binary data to the file
    if (file.write(data, length) == length) {
        Serial.println("Frame written successfully");
    } else {
        Serial.println("Write failed");
    }

    file.close();
}


void writeFile(fs::FS &fs, const char *path, const char *message) {
    Serial.printf("Writing file: %s\n", path);

    // Open file for writing
    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }

    // Write the message to the file
    if (file.print(message)) {
        Serial.println("File written successfully");
    } else {
        Serial.println("Write failed");
    }

    file.close();
}

  camera_fb_t *fb = NULL;

void loop() {
{
        if (isRecording) {
            // Check if a stop request is made
            if (reqStopRecording) {
                reqStopRecording = false;
                isRecording = false; // Stop recording
                Serial.println("Video recording stopped");
            } else {
                // Capture a frame
                fb = esp_camera_fb_get();
                if (fb) {
                    // capture camera frame
                    //fb = esp_camera_fb_get();
                    writeRecordingFile(SD_MMC, "/video", fb->buf, fb->len);
                    esp_camera_fb_return(fb);         // Return the frame buffer
                    Serial.println("Recording...");
                    delay(1000);

                } else {
                    Serial.println("Failed to capture frame");
                }
            }
        } else {
            // Check if a start request is made
            if (reqStartRecording) {
                reqStartRecording = false;
                fb = esp_camera_fb_get();
                if (fb) {
                    isRecording = true; // Start recording
                    esp_camera_fb_return(fb); // Return the frame buffer (not used here)
                    Serial.println("Recording started...");
                } else {
                    Serial.println("Failed to start recording - no frame available");
                }
            } else {
                Serial.println("Idle...");
                delay(1000);
            }
        }
        delay(100); // Small delay to prevent CPU overload
    }
  delay(100);
}
