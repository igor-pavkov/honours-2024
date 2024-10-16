SECURITY_STATUS ntlm_write_ChallengeMessage(NTLM_CONTEXT* context, PSecBuffer buffer)
{
	wStream* s;
	size_t length;
	UINT32 PayloadOffset;
	NTLM_CHALLENGE_MESSAGE* message;
	message = &context->CHALLENGE_MESSAGE;
	ZeroMemory(message, sizeof(NTLM_CHALLENGE_MESSAGE));
	s = Stream_New((BYTE*)buffer->pvBuffer, buffer->cbBuffer);

	if (!s)
		return SEC_E_INTERNAL_ERROR;

	ntlm_get_version_info(&(message->Version)); /* Version */
	ntlm_generate_server_challenge(context);    /* Server Challenge */
	ntlm_generate_timestamp(context);           /* Timestamp */

	if (ntlm_construct_challenge_target_info(context) < 0) /* TargetInfo */
	{
		Stream_Free(s, FALSE);
		return SEC_E_INTERNAL_ERROR;
	}

	CopyMemory(message->ServerChallenge, context->ServerChallenge, 8); /* ServerChallenge */
	message->NegotiateFlags = context->NegotiateFlags;
	ntlm_populate_message_header((NTLM_MESSAGE_HEADER*)message, MESSAGE_TYPE_CHALLENGE);
	/* Message Header (12 bytes) */
	ntlm_write_message_header(s, (NTLM_MESSAGE_HEADER*)message);

	if (message->NegotiateFlags & NTLMSSP_REQUEST_TARGET)
	{
		message->TargetName.Len = (UINT16)context->TargetName.cbBuffer;
		message->TargetName.Buffer = (PBYTE)context->TargetName.pvBuffer;
	}

	message->NegotiateFlags |= NTLMSSP_NEGOTIATE_TARGET_INFO;

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_TARGET_INFO)
	{
		message->TargetInfo.Len = (UINT16)context->ChallengeTargetInfo.cbBuffer;
		message->TargetInfo.Buffer = (PBYTE)context->ChallengeTargetInfo.pvBuffer;
	}

	PayloadOffset = 48;

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_VERSION)
		PayloadOffset += 8;

	message->TargetName.BufferOffset = PayloadOffset;
	message->TargetInfo.BufferOffset = message->TargetName.BufferOffset + message->TargetName.Len;
	/* TargetNameFields (8 bytes) */
	ntlm_write_message_fields(s, &(message->TargetName));
	Stream_Write_UINT32(s, message->NegotiateFlags); /* NegotiateFlags (4 bytes) */
	Stream_Write(s, message->ServerChallenge, 8);    /* ServerChallenge (8 bytes) */
	Stream_Write(s, message->Reserved, 8);           /* Reserved (8 bytes), should be ignored */
	/* TargetInfoFields (8 bytes) */
	ntlm_write_message_fields(s, &(message->TargetInfo));

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_VERSION)
		ntlm_write_version_info(s, &(message->Version)); /* Version (8 bytes) */

	/* Payload (variable) */

	if (message->NegotiateFlags & NTLMSSP_REQUEST_TARGET)
		ntlm_write_message_fields_buffer(s, &(message->TargetName));

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_TARGET_INFO)
		ntlm_write_message_fields_buffer(s, &(message->TargetInfo));

	length = Stream_GetPosition(s);
	buffer->cbBuffer = length;

	if (!sspi_SecBufferAlloc(&context->ChallengeMessage, length))
	{
		Stream_Free(s, FALSE);
		return SEC_E_INTERNAL_ERROR;
	}

	CopyMemory(context->ChallengeMessage.pvBuffer, Stream_Buffer(s), length);
#ifdef WITH_DEBUG_NTLM
	WLog_DBG(TAG, "CHALLENGE_MESSAGE (length = %d)", length);
	winpr_HexDump(TAG, WLOG_DEBUG, context->ChallengeMessage.pvBuffer,
	              context->ChallengeMessage.cbBuffer);
	ntlm_print_negotiate_flags(message->NegotiateFlags);

	if (message->NegotiateFlags & NTLMSSP_NEGOTIATE_VERSION)
		ntlm_print_version_info(&(message->Version));

	ntlm_print_message_fields(&(message->TargetName), "TargetName");
	ntlm_print_message_fields(&(message->TargetInfo), "TargetInfo");
#endif
	context->state = NTLM_STATE_AUTHENTICATE;
	Stream_Free(s, FALSE);
	return SEC_I_CONTINUE_NEEDED;
}