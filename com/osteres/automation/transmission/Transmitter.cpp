//
// Created by Thibault PLET on 04/05/2016.
//

#include "Transmitter.h"
#include <iostream>
#include <inttypes.h>

using namespace std;
using namespace com::osteres::automation::transmission;

/**
 * Default ttl (in milliseconds)
 */
unsigned int Transmitter::defaultTtl = 1000;

/**
 * Max packet allowed in queue
 */
unsigned int Transmitter::maxPacketQueue = 10;

/**
 * Constructor
 */
Transmitter::Transmitter(RF24 * radio, bool isMaster)
{
    Transmitter::construct(radio, isMaster);
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

    // Remove queue
    if (this->queue != NULL) {
        delete this->queue;
        this->queue = NULL;
    }
    if (this->queueSended != NULL) {
        delete this->queueSended;
        this->queueSended = NULL;
    }
}

/**
 * Constructor
 */
void Transmitter::construct(RF24 * radio, bool isMaster)
{
    this->radio = radio;
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

    // Queue
    this->queue = new vector<Packing *>();
    this->queueSended = new vector<Packing *>();
}

/**
 * Init step
 */
void Transmitter::stepInit()
{
    Packing * packing = NULL;

    // Move sended queue to normal queue (= resend packet!)
    this->getQueue()->insert(this->getQueue()->end(), this->getQueueSended()->begin(), this->getQueueSended()->end());
    this->getQueueSended()->clear();
}

/**
 * Send step
 */
void Transmitter::stepSend()
{
    Packing * packing = NULL;
    //Serial.println("Step send (with " + String(this->getQueue()->size()) + " packing)");
    cout << "Step send, with " << (int)this->getQueue()->size() << " packing" << endl;

    // For each packing in queue
    while (this->getQueue()->size() > 0) {
        packing = this->getQueue()->at(0);
        this->getQueue()->erase(this->getQueue()->begin());
        // Security
        if (packing == NULL) {
            continue;
        }
        if (packing->getPacket() == NULL) {
            delete packing;
            packing = NULL;
            continue;
        }
        // Set last property
        packing->getPacket()->setLast(this->getQueue()->empty());

        // Send
        this->send(packing);

        // If confirm asked, move to other queue
        if (packing->isNeedConfirm()) {
            cout << "Save packet to sended queue" << endl;
            this->getQueueSended()->push_back(packing);
            cout << "Queue sended push bash" << endl;
        } else if (packing != NULL) {
            // Free memory
            delete packing;
            packing = NULL;
        }
    }
    // Here, queue is empty
}

/**
 * Receive step
 */
void Transmitter::stepReceive(unsigned int timeout)
{
    //Serial.println("Step receive");
    cout << "Step receive" << endl;
    this->listen(timeout);
}

/**
 * Receive step
 */
void Transmitter::stepReceive()
{
    this->listen(this->getReceiver()->getTimeout());
}

/**
 * Add packing directly to queue
 */
void Transmitter::add(Packing * packing)
{
    this->getQueue()->push_back(packing);
}

/**
 * Add packet to queue
 */
void Transmitter::add(Packet * packet, bool withConfirm)
{
    Packing * packing = new Packing(packet, withConfirm);

    this->add(packing);
}

/**
 * Add packet to queue (without confirm)
 */
void Transmitter::add(Packet * packet)
{
    this->add(packet, false);
}

/**
 * Listen packets and if receive, forward to action manager for process
 * and set timeout time to receive packet in millisecond
 *
 * @return bool True if packet received during listen process
 */
bool Transmitter::listen(unsigned int timeout)
{
    //Serial.println(F("Transmitter: Listen packet..."));

    // Confirm packet
    Packet * packetOk = NULL;

    Packet * response = NULL;
    int i = 0;

    bool last = false;
    // Waiting for response
    while (!last && this->getReceiver()->listen(timeout)) {
        i++;

        response = this->getReceiver()->getResponse();

        // Confirm packet in sended queue
        if (response->getCommand() == Command::OK) {
            this->confirm(response);
        } // Prepare success receiving response (not send yet)
        else {
            packetOk = this->generatePacketOK();
            packetOk->setTarget(response->getSourceIdentifier());
            packetOk->setDataUChar1(response->getId());
            packetOk->setDate(0);
            this->add(packetOk);
        }

        // Processing
        if (this->hasActionManager()) {
            this->actionManager->processPacket(response);
        }

        // Check flag last. If true, waiting for another response
        last = response->isLast();

        // Clean receiver response (no need this instance anymore)
        this->getReceiver()->cleanResponse();
    }

    //Serial.print(F("Transmitter: Stop listening. "));
    //Serial.print(i);
    //Serial.println(F(" packet received and processed."));

    // Clean response (no more used)
    this->getReceiver()->cleanResponse();

    return i > 0;
}

/**
 * Confirm packet if possible
 */
void Transmitter::confirm(Packet * response)
{
    if (response->getCommand() == Command::OK) {
        Packing *packing = NULL;
        // Search packet in sended queue
        unsigned int size = this->getQueueSended()->size();
        for (unsigned int i = 0; i < size; i++) {
            packing = this->getQueueSended()->at(i);

            // Remove packing if id match (confirmed)
            if (packing->isNeedConfirm() && packing->getPacket()->getId() == response->getDataUChar1()) {
                packing->setConfirmed(true);
                delete packing;
                packing = NULL;

                this->getQueueSended()->erase(this->getQueueSended()->begin() + i);
                i--;
                size--;
            }
        }
    }
}

/**
 * Generate OK packet for confirm response
 */
Packet * Transmitter::generatePacketOK()
{
    Packet *packet = new Packet();
    packet->setCommand(Command::OK);

    if (this->hasPropertySensorIdentifier()) {
        packet->setSourceIdentifier(this->propertySensorIdentifier->get());
    }
    if (this->hasPropertySensorType()) {
        packet->setSourceType(this->propertySensorType->get());
    }

    return packet;
}

/**
 * Get queue
 */
vector<Packing *>* Transmitter::getQueue()
{
    return this->queue;
}

/**
 * Send packet without checking for response confirmation
 */
bool Transmitter::send(Packing * packing)
{
    cout << "Send packing" << endl;
    packing->setSended(true);

    return this->getRequester()->send(packing);
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
 * Get max packet allowed in queue
 */
unsigned int Transmitter::getMaxPacketQueue()
{
    return maxPacketQueue;
}

/**
 * Set max packet allowed in queue
 */
void Transmitter::setMaxPacketQueue(unsigned int max)
{
    maxPacketQueue = max;
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

/**
 * Flag to indicate if receiver is defined
 */
bool Transmitter::hasReceiver()
{
    return this->receiver != NULL;
}

/**
 * Set requester
 */
void Transmitter::setRequester(Requester * requester)
{
    this->requester = requester;
}

/**
 * Set receiver
 */
void Transmitter::setReceiver(Receiver * receiver)
{
    this->receiver = receiver;

    // Apply properties if available
    if (this->hasPropertySensorType()) {
        this->receiver->setPropertySensorType(this->propertySensorType);
    }
    if (this->hasPropertySensorIdentifier()) {
        this->receiver->setPropertySensorIdentifier(this->propertySensorIdentifier);
    }
}
