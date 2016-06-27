//
// Created by Thibault PLET on 06/05/2016.
//

#ifndef COM_OSTERES_AUTOMATION_TRANSMISSION_RECEIVER_H
#define COM_OSTERES_AUTOMATION_TRANSMISSION_RECEIVER_H

#include <Arduino.h>
#include <SPI.h>
#include <RF24/RF24.h>
#include <com/osteres/automation/transmission/packet/Packet.h>

using com::osteres::automation::transmission::packet::Packet;
typedef uint8_t byte;

namespace com
{
    namespace osteres
    {
        namespace automation
        {
            namespace transmission
            {
                class Receiver {
                public:
                    /**
                     * Constructor
                     */
                    Receiver(RF24 * radio, uint64_t readingChannel, byte sensor, unsigned int timeout);

                    /**
                     * Destructor
                     */
                    ~Receiver();

                    /**
                     * Listen response for certain time
                     */
                    bool listen();

                    /**
                     * Get packet received
                     */
                    Packet * getResponse();

                    /**
                     * Flag to indicate if response has been received
                     */
                    bool hasResponse();

                    /**
                     * Clean response in memory
                     */
                    void cleanResponse();

                    /**
                     * Set timeout
                     */
                    void setTimeout(unsigned int timeout);

                    /**
                     * Get timeout
                     */
                    unsigned int getTimeout();

                    /**
                     * Set channel to read
                     */
                    void setReadingChannel(uint64_t readingChannel);

                protected:
                    /**
                     * Radio used for transmission
                     */
                    RF24 * radio = NULL;

                    /**
                     * Response packet
                     */
                    Packet * response = NULL;

                    /**
                     * Sensor target which can receive packet
                     */
                    byte sensor;

                    /**
                     * Timeout after this waiting for response has been stoped
                     */
                    unsigned int timeout;

                    /**
                     * Channel to read
                     */
                    uint64_t readingChannel;
                };
            }
        }
    }
}


#endif //COM_OSTERES_AUTOMATION_TRANSMISSION_RECEIVER_H
