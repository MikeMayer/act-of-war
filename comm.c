
/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1996 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@efn.org)				   *
*	    Gabrielle Taylor						   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdarg.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "merc.h"
#include "recycle.h"
#include "olc.h"

/* command procedures needed */
DECLARE_DO_FUN(do_help);
DECLARE_DO_FUN(do_help2);
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_skills);
DECLARE_DO_FUN(do_outfit);
DECLARE_DO_FUN(do_unread);
DECLARE_DO_FUN(do_quit);
DECLARE_DO_FUN(do_recall);
DECLARE_DO_FUN(do_extra);
DECLARE_DO_FUN(do_tile);

/*
 * Socket and TCP/IP stuff.
 */
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "telnet.h"
#define FNDELAY O_NDELAY
const char echo_off_str[] =
{IAC, WILL, TELOPT_ECHO, '\0'};
const char echo_on_str[] =
{IAC, WONT, TELOPT_ECHO, '\0'};
const char go_ahead_str[] =
{IAC, GA, '\0'};

/*
 * OS-dependent declarations.
 */
/* 
   Linux shouldn't need these. If you have a problem compiling, try
   uncommenting accept and bind.
   int  accept          args( ( int s, struct sockaddr *addr, int *addrlen ) );
   int  bind            args( ( int s, struct sockaddr *name, int namelen ) );
 */

int close args((int fd));
//int   getpeername     args( ( int s, struct sockaddr *name, int *namelen) );
//int   getsockname     args( ( int s, struct sockaddr *name, int *namelen) );
int gettimeofday args((struct timeval * tp, struct timezone * tzp));
//int   listen          args( ( int s, int backlog ) );
int read args((int fd, char *buf, int nbyte));
int select args((int width, fd_set * readfds, fd_set * writefds,
		 fd_set * exceptfds, struct timeval * timeout));
int socket args((int domain, int type, int protocol));
int write args((int fd, char *buf, int nbyte));
void update_toplist(CHAR_DATA *ch);
/*
 * Global variables.
 */
DESCRIPTOR_DATA *descriptor_list;	/* All open descriptors         */
DESCRIPTOR_DATA *d_next;	/* Next descriptor in loop      */
FILE *fpReserve;		/* Reserved file handle         */
bool god;			/* All new chars are gods!      */
bool merc_down;			/* Shutdown                     */
bool wizlock=0;			/* Game is wizlocked            */
bool newlock = FALSE;			/* Game is newlocked            */
bool saltrecall = 0;			/* questmode...ppl recall to saltmines            */
bool armageddon = 0;			/* Game is in armegeddon quest mode            */
char str_boot_time[MAX_INPUT_LENGTH];
time_t current_time;		/* time of this pulse */
bool MOBtrigger = TRUE;		/* act() switch                 */
TOPPLAYER_DATA *main_warlist;
WARSCORE_DATA *main_warscore;

/*
 * Quest Stuff
 */
bool		    quest;		/* Is there a quest running?    */
bool		    auto_quest=TRUE;		/* Quests start on their own?   */
CHAR_DATA       *   quest_mob;		/* Mob which started quest      */
CHAR_DATA	*   quest_target;	/* Target of the quest		*/
OBJ_DATA	*   quest_object;	/* Object to recover		*/
int		    quest_timer;	/* Time left to get object	*/
int		    quest_wait=180;	/* Min time until next quest	*/
sh_int		quest_personality; /* mob's crusade personality :) */

/*
 * OS-dependent local functions.
 */
void game_loop_unix args((int control));
int init_socket args((int port));
void init_descriptor args((int control));
bool read_from_descriptor args((DESCRIPTOR_DATA * d));
bool write_to_descriptor args((int desc, char *txt, int length));

/*
 * Other local functions (OS-independent).
 */
void check_multi args((DESCRIPTOR_DATA * d));
bool check_reconnect args((DESCRIPTOR_DATA * d, char *name,
			   bool fConn));
bool check_playing args((DESCRIPTOR_DATA * d, char *name));
int main args((int argc, char **argv));
void nanny args((DESCRIPTOR_DATA * d, char *argument));
bool process_output args((DESCRIPTOR_DATA * d, bool fPrompt));
void read_from_buffer args((DESCRIPTOR_DATA * d));
void stop_idling args((CHAR_DATA * ch));
void bust_a_prompt args((CHAR_DATA * ch));

/* Needs to be global because of do_copyover */
int port, control; //Izlude copyover change

bool check_multi_new(DESCRIPTOR_DATA * d) //Izlude, moved up here to compile clean ^_^
{
    CHAR_DATA *cd;
    
    if (IS_IMMORTAL(d->character))
    return FALSE;
    
    if (d->character->sameip)
    return FALSE;
    
    for (cd = char_list; cd != NULL; cd = cd->next)
    {   
     if (IS_NPC(cd))
     continue;

     if (IS_SET(cd->act, PLR_FREEZE))
     continue;
          
     if (!str_cmp(d->character->name,cd->name))
     return FALSE;
     
     if (cd->sameip && !str_cmp(d->host, cd->host))
     return FALSE;
     
     if (IS_IMMORTAL(cd) && !str_cmp(d->host, cd->host))
     return FALSE;

     if (!str_cmp(d->host, cd->host))
     return TRUE;
    }
    return FALSE;
}

int main(int argc, char **argv)
{
    struct timeval now_time;
   
   //int port; //Izlude copyover change
   //int control; //Izlude copyover change
     bool fCopyOver = FALSE; //Izlude copyover change

    /*
     * Init time.
     */
    gettimeofday(&now_time, NULL);
    current_time = (time_t) now_time.tv_sec;
    strcpy(str_boot_time, ctime(&current_time));

    /*
     * Reserve one channel for our use.
     */
    if ((fpReserve = fopen(NULL_FILE, "r")) == NULL) {
	perror(NULL_FILE);
	exit(1);
    }
    /*
     * Get the port number.
     */
    port = 4000;
    if (argc > 1) {
	if (!is_number(argv[1])) {
	    fprintf(stderr, "Usage: %s [port #]\n", argv[0]);
	    exit(1);
	} else if ((port = atoi(argv[1])) <= 1024) {
	    fprintf(stderr, "Port number must be above 1024.\n");
	    exit(1);
	}
	
		/* Are we recovering from a copyover? Izlude copyover change*/ 
 	if (argv[2] && argv[2][0])
 	{
 		fCopyOver = TRUE;
 		control = atoi(argv[3]);
 	}
 	else
 		fCopyOver = FALSE;
	//Izlude copyover change end
	
    }
    /*
     * Run the game.
     */
    if (!fCopyOver) //Izlude added copyover from ch5
    control = init_socket(port);
    sprintf(log_buf, "Loading signal handlers.");
    log_string(log_buf);
    init_signals();		/* For the use of the signal handler. -Ferric */
    boot_db();
    sprintf(log_buf, "Act of War %s is ready to rock on port %d.", AOW_VERSION, port);
    log_string(log_buf); 
    if (fCopyOver) //Izlude copyover change
    	copyover_recover(); //Izlude copyover change
    	game_loop_unix(control);
    	close(control);

    /*
     * That's all, folks.
     */
    log_string("Normal termination of game.");
    exit(0);
    return 0;
}

int init_socket(int port)
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x = 1;
    int fd;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	perror("Init_socket: socket");
	exit(1);
    }
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
		   (char *) &x, sizeof(x)) < 0) {
	perror("Init_socket: SO_REUSEADDR");
	close(fd);
	exit(1);
    }
    sa = sa_zero;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
	perror("Init socket: bind");
	close(fd);
	exit(1);
    }
    if (listen(fd, 3) < 0) {
	perror("Init socket: listen");
	close(fd);
	exit(1);
    }
    return fd;
}

void game_loop_unix(int control)
{
    static struct timeval null_time;
    struct timeval last_time;
    //Vashna 07-10-01 Commenting out unused variables.
    //pthread_t thread;
    //pthread_attr_t pattr;

    gettimeofday(&last_time, NULL);
    current_time = (time_t) last_time.tv_sec;

    /* Main loop */
    while (!merc_down) {
	fd_set in_set;
	fd_set out_set;
	fd_set exc_set;
	DESCRIPTOR_DATA *d;
	int maxdesc;

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO(&in_set);
	FD_ZERO(&out_set);
	FD_ZERO(&exc_set);
	FD_SET(control, &in_set);
	maxdesc = control;
	for (d = descriptor_list; d; d = d->next) {
	    maxdesc = UMAX(maxdesc, d->descriptor);
	    FD_SET(d->descriptor, &in_set);
	    FD_SET(d->descriptor, &out_set);
	    FD_SET(d->descriptor, &exc_set);
	}

	if (select(maxdesc + 1, &in_set, &out_set, &exc_set, &null_time) < 0) {
	    perror("Game_loop: select: poll");
	    exit(1);
	}
	/*
	 * New connection?
	 */
	if (FD_ISSET(control, &in_set))
	    init_descriptor(control);


	/*
	 * New connection?
	 * Use a threaded process to create descriptor, reducing lag.
	 */
//        if (FD_ISSET(control,&in_set)) {
//            pthread_attr_setdetachstate(&pattr,PTHREAD_CREATE_DETACHED);
//            pthread_create( &thread, &pattr, (void *) init_descriptor, (void //*)control);
//        }

	/*
	 * Kick out the freaky folks.
	 */

	for (d = descriptor_list; d != NULL; d = d_next) {
	    d_next = d->next;
	    if (FD_ISSET(d->descriptor, &exc_set)) {
		FD_CLR(d->descriptor, &in_set);
		FD_CLR(d->descriptor, &out_set);
		if (d->character &&
		    d->connected == CON_PLAYING &&
		    d->character->level > 1)
		    save_char_obj(d->character);
		d->outtop = 0;
		close_socket(d);
	    }
	}

	/*
	 * Process input.
	 */

	for (d = descriptor_list; d != NULL; d = d_next) {
	    d_next = d->next;
	    d->fcommand = FALSE;

	    if (FD_ISSET(d->descriptor, &in_set)) {
		if (d->character != NULL)
		    d->character->timer = 0;
		if (!read_from_descriptor(d)) {
		    FD_CLR(d->descriptor, &out_set);
		    if (d->character != NULL
			&& d->connected == CON_PLAYING
			&& d->character->level > 1)
			save_char_obj(d->character);
		    d->outtop = 0;
		    close_socket(d);
		    continue;
		}
	    }
	    if (d->character != NULL && d->character->daze > 0)
		--d->character->daze;

	    if (d->character != NULL && d->character->wait > 0) {
		--d->character->wait;
		continue;
	    }

	    read_from_buffer(d);
	    if (d->incomm[0] != '\0') {
		d->fcommand = TRUE;
		stop_idling(d->character);

		/* OLC */
		if (d->showstr_point)
		    show_string(d, d->incomm);
		else if (d->pString)
		    string_add(d->character, d->incomm);
		else
		    switch (d->connected) {
		    case CON_PLAYING:
			if (!run_olc_editor(d))
			    substitute_alias(d, d->incomm);
			break;
		    default:
			nanny(d, d->incomm);
			break;
		    }

		d->incomm[0] = '\0';
	    }
	}

	/*
	 * Autonomous game motion.
	 */

	update_handler();

	/*
	 * Output.
	 */
	for (d = descriptor_list; d != NULL; d = d_next) {
	    d_next = d->next;

	    if ((d->fcommand || d->outtop > 0)
		&& FD_ISSET(d->descriptor, &out_set)) {
		if (!process_output(d, TRUE)) {
		    if (d->character != NULL
			&& d->connected == CON_PLAYING
			&& d->character->level > 1)
			save_char_obj(d->character);
		    d->outtop = 0;
		    close_socket(d);
		}
	    }
	}

	/*
	 * Synchronize to a clock.
	 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
	 * Careful here of signed versus unsigned arithmetic.
	 */

	{
	    struct timeval now_time;
	    long secDelta;
	    long usecDelta;

	    gettimeofday(&now_time, NULL);
	    usecDelta = ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
		+ 1000000 / PULSE_PER_SECOND;
	    secDelta = ((int) last_time.tv_sec) - ((int) now_time.tv_sec);
	    while (usecDelta < 0) {
		usecDelta += 1000000;
		secDelta -= 1;
	    }

	    while (usecDelta >= 1000000) {
		usecDelta -= 1000000;
		secDelta += 1;
	    }

	    if (secDelta > 0 || (secDelta == 0 && usecDelta > 0)) {
		struct timeval stall_time;

		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec = secDelta;
		if (select(0, NULL, NULL, NULL, &stall_time) < 0) {
		    if ( errno != EINTR ) {
		        perror("Game_loop: select: stall");
		        exit(1);
		    }
		}
	    }
	}

	
	gettimeofday(&last_time, NULL);
	current_time = (time_t) last_time.tv_sec;
    }
    sprintf(log_buf, "9999 %d", 1);
    log_string(log_buf);

    return;
}

	//Izlude copyover change
	void logf (char * fmt, ...)
{
	char buf [2*MSL];
	va_list args;
	va_start (args, fmt);
	vsprintf (buf, fmt, args);
	va_end (args);
	
	log_string (buf);
}
// Izlude copyover change


