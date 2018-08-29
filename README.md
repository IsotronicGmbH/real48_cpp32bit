# real48_cpp32bit

Delphi supports a data type that is of 48 bit width and represents a floating point number. I needed to read/write this type but work with doubles afterwards.

https://www.pcorner.com/list/C/BPREAL.ZIP/INFO/ provides a C implementation of such conversion routines. This implementation is from April 1993 and assumes a 16bit subsystem (this was suggested in https://www.c-plusplus.net/forum/topic/111876/real48-daten-lesen).

This repository contains the (few and small) changes needed to make this work under 32/64 bit systems (basically specifying that `unsigned` means `uint16_t` in this context).


# License

The original document states the following:

You may freely use and distribute BPREAL.C, BPRTEST.C, and
BPREAL.DOC, but any distribution should include all 3 files
together and intact. Send questions or comments to the author,
Richard Biffl, at 1024 N. Utah St. #618, Arlington, Virginia
22201, or on CompuServe at 73607,3043.
