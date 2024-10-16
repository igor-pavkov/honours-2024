void _checkOplogEntries(const std::vector<std::string>& actualOplogEntries,
                        const std::vector<std::string>& expectedOplogEntries) {
    std::string actualOplogEntriesStr;
    joinStringDelim(actualOplogEntries, &actualOplogEntriesStr, ',');
    std::string expectedOplogEntriesStr;
    joinStringDelim(expectedOplogEntries, &expectedOplogEntriesStr, ',');
    ASSERT_EQUALS(expectedOplogEntries.size(), actualOplogEntries.size())

        << "Incorrect number of oplog entries written to oplog. Actual: " << actualOplogEntriesStr
        << ". Expected: " << expectedOplogEntriesStr;
    std::vector<std::string>::size_type i = 0;
    for (const auto& actualOplogEntry : actualOplogEntries) {
        const auto& expectedOplogEntry = expectedOplogEntries[i++];
        ASSERT_EQUALS(expectedOplogEntry, actualOplogEntry)
            << "Mismatch in oplog entry at index " << i << ". Actual: " << actualOplogEntriesStr
            << ". Expected: " << expectedOplogEntriesStr;
    }
}