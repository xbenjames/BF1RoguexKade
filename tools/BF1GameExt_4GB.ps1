# BF1GameExt_4GB.ps1   (file build p22e-r2)
#
# r2: the Steam exe is refused for ON - Steam's wrapper will not start an exe with the flag ("Application load error
# 3:0000065432", Mr. Rogue's test). OFF still puts a changed Steam exe back. GOG 1.3 as before.
#
# OPTIONAL: the 4 GB flag on Star Wars Battlefront (2004)'s Battlefront.exe - for EGM's maps at full detail.
#
# Windows gives a 32-bit game 2 GB of address space unless its exe says it can use more (the "large address aware" flag,
# one bit in the exe's header); with the flag, 64-bit Windows gives it 4 GB. EGM's maps need it: Direct3D keeps a copy of
# every texture in that space, and EGM's are big (BF1GameExt 22c's memory log: over 1 GB of textures on one map). Without
# the flag, BF1GameExt loads EGM's biggest textures at 1024 ([Limits] MaxTextureSize) so the maps fit; with it, at full size
# ([Limits] MaxTextureSize4GB). BF1GameExt cannot set the flag itself: Windows reads it when it starts the game, before
# BF1GameExt is loaded.
#
# What this does: recognises the exe (GOG 1.3 or Steam 1.3, the builds BF1GameExt supports - by its header's build stamp and
# size), keeps a copy of it as Battlefront.exe.no4gb, sets the flag and puts the header's checksum right. Nothing else in the
# exe changes. OFF puts the kept copy back (or clears the flag). Run it with the game closed.
#
# Steam: Steam's own wrapper on the exe refuses a changed exe ("Application load error 3:0000065432"), so ON is not offered
# for the Steam exe - there BF1GameExt's texture cap ([Limits] MaxTextureSize) keeps EGM's maps running in 2 GB. OFF still
# puts back a Steam exe changed by an earlier version (or use Steam's "Verify integrity of game files"). GOG 1.3 has no
# wrapper: the flag simply works.
#
# BF1GameExt's log says which you have: "[mem] 22c: the exe is LARGE-ADDRESS AWARE" / "not large-address aware".
#
# Use: double-click BF1GameExt_4GB.bat (next to Battlefront.exe), or:
#   powershell -ExecutionPolicy Bypass -File BF1GameExt_4GB.ps1 [-Action On|Off|Status] [-Exe <path to Battlefront.exe>]

param(
    [string]$Action = '',
    [string]$Exe = ''
)

$ErrorActionPreference = 'Stop'
$FileBuild = 'p22e-r2'
$LargeAddressAware = 0x0020

# the builds BF1GameExt raises the limits on: header build stamp, image size -> name
$Known = @(
    @{ Stamp = [uint32]0x5F24321D; Size = [uint32]0x021BB000; Name = 'GOG 1.3 (2020)' },
    @{ Stamp = [uint32]0x5F243336; Size = [uint32]0x02224000; Name = 'Steam 1.3 (2020)' }
)

