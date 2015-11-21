package require Tk

source kolEdit.tcl

##### GUI helpers
proc buildGui {} {
	foreach c $kol::chars {
		set firstByteIndex [string first \0 $c]

		if {$firstByteIndex > 0} {
			.t.fL.lChars insert end [string range $c 0 $firstByteIndex-1]
		}
	}
}

##### GUI commands
proc doOpen {} {
	set ofile [tk_getOpenFile]
	if {$ofile eq ""} { return }

	wm title .t "KOL Editor - [file tail $ofile]"
	kol::readFile $ofile

	buildGui
}

proc doSave {} {
	# TODO
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

### right frame (details)
pack [frame .t.fR] -side left -fill y -expand 1 -anchor w

for {set i 0} {$i < 4} {incr i} {
	grid [label .t.fR.lName$i -text "Skill $i"] -row $i -column 0 -sticky w
	grid [label .t.fR.lOff$i -text "Offense"] -row $i -column 1 -sticky w
	grid [label .t.fR.lDef$i -text "Defense"] -row $i -column 2 -sticky w
}

bind .t <Destroy> {exit}
