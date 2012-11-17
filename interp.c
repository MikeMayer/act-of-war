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

*	ROM 2.4 is copyright 1993-1996 Russ Taylor			   *

*	ROM has been brought to you by the ROM consortium		   *

*	    Russ Taylor (rtaylor@efn.org)				   *

*	    Gabrielle Taylor						   *

*	    Brian Moore (zump@rom.org)					   *

*	By using this code, you have agreed to follow the terms of the	   *

*	ROM license, in the file Rom24/doc/rom.license			   *

***************************************************************************/



#include <sys/types.h>

#include <ctype.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <time.h>

#include "merc.h"

#include "interp.h"

#include "recycle.h"


/*Disable*/

bool check_disabled (const struct cmd_type *command); 
DISABLED_DATA *disabled_first; 
#define END_MARKER "END" /* for load_disabled() and save_disabled() */ 



bool check_social args((CHAR_DATA * ch, char *command,

			char *argument));



char last_command[MAX_STRING_LENGTH];
char last_pcom[MAX_STRING_LENGTH];


/*

 * Command logging types.

 */



#define LOG_NORMAL	0

#define LOG_ALWAYS	1

#define LOG_NEVER	2



/*

 * Log-all switch.

 */



bool fLogAll = FALSE;



/*

 * Command table.

 */

//#define MORTAL_CMD		0
//#define IMM_SOCIAL_CMD	1
//#define IMM_BUILDER_CMD	2
//#define IMM_CODER_CMD		3
//#define IMM_ADMIN_CMD		4

