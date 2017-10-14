#include "config.h"
#include "src/common.h"

drizzle_binlog_rows_event_st *drizzle_binlog_get_rows_event(
    drizzle_binlog_event_st *event)
{
    return event->binlog_rbr->get_rows_event();
}

drizzle_binlog_rows_event_st *drizzle_binlog_parse_rows_event(
    drizzle_binlog_event_st *event)
{
    auto rows_event = event->binlog_rbr->create_rows_event();
    assert(rows_event != NULL);
    set_event_header(&rows_event->header, event);

    // Get the table id
    rows_event->table_id = drizzle_get_byte6(event->data_ptr);
    event->data_ptr+=6;

    // Get the associated tablemap
    auto table_map_event = event->binlog_rbr->get_tablemap_event(rows_event->table_id);

    // copy column types
    mem_alloc_cpy<uint8_t*>(&rows_event->column_type_def,
        table_map_event->column_count,
        &table_map_event->column_type_def);

    // copy field metadata
    mem_alloc_cpy<uint8_t*>(&rows_event->field_metadata,
        table_map_event->field_metadata_len, &table_map_event->field_metadata);

    // copy table name
    strcpy(rows_event->table_name, table_map_event->table_name);

    // replication flags
    uint16_t flags = drizzle_get_byte2(event->data_ptr);
    event->data_ptr+=2;
    if ( rows_event->table_id == 0x00ffffff && flags == (1 << 0))
    {
        return NULL;
    }

    if (drizzle_binlog_rows_event_version(event->type) == 2)
    {
        auto len = drizzle_get_byte2(event->data_ptr);
        event->data_ptr+=2;
        if (len >2)
        event->data_ptr += len-2;
    }

    // column_count
    rows_event->column_count = drizzle_binlog_get_encoded_len(event);

    rows_event->bitmap_size = (rows_event->column_count + 7)/8;

    // update the mapping between table id and table row
    uint8_t columns_present[rows_event->column_count];
    memcpy(&columns_present, event->data_ptr, rows_event->bitmap_size);
    event->data_ptr += rows_event->bitmap_size;

    while ( drizzle_binlog_event_available_bytes(event) >= DRIZZLE_BINLOG_CRC32_LEN )
    {
        drizzle_binlog_parse_row(rows_event, event->data_ptr, columns_present);
        event->data_ptr += drizzle_binlog_event_available_bytes(event);
    }

    event->binlog_rbr->add_table_row_mapping(rows_event);
    return rows_event;
}

uint64_t drizzle_binlog_rows_event_table_id(
    drizzle_binlog_rows_event_st *event)
{
    return event->table_id;
}

uint64_t drizzle_binlog_rows_event_column_count(
    drizzle_binlog_rows_event_st *event)
{
    return event->column_count;
}

const char* drizzle_binlog_rows_event_table_name(
    drizzle_binlog_rows_event_st *event)
{
    return event->table_name;
}

drizzle_return_t drizzle_binlog_parse_row(
    drizzle_binlog_rows_event_st *event, unsigned char *ptr,
    unsigned char *columns_present)
{
    unsigned metadata_offset = 0;
    const unsigned char *null_bitmap = ptr;
    ptr += event->bitmap_size;
    unsigned idx_null_bitmap = 0;

    for ( unsigned i = 0; i < event->column_count; i++ )
    {
        // skip if column is not present
        if (!bit_is_set(columns_present, i))
        {
            continue;
        }

        auto column_type = (drizzle_column_type_t) event->column_type_def[i];
        printf("drizzle_binlog_parse_row: %s \n", drizzle_column_type_str(column_type));

        // parse the column value
        if (bit_is_set(null_bitmap, idx_null_bitmap++))
        {
            // set null value
            printf("NULL VALUE\n");
        }
        else if (column_protocol_datatype(column_type) == FIXED_STRING)
        {
            auto meta_column_type =
                (drizzle_column_type_t) event->field_metadata[metadata_offset];

            if (fixed_string_is_enum(meta_column_type))
            {
                uint8_t val[event->field_metadata[metadata_offset + 1]];
                uint64_t bytes = unpack_enum(ptr, &event->field_metadata[metadata_offset], val);
                printf("enum: %" PRIu64 " bytes\n", bytes);
                // parse enum
                // unsigned char val[metadata_offset+1];
            }
            else
            {
                /**
                 * PARSE VAR_STRING
                 * The first byte in the metadata stores the real type of
                 * the string (ENUM and SET types are also stored as fixed
                 * length strings).
                 *
                 * The first two bits of the second byte contain the XOR'ed
                 * field length but as that information is not relevant for
                 * us, we just use this information to know whether to read
                 * one or two bytes for string length.
                 */

/*                uint16_t meta = metadata[metadata_offset + 1] + (metadata[metadata_offset] << 8);
                int bytes = 0;
                uint16_t extra_length = (((meta >> 4) & 0x300) ^ 0x300);
                uint16_t field_length = (meta & 0xff) + extra_length;

                if (field_length > 255)
                {
                    bytes = ptr[0] + (ptr[1] << 8);
                    ptr += 2;
                }
                else
                {
                    bytes = *ptr++;
                }*/
            }
        }
        else if (column_type == DRIZZLE_COLUMN_TYPE_BIT)
        {

        }
        else if (column_type== DRIZZLE_COLUMN_TYPE_NEWDECIMAL)
        {

        }
        else if (column_protocol_datatype(column_type) == VARIABLE_STRING)
        {
            /*size_t sz;
            int bytes = metadata[metadata_offset] | metadata[metadata_offset + 1] << 8;*/
        }
        else if (column_protocol_datatype(column_type) == BLOB)
        {

        }
        else if (column_protocol_datatype(column_type) == TEMPORAL)
        {}
        else
        {
            uint8_t lval[16];
            memset(lval, 0, sizeof(lval));
            ptr += unpack_numeric_field(ptr, column_type, lval);
            printf("numeric value: %d", lval[0]);
        }

        metadata_offset += get_metadata_len(column_type);

    }

    return DRIZZLE_RETURN_OK;

    }
