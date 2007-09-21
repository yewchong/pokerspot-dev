
OBJS = 	table.o player.o card.o deck.o poller.o random_generator.o \
	pduhandler.o pduunknown.o \
       	pdulogin.o pduchat.o ring.o dbhandler.o pduplayerinfo.o \
       	pdubuyinrequest.o pduplayer.o pdulogout.o pdulogoutconfirm.o \
	pduaction.o pduactionecho.o pduactionrequest.o pduallin.o \
	pduactionrequestspreadlimit.o pduactionspreadlimit.o \
	pduannounce.o pdublindrequest.o pdubutton.o pdudealcards.o \
	pdudealcommunity.o pdudealcardsflex.o pduloginreject.o \
	pdugamenumber.o pdupostblind.o pdushowcards.o pdushowdown.o \
	pdusitin.o pdusitinquery.o pdusitout.o pdubuyinquery.o \
	pdubroadtablequery.o pdubroadtablereply.o pduplayerloginquery.o \
	pduplayerloginreply.o pdutablelogin.o pdutablelogout.o \
	pdutableupdate.o pdunotify.o pdurefuseblind.o pduhandlogrequest.o \
	pdubuyinmore.o pduping.o pdupong.o pduheartbeat.o \
	pduobserverlogin.o pduobserverlogout.o \
	pduplayerobservequery.o pduplayerobservereply.o \
	sync.o plm.o plm_allin.o plm_ante.o plm_bet.o plm_rake.o \
	plm_cleanup.o plm_deal.o plm_showdown.o pgl.o plm_rebuildring.o \
	dbinterface.o md5.o thread_extensions.o pot.o hand.o gamemain.o pmemory.o \
	pokerserv.o psi_win32.o sys.o ssl_funcs.o loungeserver.o \
	plm_7stud.o plm_7stud_ante.o plm_7stud_bet.o \
	psi_assert.o ipcheck.o accepter.o \
	pduclientreseat.o pdudissolveinfo.o pdudissolvetable.o \
	pduplayerseated.o pduplayerunseated.o pdureseatack.o \
	pdureseatplayer.o pduseatplayer.o pdusynchronize.o \
	pdutournamentparams.o tournament.o pduanteallin.o \
	pduplayerfinished.o


EXENAME = pokerserv

CC = /usr/bin/gcc
INCLUDES = -I. -I/usr/local/include -I/usr/local/include/g++-3 \
-I/usr/local/src/poker-eval/include -I/usr/local/src/prng-2.2 \
-I/usr/local/src/openssl-0.9.5a/include
CCOPTS = -ggdb -ansi -Wall -pthread $(INCLUDES)
LINK = $(CC)
LINKOPTS = -ggdb -ansi -Wall -pthread -L/usr/local/lib -lstdc++ \
	   -L/usr/local/lib/mysql -lmysqlclient \
	   -L/usr/local/src/poker-eval/lib -lpoker \
	   -L/usr/local/src/prng-2.2 -lprng \
	   -L/usr/local/src/openssl-0.9.5a -lssl -lcrypto -lRSAglue

LD_RUN_PATH = /usr/local/lib/mysql

all $(EXENAME):		$(OBJS)
	$(LINK) -o $(EXENAME) $(OBJS) $(LINKOPTS)
clean:
	-rm -f *.o $(EXENAME)
table.o:
	$(CC) -c $(CCOPTS) table/table.cpp
player.o:
	$(CC) -c $(CCOPTS) player/player.cpp
card.o:
	$(CC) -c $(CCOPTS) deck/card.cpp
deck.o:
	$(CC) -c $(CCOPTS) deck/deck.cpp
poller.o:
	$(CC) -c $(CCOPTS) poller/poller.cpp
random_generator.o:
	$(CC) -c $(CCOPTS) random_generator.cpp
pduhandler.o:
	$(CC) -c $(CCOPTS) pduhandler/pduhandler.cpp
pduunknown.o:
	$(CC) -c $(CCOPTS) pduhandler/pduunknown.cpp
pdulogin.o:
	$(CC) -c $(CCOPTS) pdulogin/pdulogin.cpp
pduloginreject.o:
	$(CC) -c $(CCOPTS) pdulogin/pduloginreject.cpp
pduchat.o:
	$(CC) -c $(CCOPTS) pduchat/pduchat.cpp
ring.o:
	$(CC) -c $(CCOPTS) ring/ring.cpp
dbhandler.o:
	$(CC) -c $(CCOPTS) dbhandler/dbhandler.cpp
