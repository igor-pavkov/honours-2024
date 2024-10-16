static inline mp_ubyte safeRead(const mp_ubyte* buffer, mp_uint32& index, mp_uint32 size, mp_ubyte errorValue = 0)
{
	if (index < size)
	{
		return buffer[index++];
	}
	
	index++;
	return errorValue;
}