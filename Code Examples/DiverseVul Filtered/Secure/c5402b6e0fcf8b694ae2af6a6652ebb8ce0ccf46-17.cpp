static ContributionInfo **DestroyContributionThreadSet(
  ContributionInfo **contribution)
{
  register ssize_t
    i;

  assert(contribution != (ContributionInfo **) NULL);
  for (i=0; i < (ssize_t) GetMagickResourceLimit(ThreadResource); i++)
    if (contribution[i] != (ContributionInfo *) NULL)
      contribution[i]=(ContributionInfo *) RelinquishAlignedMemory(
        contribution[i]);
  contribution=(ContributionInfo **) RelinquishMagickMemory(contribution);
  return(contribution);
}