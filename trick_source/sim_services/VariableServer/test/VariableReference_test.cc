#include <gtest/gtest.h>
#include <iostream>
#include <iomanip>

#include "trick/MemoryManager.hh"
#include "trick/VariableReference.hh"


/*
 Test Fixture.
 */
class VariableReference_test : public ::testing::Test {
	protected:
	    Trick::MemoryManager *memmgr;
		VariableReference_test() { memmgr = new Trick::MemoryManager; }
		~VariableReference_test() { delete memmgr;   }
		void SetUp() {}
		void TearDown() {}
};

TEST_F(VariableReference_test, getName) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_int = 5;
    (void) memmgr->declare_extern_var(&test_int, "int test_int");
    Trick::VariableReference ref("test_int");

    // ACT

    // ASSERT
    EXPECT_EQ(strcmp(ref.getName(), "test_int"), 0);
}

TEST_F(VariableReference_test, validateAddress) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");
    
    float test_b = 5.0;
    (void) memmgr->declare_extern_var(&test_b, "float test_b");
    Trick::VariableReference ref_broken("test_b");
    memmgr->delete_var("test_b");

    float test_c = 5.0;
    (void) memmgr->declare_extern_var(&test_c, "float test_c");
    Trick::VariableReference ref_tagged("test_c");
    ref_tagged.tagAsInvalid();

    // ACT
    // ASSERT
    EXPECT_EQ(ref.validate(), true);
    EXPECT_EQ(ref_broken.validate(), false);
    // A reference already tagged as invalid is considered ok by this check
    EXPECT_EQ(ref_tagged.validate(), true);
}


TEST_F(VariableReference_test, stageValue_set) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");

    // ACT
    ref.stageValue();

    // ASSERT
    EXPECT_EQ(ref.isStaged(), true);
    EXPECT_EQ(ref.isWriteReady(), false);
}

TEST_F(VariableReference_test, prepareForWrite_set) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");

    // ACT
    ref.stageValue();
    ref.prepareForWrite();

    // ASSERT
    EXPECT_EQ(ref.isStaged(), false);
    EXPECT_EQ(ref.isWriteReady(), true);
}

TEST_F(VariableReference_test, prepareForWrite_fails_if_not_staged) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");

    // ACT
    ref.prepareForWrite();

    // ASSERT
    EXPECT_EQ(ref.isStaged(), false);
    EXPECT_EQ(ref.isWriteReady(), false);
}

TEST_F(VariableReference_test, writeValueAscii_fails_if_not_write_ready) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    // ASSERT
    EXPECT_EQ(ref.writeValueAscii(ss), -1);
}

TEST_F(VariableReference_test, writeValueAscii_int) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "5");
}

TEST_F(VariableReference_test, writeValueAscii_int_arr) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a[5] = {1, 2, 3, 4, 5};
    (void) memmgr->declare_extern_var(&test_a, "int test_a[5]");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "1,2,3,4,5");
}

TEST_F(VariableReference_test, writeValueAscii_double) {
    // ARRANGE
    // Create a variable to make a reference for
    double test_a = 867.309;
    (void) memmgr->declare_extern_var(&test_a, "double test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "867.309");
}

TEST_F(VariableReference_test, writeValueAscii_char) {
    // ARRANGE
    // Create a variable to make a reference for
    char test_a = 'j';
    (void) memmgr->declare_extern_var(&test_a, "char test_a");
    Trick::VariableReference ref_a("test_a");
    std::stringstream ssa;

    char test_b[7] = "jackie";
    (void) memmgr->declare_extern_var(&test_b, "char test_b[7]");
    Trick::VariableReference ref_b("test_b");
    std::stringstream ssb;

    // ACT
    ref_a.stageValue();
    ref_b.stageValue();

    ref_a.prepareForWrite();
    ref_b.prepareForWrite();

    ref_a.writeValueAscii(ssa);
    ref_b.writeValueAscii(ssb);


    // ASSERT
    EXPECT_EQ(ssa.str(), "j");
    EXPECT_EQ(ssb.str(), "jackie");
}


TEST_F(VariableReference_test, writeValueAscii_unsigned_char) {
    // ARRANGE
    // Create a variable to make a reference for
    unsigned char test_a = 'j';
    (void) memmgr->declare_extern_var(&test_a, "unsigned char test_a");
    Trick::VariableReference ref_a("test_a");
    std::stringstream ssa;

    unsigned char test_b[8] = "jackie\n";
    (void) memmgr->declare_extern_var(&test_b, "unsigned char test_b[8]");
    Trick::VariableReference ref_b("test_b");
    std::stringstream ssb;

    // ACT
    ref_a.stageValue();
    ref_b.stageValue();

    ref_a.prepareForWrite();
    ref_b.prepareForWrite();

    ref_a.writeValueAscii(ssa);
    ref_b.writeValueAscii(ssb);

    // ASSERT
    EXPECT_EQ(ssa.str(), "j");
    EXPECT_EQ(ssb.str(), "jackie\\n");
}


TEST_F(VariableReference_test, writeValueAscii_wide_char) {
    // ARRANGE
    // Create a variable to make a reference for
    wchar_t test_a = L'J';
    (void) memmgr->declare_extern_var(&test_a, "wchar_t test_a");
    Trick::VariableReference ref_a("test_a");
    std::stringstream ssa;

    wchar_t test_b[15] = L"jackiebutwider";
    (void) memmgr->declare_extern_var(&test_b, "wchar_t test_b[15]");
    Trick::VariableReference ref_b("test_b");
    std::stringstream ssb;

   // ACT
    ref_a.stageValue();
    ref_b.stageValue();

    ref_a.prepareForWrite();
    ref_b.prepareForWrite();

    ref_a.writeValueAscii(ssa);
    ref_b.writeValueAscii(ssb);

    // ASSERT
    // Original variable server behavior prints individual wchar as its ascii value
    EXPECT_EQ(ssa.str(), "74");
    EXPECT_EQ(ssb.str(), "jackiebutwider");
}

TEST_F(VariableReference_test, writeValueAscii_std_string) {
    // ARRANGE
    // Create a variable to make a reference for
    std::string test_a = "jackiebutstringy";
    (void) memmgr->declare_extern_var(&test_a, "std::string test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "jackiebutstringy");
}

TEST_F(VariableReference_test, writeValueAscii_escape_characters) {
    // ARRANGE
    // Create a variable to make a reference for
    std::string test_a = "\n\t\b\a\"\f\r\v";
    (void) memmgr->declare_extern_var(&test_a, "std::string test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "\\n\\t\\b\\a\"\\f\\n\\v");
}

TEST_F(VariableReference_test, writeValueAscii_boolean) {
    // ARRANGE
    // Create a variable to make a reference for
    bool test_a = true;
    (void) memmgr->declare_extern_var(&test_a, "bool test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "1");
}

TEST_F(VariableReference_test, writeValueAscii_short) {
    // ARRANGE
    // Create a variable to make a reference for
    short test_a = 255;
    (void) memmgr->declare_extern_var(&test_a, "short test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "255");
}