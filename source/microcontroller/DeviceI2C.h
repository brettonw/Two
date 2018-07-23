#pragma once

#include    "Log.h"
#include    "File.h"

// headers needed for open, close, and ioctl
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#ifndef __APPLE__
#include <linux/i2c-dev.h>
#else
// shamelessly copied from a linux i2c-dev header, prefer the linux source if it is available
#define     I2C_SLAVE	0x0703	/* Use this slave address */
#define     I2C_TENBIT	0x0704	/* set to 0 for 7 bit addrs, != 0 for 10 bit */
#endif

#define DEVICE_I2C_ERROR                    -1
#define DEVICE_I2C_MAX_DEVICES              256
#define DEVICE_I2C_BUFFER_SIZE              256
#define DEVICE_I2C_FILE_PATH                "/dev/i2c-"

// references
// http://i2c.info/
// http://i2c.info/i2c-bus-specification
// https://www.nxp.com/docs/en/user-guide/UM10204.pdf
// https://www.kernel.org/doc/Documentation/i2c/dev-interface
// https://xanthium.in/serial-programming-tutorials
// https://www.cmrr.umn.edu/~strupp/serial.html
// XXX TODO: is it possible to open the device file descriptor and keep it open?
MAKE_PTR_TO(DeviceI2C) {
    protected:
        byte buffer[DEVICE_I2C_BUFFER_SIZE];
        uint length;
        int device;

        void store (byte b) {
            if ((length + 1) <= DEVICE_I2C_BUFFER_SIZE) {
                buffer[length++] = b;
            } else {
                throw RuntimeError ("DeviceI2C: out of buffer for writes");
            }
        }

        void init (uint address, Text busPath) {
            length = 0;

            // open the device, configure it to use 7 bit addresses (per i2c-dev.h), and set the
            // slave address to our requested I2C address
            if (((device = open (busPath, O_RDWR)) >= 0) && (ioctl(device, I2C_TENBIT, 0) >= 0) && (ioctl(device, I2C_SLAVE, address) >= 0)) {
                Log::info () << "Opened device " << busPath << " at address " << hex (address) << " (" << hex (device) << ")" << endl;
            } else {
                throw RuntimeError (Text ("DeviceI2C: can't open device address (") << hex (address) << ") at " << busPath);
            }
        }

        // for testing purposes
        DeviceI2C () { device = -1; length = 0; }

    public:
        DeviceI2C (uint address, Text busPath) {
            init (address, busPath);
        }

        DeviceI2C (uint address, uint busNumber = 0) {
            vector<Text> availableBusPaths = getAvailableBusPaths ();
            if (busNumber < availableBusPaths.size ()) {
                init (address, availableBusPaths[busNumber]);
            } else {
                throw RuntimeError (Text ("DeviceI2C: no I2C bus at ") << hex (busNumber));
            }
        }

        static vector<Text> getAvailableBusPaths () {
            vector<Text> availableBusPaths;
            for (uint i = 0; i < DEVICE_I2C_MAX_DEVICES; ++i) {
                auto devicePath  = Text (DEVICE_I2C_FILE_PATH) << i;
                File deviceFile (devicePath);
                if (deviceFile.getExists ()) {
                    availableBusPaths.push_back (devicePath);
                }
            }
            return availableBusPaths;
        }

        // XXX TODO: probably want a static constructor that takes the device address, and it tries
        // XXX TODO: to create a device that connects to that address across all available buses
        // XXX TODO: i don't want to be too clever, as I can't anticipate how this will be used, but
        // XXX TODO: i believe there must be some way to identify the devices available...
        ~DeviceI2C () {
            if (device >= 0) {
                close (device);
                Log::info () << "Closed device (" << hex (device) << ")" << endl;
            }
        }

        DeviceI2C* write (byte address, byte value) {
            Log::trace () << "DeviceI2C: " << "write (@" << hex (address) << ", " << hex (value) << ")" << endl;
            store (address);
            store (value);
            return this;
        }

        bool flush () {
            if (::write (device, buffer, length) == int (length)) {
                Log::trace () << "DeviceI2C: flushed " << length << " bytes" << endl;
                length = 0;
                return true;
            }
            Log::debug () << "DeviceI2C: flush (FAILED)" << endl;
            return false;
        }

        byte read (byte address) {
            // append the write command to set the target address to the write buffer, this will
            // have the nice side effect of finishing any pending writes.
            store (address);
            if ((flush ()) && (::read (device, &buffer[0], 1) == 1)) {
                Log::trace () << "DeviceI2C: " << "read (@" << hex (address) << ", got -> " << hex (buffer[0]) << ")" << endl;
                return buffer[0];
            }
            throw RuntimeError ("DeviceI2C: read failed");
        }

};
