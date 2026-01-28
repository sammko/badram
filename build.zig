const std = @import("std");

pub fn build(b: *std.Build) void {
    const optimize = b.standardOptimizeOption(.{});

    const a = b.addExecutable(.{
        .name = "badramx64",
        .root_module = b.createModule(.{
            .root_source_file = b.path("main.zig"),
            .target = b.resolveTargetQuery(.{
                .cpu_arch = .x86_64,
                .os_tag = .uefi,
            }),
            .optimize = optimize,
        }),
        .linkage = .static,
    });
    a.subsystem = .EfiBootServiceDriver;

    b.installArtifact(a);
}
