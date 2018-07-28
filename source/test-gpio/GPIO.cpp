#include        "Test.h"
#include        "Pause.h"
#include        "GPIO.h"

TEST_CASE(TestGPIO) {
    Log::Scope scope (Log::DEBUG);
    try {
        GPIO    gpio;

        const char* functions[] = {
            "I", "O", "F", "E", "A", "B", "C", "D"
        };

        GPIO::Function function;
        Log& log = Log::debug () << "TestGPIO: " << "Raspberry Pi pinout" << endl << endl;
        for (uint piPin = 0; piPin < RASPBERRY_PI_PIN_COUNT;) {
            log << ((piPin < 10) ? " " : "");
            try {
                Pin pin = getPin (static_cast<PiPin> (piPin));
                function = gpio.getFunction (pin);
                log << (piPin + 1) << " -> " << functions[function] << " | ";
            } catch (RuntimeError& runtimeError) {
                log << (piPin + 1) << " -> " << "*" << " | ";
            }
            ++piPin;
            try {
                Pin pin = getPin (static_cast<PiPin> (piPin));
                function = gpio.getFunction (pin);
                log << functions[function] << " <- " << (piPin + 1) << endl;
            } catch (RuntimeError& runtimeError) {
                log << "*" << " <- " << (piPin + 1) << endl;
            }
            ++piPin;
        }
        log << endl;

    } catch (RuntimeError& runtimeError) {
        Log::exception (runtimeError);
    } catch (...) {
    }
    TEST_X(true);
}

TEST_CASE(TestBlinkPin6) {
    Log::Scope scope (Log::DEBUG);
    try {
        GPIO    gpio;
        gpio.setFunction (GPIO_06, GPIO::Function::OUTPUT);
        TEST_XY(gpio.getFunction (GPIO_06), GPIO::Function::OUTPUT);
        gpio.clear (GPIO_06);
        TEST_XY(gpio.get (GPIO_06), false);

        for (int i = 0; i < 10; ++i) {
            Log::debug () << "TestBlinkPin6: ON" << endl;
            gpio.toggle (GPIO_06);
            Pause::milli (500);
            Log::debug () << "TestBlinkPin6: OFF" << endl;
            gpio.toggle (GPIO_06);
            Pause::milli (500);
        }
    } catch (RuntimeError& runtimeError) {
        Log::exception (runtimeError);
    } catch (...) {
    }
    TEST_X(true);
}

TEST_CASE(TestPinMappings) {
    Log::Scope scope (Log::DEBUG);

    EXPECT_FAIL(getPin (RPI_01));


    TEST_XY(getPin (RPI_03), GPIO_02);
    TEST_XY(getPin (RPI_05), GPIO_03);
    TEST_XY(getPin (RPI_07), GPIO_04); TEST_XY(getPin (RPI_08), GPIO_14);
                                       TEST_XY(getPin (RPI_10), GPIO_15);
    TEST_XY(getPin (RPI_11), GPIO_17); TEST_XY(getPin (RPI_12), GPIO_18);
    TEST_XY(getPin (RPI_13), GPIO_27);
    TEST_XY(getPin (RPI_15), GPIO_22); TEST_XY(getPin (RPI_16), GPIO_23);
                                       TEST_XY(getPin (RPI_18), GPIO_24);
    TEST_XY(getPin (RPI_19), GPIO_10);
    TEST_XY(getPin (RPI_21), GPIO_09); TEST_XY(getPin (RPI_22), GPIO_25);
    TEST_XY(getPin (RPI_23), GPIO_11); TEST_XY(getPin (RPI_24), GPIO_08);
                                       TEST_XY(getPin (RPI_26), GPIO_07);
    TEST_XY(getPin (RPI_27), GPIO_00); TEST_XY(getPin (RPI_28), GPIO_01);
    TEST_XY(getPin (RPI_29), GPIO_05);
    TEST_XY(getPin (RPI_31), GPIO_06); TEST_XY(getPin (RPI_32), GPIO_12);
    TEST_XY(getPin (RPI_33), GPIO_13);
    TEST_XY(getPin (RPI_35), GPIO_19); TEST_XY(getPin (RPI_36), GPIO_16);
    TEST_XY(getPin (RPI_37), GPIO_26); TEST_XY(getPin (RPI_38), GPIO_20);
                                       TEST_XY(getPin (RPI_40), GPIO_21);

    EXPECT_FAIL(getPiPin (GPIO_28));

    TEST_XY(getPiPin (GPIO_00), RPI_27);
    TEST_XY(getPiPin (GPIO_01), RPI_28);
    TEST_XY(getPiPin (GPIO_02), RPI_03);
    TEST_XY(getPiPin (GPIO_03), RPI_05);
    TEST_XY(getPiPin (GPIO_04), RPI_07);
    TEST_XY(getPiPin (GPIO_05), RPI_29);
    TEST_XY(getPiPin (GPIO_06), RPI_31);
    TEST_XY(getPiPin (GPIO_07), RPI_26);
    TEST_XY(getPiPin (GPIO_08), RPI_24);
    TEST_XY(getPiPin (GPIO_09), RPI_21);
    TEST_XY(getPiPin (GPIO_10), RPI_19);
    TEST_XY(getPiPin (GPIO_11), RPI_23);
    TEST_XY(getPiPin (GPIO_12), RPI_32);
    TEST_XY(getPiPin (GPIO_13), RPI_33);
    TEST_XY(getPiPin (GPIO_14), RPI_08);
    TEST_XY(getPiPin (GPIO_15), RPI_10);
    TEST_XY(getPiPin (GPIO_16), RPI_36);
    TEST_XY(getPiPin (GPIO_17), RPI_11);
    TEST_XY(getPiPin (GPIO_18), RPI_12);
    TEST_XY(getPiPin (GPIO_19), RPI_35);
    TEST_XY(getPiPin (GPIO_20), RPI_38);
    TEST_XY(getPiPin (GPIO_21), RPI_40);
    TEST_XY(getPiPin (GPIO_22), RPI_15);
    TEST_XY(getPiPin (GPIO_23), RPI_16);
    TEST_XY(getPiPin (GPIO_24), RPI_18);
    TEST_XY(getPiPin (GPIO_25), RPI_22);
    TEST_XY(getPiPin (GPIO_26), RPI_37);
    TEST_XY(getPiPin (GPIO_27), RPI_13);


    TEST_XY(getPiPin (GPIO_00), RPI_27);
    TEST_XY(getPiPin (GPIO_06), RPI_31);
    TEST_XY(getPiPin (GPIO_21), RPI_40);
}
