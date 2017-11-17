# Jeti VarioGPS-Sensor

VarioGPS-Sensor für Jeti EX-Bus. Die Firmware kann mit folgenden Sensoren betrieben werden: GPS, Luftdrucksensor BMP280, BME280, MS5611, LPS, des weiteren können an den Analogeingängen 0-3 bis zu vier Spannungen gemessen werden. Nicht benötigte Sensoren müssen nicht angeschlossen werden. Die Firmware kann einfach auf ein Arduino Pro Mini geladen werden, die nötigen Einstellungen können danach per Jetibox vorgenommen werden. 

Hardware:
- Arduino Pro Mini 8Mhz 3.3V
- Ublox GPS-Modul mit 9600baud
- Barometer Modul mit BMP280, BME280, MS5611, LPS


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

Zusätzlich können bis zu 4 Spannungen gemessen werden

Folgende Einstellungen können per Jetibox vorgenommen werden:

- GPS: deaktiviert, Basic oder Extended
- GPS Distanz: 2D oder 3D
- Filterparameter X, Y und Deadzone
- Spannungsmessungen 1-4 de-/aktiviert
