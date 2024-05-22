#include <ArduinoBLE.h>
#include <Arduino_LSM6DSOX.h>
#include <SensorFusion.h>
SF fusion;

float acc_x, acc_y, acc_z;
float gyr_x, gyr_y, gyr_z;
float gyrXoffs, gyrYoffs, gyrZoffs;
float pitch, roll, yaw;
float deltat;

float last_acc_x, last_acc_y, last_acc_z;
float last_gyr_x, last_gyr_y, last_gyr_z;

#define PI 3.14159265358979323846

#define BLE_UUID_SENSOR_DATA_SERVICE              "2BEEF31A-B10D-271C-C9EA-35D865C1F48A"
#define BLE_UUID_MULTI_SENSOR_DATA                "4664E7A1-5A13-BFFF-4636-7D0A4B16496C"

#define NUMBER_OF_SENSORS 3

const int ledPin = LED_BUILTIN; // set ledPin to on-board LED

union multi_sensor_data
{
  struct __attribute__( ( packed ) )
  {
    float values[NUMBER_OF_SENSORS];
  };
  uint8_t bytes[ NUMBER_OF_SENSORS * sizeof( float ) ];
};

union multi_sensor_data multiSensorData;

//Service for publish Accelerometer value...
BLEService sensorDataService( BLE_UUID_SENSOR_DATA_SERVICE );
BLECharacteristic multiSensorDataCharacteristic( BLE_UUID_MULTI_SENSOR_DATA, BLERead | BLENotify, sizeof multiSensorData.bytes );

// Conversion from degrees per second to radians per second
float deg_to_rad(float deg_per_sec) {
    float rad_per_sec = deg_per_sec * (PI / 180.0);
    return rad_per_sec;
}

// Conversion from grams per second to meters per second squared
// Assuming 1 gram force = 0.00980665 Newton
float g_to_ms2(float g_per_sec) {
    float ms2 = g_per_sec * 0.00980665;
    return ms2;
}

void gyroscope_calibrate(){
  Serial.println("Start gyroscope calibration, please keep device stable for around 5 seconds");
  int x;
  float xSum = 0, ySum = 0, zSum = 0;
  int num = 500;

  for (x = 0; x < num; x++){
    IMU.readGyroscope(gyr_x, gyr_y, gyr_z);
    xSum = xSum + gyr_x;
    ySum = ySum + gyr_y;
    zSum = zSum + gyr_z;
    delay(10);
  }

  gyrXoffs = xSum / num;
  gyrYoffs = ySum / num;
  gyrZoffs = zSum / num;

  Serial.println("Calibration result:");
  Serial.println("GX \t GY \t GZ");
  Serial.print(gyrXoffs);
  Serial.print("\t");
  Serial.print(gyrYoffs);
  Serial.print("\t");
  Serial.println(gyrZoffs);
  Serial.println("\t");
} 

void read_data(){
  Serial.println("Reading Acceleration");
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(acc_x, acc_y, acc_z);
  } else {
    Serial.println("Failed to read acceleration");
  }
    Serial.println("Reading Gyyroscope");
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(gyr_x, gyr_y, gyr_z);
  } else {
    Serial.println("Failed to read gyroscope");
  }
  Serial.println("Saving variables");
  if (isnan(acc_x)) acc_x = last_acc_x;
  else last_acc_x = acc_x;
  
  if (isnan(acc_y)) acc_y = last_acc_y;
  else last_acc_y = acc_y;
  
  if (isnan(acc_z)) acc_z = last_acc_z;
  else last_acc_z = acc_z;
  
  if (isnan(gyr_x)) gyr_x = last_gyr_x;
  else last_gyr_x = gyr_x;
  
  if (isnan(gyr_y)) gyr_y = last_gyr_y;
  else last_gyr_y = gyr_y;
  
  if (isnan(gyr_z)) gyr_z = last_gyr_z;
  else last_gyr_z = gyr_z;

  Serial.println("AX \t AY \t AZ");
  Serial.print(acc_x);
	Serial.print('\t');
	Serial.print(acc_y);
	Serial.print('\t');
	Serial.print(acc_z);
	Serial.println('\t');
  Serial.println("GX \t GY \t GZ");
  Serial.print(gyr_x);
	Serial.print('\t');
	Serial.print(gyr_y);
	Serial.print('\t');
	Serial.print(gyr_z);
	Serial.println('\t');


  deltat = fusion.deltatUpdate();
  Serial.println("deltat");
  Serial.print(deltat);
  Serial.println('\t');

  Serial.println("Calculating the pitch roll yaw");
  gyr_x=deg_to_rad(gyr_x-gyrXoffs);
  gyr_y=deg_to_rad(gyr_y-gyrYoffs);
  gyr_z=deg_to_rad(gyr_z-gyrZoffs);
  acc_x=g_to_ms2(acc_x);
  acc_y=g_to_ms2(acc_y);
  acc_z=g_to_ms2(acc_z);

  fusion.MahonyUpdate(gyr_x, gyr_y, gyr_z, acc_x, acc_y, acc_z, deltat); 

  pitch = fusion.getPitch();
  roll = fusion.getRoll();   
  yaw = fusion.getYaw();
  Serial.println("Pitch \t Roll \t Yaw \t");
  Serial.print(pitch);
  Serial.print('\t');
  Serial.print(roll);
  Serial.print('\t');
  Serial.print(yaw);
  Serial.println('\t');
}

void setup() {
  Serial.begin(115200);
  //while (!Serial); //Debug in Serial

  pinMode(ledPin, OUTPUT); // use the LED as an output
 
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  if (!IMU.begin())
  { 
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println("Hz");
  Serial.println();

  gyroscope_calibrate();

  BLE.setDeviceName("IMU");
  // set the local name peripheral advertises
  BLE.setLocalName("IMU");
  
  BLE.setAdvertisedService(sensorDataService);
  
  // BLE add characteristics
  sensorDataService.addCharacteristic( multiSensorDataCharacteristic );
  
  // add service
  BLE.addService( sensorDataService );
  
  // set the initial value for the characeristic:
  multiSensorDataCharacteristic.writeValue( multiSensorData.bytes, sizeof multiSensorData.bytes );
  
  BLE.setConnectable(true);
  // BLE.setAdvertisingInterval(100);// configure the interval time for BLE
  BLE.advertise();
  
  Serial.println("Bluetooth Device Active, Waiting for Connections...");

}

void loop() {
  #define UPDATE_INTERVALL 5
  static long previousMillis = 0;
  
  BLEDevice central = BLE.central();

  if(central) {
    Serial.print("Connected to Central: ");
    Serial.println(central.address());

	  while(central.connected()) {
		  unsigned long currentMillis = millis();
		  if (currentMillis - previousMillis >= UPDATE_INTERVALL) {
			  previousMillis = currentMillis;

        Serial.println(millis());
			  
        read_data();
        delay(10);

        Serial.println("Get data and write to BLE");
        multiSensorData.values[0] = pitch;
			  multiSensorData.values[1] = roll;
			  multiSensorData.values[2] = yaw;
        
			  multiSensorDataCharacteristic.writeValue( multiSensorData.bytes, sizeof multiSensorData.bytes );
        Serial.println(millis());

		  }
    }
    Serial.print("Disconnected from Central: ");
    Serial.println(central.address());
  }
  

}