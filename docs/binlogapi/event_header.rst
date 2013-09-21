EventHeader
==========


Introduction
------------

This class parses the data and set all the header varibles.

.. autoclass:: EventHeader

   :members: timestamp,type,server_id,event_size,log_pos,flag

Constructor
-----------

It will initialize all its data members to -1


Setter Functions
----------------


.. cc:function:: int setHeader(unsigned const char* data)

    Set the parsed values of  timestamp,type,server_id,event_size,log_pos,flag into given object.