void init_descriptor(int control)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    struct sockaddr_in sock;
    struct hostent *from;
    int desc;
    int size;

    size = sizeof(sock);
    getsockname(control, (struct sockaddr *) &sock, &size);
    if ((desc = accept(control, (struct sockaddr *) &sock, &size)) < 0) {
	perror("New_descriptor: accept");
//t        pthread_exit(NULL);
	return;
    }
    if (fcntl(desc, F_SETFL, FNDELAY) == -1) {
	perror("New_descriptor: fcntl: FNDELAY");
//t        pthread_exit(NULL);
	return;
    }
    /*
     * Cons a new descriptor.
     */
    /*Izlude copyover changes
    dnew = new_descriptor();
    dnew->descriptor = desc;
    dnew->connected = CON_COLOR;
    dnew->showstr_head = NULL;
    dnew->showstr_point = NULL;
    dnew->outsize = 2000;*/ 
    dnew = new_descriptor(); /* new_descriptor now also allocates things */
    dnew->descriptor = desc;
    
    //izlude copyover changes end
    dnew->pEdit = NULL;		/* OLC */
    dnew->pString = NULL;	/* OLC */
    dnew->editor = 0;		/* OLC */
   // dnew->outbuf = alloc_mem(dnew->outsize); //izlude copyover change

    size = sizeof(sock);
    if (getpeername(desc, (struct sockaddr *) &sock, &size) < 0) {
	perror("New_descriptor: getpeername");
	dnew->host = str_dup("(unknown)");
    } else {
	/*
	 * Would be nice to use inet_ntoa here but it takes a struct arg,
	 * which ain't very compatible between gcc and system libraries.
	 */
	int addr;

	addr = ntohl(sock.sin_addr.s_addr);
	sprintf(buf, "%d.%d.%d.%d", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, (addr) & 0xFF );
	sprintf(log_buf, "Sock.sinaddr:  %s", buf);
	log_string(log_buf);
	from = gethostbyaddr((char *) &sock.sin_addr, sizeof(sock.sin_addr), AF_INET);
	dnew->host = str_dup(from ? from->h_name : buf);
    }

    /*
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */
    if (check_ban(dnew->host, BAN_ALL)) {
	write_to_descriptor(desc, "Your site has been banned from this mud.\n\r", 0);
	close(desc);
	free_descriptor(dnew);
//t        pthread_exit(NULL);
	return;
    }


    /*
     * Init descriptor data.
     */
    dnew->next = descriptor_list;
    descriptor_list = dnew;

    /*
     * Send the greeting.
     */
    write_to_buffer2(dnew, "Greetings ",0);
    write_to_buffer2(dnew, dnew->host, 0);
    write_to_buffer2(dnew, " do you want full ANSI color text? ", 0);

 //t   pthread_exit(NULL);
    return;
}

void close_socket(DESCRIPTOR_DATA * dclose)
{
    CHAR_DATA *ch;

    if (dclose->outtop > 0)
	process_output(dclose, FALSE);

    if (dclose->snoop_by != NULL) {
	write_to_buffer(dclose->snoop_by,
			"Your victim has left the game.\n\r", 0);
    } {
	DESCRIPTOR_DATA *d;

	for (d = descriptor_list; d != NULL; d = d->next) {
	    if (d->snoop_by == dclose)
		d->snoop_by = NULL;
	}
    }

    if ((ch = dclose->character) != NULL) {

//	sprintf(log_buf, "%s@ has lostlink in %s", ch->name, kingdom_table[ch->side].name);


	sprintf(log_buf, "%s@%s has lostlink in %s", ch->name, dclose->host, kingdom_table[ch->side].name);
//	sprintf(log_buf, "Closing link to %s.", ch->name);
	log_string(log_buf);
	/* cut down on wiznet spam when rebooting */
	if (dclose->connected == CON_PLAYING && !merc_down) {
	    act("$n has lost $s link.", ch, NULL, NULL, TO_ROOM);
	    wiznet("Net death has claimed $N.", ch, NULL, WIZ_LINKS, 0, 0);
	    ch->desc = NULL;
	} else {
	    free_char(dclose->original ? dclose->original :
		      dclose->character);
	}
    }
    if (d_next == dclose)
	d_next = d_next->next;

    if (dclose == descriptor_list) {
	descriptor_list = descriptor_list->next;
    } else {
	DESCRIPTOR_DATA *d;

	for (d = descriptor_list; d && d->next != dclose; d = d->next);
	if (d != NULL)
	    d->next = dclose->next;
	else
	    bug("Close_socket: dclose not found.", 0);
    }

    close(dclose->descriptor);
    free_descriptor(dclose);
    return;
}

bool read_from_descriptor(DESCRIPTOR_DATA * d)
{
    int iStart;

    /* Hold horses if pending command already. */
    if (d->incomm[0] != '\0')
	return TRUE;

    /* Check for overflow. */
    iStart = strlen(d->inbuf);
    if (iStart >= sizeof(d->inbuf) - 10) {
	sprintf(log_buf, "%s input overflow!", d->host);
	log_string(log_buf);
	write_to_descriptor(d->descriptor,
			    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0);
	return FALSE;
    }
    /* Snarf input. */
    for (;;) {
	int nRead;

	nRead = read(d->descriptor, d->inbuf + iStart,
		     sizeof(d->inbuf) - 10 - iStart);
	if (nRead > 0) {
	    iStart += nRead;
	    if (d->inbuf[iStart - 1] == '\n' || d->inbuf[iStart - 1] == '\r')
		break;
	} else if (nRead == 0) {
	    log_string("EOF encountered on read.");
	    return FALSE;
	} else if (errno == EWOULDBLOCK)
	    break;
	else {
	    perror("Read_from_descriptor");
	    return FALSE;
	}
    }

    d->inbuf[iStart] = '\0';
    return TRUE;
}

