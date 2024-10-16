TEST(ExpressionMergeObjects, MergingArrayWithDocumentShouldThrowException) {
    std::vector<Document> first = {Document({{"a", 1}}), Document({{"a", 2}})};
    auto second = Document({{"b", 2}});
    ASSERT_THROWS_CODE(
        evaluateExpression("$mergeObjects", {first, second}), AssertionException, 40400);
}