pduplayerinfo.o:
	$(CC) -c $(CCOPTS) pduplayerinfo/pduplayerinfo.cpp
pdubuyinrequest.o:
	$(CC) -c $(CCOPTS) pdubuyinrequest/pdubuyinrequest.cpp
pduplayer.o:
	$(CC) -c $(CCOPTS) pduplayer/pduplayer.cpp
pdulogout.o:
	$(CC) -c $(CCOPTS) pdulogout/pdulogout.cpp
pdulogoutconfirm.o:
	$(CC) -c $(CCOPTS) pdulogoutconfirm/pdulogoutconfirm.cpp
pduaction.o:
	$(CC) -c $(CCOPTS) pduplay/pduaction.cpp
pduactionecho.o:
	$(CC) -c $(CCOPTS) pduplay/pduactionecho.cpp
pduactionrequest.o:
	$(CC) -c $(CCOPTS) pduplay/pduactionrequest.cpp
pduactionrequestspreadlimit.o:
	$(CC) -c $(CCOPTS) pduplay/pduactionrequestspreadlimit.cpp
pduactionspreadlimit.o:
	$(CC) -c $(CCOPTS) pduplay/pduactionspreadlimit.cpp
pduallin.o:
	$(CC) -c $(CCOPTS) pduplay/pduallin.cpp
pduannounce.o:
	$(CC) -c $(CCOPTS) pduplay/pduannounce.cpp
pdublindrequest.o:
	$(CC) -c $(CCOPTS) pduplay/pdublindrequest.cpp
pdubutton.o:
	$(CC) -c $(CCOPTS) pduplay/pdubutton.cpp
pdudealcards.o:
	$(CC) -c $(CCOPTS) pduplay/pdudealcards.cpp
pdudealcommunity.o:
	$(CC) -c $(CCOPTS) pduplay/pdudealcommunity.cpp
pdudealcardsflex.o:
	$(CC) -c $(CCOPTS) pduplay/pdudealcardsflex.cpp
pdugamenumber.o:
	$(CC) -c $(CCOPTS) pduplay/pdugamenumber.cpp
pdupostblind.o:
	$(CC) -c $(CCOPTS) pduplay/pdupostblind.cpp
pdurefuseblind.o:
	$(CC) -c $(CCOPTS) pduplay/pdurefuseblind.cpp
pdushowcards.o:
	$(CC) -c $(CCOPTS) pduplay/pdushowcards.cpp
pdushowdown.o:
	$(CC) -c $(CCOPTS) pduplay/pdushowdown.cpp
pdusitin.o:
	$(CC) -c $(CCOPTS) pduplay/pdusitin.cpp
pdusitinquery.o:
	$(CC) -c $(CCOPTS) pduplay/pdusitinquery.cpp
pdusitout.o:
	$(CC) -c $(CCOPTS) pduplay/pdusitout.cpp
pdubuyinquery.o:
	$(CC) -c $(CCOPTS) pdubuyinquery/pdubuyinquery.cpp
pdubroadtablequery.o:
	$(CC) -c $(CCOPTS) lounge/pdubroadtablequery.cpp
pdubroadtablereply.o:
	$(CC) -c $(CCOPTS) lounge/pdubroadtablereply.cpp
pduplayerloginquery.o:
	$(CC) -c $(CCOPTS) lounge/pduplayerloginquery.cpp
pduplayerloginreply.o:
	$(CC) -c $(CCOPTS) lounge/pduplayerloginreply.cpp
pdutablelogin.o:
	$(CC) -c $(CCOPTS) lounge/pdutablelogin.cpp
pdutablelogout.o:
	$(CC) -c $(CCOPTS) lounge/pdutablelogout.cpp
pdunotify.o:
	$(CC) -c $(CCOPTS) pdunotify/pdunotify.cpp
pdutableupdate.o:
	$(CC) -c $(CCOPTS) lounge/pdutableupdate.cpp
pduhandlogrequest.o:
	$(CC) -c $(CCOPTS) pduhandlogrequest/pduhandlogrequest.cpp
pdubuyinmore.o:
	$(CC) -c $(CCOPTS) pdubuyinquery/pdubuyinmore.cpp
pduping.o:
	$(CC) -c $(CCOPTS) pdupingpong/pduping.cpp
pdupong.o:
	$(CC) -c $(CCOPTS) pdupingpong/pdupong.cpp
pduheartbeat.o:
	$(CC) -c $(CCOPTS) pdupingpong/pduheartbeat.cpp
pduobserverlogin.o:
	$(CC) -c $(CCOPTS) pdulogin/pduobserverlogin.cpp