/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer(DESCRIPTOR_DATA * d)
{
    int i, j, k;

    /*
     * Hold horses if pending command already.
     */
    if (d->incomm[0] != '\0')
	return;

    /*
     * Look for at least one new line.
     */
    for (i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++) {
	if (d->inbuf[i] == '\0')
	    return;
    }

    /*
     * Canonical input processing.
     */
    for (i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++) {
	if (k >= MAX_INPUT_LENGTH - 2) {
	    write_to_descriptor(d->descriptor, "Line too long.\n\r", 0);

	    /* skip the rest of the line */
	    for (; d->inbuf[i] != '\0'; i++) {
		if (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
		    break;
	    }
	    d->inbuf[i] = '\n';
	    d->inbuf[i + 1] = '\0';
	    break;
	}
	if (d->inbuf[i] == '\b' && k > 0)
	    --k;
	else if (isascii(d->inbuf[i]) && isprint(d->inbuf[i]))
	    d->incomm[k++] = d->inbuf[i];
    }

    /*
     * Finish off the line.
     */
    if (k == 0)
	d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Deal with bozos with #repeat 1000 ...
     */

    if (k > 1 || d->incomm[0] == '!') {
	if (d->incomm[0] != '!' && strcmp(d->incomm, d->inlast)) {
	    d->repeat = 0;
	} else {
	    if (++d->repeat >= 25 && d->character
		&& d->connected == CON_PLAYING) {
		sprintf(log_buf, "%s input spamming!", d->host);
		log_string(log_buf);
		wiznet("Spam spam spam{R $N {xspam spam spam spam spam!",
		       d->character, NULL, WIZ_SPAM, 0, get_trust(d->character));
		if (d->incomm[0] == '!')
		    wiznet(d->inlast, d->character, NULL, WIZ_SPAM, 0,
			   get_trust(d->character));
		else
		    wiznet(d->incomm, d->character, NULL, WIZ_SPAM, 0,
			   get_trust(d->character));

		d->repeat = 0;
/*
   write_to_descriptor( d->descriptor,
   "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
   strcpy( d->incomm, "quit" );
 */
	    }
	}
    }
    /*
     * Do '!' substitution.
     */
    if (d->incomm[0] == '!')
	strcpy(d->incomm, d->inlast);
    else
	strcpy(d->inlast, d->incomm);

    /*
     * Shift the input buffer.
     */
    while (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
	i++;
    for (j = 0; (d->inbuf[j] = d->inbuf[i + j]) != '\0'; j++);
    return;
}

/*
 * Low level output function.
 */
bool process_output(DESCRIPTOR_DATA * d, bool fPrompt)
{
    extern bool merc_down;

    /*
     * Bust a prompt.
     */
    if (!merc_down){
	if (d->showstr_point)
	    write_to_buffer(d, "[Hit Return to continue]\n\r", 0);
	else if (fPrompt && d->pString && d->connected == CON_PLAYING)
	    write_to_buffer(d, "> ", 2);
	else if (fPrompt && d->connected == CON_PLAYING) {
	    CHAR_DATA *ch;
	    CHAR_DATA *victim;

	    ch = d->character;

	    /* battle prompt */
	    if ((victim = ch->fighting) != NULL && can_see(ch, victim)) {
		int percent;
		char *pbuff;
		char buf[MAX_STRING_LENGTH];
		char buffer[MAX_STRING_LENGTH * 2];
		char buf2[MAX_STRING_LENGTH];
		char pers_buf[MAX_STRING_LENGTH];

		if (victim->max_hit > 0)
		    percent = victim->hit * 100 / victim->max_hit;
		else
		    percent = -1;
		PERS(victim, ch, pers_buf);
		sprintf(buf2, "%s", pers_buf);

		if (percent >= 100) {
		    sprintf(buf, "{g%s is in excellent condition.{x\n\r", buf2);
		} else if (percent >= 90)
		    sprintf(buf, "{g%s has a few scratches.{x\n\r",
			    buf2);
		else if (percent >= 75)
		    sprintf(buf, "{g%s has some small wounds and bruises.{x\n\r",
			    buf2);
		else if (percent >= 50)
		    sprintf(buf, "{y%s has quite a few wounds.{x\n\r",
			    buf2);
		else if (percent >= 30)
		    sprintf(buf, "{y%s has some nasty wounds and scratches.{x\n\r",
			    buf2);
		else if (percent >= 15)
		    sprintf(buf, "{r%s looks pretty hurt.{x\n\r",
			    buf2);
		else if (percent >= 0)
		    sprintf(buf, "{r%s is in awful condition.{x\n\r",
			    buf2);
		else
		    sprintf(buf, "{r%s is bleeding to death.{x\n\r",
			    buf2);

		buf[2] = UPPER(buf[2]);
		pbuff = buffer;

		colourconv(pbuff, buf, d->character);

		if (d->character != NULL)
		    if (!IS_SET(d->character->act, PLR_EXTRA))
			write_to_buffer(d, buffer, 0);
	    }
	    ch = d->original ? d->original : d->character;
	    if (!IS_SET(ch->comm, COMM_COMPACT))
		write_to_buffer(d, "\n\r", 2);

	    if (IS_SET(ch->comm, COMM_PROMPT))
		bust_a_prompt(d->character);

	    if (IS_SET(ch->comm, COMM_TELNET_GA))
		write_to_buffer(d, go_ahead_str, 0);
	}
    }
    /*
     * Short-circuit if nothing to write.
     */
    if (d->outtop == 0)
	return TRUE;

    /*
     * Snoop-o-rama.
     */
    if (d->snoop_by != NULL) {
	if (d->character != NULL)
	    write_to_buffer(d->snoop_by, d->character->name, 0);
	write_to_buffer(d->snoop_by, "> ", 2);
	write_to_buffer(d->snoop_by, d->outbuf, d->outtop);
    }
    /*
     * OS-dependent output.
     */
    if (!write_to_descriptor(d->descriptor, d->outbuf, d->outtop)) {
	d->outtop = 0;
	return FALSE;
    } else {
	d->outtop = 0;
	return TRUE;
    }
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt(CHAR_DATA * ch)
{

    ROOM_INDEX_DATA *room;
    OBJ_INDEX_DATA  *obj;
    MOB_INDEX_DATA  *mob;
    char msg[MAX_STRING_LENGTH];   /* Mode */
    char msg2[MAX_STRING_LENGTH];  /* what */
    char msg3[MAX_STRING_LENGTH];  /* all */
    char msg4[MAX_STRING_LENGTH];  /* NESW */
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    const char *str;
    const char *i;
    char *point;
    char *pbuff;
    char buffer[MAX_STRING_LENGTH * 2];
    char doors[MAX_INPUT_LENGTH];
    char doors2[MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    EXIT_DATA *pexit2;
    bool found;
    bool foundz;
    bool show_prompt;
    const char *dir_name[] =
    {"N", "E", "S", "W", "U", "D"};
    int door;
    int doorx2;
    /* for enemy life meter */
    CHAR_DATA *victim;
    char wound[MAX_STRING_LENGTH];
    int percent;

    point = buf;
    str = ch->prompt;
    i = "";

    show_prompt = TRUE;

    if (!str || str[0] == '\0') {
	sprintf(buf, "<{r%d{xhp {b%d{xm {g%d{xmv> %s",
		ch->hit, ch->mana, ch->move, ch->prefix);
	send_to_char(buf, ch);
	return;
    }
    if (IS_SET(ch->act, PLR_AUTOBUILD)) {
	send_to_char("{Y<AUTOBUILD>{x", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_AFK)) {
	send_to_char("{g[AFK]{x ", ch);
	return;
    }
      if ( ch->desc->editor == ED_ROOM )
      {
	 sprintf( msg, "Redit" );
	 room = get_room_index( ch->in_room->vnum ); 
	    if (room != NULL)
           {
	     sprintf( msg2, "[%5d]: %s ", ch->in_room->vnum, room->name );
           
	    foundz = FALSE;
	    doors2[0] = '\0';
	    for (doorx2 = 0; doorx2 < 6; doorx2++) {
		if ((pexit2 = ch->in_room->exit[doorx2]) != NULL && pexit2->u1.to_room != NULL)
               {
		    foundz = TRUE;
		    strcat(doors2, dir_name[doorx2]);
		   }
	    }

	    if (!foundz)
            {
		strcat(doors2, "none");
            }
	    sprintf(msg4, "%s", doors2);
           
           }
      sprintf( msg3, "< %s %s > %s ", msg, msg2, msg4 );
      write_to_buffer( ch->desc, msg3, 0); 
      return;
      }
      
     if ( ch->desc->editor == ED_OBJECT )
      {   
	 sprintf( msg, "Oedit" );
	    obj = get_obj_index( ch->build_vnum );
	    if (obj != NULL)
              {
	     sprintf( msg2, "[%5d]: %s", obj->vnum, obj->short_descr );
              }
      sprintf( msg3, "< %s %s >", msg, msg2 );
      write_to_buffer( ch->desc, msg3, 0);
      return;

      }

      if ( ch->desc->editor == ED_MOBILE )
      {   
	 sprintf( msg, "Medit" );
	    mob = get_mob_index( ch->build_vnum );
	    if (mob != NULL)
           {
  	     sprintf( msg2, "[%5d]: %s", mob->vnum, mob->short_descr );
	 }
      sprintf( msg3, "< %s %s >", msg, msg2 );
      write_to_buffer( ch->desc, msg3, 0);
      return;
      }


    while (*str != '\0') {
	if (*str != '%') {
	    *point++ = *str++;
	    continue;
	}
	++str;
	switch (*str) {
	    int hits;
	    int manas;
	    int moves;
	default:
	    i = " ";
	    break;
	case 'e':
	    found = FALSE;
	    doors[0] = '\0';
	    for (door = 0; door < 6; door++) {
		if ((pexit = ch->in_room->exit[door]) != NULL
		    && pexit->u1.to_room != NULL
		    && (can_see_room(ch, pexit->u1.to_room)
			|| (IS_AFFECTED(ch, AFF_INFRARED)
			    && !IS_AFFECTED(ch, AFF_BLIND)))
		    && !IS_SET(pexit->exit_info, EX_CLOSED)) {
		    found = TRUE;
		    strcat(doors, dir_name[door]);
		}
	    }
	    if (!found)
		strcat(doors, "none");
	    sprintf(buf2, "%s", doors);
	    i = buf2;
	    break;
	case 'c':
	    sprintf(buf2, "%s", "\n\r");
	    i = buf2;
	    break;
	case 'h':
	    if (ch->max_hit != 0)
		hits = (ch->hit * 100) / ch->max_hit;
	    else
		hits = -1;
	    if (hits >= 100)
		sprintf(buf2, "%s%d%s", "{G", ch->hit, "{x"
		    );
	    else if (hits >= 75)
		sprintf(buf2, "%s%d%s", "{g", ch->hit,
			"{x");
	    else if (hits >= 25)
		sprintf(buf2, "%s%d%s", "{Y", ch->hit,
			"{x");
	    else if (hits >= 0)
		sprintf(buf2, "%s%d%s", "{r", ch->hit,
			"{x");
	    else
		sprintf(buf2, "%s%d%s", "{R", ch->hit, "{x");

	    i = buf2;
	    break;
	case 'H':
	    sprintf(buf2, "%d", ch->max_hit);
	    i = buf2;
	    break;
	case 'm':
	    if (ch->max_mana != 0)
		manas = (ch->mana * 100) / ch->max_mana;
	    else
		manas = -1;
	    if (manas >= 100)
		sprintf(buf2, "%s%d%s", "{G", ch->mana,
			"{x");
	    else if (manas >= 75)
		sprintf(buf2, "%s%d%s", "{g",
			ch->mana, "{x");
	    else if (manas >= 25)
		sprintf(buf2, "%s%d%s", "{Y", ch->mana,
			"{x");
	    else if (manas >= 0)
		sprintf(buf2, "%s%d%s", "{r", ch->mana,
			"{x");
	    else
		sprintf(buf2, "%s%d%s", "{R", ch->mana, "{x");

	    i = buf2;
	    break;
	case 'M':
	    sprintf(buf2, "%d", ch->max_mana);
	    i = buf2;
	    break;
	case 'v':
	    if (ch->max_move != 0)
		moves = (ch->move * 100) / ch->max_move;
	    else
		moves = -1;
	    if (moves >= 100)
		sprintf(buf2, "%s%d%s", "{G", ch->move,
			"{x");
	    else if (moves >= 75)
		sprintf(buf2, "%s%d%s", "{g",
			ch->move, "{x");
	    else if (moves >= 25)
		sprintf(buf2, "%s%d%s", "{Y", ch->move,
			"{x");
	    else if (moves >= 0)
		sprintf(buf2, "%s%d%s", "{r", ch->move,
			"{x");
	    else
		sprintf(buf2, "%s%d%s", "{R", ch->hit, "{x");

	    i = buf2;
	    break;
	case 'V':
	    sprintf(buf2, "%d", ch->max_move);
	    i = buf2;
	    break;
	case 'x':
	    sprintf(buf2, "%d", ch->exp);
	    i = buf2;
	    break;
	case 'X':
	    sprintf(buf2, "%d", IS_NPC(ch) ? 0 :
		    (ch->level + 1) * exp_per_level(ch, ch->pcdata->points) - ch->exp);
	    i = buf2;
	    break;
	case 'g':
	    sprintf(buf2, "%ld", ch->gold);
	    i = buf2;
	    break;
	case 's':
	    sprintf(buf2, "%ld", ch->silver);
	    i = buf2;
	    break;
	case 'a':
	    if (ch->level > 9)
		sprintf(buf2, "%d", ch->alignment);
	    else
		sprintf(buf2, "%s", IS_GOOD(ch) ? "good" : IS_EVIL(ch) ?
			"evil" : "neutral");
	    i = buf2;
	    break;
	case 'r':
	    if (ch->in_room != NULL)
		sprintf(buf2, "%s",
			((!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT)) ||
			 (!IS_AFFECTED(ch, AFF_BLIND) && !room_is_dark(ch->in_room)))
			? ch->in_room->name : "darkness");
	    else
		sprintf(buf2, " ");
	    i = buf2;
	    break;
	case 'R':
	    if (IS_IMMORTAL(ch) && ch->in_room != NULL)
		sprintf(buf2, "%d", ch->in_room->vnum);
	    else
		sprintf(buf2, " ");
	    i = buf2;
	    break;
	case 'z':
	    if (IS_IMMORTAL(ch) && ch->in_room != NULL)
		sprintf(buf2, "%s", ch->in_room->area->name);
	    else
		sprintf(buf2, " ");
	    i = buf2;
	    break;
	case '%':
	    sprintf(buf2, "%%");
	    i = buf2;
	    break;
	case 'G':
	switch (ch->in_room->sector_type) {
    case (SECT_INSIDE):
	case (SECT_CITY):
	case (SECT_ROAD):
	case (SECT_RUINS):
	case (SECT_STREET):
	case (SECT_BRIDGE ):
    case (SECT_ALLEY):
	case (SECT_HOUSE):
	case (SECT_WOODED_ROAD): 
	case (SECT_DOCK):
	case (SECT_BRICK_ROAD):
	case (SECT_BRICK_ROAD2):
	case (SECT_BCASTLE):
	case (SECT_BCASTLE2):
	case (SECT_BCASTLE3):
	// heal self, chance for a group heal
	sprintf(buf2, "[{yRoad{x]");
	i = buf2;
	break;
		
	case (SECT_FIELD):
	case (SECT_PLAINS):
	case (SECT_FARMLAND):
	case (SECT_SCRUBLAND):
	case (SECT_WHEATFIELD):
	//damage victim with bash, chance to slow victim
	sprintf(buf2, "[{GPlains{x]");
	i = buf2;
	break;

	case (SECT_JUNGLE):
	case (SECT_FOREST):
	case (SECT_DENSE_FOREST):
	case (SECT_DARKWOOD):
	//(small) chance to entangle victim, damage with wood
	sprintf(buf2, "[{gForest{x]");
	i = buf2;
	break;

	case (SECT_HILLS):
	case (SECT_MOUNTAIN):
	case (SECT_DIRTPATH):
	// good damage, bash, daze state
	sprintf(buf2, "[{MMnts{X]");
	i = buf2;
	break;

	case (SECT_WATER_SWIM):	
	case (SECT_OCEAN):
	case (SECT_WATER_NOSWIM):
	case (SECT_FROZEN_SEA):
	case (SECT_WHIRLPOOL):
	// area drowning damage, daze state
	sprintf(buf2, "[{BWater{x]");
	i = buf2;
	break;

	case (SECT_DESERT):
	case (SECT_BEACH):
	case (SECT_QUICKSAND):
	// pierce damage to victim, chance to blind 0 tic
	sprintf(buf2, "[{YSand{x]");
	i = buf2;
	break;
	
	case (SECT_WASTELAND):
	case (SECT_SWAMP):
	case (SECT_BOG):
	case (SECT_DARK_SWAMP):
	// disease damage to victim, chance to poison and plague
	sprintf(buf2, "[{DSwamp{x]");
	i = buf2;
	break;

	case (SECT_TUNDRA):
	case (SECT_ICE):
	case (SECT_ICE_MOUNTAIN):
	case (SECT_FROZEN_FOREST):
	// high cold damage to victim, nothing else
	sprintf(buf2, "[{cIce{x]");
	i = buf2;
	break;
	
	case (SECT_LAVA):
	case (SECT_LAVACHUTE):
	case (SECT_FIRE_ROOM):
	// high fire damage to victim, mirror of afft 7
	sprintf(buf2, "[{RFire{x]");
	i = buf2;
	break;
	
	case (SECT_PYRAMID):
	case (SECT_UNDERGROUND): 
	case (SECT_CAVE):
	case (SECT_CAVERN):
	case (SECT_EARTH):
	case (SECT_CAVE_FLOOR):
	// area bash spell, chance to knock group to feet
	sprintf(buf2, "[{mCavern{x]");
	i = buf2;
	break;
	
	case (SECT_BLOOD_RIVER):
	case (SECT_REDOCEAN):
	// light damage to victim, chance to curse
	sprintf(buf2, "[{rBlood{x]");
	i = buf2;
	break;

	case (SECT_AIR):
	case (SECT_WIND):
	case (SECT_TORNADO):
	// area pierce damage to victim, chance to blind
	sprintf(buf2, "[{CAir{x]");;
	i = buf2;
	break;
	
    default:
    sprintf(buf2, "[{wS{yp{ge{rc{ci{ba{ml{x]");
    i = buf2;
    // haste, sanc user, only 2 tiles use this.
	break;
    }
    break;
	case 'o':
	    sprintf(buf2, "%s", olc_ed_name(ch));
	    i = buf2;
	    break;
	case 'O':
	    sprintf(buf2, "%s", olc_ed_vnum(ch));
	    i = buf2;
	    break;
	case 'b':
	    if ((victim = ch->fighting) != NULL) {
		if (victim->max_hit > 0)
		    percent = victim->hit * 100 / victim->max_hit;
		else
		    percent = -1;
		if (percent >= 100)
		    sprintf(wound, "{wEnemy: [{R+++{Y+++{G++++{x]");
		else if (percent >= 90)
		    sprintf(wound, "{wEnemy: [{R+++{Y+++{G+++ {x]");
		else if (percent >= 80)
		    sprintf(wound, "{wEnemy: [{R+++{Y+++{G++  {x]");
		else if (percent >= 70)
		    sprintf(wound, "{wEnemy: [{R+++{Y+++{G+   {x]");
		else if (percent >= 58)
		    sprintf(wound, "{wEnemy: [{R+++{Y+++    {x]");
		else if (percent >= 45)
		    sprintf(wound, "{wEnemy: [{R+++{Y++     {x]");
		else if (percent >= 30)
		    sprintf(wound, "{wEnemy: [{R+++{Y+      {x]");
		else if (percent >= 28)
		    sprintf(wound, "{wEnemy: [{R+++{x       {x]");
		else if (percent >= 15)
		    sprintf(wound, "{wEnemy: [{R++{x        {x]");
		else if (percent >= 8)
		    sprintf(wound, "{wEnemy: [{R+{x         {x]");
		else
		    sprintf(wound, "{wEnemy: [          {x]");
		sprintf(buf2, " %s", wound);
		i = buf2;
	    }
	    break;
	case 'P':
	    if ((victim = ch->fighting) != NULL) {
		percent = victim->hit * 100 / victim->max_hit;
		if (percent >= 65)
		    sprintf(buf2, " {wEnemy: {G%d{x%%", percent);
		else if (percent >= 25 && percent < 65)
		    sprintf(buf2, " {wEnemy: {Y%d{x%%", percent);
		else
		    sprintf(buf2, " {wEnemy: {R%d{x%%", percent);
		i = buf2;
	    } else
		i = "";
	    break;

	case 'p':
	    /* Battle prompt rocks! :) -Ttebroc plus only players get it */
	    if (!IS_NPC(ch) && ch->fighting && IS_SET(ch->act, PLR_EXTRA)) {
		CHAR_DATA *target;
		CHAR_DATA *tank;
		int target_p, tank_p, self_p;
		char pers_buf[MAX_STRING_LENGTH];

		target = ch->fighting;
		tank = target->fighting;

		show_prompt = FALSE;

		if (target) {
		    if (target->max_hit != 0)
			target_p = (target->hit * 100) / target->max_hit;
		    else
			target_p = -1;
		    PERS(target, ch, pers_buf);
		    sprintf(buf2, "[{w%s{x: ", pers_buf);

		    if (target_p >= 100)
			strcat(buf2, "{gPerfect");
		    else if (target_p >= 90)
			strcat(buf2, "{gScratches");
		    else if (target_p >= 80)
			strcat(buf2, "{gBruises");
		    else if (target_p >= 70)
			strcat(buf2, "{ySome Cuts");
		    else if (target_p >= 60)
			strcat(buf2, "{yWounds");
		    else if (target_p >= 50)
			strcat(buf2, "{yNasty Wounds");
		    else if (target_p >= 40)
			strcat(buf2, "{yBleeding");
		    else if (target_p >= 30)
			strcat(buf2, "{rBlood Covered");
		    else if (target_p >= 20)
			strcat(buf2, "{rLeaking Guts");
		    else if (target_p >= 10)
			strcat(buf2, "{rAlmost Dead");
		    else
			strcat(buf2, "{RDYING");

		    strcat(buf2, "{x] ");

		    i = buf2;

		    while ((*point = *i) != '\0')
			++point, ++i;

		}
		if (tank && tank != ch) {
		    if (tank->max_hit != 0)
			tank_p = (tank->hit * 100) / tank->max_hit;
		    else
			tank_p = -1;

		    PERS(tank, ch, pers_buf);
		    sprintf(buf2, "[{w%s{x: ", pers_buf);

		    if (tank_p >= 100)
			strcat(buf2, "{gPerfect");
		    else if (tank_p >= 90)
			strcat(buf2, "{gScratches");
		    else if (tank_p >= 80)
			strcat(buf2, "{gBruises");
		    else if (tank_p >= 70)
			strcat(buf2, "{ySome Cuts");
		    else if (tank_p >= 60)
			strcat(buf2, "{yWounds");
		    else if (tank_p >= 50)
			strcat(buf2, "{yNasty Wounds");
		    else if (tank_p >= 40)
			strcat(buf2, "{yBleeding");
		    else if (tank_p >= 30)
			strcat(buf2, "{rBlood Covered");
		    else if (tank_p >= 20)
			strcat(buf2, "{rLeaking Guts");
		    else if (tank_p >= 10)
			strcat(buf2, "{rAlmost Dead");
		    else
			strcat(buf2, "{RDYING");

		    strcat(buf2, "{x] ");

		    i = buf2;

		    while ((*point = *i) != '\0')
			++point, ++i;
		}
		/* Show status of player */

		if (ch->max_hit != 0)
		    self_p = (ch->hit * 100) / ch->max_hit;
		else
		    self_p = -1;

		sprintf(buf2, "[{wYou{x: ");
		if (self_p >= 100)
		    strcat(buf2, "{gPerfect");
		else if (self_p >= 90)
		    strcat(buf2, "{gScratches");
		else if (self_p >= 80)
		    strcat(buf2, "{gBruises");
		else if (self_p >= 70)
		    strcat(buf2, "{ySome Cuts");
		else if (self_p >= 60)
		    strcat(buf2, "{yWounds");
		else if (self_p >= 50)
		    strcat(buf2, "{yNasty Wounds");
		else if (self_p >= 40)
		    strcat(buf2, "{yBleeding");
		else if (self_p >= 30)
		    strcat(buf2, "{rBlood Covered");
		else if (self_p >= 20)
		    strcat(buf2, "{rLeaking Guts");
		else if (self_p >= 10)
		    strcat(buf2, "{rAlmost Dead");
		else
		    strcat(buf2, "{RDYING");

		strcat(buf2, "{x]");

		i = buf2;

		while ((*point = *i) != '\0')
		    ++point, ++i;
	    }
	    break;

	}

	++str;
	while ((*point = *i) != '\0')
	    ++point, ++i;
    }

    /* Battle prompt rocks! :) -Ttebroc plus only players get it */
    if (show_prompt == TRUE)
	if (!IS_NPC(ch) && ch->fighting && IS_SET(ch->act, PLR_EXTRA)) {
	    CHAR_DATA *target;
	    CHAR_DATA *tank;
	    int target_p, tank_p, self_p;
	    char pers_buf[MAX_STRING_LENGTH];

	    target = ch->fighting;
	    tank = target->fighting;

	    if (target) {
		if (target->max_hit != 0)
		    target_p = (target->hit * 100) / target->max_hit;
		else
		    target_p = -1;

		PERS(target, tank, pers_buf);
		sprintf(buf2, "[{w%s{x: ", pers_buf);

		if (target_p >= 100)
		    strcat(buf2, "{gPerfect");
		else if (target_p >= 90)
		    strcat(buf2, "{gScratches");
		else if (target_p >= 80)
		    strcat(buf2, "{gBruises");
		else if (target_p >= 70)
		    strcat(buf2, "{ySome Cuts");
		else if (target_p >= 60)
		    strcat(buf2, "{yWounds");
		else if (target_p >= 50)
		    strcat(buf2, "{yNasty Wounds");
		else if (target_p >= 40)
		    strcat(buf2, "{yBleeding");
		else if (target_p >= 30)
		    strcat(buf2, "{rBlood Covered");
		else if (target_p >= 20)
		    strcat(buf2, "{rLeaking Guts");
		else if (target_p >= 10)
		    strcat(buf2, "{rAlmost Dead");
		else
		    strcat(buf2, "{RDYING");

		strcat(buf2, "{x] ");

		i = buf2;

		while ((*point = *i) != '\0')
		    ++point, ++i;

	    }
	    if (tank && tank != ch) {
		if (tank->max_hit != 0)
		    tank_p = (tank->hit * 100) / tank->max_hit;
		else
		    tank_p = -1;

		PERS(tank, ch, pers_buf);
		sprintf(buf2, "[{w%s{x: ", pers_buf);

		if (tank_p >= 100)
		    strcat(buf2, "{gPerfect");
		else if (tank_p >= 90)
		    strcat(buf2, "{gScratches");
		else if (tank_p >= 80)
		    strcat(buf2, "{gBruises");
		else if (tank_p >= 70)
		    strcat(buf2, "{ySome Cuts");
		else if (tank_p >= 60)
		    strcat(buf2, "{yWounds");
		else if (tank_p >= 50)
		    strcat(buf2, "{yNasty Wounds");
		else if (tank_p >= 40)
		    strcat(buf2, "{yBleeding");
		else if (tank_p >= 30)
		    strcat(buf2, "{rBlood Covered");
		else if (tank_p >= 20)
		    strcat(buf2, "{rLeaking Guts");
		else if (tank_p >= 10)
		    strcat(buf2, "{rAlmost Dead");
		else
		    strcat(buf2, "{RDYING");

		strcat(buf2, "{x] ");

		i = buf2;

		while ((*point = *i) != '\0')
		    ++point, ++i;
	    }
	    /* Show status of player */

	    if (ch->max_hit != 0)
		self_p = (ch->hit * 100) / ch->max_hit;
	    else
		self_p = -1;

	    sprintf(buf2, "[{wYou{x: ");
	    if (self_p >= 100)
		strcat(buf2, "{gPerfect");
	    else if (self_p >= 90)
		strcat(buf2, "{gScratches");
	    else if (self_p >= 80)
		strcat(buf2, "{gBruises");
	    else if (self_p >= 70)
		strcat(buf2, "{ySome Cuts");
	    else if (self_p >= 60)
		strcat(buf2, "{yWounds");
	    else if (self_p >= 50)
		strcat(buf2, "{yNasty Wounds");
	    else if (self_p >= 40)
		strcat(buf2, "{yBleeding");
	    else if (self_p >= 30)
		strcat(buf2, "{rBlood Covered");
	    else if (self_p >= 20)
		strcat(buf2, "{rLeaking Guts");
	    else if (self_p >= 10)
		strcat(buf2, "{rAlmost Dead");
	    else
		strcat(buf2, "{RDYING");

	    strcat(buf2, "{x] ");

	    i = buf2;

	    while ((*point = *i) != '\0')
		++point, ++i;
	}
    *point = '\0';
    pbuff = buffer;
    colourconv(pbuff, buf, ch);
    write_to_buffer(ch->desc, buffer, 0);

    if (ch->prefix[0] != '\0')
	write_to_buffer(ch->desc, ch->prefix, 0);
    return;
}

void write_to_buffer(DESCRIPTOR_DATA * d, const char *txt, int length)
{
    const char *point;
    char *point2;
    char buf[MAX_STRING_LENGTH * 4];
    int skip = 0;

    buf[0] = '\0';
    point2 = buf;

/* hog */
    for (point = txt; *point; point++) {
	if (*point == '{') {
	    point++;
	    if (d->color == TRUE)
		skip = colour2(*point, point2);

	    while (skip-- > 0)
		++point2;
	    continue;
	}
	*point2 = *point;
	*++point2 = '\0';
    }

    *point2 = '\0';
    write_to_buffer2(d, buf, point2 - buf);

}

/*
 * Append onto an output buffer.
 */
void write_to_buffer2(DESCRIPTOR_DATA * d, const char *txt, int length)
{

    /*
     * Find length in case caller didn't.
     */
    if (length <= 0)
	length = strlen(txt);

    /*
     * Initial \n\r if needed.
     */
    if (d->outtop == 0 && !d->fcommand) {
	d->outbuf[0] = '\n';
	d->outbuf[1] = '\r';
	d->outtop = 2;
    }
    /*
     * Expand the buffer as needed.
     */
    while (d->outtop + length >= d->outsize) {
	char *outbuf;

	if (d->outsize >= 32000) {
	    bug("Buffer overflow. Closing.\n\r", 0);
	    bug(d->character->name, 0);
	    bug("Was responsible!", 0);
	    close_socket(d);
	    return;
	}
	outbuf = alloc_mem(2 * d->outsize);
	strncpy(outbuf, d->outbuf, d->outtop);
	free_mem(d->outbuf, d->outsize);
	d->outbuf = outbuf;
	d->outsize *= 2;
    }

    /*
     * Copy.TEST
     */

    strcpy(d->outbuf + d->outtop, txt);
    d->outtop += length;
    return;
}

/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor(int desc, char *txt, int length)
{
    int iStart;
    int nWrite;
    int nBlock;

    if (length <= 0)
	length = strlen(txt);

    for (iStart = 0; iStart < length; iStart += nWrite) {
	nBlock = UMIN(length - iStart, 4096);
	if ((nWrite = write(desc, txt + iStart, nBlock)) < 0) {
	    perror("Write_to_descriptor");
	    return FALSE;
	}
    }

    return TRUE;
}

/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny(DESCRIPTOR_DATA * d, char *argument)
{
    DESCRIPTOR_DATA *d_old, *d_next;
    char buf[MAX_STRING_LENGTH];
    char bufrace[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *ch;
    char *pwdnew;
    char *p;
    int iClass, race, i, weapon, class;
    bool fOld;
    AFFECT_DATA *paf;
    CHAR_DATA *vch;

    while (isspace(*argument))
	argument++;

    ch = d->character;

    switch (d->connected) {

    default:
	bug("Nanny: bad d->connected %d.", d->connected);
	close_socket(d);
	return;

    case CON_COLOR:

	switch (*argument) {
	case 'n':
	case 'N':
	    d->color = FALSE;
	    write_to_buffer(d, "\n\r No color will be displayed.", 0);
	    break;

	default:
	    d->color = TRUE;
	    break;
	}

	d->connected = CON_GET_NAME;
	write_to_buffer(d, "\n\r\n\r\n\r", 0);

	/*
	 * Send the greeting.
	 */

	{

	    extern char *help_greeting;
	    if (help_greeting[0] == '.')
		write_to_buffer(d, help_greeting + 1, 0);
	    else
		write_to_buffer(d, help_greeting, 0);

	}

	break;

    case CON_GET_NAME:
	if (argument[0] == '\0') {
	    close_socket(d);
	    return;
	}
	argument[0] = UPPER(argument[0]);
	if (!check_parse_name(argument)) {
	    write_to_buffer(d, "{WIllegal name, try another.{x\n\r\n\rName:  ", 0);
	    return;
	}
	fOld = load_char_obj(d, argument);
	ch = d->character;

	if (IS_SET(ch->act, PLR_DENY)) {
	    sprintf(log_buf, "Denying access to %s@%s.", argument, d->host);
	    log_string(log_buf);
	    write_to_buffer(d, "{WYou are denied access to AoW.{x\n\r", 0);
	    close_socket(d);
	    return;
	}
	if (check_ban(d->host, BAN_PERMIT) && !IS_SET(ch->act, PLR_PERMIT)) {
	    write_to_buffer(d, "{WYour site has been banned from AoW.{x\n\r", 0);
	    close_socket(d);
	    return;
	}

	if (check_multi_new(d))
	{
	    write_to_buffer(d, "\n\r\n\r{RYou already have another character logged on.\n\rPlease log this character off.\n\r\n\r{x", 0);
	    close_socket(d);
	    return;
	}

	if (check_reconnect(d, argument, FALSE)) {
	    fOld = TRUE;
	} else {
	    if (wizlock && !IS_IMMORTAL(ch)) {
		write_to_buffer(d, "{WThe game is wizlocked.{x\n\r", 0);
		close_socket(d);
		return;
	    } else if (armageddon && !IS_IMMORTAL(ch)) {
                write_to_buffer(d, "{WThe game is having an armageddon.\n\rPlease come back later!{x\n\r", 0);
               close_socket(d);
               return;
            }
	}

	if (fOld) {
	    /* Old player */
	    write_to_buffer(d, "Password:  ", 0);
	    write_to_buffer(d, echo_off_str, 0);
	    d->connected = CON_GET_OLD_PASSWORD;
	    return;
	} else {
	    /* New player */
	    if (newlock) {
		write_to_buffer(d, "{WThe game is newlocked.{x\n\r", 0);
		close_socket(d);
		return;
	    }
	    if (check_ban(d->host, BAN_NEWBIES)) {
		write_to_buffer(d,
				"{WNew players are not allowed from your site.{x\n\r", 0);
		close_socket(d);
		return;
	    }
	    write_to_buffer(d, "\n\r", 0);
	    write_to_buffer(d, "{1{W###########################################################################{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1                                                                         {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1   The name you have selected is very important.  This name must match   {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1   the following criteria.                                               {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1                                                                         {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1   1. Must be of a fantasy name. This name must not be a real life       {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1      ordinary name like bob, mike, keanu, etc.                          {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1   2. Does not include any bad language in itself.                       {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1      Such as f**kmyself, bigb***h, or c*cksucker, etc.                  {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1   3. This name may belong to famous FANTASY characters from stories,    {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1      books, etc.  Beware, if you do this you will likely be made fun of.{W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1   4. It is recommended you create a unique original fantasy name.  It   {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1      will make the game more enjoyable for you and the other players.   {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1                                                                         {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1   This game does not have immortal approval for each player, but if you {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1   are caught with a name which breaks these rules, you will be asked to {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1   start over with a more suitable name.  We are VERY serious about this.{W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1                                                                         {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1   If you need some help with a good name then go to the following site: {W#{x\n\r", 0);
	    write_to_buffer(d,
			    "{1{W#{w{1   http://www.rinkworks.com/namegen/                                     {W#{x\n\r", 0);
	    write_to_buffer(d,
			 "{1{W#{w{1                                                                         {W#{x\n\r", 0);
	    write_to_buffer(d, "{1{W###########################################################################{x\n\r", 0);

	    sprintf(buf, "\n\rIs this name appropriate for AoW, {W%s{x ({gY{x/{rN{x)?  ", argument);
	    write_to_buffer(d, buf, 0);
	    d->connected = CON_CONFIRM_NEW_NAME;
	    return;
	}
	break;

    case CON_GET_OLD_PASSWORD:
	write_to_buffer(d, "\n\r", 2);

	if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd)) {
	    write_to_buffer(d, "{RWrong password.{x\n\r", 0);
	    sprintf(log_buf, "[!!!!!] %s has given bad password %s for %s.", d->host, argument, ch->name);
	    log_string(log_buf);
	    close_socket(d);
	    return;
	}
	write_to_buffer(d, echo_on_str, 0);

	if (check_playing(d, ch->name))
	    return;

	if (check_reconnect(d, ch->name, TRUE))
	    return;
/*
           check_multi(d);
*/
	sprintf(log_buf, "%s@%s has connected in %s", ch->name, d->host, kingdom_table[ch->side].name);
	log_string(log_buf);

	//no imms
	if (!IS_IMMORTAL(ch))
	{
	
        update_toplist(ch);
        log_string("Added their data to the topplist.");
	}
	wiznet(log_buf, NULL, NULL, WIZ_SITES, 0, get_trust(ch));

	if (IS_IMMORTAL(ch)) {
	    if (ch->desc->color == TRUE)
		SET_BIT(ch->act, PLR_COLOUR);
	    do_help(ch, "imotd");
	    d->connected = CON_READ_IMOTD;
	} else {

	    if (ch->desc->color == TRUE)
		SET_BIT(ch->act, PLR_COLOUR);
/*
	    do_help2(ch, "coolit");
	    do_help(ch, "motd");
*/
	    d->connected = CON_READ_MOTD;
	}
	break;

/* RT code for breaking link */

    case CON_BREAK_CONNECT:
	switch (*argument) {
	case 'y':
	case 'Y':
	    for (d_old = descriptor_list; d_old != NULL; d_old = d_next) {
		d_next = d_old->next;
		if (d_old == d || d_old->character == NULL)
		    continue;

		if (str_cmp(ch->name, d_old->original ?
			    d_old->original->name : d_old->character->name))
		    continue;

		close_socket(d_old);
	    }
	    if (check_reconnect(d, ch->name, TRUE))
		return;
	    write_to_buffer(d, "\n\r{WReconnect attempt failed.{x\n\r\n\rName:  ", 0);
	    if (d->character != NULL) {
		free_char(d->character);
		d->character = NULL;
	    }
	    d->connected = CON_GET_NAME;
	    break;

	case 'n':
	case 'N':
	    write_to_buffer(d, "\n\rName:  ", 0);
	    if (d->character != NULL) {
		free_char(d->character);
		d->character = NULL;
	    }
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer(d, "\n\rPlease type {gY{x or {rN{x?  ", 0);
	    break;
	}
	break;

    case CON_CONFIRM_NEW_NAME:
	switch (*argument) {
	case 'y':
	case 'Y':
	    sprintf(buf, "\n\rNew character.\n\r\n\rGive me a password for {W%s{x: %s ",
		    ch->name, echo_off_str);
	    write_to_buffer(d, buf, 0);
	    log_string(ch->name);
	    log_string(d->host);
	    d->connected = CON_GET_NEW_PASSWORD;
	    break;

	case 'n':
	case 'N':
	    write_to_buffer(d, "\n\rOk, what {WIS{x it, then?  ", 0);
	    free_char(d->character);
	    d->character = NULL;
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer(d, "\n\rPlease type {gY{x or {rN{x?  ", 0);
	    break;
	}
	break;

    case CON_GET_NEW_PASSWORD:
	write_to_buffer(d, "\n\r", 2);

	if (strlen(argument) < 5) {
	    write_to_buffer(d,
			    "\n\r{WPassword must be at least five characters long.{x\n\r\n\rPassword:  ",
			    0);
	    return;
	}
	pwdnew = crypt(argument, ch->name);
	for (p = pwdnew; *p != '\0'; p++) {
	    if (*p == '~') {
		write_to_buffer(d,
				"\n\r{WNew password not acceptable, try again.{x\n\r\n\rPassword:  ",
				0);
		return;
	    }
	}

	free_string(ch->pcdata->pwd);
	ch->pcdata->pwd = str_dup(pwdnew);
	write_to_buffer(d, "Please retype password:  ", 0);
	d->connected = CON_CONFIRM_NEW_PASSWORD;
	break;

    case CON_CONFIRM_NEW_PASSWORD:
	write_to_buffer(d, "\n\r", 2);

	if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd)) {
	    write_to_buffer(d, "\n\r{WPasswords don't match.{x\n\r\n\rRetype password:  ", 0);
	    d->connected = CON_GET_NEW_PASSWORD;
	    return;
	}
	if (d->color == TRUE)
	    SET_BIT(ch->act, PLR_COLOUR);

	write_to_buffer(d, echo_on_str, 0);
	write_to_buffer(d, "\n\r", 0);
	write_to_buffer(d,
			"{1{W###########################################################################{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1                                                                         {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1   First you must declare which kingdom you come from.  This is important{W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1   because it will determine which races and classes will be available to{W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1   you and which side of the war you fight on.                           {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1                                                                         {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W###########################################################################{x\n\r", 0);
	write_to_buffer(d, "\n\r", 0);

	write_to_buffer(d, "{W1.{x  Kingdom of Tier.\n\r", 0);
	write_to_buffer(d, 
			"Once the monumental city of life, Tier has been reborn anew under the guidance\n\r", 0);
	write_to_buffer(d, 
                        "of young Prince Tornoah, son of the late king. White marble buildings trumpet\n\r", 0);
	write_to_buffer(d, 
			"the purity of purpose and spirit to all who view Tier's splendor. Shining\n\r", 0);
	write_to_buffer(d, 
			"knights patrol the streets on their well trained mounts as elite guards meander\n\r", 0); 
	write_to_buffer(d, 
			"amongst the populace bringing safety everywhere they go.\n\r\n\r", 0);
/*
	write_to_buffer(d, "{W2.{x  Empire of Kael.\n\r", 0);
	write_to_buffer(d, 
			"The walls stand tall and proud as the overcast sky brings shadows to the nooks\n\r", 0);
	write_to_buffer(d, 
                        "and crannies below. Gate guards keep an eye on the few travelers who still\n\r", 0);
	write_to_buffer(d, 
			"travel this way. Being a militaristic and ruthless kingdom, Kael has prospered\n\r", 0);
	write_to_buffer(d, 
			"under the brutal and ironfisted rule of the General and his warlords. The\n\r", 0);
	write_to_buffer(d, 
			"General's rule is oppressive and brutal but has proven undeniably effective\n\r", 0); 
	write_to_buffer(d, 
			"thus far in the war.\n\r\n\r", 0);*/

	write_to_buffer(d, "{W3.{x  Horde of Undermountain.\n\r", 0);
	write_to_buffer(d, 
			"In the west the sun sets as the last shards of light flee from a time when\n\r", 0);
	write_to_buffer(d, 
			"night rules. Darkness spreads across the land as the spawn of Undermountain are\n\r", 0);
	write_to_buffer(d, 
			"unleashed. From within the tunnels and caverns of the great mountainous peaks\n\r", 0);
	write_to_buffer(d, 
			"stream the evil beings as they again try to bring destruction to the lands of\n\r", 0);
	write_to_buffer(d, 
			"Kael, and Tier. The monstrous legions of the Horde revel in destruction.\n\r\n\r", 0);

	write_to_buffer(d,			
			"{W4. Cairn{x\n\r", 0);
	write_to_buffer(d, 
			"Queen Andora demands much of her dark minions. Obey her and you will do well.\n\r\n\r", 0);

	write_to_buffer(d, "\n\rWhich kingdom are you from? ({W1{x, {W3{x or {W4{x)\n\r", 0); 

	write_to_buffer(d, "[2 ({mKael{x) is transfer-only]\n\r\n\r", 0);
	d->connected = CON_GET_NEW_SIDE;
	break;
    case CON_GET_NEW_SIDE:
	switch (argument[0]) {
	case '1':
	    ch->side = SIDE_TIER;
	    break;
	//    write_to_buffer(d, "\n\r{WWhoa there buddy... Tier is already packed...{x  ", 0);
	//	return;
/*	case '2':
	    ch->side = SIDE_KAEL;
	    break;*/
	case '3':
	    ch->side = SIDE_UM;
	    break;
	case '4':
	    ch->side = SIDE_CAIRN;
	    break;
	default:
	    write_to_buffer(d, "\n\r{WYour choices are {W1{x, or {W4{x."
"\n\rKael is currently closed to new players.{x\n\r\n\rWhich kingdom are you from?", 0);
	    return;
	}

/* RACIS */
	write_to_buffer(d, "\n\r", 0);
	write_to_buffer(d,
			"{1{W###########################################################################{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1                                                                         {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1   These are the races that are available in your kingdom.               {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1                                                                         {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W###########################################################################{x\n\r", 0);
	write_to_buffer(d, "\n\r", 0);

	for (race = 1; pc_race_table[race].name != NULL; race++) {
	    if (pc_race_table[race].isRemort)
		continue;
	    if (pc_race_table[race].side == SIDE_NONE)
		continue;
	    if ((ch->side == SIDE_TIER && IS_SET(pc_race_table[race].side, CANBE_TIER)) ||
		(ch->side == SIDE_KAEL && IS_SET(pc_race_table[race].side, CANBE_KAEL)) ||
		(ch->side == SIDE_UM && IS_SET(pc_race_table[race].side, CANBE_UM)) ||
		(ch->side == SIDE_CAIRN && IS_SET(pc_race_table[race].side, CANBE_CAIRN))) {
		sprintf(bufrace, "%15s : %s\n\r",
			capitalize(pc_race_table[race].name),
			pc_race_table[race].short_description);
		write_to_buffer(d, bufrace, 0);
	    }
	}

	write_to_buffer(d, "\n\r", 0);
	write_to_buffer(d, "Select a race.  (type '{Whelp race{x' for more info):  ", 0);

	d->connected = CON_GET_NEW_RACE;
	break;

    case CON_GET_NEW_RACE:
	one_argument(argument, arg);

	if (!strcmp(arg, "help")) {
	    argument = one_argument(argument, arg);
	    if (argument[0] == '\0')
		do_help(ch, "race help");
	    else
		do_help(ch, argument);
	    write_to_buffer(d, "Select a race.  (type '{Whelp raceNAME{x' for more info):  ", 0);
	    break;
	}
	race = pc_race_lookup(argument);

	if (race == 0 || pc_race_table[race].side == SIDE_NONE || pc_race_table[race].isRemort ||
	    (ch->side == SIDE_TIER && !IS_SET(pc_race_table[race].side, CANBE_TIER)) ||
	    (ch->side == SIDE_KAEL && !IS_SET(pc_race_table[race].side, CANBE_KAEL)) ||
	    (ch->side == SIDE_UM && !IS_SET(pc_race_table[race].side, CANBE_UM)) ||
	    (ch->side == SIDE_CAIRN && !IS_SET(pc_race_table[race].side, CANBE_CAIRN))) {
	    write_to_buffer(d, "\n\r{WThat is not a valid race.{x\n\r", 0);

	    write_to_buffer(d, "\n\rThe following races are available:\n\r\n\r", 0);

	    for (race = 1; pc_race_table[race].name != NULL; race++) {
		if (pc_race_table[race].isRemort)
		    continue;
		if (pc_race_table[race].side == SIDE_NONE)
		    continue;
		if ((ch->side == SIDE_TIER && IS_SET(pc_race_table[race].side, CANBE_TIER)) ||
		    (ch->side == SIDE_KAEL && IS_SET(pc_race_table[race].side, CANBE_KAEL)) ||
		    (ch->side == SIDE_UM && IS_SET(pc_race_table[race].side, CANBE_UM)) ||
		    (ch->side == SIDE_CAIRN && IS_SET(pc_race_table[race].side, CANBE_CAIRN))) {
		    sprintf(bufrace, "%15s : %s\n\r",
			    capitalize(pc_race_table[race].name),
			    pc_race_table[race].short_description);
		    write_to_buffer(d, bufrace, 0);
		}
	    }

	    write_to_buffer(d, "\n\r", 0);
	    write_to_buffer(d, "Select a race. (type 'help race' for more info):  ", 0);

	    break;
	}
	ch->race = race;
	/* initialize stats */
	for (i = 0; i < MAX_STATS; i++)
	    ch->perm_stat[i] = pc_race_table[race].stats[i];
	ch->affected_by = ch->affected_by | race_table[race].aff;
	ch->imm_flags = ch->imm_flags | race_table[race].imm;
	ch->res_flags = ch->res_flags | race_table[race].res;
	ch->vuln_flags = ch->vuln_flags | race_table[race].vuln;
	ch->form = race_table[race].form;
	ch->parts = race_table[race].parts;

	/* add skills */
	for (i = 0; i < 5; i++) {
	    if (pc_race_table[race].skills[i] == NULL)
		break;
	    group_add(ch, pc_race_table[race].skills[i], FALSE);
	}
	/* add cost */
	ch->pcdata->points = pc_race_table[race].points;
	ch->size = pc_race_table[race].size;

	write_to_buffer(d, "\n\rWhat is your sex ({WM{x/{WF{x)?  ", 0);
	d->connected = CON_GET_NEW_SEX;
	break;

    case CON_GET_NEW_SEX:
	switch (argument[0]) {
	case 'm':
	case 'M':
	    ch->sex = SEX_MALE;
	    ch->pcdata->true_sex = SEX_MALE;
	    break;
	case 'f':
	case 'F':
	    ch->sex = SEX_FEMALE;
	    ch->pcdata->true_sex = SEX_FEMALE;
	    break;
	default:
	    write_to_buffer(d, "\n\r{WThat's not a sex.{x\n\r\n\rWhat IS your sex({WM{x/{WF{x)?  ", 0);
	    return;
	}

	write_to_buffer(d, "\n\r", 0);
	write_to_buffer(d,
			"{1{W###########################################################################{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1                                                                         {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1   Now you must select your characters class.                            {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1                                                                         {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W###########################################################################{x\n\r", 0);
	write_to_buffer(d, "\n\r", 0);

	for (class = 0; class < MAX_CLASS; class++) {
	    if ((ch->side == SIDE_TIER && IS_SET(class_table[class].side, CANBE_TIER)) ||
		(ch->side == SIDE_KAEL && IS_SET(class_table[class].side, CANBE_KAEL)) ||
		(ch->side == SIDE_UM && IS_SET(class_table[class].side, CANBE_UM)) ||
		(ch->side == SIDE_CAIRN && IS_SET(class_table[class].side, CANBE_CAIRN))) {
		sprintf(bufrace, "%15s : %s\n\r",
			capitalize(class_table[class].name),
			class_table[class].short_description);
		write_to_buffer(d, bufrace, 0);
	    }
	}

	write_to_buffer(d, "\n\rWhich class?  (Type '{Whelp classname{x' for more info.):  ", 0);
	d->connected = CON_GET_NEW_CLASS;
	break;

    case CON_GET_NEW_CLASS:
	one_argument(argument, arg);

	if (!strcmp(arg, "help")) {
	    argument = one_argument(argument, arg);
	    if (argument[0] == '\0')
		do_help(ch, "class");
	    else
		do_help(ch, argument);
	    write_to_buffer(d, "\n\rWhich class?  ", 0);
	    break;
	}
	iClass = class_lookup(argument);
	if ( iClass == -1 )
	{
	    write_to_buffer( d,
		"That's not a choice.\n\rWhat is your class? ", 0 );
	    return;
	}

//	if (iClass == SIDE_NONE ||
//	    (ch->side == SIDE_TIER && !IS_SET(class_table[iClass].side, CANBE_TIER)) ||

	if ((ch->side == SIDE_TIER && !IS_SET(class_table[iClass].side, CANBE_TIER)) ||
	    (ch->side == SIDE_KAEL && !IS_SET(class_table[iClass].side, CANBE_KAEL)) ||
	    (ch->side == SIDE_UM && !IS_SET(class_table[iClass].side, CANBE_UM)) ||
	    (ch->side == SIDE_CAIRN && !IS_SET(class_table[iClass].side, CANBE_CAIRN))) {
	    write_to_buffer(d, "\n\r{WThat's not a valid choice.{x\n\r\n\rThe following classes are available.\n\r\n\r", 0);
	    for (class = 0; class < MAX_CLASS; class++) {
		if ((ch->side == SIDE_TIER && IS_SET(class_table[class].side, CANBE_TIER)) ||
		    (ch->side == SIDE_KAEL && IS_SET(class_table[class].side, CANBE_KAEL)) ||
		    (ch->side == SIDE_UM && IS_SET(class_table[class].side, CANBE_UM)) ||
		    (ch->side == SIDE_CAIRN && IS_SET(class_table[class].side, CANBE_CAIRN))) {
		    sprintf(bufrace, "%15s : %s\n\r",
			    capitalize(class_table[class].name),
			    class_table[class].short_description);
		    write_to_buffer(d, bufrace, 0);
		}
	    }
	    write_to_buffer(d, "\n\rWhich class?  (Type '{Whelp classname{x' for more info.):  ", 0);
	    return;
	}
	ch->class = iClass;

	sprintf(log_buf, "%s@%s new player.", ch->name, d->host);
	log_string(log_buf);
	wiznet("Newbie alert!  $N new player making character.", ch, NULL, WIZ_NEWBIE, 0, 0);
	wiznet(log_buf, NULL, NULL, WIZ_SITES, 0, get_trust(ch));
	if (ch->side == SIDE_TIER)
	    write_to_buffer(d, "\n\r\n\r{WWelcome to Tier.{x\n\r", 0);
	else if (ch->side == SIDE_KAEL)
	    write_to_buffer(d, "\n\r\n\r{WWelcome to Kael.{x\n\r", 0);
	else if (ch->side == SIDE_UM)
	    write_to_buffer(d, "\n\r\n\r{WWelcome to Undermountain.{x\n\r", 0);
	else if (ch->side == SIDE_CAIRN)
	    write_to_buffer(d, "\n\r\n\r{WWelcome to Chaos.{x\n\r", 0);
	write_to_buffer(d, "\n\r\n\r** Press RETURN to continue **\n\r", 0);

	d->connected = CON_GET_ALIGNMENT;
	break;

    case CON_GET_ALIGNMENT:

	group_add(ch, "rom basics", FALSE);
	group_add(ch, class_table[ch->class].base_group, FALSE);
	ch->pcdata->learned[gsn_recall] = 50;
	ch->pcdata->learned[gsn_riding] = 50;

	write_to_buffer(d, "\n\r", 0);
	write_to_buffer(d,
			"{1{W###########################################################################{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1                                                                         {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1   Do you wish to customize this character?                              {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1   Customization will allow you to add or drop skills                    {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1   and spells to fine tune your character.                               {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1                                                                         {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1   If you do not choose to customize a default skill and spell selection {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1   will be chosen for you automatically.  It is recommended that you     {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1   customize in order to get the most out of AoW.                        {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W#{w{1                                                                         {W#{x\n\r", 0);
	write_to_buffer(d,
			"{1{W###########################################################################{x\n\r", 0);
	write_to_buffer(d, "\n\r", 0);

	write_to_buffer(d, "\n\rCustomize this character ({gY{x/{rN{x)?  ", 0);
	d->connected = CON_DEFAULT_CHOICE;
	break;

    case CON_DEFAULT_CHOICE:

	if (d->color == TRUE)
	    SET_BIT(ch->act, PLR_COLOUR);

	write_to_buffer(d, "\n\r", 2);
	switch (argument[0]) {
	case 'y':
	case 'Y':
	    ch->gen_data = new_gen_data();
	    ch->gen_data->points_chosen = ch->pcdata->points;
	    do_help(ch, "group header");
	    list_group_costs(ch);
	    write_to_buffer(d, "\n\rYou already have the following skills:\n\r", 0);
	    do_skills(ch, "");
	    do_help(ch, "menu choice");
	    d->connected = CON_GEN_GROUPS;
	    break;
	case 'n':
	case 'N':
	    group_add(ch, class_table[ch->class].default_group, TRUE);
	    write_to_buffer(d, "\n\r", 2);
          if(!str_cmp(class_table[ch->class].name, "monk") )
          write_to_buffer(d, "Please remember, that as a monk you don't get weapons.", 0);
          else
          write_to_buffer(d,
			    "Please pick a weapon from the following choices:\n\r", 0);
	    buf[0] = '\0';
	    for (i = 0; weapon_table[i].name != NULL; i++)
		if (ch->pcdata->learned[*weapon_table[i].gsn] > 0) {
		    strcat(buf, weapon_table[i].name);
		    strcat(buf, " ");
		}
         //Monks don't use weapons.
         if(!str_cmp(class_table[ch->class].name, "monk") )
	    strcat(buf, "\n\r[Press RETURN to continue]");
          else
	    strcat(buf, "\n\rYour choice? ");

	    write_to_buffer(d, buf, 0);
         if(!str_cmp(class_table[ch->class].name, "monk") )
          d->connected = CON_READ_MOTD;
          else
	    d->connected = CON_PICK_WEAPON;

	    break;
	default:
	    write_to_buffer(d, "Please answer ({gY{x/{rN{x)? ", 0);
	    return;
	}
	break;

    case CON_PICK_WEAPON:
	write_to_buffer(d, "\n\r", 2);
	weapon = weapon_lookup(argument);
	if (weapon == -1 || ch->pcdata->learned[*weapon_table[weapon].gsn] <= 0) {
	    write_to_buffer(d,
			    "That's not a valid selection. Choices are:\n\r", 0);
	    buf[0] = '\0';
	    for (i = 0; weapon_table[i].name != NULL; i++)
		if (ch->pcdata->learned[*weapon_table[i].gsn] > 0) {
		    strcat(buf, weapon_table[i].name);
		    strcat(buf, " ");
		}
	    strcat(buf, "\n\rYour choice? ");
	    write_to_buffer(d, buf, 0);
	    return;
	}
	ch->pcdata->learned[*weapon_table[weapon].gsn] = 40;
	write_to_buffer(d, "\n\r", 2);

	if (ch->desc != NULL)
	    if (ch->desc->color == TRUE)
		SET_BIT(ch->act, PLR_COLOUR);
	do_help2(ch, "coolit");
	do_help(ch, "motd");

	d->connected = CON_READ_MOTD;
	break;

    case CON_GEN_GROUPS:
	send_to_char("\n\r", ch);
	if (!str_cmp(argument, "done")) {
	    sprintf(buf, "Creation points: %d\n\r", ch->pcdata->points);
	    send_to_char(buf, ch);
	    sprintf(buf, "Experience per level: %d\n\r",
		    exp_per_level(ch, ch->gen_data->points_chosen));
	    if (ch->pcdata->points < 40)
		ch->train = (40 - ch->pcdata->points + 1) / 2;
	    free_gen_data(ch->gen_data);
	    ch->gen_data = NULL;
	    send_to_char(buf, ch);
	    write_to_buffer(d, "\n\r", 2);
          if(!str_cmp(class_table[ch->class].name, "monk") )
          write_to_buffer(d, "Please remember, that as a monk you don't get weapons.", 0);
          else
          write_to_buffer(d,
			    "Please pick a weapon from the following choices:\n\r", 0);
	    buf[0] = '\0';
	    for (i = 0; weapon_table[i].name != NULL; i++)
		if (ch->pcdata->learned[*weapon_table[i].gsn] > 0) {
		    strcat(buf, weapon_table[i].name);
		    strcat(buf, " ");
		}
         if(!str_cmp(class_table[ch->class].name, "monk") )
	    strcat(buf, "\n\r[Press RETURN to continue]");
          else
	    strcat(buf, "\n\rYour choice? ");
	    write_to_buffer(d, buf, 0);
         if(!str_cmp(class_table[ch->class].name, "monk") )
          d->connected = CON_READ_MOTD;
          else
	    d->connected = CON_PICK_WEAPON;

	    break;
	}
	if (!parse_gen_groups(ch, argument))
	    send_to_char(
			    "Choices are: list,learned,premise,add,drop,info,help, and done.\n\r"
			    ,ch);

	do_help(ch, "menu choice");
	break;

    case CON_READ_IMOTD:
	write_to_buffer(d, "\n\r", 2);

	if (ch->desc != NULL)
	    if (ch->desc->color == TRUE)
		SET_BIT(ch->act, PLR_COLOUR);

	do_help2(ch, "coolit");
	do_help(ch, "motd");

	d->connected = CON_READ_MOTD;
	break;

    case CON_READ_MOTD:
	if (ch->pcdata == NULL || ch->pcdata->pwd[0] == '\0') {
	    write_to_buffer(d, "Warning! Null password!\n\r", 0);
	    write_to_buffer(d, "Please report old password with bug.\n\r", 0);
	    write_to_buffer(d, "Type 'password null <new password>' to fix.\n\r", 0);
	}
/*
	do_help2(ch, "coolit");
	do_help(ch, "motd");
*/
	write_to_buffer(d, "\n\r{R                              Welcome to WAR!!{x\n\r\n\r", 0);
	send_to_char("\n\n{GTo post PK logs, suggestions, or discussions, please visit:\n{Rhttp://s14.invisionfree.com/Act_Of_War/{X\n",ch);
	ch->next = char_list;
	char_list = ch;
	d->connected = CON_PLAYING;

	reset_char(ch);

	if (ch->pcdata->strategy < 0 || ch->pcdata->strategy > 3) {
	    ch->pcdata->strategy = 2;
	}
	if (ch->pcdata->rename != 1)
	    ch->pcdata->rename = 0;

	if (pc_race_table[ch->race].isRemort) {
	    if (ch->pcdata->points != 65) {
		ch->pcdata->points = 65;
		ch->exp = ch->level * exp_per_level(ch, ch->pcdata->points);
	    }
	}


	if (ch->level == 0) {
	    if (ch->desc != NULL)
		if (ch->desc->color == TRUE)
		    SET_BIT(ch->act, PLR_COLOUR);

	    ch->perm_stat[class_table[ch->class].attr_prime] += 2;
	    ch->level = 1;
	    ch->exp = exp_per_level(ch, ch->pcdata->points);
	    ch->hit = ch->max_hit;
	    ch->mana = ch->max_mana;
	    ch->move = ch->max_move;
	    ch->train = 3;
	    ch->practice = 5;
	    ch->levelpoints = 0;
	    ch->enemies_killed = 0;
	    ch->final_blows = 0;
	    ch->raids = 0;
	    ch->playerkilled = 0;
	    ch->leg_killed = 0;

	    set_title(ch, "the newbie. <Please set title soon type 'help title'>");

	    do_outfit(ch, "You MUST get to level 2 to save.\n\r");

	    ch->pcdata->strategy = 2;

	    do_extra(ch, "");
	    do_tile(ch, "");

	    if (GET_SIDE(ch) == SIDE_TIER) {
		obj_to_char(create_object(get_obj_index(20110), 0), ch);
	    }
	    if (GET_SIDE(ch) == SIDE_KAEL) {
		obj_to_char(create_object(get_obj_index(20210), 0), ch);
	    }
	    SET_BIT(ch->act, PLR_AUTOEXIT);
	    SET_BIT(ch->act, PLR_NOCLAN);

	    char_to_room(ch, get_room_index(kingdom_table[ch->side].start_room_num));

	    send_to_char("\n\r", ch);
	    send_to_char("\n\r", ch);

	    sprintf(buf, "%s is a new player welcome them to AoW.\n\r", ch->name);
	    news(buf, ch->side);
	} else if (ch->in_room != NULL) {
if (!IS_IMMORTAL(ch)) {
		if ( (ch->side == SIDE_TIER  && (ch->in_room->vnum/100  == 158 || ch->in_room->vnum/100  == 159)) ||
		     (ch->side == SIDE_KAEL  && (ch->in_room->vnum/100  == 202 || ch->in_room->vnum/100  == 203)) ||
		     (ch->side == SIDE_UM    && (ch->in_room->vnum/100  == 204 || ch->in_room->vnum/100  == 300)) ||
		     (ch->side == SIDE_CAIRN && (ch->in_room->vnum/1000 == 28  || ch->in_room->vnum/1000 == 19 )) ||
		     (ch->side == SIDE_NONE)                                                                       ) {
		        char_to_room(ch, ch->in_room);
		} else {
		        char_to_room(ch, get_room_index(kingdom_table[ch->side].start_room_num));
		}
	    } else {
		char_to_room(ch, ch->in_room);
	    }
	} else {
	    char_to_room(ch, get_room_index(kingdom_table[ch->side].start_room_num));
	}

	/* check here for affect */

	paf = affect_find(ch->affected, skill_lookup("noquit"));

	if (paf != NULL)
	    affect_remove(ch, paf);

	act("$n has entered the game.", ch, NULL, NULL, TO_ROOM);
	do_look(ch, "auto");

	check_move(ch);

	if (ch->sameip == FALSE)
	{
		wiznet("$N has entered the world.", ch, NULL, WIZ_LOGINS, WIZ_SITES, get_trust(ch));
	}
	else
	{
		wiznet("$N has entered the world. {B[Multiflag]{x", ch, NULL, WIZ_LOGINS, WIZ_SITES, get_trust(ch));
	}

	       
    //  if (ch->side == SIDE_TIER)
	//  do_help(ch, "tierrules");
	//  if (ch->side == SIDE_KAEL)
	//  do_help(ch, "kaelrules");
	    if (ch->side == SIDE_UM)
	    do_help(ch, "undermountainrules");
	//  if (ch->side == SIDE_CAIRN)
	//  do_help(ch, "cairnrules");
	       
	       
	if (ch->level <= 2)
	    do_help(ch, "newbiehelp");
	
	    for ( vch = char_list; vch != NULL; vch = vch->next )
    {
    if (vch->in_room == ch->in_room)
        continue;
	if (GET_SIDE(vch) != GET_SIDE(ch))
        continue;    
if (IS_SET(vch->comm, COMM_PENTER) )
    {
    act("{c$n{x enters the kingdom.",ch,0,vch,TO_VICT);
    }
    }
    
	if (ch->pet != NULL) {
	    char_to_room(ch->pet, ch->in_room);
	    act("$n has entered the game.", ch->pet, NULL, NULL, TO_ROOM);
	}
	if (ch->mount != NULL) {
	    char_to_room(ch->mount, ch->in_room);
	    act("$n has entered the game.", ch->mount, NULL, NULL, TO_ROOM);
	    add_follower(ch->mount, ch);
	    do_mount(ch, ch->mount->name);
	}
	do_unread(ch, "");
	break;
    }

    return;
}

