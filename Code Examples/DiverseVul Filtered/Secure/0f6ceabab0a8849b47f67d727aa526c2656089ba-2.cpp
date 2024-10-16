int challenge_response(YK_KEY *yk, int slot,
		       char *challenge, unsigned int len,
		       bool hmac, bool may_block, bool verbose,
		       char *response, unsigned int res_size, unsigned int *res_len)
{
	int yk_cmd;

  if(hmac == true) {
    *res_len = 20;
  } else {
    *res_len = 16;
  }
	if (res_size < *res_len) {
	  return 0;
  }

	memset(response, 0, res_size);

	if (verbose) {
		fprintf(stderr, "Sending %u bytes %s challenge to slot %i\n", len, (hmac == true)?"HMAC":"Yubico", slot);
		//_yk_hexdump(challenge, len);
	}

	switch(slot) {
	case 1:
		yk_cmd = (hmac == true) ? SLOT_CHAL_HMAC1 : SLOT_CHAL_OTP1;
		break;
	case 2:
		yk_cmd = (hmac == true) ? SLOT_CHAL_HMAC2 : SLOT_CHAL_OTP2;
		break;
	default:
		return 0;
	}

  if(! yk_challenge_response(yk, yk_cmd, may_block, len,
        (unsigned char*)challenge, res_size, (unsigned char*)response)) {
    return 0;
  }


	return 1;
}