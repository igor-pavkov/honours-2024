static UINT wf_cliprdr_server_capabilities(CliprdrClientContext* context,
                                           const CLIPRDR_CAPABILITIES* capabilities)
{
	UINT32 index;
	CLIPRDR_CAPABILITY_SET* capabilitySet;
	wfClipboard* clipboard = (wfClipboard*)context->custom;

	if (!context || !capabilities)
		return ERROR_INTERNAL_ERROR;

	for (index = 0; index < capabilities->cCapabilitiesSets; index++)
	{
		capabilitySet = &(capabilities->capabilitySets[index]);

		if ((capabilitySet->capabilitySetType == CB_CAPSTYPE_GENERAL) &&
		    (capabilitySet->capabilitySetLength >= CB_CAPSTYPE_GENERAL_LEN))
		{
			CLIPRDR_GENERAL_CAPABILITY_SET* generalCapabilitySet =
			    (CLIPRDR_GENERAL_CAPABILITY_SET*)capabilitySet;
			clipboard->capabilities = generalCapabilitySet->generalFlags;
			break;
		}
	}

	return CHANNEL_RC_OK;
}