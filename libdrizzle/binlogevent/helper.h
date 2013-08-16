typedef enum{
	MYSQL_TYPE_DECIMAL,
	MYSQL_TYPE_TINY,
	MYSQL_TYPE_SHORT,
	MYSQL_TYPE_LONG,
	MYSQL_TYPE_FLOAT,
	MYSQL_TYPE_DOUBLE,
	MYSQL_TYPE_NULL,
	MYSQL_TYPE_TIMESTAMP,
	MYSQL_TYPE_LONGLONG,
	MYSQL_TYPE_INT24,
	MYSQL_TYPE_DATE,
	MYSQL_TYPE_TIME,
	MYSQL_TYPE_DATETIME,
	MYSQL_TYPE_YEAR,
	MYSQL_TYPE_NEWDATE,
	MYSQL_TYPE_VARCHAR,
	MYSQL_TYPE_BIT,
	MYSQL_TYPE_NEWDECIMAL,
	MYSQL_TYPE_ENUM,
	MYSQL_TYPE_SET,
	MYSQL_TYPE_TINY_BLOB,
	MYSQL_TYPE_MEDIUM_BLOB,
	MYSQL_TYPE_LONG_BLOB,
	MYSQL_TYPE_BLOB,
	MYSQL_TYPE_VAR_STRING,
	MYSQL_TYPE_STRING,
	MYSQL_TYPE_GEOMETRY
}enum_field_types;

typedef enum{
	LEN_ENC_STR = -1,
	READ_1_BYTE = 1,
	READ_2_BYTE = 2,
	READ_4_BYTE = 4,
	READ_8_BYTE = 8,
	NOT_FOUND   = 0 
}enum_field_bytes;

typedef enum{
	TABLE_MAP_EVENT = 19,
	WRITE_ROWS_EVENTv1 = 23,
	UPDATE_ROWS_EVENTv1,
	DELETE_ROWS_EVENTv1,
}enum_event_type;



#define mask(__b) \
	((uint32_t)(__b)==32 ? 0xffffffff : \
	 ((uint32_t)(__b)==24 ? 0xffffff : \
	  ((uint32_t)(__b)==16 ? 0xffff : \
	   ((uint32_t)(__b)==8 ? 0xff : 0xffffffffffffffff ))))

#define bytes_col_count(__b) \
	((uint64_t)(__b)<0xfb ? 1 : \
	 ((uint64_t)(__b)==0xfc ? 2 : \
	  ((uint64_t)(__b)==0xfd ? 3 : 8)))

bool getNextBit(uint8_t& val);
/**
 * get 2 byte number from raw data
 *
 * @param[in] pos Start reading from pos.
 * @param[in] data Raw data from binglog.
 *
 *@retval 2 byte number
 */
uint16_t getByte2(int pos,const unsigned char* data);
/**
 * get 4 byte number from raw data
 *
 * @param[in] pos Start reading from pos.
 * @param[in] data Raw data from binglog.
 *
 *@retval 4 byte number
 */
uint32_t getByte4(int pos,const unsigned char* data);
/**
 * get 3 byte number from raw data
 *
 * @param[in] pos Start reading from pos.
 * @param[in] data Raw data from binglog.
 *
 *@retval 3 byte number
 */
uint32_t getByte3(int pos,const unsigned char* data);
/**
 * get 6 byte number from raw data
 *
 * @param[in] pos Start reading from pos.
 * @param[in] data Raw data from binglog.
 *
 *@retval 6 byte number
 */
uint64_t getByte6(int pos,const unsigned char* data);
/**
 * get 8 byte number from raw data
 *
 * @param[in] pos Start reading from pos.
 * @param[in] data Raw data from binglog.
 *
 *@retval 8 byte number
 */
uint64_t getByte8(int pos,const unsigned char* data);
/** gets the string of specified length
  *
  * @param[in] pos Start reading from pos.
  * @param[in] len Length of string
  * @param[in] data Raw data from binglog.
  */
char * getString(int pos,int len,const unsigned char * data);

uint64_t getEncodedLen(int& pos,const unsigned char * data);

int lookup_metadata_field_size(enum_field_types field_type);

enum_field_bytes lookup_field_bytes(enum_field_types field_type);

