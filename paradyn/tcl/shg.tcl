# $Id: shg.tcl,v 1.16 1998/03/03 23:09:51 wylie Exp $

proc shgChangeCurrLabelHeight {numlines} {
   if {[winfo exists .shg.nontop.labelarea.current]} {
      .shg.nontop.labelarea.current config -height $numlines
      pack .shg.nontop.labelarea.current -side left -fill both -expand true
   }
}

proc redrawKeyAndTipsAreas {drawkeychange drawtipschange} {
   if {![winfo exists .shg]} {
      return
   }

   set keyArea .shg.nontop.keyarea
   set tipArea .shg.nontop.tiparea

   set keyIsUp [winfo exists $keyArea.left]
   set tipsAreUp [winfo exists $tipArea.tip0]

   set drawKey false
   if {$drawkeychange=="on"} {
      set drawKey true
   }
   if {$drawkeychange=="nc" && $keyIsUp} {
      set drawKey true
   }
 
   set drawTips false
   if {$drawtipschange=="on"} {
      set drawTips true
   }
   if {$drawtipschange=="nc" && $tipsAreUp} {
      set drawTips true
   }

   # Note: the following can be optimized for a single case:
   # if the key was already up and is staying up, then don't erase & redraw it:
   set dontTouchTheKey 0
   if {$keyIsUp && $drawkeychange=="nc"} {
      set dontTouchTheKey 1
   }

   # erase em both; important to erase the key first so its base is redrawn first
   # (i.e. above the base window for tips)!
   # The following routines will erase and then redraw the respective bases:

   if {!$dontTouchTheKey} {
      shgDrawKeyBase
   }
   shgDrawTipsBase

   # and redraw, as appropriate
   if {$drawKey} {
      if {! $dontTouchTheKey} {
	  shgDrawKey
      }
   }

   if {$drawTips} {
      shgDrawTips
   }
}

proc shgDrawKeyBase {} {
   # Erases (if necessary) and redraws the base
   set keyArea .shg.nontop.keyarea
   if {[winfo exists $keyArea]} {
       destroy $keyArea
   }

   frame $keyArea
   pack  $keyArea -side top -fill both
}

proc shgDrawKey {} {
   set leftLabels ".shg.nontop.keyarea.left"
   set rightLabels ".shg.nontop.keyarea.right"

   frame $leftLabels 
   pack  $leftLabels -side left -fill both -expand true

   frame $rightLabels 
   pack  $rightLabels -side right -fill both -expand true

   label $leftLabels.key0 -relief groove \
	   -text "Never Evaluated" -anchor c \
	   -font { Helvetica 12 } \
	   -background grey
   pack   $leftLabels.key0 -side top -fill x -expand false

   label $leftLabels.key1 -relief groove \
	   -text "Unknown" -anchor c \
	   -font { Helvetica 12 } \
	   -background #60c0a0
           # a nice green...
   pack   $leftLabels.key1 -side top -fill x -expand false
#	   -background "#e9fbb57aa3c9"

   label $leftLabels.key2 -relief groove \
	   -text "True" -anchor c \
	   -font { Helvetica 12 } \
	   -background cornflowerblue
#	   -background "#acbff48ff6c8"
                # yuck --ari
   pack   $leftLabels.key2 -side top -fill x -expand false

   label $leftLabels.key3 -relief groove \
	   -text "False" -anchor c \
	   -font { Helvetica 12 } \
	   -background pink
#	   -background "#cc85d5c2777d" 
                # yuck --ari
   pack   $leftLabels.key3 -side top -fill x -expand false

   frame $leftLabels.key4 -relief groove -borderwidth 2
   pack  $leftLabels.key4 -side top -fill x -expand false

   canvas $leftLabels.key4.canvas -height 20 -width 90 -relief flat -highlightthickness 0
   pack   $leftLabels.key4.canvas -side left -fill y -expand false
   $leftLabels.key4.canvas create line 10 5  80 15 \
	   -fill palegoldenrod -width 2

   label  $leftLabels.key4.label -relief flat \
	   -text "Why Axis Refinement" -anchor c \
	   -font { Helvetica 12 }
   pack   $leftLabels.key4.label -side right -fill y -expand true



   label $rightLabels.key0 -relief groove \
	   -font { Helvetica 12 } \
	   -text "instrumented" \
	   -foreground ivory \
	   -background gray
   pack  $rightLabels.key0 -side top -fill x

   label $rightLabels.key1 -relief groove \
	   -font { Helvetica 12 } \
	   -text "uninstrumented" \
	   -foreground black \
	   -background gray
   pack  $rightLabels.key1 -side top -fill x

   label $rightLabels.key2 -relief groove \
	   -font { Helvetica 12 italic } \
	   -text "instrumented; shadow node" \
	   -foreground ivory \
	   -background gray
   pack  $rightLabels.key2 -side top -fill x

   label $rightLabels.key3 -relief groove \
	   -font { Helvetica 12 italic } \
	   -text "uninstrumented; shadow node" \
	   -foreground black \
	   -background gray
   pack  $rightLabels.key3 -side top -fill x

   frame $rightLabels.key4 -relief groove -borderwidth 2
   pack  $rightLabels.key4 -side top -fill x -expand false

   canvas $rightLabels.key4.canvas -height 20 -width 90 -relief flat -highlightthickness 0
   pack   $rightLabels.key4.canvas -side left -fill y -expand false
   $rightLabels.key4.canvas create line 10 5  80 15 \
	   -fill orchid -width 2

   label  $rightLabels.key4.label -relief flat \
	   -text "Where Axis Refinement" -anchor c \
	   -font { Helvetica 12 }
   pack   $rightLabels.key4.label -side right -fill y -expand true

}

