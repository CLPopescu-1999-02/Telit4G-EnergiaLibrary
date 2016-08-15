#include <stdio.h>

#include "gtest/gtest.h"
#include "LTEBase.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
/*
    HardwareSerial telit_serial;
    HardwareSerial debug_serial;
    telit_serial.begin(9600);
    debug_serial.begin(9600);

    initmillis();

    ///////////////////////////////////////////////////////////
    //                  Test LTE Base                        //
    ///////////////////////////////////////////////////////////

    LTEBase base(&telit_serial, &debug_serial);
    // Telit communication tests
    telit_serial.write("something");
    assert(base.sendATCommand(""));
    telit_serial.clear();

    telit_serial.write("something");
    assert(!base.sendATCommand("", 0));
    telit_serial.clear();

    telit_serial.write("something");
    assert(base.sendATCommand("", 1));
    telit_serial.clear();

    telit_serial.write("something");
    assert(base.sendATCommand("", 999999));
    telit_serial.clear();

    telit_serial.write("something");
    assert(base.sendATCommand("", -1)); // uses unsigned, so -1 should be 
    telit_serial.clear();

    fprintf(stderr, "All tests succeeded\n");
*/
}


///////////////////////////////////////////////////////////
//                  Test LTE Base                        //
///////////////////////////////////////////////////////////

class BaseTest : public testing::Test {
protected:
    BaseTest() {
        telit_serial.begin(9600);
        debug_serial.begin(9600);
        initmillis();
    }

    HardwareSerial telit_serial;
    HardwareSerial debug_serial;
};

TEST_F(BaseTest, TelitCommuncation) {
    LTEBase base(&telit_serial, &debug_serial);
    EXPECT_TRUE(base.sendATCommand(""));
}
