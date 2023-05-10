#include "Defnies.h"
#include <TeensyThreads.h>

// Main Loop
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
ADNS_CTRL* adnsController = nullptr;
PDB* pdb = nullptr;

void motorThread()
{
    while (1)
    {
        // Programm Cycle
        frontLeft->Update(0);
        frontRight->Update(0);
        backLeft->Update(0);
        backRight->Update(0);
    }
}

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
    frontLeft = new Antrieb("Front Left", 2, 3, 15);
    frontRight = new Antrieb("Front Right", 4, 5, 15);
    backLeft = new Antrieb("Back Left", 6, 7, 15);
    backRight = new Antrieb("Back Right", 8, 9, 15);
    driveTrain = new DriveTrain(*frontLeft, *backLeft, *frontRight, *backRight);
    //

    // Sensors
    adnsController = new ADNS_CTRL();
    gyro = new Gyro();
    pdb = new PDB();
    //

    // Odometry + Navigation
    odometry = new Odometry(gyro, adnsController);
    nav = new Navigation(driveTrain, odometry);
    //

    // Multithreading
    threads.addThread(motorThread);

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

    // Mouse Sensor
    if (adnsController)
        adnsController->Update(difftime);

    // Gyro
    if (gyro)
        gyro->Update(difftime);

    // Power Ditribution Board
    if (pdb)
        pdb->Update(difftime);

    // Odometry
    if(odometry)
        odometry->Update(difftime);

    // Navigation
    /*if (nav)
        nav->Update(difftime);*/

    // Test Timer 1 second
    if (0)
    {
        if (timer < 0)
        {
            // Drivetrain test
            //driveTrain->Drive(1.0, 0.0, 0.0, gyro->getGyroAngle(GYRO_AXIS::YAW));

            sLogger.info("Controller Loop Time = %u µs", difftime);
            timer = 10 * TimeVar::Seconds;
        }
        else
            timer = timer - difftime;
    }

    // End Loop
    old_time = new_time;
}
