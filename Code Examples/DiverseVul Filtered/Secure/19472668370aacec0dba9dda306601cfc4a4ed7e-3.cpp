BOOL ValidateOpensslSignature(BYTE* pbBuffer, DWORD dwBufferLen, BYTE* pbSignature, DWORD dwSigLen)
{
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	HCRYPTKEY hPubKey;
	// We could load and convert an openssl PEM, but since we know what we need...
	RSA_2048_PUBKEY pbMyPubKey = {
		{ PUBLICKEYBLOB, CUR_BLOB_VERSION, 0, CALG_RSA_KEYX },
		// $ openssl genrsa -aes256 -out private.pem 2048
		// Generating RSA private key, 2048 bit long modulus
		// e is 65537 (0x010001)
		// => 0x010001 below. Also 0x31415352 = "RSA1"
		{ 0x31415352, sizeof(pbMyPubKey.Modulus) * 8, 0x010001 },
		{ 0 }	// Modulus is initialized below
	};
	USHORT dwMyPubKeyLen = sizeof(pbMyPubKey);
	BOOL r;
	BYTE t;
	int i, j;

	// Get a handle to the default PROV_RSA_AES provider (AES so we get SHA-256 support).
	// 2 passes in case we need to create a new container.
	r = CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_NEWKEYSET | CRYPT_VERIFYCONTEXT);
	if (!r) {
		uprintf("PKI: Could not create the default key container: %s", WinPKIErrorString());
		goto out;
	}

	// Reverse the modulus bytes from openssl (and also remove the extra unwanted 0x00)
	assert(sizeof(rsa_pubkey_modulus) >= sizeof(pbMyPubKey.Modulus));
	for (i = 0; i < sizeof(pbMyPubKey.Modulus); i++)
		pbMyPubKey.Modulus[i] = rsa_pubkey_modulus[sizeof(rsa_pubkey_modulus) -1 - i];

	// Import our RSA public key so that the MS API can use it
	r = CryptImportKey(hProv, (BYTE*)&pbMyPubKey.BlobHeader, dwMyPubKeyLen, 0, 0, &hPubKey);
	if (!r) {
		uprintf("PKI: Could not import public key: %s", WinPKIErrorString());
		goto out;
	}

	// Create the hash object.
	r = CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash);
	if (!r) {
		uprintf("PKI: Could not create empty hash: %s", WinPKIErrorString());
		goto out;
	}

	// Compute the cryptographic hash of the buffer.
	r = CryptHashData(hHash, pbBuffer, dwBufferLen, 0);
	if (!r) {
		uprintf("PKI: Could not hash data: %s", WinPKIErrorString());
		goto out;
	}

	// Reverse the signature bytes
	for (i = 0, j = dwSigLen - 1; i < j; i++, j--) {
		t = pbSignature[i];
		pbSignature[i] = pbSignature[j];
		pbSignature[j] = t;
	}

	// Now that we have all of the public key, hash and signature data in a
	// format that Microsoft can handle, we can call CryptVerifySignature().
	r = CryptVerifySignature(hHash, pbSignature, dwSigLen, hPubKey, NULL, 0);
	if (!r) {
		// If the signature is invalid, clear the buffer so that
		// we don't keep potentially nasty stuff in memory.
		memset(pbBuffer, 0, dwBufferLen);
		uprintf("Signature validation failed: %s", WinPKIErrorString());
	}

out:
	if (hHash)
		CryptDestroyHash(hHash);
	if (hProv)
		CryptReleaseContext(hProv, 0);
	return r;
}