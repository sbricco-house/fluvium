#pragma once
/**
 * device contains all device abstraction.
 */
namespace device {
    enum PinState {
        HIGH  = 1, 
        LOW = 0
    };
    /**
     * root device structure.
     * [STANDARD]: a method starts with sense means that it can be evolved in a long computation and it is not idempotent.
     *             a method starts with get return a value instantly, used to describe some sensor/device properities. 
     */
    class Device {
        public:
            /*
            * Initialize device internal (low level, hardware thing..) state. It is different from object construction.
            * In some case, init method can be called more times.
            */
            virtual void init() = 0;
    };
}