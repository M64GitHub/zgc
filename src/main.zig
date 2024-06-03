const std = @import("std");
const SIDP = @cImport({
    @cInclude("sidsdl.c");
});

pub fn main() !void {
    const stdout_file = std.io.getStdOut().writer();
    var bw = std.io.bufferedWriter(stdout_file);
    const stdout = bw.writer();

    try stdout.print("Badda the BAM!\n", .{});

    SIDP.play_the_dump();

    // try stdout.print("Badda the BAM {d}!\n", .{p});

    try bw.flush(); // don't forget to flush!
}