const struct cmd_type cmd_table[] =
{
    /*
     * Common movement commands.
     */

	//command			function			position		level	logging	  shown		command group
    {"north",			do_north,			POS_STANDING,	0,		LOG_NEVER, 0,     MORTAL_CMD},
    {"east",			do_east,			POS_STANDING,	0,		LOG_NEVER, 0,     MORTAL_CMD},
    {"south",			do_south,			POS_STANDING,	0,		LOG_NEVER, 0,     MORTAL_CMD},
    {"west",			do_west,			POS_STANDING,	0,		LOG_NEVER, 0,     MORTAL_CMD},
    {"up",				do_up,				POS_STANDING,	0,		LOG_NEVER, 0,     MORTAL_CMD},
    {"down",			do_down,			POS_STANDING,	0,		LOG_NEVER, 0,     MORTAL_CMD},

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */

    {"at",				do_at,				POS_DEAD,		107,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"cast",			do_cast,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"auction",			do_auction,			POS_SLEEPING,	6,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"buy",				do_buy,				POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"repair",			do_repair,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"dual",			do_second,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"channels",		do_channels,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"exits",			do_exits,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"get",				do_get,				POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"group",			do_group,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"hit",				do_kill,			POS_FIGHTING,	0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"hunt",			do_hunt,			POS_STANDING,	0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"inventory",		do_inventory,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"kill",			do_kill,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"look",			do_look,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"clan",			do_clantalk,		POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"noclan",			do_noclan,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"recruit",			do_recruit,			POS_SLEEPING,	1,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"music",			do_music,			POS_SLEEPING,	6,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"news",			do_news,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"notice",			do_notice,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"newschannel",		do_info,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"map",				do_map,				POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"order",			do_order,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"practice",		do_practice,		POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"rest",			do_rest,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"sit",				do_sit,				POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"stand",			do_stand,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"deposit",			do_store,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"balance",			do_listbox,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"withdraw",		do_withdraw,		POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"tell",			do_tell,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"unlock",			do_unlock,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"wield",			do_wear,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
 

    /*
     * Informational commands.
     */

    {"affects",			do_affects,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},

    {"bug",				do_bug,				POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"changes",			do_changes,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"commands",		do_commands,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"compare",			do_compare,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"consider",		do_consider,		POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"count",			do_count,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"credits",			do_credits,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"equipment",		do_equipment,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"examine",			do_examine,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"finger",			do_finger,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"help",			do_help,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"help2",			do_help2,			POS_DEAD,		L1,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"idea",			do_idea,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"info",			do_groups,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"motd",			do_motd,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"read",			do_read,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"report",			do_report,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"rules",			do_rules,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"kingdom",			do_kingdom,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"purchase",		do_purchase,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"claim",			do_claim,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"design",			do_design,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"sale",			do_sale,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"tax",				do_tax,				POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    //{"research",		do_research,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},   
    {"memorize",		do_memorize,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"?",				do_precall,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"precall",			do_precall,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"score",			do_score,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"attribute",		do_attrib,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"strategy",		do_strategy,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"skills",			do_skills,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
	{"slist",			do_slist,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"socials",			do_socials,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"scan",			do_scan,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"show",			do_show,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"spells",			do_spells,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"story",			do_story,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"time",			do_time,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"typo",			do_typo,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"trophy",			do_trophy,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"topplayers",		do_toplist,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"warscore",		do_warscore,		POS_DEAD,		1,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"weather",			do_weather,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"who",				do_who,				POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"wizlist",			do_wizlist,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"worth",			do_worth,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"lore",			do_lore,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"focus",			do_focus,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"disrupt",			do_disrupt,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"scout",			do_scout,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},

    /*
     * Configuration commands.
     */

    {"alia",			do_alia,			POS_DEAD,		0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"alias",			do_alias,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"autolist",		do_autolist,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"tile",			do_tile,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"autoassist",		do_autoassist,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},

    {"autoexit",		do_autoexit,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"battleprompt",	do_extra,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"autogold",		do_autogold,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"autoloot",		do_autoloot,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"autosac",			do_autosac,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"autosplit",		do_autosplit,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"brief",			do_brief,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"color",			do_colour,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"combine",			do_combine,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"compact",			do_compact,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"description",		do_description,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"delet",			do_delet,			POS_DEAD,		0,		LOG_ALWAYS, 0,     MORTAL_CMD},
    {"delete",			do_delete,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"nofollow",		do_nofollow,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"nosummon",		do_nosummon,		POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"password",		do_password,		POS_DEAD,		0,		LOG_NEVER,	1,     MORTAL_CMD},
    {"prompt",			do_prompt,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"scroll",			do_scroll,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"title",			do_title,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"unalias",			do_unalias,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"wimpy",			do_wimpy,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},

    /*
     * Communication commands.
     */

    {"advice",			do_noadvice,		POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"nospam",			do_nospam,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"afk",				do_afk,				POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"answer",			do_answer,			POS_SLEEPING,	6,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"deaf",			do_deaf,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"emote",			do_emote,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"pmote",			do_pmote,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {".",				do_main_channel,	POS_SLEEPING,	0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"gossip",			do_gossip,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"crusade",			do_crufun,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"bellow",			do_bellow,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"snarl",			do_snarl,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"inform",			do_inform,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    //{"whine",			do_whine,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"trivia",			do_trivia,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {",",				do_emote,			POS_RESTING,	0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"grats",			do_grats,			POS_SLEEPING,	6,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"gtell",			do_gtell,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {";",				do_gtell,			POS_DEAD,		0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"note",			do_note,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"pose",			do_pose,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"question",		do_question,		POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"quote",			do_quote,			POS_SLEEPING,	6,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"quiet",			do_quiet,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"reply",			do_reply,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"replay",			do_replay,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"say",				do_say,				POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"'",				do_say,				POS_RESTING,	0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"shout",			do_shout,			POS_RESTING,	3,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"unread",			do_unread,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"yell",			do_yell,			POS_RESTING,	3,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"psay",			do_psay,			POS_SLEEPING,	2,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"pchannels",		do_pchannels,		POS_SLEEPING,	2,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"pclose",			do_pclose,			POS_SLEEPING,	2,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"pcreate",			do_pcreate,			POS_SLEEPING,	2,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"pdestroy",		do_pdestroy,		POS_SLEEPING,	2,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"pinfo",			do_pinfo,			POS_SLEEPING,	2,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"pjoin",			do_pjoin,			POS_SLEEPING,	2,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"poff",			do_poff,			POS_SLEEPING,	2,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"popen",			do_popen,			POS_SLEEPING,	2,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"premove",			do_premove,			POS_SLEEPING,	2,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"pwho",			do_pwho,			POS_SLEEPING,	2,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"padd",			do_padd,			POS_SLEEPING,	2,		LOG_NORMAL, 1,     MORTAL_CMD},
    //{"ignore",		do_ignore,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},   
    {"penter",			do_penter,			POS_SLEEPING,	2,		LOG_NORMAL, 1,     MORTAL_CMD},



    /*
     * Object manipulation commands.
     */

    {"brandish",		do_brandish,		POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"close",			do_close,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"drink",			do_drink,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"drop",			do_drop,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"eat",				do_eat,				POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"envenom",			do_envenom,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"fill",			do_fill,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"give",			do_give,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"heal",			do_heal,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"hold",			do_wear,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"list",			do_list,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"lock",			do_lock,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"open",			do_open,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"pick",			do_pick,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"pour",			do_pour,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"put",				do_put,				POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"quaff",			do_quaff,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"recite",			do_recite,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"remove",			do_remove,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"sell",			do_sell,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"take",			do_get,				POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"sacrifice",		do_sacrifice,		POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"junk",			do_sacrifice,		POS_RESTING,	0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"tap",				do_sacrifice,		POS_RESTING,	0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"value",			do_value,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"wear",			do_wear,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"zap",				do_zap,				POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
//    {"raid",			do_raid,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
	{"restring",		do_char_restring,	POS_RESTING,	0,		LOG_ALWAYS, 1,     MORTAL_CMD},

    /*
     * Combat commands.
     */

    {"backstab",		do_backstab,		POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"jump",			do_jump,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"weaponbreak",		do_weapon_break,	POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"tear",			do_tear,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"bash",			do_bash,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"shieldbash",		do_shieldbash,		POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"skewer",			do_skewer,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"step",			do_step,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"throw",			do_throw,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"bs",				do_backstab,		POS_FIGHTING,	0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"berserk",			do_berserk,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"circle",			do_circle,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD}, 
    {"lunge",			do_lunge,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
 //   {"dragonsword",	do_lunge,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"charge",			do_charge,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"rush",			do_rush,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"dirt",			do_dirt,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"disarm",			do_disarm,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"flee",			do_flee,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"kick",			do_kick,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"geomancy",		do_geomancy,		POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"earthlore",		do_earthlore,		POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"magic song",		do_magic_song,		POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"life song",		do_life_song,		POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"learning song",	do_learning_song,	POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"speed song",		do_speed_song,		POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"defense song",	do_defense_song,	POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"attack song",		do_attack_song,		POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD}, 
    {"strike",			do_strike,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"murde",			do_kill,			POS_FIGHTING,	0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"murder",			do_kill,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"rescue",			do_rescue,			POS_FIGHTING,	0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"stun",			do_stun,			POS_FIGHTING,	0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"surrender",		do_surrender,		POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"trip",			do_trip,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"trap",			do_trap,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"bluff",			do_bluff,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"struggle",		do_struggle,		POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"mend",			do_mend,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"chase",			do_chase,			POS_STANDING,	0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"push",			do_push,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"drag",			do_drag,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"braver",			do_braver,			POS_FIGHTING,	1,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"crossslash",		do_cross_slash,		POS_FIGHTING,	1,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"bladebeam",		do_blade_beam,		POS_FIGHTING,	1,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"climhazzard",		do_climhazzard,		POS_FIGHTING,	1,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"meteorain",		do_meteorain,		POS_FIGHTING,	1,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"finishingtouch",	do_finishing_touch, POS_FIGHTING,	1,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"omnislash",		do_omnislash,		POS_FIGHTING,	1,		LOG_NORMAL, 1,     MORTAL_CMD},
    /*
     * Mob command interpreter (placed here for faster scan...)
     */

    {"mob",				do_mob, POS_DEAD, 0, LOG_NEVER, 0,     MORTAL_CMD},

    /*
     * Miscellaneous commands.
     */

    {"enter",			do_enter,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"follow",			do_follow,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"gain",			do_gain,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"go",				do_enter,			POS_STANDING,	0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"groups",			do_groups,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"hide",			do_hide,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"play",			do_play,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"qui",				do_qui,				POS_DEAD,		0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"quit",			do_quit,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"raise",			do_raise,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"recall",			do_recall,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"/",				do_recall,			POS_FIGHTING,	0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"rent",			do_rent,			POS_DEAD,		0,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"save",			do_save,			POS_DEAD,		0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"sleep",			do_sleep,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"sneak",			do_sneak,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"split",			do_split,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"steal",			do_steal,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"train",			do_train,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"visible",			do_visible,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"wake",			do_wake,			POS_SLEEPING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"where",			do_where,			POS_RESTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"mount",			do_mount,			POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"dismount",		do_dismount,		POS_FIGHTING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"shrink",			do_shrink,			POS_STANDING,	0,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"sellsword",		do_sellsword,		POS_DEAD,		100,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"align",			do_align,			POS_RESTING,	50,		LOG_ALWAYS, 1,     MORTAL_CMD},
    {"fares,",			do_fares,			POS_RESTING,	50,		LOG_ALWAYS, 1,     MORTAL_CMD},

    /*
     * Immortal commands.
     */   
    {"pcheck",			do_pcheck,			POS_SLEEPING,	L1,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"outfit",			do_outfit,			POS_RESTING,	107,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"quest",			do_quest,			POS_RESTING,	103,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"wizhelp",			do_wizhelp,			POS_DEAD,		IM,		LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"goto",			do_goto,			POS_DEAD,		105,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"guild",			do_guild,			POS_DEAD,		107,	LOG_ALWAYS, 1,     IMM_SOCIAL_CMD},	
    {"leader",			do_leader,			POS_SLEEPING,	107,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"sockets",			do_sockets,			POS_DEAD,		107,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"nonote",			do_nonote,			POS_DEAD,		109,	LOG_ALWAYS, 1,     IMM_SOCIAL_CMD}, 
//    {"spup",			do_spup,			POS_DEAD,		105,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"remort",			do_remort,			POS_STANDING,	50,		LOG_NORMAL, 1,     MORTAL_CMD},
    {"rename",			do_rename,			POS_DEAD,		1,		LOG_ALWAYS, 1,     IMM_SOCIAL_CMD},
    {"rip",				do_sever,			POS_DEAD,		107,	LOG_ALWAYS, 1,     IMM_SOCIAL_CMD},
    {"bonus",			do_bonus,			POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"advance",			do_advance,			POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD}, 
	{"impAdvance",		implementorAdvance, POS_DEAD,		111,	LOG_ALWAYS, 1,	   IMM_ADMIN_CMD},
	{"admtalk",			implementorTalk,	POS_DEAD,		111,	LOG_NORMAL,	1,	   IMM_ADMIN_CMD},
    {"copyover",		do_copyover,		POS_DEAD,		110,	LOG_ALWAYS, 1,     IMM_CODER_CMD},
    {"coredump",		do_coredump,		POS_DEAD,		110,	LOG_ALWAYS, 1,     IMM_CODER_CMD},
    {"avator",			do_avator,			POS_DEAD,		105,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"pload",			do_pload,			POS_DEAD,		L1,		LOG_ALWAYS, 1,     IMM_ADMIN_CMD},
    {"punload",			do_punload,			POS_DEAD,		L1,		LOG_ALWAYS, 1,     IMM_ADMIN_CMD},
//    {"pipe",			do_pipe,			POS_DEAD,		ML,		LOG_ALWAYS, 1,     MORTAL_CMD},
    {"wpeace",			do_wpeace,			POS_DEAD,		L1,		LOG_ALWAYS, 1,     IMM_SOCIAL_CMD},
    {"dump",			do_dump,			POS_DEAD,		ML,		LOG_ALWAYS, 0,     IMM_CODER_CMD},
    {"trust",			do_trust,			POS_DEAD,		110,	LOG_ALWAYS, 1,     IMM_ADMIN_CMD},
    {"violate",			do_violate,			POS_DEAD,		L1,		LOG_ALWAYS, 1,     IMM_ADMIN_CMD},
    {"rawquit",			do_rawquit,			POS_DEAD,		110,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"allow",			do_allow,			POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"ban",				do_ban,				POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"banis",			do_banis,			POS_DEAD,		1,		LOG_ALWAYS, 1,     MORTAL_CMD},
    {"banish",			do_banish,			POS_DEAD,		1,		LOG_ALWAYS, 1,     MORTAL_CMD},
    {"deny",			do_deny,			POS_DEAD,		L1,		LOG_ALWAYS, 1,     MORTAL_CMD},
    {"disconnect",		do_disconnect,		POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"flag",			do_flag,			POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"freeze",			do_freeze,			POS_DEAD,		103,	LOG_ALWAYS, 1,     IMM_SOCIAL_CMD},
    {"ic",				do_ice,				POS_DEAD,		1,		LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"ice",				do_ice,				POS_DEAD,		1,		LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"permban",			do_permban,			POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"protect",			do_protect,			POS_DEAD,		110,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"reboo",			do_reboo,			POS_DEAD,		L1,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"reboot",			do_reboot,			POS_DEAD,		L1,		LOG_ALWAYS, 1,     MORTAL_CMD},
    {"sameip",			do_sameip,			POS_DEAD,		L1,		LOG_ALWAYS, 1,     MORTAL_CMD},
    {"set",				do_set,				POS_DEAD,		109,	LOG_ALWAYS, 1,     IMM_ADMIN_CMD},
    {"shutdow",			do_shutdow,			POS_DEAD,		L1,		LOG_NORMAL, 0,     IMM_CODER_CMD},
    {"shutdown",		do_shutdown,		POS_DEAD,		L1,		LOG_ALWAYS, 1,     IMM_CODER_CMD},
    {"wizlock",			do_wizlock,			POS_DEAD,		105,	LOG_ALWAYS, 1,     IMM_CODER_CMD},
//    {"setskill",		do_setskill,		POS_DEAD,		110,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"force",			do_force,			POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"load",			do_load,			POS_DEAD,		105,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"newlock",			do_newlock,			POS_DEAD,		105,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"infect",			do_infect,			POS_DEAD,		105,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"armageddo",		do_armageddo,		POS_DEAD,		110,	LOG_ALWAYS, 1,     IMM_ADMIN_CMD},
    {"saltrecall",		do_saltrecall,		POS_DEAD,		110,	LOG_ALWAYS, 1,     IMM_ADMIN_CMD},
    {"saltrecal",		do_saltrecal,		POS_DEAD,		110,	LOG_ALWAYS, 1,     IMM_ADMIN_CMD},
    {"armageddonquest", do_armageddon,		POS_DEAD,		110,	LOG_ALWAYS, 1,     IMM_ADMIN_CMD},
    {"nochannels",		do_nochannels,		POS_DEAD,		1,		LOG_ALWAYS, 1,     MORTAL_CMD},
    {"noemote",			do_noemote,			POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"noshout",			do_noshout,			POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"notell",			do_notell,			POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"pecho",			do_pecho,			POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"pardon",			do_pardon,			POS_DEAD,		1,		LOG_ALWAYS, 1,     MORTAL_CMD},
    {"constable",		do_constable,		POS_DEAD,		1,		LOG_ALWAYS, 1,     MORTAL_CMD},
    {"mconstable",		do_mconstable,		POS_DEAD,		110,	LOG_ALWAYS, 1,     IMM_ADMIN_CMD},   
    {"mhconstable",		do_mhconstable,		POS_DEAD,		110,	LOG_ALWAYS, 1,     IMM_ADMIN_CMD},
//   {"addhelp",		do_addhelp,			POS_DEAD,		110,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"purge",			do_purge,			POS_DEAD,		109,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"restore",			do_restore,			POS_DEAD,		109,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"sla",				do_sla,				POS_DEAD,		105,	LOG_NORMAL, 0,     MORTAL_CMD},
    {"slay",			do_slay,			POS_DEAD,		105,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"teleport",		do_transfer,		POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"transfer",		do_transfer,		POS_DEAD,		105,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"poofin",			do_bamfin,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"poofout",			do_bamfout,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"gecho",			do_echo,			POS_DEAD,		103,	LOG_ALWAYS, 1,     IMM_SOCIAL_CMD},
    {"holylight",		do_holylight,		POS_DEAD,		103,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"incognito",		do_incognito,		POS_DEAD,		2,		LOG_NORMAL, 0,     MORTAL_CMD},
    {"invis",			do_invis,			POS_DEAD,		103,	LOG_NORMAL, 0,     MORTAL_CMD},
    {"log",				do_log,				POS_DEAD,		L1,		LOG_ALWAYS, 1,     MORTAL_CMD},
    {"memory",			do_memory,			POS_DEAD,		107,	LOG_NORMAL, 1,     IMM_CODER_CMD},
    {"mwhere",			do_mwhere,			POS_DEAD,		105,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"owhere",			do_owhere,			POS_DEAD,		105,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"peace",			do_peace,			POS_DEAD,		107,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"penalty",			do_penalty,			POS_DEAD,		103,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"echo",			do_recho,			POS_DEAD,		107,	LOG_ALWAYS, 1,     IMM_SOCIAL_CMD},
    {"return",			do_return,			POS_DEAD,		107,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"snoop",			do_snoop,			POS_DEAD,		110,	LOG_ALWAYS, 1,     IMM_SOCIAL_CMD},
    {"stat",			do_stat,			POS_DEAD,		105,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"string",			do_string,			POS_DEAD,		105,	LOG_ALWAYS, 1,     IMM_SOCIAL_CMD},
    {"switch",			do_switch,			POS_DEAD,		107,	LOG_ALWAYS, 1,     IMM_SOCIAL_CMD},
    {"wizinvis",		do_invis,			POS_DEAD,		102,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"vnum",			do_vnum,			POS_DEAD,		105,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"zecho",			do_zecho,			POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"clone",			do_clone,			POS_DEAD,		107,	LOG_ALWAYS, 1,     MORTAL_CMD},
    {"wiznet",			do_wiznet,			POS_DEAD,		110,	LOG_NORMAL, 1,     IMM_ADMIN_CMD},
    {"immtalk",			do_immtalk,			POS_DEAD,		103,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"imotd",			do_imotd,			POS_DEAD,		105,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {":",				do_immtalk,			POS_DEAD,		105,	LOG_NORMAL, 0,     IMM_SOCIAL_CMD},
    {"smote",			do_smote,			POS_DEAD,		107,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"prefi",			do_prefi,			POS_DEAD,		107,	LOG_NORMAL, 0,     IMM_CODER_CMD},
    {"prefix",			do_prefix,			POS_DEAD,		107,	LOG_NORMAL, 1,     IMM_CODER_CMD},
    {"mpdump",			do_mpdump,			POS_DEAD,		105,	LOG_NEVER,	1,     IMM_CODER_CMD},
    {"newdump",			do_new_dump,		POS_DEAD,		110,	LOG_ALWAYS, 1,     IMM_CODER_CMD},
//    {"helpdump",		do_help_dump,		POS_DEAD,		ML,		LOG_ALWAYS, 1,     IMM_CODER_CMD},
    {"skilldump",		do_skill_dump,		POS_DEAD,		ML,		LOG_ALWAYS, 1,     IMM_CODER_CMD},
    {"illegalize",		do_illegalize,		POS_DEAD,		107,	LOG_ALWAYS, 1,     IMM_ADMIN_CMD},
    {"fremove",			do_remove_all,		POS_DEAD,		107,	LOG_ALWAYS, 1,     IMM_SOCIAL_CMD},
    {"addlag",			do_addlag,			POS_DEAD,		110,	LOG_ALWAYS, 1,     IMM_ADMIN_CMD},
    {"showclan",		do_showclan,		POS_DEAD,		107,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},
    {"disable",			do_disable,			POS_DEAD,		109,	LOG_ALWAYS, 1,     IMM_SOCIAL_CMD},
	{"gnews",			do_gnews,			POS_DEAD,		109,	LOG_ALWAYS, 0,     IMM_SOCIAL_CMD},
	{"adviceedit",		do_advice_editor,	POS_DEAD,		109,	LOG_NORMAL, 1,     IMM_SOCIAL_CMD},

    /*
     * OLC & Builder Commands
     */
    {"mpstat",			do_mpstat,			POS_DEAD,		105,	LOG_NEVER,	1,     IMM_BUILDER_CMD},
    {"areas",			do_mapping,			POS_DEAD,		105,	LOG_NORMAL, 1,     IMM_BUILDER_CMD},
    {"autobuild",		do_autobuild,		POS_DEAD,		105,	LOG_NORMAL, 1,     IMM_BUILDER_CMD},
    {"arealinks",		do_arealinks,		POS_DEAD,		105,	LOG_NORMAL, 1,	   IMM_BUILDER_CMD},
    {"edit",			do_olc,				POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"fvlist",			do_fvlist,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"asave",			do_asave,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"alist",			do_alist,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"resets",			do_resets,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"redit",			do_redit,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"medit",			do_medit,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"aedit",			do_aedit,			POS_DEAD,		109,	LOG_NORMAL, 1,     MORTAL_CMD},   
    {"spup",			do_spup,			POS_DEAD,		109,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"scatter",			do_scatter,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"oedit",			do_oedit,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"mpedit",			do_mpedit,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"rcopy",			do_rcopy,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"mcopy",			do_mcopy,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"ocopy",			do_ocopy,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD},
    {"xlist",			do_xlist,			POS_DEAD,		105,	LOG_NORMAL, 1,     MORTAL_CMD}, 

    /*
     * End of list.
     */



    {"", 0, POS_DEAD, 0, LOG_NORMAL, 0}};



/*

 * The main entry point for executing commands.

 * Can be recursively called from 'at', 'order', 'force'.

 */

void interpret(CHAR_DATA * ch, char *argument)

{

    extern int nAllocString;

    extern int nAllocPerm;

    int string_count = nAllocString;

    int perm_count = nAllocPerm;

    char cmd_copy[MAX_INPUT_LENGTH];

    char buf[MAX_STRING_LENGTH];



    char command[MAX_INPUT_LENGTH];

    char logline[MAX_INPUT_LENGTH];

    int cmd;

    int trust;

    bool found;



    /*

     * Strip leading spaces.

     */

    while (isspace(*argument))

	argument++;

    if (argument[0] == '\0')

	return;



    strcpy(cmd_copy, argument);



    /*

     * No hiding.

     */

    REMOVE_BIT(ch->affected_by, AFF_HIDE);



    /*

     * Implement freeze command.

     */

    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE)) {

	send_to_char("You're totally frozen!\n\r", ch);

	return;

    }

    if (!IS_NPC(ch) && IS_SET(ch->affected2_by, AFF_IMM_FROZEN)) {

	send_to_char("You're totally frozen!\n\r", ch);

	return;

    }

    if (IS_AFFECTED(ch, AFF_HOLD) && !IS_IMMORTAL(ch) )
    {
	send_to_char("You're a victim of time!\n\r", ch);
	return;
    }

    /*

     * Grab the command word.

     * Special parsing so ' can be a command,

     *   also no spaces needed after punctuation.

     */

    strcpy(logline, argument);

    /*Lets see who is doing what? -Ferric */

    strcpy(buf, argument);

    sprintf(last_command, "%s in room[%d]: %s.", ch->name, ch->in_room->vnum, buf);
    sprintf(last_pcom, "{R%s{W is the last person to type a command.\n\r"
                       "This does not mean that this person crashed it on purpose!\n\r"
                       "But it does mean that Nomic won't stop harrassing me about fixing Tasydra's typo!\n\r"
                       "This is to just let you know who did it.{x\n\r", ch->name);

    if (!isalpha(argument[0]) && !isdigit(argument[0])) {

	command[0] = argument[0];

	command[1] = '\0';

	argument++;

	while (isspace(*argument))

	    argument++;

    } else {

	argument = one_argument(argument, command);

    }



    /*

     * Look for command in command table.

     */

    found = FALSE;

    trust = get_trust(ch);

    for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) {

	if (command[0] == cmd_table[cmd].name[0]

	    && !str_prefix(command, cmd_table[cmd].name)

	    && cmd_table[cmd].level <= trust ) {

	    found = TRUE;

	    break;

	}

    }



    /*

     * Log and snoop.

     */

    if (cmd_table[cmd].log == LOG_NEVER)

	strcpy(logline, "");



    if ((!IS_NPC(ch) && IS_SET(ch->act, PLR_LOG))

	|| fLogAll

	|| cmd_table[cmd].log == LOG_ALWAYS) {

	sprintf(log_buf, "Log %s: %s", ch->name, logline);

	wiznet(log_buf, ch, NULL, WIZ_SECURE, 0, get_trust(ch));

	log_string(log_buf);

    }

    if (ch->desc != NULL && ch->desc->snoop_by != NULL) {

	write_to_buffer(ch->desc->snoop_by, "% ", 2);

	write_to_buffer(ch->desc->snoop_by, logline, 0);

	write_to_buffer(ch->desc->snoop_by, "\n\r", 2);

    }

    if (!found) {

	/*

	 * Look for command in socials table.

	 */

	if (!check_social(ch, command, argument))

	    send_to_char("Huh?\n\r", ch);

	return;

    }
else /* a normal valid command.. check if it is disabled */ 
if (check_disabled (&cmd_table[cmd]))  
{  send_to_char ("This command has been temporarily disabled.\n\r",ch); 
 return; 
 } 



    /*

     * Character not in position for command?

     */

    if (ch->position < cmd_table[cmd].position) {

	switch (ch->position) {

	case POS_DEAD:

	    send_to_char("Lie still; you are DEAD.\n\r", ch);

	    break;



	case POS_MORTAL:

	case POS_INCAP:

	    send_to_char("You are hurt far too bad for that.\n\r", ch);

	    break;



	case POS_STUNNED:

	    send_to_char("You are too stunned to do that.\n\r", ch);

	    break;



	case POS_SLEEPING:

	    send_to_char("In your dreams, or what?\n\r", ch);

	    break;



	case POS_RESTING:

	    send_to_char("Nah... You feel too relaxed...\n\r", ch);

	    break;



	case POS_SITTING:

	    send_to_char("Better stand up first.\n\r", ch);

	    break;



	case POS_FIGHTING:



	//bug patch

	    if (ch->fighting == NULL) ch->position = POS_STANDING;

	    else if(ch->fighting->in_room != ch->in_room) ch->position = POS_STANDING;



	    send_to_char("No way!  You are still fighting!\n\r", ch);

	    break;



	}

	return;

    }

    /*

     * Dispatch the command.

     */



    if (!IS_NPC(ch)) {

	if (ch->pcdata->confirm_delete && cmd_table[cmd].name != "delete") {

	    send_to_char("Delete status removed.\n\r", ch);

	    ch->pcdata->confirm_delete = FALSE;

	}

    }

    (*cmd_table[cmd].do_fun) (ch, argument);



    if (string_count < nAllocString) {

	sprintf(buf,

		"Memcheck : Increase in strings :: %s : %s", ch->name, cmd_copy);

	wiznet(buf, NULL, NULL, WIZ_MEMCHECK, 0, 0);

    }

    if (perm_count < nAllocPerm) {

	sprintf(buf,

		"Increase in perms :: %s : %s", ch->name, cmd_copy);

	wiznet(buf, NULL, NULL, WIZ_MEMCHECK, 0, 0);

    }

    tail_chain();

    return;

}



bool check_social(CHAR_DATA * ch, char *command, char *argument)

{

    char arg[MAX_INPUT_LENGTH];

    CHAR_DATA *victim;

    int cmd;

    bool found;



    found = FALSE;

    for (cmd = 0; social_table[cmd].name[0] != '\0'; cmd++) {

	if (command[0] == social_table[cmd].name[0]

	    && !str_prefix(command, social_table[cmd].name)) {

	    found = TRUE;

	    break;

	}

    }



    if (!found)

	return FALSE;



    if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE)) {

	send_to_char("You are anti-social!\n\r", ch);

	return TRUE;

    }

    switch (ch->position) {

    case POS_DEAD:

	send_to_char("Lie still; you are DEAD.\n\r", ch);

	return TRUE;



    case POS_INCAP:

    case POS_MORTAL:

	send_to_char("You are hurt far too bad for that.\n\r", ch);

	return TRUE;



    case POS_STUNNED:

	send_to_char("You are too stunned to do that.\n\r", ch);

	return TRUE;



    case POS_SLEEPING:

	/*

	 * I just know this is the path to a 12" 'if' statement.  :(

	 * But two players asked for it already!  -- Furey

	 */
	
	//Snore is spammy and annoying - Akuji 3/23/03

	//if (!str_cmp(social_table[cmd].name, "snore"))

	//    break;

	send_to_char("In your dreams, or what?\n\r", ch);

	return TRUE;



    }



    one_argument(argument, arg);

    victim = NULL;

    if (arg[0] == '\0') {

	act(social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM);

	act(social_table[cmd].char_no_arg, ch, NULL, victim, TO_CHAR);

    } else if ((victim = get_char_room(ch, arg)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

    } else if (victim == ch) {

	act(social_table[cmd].others_auto, ch, NULL, victim, TO_ROOM);

	act(social_table[cmd].char_auto, ch, NULL, victim, TO_CHAR);

    } else {

	act(social_table[cmd].others_found, ch, NULL, victim, TO_NOTVICT);

	act(social_table[cmd].char_found, ch, NULL, victim, TO_CHAR);

	act(social_table[cmd].vict_found, ch, NULL, victim, TO_VICT);



	if (!IS_NPC(ch) && IS_NPC(victim)

	    && !IS_AFFECTED(victim, AFF_CHARM)

	    && IS_AWAKE(victim)

	    && victim->desc == NULL) {

	    switch (number_bits(4)) {

	    case 0:



	    case 1:

	    case 2:

	    case 3:

	    case 4:

	    case 5:

	    case 6:

	    case 7:

	    case 8:

		act(social_table[cmd].others_found,

		    victim, NULL, ch, TO_NOTVICT);

		act(social_table[cmd].char_found,

		    victim, NULL, ch, TO_CHAR);

		act(social_table[cmd].vict_found,

		    victim, NULL, ch, TO_VICT);

		break;



	    case 9:

	    case 10:

	    case 11:

	    case 12:

		act("$n slaps $N.", victim, NULL, ch, TO_NOTVICT);

		act("You slap $N.", victim, NULL, ch, TO_CHAR);

		act("$n slaps you.", victim, NULL, ch, TO_VICT);

		break;

	    }

	}

    }



    return TRUE;

}



/*

 * Return true if an argument is completely numeric.

 */

bool is_number(char *arg)

{



    if (*arg == '\0')

	return FALSE;



    if (*arg == '+' || *arg == '-')

	arg++;



    for (; *arg != '\0'; arg++) {

	if (!isdigit(*arg))

	    return FALSE;

    }



    return TRUE;

}



/*

 * Given a string like 14.foo, return 14 and 'foo'

 */

int number_argument(char *argument, char *arg)

{

    char *pdot;

    int number;



    for (pdot = argument; *pdot != '\0'; pdot++) {

	if (*pdot == '.') {

	    *pdot = '\0';

	    number = atoi(argument);

	    *pdot = '.';

	    strcpy(arg, pdot + 1);

	    return number;

	}

    }



    strcpy(arg, argument);

    return 1;

}



/*

 * Given a string like 14*foo, return 14 and 'foo'

 */

int mult_argument(char *argument, char *arg)

{

    char *pdot;

    int number;



    for (pdot = argument; *pdot != '\0'; pdot++) {

	if (*pdot == '*') {

	    *pdot = '\0';

	    number = atoi(argument);

	    *pdot = '*';

	    strcpy(arg, pdot + 1);

	    return number;

	}

    }



    strcpy(arg, argument);

    return 1;

}



/*

 * Pick off one argument from a string and return the rest.

 * Understands quotes.

 */

char *one_argument(char *argument, char *arg_first)

{

    char cEnd;



    while (isspace(*argument))

	argument++;



    cEnd = ' ';

    if (*argument == '\'' || *argument == '"')

	cEnd = *argument++;



    while (*argument != '\0') {

	if (*argument == cEnd) {

	    argument++;

	    break;

	}

	*arg_first = LOWER(*argument);

	arg_first++;

	argument++;

    }

    *arg_first = '\0';



    while (isspace(*argument))

	argument++;



    return argument;

}



/*

 * Contributed by Alander.

 */

void do_commands(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];

    int cmd;

    int col;



    col = 0;

    for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) {

	if (cmd_table[cmd].level < LEVEL_HERO

	    && cmd_table[cmd].level <= get_trust(ch)

	    && cmd_table[cmd].show) {

	    sprintf(buf, "%-12s", cmd_table[cmd].name);

	    send_to_char(buf, ch);

	    if (++col % 6 == 0)

		send_to_char("\n\r", ch);

	}

    }



    if (col % 6 != 0)

	send_to_char("\n\r", ch);

    return;

}



void do_command_help_dump(CHAR_DATA *ch, char*argument)

{

    FILE *fp;

    int cmd;



    /* open file */

    fclose(fpReserve);



    /* start printing out help data */

    fp = fopen("../commands.hlp", "w");



    fprintf(fp, "#HELPS\n");



    for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) {

        fprintf(fp, "%d '%s'~\n.\n", cmd_table[cmd].level < LEVEL_HERO ? -1 : cmd_table[cmd].level,

                cmd_table[cmd].name);

        fprintf(fp, "   {w{4   Command Name: {x%s\n\n~\n", cmd_table[cmd].name);

    }



    fprintf(fp, "0 $~\n\n#$");

    fclose(fp);

    fpReserve = fopen(NULL_FILE, "r");

}



void do_wizhelp(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col = 0;
    BUFFER *output;
    output = new_buf();

    for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) 
    {
		if ((cmd_table[cmd].level == get_trust(ch))
		    && (cmd_table[cmd].level >= LEVEL_HERO)
	    	&& cmd_table[cmd].show) 
	    	{
	    	sprintf(buf, "{w[{y%3d{w] {c%-13.12s{x", cmd_table[cmd].level, cmd_table[cmd].name);
	    	add_buf(output, buf);
	    	if (++col % 4 == 0)
			add_buf(output, "{x\n\r");
			}
    }
    
        for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) 
    {
		if ((cmd_table[cmd].level == get_trust(ch)-1)
		    && (cmd_table[cmd].level >= LEVEL_HERO)
	    	&& cmd_table[cmd].show) 
	    	{
	    	sprintf(buf, "{w[{y%3d{w] {c%-13.12s{x", cmd_table[cmd].level, cmd_table[cmd].name);
	    	add_buf(output, buf);
	    	if (++col % 4 == 0)
			add_buf(output, "{x\n\r");
			}
    }
    
            for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) 
    {
		if ((cmd_table[cmd].level == get_trust(ch)-2)
		    && (cmd_table[cmd].level >= LEVEL_HERO)
	    	&& cmd_table[cmd].show) 
	    	{
	    	sprintf(buf, "{w[{y%3d{w] {c%-13.12s{x", cmd_table[cmd].level, cmd_table[cmd].name);
	    	add_buf(output, buf);
	    	if (++col % 4 == 0)
			add_buf(output, "{x\n\r");
			}
    }
    
                for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) 
    {
		if ((cmd_table[cmd].level == get_trust(ch)-3)
		    && (cmd_table[cmd].level >= LEVEL_HERO)
	    	&& cmd_table[cmd].show) 
	    	{
	    	sprintf(buf, "{w[{y%3d{w] {c%-13.12s{x", cmd_table[cmd].level, cmd_table[cmd].name);
	    	add_buf(output, buf);
	    	if (++col % 4 == 0)
			add_buf(output, "{x\n\r");
			}
    }
    
                for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) 
    {
		if ((cmd_table[cmd].level == get_trust(ch)-4)
		    && (cmd_table[cmd].level >= LEVEL_HERO)
	    	&& cmd_table[cmd].show) 
	    	{
	    	sprintf(buf, "{w[{y%3d{w] {c%-13.12s{x", cmd_table[cmd].level, cmd_table[cmd].name);
	    	add_buf(output, buf);
	    	if (++col % 4 == 0)
			add_buf(output, "{x\n\r");
			}
    }
    
                for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) 
    {
		if ((cmd_table[cmd].level == get_trust(ch)-5)
		    && (cmd_table[cmd].level >= LEVEL_HERO)
	    	&& cmd_table[cmd].show) 
	    	{
	    	sprintf(buf, "{w[{y%3d{w] {c%-13.12s{x", cmd_table[cmd].level, cmd_table[cmd].name);
	    	add_buf(output, buf);
	    	if (++col % 4 == 0)
			add_buf(output, "{x\n\r");
			}
    }
    
                for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) 
    {
		if ((cmd_table[cmd].level == get_trust(ch)-6)
		    && (cmd_table[cmd].level >= LEVEL_HERO)
	    	&& cmd_table[cmd].show) 
	    	{
	    	sprintf(buf, "{w[{y%3d{w] {c%-13.12s{x", cmd_table[cmd].level, cmd_table[cmd].name);
	    	add_buf(output, buf);
	    	if (++col % 4 == 0)
			add_buf(output, "{x\n\r");
			}
    }
    
                for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) 
    {
		if ((cmd_table[cmd].level == get_trust(ch)-7)
		    && (cmd_table[cmd].level >= LEVEL_HERO)
	    	&& cmd_table[cmd].show) 
	    	{
	    	sprintf(buf, "{w[{y%3d{w] {c%-13.12s{x", cmd_table[cmd].level, cmd_table[cmd].name);
	    	add_buf(output, buf);
	    	if (++col % 4 == 0)
			add_buf(output, "{x\n\r");
			}
    }
    
                for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) 
    {
		if ((cmd_table[cmd].level == get_trust(ch)-8)
		    && (cmd_table[cmd].level >= LEVEL_HERO)
	    	&& cmd_table[cmd].show) 
	    	{
	    	sprintf(buf, "{w[{y%3d{w] {c%-13.12s{x", cmd_table[cmd].level, cmd_table[cmd].name);
	    	add_buf(output, buf);
	    	if (++col % 4 == 0)
			add_buf(output, "{x\n\r");
			}
    }
    
                for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) 
    {
		if ((cmd_table[cmd].level == get_trust(ch)-9)
		    && (cmd_table[cmd].level >= LEVEL_HERO)
	    	&& cmd_table[cmd].show) 
	    	{
	    	sprintf(buf, "{w[{y%3d{w] {c%-13.12s{x", cmd_table[cmd].level, cmd_table[cmd].name);
	    	add_buf(output, buf);
	    	if (++col % 4 == 0)
			add_buf(output, "{x\n\r");
			}
    }

    if (col % 4 != 0)
	add_buf(output, "{x\n\r");
    page_to_char(buf_string(output), ch);
    free_buf(output);
    return;
}