# ####################################################################

proc shgDrawTipsBase {} {
   # Erases (if necessary) and redraws the base
   set tipArea .shg.nontop.tiparea
   if {[winfo exists $tipArea]} {
       destroy $tipArea
   }

   frame $tipArea
   pack  $tipArea -side top -fill x
}

proc shgDrawTips {} {
   set tipArea .shg.nontop.tiparea

   label $tipArea.tip0 -relief groove \
	   -text "Hold down Alt and move the mouse to scroll freely" \
	   -font { Helvetica 12 }
   pack $tipArea.tip0 -side top -fill both
      # fill both prevents shrinking when window is made shorter

   label $tipArea.tip1 -relief groove \
	   -text "Click middle button on a node to obtain more info on it" \
	   -font { Helvetica 12 }
   pack $tipArea.tip1 -side top -fill both
      # fill both prevents shrinking when window is made shorter

}

# ####################################################################

proc shgClickOnSearch {} {
   set buttonarea .shg.nontop.buttonarea
   if {[shgSearchCommand]} {
      $buttonarea.left.search config -state disabled -text "Resume"
      $buttonarea.middle.pause config -state normal
   }
}

proc shgClickOnPause {} {
   set buttonarea .shg.nontop.buttonarea
   if {[shgPauseCommand]} {
      $buttonarea.left.search config -state normal
      $buttonarea.middle.pause config -state disabled
   }
}

# ####################################################################

proc shgInitialize {iDeveloperMode iDrawKey iDrawTips} {
   # paradyn's entry point should be here; the shg test program
   # will call shgInitialize2 directly, since the paradyn command (next line)
   # would bomb it
   if {![paradyn applicationDefined]} {
      # No application has been started yet!!!
      # Put up an error dialog:
      showError 88 {}
      return
   }

   return [shgInitialize2 $iDeveloperMode $iDrawKey $iDrawTips]
}

