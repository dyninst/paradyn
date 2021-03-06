/* $Log: server1.C,v $
/* Revision 1.5  1994/06/08 17:34:43  newhall
/* *** empty log message ***
/*
 * Revision 1.4  1994/03/26  04:37:05  newhall
 * change all floats to double
 * */
#include "../h/visualization.h"

int dv(int dummy){

int i,j,k,noBins,noMetrics,noResources;
float value;

  fprintf(stderr,"@@@@call back routine for DATAVALUES\n");
  noMetrics = dataGrid.NumMetrics();
  noResources = dataGrid.NumResources();
  noBins = dataGrid.NumBins();
  fprintf(stderr,"@@@@ ERRROR = %lf quiet_nan = %lf\n",ERROR,quiet_nan());
  if(ERROR == quiet_nan()){
  fprintf(stderr,"@@@@ ERRROR == quiet_nan \n");
  }
  for(i = 0; i < noMetrics; i++)
    for(j = 0; j < noResources; j++){
      for(k = 0; k < noBins; k++)
       if((value = dataGrid[i][j][k]) != ERROR)
         fprintf(stderr,"@@@@dataGrid[%d,%d,%d] = %lf\n",i,j,k,value);
       else
         fprintf(stderr,"@@@@dataGrid[%d,%d,%d] = NaN\n",i,j,k);

      printf("\n");
    }

  return(OK);
}


int imr(int dummy){
  fprintf(stderr,"@@@@call back routine for INVALIDMETRICSRESOURCES\n");
  return(OK);
}

int amr(int dummy){
  fprintf(stderr,"@@@@call back routine for ADDMETRICSRESOURCES\n");
  return(OK);
}

int nmr(int dummy){
  fprintf(stderr,"@@@@call back routine for NEWMETRICSRESOURCES\n");
  fprintf(stderr,"@@@@before upcall: GetMetricResource\n");
  GetMetsRes("metrics test string","r test string",0);
  fprintf(stderr,"@@@@after upcall: GetMetricResource\n");
  fprintf(stderr,"@@@@before upcall: StopMetricResource\n");
  StopMetRes(3,2);
  fprintf(stderr,"@@@@after upcall: StopMetricResource\n");
  fprintf(stderr,"@@@@before upcall: PhaseName\n");
  NamePhase(3.0,2.0,"phase test string");
  fprintf(stderr,"@@@@after upcall: PhaseName\n");
  return(OK);
}

int pn(int dummy){
int i,j,k,noBins,noMetrics,noResources;

  fprintf(stderr,"@@@@call back routine for PHASENAME\n");

  noMetrics = dataGrid.NumMetrics();
  noResources = dataGrid.NumResources();
  noBins = dataGrid.NumBins();

  fprintf(stderr,"\n");
  for(i = 0; i < noMetrics; i++){
      fprintf(stderr,"@@@@metric %d: Id = %d, name = %s, units = %s\n",i,dataGrid.MetricId(i),dataGrid.MetricName(i),dataGrid.MetricUnits(i));
   }

   for(j = 0; j < noResources; j++)
       fprintf(stderr,"@@@@resource %d: Id = %d, name = %s\n",j,dataGrid.ResourceId(j),dataGrid.ResourceName(j));

  fprintf(stderr,"\n");
  for(i = 0; i < noMetrics; i++)
    for(j = 0; j < noResources; j++){
      for(k = 0; k < noBins; k++)
       fprintf(stderr,"@@@@dataGrid[%d,%d,%d] = %lf\n",i,j,k,dataGrid[i][j][k]);
      printf("\n");
    }
    (char *)dataGrid[0][0].userdata =  strdup("blah");
    fprintf(stderr,"(char *)dataGrid[0][0].userdata = %s\n",(char *)dataGrid[0][0].userdata);
  return(OK);
}

int f(int dummy){
  fprintf(stderr,"@@@@call back routine for FOLD\n");
  return(OK);
}




main(int argc, char *argv[]){

int ok;
 
   // call VisiInit
   if((ok = VisiInit()) != OK){
     exit(-1);
   }

  // register event callbacks

  ok = RegistrationCallback(DATAVALUES,dv);
  fprintf(stderr,"@@@@ok = %d after call to RegistrationCallback\n",ok);
  ok = RegistrationCallback(INVALIDMETRICSRESOURCES,imr);
  fprintf(stderr,"@@@@ok = %d after call to RegistrationCallback\n",ok);
  ok = RegistrationCallback(ADDMETRICSRESOURCES,amr);
  fprintf(stderr,"@@@@ok = %d after call to RegistrationCallback\n",ok);
  ok = RegistrationCallback(NEWMETRICSRESOURCES,nmr);
  fprintf(stderr,"@@@@ok = %d after call to RegistrationCallback\n",ok);
  ok = RegistrationCallback(PHASENAME,pn);
  fprintf(stderr,"@@@@ok = %d after call to RegistrationCallback\n",ok);
  ok = RegistrationCallback(FOLD,f);
  fprintf(stderr,"@@@@ok = %d after call to RegistrationCallback\n",ok);


  // start visi: calls GetMetricResources() with initial metric/resource
  // lists: this is not necessary, but for viisualizaitons that
  // don't do upcalls its the only method of starting data collection
  ok = StartVisi(argc,argv);
  fprintf(stderr,"@@@@ok=%d after call to StartVisi()\n",ok);

  // register fd and callback using appropreate X routine  
  // then enter X main loop 


 // pseudo X main loop
  while(1){
   ok = visi_callback(); 
   if(ok < 0){
    //client has exited, die
    exit(-1);
   }
  }
}
