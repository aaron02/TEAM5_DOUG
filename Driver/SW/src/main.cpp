#include "Lib/Defnies.h"
#include <TeensyThreads.h>

// Main Loop
long new_time = 0;
long old_time = 0;

int32_t initTimer = 5 * TimeVar::Seconds;
int32_t timer = 1 * TimeVar::Seconds;

int32_t ADNStimer = 5 * TimeVar::Millis;

uint8_t status = Status::Startup;

Antrieb2* mecanumDrive = nullptr;
Antrieb* frontLeft = nullptr;
Antrieb* frontRight = nullptr;
Antrieb* backLeft = nullptr;
Antrieb* backRight = nullptr;
PosAntrieb* gripperBase = nullptr;
Navigation* nav = nullptr;
DriveTrain* driveTrain = nullptr;
Odometry* odometry = nullptr;
Gyro* gyro = nullptr;
ADNS_CTRL* adnsController = nullptr;
PDB* pdb = nullptr;
Greifer* greifer = nullptr;
Communication* com = nullptr;

void MainThread(uint32_t difftime)
{
    // Motor Update
    CALL_UPDATE(frontLeft, Update(difftime));
    CALL_UPDATE(frontRight, Update(difftime));
    CALL_UPDATE(backLeft, Update(difftime));
    CALL_UPDATE(backRight, Update(difftime));
    //CALL_UPDATE(mecanumDrive, Update(difftime));
    CALL_UPDATE(gripperBase, Update(difftime));
    // Mouse Sensor ( eats alot of resources aprox 129µs )
    CALL_UPDATE(adnsController, Update(difftime));
    // Gyro
    CALL_UPDATE(gyro, Update(difftime));
    // Power Ditribution Board
    CALL_UPDATE(pdb, Update(difftime));
    // Greiffer
    CALL_UPDATE(greifer, Update(difftime));
    // Communication to Controller
    CALL_UPDATE(com, Update(difftime));

    // Started but not ready yet
    if (status == Status::Initialization)
    {
        if (initTimer <= 0)
        {
            status = Status::Started;
            // Start Location
            adnsController->reset_xy_dist();
            odometry->setStartLocation(Vector2D(0, 0), gyro->getGyroAngle(YAW));

            // Drive Absolute Test
            //nav->setSollPosition(100, 0);

            sLogger.debug("Controller Started and Ready");
        }
        else
            initTimer = initTimer - difftime;
    }

    // We are Started and Fully Ready
    if (status == Status::Started)
    {
        // Odometry
        CALL_UPDATE(odometry, Update(difftime));

        // Navigation
        CALL_UPDATE(nav, Update(difftime));
    }

    // Test Timer 1 second
    if (1)
    {
        if (timer < 0)
        {
            sLogger.info("Controller Loop Time = %u µs (%f ms)", difftime, float(float(difftime) / Millis));
            timer = 10 * TimeVar::Seconds;
        }
        else
            timer = timer - difftime;
    }
}

void setup()
{
    // Serial Interface Initialization
    Serial.begin(9600);
    Serial1.begin(115200);
    Serial4.begin(115200);
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
    //mecanumDrive = new Antrieb2("Mecanum Drive", 2, 3, 5, 7, 9, 15);
    //driveTrain = new DriveTrain(*mecanumDrive);

    gripperBase = new PosAntrieb("Gripper Base", 25, 24, 14, 32);
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

    greifer = new Greifer(36, 33, *gripperBase, *nav);

    // Communication
    com = new Communication(nav, greifer, pdb, odometry);

    status = Status::Initialization;
    // Notice Our Logs we are Running :)
    sLogger.info("Controller Running....");
}

void loop()
{
    if (!status)
        return;

    new_time = micros();
    uint32_t difftime = new_time - old_time;

    MainThread(difftime);

    // End Loop
    old_time = new_time;
}
