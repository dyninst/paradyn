# main tool bar

# $Log: mainMenu.tcl,v $
# Revision 1.10  1994/07/07 18:17:25  karavan
# bug fix:  menu name specification error
#
# Revision 1.9  1994/07/07  05:57:05  karavan
# UIM error service implementation
#
# CVr: ----------------------------------------------------------------------
#
# Revision 1.8  1994/06/29  21:47:38  hollings
# killed old background colors and switched to motif like greys.
# cleaned up option specification to use options data base.
#
# Revision 1.7  1994/06/12  22:32:44  karavan
# implemented button update by status change callback.
#
# Revision 1.6  1994/05/30  19:22:26  hollings
# Removed debugging puts.
#
# Revision 1.5  1994/05/26  20:56:36  karavan
# changed location of bitmap file for logo.
#
# Revision 1.4  1994/05/23  01:55:41  karavan
# its a whole new look for paradyn!
#
# Revision 1.3  1994/05/06  06:42:13  karavan
# buttons now functional: Performance Consultant; RUN/PAUSE; Status
#
# Revision 1.2  1994/05/05  19:51:24  karavan
# added call to uimpd command for visi menu.
#
# Revision 1.1  1994/05/03  06:36:02  karavan
# Initial version.
#

#
# Copyright (c) 1993, 1994 Barton P. Miller, Jeff Hollingsworth,
#     Bruce Irvin, Jon Cargille, Krishna Kunchithapadam, Karen
#     Karavanic, Tia Newhall, Mark Callaghan.  All rights reserved.
# 
#  This software is furnished under the condition that it may not be
#  provided or otherwise made available to, or used by, any other
#  person, except as provided for by the terms of applicable license
#  agreements.  No title to or ownership of the software is hereby
#  transferred.  The name of the principals may not be used in any
#  advertising or publicity related to this software without specific,
#  written prior authorization.  Any use of this software must include
#  the above copyright notice.
#

## changeApplicState
## changes button status of "run" and "pause" buttons, so that opposite 
##  of current state can always be pressed but current state cannot.

proc changeApplicState {newVal} {
    if {$newVal} {
	.buttons.2 configure -state normal
	.buttons.1 configure -state disabled
    } else {
	.buttons.2 configure -state disabled
	.buttons.1 configure -state normal
    }
}

proc drawToolBar {} {

    global PdBitmapDir

    option add *background grey
    option add *Frame*bg red
    option add *Scrollbar*background DimGray

    option add *Scrollbar*foreground grey
    option add *activeBackground LightGrey
    option add *Scrollbar*activeBackground DimGray

    # . seems to be created before the options data base gets loaded.
    . config -bg grey

    wm geometry . =750x750

    frame .menub -relief raised -borderwidth 2
    frame .where -height 100
    frame .main -height 400 -width 400
    frame .buttons 
    mkButtonBar .buttons {} retval {{RUN "paradyn cont"} \
	    {PAUSE "paradyn pause"} {REPORT "paradyn status"} {SAVE ""} \
	    {EXIT "destroy ."}}
    .buttons.2 configure -state disabled

    frame .menub.left
    label .menub.left.title -text "Paradyn Main Control"
    frame .menub.left.men
    menubutton .menub.left.men.b1 -text "Setup" -menu .menub.left.men.b1.m 
    menubutton .menub.left.men.b3 -text "Metrics"
    menubutton .menub.left.men.b2 -text "Options"
    menubutton .menub.left.men.b5 -text "Start Visual" \
	    -menu .menub.left.men.b5.m 
    menubutton .menub.left.men.b6 -text "Help" 
    menu .menub.left.men.b5.m -postcommand \
	    {uimpd drawStartVisiMenu .menub.left.men.b5.m}
    menu .menub.left.men.b1.m 
    .menub.left.men.b1.m add command -label "Application Control" \
	    -command ApplicDefn
    .menub.left.men.b1.m add command -label "Start Perf Consultant" \
	    -command {paradyn shg start}
    .menub.left.men.b1.m add command -label "Options Control" \
	    -state disabled
    menu .menub.left.men.b2.m 
    .menub.left.men.b2.m add command -label "Error History" \
	    -command {showErrorHistory}
    wm title . "Paradyn"

    pack .menub .main  -side top -fill x
    pack .buttons -side bottom -fill x
    pack .where -side bottom -fill x

    pack .menub.left.men.b6 -side right -padx 10
    pack .menub.left.men.b1 .menub.left.men.b3 .menub.left.men.b2 \
	    .menub.left.men.b5 \
	    -side left -padx 10
    pack .menub.left -side left -fill x
    mkLogo .menub.logobox right

    pack .menub.left.title .menub.left.men -side top -fill x

    # read in error file

    uplevel #0 {source "$PdBitmapDir/errorList.tcl"}
    
    InitApplicDefnScreen 

}


