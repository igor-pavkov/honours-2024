TEST(UriSuite, TestEquals) {
		testEqualsHelper("http://host");
		testEqualsHelper("http://host:123");
		testEqualsHelper("http://foo:bar@host:123");
		testEqualsHelper("http://foo:bar@host:123/");
		testEqualsHelper("http://foo:bar@host:123/path");
		testEqualsHelper("http://foo:bar@host:123/path?query");
		testEqualsHelper("http://foo:bar@host:123/path?query#fragment");

		testEqualsHelper("path");
		testEqualsHelper("/path");
		testEqualsHelper("/path/");
		testEqualsHelper("//path/");
		testEqualsHelper("//host");
		testEqualsHelper("//host:123");
}