#pragma once
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
 * @brief      Get the distance between two pointers in a char array
 *
 * @param      ptr1  A pointer in the a char array
 * @param      ptr2  A pointer in the a char array
 *
 * @return     distance in bytes between pointers
 */
uint32_t ptr_dist(unsigned char *ptr1, unsigned char *ptr2);


/**
 * @brief      Get remaining bytes in the binlog event
 *
 * @param      event  a drizzle_binlog_event struct
 *
 * @return     number of available bytes
 */
uint32_t drizzle_binlog_event_available_bytes(drizzle_binlog_event_st *event);

/**
 * @brief      Gets the bytes for a length-encoded numeric value from raw bytes
 *
 * @param      binlog_event  A pointer to a binlog event struct
 *
 * @return     the encoded length
 */
uint64_t drizzle_binlog_get_encoded_len(drizzle_binlog_event_st *binlog_event);


/**
 * Copy member values from one header to another.
 *
 * Raw event data is not copied
 *
 * @param      dst    The destination struct
 * @param      src    The source struct
 */
void set_event_header(drizzle_binlog_event_st* dst,
    drizzle_binlog_event_st* src);
