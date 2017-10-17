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

    if (drizzle_binlog_rows_event_version(event->type) > 1)
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
        drizzle_binlog_row_st row(is_rows_update_event(event->type));
        drizzle_binlog_parse_row(rows_event, event->data_ptr, columns_present,
            &row.values_before);
        rows_event->rows.push_back(row);
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
    unsigned char *columns_present, column_values *row)
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

        drizzle_binlog_column_value_st column_value;
        column_value.type = column_type;

        // parse the column value
        if (bit_is_set(null_bitmap, idx_null_bitmap++))
        {
            // set null value
            printf("NULL VALUE\n");
            column_value.is_null = true;
        }
        else if (column_protocol_datatype(column_type) == FIXED_STRING)
        {
            auto meta_column_type =
                (drizzle_column_type_t) event->field_metadata[metadata_offset];
            size_t field_byte_length = event->field_metadata[metadata_offset+1];

            if (fixed_string_is_enum(meta_column_type))
            {
                column_value.set_field_value(meta_column_type, ptr, field_byte_length);
                ptr += field_byte_length;

                printf("enum: %ld bytes\n", field_byte_length);
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
            auto blob_byte_length = event->field_metadata[metadata_offset];
            uint64_t blob_len = 0;
            memcpy(&blob_len, ptr, blob_byte_length);
            ptr += blob_byte_length;
            column_value.set_field_value(column_type, ptr, blob_len);
            ptr += blob_len;
        }
        else if (column_protocol_datatype(column_type) == TEMPORAL)
        {}
        else
        {
            column_value.set_field_value(column_type, ptr);
            // uint8_t lval[16];
            // memset(lval, 0, sizeof(lval));
            // ptr += unpack_numeric_field(ptr, column_type, lval);
            // printf("numeric value: %d", lval[0]);
        }

        row->push_back(column_value);

        metadata_offset += get_metadata_len(column_type);

    }

    return DRIZZLE_RETURN_OK;

    }

void drizzle_binlog_column_value_st::set_field_value(
    drizzle_column_type_t _column_type, unsigned char*ptr, size_t value_length)
{
    this->type = _column_type;
    printf("set_field_value: %s\n", drizzle_column_type_str(this->type));
    switch ( this->type )
    {
        case DRIZZLE_COLUMN_TYPE_TINY_BLOB:
        case DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB:
        case DRIZZLE_COLUMN_TYPE_LONG_BLOB:
        case DRIZZLE_COLUMN_TYPE_BLOB:
            mem_alloc_cpy(&raw_value, value_length, &ptr);
            this->field._uchar_ptr = raw_value;
            break;
        case DRIZZLE_COLUMN_TYPE_DECIMAL:
        case DRIZZLE_COLUMN_TYPE_VARCHAR:
        case DRIZZLE_COLUMN_TYPE_BIT:
        case DRIZZLE_COLUMN_TYPE_NEWDECIMAL:
        case DRIZZLE_COLUMN_TYPE_GEOMETRY:
        case DRIZZLE_COLUMN_TYPE_VAR_STRING:
            break;

        case DRIZZLE_COLUMN_TYPE_YEAR:
        case DRIZZLE_COLUMN_TYPE_DATE:
        case DRIZZLE_COLUMN_TYPE_TIME:
        case DRIZZLE_COLUMN_TYPE_TIME2 :
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP2 :
        case DRIZZLE_COLUMN_TYPE_DATETIME:
        case DRIZZLE_COLUMN_TYPE_DATETIME2 :
            break;

        // DRIZZLE_COLUMN_TYPE_ENUM and DRIZZLE_COLUMN_TYPE_SET are
        // packed as a DRIZZLE_COLUMN_TYPE_STRING and cannot appear as a
        // type in the fixed header. Their respective types must be
        // extracted from the column metadata
        case DRIZZLE_COLUMN_TYPE_STRING:
            break;

        case DRIZZLE_COLUMN_TYPE_TINY:
            this->field._int8 = ptr[0];
            break;
        case DRIZZLE_COLUMN_TYPE_SHORT:
            this->field._int16 = (ushort) drizzle_get_byte2(ptr);
            break;
        case DRIZZLE_COLUMN_TYPE_INT24 :
            this->field._uint32 = (int32_t) drizzle_get_byte3(ptr);
            break;
        case DRIZZLE_COLUMN_TYPE_LONG:
            this->field._uint32 = (uint32_t) drizzle_get_byte4(ptr);
            break;
        case DRIZZLE_COLUMN_TYPE_FLOAT:
        case DRIZZLE_COLUMN_TYPE_DOUBLE:
            break;
        case DRIZZLE_COLUMN_TYPE_LONGLONG:
            this->field._uint64 = (uint64_t) drizzle_get_byte8(ptr);
            break;

        case DRIZZLE_COLUMN_TYPE_NULL:
            break;
        case DRIZZLE_COLUMN_TYPE_ENUM:
        case DRIZZLE_COLUMN_TYPE_SET:
            mem_alloc_cpy(&raw_value, value_length+1,
            &ptr, value_length);
            this->field._uchar_ptr = raw_value;
            break;
        case DRIZZLE_COLUMN_TYPE_NEWDATE:
        default:
            break;
    }
}

drizzle_return_t drizzle_binlog_get_int(drizzle_binlog_row_st *row,
    size_t field_number, uint32_t *before, uint32_t *after)
{
    if (field_number >= row->values_before.size() )
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }

    drizzle_binlog_column_value_st *column_value = &row->values_before.at(field_number);
    *before = column_value->field._uint32;

    if (row->is_update_event)
    {
        column_value = &row->values_before.at(field_number);
        *after = column_value->field._uint32;
    }

    return DRIZZLE_RETURN_OK;
}
