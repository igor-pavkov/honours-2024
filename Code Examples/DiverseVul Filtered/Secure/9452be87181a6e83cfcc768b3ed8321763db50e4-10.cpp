static inline size_t parse_cipher_suite(uint8_t *cipher_suite_data,
                                        size_t data_len,
                                        uint32_t *iana,
                                        uint8_t *auth_alg,
                                        uint8_t *integrity_alg,
                                        uint8_t *crypt_alg,
                                        enum cipher_suite_ids *cipher_suite_id)
{
	size_t size = 0;
	const char *incomplete = "Incomplete data record in cipher suite data";

	if (*cipher_suite_data == STANDARD_CIPHER_SUITE) {
		struct std_cipher_suite_record_t *record =
			(struct std_cipher_suite_record_t*)cipher_suite_data;

		/* Verify that we have at least a full record left; id + 3 algs */
		if (data_len < sizeof(*record)) {
			lprintf(LOG_INFO, "%s", incomplete);
			goto out;
		}

		/* IANA code remains default (0) */
		*cipher_suite_id = record->cipher_suite_id;
		*auth_alg = CIPHER_ALG_MASK & record->auth_alg;
		*integrity_alg = CIPHER_ALG_MASK & record->integrity_alg;
		*crypt_alg = CIPHER_ALG_MASK & record->crypt_alg;
		size = sizeof(*record);
	} else if (*cipher_suite_data == OEM_CIPHER_SUITE) {
		/* OEM record type */
		struct oem_cipher_suite_record_t *record =
			(struct oem_cipher_suite_record_t*)cipher_suite_data;
		/* Verify that we have at least a full record left
		 * id + iana + 3 algs
		 */
		if (data_len < sizeof(*record)) {
			lprintf(LOG_INFO, "%s", incomplete);
			goto out;
		}

		/* Grab the IANA */
		*iana = ipmi24toh(record->iana);
		*cipher_suite_id = record->cipher_suite_id;
		*auth_alg = CIPHER_ALG_MASK & record->auth_alg;
		*integrity_alg = CIPHER_ALG_MASK & record->integrity_alg;
		*crypt_alg = CIPHER_ALG_MASK & record->crypt_alg;
		size = sizeof(*record);
	} else {
			lprintf(LOG_INFO, "Bad start of record byte in cipher suite data "
			                  "(value %x)", *cipher_suite_data);
	}

out:
	return size;
}