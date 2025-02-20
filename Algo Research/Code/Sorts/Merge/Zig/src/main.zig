const std = @import("std");
const expect = std.testing.expect;
const assert = std.debug.assert;
const print = std.debug.print;

fn merge(A: []i32, begin: usize, middle: usize, end: usize, B: []i32) !void {
    var i = begin;
    var j = middle;
    var k = begin;
    var temp = std.ArrayList(i32).init(std.heap.page_allocator);
    try temp.appendSlice(A[begin..end]);
    const temp_slice = temp.items;

    while (i < middle and j < end) : (k += 1) {
        if (temp_slice[i] <= temp_slice[j]) {
            B[k] = temp_slice[i];
            i += 1;
        } else {
            B[k] = temp_slice[j];
            j += 1;
        }
    }
    while (i < middle) : (k += 1) {
        B[k] = temp_slice[i];
        i += 1;
    }
    while (j < end) : (k += 1) {
        B[k] = temp_slice[j];
        j += 1;
    }
}

fn split_merge(A: []i32, begin: usize, end: usize, B: []i32) !void {
    if (end - begin <= 1) {
        return;
    }
    const middle = (end + begin) / 2;
    try split_merge(B, begin, middle, A);
    try split_merge(B, middle, end, A);
    try merge(A, begin, middle, end, B);
}

fn sort(A: []i32, B: []i32) !void {
    assert(A.len == B.len);
    try copy_array(A, 0, A.len, B);
    try split_merge(B, 0, A.len, A);
}

fn copy_array(A: []i32, begin: usize, end: usize, B: []i32) !void {
    var k = begin;
    while (k < end) : (k += 1) {
        B[k] = A[k];
    }
}

pub fn main() !void {
    var array: [6]i32 = .{12, 11, 13, 5, 6, 7};
    var work_array: [6]i32 = .{0, 0, 0, 0, 0, 0};
    std.debug.print("Before sorting: {any}\n", .{array});
    try sort(&array, &work_array);
    std.debug.print("After sorting: {any}\n", .{array});
}
