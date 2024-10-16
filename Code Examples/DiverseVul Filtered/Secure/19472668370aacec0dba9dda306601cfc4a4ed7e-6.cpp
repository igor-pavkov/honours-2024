uint64_t GetSignatureTimeStamp(const char* path)
{
	char *mpath = NULL;
	BOOL r;
	HMODULE hm;
	HCERTSTORE hStore = NULL;
	HCRYPTMSG hMsg = NULL;
	DWORD dwSize, dwEncoding, dwContentType, dwFormatType;
	PCMSG_SIGNER_INFO pSignerInfo = NULL;
	DWORD dwSignerInfo = 0;
	wchar_t *szFileName;
	uint64_t timestamp = 0ULL, nested_timestamp;

	// If the path is NULL, get the signature of the current runtime
	if (path == NULL) {
		szFileName = calloc(MAX_PATH, sizeof(wchar_t));
		if (szFileName == NULL)
			goto out;
		hm = GetModuleHandle(NULL);
		if (hm == NULL) {
			uprintf("PKI: Could not get current executable handle: %s", WinPKIErrorString());
			goto out;
		}
		dwSize = GetModuleFileNameW(hm, szFileName, MAX_PATH);
		if ((dwSize == 0) || ((dwSize == MAX_PATH) && (GetLastError() == ERROR_INSUFFICIENT_BUFFER))) {
			uprintf("PKI: Could not get module filename: %s", WinPKIErrorString());
			goto out;
		}
		mpath = wchar_to_utf8(szFileName);
	} else {
		szFileName = utf8_to_wchar(path);
	}

	// Get message handle and store handle from the signed file.
	r = CryptQueryObject(CERT_QUERY_OBJECT_FILE, szFileName,
		CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED, CERT_QUERY_FORMAT_FLAG_BINARY,
		0, &dwEncoding, &dwContentType, &dwFormatType, &hStore, &hMsg, NULL);
	if (!r) {
		uprintf("PKI: Failed to get signature for '%s': %s", (path==NULL)?mpath:path, WinPKIErrorString());
		goto out;
	}

	// Get signer information size.
	r = CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, NULL, &dwSignerInfo);
	if (!r) {
		uprintf("PKI: Failed to get signer size: %s", WinPKIErrorString());
		goto out;
	}

	// Allocate memory for signer information.
	pSignerInfo = (PCMSG_SIGNER_INFO)calloc(dwSignerInfo, 1);
	if (!pSignerInfo) {
		uprintf("PKI: Could not allocate memory for signer information");
		goto out;
	}

	// Get Signer Information.
	r = CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, (PVOID)pSignerInfo, &dwSignerInfo);
	if (!r) {
		uprintf("PKI: Failed to get signer information: %s", WinPKIErrorString());
		goto out;
	}

	// Get the RFC 3161 timestamp
	timestamp = GetRFC3161TimeStamp(pSignerInfo);
	if (timestamp)
		uprintf("Note: '%s' has timestamp %s", (path==NULL)?mpath:path, TimestampToHumanReadable(timestamp));
	// Because we are currently using both SHA-1 and SHA-256 signatures, we are in the very specific
	// situation that Windows may say our executable passes Authenticode validation on Windows 7 or
	// later (which includes timestamp validation) even if the SHA-1 signature or timestamps have
	// been altered.
	// This means that, if we don't also check the nested SHA-256 signature timestamp, an attacker
	// could alter the SHA-1 one (which is the one we use by default for chronology validation) and
	// trick us into using an invalid timestamp value. To prevent this, we validate that, if we have
	// both a regular and nested timestamp, they are within 60 seconds of each other.
	nested_timestamp = GetNestedRFC3161TimeStamp(pSignerInfo);
	if (nested_timestamp)
		uprintf("Note: '%s' has nested timestamp %s", (path==NULL)?mpath:path, TimestampToHumanReadable(nested_timestamp));
	if ((timestamp != 0ULL) && (nested_timestamp != 0ULL)) {
		if (_abs64(nested_timestamp - timestamp) > 100) {
			uprintf("PKI: Signature timestamp and nested timestamp differ by more than a minute. "
				"This could indicate something very nasty...", timestamp, nested_timestamp);
			timestamp = 0ULL;
		}
	}

out:
	safe_free(mpath);
	safe_free(szFileName);
	safe_free(pSignerInfo);
	if (hStore != NULL)
		CertCloseStore(hStore, 0);
	if (hMsg != NULL)
		CryptMsgClose(hMsg);
	return timestamp;
}