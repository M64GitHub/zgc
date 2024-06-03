const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});

    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "ZIGReSIDL",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });
    exe.addIncludePath(.{ .path = "src/ReSIDL" });
    exe.addIncludePath(.{ .path = "/usr/include" });
    exe.linkLibC();
    exe.linkSystemLibrary("SDL2");

    b.installArtifact(exe);
}
