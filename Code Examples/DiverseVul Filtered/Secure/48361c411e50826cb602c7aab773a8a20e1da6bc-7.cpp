SECURITY_STATUS ntlm_write_AuthenticateMessage(NTLM_CONTEXT* context, PSecBuffer buffer)
{
	wStream* s;
	size_t length;
	UINT32 PayloadBufferOffset;
	NTLM_AUTHENTICATE_MESSAGE* message;
	SSPI_CREDENTIALS* credentials = context->credentials;
	message = &context->AUTHENTICATE_MESSAGE;
	ZeroMemory(message, sizeof(NTLM_AUTHENTICATE_MESSAGE));
	s = Stream_New((BYTE*)buffer->pvBuffer, buffer->cbBuffer);

	if (!s)
		return SEC_E_INTERNAL_ERROR;

	if (context->NTLMv2)
	{
		message->NegotiateFlags |= NTLMSSP_NEGOTIATE_56;

		if (context->SendVersionInfo)
			message->NegotiateFlags |= NTLMSSP_NEGOTIATE_VERSION;
	}

	if (context->UseMIC)
		message->NegotiateFlags |= NTLMSSP_NEGOTIATE_TARGET_INFO;

	if (context->SendWorkstationName)
		message->NegotiateFlags |= NTLMSSP_NEGOTIATE_WORKSTATION_SUPPLIED;

	if (context->confidentiality)
		message->NegotiateFlags |= NTLMSSP_NEGOTIATE_SEAL;

	if (context->CHALLENGE_MESSAGE.NegotiateFlags & NTLMSSP_NEGOTIATE_KEY_EXCH)
		message->NegotiateFlags |= NTLMSSP_NEGOTIATE_KEY_EXCH;

	message->NegotiateFlags |= NTLMSSP_NEGOTIATE_128;
	message->NegotiateFlags |= NTLMSSP_NEGOTIATE_EXTENDED_SESSION_SECURITY;
	message->NegotiateFlags |= NTLMSSP_NEGOTIATE_ALWAYS_SIGN;
	message->NegotiateFlags |= NTLMSSP_NEGOTIATE_NTLM;
	message->NegotiateFlags |= NTLMSSP_NEGOTIATE_SIGN;
	message->NegotiateFlags |= NTLMSSP_REQUEST_TARGET;
	message->NegotiateFlags |= NTLMSSP_NEGOTIATE_UNICODE;

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_VERSION)
		ntlm_get_version_info(&(message->Version));

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_WORKSTATION_SUPPLIED)
	{
		message->Workstation.Len = context->Workstation.Length;
		message->Workstation.Buffer = (BYTE*)context->Workstation.Buffer;
	}

	if (credentials->identity.DomainLength > 0)
	{
		message->NegotiateFlags |= NTLMSSP_NEGOTIATE_DOMAIN_SUPPLIED;
		message->DomainName.Len = (UINT16)credentials->identity.DomainLength * 2;
		message->DomainName.Buffer = (BYTE*)credentials->identity.Domain;
	}

	message->UserName.Len = (UINT16)credentials->identity.UserLength * 2;
	message->UserName.Buffer = (BYTE*)credentials->identity.User;
	message->LmChallengeResponse.Len = (UINT16)context->LmChallengeResponse.cbBuffer;
	message->LmChallengeResponse.Buffer = (BYTE*)context->LmChallengeResponse.pvBuffer;
	message->NtChallengeResponse.Len = (UINT16)context->NtChallengeResponse.cbBuffer;
	message->NtChallengeResponse.Buffer = (BYTE*)context->NtChallengeResponse.pvBuffer;

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_KEY_EXCH)
	{
		message->EncryptedRandomSessionKey.Len = 16;
		message->EncryptedRandomSessionKey.Buffer = context->EncryptedRandomSessionKey;
	}

	PayloadBufferOffset = 64;

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_VERSION)
		PayloadBufferOffset += 8; /* Version (8 bytes) */

	if (context->UseMIC)
		PayloadBufferOffset += 16; /* Message Integrity Check (16 bytes) */

	message->DomainName.BufferOffset = PayloadBufferOffset;
	message->UserName.BufferOffset = message->DomainName.BufferOffset + message->DomainName.Len;
	message->Workstation.BufferOffset = message->UserName.BufferOffset + message->UserName.Len;
	message->LmChallengeResponse.BufferOffset =
	    message->Workstation.BufferOffset + message->Workstation.Len;
	message->NtChallengeResponse.BufferOffset =
	    message->LmChallengeResponse.BufferOffset + message->LmChallengeResponse.Len;
	message->EncryptedRandomSessionKey.BufferOffset =
	    message->NtChallengeResponse.BufferOffset + message->NtChallengeResponse.Len;
	ntlm_populate_message_header((NTLM_MESSAGE_HEADER*)message, MESSAGE_TYPE_AUTHENTICATE);
	ntlm_write_message_header(s, (NTLM_MESSAGE_HEADER*)message); /* Message Header (12 bytes) */
	ntlm_write_message_fields(
	    s, &(message->LmChallengeResponse)); /* LmChallengeResponseFields (8 bytes) */
	ntlm_write_message_fields(
	    s, &(message->NtChallengeResponse));               /* NtChallengeResponseFields (8 bytes) */
	ntlm_write_message_fields(s, &(message->DomainName));  /* DomainNameFields (8 bytes) */
	ntlm_write_message_fields(s, &(message->UserName));    /* UserNameFields (8 bytes) */
	ntlm_write_message_fields(s, &(message->Workstation)); /* WorkstationFields (8 bytes) */
	ntlm_write_message_fields(
	    s, &(message->EncryptedRandomSessionKey));   /* EncryptedRandomSessionKeyFields (8 bytes) */
	Stream_Write_UINT32(s, message->NegotiateFlags); /* NegotiateFlags (4 bytes) */

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_VERSION)
		ntlm_write_version_info(s, &(message->Version)); /* Version (8 bytes) */

	if (context->UseMIC)
	{
		context->MessageIntegrityCheckOffset = (UINT32)Stream_GetPosition(s);
		Stream_Zero(s, 16); /* Message Integrity Check (16 bytes) */
	}

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_DOMAIN_SUPPLIED)
		ntlm_write_message_fields_buffer(s, &(message->DomainName)); /* DomainName */

	ntlm_write_message_fields_buffer(s, &(message->UserName)); /* UserName */

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_WORKSTATION_SUPPLIED)
		ntlm_write_message_fields_buffer(s, &(message->Workstation)); /* Workstation */

	ntlm_write_message_fields_buffer(s, &(message->LmChallengeResponse)); /* LmChallengeResponse */
	ntlm_write_message_fields_buffer(s, &(message->NtChallengeResponse)); /* NtChallengeResponse */

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_KEY_EXCH)
		ntlm_write_message_fields_buffer(
		    s, &(message->EncryptedRandomSessionKey)); /* EncryptedRandomSessionKey */

	length = Stream_GetPosition(s);

	if (!sspi_SecBufferAlloc(&context->AuthenticateMessage, length))
	{
		Stream_Free(s, FALSE);
		return SEC_E_INTERNAL_ERROR;
	}

	CopyMemory(context->AuthenticateMessage.pvBuffer, Stream_Buffer(s), length);
	buffer->cbBuffer = length;

	if (context->UseMIC)
	{
		/* Message Integrity Check */
		ntlm_compute_message_integrity_check(context, message->MessageIntegrityCheck, 16);
		Stream_SetPosition(s, context->MessageIntegrityCheckOffset);
		Stream_Write(s, message->MessageIntegrityCheck, 16);
		Stream_SetPosition(s, length);
	}

