#!/bin/lua

local headers = {
    "common.h",
    "hash.h",
    "hda.h",
    "hds.h",
    "darr.h",
    "dstr.h",
    "trie.h",
    "args.h",
    "kval.h",
    "llog.h",
    "splitty.h",
    "xfuncs.h"
}

local out = assert(io.open("linc_tools.h", "w"))
local linc_path = assert(os.getenv("HOME")) .. "/Projects/linc_tools"
local handle = io.popen("git -C " .. linc_path .. " rev-parse --short HEAD")
local commit_hash = string.sub(handle:read("a"), 1, -2)
io.close(handle)

out:write(string.format([[
/*
 * linc_tools.h
 *
 * GENERATED FILE - DO NOT EDIT DIRECTLY
 *
 * Commit [%s]
 */

#ifndef LINC_TOOLS_H_INCLUDED
#define LINC_TOOLS_H_INCLUDED

]], commit_hash))


for _, name in ipairs(headers) do
    out:write(string.format([[
/*------------------------------------
%s
------------------------------------*/

]], name))

    local path = linc_path .. "/src/" .. name
    local file = assert(io.open(path, "r"))

    while true do
        local line = file:read("L")
        if not line then
            break
        end

        if not line:find('^#include "') then
            out:write(line)
        end
    end

    file:close()
    out:write("\n")
end

out:write(string.format("\n#endif"))
out:close()

-- Run tests

local clang_cmd = [[
    clang -std=gnu23 -D_GNU_SOURCE \
        -Wall -Wextra -Werror \
        -I. \
]]

assert(os.execute(clang_cmd .. string.format([[
        %s/test/amalgam/test_amalgam_double.c \
        -o /tmp/linc_test_amalgam_double]], linc_path)))
assert(os.execute(clang_cmd .. string.format([[
        %s/test/amalgam/test_amalgam_a.c %s/test/amalgam/test_amalgam_b.c \
        -o /tmp/linc_test_amalgam_a_b]], linc_path, linc_path)))

assert(os.execute("/tmp/linc_test_amalgam_double"))
assert(os.execute("/tmp/linc_test_amalgam_a_b"))

os.remove("/tmp/linc_test_amalgam_double")
os.remove("/tmp/linc_test_amalgam_a_b")
