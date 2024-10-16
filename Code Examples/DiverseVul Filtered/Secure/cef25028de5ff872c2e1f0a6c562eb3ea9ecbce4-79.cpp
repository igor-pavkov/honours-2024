TEST(UriSuite, TestPervertedQueryString) {
		helperTestQueryString("http://example.org/?&&=&&&=&&&&==&===&====", 5);
}