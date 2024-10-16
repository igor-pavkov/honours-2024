TEST(ExpressionArrayToObjectTest, ListFormatSimple) {
    assertExpectedResults("$arrayToObject",
                          {{{Value(BSON_ARRAY(BSON_ARRAY("key1" << 2) << BSON_ARRAY("key2" << 3)))},
                            {Value(BSON("key1" << 2 << "key2" << 3))}}});
}