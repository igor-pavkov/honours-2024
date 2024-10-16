TEST(UriParseSingleSuite, ErrorSyntaxParseErrorSetsErrorPos) {
	UriUriA uri;
	const char * errorPos;
	const char * const uriString = "abc{}def";

	EXPECT_EQ(uriParseSingleUriA(&uri, uriString, &errorPos),
			URI_ERROR_SYNTAX);
	EXPECT_EQ(errorPos, uriString + strlen("abc"));

	uriFreeUriMembersA(&uri);
}