pduobserverlogout.o:
	$(CC) -c $(CCOPTS) pdulogout/pduobserverlogout.cpp
pduplayerobservequery.o:
	$(CC) -c $(CCOPTS) lounge/pduplayerobservequery.cpp
pduplayerobservereply.o:
	$(CC) -c $(CCOPTS) lounge/pduplayerobservereply.cpp
sync.o:
	$(CC) -c $(CCOPTS) sync/sync.cpp
plm.o:
	$(CC) -c $(CCOPTS) plm/plm.cpp
plm_allin.o:
	$(CC) -c $(CCOPTS) plm/plm_allin.cpp
plm_ante.o:
	$(CC) -c $(CCOPTS) plm/plm_ante.cpp
plm_bet.o:
	$(CC) -c $(CCOPTS) plm/plm_bet.cpp
plm_cleanup.o:
	$(CC) -c $(CCOPTS) plm/plm_cleanup.cpp
plm_deal.o:
	$(CC) -c $(CCOPTS) plm/plm_deal.cpp
plm_showdown.o:
	$(CC) -c $(CCOPTS) plm/plm_showdown.cpp
plm_rebuildring.o:
	$(CC) -c $(CCOPTS) plm/plm_rebuildring.cpp
plm_rake.o:
	$(CC) -c $(CCOPTS) plm/plm_rake.cpp
plm_7stud.o:
	$(CC) -c $(CCOPTS) plm_7stud/plm_7stud.cpp
plm_7stud_ante.o:
	$(CC) -c $(CCOPTS) plm_7stud/plm_7stud_ante.cpp
plm_7stud_bet.o:
	$(CC) -c $(CCOPTS) plm_7stud/plm_7stud_bet.cpp
pgl.o:
	$(CC) -c $(CCOPTS) pgl/pgl.cpp
dbinterface.o:
	$(CC) -c $(CCOPTS) dbinterface/dbinterface.cpp
md5.o:
	$(CC) -c $(CCOPTS) dbinterface/md5.cpp
thread_extensions.o:
	$(CC) -c $(CCOPTS) pthread/thread_extensions.c
pot.o:
	$(CC) -c $(CCOPTS) pot/pot.cpp
hand.o:
	$(CC) -c $(CCOPTS) hand/hand.cpp
gamemain.o:
	$(CC) -c $(CCOPTS) gamemain.cpp
pokerserv.o:
	$(CC) -c $(CCOPTS) pokerserv.cpp
psi_win32.o:
	$(CC) -c $(CCOPTS) psi_win32.cpp
sys.o:
	$(CC) -c $(CCOPTS) sys.cpp
ssl_funcs.o:
	$(CC) -c $(CCOPTS) ssl_funcs.cpp
loungeserver.o:
	$(CC) -c $(CCOPTS) lounge/loungeserver.cpp
psi_assert.o:
	$(CC) -c $(CCOPTS) psi_assert.cpp
ipcheck.o:
	$(CC) -c $(CCOPTS) ipcheck.cpp
accepter.o:
	$(CC) -c $(CCOPTS) accepter.cpp
pduclientreseat.o:
	$(CC) -c $(CCOPTS) tournament/pduclientreseat.cpp
pdudissolveinfo.o:
	$(CC) -c $(CCOPTS) tournament/pdudissolveinfo.cpp
pdudissolvetable.o:
	$(CC) -c $(CCOPTS) tournament/pdudissolvetable.cpp
pduplayerseated.o:
	$(CC) -c $(CCOPTS) tournament/pduplayerseated.cpp
pduplayerunseated.o:
	$(CC) -c $(CCOPTS) tournament/pduplayerunseated.cpp
pdureseatack.o:
	$(CC) -c $(CCOPTS) tournament/pdureseatack.cpp
pdureseatplayer.o:
	$(CC) -c $(CCOPTS) tournament/pdureseatplayer.cpp
pduseatplayer.o:
	$(CC) -c $(CCOPTS) tournament/pduseatplayer.cpp
pdusynchronize.o:
	$(CC) -c $(CCOPTS) tournament/pdusynchronize.cpp
pdutournamentparams.o:
	$(CC) -c $(CCOPTS) tournament/pdutournamentparams.cpp
tournament.o:
	$(CC) -c $(CCOPTS) tournament/tournament.cpp
pduanteallin.o:
	$(CC) -c $(CCOPTS) tournament/pduanteallin.cpp
pduplayerfinished.o:
	$(CC) -c $(CCOPTS) tournament/pduplayerfinished.cpp
