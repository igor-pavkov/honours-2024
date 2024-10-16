SECURITY_STATUS ntlm_server_AuthenticateComplete(NTLM_CONTEXT* context)
{
	UINT32 flags = 0;
	size_t cbAvFlags;
	NTLM_AV_PAIR* AvFlags = NULL;
	NTLM_AUTHENTICATE_MESSAGE* message;
	BYTE messageIntegrityCheck[16];

	if (!context)
		return SEC_E_INVALID_PARAMETER;

	if (context->state != NTLM_STATE_COMPLETION)
		return SEC_E_OUT_OF_SEQUENCE;

	message = &context->AUTHENTICATE_MESSAGE;
	AvFlags = ntlm_av_pair_get(context->NTLMv2Response.Challenge.AvPairs,
	                           context->NTLMv2Response.Challenge.cbAvPairs, MsvAvFlags, &cbAvFlags);

	if (AvFlags)
		Data_Read_UINT32(ntlm_av_pair_get_value_pointer(AvFlags), flags);

	if (ntlm_compute_lm_v2_response(context) < 0) /* LmChallengeResponse */
		return SEC_E_INTERNAL_ERROR;

	if (ntlm_compute_ntlm_v2_response(context) < 0) /* NtChallengeResponse */
		return SEC_E_INTERNAL_ERROR;

	/* KeyExchangeKey */
	ntlm_generate_key_exchange_key(context);
	/* EncryptedRandomSessionKey */
	ntlm_decrypt_random_session_key(context);
	/* ExportedSessionKey */
	ntlm_generate_exported_session_key(context);

	if (flags & MSV_AV_FLAGS_MESSAGE_INTEGRITY_CHECK)
	{
		ZeroMemory(
		    &((PBYTE)context->AuthenticateMessage.pvBuffer)[context->MessageIntegrityCheckOffset],
		    16);
		ntlm_compute_message_integrity_check(context, messageIntegrityCheck,
		                                     sizeof(messageIntegrityCheck));
		CopyMemory(
		    &((PBYTE)context->AuthenticateMessage.pvBuffer)[context->MessageIntegrityCheckOffset],
		    message->MessageIntegrityCheck, 16);

		if (memcmp(messageIntegrityCheck, message->MessageIntegrityCheck, 16) != 0)
		{
			WLog_ERR(TAG, "Message Integrity Check (MIC) verification failed!");
#ifdef WITH_DEBUG_NTLM
			WLog_ERR(TAG, "Expected MIC:");
			winpr_HexDump(TAG, WLOG_ERROR, messageIntegrityCheck, sizeof(messageIntegrityCheck));
			WLog_ERR(TAG, "Actual MIC:");
			winpr_HexDump(TAG, WLOG_ERROR, message->MessageIntegrityCheck,
			              sizeof(message->MessageIntegrityCheck));
#endif
			return SEC_E_MESSAGE_ALTERED;
		}
	}
	else
	{
		/* no mic message was present

		   https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-nlmp/f9e6fbc4-a953-4f24-b229-ccdcc213b9ec
		   the mic is optional, as not supported in Windows NT, Windows 2000, Windows XP, and
		   Windows Server 2003 and, as it seems, in the NTLMv2 implementation of Qt5.

		   now check the NtProofString, to detect if the entered client password matches the
		   expected password.
		   */

#ifdef WITH_DEBUG_NTLM
		WLog_DBG(TAG, "No MIC present, using NtProofString for verification.");
#endif

		if (memcmp(context->NTLMv2Response.Response, context->NtProofString, 16) != 0)
		{
			WLog_ERR(TAG, "NtProofString verification failed!");
#ifdef WITH_DEBUG_NTLM
			WLog_ERR(TAG, "Expected NtProofString:");
			winpr_HexDump(TAG, WLOG_ERROR, context->NtProofString, sizeof(context->NtProofString));
			WLog_ERR(TAG, "Actual NtProofString:");
			winpr_HexDump(TAG, WLOG_ERROR, context->NTLMv2Response.Response,
			              sizeof(context->NTLMv2Response));
#endif
			return SEC_E_LOGON_DENIED;
		}
	}

	/* Generate signing keys */
	ntlm_generate_client_signing_key(context);
	ntlm_generate_server_signing_key(context);
	/* Generate sealing keys */
	ntlm_generate_client_sealing_key(context);
	ntlm_generate_server_sealing_key(context);
	/* Initialize RC4 seal state */
	ntlm_init_rc4_seal_states(context);
#ifdef WITH_DEBUG_NTLM
	WLog_DBG(TAG, "ClientChallenge");
	winpr_HexDump(TAG, WLOG_DEBUG, context->ClientChallenge, 8);
	WLog_DBG(TAG, "ServerChallenge");
	winpr_HexDump(TAG, WLOG_DEBUG, context->ServerChallenge, 8);
	WLog_DBG(TAG, "SessionBaseKey");
	winpr_HexDump(TAG, WLOG_DEBUG, context->SessionBaseKey, 16);
	WLog_DBG(TAG, "KeyExchangeKey");
	winpr_HexDump(TAG, WLOG_DEBUG, context->KeyExchangeKey, 16);
	WLog_DBG(TAG, "ExportedSessionKey");
	winpr_HexDump(TAG, WLOG_DEBUG, context->ExportedSessionKey, 16);
	WLog_DBG(TAG, "RandomSessionKey");
	winpr_HexDump(TAG, WLOG_DEBUG, context->RandomSessionKey, 16);
	WLog_DBG(TAG, "ClientSigningKey");
	winpr_HexDump(TAG, WLOG_DEBUG, context->ClientSigningKey, 16);
	WLog_DBG(TAG, "ClientSealingKey");
	winpr_HexDump(TAG, WLOG_DEBUG, context->ClientSealingKey, 16);
	WLog_DBG(TAG, "ServerSigningKey");
	winpr_HexDump(TAG, WLOG_DEBUG, context->ServerSigningKey, 16);
	WLog_DBG(TAG, "ServerSealingKey");
	winpr_HexDump(TAG, WLOG_DEBUG, context->ServerSealingKey, 16);
	WLog_DBG(TAG, "Timestamp");
	winpr_HexDump(TAG, WLOG_DEBUG, context->Timestamp, 8);
#endif
	context->state = NTLM_STATE_FINAL;
	ntlm_free_message_fields_buffer(&(message->DomainName));
	ntlm_free_message_fields_buffer(&(message->UserName));
	ntlm_free_message_fields_buffer(&(message->Workstation));
	ntlm_free_message_fields_buffer(&(message->LmChallengeResponse));
	ntlm_free_message_fields_buffer(&(message->NtChallengeResponse));
	ntlm_free_message_fields_buffer(&(message->EncryptedRandomSessionKey));
	return SEC_E_OK;
}