static ContributionInfo **AcquireContributionThreadSet(const size_t count)
{
  register ssize_t
    i;

  ContributionInfo
    **contribution;

  size_t
    number_threads;

  number_threads=(size_t) GetMagickResourceLimit(ThreadResource);
  contribution=(ContributionInfo **) AcquireQuantumMemory(number_threads,
    sizeof(*contribution));
  if (contribution == (ContributionInfo **) NULL)
    return((ContributionInfo **) NULL);
  (void) ResetMagickMemory(contribution,0,number_threads*sizeof(*contribution));
  for (i=0; i < (ssize_t) number_threads; i++)
  {
    contribution[i]=(ContributionInfo *) MagickAssumeAligned(
      AcquireAlignedMemory(count,sizeof(**contribution)));
    if (contribution[i] == (ContributionInfo *) NULL)
      return(DestroyContributionThreadSet(contribution));
  }
  return(contribution);
}