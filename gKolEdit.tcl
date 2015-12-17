package require Tk

source kolEdit.tcl

##### GUI helpers
proc showChar {sel} {
	for {set i 0} {$i < 4} {incr i} {
		set s [kol::skill $sel $i]
		.t.fR.lName$i configure -text [lindex $s 0]
		.t.fR.lOff$i configure -text [lindex $s 1]
		.t.fR.lDef$i configure -text [lindex $s 2]
	}

	.t.fR.vAp configure -text [kol::ap $sel]
	.t.fR.vGold configure -text [kol::gold $sel]
}

proc showSelectedChar {w} {
	set sel [$w curselection]
	if {$sel eq ""} { return }

	showChar $sel
}

proc buildGui {} {
	.t.fL.lChars delete 0 end

	for {set i 0} {$i < [llength $kol::chars]} {incr i} {
		set name [kol::name $i]
		if {$name ne ""} {
			.t.fL.lChars insert end $name
		}
	}
}

##### GUI commands
proc doOpen {} {
	set ofile [tk_getOpenFile]
	if {$ofile eq ""} { return }

	set kol::filename [file tail $ofile]
	wm title .t "KOL Editor - $kol::filename"
	kol::readFile $ofile

	buildGui
}

proc doSave {} {
	set f [tk_getSaveFile -initialfile $kol::filename]
	if {$f eq ""} { return }

	kol::writeFile $f
}

proc doTrain {w i} {
	set sel [$w curselection]
	if {$sel eq ""} { return }

	set ap [kol::ap $sel]
	if {$ap < 500} { return }

	set skill [kol::skill $sel $i]
	set off [lindex $skill 1]
	set def [lindex $skill 2]

	if {$def < $off} {
		incr def 5
		lset skill 2 $def
	} else {
		incr off 5
		lset skill 1 $off
	}
	kol::setSkill $sel $i $skill

	incr ap -500
	kol::setAp $sel $ap

	showChar $sel
}

##### GUI
wm withdraw .
toplevel .t
wm title .t "KOL Editor"

### menu
menu .mTopMenu -tearoff 0
menu .mTopMenu.mFile -tearoff 0

.mTopMenu add cascade -label "File" -menu .mTopMenu.mFile -underline 0

# file menu
.mTopMenu.mFile add command -label "Open" -command doOpen -underline 0 -accelerator "Ctrl+O"
.mTopMenu.mFile add command -label "Save" -command doSave -underline 0 -accelerator "Ctrl+S"
.mTopMenu.mFile add separator
.mTopMenu.mFile add command -label "Exit" -command exit   -underline 1 -accelerator "Ctrl+Q"

.t configure -menu .mTopMenu

### left frame (character list)
pack [frame .t.fL] -side left -fill y -expand 1 -anchor w
pack [listbox .t.fL.lChars] -anchor w -fill y -expand 1

bind .t.fL.lChars <<ListboxSelect>> {showSelectedChar %W}

### right frame (details)
pack [frame .t.fR] -side left -fill y -expand 1 -anchor w

for {set i 0} {$i < 4} {incr i} {
	grid [label .t.fR.lName$i -text "Skill $i"] -row $i -column 0 -sticky w
	grid [label .t.fR.lOff$i -text "Offense"] -row $i -column 1 -sticky w
	grid [label .t.fR.lDef$i -text "Defense"] -row $i -column 2 -sticky w
	grid [button .t.fR.bTrain$i -text "Train" -command [list doTrain .t.fL.lChars $i]] -row $i -column 3 -sticky w
}

grid [label .t.fR.lAp -text "AP"] -row 4 -column 0 -sticky w
grid [label .t.fR.vAp -text ""] -row 4 -column 1 -columnspan 2

grid [label .t.fR.lGold -text "Gold"] -row 5 -column 0 -sticky w
grid [label .t.fR.vGold -text ""] -row 5 -column 1 -columnspan 2

bind .t <Destroy> {exit}

