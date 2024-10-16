int udf_write_fi(struct inode *inode, struct fileIdentDesc *cfi,
		 struct fileIdentDesc *sfi, struct udf_fileident_bh *fibh,
		 uint8_t *impuse, uint8_t *fileident)
{
	uint16_t crclen = fibh->eoffset - fibh->soffset - sizeof(struct tag);
	uint16_t crc;
	int offset;
	uint16_t liu = le16_to_cpu(cfi->lengthOfImpUse);
	uint8_t lfi = cfi->lengthFileIdent;
	int padlen = fibh->eoffset - fibh->soffset - liu - lfi -
		sizeof(struct fileIdentDesc);
	int adinicb = 0;

	if (UDF_I(inode)->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
		adinicb = 1;

	offset = fibh->soffset + sizeof(struct fileIdentDesc);

	if (impuse) {
		if (adinicb || (offset + liu < 0)) {
			memcpy((uint8_t *)sfi->impUse, impuse, liu);
		} else if (offset >= 0) {
			memcpy(fibh->ebh->b_data + offset, impuse, liu);
		} else {
			memcpy((uint8_t *)sfi->impUse, impuse, -offset);
			memcpy(fibh->ebh->b_data, impuse - offset,
				liu + offset);
		}
	}

	offset += liu;

	if (fileident) {
		if (adinicb || (offset + lfi < 0)) {
			memcpy((uint8_t *)sfi->fileIdent + liu, fileident, lfi);
		} else if (offset >= 0) {
			memcpy(fibh->ebh->b_data + offset, fileident, lfi);
		} else {
			memcpy((uint8_t *)sfi->fileIdent + liu, fileident,
				-offset);
			memcpy(fibh->ebh->b_data, fileident - offset,
				lfi + offset);
		}
	}

	offset += lfi;

	if (adinicb || (offset + padlen < 0)) {
		memset((uint8_t *)sfi->padding + liu + lfi, 0x00, padlen);
	} else if (offset >= 0) {
		memset(fibh->ebh->b_data + offset, 0x00, padlen);
	} else {
		memset((uint8_t *)sfi->padding + liu + lfi, 0x00, -offset);
		memset(fibh->ebh->b_data, 0x00, padlen + offset);
	}

	crc = crc_itu_t(0, (uint8_t *)cfi + sizeof(struct tag),
		      sizeof(struct fileIdentDesc) - sizeof(struct tag));

	if (fibh->sbh == fibh->ebh) {
		crc = crc_itu_t(crc, (uint8_t *)sfi->impUse,
			      crclen + sizeof(struct tag) -
			      sizeof(struct fileIdentDesc));
	} else if (sizeof(struct fileIdentDesc) >= -fibh->soffset) {
		crc = crc_itu_t(crc, fibh->ebh->b_data +
					sizeof(struct fileIdentDesc) +
					fibh->soffset,
			      crclen + sizeof(struct tag) -
					sizeof(struct fileIdentDesc));
	} else {
		crc = crc_itu_t(crc, (uint8_t *)sfi->impUse,
			      -fibh->soffset - sizeof(struct fileIdentDesc));
		crc = crc_itu_t(crc, fibh->ebh->b_data, fibh->eoffset);
	}

	cfi->descTag.descCRC = cpu_to_le16(crc);
	cfi->descTag.descCRCLength = cpu_to_le16(crclen);
	cfi->descTag.tagChecksum = udf_tag_checksum(&cfi->descTag);

	if (adinicb || (sizeof(struct fileIdentDesc) <= -fibh->soffset)) {
		memcpy((uint8_t *)sfi, (uint8_t *)cfi,
			sizeof(struct fileIdentDesc));
	} else {
		memcpy((uint8_t *)sfi, (uint8_t *)cfi, -fibh->soffset);
		memcpy(fibh->ebh->b_data, (uint8_t *)cfi - fibh->soffset,
		       sizeof(struct fileIdentDesc) + fibh->soffset);
	}

	if (adinicb) {
		mark_inode_dirty(inode);
	} else {
		if (fibh->sbh != fibh->ebh)
			mark_buffer_dirty_inode(fibh->ebh, inode);
		mark_buffer_dirty_inode(fibh->sbh, inode);
	}
	return 0;
}