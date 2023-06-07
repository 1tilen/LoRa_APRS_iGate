#include "digi_utils.h"
#include "configuration.h"
#include "lora_utils.h"


extern Configuration    Config;
extern String           thirdLine;
extern String           fourthLine;
extern int              stationMode;

namespace DIGI_Utils {

void typeOfPacket(String packet) {
    String Sender = packet.substring(3,packet.indexOf(">"));
    if (packet.indexOf("::") >= 10) {
        thirdLine = "Callsign = " + Sender;
        fourthLine = "TYPE ----> MESSAGE";
    } else if (packet.indexOf(":>") >= 10) {
        thirdLine = "Callsign = " + Sender;
        fourthLine = "TYPE ----> NEW STATUS";
    } else if (packet.indexOf(":!") >= 10 || packet.indexOf(":=") >= 10) {
        thirdLine = "Callsign = " + Sender;
        fourthLine = "TYPE ----> GPS BEACON";
    } else {
        thirdLine = "Callsign = " + Sender;
        fourthLine = "TYPE ----> ??????????";
    }
}

void process(String packet) {
    String firstPart, lastPart;
    if (packet != "") {
        Serial.print("Received Lora Packet   : " + String(packet));
        if ((packet.substring(0, 3) == "\x3c\xff\x01") && (packet.indexOf("NOGATE") == -1) && (packet.indexOf("WIDE1-1") > 10)) { // confirmar lo de WIDE1-1 !!!
            Serial.println("   ---> APRS LoRa Packet");
            typeOfPacket(packet);
            firstPart = packet.substring(3,packet.indexOf(",")+1);
            lastPart = packet.substring(packet.indexOf(":"));
            Serial.println(firstPart + Config.callsign + lastPart);
            if (stationMode == 3) {
                delay(8000);// quizas no es necesario esperar tanto?
                //LoRa_Utils::sendNewPacket("APRS", firstPart + Config.callsign + lastPart);
                Serial.println("repitiendo/Tx mensaje truncado");
                LoRa_Utils::sendNewPacket("APRS", firstPart + Config.callsign + lastPart + Config.iGateComment);  // test porque esta truncado
            } else {    // enviar con != freq
                delay(8000);
                Serial.println("repitiendo/Tx mensaje otra frecuencia");
                LoRa_Utils::changeFreqTx();
                LoRa_Utils::sendNewPacket("APRS", firstPart + Config.callsign + lastPart + "1");
                LoRa_Utils::changeFreqRx();
            }
        } else {
            Serial.println("   ---> LoRa Packet Ignored (first 3 bytes or NOGATE)\n");
        }
    }
    /*void checkReceivedPacket(String packet) {
    bool queryMessage = false;
    String aprsPacket, Sender, AddresseeAndMessage, Addressee, ackMessage, receivedMessage;
    
    if ((packet.substring(0, 3) == "\x3c\xff\x01") && (packet.indexOf("TCPIP") == -1) && (packet.indexOf("NOGATE") == -1) && (packet.indexOf("RFONLY") == -1)) {
        Serial.print("   ---> APRS LoRa Packet!");
        Sender = packet.substring(3,packet.indexOf(">"));
        if (Sender != Config.callsign) {   // avoid listening yourself by digirepeating
        if (Config.loramodule.enableTx) {
            if (packet.indexOf("::") > 10) {    // its a Message!
            AddresseeAndMessage = packet.substring(packet.indexOf("::")+2);  
            Addressee = AddresseeAndMessage.substring(0,AddresseeAndMessage.indexOf(":"));
            Addressee.trim();
            if (Addressee == Config.callsign) {            // its for me!
                if (AddresseeAndMessage.indexOf("{")>0) {    // ack?
                ackMessage = "ack" + AddresseeAndMessage.substring(AddresseeAndMessage.indexOf("{")+1);
                ackMessage.trim();
                delay(4000);
                Serial.println(ackMessage);
                for(int i = Sender.length(); i < 9; i++) {
                    Sender += ' ';
                }
                LoRa_Utils::sendNewPacket("APRS", Config.callsign + ">APLRG1,RFONLY::" + Sender + ":" + ackMessage);
                receivedMessage = AddresseeAndMessage.substring(AddresseeAndMessage.indexOf(":")+1, AddresseeAndMessage.indexOf("{"));
                } else {
                receivedMessage = AddresseeAndMessage.substring(AddresseeAndMessage.indexOf(":")+1);
                }
                if (receivedMessage.indexOf("?") == 0) {
                queryMessage = true;
                delay(2000);
                if (!Config.display.alwaysOn) {
                    display_toggle(true);
                }
                LoRa_Utils::sendNewPacket("APRS", QUERY_Utils::process(receivedMessage, Sender, "LoRa"));
                lastRxTxTime = millis();
                show_display(firstLine, secondLine, "Callsign = " + Sender, "TYPE --> QUERY",  1000);
                } 
            }
            }
        }
        if (!queryMessage) {
            aprsPacket = APRS_IS_Utils::createPacket(packet);
            if (!Config.display.alwaysOn) {
            display_toggle(true);
            }
            lastRxTxTime = millis();
            espClient.write(aprsPacket.c_str());
            Serial.println("   ---> Uploaded to APRS-IS");
            STATION_Utils::updateLastHeard(Sender);
            if (aprsPacket.indexOf("::") >= 10) {
            show_display(firstLine, secondLine, "Callsign = " + Sender, "TYPE --> MESSAGE",  1000);
            } else if (aprsPacket.indexOf(":>") >= 10) {
            show_display(firstLine, secondLine, "Callsign = " + Sender, "TYPE --> NEW STATUS", 1000);
            } else if (aprsPacket.indexOf(":!") >= 10 || aprsPacket.indexOf(":=") >= 10) {
            show_display(firstLine, secondLine, "Callsign = " + Sender, "TYPE --> GPS BEACON", 1000);
            } else {
            show_display(firstLine, secondLine, "Callsign = " + Sender, "TYPE --> ??????????", 1000);
            }
        }
        }    
    } else {
        Serial.println("   ---> Not APRS Packet (Ignore)\n");
    }*/
}

}