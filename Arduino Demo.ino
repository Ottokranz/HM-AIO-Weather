    #include <Bridge.h>
    #include <YunServer.h>
    #include <YunClient.h>
    #include <HttpClient.h>

    #define WATCH_PIN_A 2
    #define WATCH_PIN_B 9

    int lastPinValueA = 0;
    int lastPinValueB = 0;

    int watchPin1 =

    YunServer server;

    void setup() {
      Serial.begin(9600);
      Bridge.begin();
      server.listenOnLocalhost(); // listen on default port 5555
      server.begin();
    }

    void loop() {
      // Nachsehen ob Befehle vom LAN kommen
      YunClient client = server.accept();
      if (client) {
        process(client);
        client.stop();
      }
      delay(50); // Poll every 50ms
      // Nachsehen ob sich PINs geändert haben und ggf. an LAN melden
      watchPins();
      delay(50); // Poll every 50ms
    }

    void process(YunClient client) {
      int value = 0;
      String command = client.readStringUntil('/');
      int pin = client.parseInt();
      if (command == "pinModeInput") {
        pinMode(pin, INPUT);
        client.println("D" + pin + "=INPUT");
      }
      if (command == "pinModeOutput") {
        pinMode(pin, OUTPUT);
        client.println("D" + pin + "=OUTPUT");
      }
      if (command == "digitalRead") {
        value = digitalRead(pin);
        client.println("D" + pin + "=" + value);
      }
      if (command == "analogRead") {
        value = analogRead(pin);
        client.println("A" + pin + "=" + value);
      }
      if (client.read() == '/') {
        value = client.parseInt();
        if (command == "digitalWrite") {
          digitalWrite(pin, value);
          client.println("D" + pin + "=" + value);
        }
        if (command == "analogWrite") {
          analogWrite(pin, value);
          client.println("A" + pin + "=" + value);
        }
      }
    }

    void watchPins() {
      int value = 0;
      // Pin A (digital)
      value = digitalRead(WATCH_PIN_A);
      if (lastPinValueA != value) {
        lastPinValueA = value;
        String url = "http://<IP der CCU>:8181/do.exe?r1=dom.GetObject(\"SystemvariablePinA\").State(" + value + ")";
        HttpClient client;
        client.get(url);
      }
      // Pin B (analog)
      value = analogRead(WATCH_PIN_B);
      if (lastPinValueB != value) {
        lastPinValueB = value;
        String url = "http://<IP der CCU>:8181/do.exe?r1=dom.GetObject(\"SystemvariablePinB\").State(" + value + ")";
        HttpClient client;
        client.get(url);
      }
    }