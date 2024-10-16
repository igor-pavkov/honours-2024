TEST(UriSuite, TestToString) {
		// Scheme
		ASSERT_TRUE(testToStringHelper(L"ftp://localhost/"));
		// UserInfo
		ASSERT_TRUE(testToStringHelper(L"http://user:pass@localhost/"));
		// IPv4
		ASSERT_TRUE(testToStringHelper(L"http://123.0.1.255/"));
		// IPv6
		ASSERT_TRUE(testToStringHelper(L"http://[abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd]/"));
		// IPvFuture
		ASSERT_TRUE(testToStringHelper(L"http://[vA.123456]/"));
		// Port
		ASSERT_TRUE(testToStringHelper(L"http://example.com:123/"));
		// Path
		ASSERT_TRUE(testToStringHelper(L"http://example.com"));
		ASSERT_TRUE(testToStringHelper(L"http://example.com/"));
		ASSERT_TRUE(testToStringHelper(L"http://example.com/abc/"));
		ASSERT_TRUE(testToStringHelper(L"http://example.com/abc/def"));
		ASSERT_TRUE(testToStringHelper(L"http://example.com/abc/def/"));
		ASSERT_TRUE(testToStringHelper(L"http://example.com//"));
		ASSERT_TRUE(testToStringHelper(L"http://example.com/./.."));
		// Query
		ASSERT_TRUE(testToStringHelper(L"http://example.com/?abc"));
		// Fragment
		ASSERT_TRUE(testToStringHelper(L"http://example.com/#abc"));
		ASSERT_TRUE(testToStringHelper(L"http://example.com/?def#abc"));

		// Relative
		ASSERT_TRUE(testToStringHelper(L"a"));
		ASSERT_TRUE(testToStringHelper(L"a/"));
		ASSERT_TRUE(testToStringHelper(L"/a"));
		ASSERT_TRUE(testToStringHelper(L"/a/"));
		ASSERT_TRUE(testToStringHelper(L"abc"));
		ASSERT_TRUE(testToStringHelper(L"abc/"));
		ASSERT_TRUE(testToStringHelper(L"/abc"));
		ASSERT_TRUE(testToStringHelper(L"/abc/"));
		ASSERT_TRUE(testToStringHelper(L"a/def"));
		ASSERT_TRUE(testToStringHelper(L"a/def/"));
		ASSERT_TRUE(testToStringHelper(L"/a/def"));
		ASSERT_TRUE(testToStringHelper(L"/a/def/"));
		ASSERT_TRUE(testToStringHelper(L"abc/def"));
		ASSERT_TRUE(testToStringHelper(L"abc/def/"));
		ASSERT_TRUE(testToStringHelper(L"/abc/def"));
		ASSERT_TRUE(testToStringHelper(L"/abc/def/"));
		ASSERT_TRUE(testToStringHelper(L"/"));
		ASSERT_TRUE(testToStringHelper(L"//a/"));
		ASSERT_TRUE(testToStringHelper(L"."));
		ASSERT_TRUE(testToStringHelper(L"./"));
		ASSERT_TRUE(testToStringHelper(L"/."));
		ASSERT_TRUE(testToStringHelper(L"/./"));
		ASSERT_TRUE(testToStringHelper(L""));
		ASSERT_TRUE(testToStringHelper(L"./abc/def"));
		ASSERT_TRUE(testToStringHelper(L"?query"));
		ASSERT_TRUE(testToStringHelper(L"#fragment"));
		ASSERT_TRUE(testToStringHelper(L"?query#fragment"));

		// Tests for bugs from the past
		ASSERT_TRUE(testToStringHelper(L"f:/.//g"));
}