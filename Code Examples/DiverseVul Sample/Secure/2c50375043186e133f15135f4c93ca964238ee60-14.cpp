void reset_prefix_lifetimes(void)
{
	struct Interface *iface;
	struct AdvPrefix *prefix;
	char pfx_str[INET6_ADDRSTRLEN];


	flog(LOG_INFO, "Resetting prefix lifetimes");
	
	for (iface = IfaceList; iface; iface = iface->next) 
	{
		for (prefix = iface->AdvPrefixList; prefix;
							prefix = prefix->next) 
		{
			if (prefix->DecrementLifetimesFlag)
			{
				print_addr(&prefix->Prefix, pfx_str);
				dlog(LOG_DEBUG, 4, "%s/%u%%%s plft reset from %u to %u secs", pfx_str, prefix->PrefixLen, iface->Name, prefix->curr_preferredlft, prefix->AdvPreferredLifetime);
				dlog(LOG_DEBUG, 4, "%s/%u%%%s vlft reset from %u to %u secs", pfx_str, prefix->PrefixLen, iface->Name, prefix->curr_validlft, prefix->AdvValidLifetime);
				prefix->curr_validlft =
						prefix->AdvValidLifetime;
				prefix->curr_preferredlft =
						prefix->AdvPreferredLifetime;
			}
		}
		
	}

}