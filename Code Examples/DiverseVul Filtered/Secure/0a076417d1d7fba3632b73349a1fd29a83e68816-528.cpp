TEST(ExpressionArrayToObjectTest, KVFormatWithDuplicates) {
    assertExpectedResults("$arrayToObject",
                          {{{Value(BSON_ARRAY(BSON("k"
                                                   << "hi"
                                                   << "v"
                                                   << 2)
                                              << BSON("k"
                                                      << "hi"
                                                      << "v"
                                                      << 3)))},
                            {Value(BSON("hi" << 3))}}});
}