void do_disable (CHAR_DATA *ch, char *argument)
{
	int i;
	DISABLED_DATA *p,*q;
	char buf[100];
	
	if (IS_NPC(ch))
	{
		send_to_char ("RETURN first.\n\r",ch);
		return;
	}
	
	if (!argument[0]) /* Nothing specified. Show disabled commands. */
	{
		if (!disabled_first) /* Any disabled at all ? */
		{
			send_to_char ("There are no commands disabled.\n\r",ch);
			return;
		}

		send_to_char ("Disabled commands:\n\r"
		              "Command      Level   Disabled by\n\r",ch);
		                
		for (p = disabled_first; p; p = p->next)
		{
			sprintf (buf, "%-12s %5d   %-12s\n\r",p->command->name, p->level, p->disabled_by);
			send_to_char (buf,ch);
		}
		return;
	}
	
	/* command given */

	/* First check if it is one of the disabled commands */
	for (p = disabled_first; p ; p = p->next)
		if (!str_cmp(argument, p->command->name))
			break;
			
	if (p) /* this command is disabled */
	{
	/* Optional: The level of the imm to enable the command must equal or exceed level
	   of the one that disabled it */
	
		if (get_trust(ch) < p->level)
		{
			send_to_char ("This command was disabled by a higher power.\n\r",ch);
			return;
		}
		
		/* Remove */
		
		if (disabled_first == p) /* node to be removed == head ? */
			disabled_first = p->next;
		else /* Find the node before this one */
		{
			for (q = disabled_first; q->next != p; q = q->next); /* empty for */
			q->next = p->next;
		}
		
		free_string (p->disabled_by); /* free name of disabler */
		free_mem (p,sizeof(DISABLED_DATA)); /* free node */
		save_disabled(); /* save to disk */
		send_to_char ("Command enabled.\n\r",ch);
	}
	else /* not a disabled command, check if that command exists */
	{
		/* IQ test */
		if (!str_cmp(argument,"disable"))
		{
			send_to_char ("You cannot disable the disable command.\n\r",ch);
			return;
		}

		/* Search for the command */
		for (i = 0; cmd_table[i].name[0] != '\0'; i++)
			if (!str_cmp(cmd_table[i].name, argument))
				break;

		/* Found? */				
		if (cmd_table[i].name[0] == '\0')
		{
			send_to_char ("No such command.\n\r",ch);
			return;
		}

		/* Can the imm use this command at all ? */				
		if (cmd_table[i].level > get_trust(ch))
		{
			send_to_char ("You dot have access to that command; you cannot disable it.\n\r",ch);
			return;
		}
		
		/* Disable the command */
		
		p = alloc_mem (sizeof(DISABLED_DATA));

		p->command = &cmd_table[i];
		p->disabled_by = str_dup (ch->name); /* save name of disabler */
		p->level = get_trust(ch); /* save trust */
		p->next = disabled_first;
		disabled_first = p; /* add before the current first element */
		
		send_to_char ("Command disabled.\n\r",ch);
		save_disabled(); /* save to disk */
	}
}

