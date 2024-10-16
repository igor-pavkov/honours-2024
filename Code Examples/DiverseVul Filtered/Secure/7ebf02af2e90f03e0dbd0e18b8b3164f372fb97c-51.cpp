	void RenderChannel(CHANNELINDEX channel, uint32 tickDuration, uint32 portaStart = uint32_max)
	{
		ModChannel &chn = state->Chn[channel];
		uint32 numTicks = chnSettings[channel].ticksToRender;
		if(numTicks == IGNORE_CHANNEL || numTicks == 0 || (!chn.IsSamplePlaying() && !chnSettings[channel].incChanged) || chn.pModSample == nullptr)
		{
			return;
		}

		const SmpLength sampleEnd = chn.dwFlags[CHN_LOOP] ? chn.nLoopEnd : chn.nLength;
		const SmpLength loopLength = chn.nLoopEnd - chn.nLoopStart;
		const bool itEnvMode = sndFile.m_playBehaviour[kITEnvelopePositionHandling];
		const bool updatePitchEnv = (chn.PitchEnv.flags & (ENV_ENABLED | ENV_FILTER)) == ENV_ENABLED;
		bool stopNote = false;

		SamplePosition inc = chn.increment * tickDuration;
		if(chn.dwFlags[CHN_PINGPONGFLAG]) inc.Negate();

		for(uint32 i = 0; i < numTicks; i++)
		{
			bool updateInc = (chn.PitchEnv.flags & (ENV_ENABLED | ENV_FILTER)) == ENV_ENABLED;
			if(i >= portaStart)
			{
				chn.isFirstTick = false;
				const ModCommand &p = *sndFile.Patterns[state->m_nPattern].GetpModCommand(state->m_nRow, channel);
				if(p.command == CMD_TONEPORTAMENTO) sndFile.TonePortamento(&chn, p.param);
				else if(p.command == CMD_TONEPORTAVOL) sndFile.TonePortamento(&chn, 0);
				if(p.volcmd == VOLCMD_TONEPORTAMENTO)
				{
					uint32 param = p.vol;
					if(sndFile.GetType() & (MOD_TYPE_IT | MOD_TYPE_MPT | MOD_TYPE_AMS | MOD_TYPE_AMS2 | MOD_TYPE_DMF | MOD_TYPE_DBM | MOD_TYPE_IMF | MOD_TYPE_PSM | MOD_TYPE_J2B | MOD_TYPE_ULT | MOD_TYPE_OKT | MOD_TYPE_MT2 | MOD_TYPE_MDL))
					{
						param = ImpulseTrackerPortaVolCmd[param & 0x0F];
					} else
					{
						// Close enough. Do not bother with idiosyncratic FT2 behaviour here.
						param <<= 4;
					}
					sndFile.TonePortamento(&chn, param);
				}
				updateInc = true;
			}

			int period = chn.nPeriod;
			if(itEnvMode) sndFile.IncrementEnvelopePositions(&chn);
			if(updatePitchEnv)
			{
				sndFile.ProcessPitchFilterEnvelope(&chn, period);
				updateInc = true;
			}
			if(!itEnvMode) sndFile.IncrementEnvelopePositions(&chn);
			int vol = 0;
			sndFile.ProcessInstrumentFade(&chn, vol);

			if(updateInc || chnSettings[channel].incChanged)
			{
				chn.increment = sndFile.GetChannelIncrement(&chn, period, 0);
				chnSettings[channel].incChanged = false;
				inc = chn.increment * tickDuration;
				if(chn.dwFlags[CHN_PINGPONGFLAG]) inc.Negate();
			}

			chn.position += inc;

			if(chn.position.GetUInt() >= sampleEnd)
			{
				if(chn.dwFlags[CHN_LOOP])
				{
					// We exceeded the sample loop, go back to loop start.
					if(chn.dwFlags[CHN_PINGPONGLOOP])
					{
						if(chn.position < SamplePosition(chn.nLoopStart, 0))
						{
							chn.position = SamplePosition(chn.nLoopStart + chn.nLoopStart, 0) - chn.position;
							chn.dwFlags.flip(CHN_PINGPONGFLAG);
							inc.Negate();
						}
						SmpLength posInt = chn.position.GetUInt() - chn.nLoopStart;
						SmpLength pingpongLength = loopLength * 2;
						if(sndFile.m_playBehaviour[kITPingPongMode]) pingpongLength--;
						posInt %= pingpongLength;
						bool forward = (posInt < loopLength);
						if(forward)
							chn.position.SetInt(chn.nLoopStart + posInt);
						else
							chn.position.SetInt(chn.nLoopEnd - (posInt - loopLength));
						if(forward == chn.dwFlags[CHN_PINGPONGFLAG])
						{
							chn.dwFlags.flip(CHN_PINGPONGFLAG);
							inc.Negate();
						}
					} else
					{
						SmpLength posInt = chn.position.GetUInt();
						if(posInt >= chn.nLoopEnd + loopLength)
						{
							const SmpLength overshoot = posInt - chn.nLoopEnd;
							posInt -= (overshoot / loopLength) * loopLength;
						}
						while(posInt >= chn.nLoopEnd)
						{
							posInt -= loopLength;
						}
						chn.position.SetInt(posInt);
					}
				} else
				{
					// Past sample end.
					stopNote = true;
					break;
				}
			}
		}

		if(stopNote)
		{
			chn.Stop();
			chn.nPortamentoDest = 0;
		}
		chnSettings[channel].ticksToRender = 0;
	}