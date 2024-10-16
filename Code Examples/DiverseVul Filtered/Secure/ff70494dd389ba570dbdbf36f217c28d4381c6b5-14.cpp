int array_contains(int *array, int length, int value)
{
    int i;
    for(i=0;i<length;i++)
        if(array[i] == value)
            return 1;

    return 0;
}