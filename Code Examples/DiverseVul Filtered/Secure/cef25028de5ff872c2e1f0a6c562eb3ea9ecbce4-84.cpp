TEST(UriSuite, TestIpSixOverread) {
		UriUriA uri;
		const char * errorPos;

		// NOTE: This string is designed to not have a terminator
		char uriText[2 + 3 + 2 + 1 + 1];
		strncpy(uriText, "//[::44.1", sizeof(uriText));

		EXPECT_EQ(uriParseSingleUriExA(&uri, uriText,
				uriText + sizeof(uriText), &errorPos), URI_ERROR_SYNTAX);
		EXPECT_EQ(errorPos, uriText + sizeof(uriText));
}