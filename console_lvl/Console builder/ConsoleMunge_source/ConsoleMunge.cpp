// ConsoleMunge.cpp   (file build p21j-r30)
// 21j-r30: -list reads a compiled script's build from its gBFX_Build_* string (a console.lvl from the mod tools'
// ScriptMunge / LevelPack, MungeConsole.bat: compiled scripts keep no comments).
// BF1GameExt's munger: packs Lua scripts into a Star Wars Battlefront (2004) .lvl - console.lvl, which BF1GameExt.dll
// reads its scripts from (BF1GameExt_split, BF1GameExt_pause34, BF1GameExt_ctrlpage).
//
//   ConsoleMunge <scripts folder> <output .lvl>   every .lua in the folder, each checked with Lua 4.0's parser first
//   ConsoleMunge -list <file.lvl>                 the scripts in a .lvl (read the way the game reads them)
//   ConsoleMunge -extract <file.lvl> <folder>     a .lvl's scripts written out as .lua files
//
// The .lvl is the game's own format (checked against GOG 1.3's level loader, 21j-r29 sim): a "ucfb" file of chunks,
// each a 4-letter tag, a 4-byte size, the data and zeros up to the next 4 bytes; a script is a "scr_" chunk holding a
// "NAME" (its name, 0-terminated, up to 255 characters) and a "BODY" (the script and one 0 byte - the game runs BODY's
// size - 1 bytes). The game's Lua takes a BODY as Lua text (it compiles it when the script is run, as it does the .lua
// files next to the exe) or as Lua 4.0 compiled code (first byte ESC); ConsoleMunge stores the text.
//
// The syntax check is Lua 4.0's own parser (numbers as float, as the game's): a script that does not parse is not
// packed, and no .lvl is written - the old one stays. Only the parser runs: no script is executed.
//
// Contains Lua 4.0 (lua.h 1.78): Copyright (C) 1994-2000 TeCGraf, PUC-Rio. All rights reserved. Permission is hereby
// granted, without written agreement and without license or royalty fees, to use, copy, modify, and distribute this
// software and its documentation for any purpose, including commercial applications, subject to the conditions in
// lua.h (the copyright notice and this permission notice in all copies or substantial portions of the software).
//
// Build (Windows, 32-bit): the Lua 4.0 core C files (lapi lcode ldebug ldo lfunc lgc llex lmem lobject lparser lstate
// lstring ltable ltm lundump lvm lzio) compiled as C with LUA_NUM_TYPE=float, and this file as C++17.

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

extern "C" {
#include "lua.h"
#include "ldo.h"
#include "lparser.h"
#include "lzio.h"
}