#ifdef WITH_DEBUG_NTLM
	WLog_DBG(TAG, "AUTHENTICATE_MESSAGE (length = %d)", length);
	winpr_HexDump(TAG, WLOG_DEBUG, Stream_Buffer(s), length);
	ntlm_print_negotiate_flags(message->NegotiateFlags);

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_VERSION)
		ntlm_print_version_info(&(message->Version));

	if (context->AuthenticateTargetInfo.cbBuffer > 0)
	{
		WLog_DBG(TAG,
		         "AuthenticateTargetInfo (%" PRIu32 "):", context->AuthenticateTargetInfo.cbBuffer);
		ntlm_print_av_pair_list(context->AuthenticateTargetInfo.pvBuffer,
		                        context->AuthenticateTargetInfo.cbBuffer);
	}

	ntlm_print_message_fields(&(message->DomainName), "DomainName");
	ntlm_print_message_fields(&(message->UserName), "UserName");
	ntlm_print_message_fields(&(message->Workstation), "Workstation");
	ntlm_print_message_fields(&(message->LmChallengeResponse), "LmChallengeResponse");
	ntlm_print_message_fields(&(message->NtChallengeResponse), "NtChallengeResponse");
	ntlm_print_message_fields(&(message->EncryptedRandomSessionKey), "EncryptedRandomSessionKey");

	if (context->UseMIC)
	{
		WLog_DBG(TAG, "MessageIntegrityCheck (length = 16)");
		winpr_HexDump(TAG, WLOG_DEBUG, message->MessageIntegrityCheck, 16);
	}

#endif
	context->state = NTLM_STATE_FINAL;
	Stream_Free(s, FALSE);
	return SEC_I_COMPLETE_NEEDED;
}