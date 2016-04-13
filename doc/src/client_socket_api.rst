Client Socket API
=================

This section explains the API for clients connecting to the Nutmeg Server. The Server accepts two types of messages: JSON and binary.

To connect with the core, the client must create a ZMQ Request socket and connect to the appropriate port. To interact with the core, it accepts multi-part messages (using ZMQ's SNDMORE flag) where the first part is the string "json" or "binary". For either message type the second part is the JSON message or command for the core. If it is a binary message, the third part of the message is the raw binary data being sent.

Binary messages are primarily used for send images in compressed format (JPEG, PNG, etc.).

The accepted JSON messages and commands are defined by Nutmeg's Server object:

.. doxygenclass:: Server
   :project: Nutmeg
   :members: