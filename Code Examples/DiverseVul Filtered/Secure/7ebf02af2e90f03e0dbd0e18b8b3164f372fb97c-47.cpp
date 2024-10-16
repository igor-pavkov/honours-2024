void CSoundFile::PortamentoMPT(ModChannel* pChn, int param)
{
	//Behavior: Modifies portamento by param-steps on every tick.
	//Note that step meaning depends on tuning.

	pChn->m_PortamentoFineSteps += param;
	pChn->m_CalculateFreq = true;
}