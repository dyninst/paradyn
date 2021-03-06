
USES_PVM = true

include	../../make.config

.SUFFIXES: .c .I .C .o

ADV_C_OBJ = class.o class.CLNT.o advancedC.o
ADV_S_OBJ = class.o class.SRVR.o advancedS.o
C_OBJSPVM	= testPVM.o clientPVM.o testPVM.CLNT.o 
S_OBJSPVM	= testPVM.o serverPVM.o testPVM.SRVR.o 
C_OBJS1	= test1.o client1.o test1.CLNT.o 
S_OBJS1	= test1.o server1.o test1.SRVR.o 
OBJS2	= test2.o client2.o server2.o 

SRCS 	= client1.C \
          client2.C \
          server1.C \
          server2.C \
          clientPVM.C \
          serverPVM.C \
	  advancedS.C \
	  advancedC.C 
           
EXTRA_CFLAGS = $(CFLAGS) -I./util/h

all:  DEPENDS all_igen client1 server1 program2 advancedS advancedC 

advanced: advancedS advancedC 

all_igen: test1_igen test2_igen testPVM_igen class_igen

all_pure: clientPVM_pure serverPVM_pure server1_pure client1_pure program2_pure

client1_pure: $(C_OBJS1) 
	purify -g++ -collector=/usr/sup/purify/collect2 $(CC) -o client1 \
	         $(C_OBJS1) $(LIBDIR) -lutil

server1_pure: $(S_OBJS1) 
	purify -g++ -collector=/usr/sup/purify/collect2 $(CC) -o server1  \
	         $(S_OBJS1) $(LIBDIR) -lutil

program2_pure: $(OBJS2) 
	purify -g++ -collector=/usr/sup/purify/collect2 $(CC) -o program2 \
	          $(OBJS2) $(LIBDIR) -lthread -lutil

clientPVM_pure: $(C_OBJSPVM) 
	purify -g++ -collector=/usr/sup/purify/collect2 $(CC) -o clientPVM \
	            $(C_OBJSPVM) $(LIBDIR) -lutil -lutilPVM -lpvm3 

serverPVM_pure: $(S_OBJSPVM) 
	purify -g++ -collector=/usr/sup/purify/collect2 $(CC) -o serverPVM \
	         $(S_OBJSPVM) $(LIBDIR) -lutil -lutilPVM -lpvm3 

advanced_pure: advancedS_pure advancedC_pure

advancedS_pure: $(ADV_S_OBJ)
	purify -g++ -collector=/usr/sup/purify/collect2 $(CC) $(CFLAGS) -static -o advS $(ADV_S_OBJ) $(LIBDIR) -lutil

advancedC_pure: $(ADV_C_OBJ)
	purify -g++ -collector=/usr/sup/purify/collect2 $(CC) $(CFLAGS) -static -o advC $(ADV_C_OBJ) $(LIBDIR) -lutil

advancedS: $(ADV_S_OBJ)
	$(CC) $(CFLAGS) -static -o advS $(ADV_S_OBJ) $(LIBDIR) -lutil

advancedC: $(ADV_C_OBJ)
	$(CC) $(CFLAGS) -static -o advC $(ADV_C_OBJ) $(LIBDIR) -lutil

client1: $(C_OBJS1) 
	$(CC) $(CFLAGS) -o client1 $(C_OBJS1) $(LIBDIR) -lutil

server1: $(S_OBJS1) 
	$(CC) $(CFLAGS) -o server1 $(S_OBJS1) $(LIBDIR) -lutil

program2: $(OBJS2) 
	$(CC) $(CFLAGS) -static -o program2 $(OBJS2) $(LIBDIR) -lthread -lutil

clientPVM: $(C_OBJSPVM) 
	$(CC) $(CFLAGS) -static -o clientPVM $(C_OBJSPVM) $(LIBDIR) -lutil -lutilPVM -lpvm3 

serverPVM: $(S_OBJSPVM) 
	$(CC) $(CFLAGS) -static -o serverPVM $(S_OBJSPVM) $(LIBDIR) -lutil -lutilPVM -lpvm3 

clean: 
	rm *.o *CLNT* *SRVR* *.h clientPVM serverPVM \
              testPVM.C *pure* client1 server1 program2 \
              test1.C test2.C class.C advC advS

class_igen: class.I
	../$(PLATFORM)/igen -xdr $<
	indent++ -npro -i2 -tabs4 class.C
	indent++ -npro -i2 -tabs4  class.SRVR.C
	indent++ -npro -i2 -tabs4  class.CLNT.C

test1_igen: test1.I
	../$(PLATFORM)/igen -xdr $<

test2_igen: test2.I
	../$(PLATFORM)/igen -thread $<

testPVM_igen: testPVM.I
	../$(PLATFORM)/igen -pvm $<

test2.C: test2.I ../$(PLATFORM)/igen
	../$(PLATFORM)/igen -thread $<

test1.C: test1.I ../$(PLATFORM)/igen
	../$(PLATFORM)/igen -xdr $<

testPVM.C: testPVM.I ../$(PLATFORM)/igen
	../$(PLATFORM)/igen -pvm $<

class.C: class.I ../$(PLATFORM)/igen
	../$(PLATFORM)/igen -xdr $<

DEPENDS: $(SRCS) 
	touch DEPENDS
	makedepend -fDEPENDS -- $(CFLAGS) $(SRCS)
