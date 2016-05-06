//
// Created by Thibault PLET on 06/05/2016.
//

#ifndef COM_OSTERES_AUTOMATION_ACTION_ACTIONMANAGER_H
#define COM_OSTERES_AUTOMATION_ACTION_ACTIONMANAGER_H

#include <com/osteres/automation/transmission/packet/Packet.h>

using com::osteres::automation::transmission::packet::Packet;

namespace com
{
    namespace osteres
    {
        namespace automation
        {
            namespace action
            {
                class ActionManager
                {
                public:
                    /**
                     * Process packet
                     * Execute right action associated to this packet
                     */
                    void processPacket(Packet &packet);
                };
            }
        }
    }
}

#endif //COM_OSTERES_AUTOMATION_ACTION_ACTIONMANAGER_H