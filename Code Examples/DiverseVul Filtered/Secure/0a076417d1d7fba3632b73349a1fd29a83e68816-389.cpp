TEST(ExpressionTypeTest, WithDateValue) {
    Date_t dateVal = BSON("" << DATENOW).firstElement().Date();
    assertExpectedResults("$type", {{{Value(dateVal)}, Value("date"_sd)}});
}