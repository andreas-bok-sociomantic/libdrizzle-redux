TableMapEvent
=============

Introduction
------------

This class parse the tablemap event.

.. autoclass:: TableMapEvent

   :members: table_id,flag,schema_name_len,schema_name,table_name_len,table_name,column_count,column_type_def,column_meta_data

Constructor
-----------

It will intialize all its data member to NULL/-1.


Getter Functions
----------------


.. cc:function::  void initWithData(const unsigned char * data)

   Get the raw data and call all the setter functions with appropriate value

   :param data: Raw data from binlog 

.. cc:function::uint32_t  getTimestamp()


   Returns the timestamp for given event

.. cc:function:: uint8_t getType()


   Returns the Type for given event
    
.. cc:function:: uint32_t  getServerId()

    
   Returns the timestamp for given event
    
.. cc:function:: uint32_t  getEventSize()


   Returns the Event Size for given event

.. cc:function:: uint32_t  getLogPos()


   Returns the Log Position for given event

.. cc:function:: uint32_t  getFlagH()


   Returns the flag header for given event

.. cc:function:: uint64_t getTableId()

   Return the numeric table id for given event

.. cc:funciton:: uint16_t getFlagPh()

	
   Return the flag of post header for given event

.. cc:function:: int getSchemaNameLen()


   Return the length of the schema name for given event

.. cc:function:: char * getSchemaName()

   Return the schema name for given event

.. cc:function:: int getTableNameLen()

   Return the table name length for given event

.. cc:function:: char * getTableName()

   Return the table name for given event

.. cc:function:: uint64_t getColumnCount()

   Return the number of column for given event

.. cc:function:: uint8_t * getColumnTypeDef()

   Return the column definations (one byte per field type) for given event

Setter Functions
----------------

.. cc:function:: void setTableId(uint64_t value)

   Set the parsed value of table_id  into given object.
   
.. cc:function:: void setFlagPh(uint16_t value)

   Set the parsed value of  flag into given object.

.. cc:function:: void setSchemaNameLen(uint8_t value)

   Set the parsed value of schena_name_len into given object.

.. cc:function::void setSchemaName(char *value)   

   Set the parsed value of schema_name  into given object.

.. cc:function::void setTableNameLen(uint8_t value)

   Set the parsed value of table_name_len  into given object.

.. cc:function:: void setTableName(char *value)

  Set the parsed value of table_name  into given object.

.. cc:function:: void setColumnCount(uint64_t value)

  Set the parsed value of column_count into given object.
  
.. cc:function:: void setColumnTypeDef(uint8_t * value)

  Set the parsed value of column_type_def into given object.

.. cc:function:: TODO

  Set the parsed value of column_meta_data into given object.
