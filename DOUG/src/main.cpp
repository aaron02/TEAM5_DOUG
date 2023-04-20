#include "Defnies.h"

long new_time = 0;
long old_time = 0;
int32_t timer = 1 * TimeVar::Seconds;
uint8_t status = Status::Startup;

Antrieb* frontLeft = nullptr;
Antrieb* frontRight = nullptr;
Antrieb* backLeft = nullptr;
Antrieb* backRight = nullptr;
Navigation* nav = nullptr;
DriveTrain* driveTrain = nullptr;
Odometry* odometry = nullptr;
Gyro* gyro = nullptr;
ADNS_CTRL*   adnsController = nullptr;

void setup()
{
    // Serial Interface Initialization
    Serial.begin(9600);
    Wire.begin();
    delay(100);

    status = Status::Startup;

    // Notice us we are Starting now
    sLogger.info("Controller Staring....");

    // Set Debug Level for Logger
    sLogger.setLogType(MessageType::DEBUG);

    // Initialize Interfaces
    sLogger.info("Controller Initialize Interfaces....");
    // Motor Interface
    //frontLeft = new Antrieb("Front Left", 13, 12, 14);
    //frontRight = new Antrieb("Front Right", 27, 26, 14);
    //backLeft = new Antrieb("Back Left", 32, 33, 14);
    //backRight = new Antrieb("Back Right", 23, 25, 14);
    //driveTrain = new DriveTrain(*frontLeft, *backLeft, *frontRight, *backRight);
    //

    // Sensors
    adnsController = new ADNS_CTRL();
    gyro = new Gyro();
    //

    // Odometry + Navigation
    odometry = new Odometry(gyro, adnsController);
    //nav = new Navigation(driveTrain, odometry);
    //

    status = Status::Started;
    // Notice Our Logs we are Running :)
    sLogger.info("Controller Running....");
}

void loop()
{
    if (!status)
        return;

    new_time = micros();
    uint32_t difftime = new_time - old_time;

    // Programm Cycle
    //frontLeft->Update(difftime);
    //frontRight->Update(difftime);
    //backLeft->Update(difftime);
    //backRight->Update(difftime);

    // Mouse Sensor
    if (adnsController)
        adnsController->Update(difftime);

    // Gyro
    if (gyro)
        gyro->Update(difftime);

    // Odometry
    if(odometry)
        odometry->Update(difftime);

    // Navigation
    //nav->Update(difftime);

    // Drivetrain test
    // driveTrain->Drive(0.0, 1.0, 0.0, gyro->getGyroAngle(GYRO_AXIS::YAW));

    // Test Timer 1 second
    if (timer < 0)
    {
        sLogger.info("Controller Loop Time = %u µs", difftime);
        timer = 10 * TimeVar::Seconds;
    }
    else
        timer = timer - difftime;

    // End Loop
    old_time = new_time;
    // delay(1);
}