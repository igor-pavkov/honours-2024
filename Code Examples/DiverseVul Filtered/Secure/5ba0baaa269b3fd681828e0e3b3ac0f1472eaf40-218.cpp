static stbi__uint32 stbi__get32le(stbi__context *s)
{
   stbi__uint32 z = stbi__get16le(s);
   z += (stbi__uint32)stbi__get16le(s) << 16;
   return z;
}