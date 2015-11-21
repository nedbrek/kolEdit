namespace eval kol {
	variable chars
	variable weapons

	proc init {} {
		variable weapons
		dict set weapons [binary decode hex {81 49 e1 20 70 50 00 00 75}] "Broad Axe"
		dict set weapons [binary decode hex {a1 49 e1 24 ee f9 10 c0 5d}] "Broadsword"
		dict set weapons [binary decode hex {72 b0 c0 13 ee 00 00 00 0b}] "Elf Bow"
		dict set weapons [binary decode hex {a3 c8 a1 a4 ee f9 10 03 57}] "Greatsword"
		dict set weapons [binary decode hex {74 05 82 2c 88 00 00 00 7e}] "Halberd"
		dict set weapons [binary decode hex {86 3d c7 00 9f 70 00 00 0a}] "Longbow"
		dict set weapons [binary decode hex {96 3d c7 9d df 22 68 13 5c}] "Longsword"
		dict set weapons [binary decode hex {46 84 65 00 00 00 00 00 3c}] "Mace"
	}
	init

	proc readFile {ifilename} {
		variable chars
		set chars [list]

		set f [open $ifilename]
		fconfigure $f -translation binary
		set d [read $f 768]
		while {![eof $f]} {
			lappend chars $d
			set d [read $f 768]
		}
		close $f
	}

	proc writeFile {ofilename} {
		variable chars

		set f [open $ofilename w]
		fconfigure $f -translation binary
		foreach c $chars {
			puts -nonewline $f $c
		}
		close $f
	}

	proc valid {charNum} {
		return [expr [name $charNum] ne ""]
	}

	proc name {charNum} {
		variable chars
		set c [lindex $chars $charNum]

		set firstByteIndex [string first \0 $c]
		if {$firstByteIndex > 0} {
			return [string range $c 0 $firstByteIndex-1]
		}
		return ""
	}

	proc ap {charNum} {
		variable chars

		binary scan [string range [lindex $chars $charNum] 38 39] S ap
		return $ap
	}

	proc setAp {charNum newAp} {
		variable chars

		set c [lindex $chars $charNum]

		set newBin [binary format S $newAp]

		lset chars $charNum [string replace $c 38 39 $newBin]
	}

	proc gold {charNum} {
		variable chars

		binary scan [string range [lindex $chars $charNum] 42 44] cS top bot
		# TODO handle large values properly
		return [expr {$top * 65536 + $bot}]
	}

	proc skill {charNum skillNum} {
		variable chars
		variable weapons

		set c [lindex $chars $charNum]
		set i [expr {74 + $skillNum * 13}]

		set weaponCode [string range $c $i $i+8]
		if {![dict exists $weapons $weaponCode]} {
			return "None"
		}
		set weaponName [dict get $weapons $weaponCode]

		binary scan [string range $c $i+9  $i+10] S offense
		binary scan [string range $c $i+11 $i+12] S defense

		return [list $weaponName [expr {$offense/10}] [expr {$defense/10}]]
	}

	proc setSkill {charNum skillNum skillVal} {
		variable chars
		variable weapons

		set weaponCode ""
		dict for {k v} $weapons {
			if {$v eq [lindex $skillVal 0]} {
				set weaponCode $k
				break
			}
		}
		if {$weaponCode eq ""} { return "Error" }

		set off [expr {[lindex $skillVal 1] * 10}]
		set def [expr {[lindex $skillVal 2] * 10}]

		set newBin $weaponCode
		append newBin [binary format S2 [list $off $def]]

		set c [lindex $chars $charNum]
		set i [expr {74 + $skillNum * 13}]

		set c [string replace $c $i $i+12 $newBin]
		lset chars $charNum $c
	}
}

