/* GNUPLOT - command.c */
/*
 * Copyright (C) 1986, 1987, 1990   Thomas Williams, Colin Kelley
 *
 * Permission to use, copy, and distribute this software and its
 * documentation for any purpose with or without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and 
 * that both that copyright notice and this permission notice appear 
 * in supporting documentation.
 *
 * Permission to modify the software is granted, but not the right to
 * distribute the modified code.  Modifications are to be distributed 
 * as patches to released version.
 *  
 * This software  is provided "as is" without express or implied warranty.
 * 
 *
 * AUTHORS
 * 
 *   Original Software:
 *     Thomas Williams,  Colin Kelley.
 * 
 *   Gnuplot 2.0 additions:
 *       Russell Lang, Dave Kotz, John Campbell.
 * 
 * send your comments or suggestions to (pixar!info-gnuplot@sun.com).
 *
 */

/*
 * Heavily modified for Terrain Plot:
 *    Chi-Ting Lam     (Apr 25, 1992).
 *
 * send you comments or suggestions to (ips@cs.wisc.edu).
 *
 */     

#ifndef lint
static char rcsid[] = "@(#) $Header: /home/jaw/paradyn_2008_10_14/CVSROOT/core/visiClients/terrain/src/command.c,v 1.1 1997/05/12 20:15:23 naim Exp $";
#endif

/*
 * command.c - main switchboard of the program.
 *
 * $Log: command.c,v $
 * Revision 1.1  1997/05/12 20:15:23  naim
 * Adding "Terrain" visualization to paradyn (commited by naim, done by tung).
 *
 * Revision 1.4  1992/05/19  20:21:01  lam
 * Added detection for dead master when checking visible nodes.
 * But terrain die (no core dum) when this happen for unknown reason.
 *
 * Revision 1.3  1992/05/19  18:43:13  lam
 * Correction on error mesesages.
 *
 * Revision 1.2  1992/05/19  17:30:23  lam
 * Treat not enough visible notes as a warning instead of an error.
 *
 * Revision 1.1  1992/05/19  06:30:55  lam
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <math.h>
#include <strings.h>

#include "plot.h"
#include "setshow.h"

#include "misc.h"
#include "reduce.h"
#include "smooth.h"
#include "shadow.h"
#include "command.h"
#include "terrain.h"



#ifdef EVU_LIB
#include "ipsstuff.h"
#endif

#define inrange(z,min,max) ((min<max) ? ((z>=min)&&(z<=max)) : ((z>=max)&&(z<=min)) )

extern struct at_type *temp_at(), *perm_at();
extern void squash_spaces();
extern void lower_case();

/* Local functions */
struct surface_points *get_newplot();

/* Reduce needs screen width */
extern W;

#ifdef EVU_LIB
Metric *metric;			        /* For node reduction */
#endif

/* input data, parsing variables */
struct lexical_unit token[MAX_TOKENS];
char input_line[MAX_LINE_LEN+1];
int num_tokens, c_token;
int inline_num = 0;			/* input line number */

char *data_file;

/* new variables for dynamic graph */
static struct surface_points *curves = 0;  /* the surface of points */
static const float y_interval = 1;         /* the separation of each curve */
static float x_interval;
static int nopoints;                       /* the number of points on each curve */
static int nocurves;
static int change = 0;

/* Different kinds of plots which plot3drequest manages.
 * Note that the order is important here since they are organized in
 * a binary tree form.
 */

#define S_ORIG 8
#define S_HIDE 4
#define S_MINI 2
#define S_SMTH 1
#define S_REDU 0

#define WIN_SIZE 5		/* Hard-wired smooth window size */

/* variables as indicator when smooth data or med is need */
static int is_smooth = 0,	/* Default is no smoothing  */
           is_med = 0;	        /* Default is using mean for smoothing */



/********************************************************************
*
* plot3drequest - The main switch broad for drawing terrains.
*
* plot3drequest draw the approiate terrain (and do the related
* calculations) accroding to the action given.
*
* The possible actions are in command.h
*
********************************************************************/

plot3drequest(action)
int action;
{
    int reDraw_atOnce = 0;
    color_mode = 1;		/* No color only when rotating */


    switch (action) {
	case SA_ROTATE: color_mode = 0;
 	                reDraw_atOnce = 1;

	           break;

	case SA_JUMP:   reDraw_atOnce = 1;
                        break;	/* Just redraw with the new view angle */

	case SA_SMOOTH: is_smooth = 1;
	                reDraw_atOnce = 1;;
	           break;

	case SA_ROUGH:  is_smooth = 0;
                        change = 1;
	           break;

	case SA_USEMED: is_med = 1;
	                if (is_smooth == 1)
                           change = 1;
		   break;

	case SA_NOMED:  is_med = 0;
                        change = 1;
		   break;


	case SA_RESIZE: reDraw_atOnce = 1;
	           break;
	
        default: terrain_error("Wrong display command!");
	         break;
    }

       ReDisplayGraph();


}


/* Keep old GNUPlot code happy */

done(status)
int status;
{
	exit(status);
}


