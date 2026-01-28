badram
===

This is an UEFI driver that marks a set of memory pages as "Unusable" so that the operating system does not use them.
This is useful for avoiding regions of bad memory. The memory ranges are configured in `main.zig`.
The driver is intended to be placed in systemd-boot drop-in driver directory `esp/EFI/systemd/drivers`.
