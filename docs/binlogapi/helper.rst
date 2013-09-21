Helper Functions
================

Introduction
------------

Helper contains functions which give binlog api the capabilities to parse binary MySQL logs.

.. warning::
      You should call any binlogapi functions with the raw data for corresponding event otherwise you may end up getting INT_MAX or INT_MIN type of values which may or may not be erroneous.


MySQL Fields
------------

.. cc:type:: enum_field_types
   
   An ENUM of the possible MySQL field types

   .. py:data:: MYSQL_TYPE_DECIMAL

   .. py:data:: MYSQL_TYPE_TINY

   .. py:data:: MYSQL_TYPE_SHORT

   .. py:data:: MYSQL_TYPE_LONG

   .. py:data:: MYSQL_TYPE_FLOAT

   .. py:data:: MYSQL_TYPE_DOUBLE

   .. py:data:: MYSQL_TYPE_NULL

   .. py:data:: MYSQL_TYPE_TIMESTAMP

   .. py:data:: MYSQL_TYPE_LONGLONG

   .. py:data:: MYSQL_TYPE_INT24

   .. py:data:: MYSQL_TYPE_DATE

   .. py:data:: MYSQL_TYPE_TIME

   .. py:data:: MYSQL_TYPE_DATETIME

   .. py:data:: MYSQL_TYPE_YEAR

   .. py:data:: MYSQL_TYPE_NEWDATE

   .. py:data:: MYSQL_TYPE_VARCHAR

   .. py:data:: MYSQL_TYPE_BIT

   .. py:data:: MYSQL_TYPE_NEWDECIMAL

   .. py:data:: MYSQL_TYPE_ENUM

   .. py:data:: MYSQL_TYPE_SET

   .. py:data:: MYSQL_TYPE_TINY_BLOB

   .. py:data:: MYSQL_TYPE_MEDIUM_BLOB

   .. py:data:: MYSQL_TYPE_LONG_BLOB

   .. py:data:: MYSQL_TYPE_BLOB

   .. py:data:: MYSQL_TYPE_VAR_STRING

   .. py:data:: MYSQL_TYPE_STRING

   .. py:data:: MYSQL_TYPE_GEOMETRY

.. cc:type:: enum_field_bytes   

   An ENUM of formats in which Field types are stored

   .. py:data:: LEN_ENC_STR
      
      Length Encoded String

   .. py:data:: READ_1_BYTE

   .. py:data:: READ_2_BYTE

   .. py:data:: READ_4_BYTE

   .. py:data:: READ_8_BYTE

   .. py:data:: NOT_FOUND

      No format for corresponding field type present
 
.. cc:type:: enum_event_type

   An ENUM for possible Event Types

   .. py:data:: TABLE_MAP_EVENT

   .. py:data:: WRITE_ROWS_EVENTv1

      v1 is for version number

   .. py:data:: UPDATE_ROWS_EVENTv1

      v1 is for version number

   .. py:data:: DELETE_ROWS_EVENTv1

      v1 is for version number

.. cc:type:: enum_col_type

   An ENUM for getting the filed type in abstract level.

   .. py:data:: STRING

      Type is string

   .. py:data:: INT
 
      Type is INTEGER

Functions
---------

.. cc:function:: bool getNextBit(uint8_t& val)

   Returns the next bit of the number (LSB)

   :param val: The number

.. cc:function:: uint16_t getByte2(int pos,const unsigned char* data)

   get 2 byte number from raw data

   :param pos: Start psosition
   :param data: Raw Data of binlog
   
.. cc:function:: uint32_t getByte3(int pos,const unsigned char* data)

   get 3 byte number from raw data

   :param pos: Start psosition
   :param data: Raw Data of binlog

.. cc:function:: uint32_t getByte4(int pos,const unsigned char* data)

   get 4 byte number from raw data

   :param pos: Start psosition
   :param data: Raw Data of binlog
   
.. cc:function:: uint64_t getByte6(int pos,const unsigned char* data)

   get 6 byte number from raw data

   :param pos: Start psosition
   :param data: Raw Data of binlog
   
.. cc:function:: uint64_t getByte8(int pos,const unsigned char* data)

   get 8 byte number from raw data

   :param pos: Start psosition
   :param data: Raw Data of binlog

.. cc:function:: char * getString(int pos,int len,const unsigned char * data)

   Gets the string of specified length	

   :param pos: Start  position
   :param len: Length of string to read
   :param data: Raw data from which we have to read the string

.. cc:function:: uint64_t getEncodedLen(int& pos,const unsigned char * data)

   This will decode the Length Encoded string and returns the number of byted we should read. It will also update the start pos.

   :param pos: reference to start position
   :param data: Raw data which conatins LenEncoded string

.. cc:function:: int lookup_metadata_field_size(enum_field_types field_type)

   Returns the Meta len of given field type

   :param field_type: The field type (enum)
   	
.. cc:function:: string getIntToStr(uint64_t num)	

   converts int to string

   :param num: The integer

.. cc:function:: int getBoolArray(bool arr[],const unsigned char data[],int start_pos,int _byte,int _bit)

   Returns the count of columns not present and also fills the 'arr' with column present bitmap

   :param arr: reference to boolean array representing column present bitmap
   :param data: Raw data
   :param start_pos: start position
   :param _byte: total number of bytes to read (remember column count can be less than _byte*8)
   :param _bit: Total number of bits(column count) to read	

.. cc:function:: enum_field_bytes lookup_field_bytes(enum_field_types field_type)	

   Returns the enum of field bytes which should be read for corresponding column/field type

   :param field_type: Field type enum
