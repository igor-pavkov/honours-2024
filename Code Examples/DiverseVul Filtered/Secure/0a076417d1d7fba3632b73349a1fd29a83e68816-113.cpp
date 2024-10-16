TEST(ExpressionPowTest, LargeExponentValuesWithBaseOfNegativeOne) {
    assertExpectedResults("$pow",
                          {
                              {{Value(-1), Value(-1)}, Value(-1)},
                              {{Value(-1), Value(-2)}, Value(1)},
                              {{Value(-1), Value(-3)}, Value(-1)},

                              {{Value(-1LL), Value(0LL)}, Value(1LL)},
                              {{Value(-1LL), Value(-1LL)}, Value(-1LL)},
                              {{Value(-1LL), Value(-2LL)}, Value(1LL)},
                              {{Value(-1LL), Value(-3LL)}, Value(-1LL)},
                              {{Value(-1LL), Value(-4LL)}, Value(1LL)},
                              {{Value(-1LL), Value(-5LL)}, Value(-1LL)},

                              {{Value(-1LL), Value(-61LL)}, Value(-1LL)},
                              {{Value(-1LL), Value(61LL)}, Value(-1LL)},

                              {{Value(-1LL), Value(-62LL)}, Value(1LL)},
                              {{Value(-1LL), Value(62LL)}, Value(1LL)},

                              {{Value(-1LL), Value(-101LL)}, Value(-1LL)},
                              {{Value(-1LL), Value(-102LL)}, Value(1LL)},

                              // Use a value large enough that will make the test hang for a
                              // considerable amount of time if a loop is used to compute the
                              // answer.
                              {{Value(-1LL), Value(63234673905128LL)}, Value(1LL)},
                              {{Value(-1LL), Value(-63234673905128LL)}, Value(1LL)},

                              {{Value(-1LL), Value(63234673905127LL)}, Value(-1LL)},
                              {{Value(-1LL), Value(-63234673905127LL)}, Value(-1LL)},
                          });
}