# API-Beschreibung Driver

Dieses Dokument beschreibt, die Kommunikation zum und vom Driver. In der Anwendung empfängt der Driver Befehle vom Controller über eine UART-Schnittstelle. Die Kommunikation wird immer vom Controller initiiert. Jeder Befehl muss mit einer Antwort quittiert werden, damit die Steuerung weiss, dass der Befehl empfangen wurde. 

## Schnittstellenparameter

Pinbelegung ist im HW-Schema ersichtlich.

| Parameter    | Wert        |
| ------------ | ----------- |
| Baud Rate    | 115200 bits/s |
| Data         | 8 bits      |
| Parity       | none        |
| Stop bits    | 1 bit       |
| Flow Control | none        |

## Syntax

Für Befehle und Antworten wird das JSON-Format verwendet.

### Senden
```json
{
    "Command":"SendBatteryState"
    "Data":{
        "BatteryState":"85"
        }
}
```

```json
{
    "Command":"SendCurrentPosition"
    "Data":{
        "x":"85",
        "y":"15"
        }
}
```

```
enum DrivingState : uint8_t
{
    DRIVE_STATE_ERROR       = 0,
    DRIVE_STATE_BUSY        = 1,
    DRIVE_STATE_FINISHED    = 2
};
```

```json
{
    "Command":"SendDrivingState"
    "Data":{
        "State":"DrivingState",
        }
}
```

```
enum ArmStatus : uint8_t
{
    AS_Undefined            = 0,
    AS_Grundstellung        = 1,
    AS_PickPackage          = 2,
    AS_PlacePackage         = 3,
    AS_Ready                = 4,
    AS_Error                = 5
};
```

```json
{
    "Command":"SendArmState"
    "Data":{
        "State":"ArmStatus",
        }
}
```
### Empfangen

```json
{
    "Command":"GetCurrentPosition"
}
```

```json
{
    "Command":"SetNextWaypoint",
    "Data":{
        "x":"240",
        "y":"30"
    }
}
```

```json
{
    "Command":"AbortDriving"
}
```

```json
{
    "Command":"GetDrivingState"
}
```

```
enum ArmStatus : uint8_t
{
    AS_Undefined            = 0,
    AS_Grundstellung        = 1,
    AS_PickPackage          = 2,
    AS_PlacePackage         = 3,
    AS_Ready                = 4,
    AS_Error                = 5
};
```

```json
{
    "Command":"SetArmState"
     "Data":{
        "State":"ArmStatus"
}
```

```json
{
    "Command":"GetArmState"
}
```

```json
{
    "Command":"PickPackage"
    "Data":{
        "RoboterIndex":"1-4",
        "LagerIndex":"1-4"
        }
}
```

```json
{
    "Command":"PlacePackage"
    "Data":{
        "RoboterIndex":"1-4"
        }
}
```

```json
{
    "Command":"GetBatteryState"
}
```

```json
{
    "Command":"CustomerAccepted"
}
```
