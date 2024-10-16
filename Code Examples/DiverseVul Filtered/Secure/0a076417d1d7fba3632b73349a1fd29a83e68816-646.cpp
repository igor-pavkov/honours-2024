TEST(ExpressionMergeObjects, MergingWithSingleObjectShouldLeaveUnchanged) {
    assertExpectedResults("$mergeObjects", {{{}, {Document({})}}});

    auto doc = Document({{"a", 1}, {"b", 1}});
    assertExpectedResults("$mergeObjects", {{{doc}, doc}});
}