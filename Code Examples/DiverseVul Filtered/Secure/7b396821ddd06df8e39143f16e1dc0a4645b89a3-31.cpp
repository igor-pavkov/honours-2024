static bool checkreturn pb_decode_varint32(pb_istream_t *stream, uint32_t *dest)
{
    uint8_t byte;
    uint32_t result;
    
    if (!pb_readbyte(stream, &byte))
        return false;
    
    if ((byte & 0x80) == 0)
    {
        /* Quick case, 1 byte value */
        result = byte;
    }
    else
    {
        /* Multibyte case */
        uint8_t bitpos = 7;
        result = byte & 0x7F;
        
        do
        {
            if (bitpos >= 32)
                PB_RETURN_ERROR(stream, "varint overflow");
            
            if (!pb_readbyte(stream, &byte))
                return false;
            
            result |= (uint32_t)(byte & 0x7F) << bitpos;
            bitpos = (uint8_t)(bitpos + 7);
        } while (byte & 0x80);
   }
   
   *dest = result;
   return true;
}