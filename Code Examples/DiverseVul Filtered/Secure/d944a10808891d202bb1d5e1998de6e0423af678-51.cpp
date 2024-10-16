void UDPSendPacketContext_t::Trim( int cbHdrOutSpaceRemaining )
{
	while ( m_cbTotalSize > cbHdrOutSpaceRemaining )
	{
		if ( !msg.has_stats() )
		{
			// Nothing left to clear!?  We shouldn't get here!
			AssertMsg( false, "Serialized stats message still won't fit, ever after clearing everything?" );
			m_cbTotalSize = 0;
			break;
		}

		if ( m_nStatsNeed & k_nSendStats_Instantanous_Ready )
		{
			msg.mutable_stats()->clear_instantaneous();
			m_nStatsNeed &= ~k_nSendStats_Instantanous_Ready;
		}
		else if ( m_nStatsNeed & k_nSendStats_Lifetime_Ready )
		{
			msg.mutable_stats()->clear_lifetime();
			m_nStatsNeed &= ~k_nSendStats_Lifetime_Ready;
		}
		else
		{
			AssertMsg( false, "We didn't reserve enough space for stats!" );
			if ( m_nStatsNeed & k_nSendStats_Instantanous_Due )
			{
				msg.mutable_stats()->clear_instantaneous();
				m_nStatsNeed &= ~k_nSendStats_Instantanous_Due;
			}
			else
			{
				m_nStatsNeed = 0;
			}
		}

		if ( m_nStatsNeed == 0 )
			msg.clear_stats();

		SlamFlagsAndCalcSize();
	}
}