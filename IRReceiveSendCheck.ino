#include <Arduino.h>
#include <IRremote.h>

// Pins
#define IR_RECV_PIN 35 // TSOP1838
#define IR_SEND_PIN 4  // IR LED via 100-220Ω resistor

uint32_t lastCode = 0; // Store last received code

void setup()
{
    Serial.begin(115200);
    delay(200);

    // Start IR receiver
    IrReceiver.begin(IR_RECV_PIN, ENABLE_LED_FEEDBACK);
    Serial.println("=== ESP32 IR Receiver Ready ===");

    // Start IR sender
    IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK);
    Serial.println("=== ESP32 IR Sender Ready ===");
}

void loop()
{
    // Check if IR received
    if (IrReceiver.decode())
    {
        uint32_t code = IrReceiver.decodedIRData.decodedRawData;
        decode_type_t protocol = IrReceiver.decodedIRData.protocol;

        // Ignore repeat / invalid signals
        if (code != 0 && code != lastCode)
        {
            Serial.print("Received HEX: 0x");
            Serial.print(code, HEX);
            Serial.print("  Protocol: ");
            Serial.println(protocol);

            lastCode = code;

            // Send IR back (clone)
            Serial.println("Sending IR...");
            switch (protocol)
            {
            case NEC:
                IrSender.sendNEC(code, 32);
                break;
            case SONY:
                IrSender.sendSony(code, IrReceiver.decodedIRData.numberOfBits);
                break;
            case RC5:
                IrSender.sendRC5(code, IrReceiver.decodedIRData.numberOfBits);
                break;
            case RC6:
                IrSender.sendRC6(code, IrReceiver.decodedIRData.numberOfBits);
                break;
            case JVC:
                IrSender.sendJVC(code, IrReceiver.decodedIRData.numberOfBits, false);
                break;
            case SAMSUNG:
                IrSender.sendSAMSUNG(code, 32);
                break;
            case LG:
                IrSender.sendLG(code, 32);
                break;
            case PANASONIC:
            {
                uint16_t address = (code >> 8) & 0xFFFF;     // top 16 bits
                uint8_t command = code & 0xFF;               // bottom 8 bits
                IrSender.sendPanasonic(address, command, 0); // send once
                break;
            }
            default:
                Serial.println("Protocol not supported for sending!");
                break;
            }
        }

        // Prepare for next reception
        IrReceiver.resume();
    }
}
