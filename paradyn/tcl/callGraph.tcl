#This is used to create the actual call graph window. 
# $Id: callGraph.tcl,v 1.2 1999/07/13 16:51:00 cain Exp $

proc callGraphChangeCurrLabelHeight {numlines} {
   if {[winfo exists .callGraph.nontop.labelarea.current]} {
      .callGraph.nontop.labelarea.current config -height $numlines
      pack .callGraph.nontop.labelarea.current -side left -fill both -expand true
   }
}

# ####################################################################

proc callGraphInitialize {} {
   if {![paradyn applicationDefined]} {
      # No application has been started yet!!!
      # Put up an error dialog:
      showError 110 {}
      return 
   }
   return [callGraphInitialize2]
}

proc callGraphInitialize2 {} {
   if {[winfo exists .callGraph]} {
      wm deiconify .callGraph
      raise .callGraph
      return
   }
   
   toplevel .callGraph -class "callGraph"
   option add *callGraph*Background grey
   option add *callGraph*activeBackground LightGrey
   option add *callGraph*activeForeground black
   wm protocol .callGraph WM_DELETE_WINDOW {wm iconify .callGraph}

   # area for title, menubar, logo
   frame .callGraph.titlearea
   pack  .callGraph.titlearea -side top -fill x -expand false -anchor n

   frame .callGraph.titlearea.right
   pack  .callGraph.titlearea.right -side right -fill y -expand false

   makeLogo .callGraph.titlearea.right.logo paradynLogo raised 2 navy
   pack  .callGraph.titlearea.right.logo -side top

   frame .callGraph.titlearea.left
   pack  .callGraph.titlearea.left -side left -fill both -expand true

   label .callGraph.titlearea.left.title -text "Call Graph" \
	   -foreground white -anchor c \
           -font { Times 14 bold } -relief raised \
	   -background navy
   pack  .callGraph.titlearea.left.title -side top -fill both -expand true

   # area for menubar:
   frame .callGraph.titlearea.left.menu
   pack  .callGraph.titlearea.left.menu -side top -fill x -expand false -anchor n

   frame .callGraph.titlearea.left.menu.mbar -borderwidth 2 -relief raised
   pack  .callGraph.titlearea.left.menu.mbar -side top -fill both -expand false

   menubutton .callGraph.titlearea.left.menu.mbar.program -text "Programs" -menu .callGraph.titlearea.left.menu.mbar.program.m
   menu .callGraph.titlearea.left.menu.mbar.program.m -selectcolor cornflowerblue
   menubutton .callGraph.titlearea.left.menu.mbar.view -text "View" -menu .callGraph.titlearea.left.menu.mbar.view.m
   menu .callGraph.titlearea.left.menu.mbar.view.m -selectcolor cornflowerblue
   .callGraph.titlearea.left.menu.mbar.view.m add command \
	   -label "Simple function names" -command "callGraphHideFullPath"
   .callGraph.titlearea.left.menu.mbar.view.m add command \
	   -label "Function names show full path" -command "callGraphShowFullPath"
   
   pack .callGraph.titlearea.left.menu.mbar.program -side left -padx 4
   pack .callGraph.titlearea.left.menu.mbar.view -side left -padx 4
   
   # -----------------------------------------------------------

   frame .callGraph.nontop
   pack  .callGraph.nontop -side bottom -fill both -expand true

   # -----------------------------------------------------------

   frame .callGraph.nontop.currprogramarea
   pack  .callGraph.nontop.currprogramarea -side top -fill x -expand false

    label .callGraph.nontop.currprogramarea.label1 -text "Current Program: " \
	    -font { Helvetica 12 } -anchor e
    pack  .callGraph.nontop.currprogramarea.label1 -side left -fill both -expand true

   label .callGraph.nontop.currprogramarea.label2 -text "" \
	   -font { Helvetica 12 } -anchor w
   pack  .callGraph.nontop.currprogramarea.label2 -side left -fill both -expand true

   # -----------------------------------------------------------

   frame .callGraph.nontop.main -width 6i -height 3.5i
   pack  .callGraph.nontop.main -side top -fill both -expand true

   scrollbar .callGraph.nontop.main.leftsb -orient vertical -width 16 \
	   -background gray \
	   -activebackground gray \
	   -command "callGraphNewVertScrollPosition"
   
   pack .callGraph.nontop.main.leftsb -side left -fill y -expand false

   scrollbar .callGraph.nontop.main.bottsb -orient horizontal -width 16 \
	   -activebackground gray \
	   -command "callGraphNewHorizScrollPosition"
   
   pack .callGraph.nontop.main.bottsb -side bottom -fill x -expand false
   
   frame .callGraph.nontop.main.all -relief flat -width 6i -height 2i
   pack .callGraph.nontop.main.all -side left -fill both -expand true
   
   # -----------------------------------------------------------
   frame .callGraph.nontop.find
   pack  .callGraph.nontop.find -side top -fill both -expand false
   
   label .callGraph.nontop.find.label -relief sunken -text "Search:" \
	   -font { Helvetica 12 }
   pack  .callGraph.nontop.find.label -side left -fill y -expand false

   entry .callGraph.nontop.find.entry -relief sunken -font { Helvetica 12 }
   pack  .callGraph.nontop.find.entry -side left -fill x -expand true
   
   bind  .callGraph.nontop.find.entry <Return> {callGraphFindHook [.callGraph.nontop.find.entry get]}
   
   # -----------------------------------------------------------
   
   bind .callGraph.nontop.main.all <Configure> {callGraphConfigureHook}
   bind .callGraph.nontop.main.all <Expose>    {callGraphExposeHook %c}
   bind .callGraph.nontop.main.all <Button-1>  {callGraphSingleClickHook %x %y}
   bind .callGraph.nontop.main.all <Double-Button-1> {callGraphDoubleClickHook %x %y}
   bind .callGraph.nontop.main.all <Alt-Motion> {callGraphAltPressHook %x %y}
   bind .callGraph.nontop.main.all <Motion> {callGraphAltReleaseHook}
   callGraphCreateHook
}
