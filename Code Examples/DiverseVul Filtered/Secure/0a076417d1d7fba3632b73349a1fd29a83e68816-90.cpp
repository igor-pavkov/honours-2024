TEST(ExpressionArrayToObjectTest, ListFormWithDuplicates) {
    assertExpectedResults("$arrayToObject",
                          {{{Value(BSON_ARRAY(BSON_ARRAY("key1" << 2) << BSON_ARRAY("key1" << 3)))},
                            {Value(BSON("key1" << 3))}}});
}