if (-not ('Bf1GameExtPe' -as [type])) {
    Add-Type -TypeDefinition @'
public static class Bf1GameExtPe
{
    // the PE header checksum (as Windows' CheckSumMappedFile): 16-bit sum with carries folded, its own field left out,
    // plus the file's length
    public static uint CheckSum(byte[] b, int sumOfs)
    {
        ulong sum = 0;
        int n = b.Length;
        for (int i = 0; i + 1 < n; i += 2)
        {
            if (i >= sumOfs && i < sumOfs + 4)
                continue;
            sum += (ulong)(b[i] | (b[i + 1] << 8));
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        if ((n & 1) != 0)
        {
            sum += b[n - 1];
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        sum = (sum & 0xFFFF) + (sum >> 16);
        return (uint)(sum + (ulong)n);
    }

    // the same file apart from the flag (2 bytes) and the checksum (4 bytes)?
    public static bool SameApartFromFlag(byte[] a, byte[] b, int charOfs, int sumOfs)
    {
        if (a.Length != b.Length)
            return false;
        for (int i = 0; i < a.Length; i++)
        {
            if (a[i] == b[i])
                continue;
            if ((i >= charOfs && i < charOfs + 2) || (i >= sumOfs && i < sumOfs + 4))
                continue;
            return false;
        }
        return true;
    }
}
'@
}

function Write-Line([string]$Text, [string]$Color = '') {
    if ($Color) { Write-Host $Text -ForegroundColor $Color } else { Write-Host $Text }
}

# the header fields this needs; $null if it is not a 32-bit Windows exe
function Get-PeInfo([byte[]]$Bytes) {
    if ($Bytes.Length -lt 0x400 -or $Bytes[0] -ne 0x4D -or $Bytes[1] -ne 0x5A) { return $null }
    $pe = [BitConverter]::ToInt32($Bytes, 0x3C)
    if ($pe -lt 0x40 -or $pe + 0x60 -gt $Bytes.Length) { return $null }
    if ([BitConverter]::ToUInt32($Bytes, $pe) -ne 0x00004550) { return $null }      # "PE\0\0"
    if ([BitConverter]::ToUInt16($Bytes, $pe + 4) -ne 0x014C) { return $null }      # x86
    if ([BitConverter]::ToUInt16($Bytes, $pe + 24) -ne 0x010B) { return $null }     # PE32
    $info = @{
        CharOfs = $pe + 22
        SumOfs  = $pe + 24 + 64
        Stamp   = [BitConverter]::ToUInt32($Bytes, $pe + 8)
        Size    = [BitConverter]::ToUInt32($Bytes, $pe + 24 + 56)
    }
    $info.Char = [BitConverter]::ToUInt16($Bytes, $info.CharOfs)
    $info.Sum = [BitConverter]::ToUInt32($Bytes, $info.SumOfs)
    $info.On = ($info.Char -band $LargeAddressAware) -ne 0
    $info.Build = $null
    foreach ($k in $Known) {
        if ($k.Stamp -eq $info.Stamp -and $k.Size -eq $info.Size) { $info.Build = $k.Name }
    }
    return $info
}

# a copy of the bytes with the flag set or cleared and the checksum put right
function Set-Flag([byte[]]$Bytes, $Info, [bool]$On) {
    $b = [byte[]]$Bytes.Clone()
    $c = [int]$Info.Char
    if ($On) { $c = $c -bor $LargeAddressAware } else { $c = $c -band (0xFFFF -bxor $LargeAddressAware) }
    $b[$Info.CharOfs] = [byte]($c -band 0xFF)
    $b[$Info.CharOfs + 1] = [byte](($c -shr 8) -band 0xFF)
    if ($Info.Sum -ne 0) {
        $sum = [Bf1GameExtPe]::CheckSum($b, $Info.SumOfs)
        [Array]::Copy([BitConverter]::GetBytes([uint32]$sum), 0, $b, $Info.SumOfs, 4)
    }
    return , $b
}

# written to a temporary file first, then copied over the exe (fails cleanly while the game is running)
function Write-Exe([string]$Path, [byte[]]$Bytes) {
    $tmp = $Path + '.bf1gameext-tmp'
    [System.IO.File]::WriteAllBytes($tmp, $Bytes)
    try {
        [System.IO.File]::Copy($tmp, $Path, $true)
    } finally {
        Remove-Item -LiteralPath $tmp -Force -ErrorAction SilentlyContinue
    }
}

function Find-Exe([string]$Given) {
    if ($Given) { return $Given }
    $here = Split-Path -Parent $PSCommandPath
    foreach ($p in @((Join-Path $here 'Battlefront.exe'), (Join-Path $here 'GameData\Battlefront.exe'))) {
        if (Test-Path -LiteralPath $p) { return $p }
    }
    return (Join-Path $here 'Battlefront.exe')
}

Write-Line ''
Write-Line "Star Wars Battlefront (2004) - the 4 GB flag, for EGM's maps at full detail (BF1GameExt $FileBuild, optional)" 'Cyan'
$exePath = Find-Exe $Exe
if (-not (Test-Path -LiteralPath $exePath)) {
    Write-Line "Battlefront.exe was not found at $exePath. Put this file next to Battlefront.exe (where dinput8.dll is)," 'Red'
    Write-Line "or run it with -Exe <path to Battlefront.exe>." 'Red'
    exit 1
}
$exePath = (Resolve-Path -LiteralPath $exePath).Path
$backup = $exePath + '.no4gb'

try {
    $bytes = [System.IO.File]::ReadAllBytes($exePath)
} catch {
    Write-Line "Could not read $exePath - $($_.Exception.Message)" 'Red'
    exit 1
}
$info = Get-PeInfo $bytes
if (-not $info) {
    Write-Line "$exePath is not a 32-bit Windows exe - nothing changed." 'Red'
    exit 1
}
Write-Line "Game: $exePath"
if (-not $info.Build) {
    Write-Line ("This exe is not GOG 1.3 or Steam 1.3 (build stamp 0x{0:X8}, size 0x{1:X8}) - the builds BF1GameExt supports. Nothing changed." -f $info.Stamp, $info.Size) 'Red'
    exit 1
}
$state = 'OFF (2 GB)'
if ($info.On) { $state = 'ON (4 GB)' }
Write-Line "Build: $($info.Build)    4 GB flag: $state"
if (Test-Path -LiteralPath $backup) { Write-Line "Kept copy of the original: $backup" }

$act = $Action
if (-not $act) {
    Write-Line ''
    Write-Line '  1 = turn the 4 GB flag ON  (EGM''s maps at full detail; the original is kept as Battlefront.exe.no4gb)'
    Write-Line '  2 = turn it OFF            (the original put back)'
    Write-Line '  Q = leave it as it is'
    $answer = Read-Host 'Choose'
    switch -Regex ($answer) {
        '^\s*1\s*$' { $act = 'On' }
        '^\s*2\s*$' { $act = 'Off' }
        default { $act = 'Status' }
    }
}

try {
    switch ($act.ToLowerInvariant()) {
        'on' {
            if ($info.Build -like 'Steam*' -and -not $info.On) {
                Write-Line 'Not on the Steam exe: Steam''s wrapper will not start an exe with the flag ("Application load error' 'Yellow'
                Write-Line '3:0000065432"). BF1GameExt''s texture cap ([Limits] MaxTextureSize=1024) keeps EGM''s maps running in 2 GB.' 'Yellow'
                Write-Line 'EGM at full detail needs the GOG version. Nothing changed.' 'Yellow'
                break
            }
            if ($info.On) {
                Write-Line 'The 4 GB flag is already ON - nothing to do.' 'Green'
                break
            }
            # the original (this exe, without the flag) kept first
            [System.IO.File]::WriteAllBytes($backup, $bytes)
            $new = Set-Flag $bytes $info $true
            Write-Exe $exePath $new
            $check = Get-PeInfo ([System.IO.File]::ReadAllBytes($exePath))
            if (-not $check -or -not $check.On -or $check.Build -ne $info.Build) { throw 'the exe did not read back as expected' }
            Write-Line 'The 4 GB flag is ON. Start the game as usual: BF1GameExt''s log will say "LARGE-ADDRESS AWARE".' 'Green'
        }
        'off' {
            if (-not $info.On) {
                Write-Line 'The 4 GB flag is already OFF - nothing to do.' 'Green'
                break
            }
            $restored = $false
            if (Test-Path -LiteralPath $backup) {
                $old = [System.IO.File]::ReadAllBytes($backup)
                $oldInfo = Get-PeInfo $old
                if ($oldInfo -and -not $oldInfo.On -and $oldInfo.Build -eq $info.Build -and [Bf1GameExtPe]::SameApartFromFlag($old, $bytes, $info.CharOfs, $info.SumOfs)) {
                    Write-Exe $exePath $old
                    $restored = $true
                    Write-Line "The original exe is back (from $backup)." 'Green'
                }
            }
            if (-not $restored) {
                Write-Exe $exePath (Set-Flag $bytes $info $false)
                Write-Line 'The 4 GB flag is OFF (cleared in the exe - no matching kept copy).' 'Green'
            }
            $check = Get-PeInfo ([System.IO.File]::ReadAllBytes($exePath))
            if (-not $check -or $check.On) { throw 'the exe did not read back as expected' }
        }
        default {
            Write-Line 'Nothing changed.'
        }
    }
} catch {
    Write-Line "Nothing more was changed: $($_.Exception.Message)" 'Red'
    Write-Line 'Is the game still running? Close it first. If Windows denies access, run this as administrator.' 'Red'
    exit 1
}
exit 0
