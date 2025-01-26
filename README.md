badram
===

This is an UEFI program that marks a set of memory pages as "Unusable" so that the operating system does not use them.
This is useful for avoiding regions of bad memory. The memory ranges are configured in `main.c`.
Once the memory regions are allocated, the program executes systemd-boot.