proc shgInitialize2 {iDeveloperMode iDrawKey iDrawTips} {
   if {[winfo exists .shg]} {
#      puts stderr "(shg window already exists; not creating)"
      wm deiconify .shg
      raise .shg
      return
   }

   toplevel .shg -class "Shg"
   option add *shg*Background grey
   option add *shg*activeBackground LightGrey
   option add *shg*activeForeground black
   wm protocol .shg WM_DELETE_WINDOW {wm iconify .shg}

   # area for title, menubar, logo
   frame .shg.titlearea
   pack  .shg.titlearea -side top -fill x -expand false -anchor n

   frame .shg.titlearea.right
   pack  .shg.titlearea.right -side right -fill y -expand false

   makeLogo .shg.titlearea.right.logo paradynLogo raised 2 mediumseagreen
   pack  .shg.titlearea.right.logo -side top

   frame .shg.titlearea.left
   pack  .shg.titlearea.left -side left -fill both -expand true

   label .shg.titlearea.left.title -text "The Performance Consultant" \
	   -foreground white -anchor c \
           -font { Times 14 bold } -relief raised \
	   -background mediumseagreen
   pack  .shg.titlearea.left.title -side top -fill both -expand true

   # area for menubar:
   frame .shg.titlearea.left.menu
   pack  .shg.titlearea.left.menu -side top -fill x -expand false -anchor n

   frame .shg.titlearea.left.menu.mbar -borderwidth 2 -relief raised
   pack  .shg.titlearea.left.menu.mbar -side top -fill both -expand false

   menubutton .shg.titlearea.left.menu.mbar.phase -text Searches -menu .shg.titlearea.left.menu.mbar.phase.m
   menu .shg.titlearea.left.menu.mbar.phase.m -selectcolor cornflowerblue

   pack .shg.titlearea.left.menu.mbar.phase -side left -padx 4

   # -----------------------------------------------------------

   frame .shg.nontop
   pack  .shg.nontop -side bottom -fill both -expand true

   # -----------------------------------------------------------

   frame .shg.nontop.currphasearea
   pack  .shg.nontop.currphasearea -side top -fill x -expand false

   label .shg.nontop.currphasearea.label1 -text "Current Search: " \
	   -font { Helvetica 12 } -anchor e
   pack  .shg.nontop.currphasearea.label1 -side left -fill both -expand true

   label .shg.nontop.currphasearea.label2 -text "" \
	   -font { Helvetica 12 } -anchor w
   pack  .shg.nontop.currphasearea.label2 -side left -fill both -expand true

   # -----------------------------------------------------------

   frame .shg.nontop.textarea
   pack  .shg.nontop.textarea -side top -fill x -expand false

   text .shg.nontop.textarea.text -borderwidth 2 -width 40 -height 5 -relief sunken \
	   -font { Helvetica 12 } \
	   -yscrollcommand ".shg.nontop.textarea.sb set" \
	   -selectbackground black
   pack .shg.nontop.textarea.text -side left -fill both -expand true

   scrollbar .shg.nontop.textarea.sb -relief sunken \
	   -command ".shg.nontop.textarea.text yview"
   pack .shg.nontop.textarea.sb -side right -fill y -expand false

   # -----------------------------------------------------------

   frame .shg.nontop.main -width 4i -height 3.5i
   pack  .shg.nontop.main -side top -fill both -expand true

   scrollbar .shg.nontop.main.leftsb -orient vertical -width 16 \
	   -background gray \
	   -activebackground gray \
	   -command "shgNewVertScrollPosition"

   pack .shg.nontop.main.leftsb -side left -fill y -expand false

   scrollbar .shg.nontop.main.bottsb -orient horizontal -width 16 \
        -activebackground gray \
        -command "shgNewHorizScrollPosition"

   pack .shg.nontop.main.bottsb -side bottom -fill x -expand false

   frame .shg.nontop.main.all -relief flat -width 3i -height 2i
   pack .shg.nontop.main.all -side left -fill both -expand true

   # -----------------------------------------------------------

   frame .shg.nontop.labelarea
   pack  .shg.nontop.labelarea -side top -fill x -expand false

   text .shg.nontop.labelarea.current -relief sunken -height 1 \
	   -font { Helvetica 12 } -wrap none
   if {$iDeveloperMode} {
      set numlines 4
   } else {
      set numlines 1
   }
   shgChangeCurrLabelHeight $numlines

   # -----------------------------------------------------------

   frame .shg.nontop.buttonarea
   pack  .shg.nontop.buttonarea -side top -fill x -expand false

   frame .shg.nontop.buttonarea.left
   pack  .shg.nontop.buttonarea.left -side left -fill y -expand true

   # Note: the search button doubles as a "Resume" button when appropriate
   button .shg.nontop.buttonarea.left.search -text "Search" -anchor c \
	   -command shgClickOnSearch
   pack   .shg.nontop.buttonarea.left.search -side left -ipadx 10 -fill y -expand false

   frame .shg.nontop.buttonarea.middle
   pack  .shg.nontop.buttonarea.middle -side left -fill y -expand true

   button .shg.nontop.buttonarea.middle.pause -text "Pause" -state disabled -anchor c \
	   -command shgClickOnPause
   pack   .shg.nontop.buttonarea.middle.pause -side left -fill y -expand false

   # -----------------------------------------------------------

   shgDrawKeyBase
   if {$iDrawKey} {
      shgDrawKey
   }

   shgDrawTipsBase
   if {$iDrawTips} {
      shgDrawTips
   }

   # -----------------------------------------------------------

   bind .shg.nontop.main.all <Configure> {shgConfigureHook}
   bind .shg.nontop.main.all <Expose>    {shgExposeHook %c}
   bind .shg.nontop.main.all <Button-1>  {shgSingleClickHook %x %y}
   bind .shg.nontop.main.all <Button-2>  {shgMiddleClickHook %x %y}
   bind .shg.nontop.main.all <Double-Button-1> {shgDoubleClickHook %x %y}
#   bind .shg.nontop.main.all <Shift-Double-Button-1> {shgShiftDoubleClickHook %x %y}
#   bind .shg.nontop.main.all <Control-Double-Button-1> {shgCtrlDoubleClickHook %x %y}
   bind .shg.nontop.main.all <Alt-Motion> {shgAltPressHook %x %y}
   bind .shg.nontop.main.all <Motion> {shgAltReleaseHook}
}
