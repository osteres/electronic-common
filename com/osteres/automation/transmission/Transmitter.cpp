//
// Created by Thibault PLET on 04/05/2016.
//

#include "Transmitter.h"

using namespace com::osteres::automation::transmission;

/**
 * Default ttl (in milliseconds)
 */
unsigned int Transmitter::defaultTtl = 1000;

/**
 * Constructor
 */
Transmitter::Transmitter(RF24 * radio, unsigned char sensor, bool isMaster)
{
    Transmitter::construct(radio, sensor, isMaster);
}

/**
 * Destructor
 */
Transmitter::~Transmitter()
{
    // Remove requester and receiver
    if (this->requester != NULL) {
        delete this->requester;
        this->requester = NULL;
    }
    if (this->receiver != NULL) {
        delete this->receiver;
        this->receiver = NULL;
    }
}

/**
 * Constructor
 */
void Transmitter::construct(RF24 * radio, unsigned char sensor, bool isMaster)
{
    this->radio = radio;
    this->sensor = sensor;
    this->master = isMaster;

    // Channels
    unsigned long long writingChannel = Command::CHANNEL_SLAVE;
    unsigned long long readingChannel = Command::CHANNEL_MASTER;
    if (this->isMaster()) {
        writingChannel = Command::CHANNEL_MASTER;
        readingChannel = Command::CHANNEL_SLAVE;
    }
    this->setReadingChannel(readingChannel);
    this->setWritingChannel(writingChannel);
}

/**
 * Get default ttl before timeout (in seconds)
 */
unsigned int Transmitter::getDefaultTTL()
{
    return defaultTtl;
}

/**
 * Set default ttl before timeout (in seconds)
 */
void Transmitter::setDefaultTTL(unsigned int ttl)
{
    defaultTtl = ttl;
}

/**
 * Action manager setter
 */
void Transmitter::setActionManager(ActionManagerBase * actionManager) {
    this->actionManager = actionManager;
}

/**
 * Get requester
 * Initialize it if necessary
 */
Requester * Transmitter::getRequester() {
    return this->requester;
}

/**
 * Get receiver
 * Initialize if if necessary
 */
Receiver * Transmitter::getReceiver() {
    return this->receiver;
}