void CSoundFile::PortamentoExtraFineMPT(ModChannel* pChn, int param)
{
	// This kinda behaves like regular fine portamento.
	// It changes the pitch by n finetune steps on the first tick.

	if(pChn->isFirstTick)
	{
		pChn->m_PortamentoFineSteps += param;
		pChn->m_CalculateFreq = true;
	}
}