/* There be Dragons!
 *
 * Er, I mean, this is the new multi check code. After you type a name
 * at login, it checks to see if you have another char on linkdead BEFORE you enter your
 * password. If you have one linkdead, it drops you like a rock, and tells you to
 * go disconnect your LD char.
 *
 * Skips over frozen LD chars though.
 *
 * Don't you just love me?
 *
 * -Tasydra whom is in a good mood. 6-22-02
 */
/*bool check_multi_new(DESCRIPTOR_DATA * d)
{
    CHAR_DATA *cd;
    
    if (IS_IMMORTAL(d->character))
    return FALSE;
    
    if (d->character->sameip)
    return FALSE;
    
    for (cd = char_list; cd != NULL; cd = cd->next)
    {   
     if (IS_NPC(cd))
     continue;

     if (IS_SET(cd->act, PLR_FREEZE))
     continue;
          
     if (!str_cmp(d->character->name,cd->name))
     return FALSE;
     
     if (cd->sameip && !str_cmp(d->host, cd->host))
     return FALSE;
     
     if (IS_IMMORTAL(cd) && !str_cmp(d->host, cd->host))
     return FALSE;

     if (!str_cmp(d->host, cd->host))
     return TRUE;
    }
    return FALSE;
}*/

void check_multi(DESCRIPTOR_DATA * chk)
{
    DESCRIPTOR_DATA *d;
    bool multi = FALSE;

    if (chk->host == NULL)
	return;

    for (d = descriptor_list; d != NULL; d = d->next) 
    {
	if (d->character != NULL) 
	{
	    if (d->host != NULL) 
	    {
		if (chk->character->sameip && d->character->sameip)
		    return;

		if (!strcmp(d->host, chk->host) && (d->descriptor != chk->descriptor)) 
		{

		    if (!IS_IMMORTAL(d->character) && !IS_IMMORTAL(chk->character) && (d->connected < 1)) 
		    {
			multi = TRUE;
		    }
		    if ((IS_IMMORTAL(d->character) || IS_IMMORTAL(chk->character)) && (d->connected < 1)) 
		    {
			multi = FALSE;
			break;
		    }
		}
		if (!strcmp(chk->character->name, d->character->name) && (d->descriptor != chk->descriptor)) 
		{
		    multi = TRUE;
		}
	    }
	}
    }

    if (multi) 
    {
	write_to_buffer(chk, "Only one player allowed to play at a time.\n\r", 0);
	close_socket(chk);
    }
    return;
}

