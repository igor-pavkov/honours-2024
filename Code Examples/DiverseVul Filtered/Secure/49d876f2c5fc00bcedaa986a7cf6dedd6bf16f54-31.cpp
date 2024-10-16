void __fastcall TCommandSet::CopyFrom(TCommandSet * Source)
{
  memmove(&CommandSet, Source->CommandSet, sizeof(CommandSet));
}