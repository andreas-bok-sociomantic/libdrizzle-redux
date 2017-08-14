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

/**
 * @brief Check if a bit is set
 *
 * @param ptr Pointer to start of bitfield
 * @param columns Number of columns (bits)
 * @param current_column Zero indexed column number
 * @return True if the bit is set
 */
//static bool bit_is_set(uint8_t *ptr, int columns, int current_column);


/**
 * @brief      Gets the length encoded integer in bytes
 *
 * @param      __b   a char value
 *
 * @return     { description_of_the_return_value }
 */
#define bytes_col_count(__b) \
    ((uint64_t) (__b) < 0xfb ? 1 : \
     ((uint64_t) (__b) == 0xfc ? 2 : \
      ((uint64_t) (__b) == 0xfd ? 3 : 8)))


/**
 * @brief      Create a bitmask with all bits set to 1
 *
 * @param      __b   length of bitmask in bits
 *
 * @return     bitmask with a all bits set to 1
 */
#define mask(__b) \
    ((uint32_t) (__b) == 32 ? 0xffffffff : \
     ((uint32_t) (__b) == 24 ? 0xffffff : \
      ((uint32_t) (__b) == 16 ? 0xffff : \
       ((uint32_t) (__b) == 8 ? 0xff : 0xffffffffffffffff ))))
