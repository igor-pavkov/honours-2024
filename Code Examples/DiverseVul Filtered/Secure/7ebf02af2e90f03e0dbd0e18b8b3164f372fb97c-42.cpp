void CSoundFile::KeyOff(ModChannel *pChn) const
{
	const bool bKeyOn = !pChn->dwFlags[CHN_KEYOFF];
	pChn->dwFlags.set(CHN_KEYOFF);
	if(pChn->pModInstrument != nullptr && !pChn->VolEnv.flags[ENV_ENABLED])
	{
		pChn->dwFlags.set(CHN_NOTEFADE);
	}
	if (!pChn->nLength) return;
	if (pChn->dwFlags[CHN_SUSTAINLOOP] && pChn->pModSample && bKeyOn)
	{
		const ModSample *pSmp = pChn->pModSample;
		if(pSmp->uFlags[CHN_LOOP])
		{
			if (pSmp->uFlags[CHN_PINGPONGLOOP])
				pChn->dwFlags.set(CHN_PINGPONGLOOP);
			else
				pChn->dwFlags.reset(CHN_PINGPONGLOOP | CHN_PINGPONGFLAG);
			pChn->dwFlags.set(CHN_LOOP);
			pChn->nLength = pSmp->nLength;
			pChn->nLoopStart = pSmp->nLoopStart;
			pChn->nLoopEnd = pSmp->nLoopEnd;
			if (pChn->nLength > pChn->nLoopEnd) pChn->nLength = pChn->nLoopEnd;
			if(pChn->position.GetUInt() > pChn->nLength)
			{
				// Test case: SusAfterLoop.it
				pChn->position.Set(pChn->position.GetInt() - pChn->nLength + pChn->nLoopStart);
			}
		} else
		{
			pChn->dwFlags.reset(CHN_LOOP | CHN_PINGPONGLOOP | CHN_PINGPONGFLAG);
			pChn->nLength = pSmp->nLength;
		}
	}

	if (pChn->pModInstrument)
	{
		const ModInstrument *pIns = pChn->pModInstrument;
		if((pIns->VolEnv.dwFlags[ENV_LOOP] || (GetType() & (MOD_TYPE_XM | MOD_TYPE_MT2 | MOD_TYPE_MDL))) && pIns->nFadeOut != 0)
		{
			pChn->dwFlags.set(CHN_NOTEFADE);
		}

		if (pIns->VolEnv.nReleaseNode != ENV_RELEASE_NODE_UNSET && pChn->VolEnv.nEnvValueAtReleaseJump == NOT_YET_RELEASED)
		{
			pChn->VolEnv.nEnvValueAtReleaseJump = pIns->VolEnv.GetValueFromPosition(pChn->VolEnv.nEnvPosition, 256);
			pChn->VolEnv.nEnvPosition = pIns->VolEnv[pIns->VolEnv.nReleaseNode].tick;
		}
	}
}