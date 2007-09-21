# Makefile for loungeserv (c) 2000 PSICorp and Robert Boyd

OBJS = 	main.o \
		poller.o \
		pinger.o \
		pingcounter.o \
		server.o \
		sys.o \
		ssl_funcs.o \
		pdu.o \
		pduaccountbalance.o \
		pduallocator.o \
		pdubalancequery.o \
		pdubuyinreply.o \
		pdubuyinrequest.o \
		pducancelsitdown.o \
		pducashoutreply.o \
		pducashoutrequest.o \
		pduchat.o \
		pducreditcardquery.o \
		pducreditcardreply.o \
		pdulogin.o \
		pduloginaccept.o \
		pduloginreject.o \
		pdulogout.o \
		pdulogoutconfirmed.o \
		pdunotify.o \
		pduqueueupdate.o \
		pdusitdown.o \
		pdustatus.o \
		pdusurvey.o \
		pduunknown.o \
		pduloungestats.o \
		pduupgraderequest.o \
		pdusitting.o \
		pduqueuefull.o \
		pduplayerloginquery.o \
		pduplayerloginreply.o \
		pduspawntable.o \
		pdubroadtablequery.o \
		pdubroadtablereply.o \
		pdutableupdate.o \
		pdutablelogin.o \
		pdutablelogout.o \
		pdutransactionquery.o \
		pdutransactionresponse.o \
		pduping.o \
		pdupong.o \
		pduobserve.o \
		pduobserving.o \
		pduplayerobservequery.o \
		pduplayerobservereply.o \
		game.o \
		lounge.o \
		table.o \
		player.o \
		queueservice.o \
		tablemaster.o \
		waitinglist.o \
		dbhandler.o \
		dbinterface.o \
		autoupgrade.o \
		md5c.o \
		compute_md5_checksum.o \
		md5.o \
	action.o endstate.o event.o parser.o \
	pdudissolveinfo.o pdudissolvetable.o pduplayerseated.o \
	pduplayerunseated.o pdureseatack.o pdureseatnack.o pdureseatplayer.o \
	pduseatplayer.o pdusynchronize.o pdutournamentparams.o \
	pdutournamentdetailsquery.o pdutournamentdetailsreply.o \
	runstate.o runstateunfrozen.o seater.o startstate.o \
	tournament.o tournamentqueue.o tournamentstate.o waitpause.o \
	waitpredicate.o waitseated.o waitstartstate.o waitstate.o \
	pdueditrules.o pdueditformat.o tester.o \
	accepter.o sync.o thread_extensions.o \
	pducreditcardquery2.o pducreditcardreply2.o pducreditcardtypes.o \
        pducreditcardedit.o pduchatenable.o

EXENAME = loungeserv
CC = /usr/bin/gcc
CCOPTS = -pthread -ggdb -ansi -Wall
INCLUDES = -I. -I/usr/local/include/g++-3 -I../../sgi_stl \
	   -I/usr/local/src/openssl-0.9.5a/include -I/usr/local/include
LINK = $(CC)
LINKOPTS = -pthread -ggdb -ansi -Wall -L/usr/local/lib \
	-lstdc++ \
	-L/usr/local/src/openssl-0.9.5a -lssl -lcrypto -lrsaref -lRSAglue \
	-L/usr/local/lib/mysql -lmysqlclient
LD_RUN_PATH = /usr/local/lib/mysql

all $(EXENAME):  $(OBJS)
	$(LINK) -o $(EXENAME) $(OBJS) $(LINKOPTS)  
clean:
	-rm -f *.o $(EXENAME)