/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect(DESCRIPTOR_DATA * d, char *name, bool fConn)
{
    CHAR_DATA *ch;

    for (ch = char_list; ch != NULL; ch = ch->next) {
	if (!IS_NPC(ch)
	    && (!fConn || ch->desc == NULL)
	    && !str_cmp(d->character->name, ch->name)) {
	    if (fConn == FALSE) {
		free_string(d->character->pcdata->pwd);
		d->character->pcdata->pwd = str_dup(ch->pcdata->pwd);
	    } else {
		free_char(d->character);
		d->character = ch;
		ch->desc = d;
		ch->timer = 0;
		send_to_char(
				"Reconnecting. Type replay to see missed tells.\n\r", ch);
		act("$n has reconnected.", ch, NULL, NULL, TO_ROOM);

	sprintf(log_buf, "%s@%s has reconnected in %s", ch->name, d->host, kingdom_table[ch->side].name);
//		sprintf(log_buf, "%s@%s reconnected.", ch->name, d->host);
		log_string(log_buf);
		wiznet("$N groks the fullness of $S link.",
		       ch, NULL, WIZ_LINKS, 0, 0);
		d->connected = CON_PLAYING;
		if (check_multi_new(d))
                 {
                  close_socket(d);
                  return FALSE;
                 }
	    }
	    return TRUE;
	}
    }

    return FALSE;
}

