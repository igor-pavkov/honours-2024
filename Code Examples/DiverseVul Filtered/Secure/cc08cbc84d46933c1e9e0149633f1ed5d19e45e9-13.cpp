static dynamicPtr * newDynamic (int initialSize, void *data, int freeOKFlag)
{
	dynamicPtr *dp;
	dp = (dynamicPtr *) gdMalloc (sizeof (dynamicPtr));

	allocDynamic (dp, initialSize, data);

	dp->pos = 0;
	dp->freeOK = freeOKFlag;

	return dp;
}