namespace
{
const char kBuild[] = "p21j-r30";
const size_t kMaxScript = 4u << 20;       // 4 MB a script
const size_t kMaxLvl = 64u << 20;         // 64 MB a .lvl to list / extract
const char* const kDllNames[] = {"BF1GameExt_split", "BF1GameExt_pause34", "BF1GameExt_ctrlpage"};

struct Script
{
    std::string name;   // NAME
    std::string body;   // the script as the game runs it (BODY without its last 0 byte)
};

bool ReadWhole(const std::string& path, std::string& out, size_t cap)
{
    FILE* f = fopen(path.c_str(), "rb");
    if (!f)
        return false;
    out.clear();
    char buf[65536];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
    {
        out.append(buf, n);
        if (out.size() > cap)
        {
            fclose(f);
            return false;
        }
    }
    const bool good = !ferror(f);
    fclose(f);
    return good;
}

bool WriteWhole(const std::string& path, const std::string& data)
{
    FILE* f = fopen(path.c_str(), "wb");
    if (!f)
        return false;
    const bool good = fwrite(data.data(), 1, data.size(), f) == data.size();
    return (fclose(f) == 0) && good;
}

bool MoveOver(const std::string& from, const std::string& to)
{
#ifdef _WIN32
    return MoveFileExA(from.c_str(), to.c_str(), MOVEFILE_REPLACE_EXISTING) != 0;
#else
    return rename(from.c_str(), to.c_str()) == 0;
#endif
}

std::string Lower(std::string s)
{
    for (char& c : s)
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return s;
}

// the .lua files in a folder (just the names), sorted
bool LuaFiles(const std::string& dir, std::vector<std::string>& out)
{
    out.clear();
#ifdef _WIN32
    WIN32_FIND_DATAA fd;
    const HANDLE h = FindFirstFileA((dir + "\\*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE)
        return false;
    do
    {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;
        out.push_back(fd.cFileName);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
#else
    DIR* dp = opendir(dir.c_str());
    if (!dp)
        return false;
    while (dirent* e = readdir(dp))
    {
        struct stat st;
        if (stat((dir + "/" + e->d_name).c_str(), &st) == 0 && S_ISREG(st.st_mode))
            out.push_back(e->d_name);
    }
    closedir(dp);
#endif
    // exactly ".lua" (FindFirstFile's "*.lua" would take ".luac" too, through the short names)
    out.erase(std::remove_if(out.begin(), out.end(),
                             [](const std::string& n) { return n.size() < 5 || Lower(n.substr(n.size() - 4)) != ".lua"; }),
              out.end());
    std::sort(out.begin(), out.end(), [](const std::string& a, const std::string& b) { return Lower(a) < Lower(b); });
    return true;
}

std::string JoinPath(const std::string& dir, const std::string& name)
{
#ifdef _WIN32
    const char sep = '\\';
#else
    const char sep = '/';
#endif
    if (dir.empty())
        return name;
    const char last = dir.back();
    return (last == '\\' || last == '/') ? dir + name : dir + sep + name;
}

bool IsCompiled(const std::string& body)
{
    return !body.empty() && static_cast<unsigned char>(body[0]) == 0x1B;
}

// a script's "(file build ...)": a text script's from its first lines, a compiled one's (no comments left) from its
// gBFX_Build_* string anywhere in it ("" when it has none)
std::string FileBuild(const std::string& body)
{
    const size_t p = body.find("(file build ");
    if (p == std::string::npos || (p > 400 && !IsCompiled(body)))
        return "";
    const size_t e = body.find(')', p);
    if (e == std::string::npos || e - p > 40)
        return "";
    return body.substr(p + 12, e - p - 12);
}

// ---- the Lua 4.0 syntax check: the parser only, nothing runs ----
std::string g_luaError;

int CatchError(lua_State* L)
{
    const char* s = lua_tostring(L, 1);
    g_luaError = s ? s : "(no message)";
    return 0;
}

struct ParseOnly
{
    ZIO* z;
};

void ParseOnlyFn(lua_State* L, void* ud)
{
    luaY_parser(L, static_cast<ParseOnly*>(ud)->z);
}

// true = the text parses as Lua 4.0; else the parser's message in error
bool LuaSyntaxOk(const std::string& text, const std::string& chunkName, std::string& error)
{
    lua_State* L = lua_open(0);
    if (!L)
    {
        error = "out of memory";
        return false;
    }
    lua_register(L, LUA_ERRORMESSAGE, CatchError);
    g_luaError.clear();
    ZIO z;
    luaZ_mopen(&z, text.data(), text.size(), chunkName.c_str());
    ParseOnly p = {&z};
    const int status = luaD_runprotected(L, ParseOnlyFn, &p);
    lua_close(L);
    if (status == 0)
        return true;
    error = g_luaError.empty() ? "a syntax error (no message)" : g_luaError;
    while (!error.empty() && (error.back() == '\n' || error.back() == '\r'))
        error.pop_back();
    return false;
}

// a compiled chunk made for the game's Lua: Lua 4.0, little-endian, int / size_t / Instruction 4 bytes, 32-bit
// instructions, OP 6 bits, B 9 bits, 4-byte numbers (float) - GOG 1.3's undump header check (0x66AD90)
bool CompiledForTheGame(const std::string& b)
{
    static const unsigned char kHead[] = {0x1B, 'L', 'u', 'a', 0x40, 1, 4, 4, 4, 32, 6, 9, 4};
    return b.size() > sizeof(kHead) + 4 && memcmp(b.data(), kHead, sizeof(kHead)) == 0;
}

// ---- writing: the game's chunk format ----
void Put32(std::string& out, uint32_t v)
{
    const char b[4] = {static_cast<char>(v & 0xFF), static_cast<char>((v >> 8) & 0xFF), static_cast<char>((v >> 16) & 0xFF),
                       static_cast<char>((v >> 24) & 0xFF)};
    out.append(b, 4);
}

void PutChunk(std::string& out, const char* tag, const std::string& data)
{
    out.append(tag, 4);
    Put32(out, static_cast<uint32_t>(data.size()));
    out += data;
    while (out.size() % 4)
        out.push_back('\0');
}

std::string BuildLvl(const std::vector<Script>& scripts)
{
    std::string top;
    for (const Script& s : scripts)
    {
        std::string scr;
        PutChunk(scr, "NAME", s.name + std::string(1, '\0'));
        PutChunk(scr, "BODY", s.body + std::string(1, '\0'));   // the game runs size - 1: the 0 is not part of the script
        PutChunk(top, "scr_", scr);
    }
    std::string out = "ucfb";
    Put32(out, static_cast<uint32_t>(top.size()));
    return out + top;
}

// ---- reading: as the game's level loader does (every child at a 4-byte boundary; in a scr_ only NAME and BODY) ----
uint32_t Get32(const std::string& d, size_t p)
{
    return static_cast<uint32_t>(static_cast<unsigned char>(d[p])) | (static_cast<uint32_t>(static_cast<unsigned char>(d[p + 1])) << 8) |
           (static_cast<uint32_t>(static_cast<unsigned char>(d[p + 2])) << 16) |
           (static_cast<uint32_t>(static_cast<unsigned char>(d[p + 3])) << 24);
}

struct Child
{
    std::string tag;
    size_t at;
    size_t size;
};

// the children of the chunk data [lo, hi); false = a child runs past hi (a broken file)
bool Children(const std::string& d, size_t lo, size_t hi, std::vector<Child>& out)
{
    out.clear();
    size_t p = lo;
    for (;;)
    {
        p = (p + 3) & ~static_cast<size_t>(3);
        if (p >= hi || p + 8 > hi)
            return true;
        const size_t n = Get32(d, p + 4);
        if (n > hi - (p + 8))
            return false;
        out.push_back({d.substr(p, 4), p + 8, n});
        p += 8 + n;
    }
}

bool ReadLvl(const std::string& d, std::vector<Script>& out, std::string& error)
{
    out.clear();
    if (d.size() < 8 || d.compare(0, 4, "ucfb") != 0)
    {
        error = "not a .lvl (it does not start with \"ucfb\")";
        return false;
    }
    const size_t top = std::min<size_t>(d.size(), 8 + static_cast<size_t>(Get32(d, 4)));
    std::vector<Child> kids;
    if (!Children(d, 8, top, kids))
    {
        error = "broken: a chunk runs past the end of the file";
        return false;
    }
    for (const Child& k : kids)
    {
        if (k.tag != "scr_")
            continue;
        std::vector<Child> parts;
        if (!Children(d, k.at, k.at + k.size, parts))
        {
            error = "broken: a script chunk's part runs past its end";
            return false;
        }
        Script s;
        bool haveBody = false;
        for (const Child& c : parts)
        {
            if (c.tag == "NAME")
            {
                const std::string raw = d.substr(c.at, std::min<size_t>(c.size, 0x100));
                s.name = raw.substr(0, raw.find('\0'));
            }
            else if (c.tag == "BODY")
            {
                s.body = d.substr(c.at, c.size ? c.size - 1 : 0);
                haveBody = true;
            }
        }
        if (!haveBody)
            s.body.clear();
        out.push_back(s);
    }
    return true;
}

bool KnownToTheDll(const std::string& name)
{
    for (const char* k : kDllNames)
        if (Lower(name) == Lower(k))
            return true;
    return false;
}

void Banner()
{
    printf("ConsoleMunge (file build %s) - BF1GameExt's console.lvl munger for Star Wars Battlefront (2004)\n", kBuild);
    printf("Contains %s: %s\n\n", LUA_VERSION, LUA_COPYRIGHT);
}

void Usage()
{
    printf("Usage:\n"
           "  ConsoleMunge <scripts folder> <output .lvl>   pack every .lua in the folder (each checked with Lua 4.0's\n"
           "                                                parser first; nothing is written if one does not parse)\n"
           "  ConsoleMunge -list <file.lvl>                 list the scripts in a .lvl\n"
           "  ConsoleMunge -extract <file.lvl> <folder>     write a .lvl's scripts out as .lua files\n"
           "\n"
           "BF1GameExt.dll reads GameData\\Data\\_LVL_PC\\console.lvl and runs these scripts from it:\n"
           "  BF1GameExt_split, BF1GameExt_pause34, BF1GameExt_ctrlpage (the .lua file names without .lua)\n");
}

int Pack(const std::string& dir, const std::string& outPath)
{
    std::vector<std::string> files;
    if (!LuaFiles(dir, files))
    {
        printf("ERROR: no folder \"%s\"\n", dir.c_str());
        return 1;
    }
    if (files.empty())
    {
        printf("ERROR: no .lua files in \"%s\" - nothing written\n", dir.c_str());
        return 1;
    }
    std::vector<Script> scripts;
    int errors = 0;
    for (const std::string& f : files)
    {
        const std::string path = JoinPath(dir, f);
        Script s;
        s.name = f.substr(0, f.size() - 4);
        if (s.name.size() > 255)
        {
            printf("ERROR: %s - the name is longer than the game's 255 characters\n", f.c_str());
            ++errors;
            continue;
        }
        if (!ReadWhole(path, s.body, kMaxScript))
        {
            printf("ERROR: %s - cannot read it (or it is over 4 MB)\n", f.c_str());
            ++errors;
            continue;
        }
        bool bom = false;
        if (s.body.size() >= 3 && s.body.compare(0, 3, "\xEF\xBB\xBF") == 0)
        {
            s.body.erase(0, 3);   // the game's Lua does not take a UTF-8 BOM
            bom = true;
        }
        if (s.body.empty())
        {
            printf("ERROR: %s - empty\n", f.c_str());
            ++errors;
            continue;
        }
        const bool compiled = static_cast<unsigned char>(s.body[0]) == 0x1B;
        std::string kind;
        if (compiled)
        {
            if (!CompiledForTheGame(s.body))
            {
                printf("ERROR: %s - compiled Lua, but not for the game's Lua (it takes Lua 4.0, 32-bit, float numbers)\n",
                       f.c_str());
                ++errors;
                continue;
            }
            kind = "compiled Lua 4.0";
        }
        else
        {
            if (s.body.find('\0') != std::string::npos)
            {
                printf("ERROR: %s - has a 0 byte in it (not a text file?)\n", f.c_str());
                ++errors;
                continue;
            }
            std::string error;
            if (!LuaSyntaxOk(s.body, "@" + f, error))
            {
                printf("ERROR: %s does not parse as Lua 4.0:\n  %s\n", f.c_str(), error.c_str());
                ++errors;
                continue;
            }
            kind = "Lua text, parses";
        }
        const std::string build = FileBuild(s.body);
        printf("  %-24s %8lu bytes  %s%s%s%s\n", s.name.c_str(), static_cast<unsigned long>(s.body.size()), kind.c_str(),
               build.empty() ? "" : ", file build ", build.c_str(), bom ? " (its UTF-8 BOM taken off)" : "");
        if (!KnownToTheDll(s.name))
            printf("    note: BF1GameExt.dll does not run a script named %s - it is packed, but nothing runs it\n",
                   s.name.c_str());
        scripts.push_back(s);
    }
    if (errors)
    {
        printf("\n%d error(s) - NOTHING WRITTEN (%s is left as it was)\n", errors, outPath.c_str());
        return 1;
    }
    const std::string lvl = BuildLvl(scripts);
    // read it back the way the game will before it replaces the old one
    std::vector<Script> back;
    std::string error;
    bool same = ReadLvl(lvl, back, error) && back.size() == scripts.size();
    for (size_t i = 0; same && i < back.size(); ++i)
        same = back[i].name == scripts[i].name && back[i].body == scripts[i].body;
    if (!same)
    {
        printf("\nERROR: the .lvl did not read back the same - NOTHING WRITTEN\n");
        return 1;
    }
    const std::string tmp = outPath + ".tmp";
    if (!WriteWhole(tmp, lvl) || !MoveOver(tmp, outPath))
    {
        remove(tmp.c_str());
        printf("\nERROR: cannot write %s (is the folder there? is the game running?)\n", outPath.c_str());
        return 1;
    }
    printf("\n%s written: %lu script(s), %lu bytes - read back as the game reads it: the same\n", outPath.c_str(),
           static_cast<unsigned long>(scripts.size()), static_cast<unsigned long>(lvl.size()));
    return 0;
}

int List(const std::string& path)
{
    std::string d;
    if (!ReadWhole(path, d, kMaxLvl))
    {
        printf("ERROR: cannot read %s\n", path.c_str());
        return 1;
    }
    std::vector<Script> scripts;
    std::string error;
    if (!ReadLvl(d, scripts, error))
    {
        printf("ERROR: %s - %s\n", path.c_str(), error.c_str());
        return 1;
    }
    printf("%s: %lu bytes, %lu script(s)\n", path.c_str(), static_cast<unsigned long>(d.size()),
           static_cast<unsigned long>(scripts.size()));
    for (const Script& s : scripts)
    {
        const bool compiled = IsCompiled(s.body);
        const std::string build = FileBuild(s.body);
        printf("  %-24s %8lu bytes  %s%s%s%s\n", s.name.c_str(), static_cast<unsigned long>(s.body.size()),
               compiled ? "compiled Lua" : "Lua text", build.empty() ? "" : ", file build ", build.c_str(),
               KnownToTheDll(s.name) ? "" : "  (BF1GameExt.dll does not run this one)");
    }
    return 0;
}

int Extract(const std::string& path, const std::string& dir)
{
    std::string d;
    if (!ReadWhole(path, d, kMaxLvl))
    {
        printf("ERROR: cannot read %s\n", path.c_str());
        return 1;
    }
    std::vector<Script> scripts;
    std::string error;
    if (!ReadLvl(d, scripts, error))
    {
        printf("ERROR: %s - %s\n", path.c_str(), error.c_str());
        return 1;
    }
    int errors = 0;
    for (const Script& s : scripts)
    {
        bool safe = !s.name.empty();
        for (char c : s.name)
            if (c == '\\' || c == '/' || c == ':' || static_cast<unsigned char>(c) < 32)
                safe = false;
        if (!safe)
        {
            printf("  skipped a script named \"%s\" (not a file name)\n", s.name.c_str());
            ++errors;
            continue;
        }
        const bool compiled = !s.body.empty() && static_cast<unsigned char>(s.body[0]) == 0x1B;
        const std::string out = JoinPath(dir, s.name + (compiled ? ".luac" : ".lua"));
        if (!WriteWhole(out, s.body))
        {
            printf("  ERROR: cannot write %s\n", out.c_str());
            ++errors;
            continue;
        }
        printf("  %s (%lu bytes)\n", out.c_str(), static_cast<unsigned long>(s.body.size()));
    }
    return errors ? 1 : 0;
}
}   // namespace

int main(int argc, char** argv)
{
    Banner();
    if (argc == 3 && strcmp(argv[1], "-list") == 0)
        return List(argv[2]);
    if (argc == 4 && strcmp(argv[1], "-extract") == 0)
        return Extract(argv[2], argv[3]);
    if (argc == 3 && argv[1][0] != '-')
        return Pack(argv[1], argv[2]);
    Usage();
    return 2;
}