/***********************************************************************
 ******************** modified section starts***************************/
 


int getStartIndex(ID)
int ID;
{

  return ID * nopoints;

}


void kill_surface()
{
  if (curves != NULL)
     free_surface(curves);
}


int checkDecimal(zmax)
float zmax;
{
  int i = 0;

  if (zmax > 10)
     return -1;

  for (i = 0; zmax < 1; i++)
      zmax *=10;

  return i;

}



void ReDisplayGraph()
{

  struct surface_points *thisCurve;
  int del = 0;

  if (is_smooth == 1 && curves != 0)
  {
     thisCurve = (struct surface_points* ) terrain_alloc(sizeof(struct surface_points));
     if (is_med == 1)
     {
        smooth_med(thisCurve, curves, WIN_SIZE);
        del = 1;
     }
     else
     {
        smooth(thisCurve, curves, WIN_SIZE, curves->lastprintIndex);
        del = 1;
     }
  }
  else
     thisCurve = curves;

  if (thisCurve != 0 && thisCurve->iso_samples > 1)
  {
     float minx = thisCurve->x_min;
     float maxx = thisCurve->x_max;
     float miny = thisCurve->y_min;
     float maxy = thisCurve->y_max;
     float minz = thisCurve->z_min;
     float maxz = thisCurve->z_max;
     int count = 1;
     struct text_label *currentLabel;

     /* GNUPlot uses these globle values in graph3d.c.  So set them here */
     zmin = thisCurve->z_min;
     zmax = thisCurve->z_max;
     ymax = thisCurve->y_max;
     ymin = thisCurve->y_min;

     samples = thisCurve->samples;
     iso_samples = thisCurve->iso_samples;

     if (maxz != minz)
     {
        for (currentLabel = first_label; currentLabel != 0;
	     currentLabel = currentLabel->next)
            currentLabel->x = maxx;

  	do_3dplot(thisCurve, count, minx, maxx, miny, maxy, minz, maxz);
     }
   }

   
   if (del == 1)
   {
      free_surface(thisCurve);
   }
   thisCurve = 0;


}


void ProcessNewSegments(printIndex)
int printIndex;
{
   struct surface_points* thisCurve;
   int allvalid = 1;
   int i;
   int del = 0;

   for (i = 0; i < curves->iso_samples; i++)
   {
       if (curves->points[i * nopoints + printIndex].valid != 1)
       {
	  allvalid = 0;
          break;
       }
   }
  
   if (allvalid == 1)
   {
      if (is_smooth == 1 && curves != 0)
      {
         thisCurve = (struct surface_points*)terrain_alloc(sizeof(struct surface_points));
         if (is_med == 1)
	 {
            smooth_med(thisCurve, curves, WIN_SIZE);
            del = 1;
	 }
	 else
	 {
          smooth(thisCurve, curves, WIN_SIZE, printIndex);
            del = 1;
	 }
      }
      else
         thisCurve = curves;

      plot3d_lines(thisCurve, printIndex);
      curves->lastprintIndex = printIndex;

      if (del == 1)
      {
         free_surface(thisCurve);
      }
      thisCurve = 0;


   }

}



void Graph3DSetCurveData(curveID, firstSample, numSamples, sample, startTime, x_inter, fold,
			 color_disp, dpy, gc, rv, pixmap, W, H, win)
