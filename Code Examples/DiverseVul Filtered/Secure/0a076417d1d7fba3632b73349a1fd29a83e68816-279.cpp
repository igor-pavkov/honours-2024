TEST(ExpressionMergeObjects, MergingIntersectingObjectsShouldOverrideInOrderReceived) {
    auto first = Document({{"a", "oldValue"_sd}, {"b", 0}, {"c", 1}});
    auto second = Document({{"a", "newValue"_sd}});
    assertExpectedResults(
        "$mergeObjects", {{{first, second}, Document({{"a", "newValue"_sd}, {"b", 0}, {"c", 1}})}});
}