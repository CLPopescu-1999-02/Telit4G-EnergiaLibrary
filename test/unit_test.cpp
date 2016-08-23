#include <stdio.h>

#include "gtest/gtest.h"
#include "LTEBase.h"
#include "LTEHttp.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
	initmillis();
    return RUN_ALL_TESTS();
}

///////////////////////////////////////////////////////////
//                  Test LTE Base                        //
///////////////////////////////////////////////////////////

TEST(BaseTest, TelitCommunication) {
    // SETUP
    HardwareSerial telit_serial;
    HardwareSerial debug_serial;
    telit_serial.begin(9600);
    debug_serial.begin(9600);
    initmillis();
    LTEBase base(&telit_serial, &debug_serial);
    
    // TESTS
    // receiveData()
    ASSERT_FALSE(base.receiveData(0, 0));
    ASSERT_FALSE(base.receiveData(1, 0));
    ASSERT_FALSE(base.receiveData(0, 1));
    ASSERT_FALSE(base.receiveData(1, 1));
    ASSERT_TRUE((telit_serial.write("x") && base.receiveData(1, 1)));
    telit_serial.clear();
    ASSERT_FALSE((telit_serial.write("x") && base.receiveData(1, 0)));
    telit_serial.clear();
    ASSERT_FALSE((telit_serial.write("x") && base.receiveData(0, 1)));
    telit_serial.clear();

    // sendATCommand()
    EXPECT_FALSE(base.sendATCommand(""));
    EXPECT_TRUE(base.sendATCommand("text"));
    EXPECT_FALSE(base.sendATCommand("text", 0));
    telit_serial.clear();
    EXPECT_TRUE(base.sendATCommand("text", 1));
    EXPECT_FALSE(base.sendATCommand("text", 0, 1));
    EXPECT_FALSE(base.sendATCommand("text", 1, 0));
    telit_serial.clear();
    EXPECT_TRUE(base.sendATCommand("text", 1, 1));
    EXPECT_TRUE(base.sendATCommand("Some text"));
    EXPECT_TRUE(base.sendATCommand("Some text", 1, 1));
    EXPECT_FALSE(base.sendATCommand("Some text", 0, 0));
    EXPECT_TRUE(base.sendATCommand("text", 1, (uint32_t) 9999999999));
    EXPECT_TRUE(base.sendATCommand("text", (uint32_t) 9999999999, 1));

	// getCommandOK()
}

TEST(BaseTest, GetData) {
    // SETUP
    HardwareSerial telit_serial;
    HardwareSerial debug_serial;
    telit_serial.begin(9600);
    debug_serial.begin(9600);
    initmillis();
    LTEBase base(&telit_serial, &debug_serial);

    // TESTS
    // getData()
    EXPECT_EQ("", base.getData());
    base.sendATCommand("x");
    EXPECT_NE("x", base.getData());
    EXPECT_EQ("x\r\n", base.getData());
    EXPECT_EQ("x\r\n", base.getData()); // Data is not deleted by getData()
    base.sendATCommand("Some text");
    EXPECT_EQ("Some text\r\n", base.getData());  // However, it is erased
                                                    // by sendATCommand()
}

TEST(BaseTest, ParseData) {
    // SETUP
    HardwareSerial telit_serial;
    HardwareSerial debug_serial;
    telit_serial.begin(9600);
    debug_serial.begin(9600);
    initmillis();
    LTEBase base(&telit_serial, &debug_serial);

    // TESTS
    // parseFind()
    base.sendATCommand("Some text");
    EXPECT_TRUE(base.parseFind("text"));
    base.clearData();

    base.sendATCommand("text");
    EXPECT_TRUE(base.parseFind("\r\n"));
    base.clearData();

    EXPECT_FALSE(base.parseFind("\r\n"));
    EXPECT_FALSE(base.parseFind(""));

    base.sendATCommand("");
    EXPECT_FALSE(base.parseFind("")); // Empty string is always false
    base.clearData();

    // getParsedData()
	EXPECT_EQ("", base.getParsedData());
	base.sendATCommand("Some text goes here");
	EXPECT_TRUE(base.parseFind("text"));
	EXPECT_EQ(" goes here\r\n", base.getParsedData());

	base.clearData();
	base.sendATCommand("Some text goes here");
	EXPECT_TRUE(base.parseFind("\r\n"));
	EXPECT_EQ("", base.getParsedData());
	base.clearData();

	EXPECT_FALSE(base.parseFind("\r\n"));
	EXPECT_EQ("", base.getParsedData());
	base.clearData();
}



///////////////////////////////////////////////////////////
//                  Test LTE HTTP                        //
///////////////////////////////////////////////////////////
//
TEST(HttpTest, SetIP) {
    // SETUP
    HardwareSerial telit_serial;
    HardwareSerial debug_serial;
    telit_serial.begin(9600);
    debug_serial.begin(9600);
    initmillis();
    LTEHttp http(&telit_serial, &debug_serial);

    // TESTS
    // setAddress()
    
}