/*
 * Check if already playing.
 */
bool check_playing(DESCRIPTOR_DATA * d, char *name)
{
    DESCRIPTOR_DATA *dold;

    for (dold = descriptor_list; dold; dold = dold->next) {
	if (dold != d
	    && dold->character != NULL
	    && dold->connected != CON_GET_NAME
	    && dold->connected != CON_GET_OLD_PASSWORD
	    && !str_cmp(name, dold->original
			? dold->original->name : dold->character->name)) {
	    write_to_buffer(d, "That character is already playing.\n\r", 0);
	    write_to_buffer(d, "Do you wish to connect anyway (Y/N)?", 0);
	    d->connected = CON_BREAK_CONNECT;
	    return TRUE;
	}
    }

    return FALSE;
}

void stop_idling(CHAR_DATA * ch)
{
    if (ch == NULL
	|| ch->desc == NULL
	|| ch->desc->connected != CON_PLAYING
	|| ch->was_in_room == NULL
	|| ch->in_room != get_room_index(ROOM_VNUM_LIMBO))
	return;

    ch->timer = 0;
    char_from_room(ch);
    char_to_room(ch, ch->was_in_room);
    ch->was_in_room = NULL;
    act("$n has returned from the void.", ch, NULL, NULL, TO_ROOM);
    return;
}

