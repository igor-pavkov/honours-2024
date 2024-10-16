TEST(UriSuite, TestFilenameUriConversion) {
		const bool FOR_UNIX = true;
		const bool FOR_WINDOWS = false;
		testFilenameUriConversionHelper(L"/bin/bash", L"file:///bin/bash", FOR_UNIX);
		testFilenameUriConversionHelper(L"/bin/bash", L"file:/bin/bash", FOR_UNIX, L"file:///bin/bash");
		testFilenameUriConversionHelper(L"./configure", L"./configure", FOR_UNIX);

		testFilenameUriConversionHelper(L"E:\\Documents and Settings", L"file:///E:/Documents%20and%20Settings", FOR_WINDOWS);
		testFilenameUriConversionHelper(L"c:\\path\\to\\file.txt", L"file:c:/path/to/file.txt", FOR_WINDOWS, L"file:///c:/path/to/file.txt");

		testFilenameUriConversionHelper(L".\\Readme.txt", L"./Readme.txt", FOR_WINDOWS);

		testFilenameUriConversionHelper(L"index.htm", L"index.htm", FOR_WINDOWS);
		testFilenameUriConversionHelper(L"index.htm", L"index.htm", FOR_UNIX);

		testFilenameUriConversionHelper(L"abc def", L"abc%20def", FOR_WINDOWS);
		testFilenameUriConversionHelper(L"abc def", L"abc%20def", FOR_UNIX);

		testFilenameUriConversionHelper(L"\\\\Server01\\user\\docs\\Letter.txt", L"file://Server01/user/docs/Letter.txt", FOR_WINDOWS);
}