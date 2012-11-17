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
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*       ROM 2.4 is copyright 1993-1996 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@efn.org)                                  *
*           Gabrielle Taylor                                               *
*           Brian Moore (zump@rom.org)                                     *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <google/coredumper.h>
#include "merc.h"

void dump_core();

void sig_handler(int sig)
{
    un_init_signals();		/* signal caught, don't wanna catch it again, 'cause me might loop */
    switch (sig) {
    case SIGTERM:
	bug("Sig_handler: SIGTERM (crashed or killed)", 0);
	do_auto_shutdown(TRUE);
	raise(SIGTRAP);
	break;
    case SIGSEGV:
	bug("Sig_handler: SIGSEGV (crashed)", 0);
	do_auto_shutdown(TRUE);
	raise(SIGTRAP);
	break;
    case SIGFPE:
	bug("Sig_handler: SIGFPE (crashed)", 0);
	do_auto_shutdown(TRUE);
	raise(SIGTRAP);
	break;
    case SIGUSR2:
	bug("Sig_handler: SIGUSR2 (external signal for auto-rebooting)", 0);
	do_auto_shutdown(FALSE);
	raise(SIGTRAP);
	break;
    default:
	bug("Sig_handler: Unknown signal caught, shutting down", 0);
	do_auto_shutdown(TRUE);
	raise(SIGTRAP);
	break;
    }
    dump_core();
}

void sig_message()
{
    DESCRIPTOR_DATA *d, *d_next;

    bug("Sig_message: SIGTSTP (advance warning of auto-reboot)", 0);

    for (d = descriptor_list; d != NULL; d = d_next) {
        if (d->character) {
            send_to_char("\n\r{4{W                      Automatic Reboot Warning!                       {x", d->character);
	    send_to_char("\n\r{4{W              ActofWar Will be rebooting in {YONE{W minute!               {x", d->character);
	    send_to_char("\n\r{4{W       It may take up to 5 minutes for AoW to come back online.       {x", d->character);
	    send_to_char("\n\r{4{W                      * Currently in testing *                        {x", d->character);
	    send_to_char("\n\r", d->character);
        }
        d_next = d->next;
    }
}

void init_signals()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, sig_handler);
    signal(SIGSEGV, sig_handler);
    signal(SIGFPE,  sig_handler);
    signal(SIGUSR2, sig_handler);
    signal(SIGTSTP, sig_message);
}

void un_init_signals()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGFPE,  SIG_DFL);
    signal(SIGUSR2, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
}

void do_auto_shutdown(bool signal)
{

/*This allows for a shutdown without somebody in-game actually calling it. */

    FILE *fp;
    extern bool merc_down;
    DESCRIPTOR_DATA *d, *d_next;
    OBJ_DATA *obj, *obj_next;
    char buf[MAX_STRING_LENGTH];
    merc_down = TRUE;

    /* This is to write to the file. */
    fclose(fpReserve);
    if ((fp = fopen(LAST_COMMAND, "a")) == NULL) {
	bug("Error in do_auto_save opening LAST_COMMAND", 0);
        return;
    }

    fprintf(fp, "Last Command: %s\n", last_command);
    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");

    for (d = descriptor_list; d != NULL; d = d_next) {
	if (d->character) {
	    if (signal)
            {
            send_to_char("\n\r{RThere has been a crash.{x\n\r", d->character);
            if (IS_IMMORTAL(d->character))
               {
            sprintf(buf, "{WThe last command typed was:{R %s{x\n\r", last_command);
            send_to_char(buf, d->character);
               } else
               {
               send_to_char(last_pcom, d->character);
               }
            }
	    send_to_char("\n\r{WShuting down{x\n\r", d->character);
	    send_to_char("{Y***{RPFILES DO {YNOT{R SAVE ON CRASH ANYMORE!{Y***{x\n\r", d->character);
//	    do_save(d->character, ""); Causes bugs!
	}
	d_next = d->next;
	close_socket(d);
    }

    /* This is to write to the file. */
    fclose(fpReserve);
    if ((fp = fopen(OBJ_SAVE_FILE, "w")) == NULL) {
        bug("Error in do_auto_save opening OBJ_SAVE_FILE", 0);
	return;
    }
    for (obj = object_list; obj != NULL; obj = obj_next) {
        obj_next = obj->next;
	if (obj->pIndexData->vnum == OBJ_VNUM_TIER_STORAGE  || 
            obj->pIndexData->vnum == OBJ_VNUM_KAEL_STORAGE  ||
	    obj->pIndexData->vnum == OBJ_VNUM_UM_STORAGE    || 
            obj->pIndexData->vnum == OBJ_VNUM_CAIRN_STORAGE ||
	    obj->pIndexData->vnum == OBJ_VNUM_CORPSE_PC       ) {
	    if (obj->in_room      != NULL ) {
	        fprintf( fp, "#RoomVnum %d\n", obj->in_room->vnum);
	        fwrite_obj( NULL, obj, fp, 0);
	    }
	}
    }

    fprintf( fp, "\n\n#END\n");
    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");

    return;
}

void dump_core() // -- Archan
{
  // Figure out a good core dump name:
  // I figure a time stamp is fine: More than 1 crash a second is silly. 
  char file_name[MAX_STRING_LENGTH];
  //FILE * f = 0;
  //file_name[0] = 0;
  sprintf(file_name,"./cores/core-%ld",current_time);
  /*
  if(!(f = fopen(file_name,"w")))
  {
    char err[MAX_STRING_LENGTH];
    sprintf(err,"Couldn't dump %s",file_name);
    bug(err);
    return;
  }
  */
  if( WriteCoreDump(file_name) < 0)
  {
    char err[MAX_STRING_LENGTH];
    sprintf(err,"Couldn't dump %s",file_name);
    bug(err,0);
    return;
  }
   
}
