TEST(UriSuite, TestCrashReport2418192) {
		// Testcase by Harvey Vrsalovic
		helperTestQueryString("http://svcs.cnn.com/weather/wrapper.jsp?&csiID=csi1", 1);
}