/*
 * Write to one char.
 */
void send_to_char_bw(const char *txt, CHAR_DATA * ch)
{
    if (txt != NULL && ch->desc != NULL)
	write_to_buffer(ch->desc, txt, strlen(txt));
    return;
}

/*
 * Write to one char, new colour version, by Lope.
 */
void send_to_char(const char *txt, CHAR_DATA * ch)
{
    const char *point;
    char *point2;
    char buf[MAX_STRING_LENGTH * 4];
    int skip = 0;

    buf[0] = '\0';
    point2 = buf;

    /* mud had crashed earlier on a null CHAR_DATA pointer 
       (Orag: July 3, 2001)
    */
    if (ch == NULL)
      return;

    if (txt && ch->desc) {

	if (IS_SET(ch->act, PLR_COLOUR) && ch->desc->color == TRUE) {
	    for (point = txt; *point; point++) {
		if (*point == '{') {
		    point++;
		    skip = colour(*point, ch, point2);
		    while (skip-- > 0)
			++point2;
		    continue;
		}
		*point2 = *point;
		*++point2 = '\0';
	    }
	    *point2 = '\0';
	    write_to_buffer(ch->desc, buf, point2 - buf);
	} else {
	    for (point = txt; *point; point++) {
		if (*point == '{') {
		    point++;
		    continue;
		}
		*point2 = *point;
		*++point2 = '\0';
	    }
	    *point2 = '\0';
	    write_to_buffer(ch->desc, buf, point2 - buf);
	}
    }
    return;
}

