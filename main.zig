const uefi = @import("std").os.uefi;

const ranges = [_]struct { addr: u64, pages: u64 }{
    .{ .addr = 0x04f950000, .pages = 0x10 },
    .{ .addr = 0x297f00000, .pages = 0x50 },
};

pub fn main() void {
    const boot_services = uefi.system_table.boot_services.?;

    for (ranges) |range| {
        _ = boot_services.allocatePages(
            .{ .address = @ptrFromInt(range.addr) },
            .unusable_memory,
            range.pages,
        ) catch {};
    }
}
