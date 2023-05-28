# API-Beschreibung Driver

Dieses Dokument beschreibt, die Kommunikation zum und vom Driver. In der Anwendung empfängt der Driver Befehle vom Controller über eine UART-Schnittstelle. Die Kommunikation wird immer vom Controller initiiert. Jeder Befehl muss mit einer Antwort quittiert werden, damit die Steuerung weiss, dass der Befehl empfangen wurde. 

## Schnittstellenparameter

Pinbelegung ist im HW-Schema ersichtlich.

| Parameter    | Wert        |
| ------------ | ----------- |
| Baud Rate    | 9600 bits/s |
| Data         | 8 bits      |
| Parity       | none        |
| Stop bits    | 1 bit       |
| Flow Control | none        |

## Syntax

Für Befehle und Antworten wird das JSON-Format verwendet.

### Beispiele

Batteriestatus abfragen:

```json
{
    "Command":"GetBatteryState"
}
```

```json
{
    "Data":{
        "BatteryState":"85"
    }
}
```

Nächsten Waypoint senden:

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
    "Response":"Ok"
}
```

Nächsten Waypoint senden (mit Fehler):

```json
{
    "Command":"SetNextWaypoint",
    "Data":{
        "x":"240"
    }
}
```

```json
{
    "Response":"Error",
    "Message":"ArgumentError"
}
```

## Befehle

| Befehl               | Parameter                                           | Antwort                                                        |
| -------------------- | --------------------------------------------------- | -------------------------------------------------------------- |
| `GetCurrentPosition` |                                                     |                                                                |
| `SetDrivingWaypoint` | X und Y-Koordinaten in mm                           | `Ok`                                                           |
| `AbortDriving`       |                                                     | `Ok`                                                           |
| `GetDrivingState`    |                                                     | `Busy`: Unterwegs<br>`Finished`: Angekommen<br>`Error`: Fehler |
| `SetArmState`        | `Ready`: Arm vorbereiten<br>`Stored`: Arm verstauen | `Ok`                                                           |
| `GetArmState`        |                                                     | `Ready`: Arm beriet<br>`Stored`: Arm verstaut                  |
| `PickPackage`        | Lagerindex                                          | `Ok`                                                           |
| `PlacePackage`       | Lagerindex                                          | `Ok`                                                           |
| `GetBatteryState`    |                                                     | Batteriestatus in %                                            |