int curveID;
int firstSample;
int numSamples;
const float *sample;
float startTime;
float x_inter;
int fold;
int color_disp;
Display* dpy;
GC gc;
RValues rv;
Pixmap pixmap;
Dimension W;
Dimension H;
Window win;
{
  int startIndex = getStartIndex(curveID);
  int i = 0;
  int decimal;
  static int curves_ready_afterFold = 0;

  x_interval = x_inter;
 

  if (fold)
  {
     curves->lastprintIndex = 0;
     for (i = startIndex; i < startIndex + nopoints; i++)
         curves->points[i].valid = 0;
  
     curves_ready_afterFold ++;    

     fprintf(stderr, "Fold!\n");
  }

  i = 0;

  while (firstSample + i < curves->samples && i < numSamples)
  {
      /* z - values insertion */
      if (!isnan(sample[i]))    /* test for NaN */
         curves->points[startIndex + firstSample + i].z = sample[i];
      else
         curves->points[startIndex + firstSample + i].z = 0;

      if (curves->points[startIndex + firstSample + i].z > curves->z_max)
      {   
         curves->z_max = curves->points[startIndex + firstSample + i].z * 2;
	 decimal = checkDecimal(curves->z_max);
	 if (decimal == -1)
	    changeDecimal(0);
	 else
	    changeDecimal(decimal + 1);
	 
         change = 1;
      }
   
      curves->points[startIndex + firstSample + i].x = startTime + (firstSample + i) * 
	                                               x_interval;
      if (curves->points[startIndex + firstSample + i].x > curves->x_max)
      {
        curves->x_max *= 2;
        change = 1;
      }
   
      curves->points[startIndex + firstSample + i].valid = 1;
 
      if (fold == 0)
      {
         if (curves->iso_samples == nocurves)
         {
	    if (change == 1 || (startIndex + firstSample + i) % nopoints == 0 )
	    {
               #ifndef MOTIF
	       if (color_disp) { /* Athena needs different erase for color and mono */
               #endif
		  XSetForeground(dpy, gc, rv.bg);
		  XFillRectangle(dpy, pixmap, gc, 0, 0, W, H);
		  XFillRectangle(dpy, win, gc, 0, 0, W, H);
		  XSetForeground(dpy, gc, rv.fg);
		  XSetBackground(dpy, gc, rv.bg);
               #ifndef MOTIF
               }
               else {
                  XSetFunction(dpy, gc, GXxor);
		  XCopyArea(dpy, win, win, gc, 0, 0, W, H, 0, 0);
		  XCopyArea(dpy, pixmap, pixmap, gc, 0, 0, W, H, 0, 0);
		  XSetFunction(dpy, gc, GXcopyInverted);
               }
               #endif

	       ReDisplayGraph();

	       /* draw the graph on the window */
               XClearArea(dpy, win, 0, 0, 0, 0, True);

               ProcessNewSegments(firstSample + i);
	       setNotFt();
               change = 0;
            }
	    else
	    {
	       ProcessNewSegments(firstSample + i);
            }
      
         }
      }
      else
	 curves->lastprintIndex = firstSample + i;
     
      i ++;
  }

  if (fold == 1 && curves_ready_afterFold == nocurves)
  { 
              #ifndef MOTIF
	       if (color_disp) { /* Athena needs different erase for color and mono */
               #endif
		  XSetForeground(dpy, gc, rv.bg);
		  XFillRectangle(dpy, pixmap, gc, 0, 0, W, H);
		  XFillRectangle(dpy, win, gc, 0, 0, W, H);
		  XSetForeground(dpy, gc, rv.fg);
		  XSetBackground(dpy, gc, rv.bg);
               #ifndef MOTIF
               }
               else {
                  XSetFunction(dpy, gc, GXxor);
		  XCopyArea(dpy, win, win, gc, 0, 0, W, H, 0, 0);
		  XCopyArea(dpy, pixmap, pixmap, gc, 0, 0, W, H, 0, 0);
		  XSetFunction(dpy, gc, GXcopyInverted);
               }
               #endif

	       ReDisplayGraph();

	       /* draw the graph on the window */
               XClearArea(dpy, win, 0, 0, 0, 0, True);
	       
	       curves_ready_afterFold = 0;
  }  

  return ;

}


int Graph3DAddNewCurve (m_name, r_name, p_name, axis_label, groupID, no_points, 
			no_curves)
char *m_name;
char *r_name;
char *p_name;
char *axis_label;
int groupID;
int no_points;
int no_curves;
{
  int i = 0;
  float y_value;
  int curveID;
  struct coordinate *temp;
  struct text_label *currentLabel;
  
 
  if (curves == 0)
  {
    /* initialize the required value at the FIRST time */
    nopoints = no_points;
    nocurves = no_curves;

    curves = (struct surface_points *)terrain_alloc(sizeof(struct surface_points));
    curves->next_sp = 0;
    curves->points = (struct coordinate *) terrain_alloc(sizeof(struct coordinate)*nopoints);

    strcpy(title, "Phase: ");
    strcpy(title + 7, p_name);

    curves->title = m_name;
    curves->line_type = 0;
    curveID = 0;
    curves->p_count = nopoints;
    curves->samples = nopoints;
    y_value = 0;
    curves->iso_samples = 1;
    curves->x_max = 100;
    curves->x_min = 0;
    curves->z_max = 0.0001;
    curves->z_min = 0;
    curves->y_min = 0;
    curves->lastprintIndex = 0;

    first_label = (struct text_label*)terrain_alloc(sizeof(struct text_label));
    currentLabel = first_label;

    strcpy(xlabel, "time in seconds");    
    strcpy(zlabel, axis_label);

  }
  else
  {
 
    temp = (struct coordinate*)terrain_alloc(sizeof(struct coordinate) *
           (curves->p_count + nopoints));
    for( i = 0; i < curves->p_count; i++)
      temp[i] = curves->points[i];

    free(curves->points);
    curves->points = temp;

    temp = 0;

    curveID = curves->p_count;
    curves->p_count += nopoints;
    curveID = curves->iso_samples;
    y_value = curves->y_max + y_interval;
    curves->iso_samples++;   

    currentLabel = first_label;
    while (currentLabel->next != 0)
          currentLabel = currentLabel->next;

    currentLabel->next = (struct text_label*)terrain_alloc(sizeof(struct text_label));
    currentLabel = currentLabel->next;


  }

  curves->y_max = y_value;
  for (;i < curves->p_count; i++)
  {
     curves->points[i].y = y_value;
     curves->points[i].valid = 0;
  }

  currentLabel->tag = curveID;
  currentLabel->y = y_value;
  currentLabel->z = 0;
  currentLabel->next = 0;
  strcpy(currentLabel->text, r_name);

  return curveID;

}



/********************** modified section ends***************************
 ***********************************************************************/