/* Check if that command is disabled 
   Note that we check for equivalence of the do_fun pointers; this means
   that disabling 'chat' will also disable the '.' command
*/   
bool check_disabled (const struct cmd_type *command)
{
	DISABLED_DATA *p;
	
	for (p = disabled_first; p ; p = p->next)
		if (p->command->do_fun == command->do_fun)
			return TRUE;

	return FALSE;
}

/* Load disabled commands */
void load_disabled()
{
	FILE *fp;
	DISABLED_DATA *p;
	char *name;
	int i;
	
	disabled_first = NULL;
	
	fp = fopen (DISABLED_FILE, "r");
	
	if (!fp) /* No disabled file.. no disabled commands : */
		return;
		
	name = fread_word (fp);
	
	while (str_cmp(name, END_MARKER)) /* as long as name is NOT END_MARKER :) */
	{
		/* Find the command in the table */
		for (i = 0; cmd_table[i].name[0] ; i++)
			if (!str_cmp(cmd_table[i].name, name))
				break;
				
		if (!cmd_table[i].name[0]) /* command does not exist? */
		{
			bug ("Skipping uknown command in " DISABLED_FILE " file.",0);
			fread_number(fp); /* level */
			fread_word(fp); /* disabled_by */
		}
		else /* add new disabled command */
		{
			p = alloc_mem(sizeof(DISABLED_DATA));
			p->command = &cmd_table[i];
			p->level = fread_number(fp);
			p->disabled_by = str_dup(fread_word(fp)); 
			p->next = disabled_first;
			
			disabled_first = p;

		}
		
		name = fread_word(fp);
	}

	fclose (fp);		
}

/* Save disabled commands */
void save_disabled()
{
	FILE *fp;
	DISABLED_DATA *p;
	
	if (!disabled_first) /* delete file if no commands are disabled */
	{
		unlink (DISABLED_FILE);
		return;
	}
	
	fp = fopen (DISABLED_FILE, "w");
	
	if (!fp)
	{
		bug ("Could not open " DISABLED_FILE " for writing",0);
		return;
	}
	
	for (p = disabled_first; p ; p = p->next)
		fprintf (fp, "%s %d %s\n", p->command->name, p->level, p->disabled_by);
		
	fprintf (fp, "%s\n",END_MARKER);
		
	fclose (fp);
}


