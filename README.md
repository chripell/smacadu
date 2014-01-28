
this is port of: http://www.literatecode.com/smacadu to Linux

smacdig

This tool let you map all existing objects within specified Dedicated
File, even if the smart card does not have DIR command or it tries to
hide an object from DIR. Such mapping is a continuous procedure which
you may terminate any moment by pressing a key. You may also specify a
DF path and/or initial object ID to start from.

smacget

You can dump any publicly accessible Elementary File (EF) from smart
card to file by using smacget. Simply specify the full path of EF and
it will be extracted to file named as EF's ID.

smactty

This is the APDU console that you may use to directly send commands to
a smart card. To use this tool, you need to know what is PC/SC and at
least C0 A4 00 00 02 3F 00 must ring some bells.

Run any of these tools without parameters will output usage info and a
list of available smart card readers. You must specify desired smart
card reader by its name as a first parameter then. You do not need to
specify a whole name actually. A unique substring from the name would
do just fine. For smactty, you may specify characters inside APDU
commands as is, without remembering their hexadecimal representation
by using "-" in front of a character. For example, you may simply type
-p-a-s-s-w-o-r-d instead of 70 61 73 73 77 6F 72 64.
