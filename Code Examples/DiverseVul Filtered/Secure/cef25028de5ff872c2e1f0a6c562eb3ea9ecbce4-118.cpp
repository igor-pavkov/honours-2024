TEST(UriParseSingleSuite, ErrorNullFirstDetected) {
	UriUriA uri;
	const char * errorPos;

	EXPECT_EQ(uriParseSingleUriExA(&uri, NULL, "notnull", &errorPos),
			URI_ERROR_NULL);
}