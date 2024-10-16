SECURITY_STATUS ntlm_write_NegotiateMessage(NTLM_CONTEXT* context, PSecBuffer buffer)
{
	wStream* s;
	size_t length;
	NTLM_NEGOTIATE_MESSAGE* message;
	message = &context->NEGOTIATE_MESSAGE;
	ZeroMemory(message, sizeof(NTLM_NEGOTIATE_MESSAGE));
	s = Stream_New((BYTE*)buffer->pvBuffer, buffer->cbBuffer);

	if (!s)
		return SEC_E_INTERNAL_ERROR;

	ntlm_populate_message_header((NTLM_MESSAGE_HEADER*)message, MESSAGE_TYPE_NEGOTIATE);

	if (context->NTLMv2)
	{
		message->NegotiateFlags |= NTLMSSP_NEGOTIATE_56;
		message->NegotiateFlags |= NTLMSSP_NEGOTIATE_VERSION;
		message->NegotiateFlags |= NTLMSSP_NEGOTIATE_LM_KEY;
		message->NegotiateFlags |= NTLMSSP_NEGOTIATE_OEM;
	}

	message->NegotiateFlags |= NTLMSSP_NEGOTIATE_KEY_EXCH;
	message->NegotiateFlags |= NTLMSSP_NEGOTIATE_128;
	message->NegotiateFlags |= NTLMSSP_NEGOTIATE_EXTENDED_SESSION_SECURITY;
	message->NegotiateFlags |= NTLMSSP_NEGOTIATE_ALWAYS_SIGN;
	message->NegotiateFlags |= NTLMSSP_NEGOTIATE_NTLM;
	message->NegotiateFlags |= NTLMSSP_NEGOTIATE_SIGN;
	message->NegotiateFlags |= NTLMSSP_REQUEST_TARGET;
	message->NegotiateFlags |= NTLMSSP_NEGOTIATE_UNICODE;

	if (context->confidentiality)
		message->NegotiateFlags |= NTLMSSP_NEGOTIATE_SEAL;

	if (context->SendVersionInfo)
		message->NegotiateFlags |= NTLMSSP_NEGOTIATE_VERSION;

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_VERSION)
		ntlm_get_version_info(&(message->Version));

	context->NegotiateFlags = message->NegotiateFlags;
	/* Message Header (12 bytes) */
	ntlm_write_message_header(s, (NTLM_MESSAGE_HEADER*)message);
	Stream_Write_UINT32(s, message->NegotiateFlags); /* NegotiateFlags (4 bytes) */
	/* only set if NTLMSSP_NEGOTIATE_DOMAIN_SUPPLIED is set */
	/* DomainNameFields (8 bytes) */
	ntlm_write_message_fields(s, &(message->DomainName));
	/* only set if NTLMSSP_NEGOTIATE_WORKSTATION_SUPPLIED is set */
	/* WorkstationFields (8 bytes) */
	ntlm_write_message_fields(s, &(message->Workstation));

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_VERSION)
		ntlm_write_version_info(s, &(message->Version));

	length = Stream_GetPosition(s);
	buffer->cbBuffer = length;

	if (!sspi_SecBufferAlloc(&context->NegotiateMessage, length))
	{
		Stream_Free(s, FALSE);
		return SEC_E_INTERNAL_ERROR;
	}

	CopyMemory(context->NegotiateMessage.pvBuffer, buffer->pvBuffer, buffer->cbBuffer);
	context->NegotiateMessage.BufferType = buffer->BufferType;
#ifdef WITH_DEBUG_NTLM
	WLog_DBG(TAG, "NEGOTIATE_MESSAGE (length = %d)", length);
	winpr_HexDump(TAG, WLOG_DEBUG, Stream_Buffer(s), length);

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_VERSION)
		ntlm_print_version_info(&(message->Version));

#endif
	context->state = NTLM_STATE_CHALLENGE;
	Stream_Free(s, FALSE);
	return SEC_I_CONTINUE_NEEDED;
}