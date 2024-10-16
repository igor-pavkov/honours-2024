TEST(ExpressionMergeObjects, MergingDisjointObjectsShouldIncludeAllFields) {
    auto first = Document({{"a", 1}, {"b", 1}});
    auto second = Document({{"c", 1}});
    assertExpectedResults("$mergeObjects",
                          {{{first, second}, Document({{"a", 1}, {"b", 1}, {"c", 1}})}});
}