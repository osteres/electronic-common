//
// Created by Thibault PLET on 06/05/2016.
//

#include "Receiver.h"
#include <com/osteres/automation/transmission/packet/Packet.h>

using namespace com::osteres::automation::transmission;

using com::osteres::automation::transmission::packet::Packet;

/**
 * Constructor
 */
Receiver::Receiver(RF24 * radio, byte sensor, unsigned int timeout) {
    this->radio = radio;
    this->sensor = sensor;
    this->timeout = timeout;
}

/**
 * Destructor
 */
Receiver::~Receiver() {
    this->cleanResponse();
}

/**
 * Clean class
 */
void Receiver::cleanResponse() {
    delete this->response;
}

/**
 * Listen response
 */
bool Receiver::listen() {
    // Start listening
    this->radio->startListening();

    // Prepare timeout
    unsigned long start = millis();
    bool noResponse = false;

    // Try to read data
    while (!noResponse) {
        // Waiting for reading data
        while (!this->radio->available() && !noResponse) {
            if (millis() - start > this->timeout) {
                noResponse = true;
            }
        }

        // Read response if available
        if (!noResponse) {
            // Clean previous response and prepare another
            this->cleanResponse();
            this->response = new Packet();

            // Read response
            this->radio->read(this->response, this->radio->getDynamicPayloadSize());

            // Check if packet are right destined to this (false positive)
            if (this->response->getTarget() != this->sensor) {
                this->response->resetData();
                noResponse = false;
            }
        }
    }

    // Security: Remove response if no response...
    if (noResponse) {
        this->cleanResponse();
    }

    // Stop listening
    this->radio->stopListening();

    // Return flag to indicate if response has been received
    return !noResponse;
}

/**
 * Flag to indicate if response has been received
 */
bool Receiver::hasResponse() {
    return this->response != 0;
}

/**
 * Get response received
 */
Packet * Receiver::getResponse() {
    return this->response;
}

/**
 * Set timeout
 */
void Receiver::setTimeout(unsigned int timeout)
{
    this->timeout = timeout;
}