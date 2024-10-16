void ItemStackMetadata::clear()
{
	Metadata::clear();
	updateToolCapabilities();
}