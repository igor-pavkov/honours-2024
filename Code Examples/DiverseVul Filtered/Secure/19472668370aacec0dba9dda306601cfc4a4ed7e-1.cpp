char* GetSignatureName(const char* path, const char* country_code, BOOL bSilent)
{
	static char szSubjectName[128];
	char szCountry[3] = "__";
	char *p = NULL, *mpath = NULL;
	int i;
	BOOL r;
	HCERTSTORE hStore = NULL;
	HCRYPTMSG hMsg = NULL;
	PCCERT_CONTEXT pCertContext = NULL;
	DWORD dwSize, dwEncoding, dwContentType, dwFormatType;
	PCMSG_SIGNER_INFO pSignerInfo = NULL;
	DWORD dwSignerInfo = 0;
	CERT_INFO CertInfo = { 0 };
	SPROG_PUBLISHERINFO ProgPubInfo = { 0 };
	wchar_t *szFileName;

	// If the path is NULL, get the signature of the current runtime
	if (path == NULL) {
		szFileName = calloc(MAX_PATH, sizeof(wchar_t));
		if (szFileName == NULL)
			return NULL;
		dwSize = GetModuleFileNameW(NULL, szFileName, MAX_PATH);
		if ((dwSize == 0) || ((dwSize == MAX_PATH) && (GetLastError() == ERROR_INSUFFICIENT_BUFFER))) {
			uprintf("PKI: Could not get module filename: %s", WinPKIErrorString());
			goto out;
		}
		mpath = wchar_to_utf8(szFileName);
	} else {
		szFileName = utf8_to_wchar(path);
	}

	// Get message handle and store handle from the signed file.
	for (i = 0; i < 5; i++) {
		r = CryptQueryObject(CERT_QUERY_OBJECT_FILE, szFileName,
			CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED, CERT_QUERY_FORMAT_FLAG_BINARY,
			0, &dwEncoding, &dwContentType, &dwFormatType, &hStore, &hMsg, NULL);
		if (r)
			break;
		if (i == 0)
			uprintf("PKI: Failed to get signature for '%s': %s", (path==NULL)?mpath:path, WinPKIErrorString());
		if (path == NULL)
			break;
		uprintf("PKI: Retrying...");
		Sleep(2000);
	}
	if (!r)
		goto out;

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

	// Search for the signer certificate in the temporary certificate store.
	CertInfo.Issuer = pSignerInfo->Issuer;
	CertInfo.SerialNumber = pSignerInfo->SerialNumber;

	pCertContext = CertFindCertificateInStore(hStore, ENCODING, 0, CERT_FIND_SUBJECT_CERT, (PVOID)&CertInfo, NULL);
	if (!pCertContext) {
		uprintf("PKI: Failed to locate signer certificate in temporary store: %s", WinPKIErrorString());
		goto out;
	}

	// If a country code is provided, validate that the certificate we have is for the same country
	if (country_code != NULL) {
		dwSize = CertGetNameStringA(pCertContext, CERT_NAME_ATTR_TYPE, 0, szOID_COUNTRY_NAME,
			szCountry, sizeof(szCountry));
		if (dwSize < 2) {
			uprintf("PKI: Failed to get Country Code");
			goto out;
		}
		if (strcmpi(country_code, szCountry) != 0) {
			uprintf("PKI: Unexpected Country Code (Found '%s', expected '%s')", szCountry, country_code);
			goto out;
		}
	}

	// Isolate the signing certificate subject name
	dwSize = CertGetNameStringA(pCertContext, CERT_NAME_ATTR_TYPE, 0, szOID_COMMON_NAME,
		szSubjectName, sizeof(szSubjectName));
	if (dwSize <= 1) {
		uprintf("PKI: Failed to get Subject Name");
		goto out;
	}

	if (szCountry[0] == '_')
		suprintf("Binary executable is signed by '%s'", szSubjectName);
	else
		suprintf("Binary executable is signed by '%s' (%s)", szSubjectName, szCountry);
	p = szSubjectName;

out:
	safe_free(mpath);
	safe_free(szFileName);
	safe_free(ProgPubInfo.lpszProgramName);
	safe_free(ProgPubInfo.lpszPublisherLink);
	safe_free(ProgPubInfo.lpszMoreInfoLink);
	safe_free(pSignerInfo);
	if (pCertContext != NULL)
		CertFreeCertificateContext(pCertContext);
	if (hStore != NULL)
		CertCloseStore(hStore, 0);
	if (hMsg != NULL)
		CryptMsgClose(hMsg);
	return p;
}