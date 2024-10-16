TEST(UriSuite, TestUnescaping) {
		const bool PLUS_TO_SPACE = true;
		const bool PLUS_DONT_TOUCH = false;


		// Proper
		ASSERT_TRUE(testUnescapingHelper(L"abc%20%41BC", L"abc ABC"));
		ASSERT_TRUE(testUnescapingHelper(L"%20", L" "));

		// Incomplete
		ASSERT_TRUE(testUnescapingHelper(L"%0", L"%0"));

		// Nonhex
		ASSERT_TRUE(testUnescapingHelper(L"%0g", L"%0g"));
		ASSERT_TRUE(testUnescapingHelper(L"%G0", L"%G0"));

		// No double decoding
		ASSERT_TRUE(testUnescapingHelper(L"%2520", L"%20"));

		// Decoding of '+'
		ASSERT_TRUE(testUnescapingHelper(L"abc+def", L"abc+def", PLUS_DONT_TOUCH));
		ASSERT_TRUE(testUnescapingHelper(L"abc+def", L"abc def", PLUS_TO_SPACE));

		// Line break conversion
		ASSERT_TRUE(testUnescapingHelper(L"%0d", L"\x0a", PLUS_DONT_TOUCH, URI_BR_TO_UNIX));
		ASSERT_TRUE(testUnescapingHelper(L"%0d", L"\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_TO_WINDOWS));
		ASSERT_TRUE(testUnescapingHelper(L"%0d", L"\x0d", PLUS_DONT_TOUCH, URI_BR_TO_MAC));
		ASSERT_TRUE(testUnescapingHelper(L"%0d", L"\x0d", PLUS_DONT_TOUCH, URI_BR_DONT_TOUCH));

		ASSERT_TRUE(testUnescapingHelper(L"%0d%0d", L"\x0a\x0a", PLUS_DONT_TOUCH, URI_BR_TO_UNIX));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0d", L"\x0d\x0a\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_TO_WINDOWS));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0d", L"\x0d\x0d", PLUS_DONT_TOUCH, URI_BR_TO_MAC));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0d", L"\x0d\x0d", PLUS_DONT_TOUCH, URI_BR_DONT_TOUCH));


		ASSERT_TRUE(testUnescapingHelper(L"%0a", L"\x0a", PLUS_DONT_TOUCH, URI_BR_TO_UNIX));
		ASSERT_TRUE(testUnescapingHelper(L"%0a", L"\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_TO_WINDOWS));
		ASSERT_TRUE(testUnescapingHelper(L"%0a", L"\x0d", PLUS_DONT_TOUCH, URI_BR_TO_MAC));
		ASSERT_TRUE(testUnescapingHelper(L"%0a", L"\x0a", PLUS_DONT_TOUCH, URI_BR_DONT_TOUCH));

		ASSERT_TRUE(testUnescapingHelper(L"%0a%0a", L"\x0a\x0a", PLUS_DONT_TOUCH, URI_BR_TO_UNIX));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0a", L"\x0d\x0a\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_TO_WINDOWS));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0a", L"\x0d\x0d", PLUS_DONT_TOUCH, URI_BR_TO_MAC));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0a", L"\x0a\x0a", PLUS_DONT_TOUCH, URI_BR_DONT_TOUCH));


		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a", L"\x0a", PLUS_DONT_TOUCH, URI_BR_TO_UNIX));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a", L"\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_TO_WINDOWS));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a", L"\x0d", PLUS_DONT_TOUCH, URI_BR_TO_MAC));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a", L"\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_DONT_TOUCH));

		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a%0a", L"\x0a\x0a", PLUS_DONT_TOUCH, URI_BR_TO_UNIX));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a%0a", L"\x0d\x0a\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_TO_WINDOWS));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a%0a", L"\x0d\x0d", PLUS_DONT_TOUCH, URI_BR_TO_MAC));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a%0a", L"\x0d\x0a\x0a", PLUS_DONT_TOUCH, URI_BR_DONT_TOUCH));

		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a%0d", L"\x0a\x0a", PLUS_DONT_TOUCH, URI_BR_TO_UNIX));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a%0d", L"\x0d\x0a\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_TO_WINDOWS));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a%0d", L"\x0d\x0d", PLUS_DONT_TOUCH, URI_BR_TO_MAC));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a%0d", L"\x0d\x0a\x0d", PLUS_DONT_TOUCH, URI_BR_DONT_TOUCH));

		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a%0d%0a", L"\x0a\x0a", PLUS_DONT_TOUCH, URI_BR_TO_UNIX));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a%0d%0a", L"\x0d\x0a\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_TO_WINDOWS));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a%0d%0a", L"\x0d\x0d", PLUS_DONT_TOUCH, URI_BR_TO_MAC));
		ASSERT_TRUE(testUnescapingHelper(L"%0d%0a%0d%0a", L"\x0d\x0a\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_DONT_TOUCH));


		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d", L"\x0a\x0a", PLUS_DONT_TOUCH, URI_BR_TO_UNIX));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d", L"\x0d\x0a\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_TO_WINDOWS));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d", L"\x0d\x0d", PLUS_DONT_TOUCH, URI_BR_TO_MAC));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d", L"\x0a\x0d", PLUS_DONT_TOUCH, URI_BR_DONT_TOUCH));

		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d%0a", L"\x0a\x0a", PLUS_DONT_TOUCH, URI_BR_TO_UNIX));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d%0a", L"\x0d\x0a\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_TO_WINDOWS));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d%0a", L"\x0d\x0d", PLUS_DONT_TOUCH, URI_BR_TO_MAC));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d%0a", L"\x0a\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_DONT_TOUCH));

		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d%0d", L"\x0a\x0a\x0a", PLUS_DONT_TOUCH, URI_BR_TO_UNIX));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d%0d", L"\x0d\x0a\x0d\x0a\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_TO_WINDOWS));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d%0d", L"\x0d\x0d\x0d", PLUS_DONT_TOUCH, URI_BR_TO_MAC));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d%0d", L"\x0a\x0d\x0d", PLUS_DONT_TOUCH, URI_BR_DONT_TOUCH));

		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d%0a%0d", L"\x0a\x0a\x0a", PLUS_DONT_TOUCH, URI_BR_TO_UNIX));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d%0a%0d", L"\x0d\x0a\x0d\x0a\x0d\x0a", PLUS_DONT_TOUCH, URI_BR_TO_WINDOWS));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d%0a%0d", L"\x0d\x0d\x0d", PLUS_DONT_TOUCH, URI_BR_TO_MAC));
		ASSERT_TRUE(testUnescapingHelper(L"%0a%0d%0a%0d", L"\x0a\x0d\x0a\x0d", PLUS_DONT_TOUCH, URI_BR_DONT_TOUCH));
}