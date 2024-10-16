TEST(UriSuite, TestHostTextTerminationIssue15) {
		UriParserStateA state;
		UriUriA uri;
		state.uri = &uri;

		// Empty host and port
		const char * const emptyHostWithPortUri = "//:123";
		ASSERT_TRUE(URI_SUCCESS == uriParseUriA(&state, emptyHostWithPortUri));
		ASSERT_TRUE(uri.hostText.first == emptyHostWithPortUri + strlen("//"));
		ASSERT_TRUE(uri.hostText.afterLast == uri.hostText.first + 0);
		ASSERT_TRUE(uri.portText.first == emptyHostWithPortUri
															+ strlen("//:"));
		ASSERT_TRUE(uri.portText.afterLast == uri.portText.first
															+ strlen("123"));
		uriFreeUriMembersA(&uri);

		// Non-empty host and port
		const char * const hostWithPortUri = "//h:123";
		ASSERT_TRUE(URI_SUCCESS == uriParseUriA(&state, hostWithPortUri));
		ASSERT_TRUE(uri.hostText.first == hostWithPortUri + strlen("//"));
		ASSERT_TRUE(uri.hostText.afterLast == uri.hostText.first
															+ strlen("h"));
		ASSERT_TRUE(uri.portText.first == hostWithPortUri + strlen("//h:"));
		ASSERT_TRUE(uri.portText.afterLast == uri.portText.first
															+ strlen("123"));
		uriFreeUriMembersA(&uri);

		// Empty host, empty user info
		const char * const emptyHostEmptyUserInfoUri = "//@";
		ASSERT_TRUE(URI_SUCCESS == uriParseUriA(&state,
												emptyHostEmptyUserInfoUri));
		ASSERT_TRUE(uri.userInfo.first == emptyHostEmptyUserInfoUri
															+ strlen("//"));
		ASSERT_TRUE(uri.userInfo.afterLast == uri.userInfo.first + 0);
		ASSERT_TRUE(uri.hostText.first == emptyHostEmptyUserInfoUri
															+ strlen("//@"));
		ASSERT_TRUE(uri.hostText.afterLast == uri.hostText.first + 0);
		uriFreeUriMembersA(&uri);

		// Non-empty host, empty user info
		const char * const hostEmptyUserInfoUri = "//@h";
		ASSERT_TRUE(URI_SUCCESS == uriParseUriA(&state, hostEmptyUserInfoUri));
		ASSERT_TRUE(uri.userInfo.first == hostEmptyUserInfoUri + strlen("//"));
		ASSERT_TRUE(uri.userInfo.afterLast == uri.userInfo.first + 0);
		ASSERT_TRUE(uri.hostText.first == hostEmptyUserInfoUri
															+ strlen("//@"));
		ASSERT_TRUE(uri.hostText.afterLast == uri.hostText.first
															+ strlen("h"));
		uriFreeUriMembersA(&uri);

		// Empty host, non-empty user info
		const char * const emptyHostWithUserInfoUri = "//:@";
		ASSERT_TRUE(URI_SUCCESS == uriParseUriA(&state,
												emptyHostWithUserInfoUri));
		ASSERT_TRUE(uri.userInfo.first == emptyHostWithUserInfoUri
															+ strlen("//"));
		ASSERT_TRUE(uri.userInfo.afterLast == uri.userInfo.first + 1);
		ASSERT_TRUE(uri.hostText.first == emptyHostWithUserInfoUri
															+ strlen("//:@"));
		ASSERT_TRUE(uri.hostText.afterLast == uri.hostText.first + 0);
		uriFreeUriMembersA(&uri);

		// Exact case from issue #15
		const char * const issue15Uri = "//:%aa@";
		ASSERT_TRUE(URI_SUCCESS == uriParseUriA(&state, issue15Uri));
		ASSERT_TRUE(uri.userInfo.first == issue15Uri + strlen("//"));
		ASSERT_TRUE(uri.userInfo.afterLast == uri.userInfo.first
															+ strlen(":%aa"));
		ASSERT_TRUE(uri.hostText.first == issue15Uri + strlen("//:%aa@"));
		ASSERT_TRUE(uri.hostText.afterLast == uri.hostText.first + 0);
		uriFreeUriMembersA(&uri);
}