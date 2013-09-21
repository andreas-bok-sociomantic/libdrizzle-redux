XidEvent
==========

Introduction
------------

This class parse the XidEvent

.. autoclass:: XidEvent 

   :members: Xid

Constructor
-----------

It will initialize all its data members to -1

Getter Functions
----------------


.. cc:function::uint32_t  getTimestamp()


   Returns the timestamp for given event

.. cc:function:: uint8_t getType()


   Returns the Type for given event

.. cc:function:: uint32_t  getServerId()


   Returns the timestamp for given event

.. cc:function:: uint32_t  getEventSize()


   Returns the Event Size for given event

.. cc:function:: uint32_t  getLogPos()


   Returns the Log Position for next event

.. cc:function:: uint32_t  getFlagH()


   Returns the binlog Event Flag from header


.. cc:function:: uint64_t  getXid()


   Returns the master's transaction id for event


Setter Functions
----------------

.. cc:function::  void initWithData(const unsigned char * data)

   Get the raw data and call all the setter functions with appropriate value

   :param data: Raw data from binlog 

.. cc:function:: void  setXid(uint64_t value)


   sets the parsed transaction id of event into its object

   :param value: parsed Transaction id 


