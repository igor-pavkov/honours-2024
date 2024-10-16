static void assertBinaryEqual(const BSONObj& expected, const BSONObj& actual) {
    ASSERT_BSONOBJ_EQ(expected, actual);
    ASSERT(expected.binaryEqual(actual));
}