thread_extensions.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pthread/thread_extensions.c
pdu.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdu.cpp
pduaccountbalance.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduaccountbalance.cpp
pduallocator.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduallocator.cpp
pdubalancequery.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdubalancequery.cpp
pdubuyinreply.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdubuyinreply.cpp
pdubuyinrequest.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdubuyinrequest.cpp
pducancelsitdown.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pducancelsitdown.cpp
pducashoutreply.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pducashoutreply.cpp
pducashoutrequest.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pducashoutrequest.cpp
pduchat.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduchat.cpp
pducreditcardquery.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pducreditcardquery.cpp
pducreditcardreply.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pducreditcardreply.cpp
pducreditcardquery2.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pducreditcardquery2.cpp
pducreditcardreply2.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pducreditcardreply2.cpp
pducreditcardtypes.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pducreditcardtypes.cpp
pducreditcardedit.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pducreditcardedit.cpp
pduchatenable.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduchatenable.cpp
pdulogin.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdulogin.cpp
pduloginaccept.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduloginaccept.cpp
pduloginreject.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduloginreject.cpp
pdulogout.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdulogout.cpp
pdulogoutconfirmed.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdulogoutconfirmed.cpp
pdunotify.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdunotify.cpp
pduqueueupdate.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduqueueupdate.cpp
pdusitdown.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdusitdown.cpp
pdustatus.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdustatus.cpp
pdusurvey.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdusurvey.cpp
pduspawntable.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduspawntable.cpp
pduloungestats.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduloungestats.cpp
pduupgraderequest.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduupgraderequest.cpp
pdusitting.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdusitting.cpp
pduqueuefull.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduqueuefull.cpp
pduplayerloginquery.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduplayerloginquery.cpp
pduplayerloginreply.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduplayerloginreply.cpp
pdubroadtablequery.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdubroadtablequery.cpp
pdubroadtablereply.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdubroadtablereply.cpp
pdutableupdate.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdutableupdate.cpp
pdutablelogin.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdutablelogin.cpp
pdutablelogout.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdutablelogout.cpp
pduunknown.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduunknown.cpp
pdutransactionquery.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdutransactionquery.cpp
pdutransactionresponse.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdutransactionresponse.cpp
pduping.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduping.cpp
pdupong.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pdupong.cpp
pduobserve.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduobserve.cpp
pduobserving.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduobserving.cpp
pduplayerobservequery.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduplayerobservequery.cpp
pduplayerobservereply.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pdu/pduplayerobservereply.cpp
game.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) lounge/game.cpp
lounge.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) lounge/lounge.cpp
player.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) lounge/player.cpp
table.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) lounge/table.cpp
queueservice.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) lounge/queueservice.cpp
tablemaster.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) lounge/tablemaster.cpp
waitinglist.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) lounge/waitinglist.cpp
dbhandler.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) dbhandler/dbhandler.cpp
dbinterface.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) dbhandler/dbinterface.cpp
md5.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) dbhandler/md5.cpp
main.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) main.cpp
poller.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) poller.cpp
pinger.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pinger.cpp
pingcounter.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pingcounter.cpp
server.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) server.cpp
sys.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) sys.cpp
ssl_funcs.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) ssl_funcs.cpp
autoupgrade.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) upgrade/autoupgrade.cpp
md5c.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) upgrade/md5c.c
compute_md5_checksum.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) upgrade/compute_md5_checksum.cpp
action.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/action.cpp
endstate.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/endstate.cpp
event.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/event.cpp
parser.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/parser.cpp
pdudissolveinfo.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/pdudissolveinfo.cpp
pdudissolvetable.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/pdudissolvetable.cpp
pduplayerseated.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/pduplayerseated.cpp
pduplayerunseated.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/pduplayerunseated.cpp
pdureseatack.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/pdureseatack.cpp
pdureseatnack.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/pdureseatnack.cpp
pdureseatplayer.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/pdureseatplayer.cpp
pduseatplayer.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/pduseatplayer.cpp
pdusynchronize.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/pdusynchronize.cpp
pdutournamentparams.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/pdutournamentparams.cpp
pdutournamentdetailsquery.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/pdutournamentdetailsquery.cpp
pdutournamentdetailsreply.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/pdutournamentdetailsreply.cpp
runstate.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/runstate.cpp
runstateunfrozen.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/runstateunfrozen.cpp
seater.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/seater.cpp
startstate.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/startstate.cpp
tournament.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/tournament.cpp
tournamentqueue.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/tournamentqueue.cpp
tournamentstate.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/tournamentstate.cpp
waitpause.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/waitpause.cpp
waitpredicate.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/waitpredicate.cpp
waitseated.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/waitseated.cpp
waitstartstate.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/waitstartstate.cpp
waitstate.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/waitstate.cpp
pdueditrules.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/pdueditrules.cpp
pdueditformat.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) tournament/pdueditformat.cpp
tester.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) lounge/tester.cpp
accepter.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) accepter.cpp
sync.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) sync/sync.cpp
thread_extensions.o:
	$(CC) -c $(CCOPTS) $(INCLUDES) pthread/thread_extensions.c


