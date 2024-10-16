TEST(ExpressionArrayToObjectTest, KVFormatSimple) {
    assertExpectedResults("$arrayToObject",
                          {{{Value(BSON_ARRAY(BSON("k"
                                                   << "key1"
                                                   << "v"
                                                   << 2)
                                              << BSON("k"
                                                      << "key2"
                                                      << "v"
                                                      << 3)))},
                            {Value(BSON("key1" << 2 << "key2" << 3))}}});
}