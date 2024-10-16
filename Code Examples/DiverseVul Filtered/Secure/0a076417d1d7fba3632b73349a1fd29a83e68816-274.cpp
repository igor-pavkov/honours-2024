TEST(ExpressionMergeObjects, MergingNonObjectsShouldThrowException) {
    ASSERT_THROWS_CODE(
        evaluateExpression("$mergeObjects", {"invalidArg"_sd}), AssertionException, 40400);

    ASSERT_THROWS_CODE(
        evaluateExpression("$mergeObjects", {"invalidArg"_sd, Document({{"validArg", 1}})}),
        AssertionException,
        40400);

    ASSERT_THROWS_CODE(evaluateExpression("$mergeObjects", {1, Document({{"validArg", 1}})}),
                       AssertionException,
                       40400);
}