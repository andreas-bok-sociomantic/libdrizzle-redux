/**
 * @brief      Get the version of the RBR replication Rows Event
 *
 * @param[in]  column_type  a column type
 *
 * @return     Return either 1 or 2 or 0 if column type wasn't found
 */
uint8_t drizzle_binlog_rows_event_version(
    drizzle_binlog_event_types_t column_type);

/**
 * @brief      Print an array of char in hex format
 *
 * @param[in]  ptr   pointer to a char array
 * @param[in]  len   length of the char array
 */
void dump_array_to_hex(const unsigned char *ptr, uint32_t len);

/**
 * @brief      Get the distance between two pointers in a char array
 *
 * @param      ptr1  A pointer in the a char array
 * @param      ptr2  A pointer in the a char array
 *
 * @return     distance in bytes between pointers
 */
uint32_t ptr_dist(unsigned char *ptr1, unsigned char *ptr2);
