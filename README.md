# Jeti VarioGPS-Sensor

VarioGPS-Sensor für Jeti EX-Bus. Die Firmware kann mit folgenden Sensoren betrieben werden: GPS, Luftdrucksensor BMP280, BME280, MS5611, LPS, des weiteren können an den Analogeingängen 0-3 bis zu vier Spannungen oder vier Ströme gemessen werden. Nicht benötigte Sensoren müssen nicht angeschlossen werden. Die Firmware kann einfach auf ein Arduino Pro Mini geladen werden, die nötigen Einstellungen können danach per Jetibox vorgenommen werden. 

Unterstützte Hardware:
- Arduino Pro Mini 8Mhz 3.3V
- GPS-Modul mit UART@9600baud
- Luftdrucksensoren: BMP280, BME280, MS5611, LPS
- Stromsensoren: ACS758_50B, ACS758_100B, ACS758_150B, ACS758_200B, ACS758_50U, ACS758_100U, ACS758_150U, ACS758_200U


Ohne GPS mit Barometer werden die Werte angezeigt:
- Rel. und Abs. Höhe
- Vario
- Luftdruck
- Temperatur
- Luftfeuchte (nur mit BME280)

Im GPS Basic Mode werden die Werte angezeigt:
- Position
- Geschwindigkeit 
- Rel. und Abs. Höhe
- Vario

Im GPS Extended Mode werden zusätzlich die Werte angezeigt:
- Distanz vom Modell zum Startpunkt (2D und 3D)
- Flugrichtung (Heading)
- Kurs vom Modell zum Startpunkt
- Anzahl Satelliten
- HDOP (Horizontaler Faktor der Positionsgenauigkeit)
- Luftdruck
- Temperatur
- Luftfeuchtigkeit 

Zusätzlich können bis zu vier Spannungen[V] oder vier Ströme[A] gemessen werden.

Folgende Einstellungen können per Jetibox vorgenommen werden:

- GPS: deaktiviert, Basic oder Extended
- GPS Distanz: 2D oder 3D
- Vario Filterparameter X, Y und Deadzone
- Analogeingänge 1-4 konfigurierbar (deaktiviert, Spannungs- oder Strommessung)
