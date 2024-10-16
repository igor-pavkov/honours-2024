static uint64_t GetNestedRFC3161TimeStamp(PCMSG_SIGNER_INFO pSignerInfo)
{
	BOOL r, found = FALSE;
	DWORD n, dwSize = 0;
	PCRYPT_CONTENT_INFO pNestedSignature = NULL;
	PCMSG_SIGNER_INFO pNestedSignerInfo = NULL;
	HCRYPTMSG hMsg = NULL;
	uint64_t ts = 0ULL;

	// Loop through unauthenticated attributes for szOID_NESTED_SIGNATURE OID
	for (n = 0; ; n++) {
		if (pNestedSignature != NULL) {
			LocalFree(pNestedSignature);
			pNestedSignature = NULL;
		}
		if (hMsg != NULL) {
			CryptMsgClose(hMsg);
			hMsg = NULL;
		}
		safe_free(pNestedSignerInfo);
		if (n >= pSignerInfo->UnauthAttrs.cAttr)
			break;
		if (lstrcmpA(pSignerInfo->UnauthAttrs.rgAttr[n].pszObjId, szOID_NESTED_SIGNATURE) == 0) {
			if (found) {
				uprintf("PKI: Multiple nested signatures found. This could indicate something very nasty...");
				return 0ULL;
			}
			found = TRUE;
			r = CryptDecodeObjectEx(PKCS_7_ASN_ENCODING, PKCS_CONTENT_INFO,
				pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].pbData,
				pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].cbData,
				CRYPT_DECODE_ALLOC_FLAG, NULL, (PVOID)&pNestedSignature, &dwSize);
			if (!r) {
				uprintf("PKI: Could not retrieve nested signature data: %s", WinPKIErrorString());
				continue;
			}

			hMsg = CryptMsgOpenToDecode(ENCODING, CMSG_DETACHED_FLAG, CMSG_SIGNED, (HCRYPTPROV)NULL, NULL, NULL);
			if (hMsg == NULL) {
				uprintf("PKI: Could not create nested signature message: %s", WinPKIErrorString());
				continue;
			}
			r = CryptMsgUpdate(hMsg, pNestedSignature->Content.pbData, pNestedSignature->Content.cbData, TRUE);
			if (!r) {
				uprintf("PKI: Could not update message: %s", WinPKIErrorString());
				continue;
			}
			// Get nested signer
			r = CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, NULL, &dwSize);
			if (!r) {
				uprintf("PKI: Failed to get nested signer size: %s", WinPKIErrorString());
				continue;
			}
			pNestedSignerInfo = (PCMSG_SIGNER_INFO)calloc(dwSize, 1);
			if (!pNestedSignerInfo) {
				uprintf("PKI: Could not allocate memory for nested signer");
				continue;
			}
			r = CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, (PVOID)pNestedSignerInfo, &dwSize);
			if (!r) {
				uprintf("PKI: Failed to get nested signer information: %s", WinPKIErrorString());
				continue;
			}
			ts = GetRFC3161TimeStamp(pNestedSignerInfo);
		}
	}
	return ts;
}