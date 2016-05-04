//
// Created by Thibault PLET on 04/05/2016.
//

#ifndef COM_OSTERES_AUTOMATION_APPLICATION_H
#define COM_OSTERES_AUTOMATION_APPLICATION_H


namespace com {
    namespace osteres {
        namespace automation {
            class Application {
            public:
                /**
                 * Process business work
                 */
                virtual void process();

                /**
                 * Setup application
                 */
                virtual void setup();
            };
        }
    }
}


#endif //COM_OSTERES_AUTOMATION_APPLICATION_H
