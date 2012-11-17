CC      = gcc
PROF    = -O -g3
NOCRYPT =
C_FLAGS =  -Wall -D_REENTRANT -D_AOW_CODER -I/home/actofwar/include $(PROF) $(NOCRYPT)
L_FLAGS =  -lcrypt -lpthread -lcoredumper -lsqlite3 -L/home/actofwar/lib $(PROF)

O_FILES =									\
	act_comm.o    act_enter.o   act_info.o    act_move.o    act_obj.o	\
	act_wiz.o     alias.o       ban.o         bit.o         comm.o		\
	const.o       db.o          db2.o         effects.o     fight.o		\
	flags.o       handler.o     healer.o      hunt.o        illegal.o	\
	interp.o      lookup.o      magic.o       magic2.o      mem.o		\
	mob_cmds.o    mob_prog.o    mount.o       music.o       new_dump.o	\
	note.o        olc.o         olc_act.o     olc_mpcode.o  olc_save.o	\
	pchannels.o   combat.o      randomEq.o    					\
	quest.o       recycle.o     save.o        scan.o        signals.o	\
	skills.o      special.o     string.o      tables.o      update.o	

C_FILES = 									\
	act_comm.c    act_enter.c   act_info.c    act_move.c    act_obj.c	\
	act_wiz.c     alias.c       ban.c         bit.c         comm.c		\
	const.c       db.c          db2.c         effects.c     fight.c		\
	flags.c       handler.c     healer.c      hunt.c        illegal.c	\
	interp.c      lookup.c      magic.c       magic2.c      mem.c		\
	mob_cmds.c    mob_prog.c    mount.c       music.c       new_dump.c	\
	note.c        olc.c         olc_act.c     olc_mpcode.c  olc_save.c	\
	pchannels.c   combat.c      randomEq.c   					\
	quest.c       recycle.c     save.c        scan.c        signals.c	\
	skills.c      special.c     string.c      tables.c      update.c	\
	cgi/aowutils.c   cgi/cgiutils.c   cgi/class.c      cgi/library.c	\
	cgi/race.c       cgi/raceclass.c  cgi/warscore.c

H_FILES = 									\
	db.h        interp.h    lookup.h    magic.h     merc.h      mob_cmds.h	\
	music.h     olc.h       recycle.h   tables.h    telnet.h    randomEq.h	\
	cgi/aowutils.h  cgi/cgiutils.h		

M_FILES = Makefile cgi/Makefile

PL_FILE = mudcheck.pl pfc.pl

aow: $(O_FILES)
	rm -f aow
	$(CC) $(L_FLAGS) -o aow $(O_FILES)

backup:
	tar cf - $(C_FILES) $(H_FILES) $(M_FILES) $(PL_FILE) | gzip -9 >../src.tgz

clean:
	rm *.o *.c~ *.h~

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<
