TEST(UriSuite, TestQueryListPair) {
		testQueryListPairHelper("one+two+%26+three=%2B", "one two & three", "+");
		testQueryListPairHelper("one=two=three", "one", "two=three", "one=two%3Dthree");
		testQueryListPairHelper("one=two=three=four", "one", "two=three=four", "one=two%3Dthree%3Dfour");
}