/*
 * Send a page to one char.
 */
void page_to_char_bw(const char *txt, CHAR_DATA * ch)
{
    if (txt == NULL || ch->desc == NULL)
	return;

    if (ch->lines == 0) {
	send_to_char(txt, ch);
	return;
    }
    ch->desc->showstr_head = alloc_mem(strlen(txt) + 1);
    strcpy(ch->desc->showstr_head, txt);
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string(ch->desc, "");
}

/*
 * Page to one char, new colour version, by Lope.
 */
void page_to_char(const char *txt, CHAR_DATA * ch)
{
    const char *point;
    char *point2;
    char buf[MAX_STRING_LENGTH * 16];
    int skip = 0;

    buf[0] = '\0';
    point2 = buf;

    if (ch == NULL || txt == NULL)
	return;
    if (ch->desc == NULL)
	return;

    if (txt && ch->desc) {
	if (IS_SET(ch->act, PLR_COLOUR) && ch->desc->color == TRUE) {
	    for (point = txt; *point; point++) {
		if (*point == '{') {
		    point++;
		    skip = colour(*point, ch, point2);
		    while (skip-- > 0)
			++point2;
		    continue;
		}
		*point2 = *point;
		*++point2 = '\0';
	    }
	    *point2 = '\0';
	    if (ch == NULL)
		return;
	    if (ch->desc == NULL)
		return;

	    ch->desc->showstr_head = alloc_mem(strlen(buf) + 1);
	    strcpy(ch->desc->showstr_head, buf);
	    ch->desc->showstr_point = ch->desc->showstr_head;
	    show_string(ch->desc, "");
	} else {
	    for (point = txt; *point; point++) {
		if (*point == '{') {
		    point++;
		    continue;
		}
		*point2 = *point;
		*++point2 = '\0';
	    }
	    *point2 = '\0';
	    ch->desc->showstr_head = alloc_mem(strlen(buf) + 1);
	    strcpy(ch->desc->showstr_head, buf);
	    ch->desc->showstr_point = ch->desc->showstr_head;
	    show_string(ch->desc, "");
	}
    }
    return;
}

/* string pager */
void show_string(struct descriptor_data *d, char *input)
{
    char buffer[4 * MAX_STRING_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    register char *scan, *chk;
    int lines = 0, toggle = 1;
    int show_lines;

    one_argument(input, buf);
    if (buf[0] != '\0') {
	if (d->showstr_head) {
	    free_mem(d->showstr_head, strlen(d->showstr_head));
	    d->showstr_head = 0;
	}
	d->showstr_point = 0;
	return;
    }
    if (d->character)
	show_lines = d->character->lines;
    else
	show_lines = 0;

    for (scan = buffer;; scan++, d->showstr_point++) {
	if (((*scan = *d->showstr_point) == '\n' || *scan == '\r')
	    && (toggle = -toggle) < 0)
	    lines++;

	else if (!*scan || (show_lines > 0 && lines >= show_lines)) {
	    *scan = '\0';
	    write_to_buffer(d, buffer, strlen(buffer));
	    for (chk = d->showstr_point; isspace(*chk); chk++);
	    {
		if (!*chk) {
		    if (d->showstr_head) {
			free_mem(d->showstr_head, strlen(d->showstr_head));
			d->showstr_head = 0;
		    }
		    d->showstr_point = 0;
		}
	    }
	    return;
	}
    }
    return;
}

/* quick sex fixer */
void fix_sex(CHAR_DATA * ch)
{
    if (ch->sex < 0 || ch->sex > 2)
	ch->sex = IS_NPC(ch) ? 0 : ch->pcdata->true_sex;
}

void act(const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2,
	 int type)
{
    /* to be compatible with older code */
    act_new(format, ch, arg1, arg2, type, POS_RESTING);
}

void act_new(const char *format, CHAR_DATA * ch, const void *arg1,
	     const void *arg2, int type, int min_pos)
{
    static char *const he_she[] =
    {"it", "he", "she"};
    static char *const him_her[] =
    {"it", "him", "her"};
    static char *const his_her[] =
    {"its", "his", "her"};

    char buf[MAX_STRING_LENGTH];
    char pers_buf[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    CHAR_DATA *to;
    CHAR_DATA *vch = (CHAR_DATA *) arg2;
    OBJ_DATA *obj1 = (OBJ_DATA *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA *) arg2;
    const char *str;
    const char *i;
    char *point;

    /*
     * Discard null and zero-length messages.
     */
    if (format == NULL || format[0] == '\0')
	return;

    /* discard null rooms and chars */
    if (ch == NULL || ch->in_room == NULL)
	return;

    to = ch->in_room->people;
    if (type == TO_VICT) {
	if (vch == NULL) {
	    bug("Act: null vch with TO_VICT.", 0);
	    return;
	}
	if (vch->in_room == NULL)
	    return;

	to = vch->in_room->people;
    }
    for (; to != NULL; to = to->next_in_room) {
	if ((!IS_NPC(to) && to->desc == NULL)
	    || (IS_NPC(to) && !HAS_TRIGGER(to, TRIG_ACT))
	    || to->position < min_pos)
	    continue;

	if ((type == TO_CHAR) && to != ch)
	    continue;
	if (type == TO_VICT && (to != vch || to == ch))
	    continue;
	if (type == TO_ROOM && to == ch)
	    continue;
	if (type == TO_NOTVICT && (to == ch || to == vch))
	    continue;

	point = buf;
	str = format;
	while (*str != '\0') {
	    if (*str != '$') {
		*point++ = *str++;
		continue;
	    }
	    ++str;

	    if (arg2 == NULL && *str >= 'A' && *str <= 'Z') {
		bug("Act: missing arg2 for code %d.", *str);
		i = " <@@@> ";
	    } else {
		switch (*str) {
		default:
		    bug("Act: bad code %d.", *str);
		    i = " <@@@> ";
		    break;
		    /* Thx alex for 't' idea */
		case 't':
		    i = (char *) arg1;
		    break;
		case 'T':
		    i = (char *) arg2;
		    break;
		case 'n':
		    PERS(ch, to, pers_buf);
		    i = pers_buf;
		    break;
		case 'N':
		    PERS(vch, to, pers_buf);
		    i = pers_buf;
		    break;
		case 'e':
		    i = he_she[URANGE(0, ch->sex, 2)];
		    break;
		case 'E':
		    i = he_she[URANGE(0, vch->sex, 2)];
		    break;
		case 'm':
		    i = him_her[URANGE(0, ch->sex, 2)];
		    break;
		case 'M':
		    i = him_her[URANGE(0, vch->sex, 2)];
		    break;
		case 's':
		    i = his_her[URANGE(0, ch->sex, 2)];
		    break;
		case 'S':
		    i = his_her[URANGE(0, vch->sex, 2)];
		    break;

		case 'p':
		    i = can_see_obj(to, obj1)
			? obj1->short_descr
			: "something";
		    break;

		case 'P':
		    i = can_see_obj(to, obj2)
			? obj2->short_descr
			: "something";
		    break;

		case 'd':
		    if (arg2 == NULL || ((char *) arg2)[0] == '\0') {
			i = "door";
		    } else {
			one_argument((char *) arg2, fname);
			i = fname;
		    }
		    break;
		}
	    }

	    ++str;
	    while ((*point = *i) != '\0') {
		++point, ++i;
	    }
	}

	*point++ = '\n';
	*point++ = '\r';
	*point = '\0';
	buf[0] = UPPER(buf[0]);
	if (to->desc != NULL)
	    write_to_buffer(to->desc, buf, point - buf);
	else if (MOBtrigger)
	    mp_act_trigger(buf, to, ch, arg1, arg2, TRIG_ACT);
    }
    return;
}

int colour2(char type, char *string)
{
    char code[20];
    char *p = '\0';

    switch (type) {
    default:
	sprintf(code, CLEAR);
	break;
    case 'x':
	sprintf(code, CLEAR);
	break;
    case 'b':
	sprintf(code, C_BLUE);
	break;
    case 'c':
	sprintf(code, C_CYAN);
	break;
    case 'g':
	sprintf(code, C_GREEN);
	break;
    case 'm':
	sprintf(code, C_MAGENTA);
	break;
    case 'r':
	sprintf(code, C_RED);
	break;
    case 'w':
	sprintf(code, C_WHITE);
	break;
    case 'y':
	sprintf(code, C_YELLOW);
	break;
    case 'B':
	sprintf(code, C_B_BLUE);
	break;
    case 'C':
	sprintf(code, C_B_CYAN);
	break;
    case 'G':
	sprintf(code, C_B_GREEN);
	break;
    case 'M':
	sprintf(code, C_B_MAGENTA);
	break;
    case 'R':
	sprintf(code, C_B_RED);
	break;
    case 'W':
	sprintf(code, C_B_WHITE);
	break;
    case 'Y':
	sprintf(code, C_B_YELLOW);
	break;
    case 'D':
	sprintf(code, C_D_GREY);
	break;
    case '0':
	sprintf(code, C_BACK_BLK);
	break;
    case '1':
	sprintf(code, C_BACK_RED);
	break;
    case '2':
	sprintf(code, C_BACK_GRN);
	break;
    case '3':
	sprintf(code, C_BACK_YEL);
	break;
    case '4':
	sprintf(code, C_BACK_BLU);
	break;
    case '5':
	sprintf(code, C_BACK_MAG);
	break;
    case '6':
	sprintf(code, C_BACK_CYN);
	break;
    case '7':
	sprintf(code, C_BACK_WHT);
	break;

/* Out, out damn beep.
   case '*':
   sprintf( code, "%c", 007 );
   break;
 */
/* Time for cr's to come out as well.
   case '/':
   sprintf( code, "%c", 012 );
   break;
 */
    case '{':
	sprintf(code, "%c", '{');
	break;
    }

    p = code;
    while (*p != '\0') {
	*string = *p++;
	*++string = '\0';
    }

    return (strlen(code));
}

int colour(char type, CHAR_DATA * ch, char *string)
{
    char code[MAX_INPUT_LENGTH];
    char *p = '\0';

    if (ch == NULL)
	return (0);

    if (IS_NPC(ch))
	return (0);

    switch (type) {
    default:
	sprintf(code, CLEAR);
	break;
    case 'x':
	sprintf(code, CLEAR);
	break;
    case 'b':
	sprintf(code, C_BLUE);
	break;
    case 'c':
	sprintf(code, C_CYAN);
	break;
    case 'g':
	sprintf(code, C_GREEN);
	break;
    case 'm':
	sprintf(code, C_MAGENTA);
	break;
    case 'r':
	sprintf(code, C_RED);
	break;
    case 'w':
	sprintf(code, C_WHITE);
	break;
    case 'y':
	sprintf(code, C_YELLOW);
	break;
    case 'B':
	sprintf(code, C_B_BLUE);
	break;
    case 'C':
	sprintf(code, C_B_CYAN);
	break;
    case 'G':
	sprintf(code, C_B_GREEN);
	break;
    case 'M':
	sprintf(code, C_B_MAGENTA);
	break;
    case 'R':
	sprintf(code, C_B_RED);
	break;
    case 'W':
	sprintf(code, C_B_WHITE);
	break;
    case 'Y':
	sprintf(code, C_B_YELLOW);
	break;
    case 'D':
	sprintf(code, C_D_GREY);
	break;
    case '0':
	sprintf(code, C_BACK_BLK);
	break;
    case '1':
	sprintf(code, C_BACK_RED);
	break;
    case '2':
	sprintf(code, C_BACK_GRN);
	break;
    case '3':
	sprintf(code, C_BACK_YEL);
	break;
    case '4':
	sprintf(code, C_BACK_BLU);
	break;
    case '5':
	sprintf(code, C_BACK_MAG);
	break;
    case '6':
	sprintf(code, C_BACK_CYN);
	break;
    case '7':
	sprintf(code, C_BACK_WHT);
	break;

/* Out out damn beep.
   case '*':
   sprintf( code, "%c", 007 );
   break;
 */
/* Time for cr's to come out as well.
   case '/':
   sprintf( code, "%c", 012 );
   break;
 */
    case '{':
	sprintf(code, "%c", '{');
	break;
    }

    p = code;
    while (*p != '\0') {
	*string = *p++;
	*++string = '\0';
    }

    return (strlen(code));
}

void colourconv(char *buffer, const char *txt, CHAR_DATA * ch)
{
    const char *point;
    int skip = 0;

    if (ch->desc && txt) {
	if (IS_SET(ch->act, PLR_COLOUR) && ch->desc->color == TRUE) {
	    for (point = txt; *point; point++) {
		if (*point == '{') {
		    point++;
		    skip = colour(*point, ch, buffer);
		    while (skip-- > 0)
			++buffer;
		    continue;
		}
		*buffer = *point;
		*++buffer = '\0';
	    }
	    *buffer = '\0';
	} else {
	    for (point = txt; *point; point++) {
		if (*point == '{') {
		    point++;
		    continue;
		}
		*buffer = *point;
		*++buffer = '\0';
	    }
	    *buffer = '\0';
	}
    }
    return;
}

void ptc (CHAR_DATA *ch, char *fmt, ...)
{
	char buf [MAX_STRING_LENGTH];
	va_list args;
	va_start (args, fmt);
	vsprintf (buf, fmt, args);
	va_end (args);
        stc(buf, ch);
}

