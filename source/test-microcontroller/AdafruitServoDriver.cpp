#include        "Test.h"
#include        "AdafruitServoDriver.h"
#include        "Servo.h"
#include        "TestDevice.h"
#include        "DeviceI2C.h"

TEST_CASE(TestAdafruitServoDriver) {
    //Log::Scope scope (Log::TRACE);
    PtrToTestDevice device = new TestDevice (ADAFRUIT_SERVER_DRIVER_DEFAULT_ADDRESS);
    device
        // PCA9685 constructor
        ->expect (0xfa, 0x00)
        ->expect (0xfb, 0x00)
        ->expect (0xfc, 0x00)
        ->expect (0xfd, 0x00)
        ->expect (0x01, 0x04)
        ->expect (0x00, 0x01)
        ->expect (0x00, 0x00)
        // which calls setPulseFrequency
        ->expect (0x00, (byte) 0x10)
        ->expect (0xfe, (byte) 0x79)
        ->expect (0x00, (byte) 0x00)
        ->expect (0x00, (byte) 0x80);

    PtrTo<AdafruitServoDriver<TestDevice> > driver = new AdafruitServoDriver<TestDevice> (device);

    // followed by a call to set channel pulse to set the stop position on servo 1
    device
        ->expect (0x06, (byte) 0x00)
        ->expect (0x07, (byte) 0x00)
        ->expect (0x08, (byte) 0x33)
        ->expect (0x09, (byte) 0x01);

    PtrTo<Servo<AdafruitServoDriver<TestDevice> > > servo1 = new Servo<AdafruitServoDriver<TestDevice> > (driver, ServoId::SERVO_01);

    // followed by a call to set channel pulse to set the stop position
    device
        ->expect (0x0a, (byte) 0x00)
        ->expect (0x0b, (byte) 0x00)
        ->expect (0x0c, (byte) 0xcd)
        ->expect (0x0d, (byte) 0x00);

    PtrTo<Servo<AdafruitServoDriver<TestDevice> > > servo2 = new Servo<AdafruitServoDriver<TestDevice> > (driver, ServoId::SERVO_02, 0.5, 1.5);

    // the PCA9685 class otherwise exposes no useful public interface to test
    TEST_ASSERTION(device->report ());
}

TEST_CASE(LiveTestAdafruitServoDriver) {
    try {
        PtrTo<AdafruitServoDriver<DeviceI2C> > driver = new AdafruitServoDriver<DeviceI2C> ();
        PtrTo<Servo<AdafruitServoDriver<DeviceI2C> > > servo8 = new Servo<AdafruitServoDriver<DeviceI2C> > (driver, ServoId::SERVO_08);
        Pause::milli (1000);
        servo8->setPosition (-0.5);
        Pause::milli (1000);
        servo8->setPosition (0.5);
        Pause::milli (1000);
        servo8->setPosition (-0.5);
        Pause::milli (1000);
        servo8->setPosition (0.5);
    } catch (...) {
    }
    TEST_X(true);
}
