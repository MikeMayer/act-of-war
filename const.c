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
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"

/* This is where all gsn declarations should go.  */

sh_int gsn_circle;
sh_int gsn_backstab;
sh_int gsn_dodge;
sh_int gsn_envenom;
sh_int gsn_hide;
sh_int gsn_peek;
sh_int gsn_pick_lock;
sh_int gsn_sneak;
sh_int gsn_steal;
sh_int gsn_hunt;
sh_int gsn_disarm;
sh_int gsn_enhanced_damage;
sh_int gsn_brutal_damage;
sh_int gsn_blind_fighting;
sh_int gsn_kick;
sh_int gsn_life_song;
sh_int gsn_magic_song;
sh_int gsn_speed_song;
sh_int gsn_attack_song;
sh_int gsn_defense_song;
sh_int gsn_sung_defense;
sh_int gsn_learning_song;
sh_int gsn_geomancy;
sh_int gsn_earthlore;
sh_int gsn_lava_shield;
sh_int gsn_frost_shield;
sh_int gsn_vine_counter;
sh_int gsn_stone_protect;
sh_int gsn_parry;
sh_int gsn_fencing;
sh_int gsn_rescue;
sh_int gsn_second_attack;
sh_int gsn_third_attack;
sh_int gsn_fourth_attack;
sh_int gsn_fifth_attack;
sh_int gsn_dual_wield;
sh_int gsn_stun;
sh_int gsn_detect_trap;
sh_int gsn_trap;
sh_int gsn_blindness;
sh_int gsn_charm_person;
sh_int gsn_curse;
sh_int gsn_invis;
sh_int gsn_mass_invis;
sh_int gsn_poison;
sh_int gsn_plague;
sh_int gsn_flaming_wall;
sh_int gsn_sleep;
sh_int gsn_sanctuary;
sh_int gsn_reflect;
sh_int gsn_fly;

/* new gsns */
sh_int gsn_axe;
sh_int gsn_dagger;
sh_int gsn_flail;
sh_int gsn_mace;
sh_int gsn_polearm;
sh_int gsn_shield_block;
sh_int gsn_spear;
sh_int gsn_sword;
sh_int gsn_whip;
sh_int gsn_bash;
sh_int gsn_berserk;
sh_int gsn_dirt;
sh_int gsn_hand_to_hand;
sh_int gsn_trip;
sh_int gsn_charge;
sh_int gsn_fast_healing;
sh_int gsn_nature_walk;
sh_int gsn_haggle;
sh_int gsn_lore;
sh_int gsn_focus;
sh_int gsn_disrupt;
sh_int gsn_scout;
sh_int gsn_strike;
sh_int gsn_bluff;
sh_int gsn_struggle;
sh_int gsn_mend;
sh_int gsn_chase;
sh_int gsn_push;
sh_int gsn_drag;
sh_int gsn_meditation;
sh_int gsn_ki_power;
sh_int gsn_scrolls;
sh_int gsn_staves;
sh_int gsn_wands;
sh_int gsn_recall;
sh_int gsn_riding;
sh_int gsn_dragonsword;
sh_int gsn_step;
sh_int gsn_ground;
sh_int gsn_rush;
sh_int gsn_shieldbash;
sh_int gsn_skewer;
sh_int gsn_weapon_break;
sh_int gsn_tear;
sh_int gsn_throw;
sh_int gsn_decay;
sh_int gsn_chaotic;
sh_int gsn_deflect;
sh_int gsn_jump;
sh_int  gsn_braver;
sh_int  gsn_cross_slash;
sh_int  gsn_blade_beam;
sh_int  gsn_climhazzard;
sh_int  gsn_meteorain;
sh_int  gsn_finishing_touch;
sh_int  gsn_omnislash;

/* item type list */
const struct item_type item_table[] =
{
    {ITEM_LIGHT, "light"},
    {ITEM_DARK, "dark"},
    {ITEM_SCROLL, "scroll"},
    {ITEM_WAND, "wand"},
    {ITEM_STAFF, "staff"},
    {ITEM_WEAPON, "weapon"},
    {ITEM_TREASURE, "treasure"},
    {ITEM_ARMOR, "armor"},
    {ITEM_POTION, "potion"},
    {ITEM_CLOTHING, "clothing"},
    {ITEM_FURNITURE, "furniture"},
    {ITEM_TRASH, "trash"},
    {ITEM_CONTAINER, "container"},
    {ITEM_DRINK_CON, "drink"},
    {ITEM_KEY, "key"},
    {ITEM_FOOD, "food"},
    {ITEM_MONEY, "money"},
    {ITEM_BOAT, "boat"},
    {ITEM_CORPSE_NPC, "npc_corpse"},
    {ITEM_CORPSE_PC, "pc_corpse"},
    {ITEM_FOUNTAIN, "fountain"},
    {ITEM_PILL, "pill"},
    {ITEM_PROTECT, "protect"},
    {ITEM_MAP, "map"},
    {ITEM_PORTAL, "portal"},
    {ITEM_WARP_STONE, "warp_stone"},
    {ITEM_ROOM_KEY, "room_key"},
    {ITEM_GEM, "gem"},
    {ITEM_JEWELRY, "jewelry"},
    {ITEM_JUKEBOX, "jukebox"},
    {ITEM_CASTLE_SPECIAL, "castle_special"},
    {ITEM_FLAG, "flag"},
{ITEM_INGREDIENT, "ingredient"},
{ITEM_PARCHMENT, "parchment"},
{ITEM_SHIELD, "shield"},
    {0, NULL}
};

/* weapon selection table */
const struct weapon_type weapon_table[] =
{
    {"sword", OBJ_VNUM_SCHOOL_SWORD, WEAPON_SWORD, &gsn_sword},
    {"mace", OBJ_VNUM_SCHOOL_MACE, WEAPON_MACE, &gsn_mace},
    {"dagger", OBJ_VNUM_SCHOOL_DAGGER, WEAPON_DAGGER, &gsn_dagger},
    {"axe", OBJ_VNUM_SCHOOL_AXE, WEAPON_AXE, &gsn_axe},
    {"staff", OBJ_VNUM_SCHOOL_STAFF, WEAPON_SPEAR, &gsn_spear},
    {"flail", OBJ_VNUM_SCHOOL_FLAIL, WEAPON_FLAIL, &gsn_flail},
    {"whip", OBJ_VNUM_SCHOOL_WHIP, WEAPON_WHIP, &gsn_whip},
    {"polearm", OBJ_VNUM_SCHOOL_POLEARM, WEAPON_POLEARM, &gsn_polearm},
    {NULL, 0, 0, NULL}
};

/* wiznet table and prototype for future flag setting */
const struct wiznet_type wiznet_table[] =
{
    {"on", WIZ_ON, IM},
    {"prefix", WIZ_PREFIX, IM},
    {"ticks", WIZ_TICKS, IM},
    {"logins", WIZ_LOGINS, IM},
    {"sites", WIZ_SITES, L4},
    {"links", WIZ_LINKS, L7},
    {"newbies", WIZ_NEWBIE, IM},
    {"spam", WIZ_SPAM, L5},
    {"deaths", WIZ_DEATHS, IM},
    {"resets", WIZ_RESETS, L4},
    {"mobdeaths", WIZ_MOBDEATHS, L4},
    {"flags", WIZ_FLAGS, L5},
    {"penalties", WIZ_PENALTIES, L5},
    {"saccing", WIZ_SACCING, L5},
    {"levels", WIZ_LEVELS, IM},
    {"load", WIZ_LOAD, L2},
    {"restore", WIZ_RESTORE, L2},
    {"snoops", WIZ_SNOOPS, L2},
    {"switches", WIZ_SWITCHES, L2},
    {"secure", WIZ_SECURE, L1},
    {"memcheck", WIZ_MEMCHECK, L1},
    {NULL, 0, 0}
};

/* attack table  -- not very organized :( */
const struct attack_type attack_table[MAX_DAMAGE_MESSAGE] =
{
    {"none", "hit", -1},	/*  0 */
    {"slice", "slice", DAM_SLASH},
    {"stab", "stab", DAM_PIERCE},
    {"slash", "slash", DAM_SLASH},
    {"whip", "whip", DAM_SLASH},
    {"claw", "claw", DAM_SLASH},	/*  5 */
    {"blast", "blast", DAM_BASH},
    {"pound", "pound", DAM_BASH},
    {"crush", "crush", DAM_BASH},
    {"grep", "grep", DAM_SLASH},
    {"bite", "bite", DAM_PIERCE},	/* 10 */
    {"pierce", "pierce", DAM_PIERCE},
    {"suction", "suction", DAM_BASH},
    {"beating", "beating", DAM_BASH},
    {"digestion", "digestion", DAM_ACID},
    {"charge", "charge", DAM_BASH},	/* 15 */
    {"slap", "slap", DAM_BASH},
    {"punch", "punch", DAM_BASH},
    {"wrath", "wrath", DAM_ENERGY},
    {"magic", "magic", DAM_ENERGY},
    {"divine", "divine power", DAM_HOLY},	/* 20 */
    {"cleave", "cleave", DAM_SLASH},
    {"scratch", "scratch", DAM_PIERCE},
    {"peck", "peck", DAM_PIERCE},
    {"peckb", "peck", DAM_BASH},
    {"chop", "chop", DAM_SLASH},	/* 25 */
    {"sting", "sting", DAM_PIERCE},
    {"smash", "smash", DAM_BASH},
    {"shbite", "shocking bite", DAM_LIGHTNING},
    {"flbite", "flaming bite", DAM_FIRE},
    {"frbite", "freezing bite", DAM_COLD},	/* 30 */
    {"acbite", "acidic bite", DAM_ACID},
    {"chomp", "chomp", DAM_PIERCE},
    {"drain", "life drain", DAM_NEGATIVE},
    {"thrust", "thrust", DAM_PIERCE},
    {"slime", "slime", DAM_ACID},
    {"shock", "shock", DAM_LIGHTNING},
    {"thwack", "thwack", DAM_BASH},
    {"flame", "flame", DAM_FIRE},
    {"chill", "chill", DAM_COLD},
    {NULL, NULL, 0}
};

/* race table */
const struct race_type race_table[] =
{
/*
   {
   name,                pc_race?,
   act bits,    aff_by bits,    off bits,
   imm,         res,            vuln,
   form,                parts 
   },
 */
    {"unique", FALSE, 0, 0, 0, 0, 0, 0, 0, 0},

    {
	"cerberus", TRUE,
	0, AFF_FLYING, 0,
	IMM_DISEASE, RES_FIRE, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"mhrynn", TRUE,
	0, AFF_FLYING, 0,
	IMM_DISEASE, RES_FIRE, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },

    {
	"demon", TRUE,
	0, 0, 0,
	0,RES_LIGHTNING, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"greyman", TRUE,
	0, AFF_PASS_DOOR, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"shadow", TRUE,
	0, AFF_PASS_DOOR, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"storm giant", TRUE,
	0, 0, 0,
	0,RES_LIGHTNING, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"aiel", TRUE,
	0, 0, 0,
	0, RES_POISON | RES_DISEASE, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"sith", TRUE,
	0, 0, 0,
	0, RES_POISON | RES_DISEASE, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
     {
	"viera", TRUE,
	0, 0, 0,
	0, RES_PIERCE, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"other", TRUE,
	0, 0, 0,
	0, RES_PIERCE, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    
         {
	"tauren", TRUE,
	0, 0, 0,
	0, RES_ACID, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"bristleback", TRUE,
	0, 0, 0,
	0, RES_ACID, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    
    {
	"high elf", TRUE,
	0, 0, 0,
	IMM_CHARM, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"vampire", TRUE,
	0, AFF_FLYING, 0,
	IMM_POISON | IMM_DISEASE, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"aratar", TRUE,
	0, 0, 0,
	IMM_DISEASE | IMM_POISON, RES_HOLY, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"archon", TRUE,
	0, 0, 0,
	0, RES_COLD, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"devil", TRUE,
	0, 0, 0,
	0, RES_COLD, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"succubi", TRUE,
	0, 0, 0,
	IMM_DISEASE | IMM_POISON, RES_HOLY, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
       {
	"lich", TRUE,
	0, 0, 0,
	IMM_CHARM, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"phoenix", TRUE,
	0, AFF_FLYING, 0,
	IMM_POISON | IMM_DISEASE, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"ogre", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"troll", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"dwarf", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"ghoul", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"hill giant", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"nym", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"hobbit", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },

    {
	"human", TRUE,
	0, AFF_INFRARED, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },

    {
	"kobold", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },

    {
	"rakasta", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },

    {
	"skaven", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },

    {
	"vampiel", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },

    {
	"githyanki", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },

    {
	"harpy", TRUE,
	0, 0, 0,
	0, RES_ENERGY, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },

    {
	"satyr", TRUE,
	0, 0, 0,
	0, RES_ENERGY, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"banshee", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"centaur", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"drow", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"dryad", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },

    {
	"gnome", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"elf", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"spector", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"draghkar", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"faerie", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"gaeb-dur", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"kaelan", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"wood", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"mountain", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"plains", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"kender", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"plains centaur", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"thri-kreen", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"saurial", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"gargoyle", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"dryad", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"rakasta", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"lyscian", TRUE,
	0, 0, 0,
	0, 0, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"orcpc", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"goblinpc", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"skaven", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"deugar", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"githyanki", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"svirfnebli", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"illithid", TRUE,
	0, 0, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },
    {
	"nether sprite", TRUE,
	0, AFF_FLYING, 0,
	0, 0, VULN_SUN,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },

    {
	"bat", FALSE,
	0, AFF_FLYING | AFF_DARK_VISION, OFF_DODGE | OFF_FAST,
	0, 0, VULN_LIGHT,
	A | G | V, A | C | D | E | F | H | J | K | P
    },

    {
	"bear", FALSE,
	0, 0, OFF_CRUSH | OFF_DISARM | OFF_BERSERK,
	0, RES_BASH | RES_COLD, 0,
	A | G | V, A | B | C | D | E | F | H | J | K | U | V
    },
		/* 13 at cat */
    {
	"cat", FALSE,
	0, AFF_DARK_VISION, OFF_FAST | OFF_DODGE,
	0, 0, 0,
	A | G | V, A | C | D | E | F | H | J | K | Q | U | V
    },

    {
	"centipede", FALSE,
	0, AFF_DARK_VISION, 0,
	0, RES_PIERCE | RES_COLD, VULN_BASH,
	A | B | G | O, A | C | K
    },

    {
	"dog", FALSE,
	0, 0, OFF_FAST,
	0, 0, 0,
	A | G | V, A | C | D | E | F | H | J | K | U | V
    },

    {
	"doll", FALSE,
	0, 0, 0,
	IMM_COLD | IMM_POISON | IMM_HOLY | IMM_NEGATIVE | IMM_MENTAL | IMM_DISEASE
	| IMM_DROWNING, RES_BASH | RES_LIGHT,
	VULN_SLASH | VULN_FIRE | VULN_ACID | VULN_LIGHTNING | VULN_ENERGY,
	E | J | M | cc, A | B | C | G | H | K
    },

    {"dragon", FALSE,
     0, AFF_INFRARED | AFF_FLYING, 0,
     0, RES_FIRE | RES_BASH | RES_CHARM,
     VULN_PIERCE | VULN_COLD,
     A | H | Z, A | C | D | E | F | G | H | I | J | K | P | Q | U | V | X
    },

    {
	"fido", FALSE,
	0, 0, OFF_DODGE | ASSIST_RACE,
	0, 0, VULN_MAGIC,
	A | B | G | V, A | C | D | E | F | H | J | K | Q | V
    },

    {
	"fox", FALSE,
	0, AFF_DARK_VISION, OFF_FAST | OFF_DODGE,
	0, 0, 0,
	A | G | V, A | C | D | E | F | H | J | K | Q | V
    },

    {
	"goblin", FALSE,
	0, AFF_INFRARED, 0,
	0, RES_DISEASE, VULN_MAGIC,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },

    {
	"hobgoblin", FALSE,
	0, AFF_INFRARED, 0,
	0, RES_DISEASE | RES_POISON, 0,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K | Y
    },

    {
	"kobold", FALSE,
	0, AFF_INFRARED, 0,
	0, RES_POISON, VULN_MAGIC,
	A | B | H | M | V, A | B | C | D | E | F | G | H | I | J | K | Q
    },

    {
	"lizard", FALSE,
	0, 0, 0,
	0, RES_POISON, VULN_COLD,
	A | G | X | cc, A | C | D | E | F | H | K | Q | V
    },

    {
	"modron", FALSE,
	0, AFF_INFRARED, ASSIST_RACE | ASSIST_ALIGN,
	IMM_CHARM | IMM_DISEASE | IMM_MENTAL | IMM_HOLY | IMM_NEGATIVE,
	RES_FIRE | RES_COLD | RES_ACID, 0,
	H, A | B | C | G | H | J | K
    },

    {
	"orc", FALSE,
	0, AFF_INFRARED, 0,
	0, RES_DISEASE, VULN_LIGHT,
	A | H | M | V, A | B | C | D | E | F | G | H | I | J | K
    },

    {
	"pig", FALSE,
	0, 0, 0,
	0, 0, 0,
	A | G | V, A | C | D | E | F | H | J | K
    },

    {
	"rabbit", FALSE,
	0, 0, OFF_DODGE | OFF_FAST,
	0, 0, 0,
	A | G | V, A | C | D | E | F | H | J | K
    },

    {
	"school monster", FALSE,
	ACT_NOALIGN, 0, 0,
	IMM_CHARM | IMM_SUMMON, 0, VULN_MAGIC,
	A | M | V, A | B | C | D | E | F | H | J | K | Q | U
    },

    {
	"snake", FALSE,
	0, 0, 0,
	0, RES_POISON, VULN_COLD,
	A | G | X | Y | cc, A | D | E | F | K | L | Q | V | X
    },

    {
	"song bird", FALSE,
	0, AFF_FLYING, OFF_FAST | OFF_DODGE,
	0, 0, 0,
	A | G | W, A | C | D | E | F | H | K | P
    },

    {
	"troll", FALSE,
	0, AFF_REGENERATION | AFF_INFRARED | AFF_DETECT_HIDDEN,
	OFF_BERSERK,
	0, RES_CHARM | RES_BASH, VULN_FIRE | VULN_ACID,
	A | B | H | M | V, A | B | C | D | E | F | G | H | I | J | K | U | V
    },

    {
	"water fowl", FALSE,
	0, AFF_SWIM | AFF_FLYING, 0,
	0, RES_DROWNING, 0,
	A | G | W, A | C | D | E | F | H | K | P
    },

    {
	"wolf", FALSE,
	0, AFF_DARK_VISION, OFF_FAST | OFF_DODGE,
	0, 0, 0,
	A | G | V, A | C | D | E | F | J | K | Q | V
    },

    {
	"wyvern", FALSE,
	0, AFF_FLYING | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
	OFF_BASH | OFF_FAST | OFF_DODGE,
	IMM_POISON, 0, VULN_LIGHT,
	A | B | G | Z, A | C | D | E | F | H | J | K | Q | V | X
    },

    {
	"unique", FALSE,
	0, 0, 0,
	0, 0, 0,
	0, 0
    },
    {
	NULL, 0, 0, 0, 0, 0, 0
    }
};

const struct pc_race_type pc_race_table[] =
{
    {"null race", "", 0,
     {100, 100, 100, 100},
     {""},
     {13, 13, 13, 13, 13},
     {18, 18, 18, 18, 18}, 0,
     "Null Race"},

/*
   {
   "race name",         short name,     points, { class multipliers },
   { bonus skills },
   { base stats },              { max stats },          size,
   "race hidden name" , available sides, isremort?, "creation screen name"
   },
 */
//begin races

    {"cerberus", "Crb", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"chase", "hunt"},
     {13, 13, 13, 13, 13},
     {24, 22, 22, 24, 25},
     SIZE_MEDIUM, "cerberus", CANBE_UM + CANBE_CAIRN, TRUE, ""},

    {"mhrynn", "Mhr", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"hunt", "chase"},
     {13, 13, 13, 13, 13},
     {24, 22, 22, 24, 25},
     SIZE_MEDIUM, "mhrynn", CANBE_TIER + CANBE_KAEL, TRUE, ""},

    {"demon", "Dem", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"parry", "dual wield"},
     {13, 13, 13, 13, 13},
     {25, 21, 23, 23, 24},
     SIZE_MEDIUM, "demon", CANBE_UM + CANBE_CAIRN, TRUE, ""},

    {"greyman", "Gry", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"sneak", "hide"},
     {13, 13, 13, 13, 13},
     {23, 23, 21, 25, 24},
     SIZE_MEDIUM, "greyman", CANBE_TIER + CANBE_KAEL, TRUE, ""},

    {"shadow", "Sha", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"sneak", "hide"},
     {13, 13, 13, 13, 13},
     {23, 23, 21, 25, 24},
     SIZE_MEDIUM, "shadow", CANBE_UM + CANBE_CAIRN, TRUE, ""},

    {"storm giant", "StG", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"parry", "dual wield"},
     {13, 13, 13, 13, 13},
     {25, 21, 23, 23, 24},
     SIZE_LARGE, "storm giant", CANBE_TIER + CANBE_KAEL, TRUE, ""},

    {"aiel", "Aie", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"parry", "dual wield"},
     {13, 13, 13, 13, 13},
     {24, 22, 24, 22, 23},
     SIZE_MEDIUM, "fierce aiel", CANBE_TIER + CANBE_KAEL, TRUE, ""},

    {"sith", "Sit", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"parry", "dual wield"},
     {13, 13, 13, 13, 13},
     {24, 22, 24, 22, 23},
     SIZE_MEDIUM, "dark sith", CANBE_UM + CANBE_CAIRN, TRUE, ""},
     
         {"viera", "Vie", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"fifth attack"},
     {13, 13, 13, 13, 13},
     {22, 24, 23, 24, 22},
     SIZE_MEDIUM, "delicate viera", CANBE_TIER + CANBE_KAEL, TRUE, ""},
     
         {"other", "Oth", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"fifth attack"},
     {13, 13, 13, 13, 13},
     {22, 24, 23, 24, 22},
     SIZE_MEDIUM, "shadowy other", CANBE_UM + CANBE_CAIRN, TRUE, ""},
     
     {"tauren", "Tau", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"forcing"},
     {13, 13, 13, 13, 13},
     {24, 24, 23, 22, 23},
     SIZE_MEDIUM, "destructive tauren", CANBE_TIER + CANBE_KAEL, TRUE, ""},
     
         {"bristleback", "Brs", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"forcing"},
     {13, 13, 13, 13, 13},
     {24, 24, 23, 22, 23},
     SIZE_MEDIUM, "spiny bristleback", CANBE_UM + CANBE_CAIRN, TRUE, ""},
     

    {"high elf", "HgE", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"eagle eye", "illusions"},
     {13, 13, 13, 13, 13},
     {22, 24, 22, 24, 23},
     SIZE_MEDIUM, "high elf", CANBE_TIER + CANBE_KAEL, TRUE, ""},

    {"vampire", "Vam", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"death coil", "soul coil"},
     {13, 13, 13, 13, 13},
     {22, 24, 24, 22, 21},
     SIZE_MEDIUM, "vampire", CANBE_UM + CANBE_CAIRN, TRUE, ""},

    {"aratar", "Ara", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"heal"},
     {13, 13, 13, 13, 13},
     {23, 23, 25, 23, 23},
     SIZE_MEDIUM, "aratar", CANBE_TIER + CANBE_KAEL, TRUE, ""},

    {"archon", "Arc", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"staves", "wands", "scrolls"},
     {13, 13, 13, 13, 13},
     {21, 25, 23, 23, 22},
     SIZE_LARGE, "archon", CANBE_TIER + CANBE_KAEL, TRUE, ""},

    {"devil", "Dvl", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"staves", "wands", "scrolls"},
     {13, 13, 13, 13, 13},
     {21, 25, 23, 23, 22},
     SIZE_MEDIUM, "devil", CANBE_UM + CANBE_CAIRN, TRUE, ""},

    {"succubi", "Suc", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"heal"},
     {13, 13, 13, 13, 13},
     {23, 23, 25, 23, 23},
     SIZE_MEDIUM, "succubi", CANBE_UM + CANBE_CAIRN, TRUE, ""},

    {"lich", "Lic", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"eagle eye", "illusions"},
     {13, 13, 13, 13, 13},
     {22, 24, 22, 24, 23},
     SIZE_MEDIUM, "lich", CANBE_UM + CANBE_CAIRN, TRUE, ""},

    {"phoenix", "Pho", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"death coil", "soul coil"},
     {13, 13, 13, 13, 13},
     {22, 24, 24, 22, 21},
     SIZE_MEDIUM, "phoenix", CANBE_TIER + CANBE_KAEL, TRUE, ""},

    {"ogre", "Ogr", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {16, 10, 11, 12, 16},
     {21, 15, 14, 17, 20},
     SIZE_MEDIUM, "large ogre", CANBE_KAEL + CANBE_TIER + CANBE_CAIRN, TRUE, "Burly creatures with much strength."},

    {"troll", "Tro", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {16, 11, 11, 13, 14},
     {21, 15, 14, 17, 20},
     SIZE_MEDIUM, "wretched troll", CANBE_UM, TRUE, "Scaley tough creature of the night."},

    {"dwarf", "Dwa", 5,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {14, 12, 14, 10, 15},
     {18, 16, 19, 16, 19},
     SIZE_MEDIUM, "stalwart dwarf", CANBE_KAEL + CANBE_TIER + CANBE_CAIRN, TRUE, "Wise stalwart warriors."},

    {"ghoul", "Gho", 6,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {14, 12, 14, 10, 15},
     {18, 16, 19, 16, 19},
     SIZE_MEDIUM, "sinister ghoul", CANBE_UM, TRUE, "Evil spiritual fiends."},

    {"hill giant", "Hgi", 4,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {18, 9, 9, 13, 16},
     {22, 11, 12, 16, 19},
     SIZE_LARGE, "ugly hill giant", CANBE_UM + CANBE_CAIRN, FALSE, "Fierce giants."},

    {"nym", "Nym", 4,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {18, 9, 9, 13, 16},
     {22, 11, 12, 16, 19},
     SIZE_LARGE, "hardy nym", CANBE_KAEL + CANBE_TIER, FALSE, "Noble giants."},

    {"hobbit", "Hob", 4,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {9, 14, 13, 17, 13},
     {11, 16, 15, 22, 18},
     SIZE_SMALL, "simple hobbit", CANBE_KAEL + CANBE_TIER + CANBE_CAIRN, FALSE, "Short stubby kin furry feet. good stealth."},

    {"human", "Hum", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"alchemy", "scribery"},
     {13, 13, 13, 13, 13},
     {23, 25, 25, 23, 23},
     SIZE_MEDIUM, "stout human", CANBE_KAEL + CANBE_TIER + CANBE_CAIRN + CANBE_UM, TRUE, "Well rounded race."},

    {"kobold", "Kob", 4,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {10, 13, 13, 16, 13},
     {19, 16, 15, 20, 18},
     SIZE_SMALL, "fierce kobold", CANBE_UM, TRUE, "Dog men."},

    {"rakasta", "Rak", 6,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {10, 13, 13, 16, 13},
     {19, 16, 15, 20, 18},
     SIZE_MEDIUM, "fur covered rakasta", CANBE_KAEL + CANBE_TIER + CANBE_CAIRN, TRUE, "Violent cat like beast."},

    {"skaven", "Ska", 6,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {9, 14, 13, 17, 13},
     {11, 16, 15, 22, 18},
     SIZE_MEDIUM, "filthy skaven", CANBE_UM+CANBE_CAIRN, FALSE, "Goblin kin but smaller and stealthy."},

    {"vampiel", "Vpl", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {13, 13, 13, 13, 13},
     {18, 18, 18, 18, 18},
     SIZE_MEDIUM, "foul vampiel", CANBE_UM + CANBE_CAIRN, TRUE, "Half vampire half humans."},

    {"githyanki", "Git", 6,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {15, 11, 11, 14, 14},
     {19, 19, 16, 18, 17},
     SIZE_MEDIUM, "evil githyanki", CANBE_UM + CANBE_CAIRN, TRUE, "Skeletal warriors of ancient magic."},

    {"harpy", "Hpy", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"strengthen ground"},
     {13, 13, 13, 13, 13},
     {23, 23, 24, 23, 24},
     SIZE_MEDIUM, "harpy", CANBE_UM + CANBE_CAIRN, TRUE, "Half bird, half man."},

    {"satyr", "Sat", 0,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {"strengthen ground"},
     {14, 13, 13, 13, 12},
     {23, 23, 24, 23, 24},
     SIZE_MEDIUM, "reclusive satyr", CANBE_KAEL + CANBE_TIER, TRUE, "Half goat half humanoid."},

    {"banshee", "Ban", 5,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {12, 14, 13, 15, 11},
     {16, 18, 20, 16, 16},
     SIZE_MEDIUM, "wailing banshee", CANBE_UM + CANBE_CAIRN, TRUE, "Strong spirits."},

    {"centaur", "Cen", 6,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {15, 11, 11, 14, 14},
     {19, 19, 17, 17, 16},
     SIZE_MEDIUM, "noble centaur", CANBE_KAEL + CANBE_TIER, TRUE, "Warriors with magical inclinations."},

    {"drow", "Dro", 2,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {12, 14, 12, 15, 12},
     {17, 19, 17, 19, 16},
     SIZE_MEDIUM, "evil drow", CANBE_UM + CANBE_CAIRN, TRUE, "Violent brethren to elves corrupted."},

    {"dryad", "Dry", 6,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {9, 14, 13, 16, 13},
     {16, 18, 20, 16, 16},
     SIZE_SMALL, "curious dryad", CANBE_KAEL + CANBE_TIER, TRUE, "Simple magical creatures of forest."},

    {"gnome", "Gno", 4,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {12, 14, 12, 15, 12},
     {17, 19, 17, 19, 16},
     SIZE_SMALL, "little gnome", CANBE_KAEL + CANBE_TIER, TRUE, "Short stealthy beings with aptitudes for magic."},

    {"elf", "Elf", 5,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {12, 14, 14, 11, 18},
     {16, 18, 17, 13, 22},
     SIZE_MEDIUM, "nimble elf", CANBE_KAEL + CANBE_TIER, FALSE, "Simple intelligent elves dedicated to con."},

    {"spector", "Spe", 6,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {12, 14, 14, 11, 18},
     {16, 18, 17, 13, 22},
     SIZE_MEDIUM, "glittering spector", CANBE_UM + CANBE_CAIRN, FALSE, "Foul constitutional spirits."},

    {"draghkar", "Drg", 4,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {5, 17, 17, 15, 11},
     {8, 21, 21, 19, 13},
     SIZE_MEDIUM, "mysterious draghkar", CANBE_UM + CANBE_CAIRN, FALSE, "Dark creatures with incredible magic ability."},

    {"faerie", "Fae", 6,
     {
/* Mag */ 100,
/* Dru */ 100,
/* She */ 100,
/* Cle */ 100,
/* Def */ 100,
/* Nec */ 100,
/* Dea */ 100,
/* Kni */ 100,
/* War */ 100,
/* Wit */ 100,
/* Bar */ 100,
/* Ran */ 100,
/* Ass */ 100,
/* Mon */ 100,
/* Brd */ 100,
/* Ill */ 100,
/* Tmp */ 100,
/* Lan */ 100,
/* Zea */ 100,
/* Geo */ 100,
/* Dra */ 100},
     {""},
     {5, 17, 17, 15, 11},
     {8, 21, 21, 19, 13},
     SIZE_SMALL, "flittering faerie", CANBE_KAEL + CANBE_TIER, FALSE, "Tiny beings with incredible magic ability."},

//end races
    {NULL, "", 0,
     {100, 100, 100, 100},
     {""},
     {13, 13, 13, 13, 13},
     {18, 18, 18, 18, 18}, 0,
     "Null Race"}
};

const struct kingdom_type kingdom_table[MAX_KINGDOM + 1] =
{
	/* { name , start_room_num, die_room_num, recall_room_num, miscfield used by saltrecall } */
    {"None", 200, 200, 61606, 0, "{DNone{x"},	
    {"Tier", 15861, 15811, 15965, 203, "{cTier{x"},
    {"Kael", 20342, 20200, 20342, 200, "{mKael{x"},
    {"Undermountain", 20423, 20400, 20499, 206, "{rUM{x"},
    {"Cairn", 19132, 28447, 28197, 202, "{gCairn{x"}
};




// - Example blank class
// {
// 	"/* the full name of the class 		*/", "/* Three-letter name for 'who'		*/", 
// 	/* Prime attribute			*/, /* First weapon				*/,
// 	{/* Vnum of guild rooms			*/}, /* Maximum skill level			*/,/* Thac0 for level  0			*/, 
// 	/* Thac0 for level 32			*/, /* Min hp gained on leveling		*/, /* Max hp gained on leveling		*/,
// 	/* Min mana gained on leveling		*/, /* Max mana gained on leveling		*/, /* Class gains mana on level T/F*/,
// 		"/* base skills gained			*/", "/* default skills gained		*/", /* max moves base on class      	*/,
// 		/* defines what sides may use the class	*/, "/* a short desc of what the class is    */"
//     },

/*
 * Class table.
 */
const struct class_type class_table[MAX_CLASS] =
{
    {
	"Mage", "Mag", STAT_INT, OBJ_VNUM_SCHOOL_DAGGER,
	{3018, 9618}, 85, 20, 6, 4, 7, 20, 23, TRUE,
	"mage basics", "mage default", 260, CANBE_ALL, "Powerful spell caster uses offensive spells."
    },

    {
	"Druid", "Dru", STAT_WIS, OBJ_VNUM_SCHOOL_MACE,
	{3003, 9619}, 85, 20, 2, 6, 9, 8, 11, TRUE,
	"cleric basics", "druid default", 300, CANBE_ALL, "Clerics who have developed skills of the woods."

    },

    {
	"Warlock", "Wlk", STAT_STR, OBJ_VNUM_SCHOOL_SWORD,
	{3022, 9633}, 85, 20, -10, 11, 14, 3, 6, FALSE,
	"warrior basics", "warlock default", 400, CANBE_ALL, "A relic of the past."

    },

    {
	"Cleric", "Cle", STAT_WIS, OBJ_VNUM_SCHOOL_MACE,
	{3003, 9619}, 85, 20, 2, 7, 10, 7, 10, TRUE,
	"cleric basics", "cleric default", 350, CANBE_ALL, "Healers and priests of the lands."

    },

    {
	"Soldier", "Sld", STAT_STR, OBJ_VNUM_SCHOOL_SWORD,
	{3003, 9619}, 85, 20, -10, 19, 25, 1, 1, TRUE,
	"warrior basics", "soldier default", 900, CANBE_NONE, "Sword skill based fighters."

    },
    {
	"Necromancer", "Nec", STAT_WIS, OBJ_VNUM_SCHOOL_DAGGER,
	{3018, 9618}, 85, 20, 2, 7, 10, 7, 10, TRUE,
	"cleric basics", "necromancer default", 350, CANBE_ALL, "Ancient masters of the dead."

    },
    {
	"Deathknight", "Dea", STAT_STR, OBJ_VNUM_SCHOOL_SWORD,
	{3022, 9633}, 85, 20, -10, 11, 14, 3, 6, TRUE,
	"warrior basics", "deathknight default", 360, CANBE_ALL, "Knights sworn to chaos."

    },
    {
	"Knight", "Kni", STAT_STR, OBJ_VNUM_SCHOOL_SWORD,
	{3022, 9633}, 85, 20, -10, 11, 14, 3, 6, FALSE,
	"warrior basics", "knight default", 360, CANBE_ALL, "Warriors which have achieved a noble order."

    },

    {
	"Warrior", "War", STAT_CON, OBJ_VNUM_SCHOOL_SWORD,
	{3022, 9633}, 85, 20, -10, 26, 29, 1, 4, FALSE,
	"warrior basics", "warrior default", 500, CANBE_ALL, "Strongest of the fighter classes."

    },
    {
	"Witch", "Wit", STAT_WIS, OBJ_VNUM_SCHOOL_DAGGER,
	{3018, 9618}, 85, 20, 0, 5, 8, 9, 12, TRUE,
	"cleric basics", "witch default", 350, CANBE_ALL, "Enchantresses with strong powers."

    },
    {
	"Barbarian", "Bar", STAT_STR, OBJ_VNUM_SCHOOL_SWORD,
	{3022, 9633}, 85, 20, -15, 17, 20, -3, 0, FALSE,
	"warrior basics", "barbarian default", 500, CANBE_ALL, "Poweful warriors who have sworn off magic."

    },
    {
	"Ranger", "Ran", STAT_STR, OBJ_VNUM_SCHOOL_SWORD,
	{3022, 9633}, 85, 20, -10, 11, 14, 3, 6, FALSE,
	"warrior basics", "ranger default", 500, CANBE_ALL, "Combatants of the woods use stealth and cunning."

    },
    {
	"Assassin", "Ass", STAT_DEX, OBJ_VNUM_SCHOOL_DAGGER,
	{3028, 9639}, 85, 20, -4, 11, 14, 3, 6, FALSE,
	"assassin basics", "assassin default", 450, CANBE_ALL, "Dangerous character with stealthy skills."

    },
    {
	"Monk", "Mnk", STAT_DEX, OBJ_VNUM_SCHOOL_DAGGER,
	{3028, 9639}, 85, 20, -4, 9, 12, 5, 8, FALSE,
	"monk basics", "monk default", 350, CANBE_ALL, "Martial artists with superior fighting skills."

    },
    {
	"Bard", "Brd", STAT_DEX, OBJ_VNUM_SCHOOL_DAGGER,
	{3028, 9639}, 85, 20, -4, 10, 13, 4, 7, FALSE,
	"assassin basics", "bard default", 350, CANBE_ALL, "Travelling minstrel with many skills."

    },
    {
	"Illusionist", "Ill", STAT_INT, OBJ_VNUM_SCHOOL_DAGGER,
	{3018, 9618}, 85, 20, 6, 6, 9, 8, 11, TRUE,
	"mage basics", "illusionist default", 300, CANBE_ALL, "Why fight the enemy when you can control them?"

    },
    {
	"Temporalist", "Tmp", STAT_INT, OBJ_VNUM_SCHOOL_DAGGER,
	{3018, 9618}, 85, 20, 6, 6, 9, 8, 11, TRUE,
	"mage basics", "time default", 300, CANBE_ALL, "Masters of manipulating time and space."

    },
    {
	"Lancer", "Lan", STAT_STR, OBJ_VNUM_SCHOOL_DAGGER,
	{3028, 9639}, 85, 20, -4, 9, 12, 5, 8, FALSE,
	"warrior basics", "lancer default", 350, CANBE_ALL, "Specialist warriors of the polearm and spear."
	
    },
    {
	"Zealot", "Zea", STAT_CON, OBJ_VNUM_SCHOOL_DAGGER,
	{3018, 9618}, 85, 20, -10, 13, 16, 1, 4, FALSE,
	"zealot basics", "zealot default", 260, CANBE_ALL, "Very masterful melee fighters.\n"
	
    },
    {
	"Geomancer", "Geo", STAT_STR, OBJ_VNUM_SCHOOL_AXE,
	{3018, 9618}, 85, 20, 6, 8, 12, 10, 13, TRUE,
	"geomancer basics", "geomancer default", 460, CANBE_ALL, "Very strong, untrained fighters with skill over elements."
	
    },
    {
	"Dragoon", "Dra", STAT_CON, OBJ_VNUM_SCHOOL_DAGGER,
	{3022, 9633}, 85, 20, -10, 13, 16, 1, 4, FALSE,
	"dragoon basics", "jump command", 260, CANBE_ALL, "Heavily armed masters of the dragon arts."
	
    },
};

/*
 * Attribute bonus tables.
 */

//raise_bonus
const struct prime_app_type prime_app[26] =
{
    {0},			/*  0 */
    {0},			/*  1 */
    {0},
    {0},			/*  3 */
    {0},
    {0},			/*  5 */
    {0},
    {0},
    {0},
    {0},
    {1},			/* 10 */
    {2},
    {2},
    {3},
    {3},
    {4},			/* 15 */
    {5},
    {6},
    {7},			/* 18 */
    {8},
    {9},			/* 20 */
    {14},
    {20},
    {28},
    {36},
    {45}			/* 25 */
};

//tohit, todam, carry, wield
const struct str_app_type str_app[26] =
{
    {-5, -4, 0, 0},		/* 0  */
    {-5, -4, 3, 1},		/* 1  */
    {-3, -2, 3, 2},
    {-3, -1, 10, 3},		/* 3  */
    {-2, -1, 25, 4},
    {-2, -1, 55, 5},		/* 5  */
    {-1, 0, 80, 6},
    {-1, 0, 90, 7},
    {0, 0, 100, 8},
    {0, 0, 100, 9},
    {0, 0, 115, 10},		/* 10  */
    {0, 0, 115, 11},
    {0, 0, 130, 12},
    {0, 0, 130, 13},		/* 13  */
    {1, 1, 140, 14},
    {2, 2, 150, 15},		/* 15  */
    {3, 3, 165, 16},
    {4, 4, 180, 22},
    {5, 5, 200, 25},		/* 18  */
    {6, 7, 225, 30},
    {7, 10, 250, 35},		/* 20  */
    {8, 13, 300, 40},
    {9, 16, 350, 45},
    {10, 18, 400, 50},
    {11, 21, 450, 55},
    {12, 24, 500, 60}		/* 25   */
};

//learn
const struct int_app_type int_app[26] =
{
    {3},			/*  0 */
    {5},			/*  1 */
    {7},
    {8},			/*  3 */
    {9},
    {10},			/*  5 */
    {11},
    {12},
    {13},
    {15},
    {17},			/* 10 */
    {19},
    {22},
    {25},
    {28},
    {31},			/* 15 */
    {34},
    {37},
    {40},			/* 18 */
    {44},
    {49},			/* 20 */
    {55},
    {60},
    {70},
    {80},
    {85}			/* 25 */
};

//practice
const struct wis_app_type wis_app[26] =
{
    {0},			/*  0 */
    {0},			/*  1 */
    {0},
    {0},			/*  3 */
    {0},
    {0},			/*  5 */
    {0},
    {0},
    {0},
    {0},
    {1},			/* 10 */
    {1},
    {2},
    {2},
    {2},
    {3},			/* 15 */
    {3},
    {3},
    {3},			/* 18 */
    {3},
    {4},			/* 20 */
    {4},
    {4},
    {4},
    {4},
    {5}				/* 25 */
};

//defensive, tohit
const struct dex_app_type dex_app[26] =
{
    {60, -4},			/* 0 */
    {50, -3},			/* 1 */
    {50, -3},
    {40, -2},
    {30. - 2},
    {20, -1},			/* 5 */
    {10, -1},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},			/* 10 */
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {-10, 1},			/* 15 */
    {-15, 2},
    {-20, 3},
    {-30, 4},
    {-40, 5},
    {-50, 6},			/* 20 */
    {-60, 8},
    {-75, 12},
    {-90, 18},
    {-105, 22},
    {-120, 26}			/* 25 */
};

//hitp, shock
const struct con_app_type con_app[26] =
{
    {-4, 20},			/*  0 */
    {-3, 25},			/*  1 */
    {-2, 30},
    {-2, 35},			/*  3 */
    {-1, 40},
    {-1, 45},			/*  5 */
    {-1, 50},
    {0, 55},
    {0, 60},
    {0, 65},
    {0, 70},			/* 10 */
    {0, 75},
    {0, 80},
    {0, 85},
    {0, 88},
    {1, 90},			/* 15 */
    {2, 95},
    {2, 97},
    {3, 99},			/* 18 */
    {3, 99},
    {4, 99},			/* 20 */
    {4, 99},
    {5, 99},
    {6, 99},
    {7, 99},
    {8, 99}			/* 25 */
};

/*
 * Liquid properties.
 * Used in world.obj.
 */
const struct liq_type liq_table[] =
{
/*    name                      color   proof, full, thirst, food, ssize */
    {"water", "clear",
     {0, 1, 10, 0, 16}},
    {"beer", "amber",
     {12, 1, 8, 1, 12}},
    {"red wine", "burgundy",
     {30, 1, 8, 1, 5}},
    {"ale", "brown",
     {15, 1, 8, 1, 12}},
    {"dark ale", "dark",
     {16, 1, 8, 1, 12}},

    {"whisky", "golden",
     {120, 1, 5, 0, 2}},
    {"lemonade", "pink",
     {0, 1, 9, 2, 12}},
    {"firebreather", "boiling",
     {190, 0, 4, 0, 2}},
    {"local specialty", "clear",
     {151, 1, 3, 0, 2}},
    {"slime mold juice", "green",
     {0, 2, -8, 1, 2}},

    {"milk", "white",
     {0, 2, 9, 3, 12}},
    {"tea", "tan",
     {0, 1, 8, 0, 6}},
    {"coffee", "black",
     {0, 1, 8, 0, 6}},
    {"blood", "red",
     {0, 2, -1, 2, 6}},
    {"salt water", "clear",
     {0, 1, -2, 0, 1}},

    {"coke", "brown",
     {0, 2, 9, 2, 12}},
    {"root beer", "brown",
     {0, 2, 9, 2, 12}},
    {"elvish wine", "green",
     {35, 2, 8, 1, 5}},
    {"white wine", "golden",
     {28, 1, 8, 1, 5}},
    {"champagne", "golden",
     {32, 1, 8, 1, 5}},

    {"mead", "honey-colored",
     {34, 2, 8, 2, 12}},
    {"rose wine", "pink",
     {26, 1, 8, 1, 5}},
    {"benedictine wine", "burgundy",
     {40, 1, 8, 1, 5}},
    {"vodka", "clear",
     {130, 1, 5, 0, 2}},
    {"cranberry juice", "red",
     {0, 1, 9, 2, 12}},

    {"orange juice", "orange",
     {0, 2, 9, 3, 12}},
    {"absinthe", "green",
     {200, 1, 4, 0, 2}},
    {"brandy", "golden",
     {80, 1, 5, 0, 4}},
    {"aquavit", "clear",
     {140, 1, 5, 0, 2}},
    {"schnapps", "clear",
     {90, 1, 5, 0, 2}},

    {"icewine", "purple",
     {50, 2, 6, 1, 5}},
    {"amontillado", "burgundy",
     {35, 2, 8, 1, 5}},
    {"sherry", "red",
     {38, 2, 7, 1, 5}},
    {"framboise", "red",
     {50, 1, 7, 1, 5}},
    {"rum", "amber",
     {151, 1, 4, 0, 2}},

    {"cordial", "clear",
     {100, 1, 5, 0, 2}},
     
     {"akuji pee", "urine",
     {0, 2, 9, 3, 12}},
    {NULL, NULL,
     {0, 0, 0, 0, 0}}
};

/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n

const struct skill_type skill_table[MAX_SKILL] =
{

/*
 * Magic spells.
 */

    {
	"reserved",
	{99, 99, 99, 99},
	{99, 99, 99, 99},
	0, TAR_IGNORE, POS_STANDING,
	NULL, SLOT(0), 0, 0,
	"", "", ""
    },
//begin skill table


{
"acid blast",
{
/* Mag */	28,
/* Dru */	40,
/* She */	40,
/* Cle */	50,
/* Def */	50,
/* Nec */	28,
/* Dea */	50,
/* Kni */	50,
/* War */	32,
/* Wit */	28,
/* Bar */	50,
/* Ran */	50,
/* Ass */	35,
/* Mnk */	40,
/* Brd */	28,
/* Ill */	28,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   99,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(70),	30,	12,	"acid blast",	"!Acid Blast!",	""
}
,	

{
"animate dead",
{
/* Mag */	50,
/* Dru */	50,
/* She */	20,
/* Cle */	25,
/* Def */	16,
/* Nec */	20,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_animate,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(302),	125,	12,	"animate dead",	"!Animate!",	""
}
,	

{
"armor",
{
/* Mag */	9,
/* Dru */	6,
/* She */	6,
/* Cle */	5,
/* Def */	5,
/* Nec */	9,
/* Dea */	9,
/* Kni */	9,
/* War */	14,
/* Wit */	9,
/* Bar */	50,
/* Ran */	12,
/* Ass */	12,
/* Mnk */	8,
/* Brd */	5,
/* Ill */	5,
/* Tmp */	5,
/* Lan */	0,
/* Zea */   12,
/* Geo */   0,
/* Dra */   9 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   2,
/* Geo */   0,
/* Dra */   2 
}
,	spell_armor,	TAR_CHAR_DEFENSIVE,	POS_STANDING,	NULL,	SLOT(1),	5,	12,	"",	"You feel less armored.",	""
}
,	

{
"web",
{
/* Mag */	50,
/* Dru */	48,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	99,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	48,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	56,
/* Ill */	46,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	0,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_web,	TAR_CHAR_OFFENSIVE,	POS_STANDING,	NULL,	SLOT(300),	50,	12,	"",	"The web holding you fades away.",	""
}
,	

{
"decripify",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	45,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_decripify,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(1337),	100,	6,	"",	"You feel relieved.",	""
}
,	


{
"decay aura",
{
/* Mag */	20,
/* Dru */	20,
/* She */	20,
/* Cle */	20,
/* Def */	20,
/* Nec */	20,
/* Dea */	20,
/* Kni */	20,
/* War */	42,
/* Wit */	36,
/* Bar */	50,
/* Ran */	20,
/* Ass */	42,
/* Mnk */	20,
/* Brd */	20,
/* Ill */	36,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	1,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_decay_aura,	TAR_CHAR_SELF,	POS_STANDING,	NULL,	SLOT(1338),	20,	12,	"",	"The Decay Aura around your body fades.",	""
}
,


{
"entangle",
{
/* Mag */	50,
/* Dru */	30,
/* She */	30,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	99,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	5,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_entangle,	TAR_CHAR_OFFENSIVE,	POS_STANDING,	NULL,	SLOT(310),	50,	12,	"",	"The vines have released you.",	""
}
,	

{
"bless",
{
/* Mag */	6,
/* Dru */	2,
/* She */	2,
/* Cle */	2,
/* Def */	2,
/* Nec */	3,
/* Dea */	4,
/* Kni */	4,
/* War */	10,
/* Wit */	6,
/* Bar */	50,
/* Ran */	8,
/* Ass */	7,
/* Mnk */	8,
/* Brd */	8,
/* Ill */	6,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_bless,	TAR_OBJ_CHAR_DEF,	POS_STANDING,	NULL,	SLOT(3),	5,	12,	"",	"You feel less righteous.",	"$p's holy aura fades."
}
,	

{
"blindness",
{
/* Mag */	15,
/* Dru */	20,
/* She */	20,
/* Cle */	20,
/* Def */	20,
/* Nec */	10,
/* Dea */	20,
/* Kni */	20,
/* War */	28,
/* Wit */	15,
/* Bar */	50,
/* Ran */	50,
/* Ass */	22,
/* Mnk */	14,
/* Brd */	14,
/* Ill */	14,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_blindness,	SLOT(4),	5,	12,	"",	"You can see again.",	""
}
,	

{
"death coil",
{
/* Mag */	30,
/* Dru */	30,
/* She */	30,
/* Cle */	30,
/* Def */	30,
/* Nec */	30,
/* Dea */	30,
/* Kni */	30,
/* War */	30,
/* Wit */	30,
/* Bar */	50,
/* Ran */	30,
/* Ass */	30,
/* Mnk */	30,
/* Brd */	30,
/* Ill */	30,
/* Tmp */	30,
/* Lan */	30,
/* Zea */   30,
/* Geo */   30,
/* Dra */   30 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_death_coil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(45),	30,	12,	"coil",	"!Death Coil!",	""
}
,	

{
"haven",
{
/* Mag */	15,
/* Dru */	20,
/* She */	20,
/* Cle */	20,
/* Def */	20,
/* Nec */	10,
/* Dea */	20,
/* Kni */	20,
/* War */	28,
/* Wit */	15,
/* Bar */	50,
/* Ran */	50,
/* Ass */	22,
/* Mnk */	14,
/* Brd */	14,
/* Ill */	14,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_haven,	TAR_CHAR_SELF,	POS_STANDING,	NULL,	SLOT(46),	80,	24,	"",	"!Haven!",	""
}
,	

{
"unholy word",
{
/* Mag */	47,
/* Dru */	47,
/* She */	47,
/* Cle */	47,
/* Def */	47,
/* Nec */	77,
/* Dea */	47,
/* Kni */	47,
/* War */	47,
/* Wit */	47,
/* Bar */	50,
/* Ran */	47,
/* Ass */	47,
/* Mnk */	47,
/* Brd */	47,
/* Ill */	47,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_unholy_word,	TAR_IGNORE,	POS_FIGHTING,	NULL,	SLOT(47),	70,	12,	"",	"!Unholy Word!",	""
}
,	
{
"life drain",
{
/* Mag */	60,
/* Dru */	60,
/* She */	60,
/* Cle */	60,
/* Def */	52,
/* Nec */	40,
/* Dea */	50,
/* Kni */	60,
/* War */	60,
/* Wit */	50,
/* Bar */	60,
/* Ran */	60,
/* Ass */	60,
/* Mnk */	60,
/* Brd */	60,
/* Ill */	60,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_life_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(800),	30,	12,	"",	"You no longer are being drained of life.",	""
}
,
{
"mana burn",
{
/* Mag */	60,
/* Dru */	60,
/* She */	60,
/* Cle */	60,
/* Def */	52,
/* Nec */	35,
/* Dea */	50,
/* Kni */	60,
/* War */	60,
/* Wit */	50,
/* Bar */	60,
/* Ran */	60,
/* Ass */	60,
/* Mnk */	60,
/* Brd */	60,
/* Ill */	60,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_mana_burn,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(801),	30,	12,	"",	"You no longer lose mana.",	""
}
,

{
"hex",
{
/* Mag */	60,
/* Dru */	60,
/* She */	60,
/* Cle */	60,
/* Def */	52,
/* Nec */	33,
/* Dea */	50,
/* Kni */	60,
/* War */	60,
/* Wit */	50,
/* Bar */	60,
/* Ran */	60,
/* Ass */	60,
/* Mnk */	60,
/* Brd */	60,
/* Ill */	60,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_hex,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(802),	70,	12,	"",	"You are no longer cursed with black magic.",	""
}
,

{
"spirit rain",
{
/* Mag */	60,
/* Dru */	60,
/* She */	60,
/* Cle */	60,
/* Def */	52,
/* Nec */	48,
/* Dea */	50,
/* Kni */	60,
/* War */	60,
/* Wit */	50,
/* Bar */	60,
/* Ran */	60,
/* Ass */	60,
/* Mnk */	60,
/* Brd */	60,
/* Ill */	60,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_spirit_rain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(803),	150,	18,	"spirit rain",	"!Spirit Rain!",	""
}
,

{
"death touch",
{
/* Mag */	60,
/* Dru */	60,
/* She */	60,
/* Cle */	60,
/* Def */	52,
/* Nec */	99,
/* Dea */	50,
/* Kni */	60,
/* War */	60,
/* Wit */	50,
/* Bar */	60,
/* Ran */	60,
/* Ass */	60,
/* Mnk */	60,
/* Brd */	60,
/* Ill */	60,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_death_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(48),	30,	12,	"",	"You are no longer undead.",	""
}
,	

{
"minion",
{
/* Mag */	42,
/* Dru */	42,
/* She */	42,
/* Cle */	42,
/* Def */	42,
/* Nec */	42,
/* Dea */	42,
/* Kni */	42,
/* War */	42,
/* Wit */	42,
/* Bar */	50,
/* Ran */	42,
/* Ass */	42,
/* Mnk */	42,
/* Brd */	42,
/* Ill */	42,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0
 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_minion,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(49),	40,	12,	"",	"!Minion!",	""
}
,	

{
"soul coil",
{
/* Mag */	37,
/* Dru */	37,
/* She */	37,
/* Cle */	37,
/* Def */	37,
/* Nec */	37,
/* Dea */	37,
/* Kni */	37,
/* War */	37,
/* Wit */	37,
/* Bar */	50,
/* Ran */	37,
/* Ass */	37,
/* Mnk */	37,
/* Brd */	37,
/* Ill */	37,
/* Tmp */	37,
/* Lan */	37,
/* Zea */   37,
/* Geo */   37,
/* Dra */   37 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   2 
}
,	spell_soul_coil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,
SLOT(50),	5,	15,	"{Wmental attack{x",	"!Soul Coil!",	""
}
,	

{
"wall of force",
{
/* Mag */	36,
/* Dru */	36,
/* She */	36,
/* Cle */	36,
/* Def */	36,
/* Nec */	36,
/* Dea */	36,
/* Kni */	36,
/* War */	36,
/* Wit */	36,
/* Bar */	50,
/* Ran */	36,
/* Ass */	36,
/* Mnk */	36,
/* Brd */	36,
/* Ill */	36,
/* Tmp */	38,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_wall_of_force,	TAR_IGNORE,	POS_STANDING,	NULL, SLOT(51),	75,	12,	"",	"!Wall Of Force!",	""
}
,	

{
"flaming wall",
{
/* Mag */	26,
/* Dru */	26,
/* She */	26,
/* Cle */	26,
/* Def */	26,
/* Nec */	26,
/* Dea */	26,
/* Kni */	26,
/* War */	26,
/* Wit */	26,
/* Bar */	50,
/* Ran */	26,
/* Ass */	26,
/* Mnk */	26,
/* Brd */	26,
/* Ill */	26,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   45,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_flaming_wall,	TAR_IGNORE,	POS_STANDING,	&gsn_flaming_wall,	SLOT(52),	60,	12,	"",	"!Flaming Wall!",	""
}
,	

{
"screaming wall",
{
/* Mag */	16,
/* Dru */	16,
/* She */	16,
/* Cle */	16,
/* Def */	16,
/* Nec */	16,
/* Dea */	16,
/* Kni */	16,
/* War */	16,
/* Wit */	16,
/* Bar */	50,
/* Ran */	16,
/* Ass */	16,
/* Mnk */	16,
/* Brd */	16,
/* Ill */	16,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_screaming_wall,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(84),	35,	12,	"",	"!Screaming Wall!",	""
}
,	

{
"living wall",
{
/* Mag */	46,
/* Dru */	46,
/* She */	46,
/* Cle */	46,
/* Def */	46,
/* Nec */	46,
/* Dea */	46,
/* Kni */	46,
/* War */	46,
/* Wit */	46,
/* Bar */	50,
/* Ran */	46,
/* Ass */	46,
/* Mnk */	46,
/* Brd */	46,
/* Ill */	46,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_living_wall,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(85),	80,	12,	"",	"!Living Wall!",	""
}
,	

{
"purify",
{
/* Mag */	34,
/* Dru */	34,
/* She */	34,
/* Cle */	34,
/* Def */	34,
/* Nec */	34,
/* Dea */	34,
/* Kni */	34,
/* War */	34,
/* Wit */	34,
/* Bar */	50,
/* Ran */	34,
/* Ass */	34,
/* Mnk */	34,
/* Brd */	34,
/* Ill */	34,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   45,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   2,
/* Geo */   0,
/* Dra */   0 
}
,	spell_purify,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,	NULL,	SLOT(86),	20,	12,	"",	"!Purify!",	""
}
,	

{
"nullify",
{
/* Mag */	34,
/* Dru */	34,
/* She */	34,
/* Cle */	34,
/* Def */	34,
/* Nec */	99,
/* Dea */	34,
/* Kni */	34,
/* War */	34,
/* Wit */	34,
/* Bar */	50,
/* Ran */	34,
/* Ass */	34,
/* Mnk */	34,
/* Brd */	34,
/* Ill */	34,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   45,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   2,
/* Geo */   0,
/* Dra */   0 
}
,	spell_nullify,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(87),	20,	12,	"",	"!Nullify!",	""
}
,	

{
"burning hands",
{
/* Mag */	7,
/* Dru */	10,
/* She */	10,
/* Cle */	10,
/* Def */	10,
/* Nec */	10,
/* Dea */	12,
/* Kni */	12,
/* War */	13,
/* Wit */	7,
/* Bar */	50,
/* Ran */	8,
/* Ass */	7,
/* Mnk */	8,
/* Brd */	8,
/* Ill */	7,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(5),	15,	12,	"burning hands",	"!Burning Hands!",	""
}
,	

{
"call lightning",
{
/* Mag */	21,
/* Dru */	14,
/* She */	14,
/* Cle */	18,
/* Def */	18,
/* Nec */	16,
/* Dea */	19,
/* Kni */	19,
/* War */	22,
/* Wit */	21,
/* Bar */	50,
/* Ran */	26,
/* Ass */	26,
/* Mnk */	26,
/* Brd */	26,
/* Ill */	21,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   35,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_call_lightning,	TAR_IGNORE,	POS_FIGHTING,	NULL,	SLOT(6),	15,	12,	"lightning bolt",	"!Call Lightning!",	""
}
,	

{
"calm",
{
/* Mag */	40,
/* Dru */	22,
/* She */	22,
/* Cle */	20,
/* Def */	20,
/* Nec */	29,
/* Dea */	24,
/* Kni */	24,
/* War */	46,
/* Wit */	40,
/* Bar */	50,
/* Ran */	40,
/* Ass */	45,
/* Mnk */	40,
/* Brd */	40,
/* Ill */	40,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_calm,	TAR_IGNORE,	POS_FIGHTING,	NULL,	SLOT(509),	30,	12,	"",	"You have lost your peace of mind.",	""
}
,	

{
"cancellation",
{
/* Mag */	18,
/* Dru */	20,
/* She */	20,
/* Cle */	20,
/* Def */	20,
/* Nec */	20,
/* Dea */	21,
/* Kni */	21,
/* War */	34,
/* Wit */	18,
/* Bar */	50,
/* Ran */	22,
/* Ass */	31,
/* Mnk */	23,
/* Brd */	21,
/* Ill */	18,
/* Tmp */	18,
/* Lan */	0,
/* Zea */   34,
/* Geo */   0,
/* Dra */   18 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   2,
/* Geo */   0,
/* Dra */   2 
}
,	spell_cancellation,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,	NULL,	SLOT(507),	20,	12,	"" "!cancellation!",	"",	""
}
,	

{
"cause critical",
{
/* Mag */	18,
/* Dru */	15,
/* She */	15,
/* Cle */	14,
/* Def */	14,
/* Nec */	12,
/* Dea */	16,
/* Kni */	16,
/* War */	22,
/* Wit */	18,
/* Bar */	50,
/* Ran */	22,
/* Ass */	24,
/* Mnk */	24,
/* Brd */	22,
/* Ill */	18,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(63),	20,	12,	"spell",	"!Cause Critical!",	""
}
,	

{
"cause light",
{
/* Mag */	5,
/* Dru */	2,
/* She */	2,
/* Cle */	2,
/* Def */	2,
/* Nec */	1,
/* Dea */	4,
/* Kni */	4,
/* War */	10,
/* Wit */	5,
/* Bar */	50,
/* Ran */	4,
/* Ass */	12,
/* Mnk */	6,
/* Brd */	6,
/* Ill */	5,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(62),	15,	12,	"spell",	"!Cause Light!",	""
}
,	

{
"cause serious",
{
/* Mag */	9,
/* Dru */	15,
/* She */	15,
/* Cle */	7,
/* Def */	7,
/* Nec */	6,
/* Dea */	11,
/* Kni */	11,
/* War */	17,
/* Wit */	12,
/* Bar */	50,
/* Ran */	15,
/* Ass */	15,
/* Mnk */	15,
/* Brd */	15,
/* Ill */	12,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(64),	17,	12,	"spell",	"!Cause Serious!",	""
}
,	

{
"chain lightning",
{
/* Mag */	33,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	45,
/* Wit */	33,
/* Bar */	50,
/* Ran */	50,
/* Ass */	39,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	33,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_chain_lightning,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(500),	40,	12,	"lightning",	"!Chain Lightning!",	""
}
,	

{
"change sex",
{
/* Mag */	18,
/* Dru */	15,
/* She */	15,
/* Cle */	14,
/* Def */	14,
/* Nec */	12,
/* Dea */	16,
/* Kni */	16,
/* War */	22,
/* Wit */	18,
/* Bar */	50,
/* Ran */	50,
/* Ass */	24,
/* Mnk */	23,
/* Brd */	23,
/* Ill */	18,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,	NULL,	SLOT(82),	15,	12,	"",	"Your body feels familiar again.",	""
}
,	

{
"charm person",
{
/* Mag */	26,
/* Dru */	32,
/* She */	32,
/* Cle */	32,
/* Def */	32,
/* Nec */	32,
/* Dea */	32,
/* Kni */	32,
/* War */	49,
/* Wit */	26,
/* Bar */	50,
/* Ran */	32,
/* Ass */	39,
/* Mnk */	32,
/* Brd */	22,
/* Ill */	20,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_charm_person,	TAR_CHAR_OFFENSIVE,	POS_STANDING,	&gsn_charm_person,	SLOT(7),	5,	12,	"",	"You feel more self-confident.",	""
}
,	

{
"chill touch",
{
/* Mag */	4,
/* Dru */	7,
/* She */	7,
/* Cle */	7,
/* Def */	7,
/* Nec */	4,
/* Dea */	11,
/* Kni */	11,
/* War */	11,
/* Wit */	4,
/* Bar */	50,
/* Ran */	7,
/* Ass */	9,
/* Mnk */	7,
/* Brd */	8,
/* Ill */	4,
/* Tmp */	4,
/* Lan */	0,
/* Zea */   0,
/* Geo */   14,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(8),	15,	12,	"chilling touch",	"You feel less cold.",	""
}
,	

{
"colour spray",
{
/* Mag */	16,
/* Dru */	19,
/* She */	19,
/* Cle */	19,
/* Def */	19,
/* Nec */	16,
/* Dea */	22,
/* Kni */	22,
/* War */	23,
/* Wit */	16,
/* Bar */	50,
/* Ran */	23,
/* Ass */	24,
/* Mnk */	19,
/* Brd */	19,
/* Ill */	16,
/* Tmp */	16,
/* Lan */	0,
/* Zea */   0,
/* Geo */   28,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(10),	15,	12,	"colour spray",	"!Colour Spray!",	""
}
,	

{
"continual light",
{
/* Mag */	6,
/* Dru */	4,
/* She */	4,
/* Cle */	4,
/* Def */	4,
/* Nec */	4,
/* Dea */	8,
/* Kni */	8,
/* War */	9,
/* Wit */	6,
/* Bar */	50,
/* Ran */	8,
/* Ass */	6,
/* Mnk */	10,
/* Brd */	10,
/* Ill */	6,
/* Tmp */	6,
/* Lan */	0,
/* Zea */   10,
/* Geo */   22,
/* Dra */   9 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   2,
/* Geo */   0,
/* Dra */   2 
}
,	spell_continual_light,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(57),	7,	12,	"",	"!Continual Light!",	""
}
,	

{
"servant",
{
/* Mag */	10,
/* Dru */	10,
/* She */	10,
/* Cle */	10,
/* Def */	10,
/* Nec */	10,
/* Dea */	10,
/* Kni */	10,
/* War */	10,
/* Wit */	10,
/* Bar */	50,
/* Ran */	10,
/* Ass */	10,
/* Mnk */	10,
/* Brd */	10,
/* Ill */	10,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_servant,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(301),	50,	12,	"",	"!Servant!",	""
}
,	

{
"decoy",
{
/* Mag */	33,
/* Dru */	33,
/* She */	33,
/* Cle */	33,
/* Def */	33,
/* Nec */	33,
/* Dea */	33,
/* Kni */	33,
/* War */	33,
/* Wit */	33,
/* Bar */	50,
/* Ran */	33,
/* Ass */	33,
/* Mnk */	33,
/* Brd */	33,
/* Ill */	33,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_decoy,	TAR_CHAR_DEFENSIVE,	POS_STANDING,	NULL,	SLOT(303),	30,	12,	"",	"!Decoy!",	""
}
,	

{
"guardian",
{
/* Mag */	38,
/* Dru */	38,
/* She */	38,
/* Cle */	38,
/* Def */	38,
/* Nec */	38,
/* Dea */	38,
/* Kni */	38,
/* War */	38,
/* Wit */	38,
/* Bar */	50,
/* Ran */	38,
/* Ass */	38,
/* Mnk */	38,
/* Brd */	38,
/* Ill */	38,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_guardian,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(302),	150,	12,	"",	"!Guardian!",	""
}
,	

{
"mount",
{
/* Mag */	21,
/* Dru */	21,
/* She */	21,
/* Cle */	21,
/* Def */	21,
/* Nec */	21,
/* Dea */	21,
/* Kni */	21,
/* War */	21,
/* Wit */	21,
/* Bar */	50,
/* Ran */	21,
/* Ass */	21,
/* Mnk */	21,
/* Brd */	21,
/* Ill */	21,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_mount,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(304),	50,	12,	"",	"!Mount!",	""
}
,	

{
"control weather",
{
/* Mag */	15,
/* Dru */	14,
/* She */	14,
/* Cle */	19,
/* Def */	19,
/* Nec */	19,
/* Dea */	20,
/* Kni */	20,
/* War */	22,
/* Wit */	15,
/* Bar */	50,
/* Ran */	16,
/* Ass */	16,
/* Mnk */	16,
/* Brd */	16,
/* Ill */	15,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   30,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_control_weather,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(11),	10,	12,	"",	"!Control Weather!",	""
}
,	

{
"create food",
{
/* Mag */	5,
/* Dru */	5,
/* She */	5,
/* Cle */	5,
/* Def */	5,
/* Nec */	5,
/* Dea */	5,
/* Kni */	5,
/* War */	5,
/* Wit */	5,
/* Bar */	50,
/* Ran */	5,
/* Ass */	5,
/* Mnk */	5,
/* Brd */	5,
/* Ill */	5,
/* Tmp */	5,
/* Lan */	0,
/* Zea */   5,
/* Geo */   2,
/* Dra */   5 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   2,
/* Geo */   0,
/* Dra */   2 
}
,	spell_create_food,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(12),	5,	12,	"",	"!Create Food!",	""
}
,	

{
"create rose",
{
/* Mag */	15,
/* Dru */	15,
/* She */	15,
/* Cle */	15,
/* Def */	15,
/* Nec */	15,
/* Dea */	15,
/* Kni */	15,
/* War */	15,
/* Wit */	15,
/* Bar */	50,
/* Ran */	15,
/* Ass */	15,
/* Mnk */	15,
/* Brd */	15,
/* Ill */	15,
/* Tmp */	15,
/* Lan */	0,
/* Zea */   15,
/* Geo */   5,
/* Dra */   15 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   1,
/* Geo */   0,
/* Dra */   2 
}
,	spell_create_rose,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(511),	3,	12,	"",	"!Create Rose!",	""
}
,	

{
"create spring",
{
/* Mag */	14,
/* Dru */	13,
/* She */	13,
/* Cle */	17,
/* Def */	17,
/* Nec */	17,
/* Dea */	20,
/* Kni */	20,
/* War */	20,
/* Wit */	14,
/* Bar */	50,
/* Ran */	16,
/* Ass */	21,
/* Mnk */	20,
/* Brd */	18,
/* Ill */	14,
/* Tmp */	14,
/* Lan */	0,
/* Zea */   20,
/* Geo */   8,
/* Dra */   14 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   1,
/* Geo */   0,
/* Dra */   1 
}
,	spell_create_spring,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(80),	20,	12,	"",	"!Create Spring!",	""
}
,	

{
"create water",
{
/* Mag */	8,
/* Dru */	3,
/* She */	3,
/* Cle */	3,
/* Def */	3,
/* Nec */	8,
/* Dea */	11,
/* Kni */	11,
/* War */	11,
/* Wit */	8,
/* Bar */	50,
/* Ran */	5,
/* Ass */	8,
/* Mnk */	5,
/* Brd */	8,
/* Ill */	8,
/* Tmp */	8,
/* Lan */	0,
/* Zea */   5,
/* Geo */   18,
/* Dra */   8 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   1,
/* Geo */   0,
/* Dra */   1 
}
,	spell_create_water,	TAR_OBJ_INV,	POS_STANDING,	NULL,	SLOT(13),	5,	12,	"",	"!Create Water!",	""
}
,	

{
"cure blindness",
{
/* Mag */	11,
/* Dru */	6,
/* She */	6,
/* Cle */	6,
/* Def */	6,
/* Nec */	17,
/* Dea */	6,
/* Kni */	6,
/* War */	17,
/* Wit */	91,
/* Bar */	50,
/* Ran */	17,
/* Ass */	18,
/* Mnk */	17,
/* Brd */	18,
/* Ill */	11,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,	NULL,	SLOT(14),	5,	12,	"",	"!Cure Blindness!",	""
}
,	

{
"cure critical",
{
/* Mag */	18,
/* Dru */	14,
/* She */	14,
/* Cle */	13,
/* Def */	13,
/* Nec */	15,
/* Dea */	18,
/* Kni */	18,
/* War */	26,
/* Wit */	18,
/* Bar */	50,
/* Ran */	26,
/* Ass */	26,
/* Mnk */	99,
/* Brd */	26,
/* Ill */	26,
/* Tmp */	40,
/* Lan */	40,
/* Zea */   40,
/* Geo */   40,
/* Dra */   40 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,	NULL,	SLOT(15),	20,	12,	"",	"!Cure Critical!",	""
}
,	

{
"cure disease",
{
/* Mag */	16,
/* Dru */	13,
/* She */	13,
/* Cle */	13,
/* Def */	13,
/* Nec */	16,
/* Dea */	13,
/* Kni */	13,
/* War */	16,
/* Wit */	99,
/* Bar */	50,
/* Ran */	16,
/* Ass */	17,
/* Mnk */	16,
/* Brd */	16,
/* Ill */	16,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0
}
,	spell_cure_disease,	TAR_CHAR_DEFENSIVE,	POS_STANDING,	NULL,	SLOT(501),	20,	12,	"",	"!Cure Disease!",	""
}
,	

{
"cure light",
{
/* Mag */	7,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	4,
/* Dea */	3,
/* Kni */	3,
/* War */	12,
/* Wit */	7,
/* Bar */	50,
/* Ran */	12,
/* Ass */	12,
/* Mnk */	12,
/* Brd */	12,
/* Ill */	7,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   20,
/* Geo */   10,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   2,
/* Geo */   0,
/* Dra */   0 
}
,	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,	NULL,	SLOT(16),	10,	12,	"",	"!Cure Light!",	""
}
,	

{
"cure poison",
{
/* Mag */	17,
/* Dru */	12,
/* She */	12,
/* Cle */	12,
/* Def */	12,
/* Nec */	12,
/* Dea */	14,
/* Kni */	14,
/* War */	18,
/* Wit */	99,
/* Bar */	50,
/* Ran */	17,
/* Ass */	18,
/* Mnk */	15,
/* Brd */	18,
/* Ill */	17,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,	NULL,	SLOT(43),	5,	12,	"",	"!Cure Poison!",	""
}
,	

{
"cure serious",
{
/* Mag */	13,
/* Dru */	8,
/* She */	8,
/* Cle */	8,
/* Def */	8,
/* Nec */	9,
/* Dea */	10,
/* Kni */	10,
/* War */	20,
/* Wit */	13,
/* Bar */	50,
/* Ran */	13,
/* Ass */	20,
/* Mnk */	13,
/* Brd */	19,
/* Ill */	19,
/* Tmp */	19,
/* Lan */	19,
/* Zea */   0,
/* Geo */   0,
/* Dra */   30 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	2,
/* Zea */   0,
/* Geo */   0,
/* Dra */   2 
}
,	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,	NULL,	SLOT(61),	15,	12,	"",	"!Cure Serious!",	""
}
,	

{
"curse",
{
/* Mag */	18,
/* Dru */	18,
/* She */	18,
/* Cle */	18,
/* Def */	18,
/* Nec */	15,
/* Dea */	19,
/* Kni */	19,
/* War */	22,
/* Wit */	18,
/* Bar */	50,
/* Ran */	18,
/* Ass */	26,
/* Mnk */	18,
/* Brd */	18,
/* Ill */	18,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_curse,	TAR_OBJ_CHAR_OFF,	POS_FIGHTING,	&gsn_curse,	SLOT(17),	20,	12,	"curse",	"The curse wears off.",	"$p is no longer impure."
}
,	

{
"agony",
{
/* Mag */	45,
/* Dru */	45,
/* She */	45,
/* Cle */	45,
/* Def */	45,
/* Nec */	45,
/* Dea */	45,
/* Kni */	45,
/* War */	45,
/* Wit */	45,
/* Bar */	45,
/* Ran */	45,
/* Ass */	45,
/* Mnk */	45,
/* Brd */	45,
/* Ill */	45,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_agony,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(555),	100,	12,	"agony",	"",	""
}
,	

{
"demonfire",
{
/* Mag */	39,
/* Dru */	31,
/* She */	31,
/* Cle */	34,
/* Def */	34,
/* Nec */	30,
/* Dea */	34,
/* Kni */	34,
/* War */	45,
/* Wit */	39,
/* Bar */	50,
/* Ran */	35,
/* Ass */	46,
/* Mnk */	39,
/* Brd */	39,
/* Ill */	39,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_demonfire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(505),	20,	12,	"torments",	"!Demonfire!",	""
}
,	

{
"detect alignment",
{
/* Mag */	4,
/* Dru */	4,
/* She */	4,
/* Cle */	4,
/* Def */	4,
/* Nec */	4,
/* Dea */	4,
/* Kni */	4,
/* War */	4,
/* Wit */	4,
/* Bar */	50,
/* Ran */	4,
/* Ass */	4,
/* Mnk */	4,
/* Brd */	4,
/* Ill */	4,
/* Tmp */	4,
/* Lan */	0,
/* Zea */   4,
/* Geo */   1,
/* Dra */   4 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   1,
/* Geo */   0,
/* Dra */   1 
}
,	spell_detect_alignment,	TAR_CHAR_SELF,	POS_STANDING,	NULL,	SLOT(513),	5,	12,	"",	"The gold in your vision disappears.",	""
}
,	

{
"detect hidden",
{
/* Mag */	11,
/* Dru */	11,
/* She */	11,
/* Cle */	11,
/* Def */	11,
/* Nec */	15,
/* Dea */	11,
/* Kni */	11,
/* War */	11,
/* Wit */	11,
/* Bar */	50,
/* Ran */	11,
/* Ass */	11,
/* Mnk */	11,
/* Brd */	11,
/* Ill */	11,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   38,
/* Dra */   45 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   2 
}
,	spell_detect_hidden,	TAR_CHAR_SELF,	POS_STANDING,	NULL,	SLOT(44),	5,	12,	"",	"You feel less aware of your surroundings.",	""
}
,	

{
"detect invisibility",
{
/* Mag */	8,
/* Dru */	8,
/* She */	8,
/* Cle */	8,
/* Def */	8,
/* Nec */	15,
/* Dea */	8,
/* Kni */	8,
/* War */	8,
/* Wit */	8,
/* Bar */	50,
/* Ran */	8,
/* Ass */	8,
/* Mnk */	8,
/* Brd */	8,
/* Ill */	8,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   45,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_detect_invis,	TAR_CHAR_SELF,	POS_STANDING,	NULL,	SLOT(19),	5,	12,	"",	"You no longer see invisible objects.",	""
}
,	

{
"detect magic",
{
/* Mag */	2,
/* Dru */	6,
/* She */	6,
/* Cle */	6,
/* Def */	6,
/* Nec */	6,
/* Dea */	6,
/* Kni */	6,
/* War */	6,
/* Wit */	6,
/* Bar */	50,
/* Ran */	6,
/* Ass */	6,
/* Mnk */	6,
/* Brd */	6,
/* Ill */	2,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_detect_magic,	TAR_CHAR_SELF,	POS_STANDING,	NULL,	SLOT(20),	5,	12,	"",	"The detect magic wears off.",	""
}
,	

{
"detect poison",
{
/* Mag */	7,
/* Dru */	7,
/* She */	7,
/* Cle */	7,
/* Def */	7,
/* Nec */	7,
/* Dea */	7,
/* Kni */	7,
/* War */	7,
/* Wit */	7,
/* Bar */	50,
/* Ran */	7,
/* Ass */	7,
/* Mnk */	7,
/* Brd */	7,
/* Ill */	7,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_detect_poison,	TAR_OBJ_INV,	POS_STANDING,	NULL,	SLOT(21),	5,	12,	"",	"!Detect Poison!",	""
}
,	

{
"dispel evil",
{
/* Mag */	19,
/* Dru */	19,
/* She */	19,
/* Cle */	19,
/* Def */	19,
/* Nec */	19,
/* Dea */	19,
/* Kni */	19,
/* War */	19,
/* Wit */	19,
/* Bar */	50,
/* Ran */	19,
/* Ass */	19,
/* Mnk */	19,
/* Brd */	19,
/* Ill */	19,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(22),	15,	12,	"dispel evil",	"!Dispel Evil!",	""
}
,	

{
"dispel magic",
{
/* Mag */	30,
/* Dru */	24,
/* She */	24,
/* Cle */	24,
/* Def */	24,
/* Nec */	17,
/* Dea */	24,
/* Kni */	24,
/* War */	30,
/* Wit */	16,
/* Bar */	30,
/* Ran */	3,
/* Ass */	30,
/* Mnk */	30,
/* Brd */	30,
/* Ill */	16,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_dispel_magic,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(59),	15,	12,	"",	"!Dispel Magic!",	""
}
,	

{
"dispel good",
{
/* Mag */	19,
/* Dru */	19,
/* She */	19,
/* Cle */	19,
/* Def */	19,
/* Nec */	19,
/* Dea */	19,
/* Kni */	19,
/* War */	19,
/* Wit */	19,
/* Bar */	50,
/* Ran */	19,
/* Ass */	19,
/* Mnk */	19,
/* Brd */	19,
/* Ill */	19,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_dispel_good,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(512),	15,	12,	"dispel good",	"!Dispel Good!",	""
}
,	

{
"earthquake",
{
/* Mag */	12,
/* Dru */	10,
/* She */	10,
/* Cle */	10,
/* Def */	10,
/* Nec */	10,
/* Dea */	10,
/* Kni */	10,
/* War */	14,
/* Wit */	12,
/* Bar */	50,
/* Ran */	14,
/* Ass */	14,
/* Mnk */	14,
/* Brd */	14,
/* Ill */	12,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_earthquake,	TAR_IGNORE,	POS_FIGHTING,	NULL,	SLOT(23),	15,	12,	"earthquake",	"!Earthquake!",	""
}
,	

{
"tremor",
{
/* Mag */	50,
/* Dru */	45,
/* She */	45,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_tremor,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(311),	40,	12,	"tremor",	"!Tremor!",	""
}
,	

{
"enchant armor",
{
/* Mag */	18,
/* Dru */	18,
/* She */	18,
/* Cle */	18,
/* Def */	18,
/* Nec */	18,
/* Dea */	18,
/* Kni */	18,
/* War */	18,
/* Wit */	18,
/* Bar */	50,
/* Ran */	18,
/* Ass */	18,
/* Mnk */	18,
/* Brd */	18,
/* Ill */	18,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_enchant_armor,	TAR_OBJ_INV,	POS_STANDING,	NULL,	SLOT(510),	50,	12,	"",	"!Enchant Armor!",	""
}
,	

{
"enchant weapon",
{
/* Mag */	18,
/* Dru */	18,
/* She */	18,
/* Cle */	18,
/* Def */	18,
/* Nec */	18,
/* Dea */	18,
/* Kni */	18,
/* War */	18,
/* Wit */	18,
/* Bar */	50,
/* Ran */	18,
/* Ass */	18,
/* Mnk */	18,
/* Brd */	18,
/* Ill */	18,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_enchant_weapon,	TAR_OBJ_INV,	POS_STANDING,	NULL,	SLOT(24),	50,	12,	"",	"!Enchant Weapon!",	""
}
,	

{
"imbue",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	30,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_imbue,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(100),	50,	12,	"",	"!Imbue!",	""
}
,	

{
"alchemy",
{
/* Mag */	15,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	15,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_alchemy,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(101),	15,	12,	"",	"!Alchemy!",	""
}
,	

{
"scribery",
{
/* Mag */	29,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	29,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_scribery,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(102),	15,	12,	"",	"!scribery!",	""
}
,	

{
"silence",
{
/* Mag */	44,
/* Dru */	44,
/* She */	44,
/* Cle */	44,
/* Def */	44,
/* Nec */	44,
/* Dea */	44,
/* Kni */	44,
/* War */	44,
/* Wit */	44,
/* Bar */	50,
/* Ran */	44,
/* Ass */	44,
/* Mnk */	44,
/* Brd */	44,
/* Ill */	44,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_silence,	TAR_CHAR_SELF,	POS_FIGHTING,	NULL,	SLOT(314),	50,	12,	"energy drain",	"!Silence!",	""
}
,	

{
"strengthen ground",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	25,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   40,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   4,
/* Dra */   0 
}
,	spell_strengthen_ground,	TAR_CHAR_SELF,	POS_STANDING,	NULL,	SLOT(315),	90,	12,	"energy drain",	"!Silence!",	""
}
,	

{
"energy drain",
{
/* Mag */	19,
/* Dru */	22,
/* She */	22,
/* Cle */	22,
/* Def */	22,
/* Nec */	18,
/* Dea */	22,
/* Kni */	22,
/* War */	26,
/* Wit */	19,
/* Bar */	50,
/* Ran */	22,
/* Ass */	26,
/* Mnk */	22,
/* Brd */	22,
/* Ill */	19,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   38,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(25),	20,	12,	"energy drain",	"!Energy Drain!",	""
}
,	

{
"faerie fire",
{
/* Mag */	6,
/* Dru */	3,
/* She */	3,
/* Cle */	3,
/* Def */	3,
/* Nec */	3,
/* Dea */	3,
/* Kni */	3,
/* War */	8,
/* Wit */	6,
/* Bar */	50,
/* Ran */	6,
/* Ass */	7,
/* Mnk */	6,
/* Brd */	6,
/* Ill */	6,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(72),	5,	12,	"faerie fire",	"The pink aura around you fades away.",	""
}
,	

{
"faerie fog",
{
/* Mag */	14,
/* Dru */	12,
/* She */	12,
/* Cle */	12,
/* Def */	12,
/* Nec */	14,
/* Dea */	15,
/* Kni */	15,
/* War */	23,
/* Wit */	14,
/* Bar */	50,
/* Ran */	14,
/* Ass */	23,
/* Mnk */	23,
/* Brd */	23,
/* Ill */	14,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_faerie_fog,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(73),	12,	4,	"faerie fog",	"!Faerie Fog!",	""
}
,	

{
"farsight",
{
/* Mag */	14,
/* Dru */	16,
/* She */	16,
/* Cle */	16,
/* Def */	16,
/* Nec */	16,
/* Dea */	16,
/* Kni */	16,
/* War */	16,
/* Wit */	14,
/* Bar */	50,
/* Ran */	16,
/* Ass */	16,
/* Mnk */	16,
/* Brd */	14,
/* Ill */	14,
/* Tmp */	14,
/* Lan */	0,
/* Zea */   0,
/* Geo */   99,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_farsight,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(521),	36,	12,	"farsight",	"!Farsight!",	""
}
,	

{
"eagle eye",
{
/* Mag */	14,
/* Dru */	16,
/* She */	16,
/* Cle */	16,
/* Def */	16,
/* Nec */	16,
/* Dea */	16,
/* Kni */	16,
/* War */	16,
/* Wit */	14,
/* Bar */	50,
/* Ran */	16,
/* Ass */	16,
/* Mnk */	16,
/* Brd */	14,
/* Ill */	14,
/* Tmp */	35,
/* Lan */	35,
/* Zea */   35,
/* Geo */   35,
/* Dra */   35 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	2,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_eagle_eye,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(556),	36,	12,	"eagle eye",	"!Eagle eye!",	""
}
,	

{
"call animal",
{
/* Mag */	99,
/* Dru */	99,
/* She */	99,
/* Cle */	99,
/* Def */	99,
/* Nec */	99,
/* Dea */	99,
/* Kni */	99,
/* War */	99,
/* Wit */	99,
/* Bar */	99,
/* Ran */	99,
/* Ass */	99,
/* Mnk */	99,
/* Brd */	99,
/* Ill */	99,
/* Tmp */	99,
/* Lan */	99,
/* Zea */   99,
/* Geo */   99,
/* Dra */   99 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_call_animal,	TAR_CHAR_OFFENSIVE,	POS_STANDING,	NULL,	SLOT(521),	30,	12,	"call animal",	"!Call Animal!",	""
}
,	

{
"fireball",
{
/* Mag */	22,
/* Dru */	28,
/* She */	28,
/* Cle */	28,
/* Def */	28,
/* Nec */	25,
/* Dea */	28,
/* Kni */	28,
/* War */	30,
/* Wit */	24,
/* Bar */	50,
/* Ran */	28,
/* Ass */	30,
/* Mnk */	30,
/* Brd */	30,
/* Ill */	22,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   16,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_fireball,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(26),	15,	12,	"fireball",	"!Fireball!",	""
}
,	

{
"fireproof",
{
/* Mag */	12,
/* Dru */	12,
/* She */	12,
/* Cle */	12,
/* Def */	12,
/* Nec */	12,
/* Dea */	12,
/* Kni */	12,
/* War */	18,
/* Wit */	13,
/* Bar */	50,
/* Ran */	12,
/* Ass */	12,
/* Mnk */	12,
/* Brd */	12,
/* Ill */	12,
/* Tmp */	12,
/* Lan */	0,
/* Zea */   12,
/* Geo */   0,
/* Dra */   18 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   2,
/* Geo */   0,
/* Dra */   2 
}
,	spell_fireproof,	TAR_OBJ_INV,	POS_STANDING,	NULL,	SLOT(523),	10,	12,	"",	"",	"$p's protective aura fades."
}
,	

{
"flamestrike",
{
/* Mag */	22,
/* Dru */	20,
/* She */	20,
/* Cle */	20,
/* Def */	20,
/* Nec */	18,
/* Dea */	22,
/* Kni */	22,
/* War */	27,
/* Wit */	22,
/* Bar */	50,
/* Ran */	22,
/* Ass */	27,
/* Mnk */	22,
/* Brd */	22,
/* Ill */	22,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(65),	20,	12,	"flamestrike",	"!Flamestrike!",	""
}
,	

{
"fly",
{
/* Mag */	10,
/* Dru */	18,
/* She */	18,
/* Cle */	18,
/* Def */	18,
/* Nec */	18,
/* Dea */	18,
/* Kni */	18,
/* War */	20,
/* Wit */	10,
/* Bar */	50,
/* Ran */	18,
/* Ass */	18,
/* Mnk */	18,
/* Brd */	18,
/* Ill */	10,
/* Tmp */	10,
/* Lan */	0,
/* Zea */   18,
/* Geo */   32,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   1,
/* Geo */   0,
/* Dra */   0 
}
,	spell_fly,	TAR_CHAR_DEFENSIVE,	POS_STANDING,	NULL,	SLOT(56),	10,	12,	"",	"You slowly float to the ground.",	""
}
,	

{
"floating disc",
{
/* Mag */	4,
/* Dru */	10,
/* She */	10,
/* Cle */	10,
/* Def */	10,
/* Nec */	10,
/* Dea */	10,
/* Kni */	10,
/* War */	16,
/* Wit */	4,
/* Bar */	50,
/* Ran */	10,
/* Ass */	16,
/* Mnk */	10,
/* Brd */	10,
/* Ill */	4,
/* Tmp */	4,
/* Lan */	0,
/* Zea */   16,
/* Geo */   30,
/* Dra */   16 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   1,
/* Geo */   0,
/* Dra */   2 
}
,	spell_floating_disc,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(522),	10,	12,	"",	"!Floating disc!",	""
}
,	

{
"frenzy",
{
/* Mag */	27,
/* Dru */	24,
/* She */	24,
/* Cle */	24,
/* Def */	24,
/* Nec */	24,
/* Dea */	24,
/* Kni */	24,
/* War */	26,
/* Wit */	27,
/* Bar */	50,
/* Ran */	24,
/* Ass */	29,
/* Mnk */	24,
/* Brd */	24,
/* Ill */	27,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_frenzy,	TAR_CHAR_DEFENSIVE,	POS_STANDING,	NULL,	SLOT(504),	30,	12,	"",	"Your rage ebbs.",	""
}
,	

{
"gate",
{
/* Mag */	26,
/* Dru */	27,
/* She */	27,
/* Cle */	27,
/* Def */	27,
/* Nec */	27,
/* Dea */	27,
/* Kni */	27,
/* War */	32,
/* Wit */	26,
/* Bar */	50,
/* Ran */	27,
/* Ass */	32,
/* Mnk */	27,
/* Brd */	27,
/* Ill */	26,
/* Tmp */	25,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_gate,	TAR_IGNORE,	POS_FIGHTING,	NULL,	SLOT(83),	40,	12,	"",	"!Gate!",	""
}
,	

{
"giant strength",
{
/* Mag */	11,
/* Dru */	17,
/* She */	17,
/* Cle */	17,
/* Def */	17,
/* Nec */	13,
/* Dea */	17,
/* Kni */	17,
/* War */	33,
/* Wit */	11,
/* Bar */	50,
/* Ran */	17,
/* Ass */	17,
/* Mnk */	17,
/* Brd */	17,
/* Ill */	11,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   40,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,	NULL,	SLOT(39),	20,	12,	"",	"You feel weaker.",	""
}
,	

{
"harm",
{
/* Mag */	31,
/* Dru */	23,
/* She */	23,
/* Cle */	23,
/* Def */	23,
/* Nec */	21,
/* Dea */	23,
/* Kni */	23,
/* War */	28,
/* Wit */	31,
/* Bar */	50,
/* Ran */	23,
/* Ass */	28,
/* Mnk */	23,
/* Brd */	23,
/* Ill */	31,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_harm,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(27),	35,	12,	"harm spell",	"!Harm!,	" ""
}
,	

{
"haste",
{
/* Mag */	21,
/* Dru */	28,
/* She */	28,
/* Cle */	28,
/* Def */	28,
/* Nec */	24,
/* Dea */	28,
/* Kni */	28,
/* War */	29,
/* Wit */	21,
/* Bar */	50,
/* Ran */	38,
/* Ass */	29,
/* Mnk */	35,
/* Brd */	38,
/* Ill */	21,
/* Tmp */	21,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_haste,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,	NULL,	SLOT(502),	30,	12,	"",	"You feel yourself slow down.",	""
}
,	

{
"heal",
{
/* Mag */	50,
/* Dru */	24,
/* She */	28,
/* Cle */	18,
/* Def */	24,
/* Nec */	28,
/* Dea */	24,
/* Kni */	35,
/* War */	33,
/* Wit */	35,
/* Bar */	50,
/* Ran */	50,
/* Ass */	33,
/* Mnk */	35,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_heal,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,	NULL,	SLOT(28),	50,	12,	"heal",	"!Heal!",	""
}
,	

{
"prism",
{
/* Mag */	50,
/* Dru */	22,
/* She */	22,
/* Cle */	46,
/* Def */	51,
/* Nec */	23,
/* Dea */	24,
/* Kni */	24,
/* War */	33,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	33,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_prism,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,	NULL, SLOT(701),	500,	12,	"prism",	"!Prism!",	""
}
,	

{
"heat metal",
{
/* Mag */	19,
/* Dru */	16,
/* She */	16,
/* Cle */	16,
/* Def */	16,
/* Nec */	16,
/* Dea */	16,
/* Kni */	16,
/* War */	23,
/* Wit */	19,
/* Bar */	50,
/* Ran */	50,
/* Ass */	23,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	19,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_heat_metal,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(516),	25,	18,	"spell",	"!Heat Metal!",	""
}
,	

{
"holy word",
{
/* Mag */	36,
/* Dru */	36,
/* She */	36,
/* Cle */	36,
/* Def */	36,
/* Nec */	36,
/* Dea */	36,
/* Kni */	36,
/* War */	50,
/* Wit */	36,
/* Bar */	50,
/* Ran */	36,
/* Ass */	36,
/* Mnk */	36,
/* Brd */	36,
/* Ill */	36,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_holy_word,	TAR_IGNORE,	POS_FIGHTING,	NULL,	SLOT(506),	70,	12,	"divine wrath",	"!Holy Word!",	""
}
,	

{
"identify",
{
/* Mag */	7,
/* Dru */	7,
/* She */	7,
/* Cle */	7,
/* Def */	7,
/* Nec */	7,
/* Dea */	7,
/* Kni */	7,
/* War */	7,
/* Wit */	7,
/* Bar */	50,
/* Ran */	7,
/* Ass */	7,
/* Mnk */	7,
/* Brd */	7,
/* Ill */	7,
/* Tmp */	7,
/* Lan */	0,
/* Zea */   7,
/* Geo */   3,
/* Dra */   7 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   1,
/* Geo */   0,
/* Dra */   2
}
,	spell_identify,	TAR_OBJ_INV,	POS_STANDING,	NULL,	SLOT(53),	12,	12,	"",	"!Identify!",	""
}
,	

{
"infravision",
{
/* Mag */	9,
/* Dru */	13,
/* She */	13,
/* Cle */	13,
/* Def */	13,
/* Nec */	13,
/* Dea */	13,
/* Kni */	13,
/* War */	16,
/* Wit */	9,
/* Bar */	50,
/* Ran */	13,
/* Ass */	16,
/* Mnk */	13,
/* Brd */	13,
/* Ill */	9,
/* Tmp */	9,
/* Lan */	0,
/* Zea */   16,
/* Geo */   7,
/* Dra */   16 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   2 
}
,	spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,	NULL,	SLOT(77),	5,	12,	"",	"You no longer see in the dark.",	""
}
,	

{
"invisibility",
{
/* Mag */	5,
/* Dru */	8,
/* She */	8,
/* Cle */	8,
/* Def */	8,
/* Nec */	8,
/* Dea */	8,
/* Kni */	8,
/* War */	9,
/* Wit */	5,
/* Bar */	50,
/* Ran */	8,
/* Ass */	13,
/* Mnk */	13,
/* Brd */	11,
/* Ill */	8,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_invis,	TAR_CHAR_SELF,	POS_STANDING, &gsn_invis, SLOT(29), 5,	12,	"",	"You are no longer invisible.",	"$p fades into view."
}
,	

{
"know level",
{
/* Mag */	9,
/* Dru */	9,
/* She */	9,
/* Cle */	9,
/* Def */	9,
/* Nec */	9,
/* Dea */	9,
/* Kni */	9,
/* War */	20,
/* Wit */	12,
/* Bar */	50,
/* Ran */	20,
/* Ass */	20,
/* Mnk */	20,
/* Brd */	12,
/* Ill */	9,
/* Tmp */	9,
/* Lan */	0,
/* Zea */   9,
/* Geo */   15,
/* Dra */   20 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   1,
/* Geo */   0,
/* Dra */   2 
}
,	spell_know_level,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,	NULL,	SLOT(58),	9,	12,	"",	"!Know Level!",	""
}
,	

{
"lightning bolt",
{
/* Mag */	13,
/* Dru */	15,
/* She */	15,
/* Cle */	23,
/* Def */	23,
/* Nec */	13,
/* Dea */	23,
/* Kni */	23,
/* War */	18,
/* Wit */	13,
/* Bar */	50,
/* Ran */	18,
/* Ass */	18,
/* Mnk */	13,
/* Brd */	13,
/* Ill */	13,
/* Tmp */	13,
/* Lan */	0,
/* Zea */   0,
/* Geo */   12,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(30),	15,	12,	"lightning bolt",	"!Lightning Bolt!",	""
}
,	

{
"locate object",
{
/* Mag */	9,
/* Dru */	15,
/* She */	15,
/* Cle */	15,
/* Def */	15,
/* Nec */	15,
/* Dea */	15,
/* Kni */	15,
/* War */	15,
/* Wit */	15,
/* Bar */	50,
/* Ran */	15,
/* Ass */	15,
/* Mnk */	15,
/* Brd */	9,
/* Ill */	9,
/* Tmp */	9,
/* Lan */	0,
/* Zea */   15,
/* Geo */   42,
/* Dra */   15 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   1,
/* Geo */   0,
/* Dra */   2 
}
,	spell_locate_object,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(31),	20,	12,	"",	"!Locate Object!",	""
}
,	

{
"magic missile",
{
/* Mag */	1,
/* Dru */	2,
/* She */	2,
/* Cle */	5,
/* Def */	5,
/* Nec */	2,
/* Dea */	5,
/* Kni */	5,
/* War */	3,
/* Wit */	2,
/* Bar */	50,
/* Ran */	5,
/* Ass */	5,
/* Mnk */	5,
/* Brd */	5,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(32),	15,	12,	"magic missile",	"!Magic Missile!",	""
}
,	

{
"mass healing",
{
/* Mag */	50,
/* Dru */	45,
/* She */	45,
/* Cle */	38,
/* Def */	45,
/* Nec */	45,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	45,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_mass_healing,	TAR_IGNORE,	POS_FIGHTING,	NULL,	SLOT(508),	100,	12,	"",	"!Mass Healing!",	""
}
,	

{
"mass invisibility",
{
/* Mag */	30,
/* Dru */	30,
/* She */	30,
/* Cle */	30,
/* Def */	30,
/* Nec */	30,
/* Dea */	30,
/* Kni */	30,
/* War */	30,
/* Wit */	30,
/* Bar */	50,
/* Ran */	30,
/* Ass */	31,
/* Mnk */	30,
/* Brd */	30,
/* Ill */	20,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_mass_invis,	TAR_IGNORE,	POS_STANDING,	&gsn_mass_invis,	SLOT(69),	20,	24,	"",	"You are no longer invisible.",	""
}
,	

{
"nexus",
{
/* Mag */	40,
/* Dru */	35,
/* She */	35,
/* Cle */	35,
/* Def */	35,
/* Nec */	35,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	40,
/* Bar */	50,
/* Ran */	40,
/* Ass */	50,
/* Mnk */	40,
/* Brd */	40,
/* Ill */	40,
/* Tmp */	43,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_nexus,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(520),	200,	12,	"",	"!Nexus!",	""
}
,	

{
"pass door",
{
/* Mag */	24,
/* Dru */	32,
/* She */	32,
/* Cle */	32,
/* Def */	32,
/* Nec */	32,
/* Dea */	35,
/* Kni */	35,
/* War */	37,
/* Wit */	24,
/* Bar */	50,
/* Ran */	35,
/* Ass */	37,
/* Mnk */	30,
/* Brd */	30,
/* Ill */	24,
/* Tmp */	20,
/* Lan */	0,
/* Zea */   35,
/* Geo */   99,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   1,
/* Geo */   0,
/* Dra */   0 
}
,	spell_pass_door,	TAR_CHAR_SELF,	POS_STANDING,	NULL,	SLOT(74),	20,	12,	"",	"You feel solid again.",	""
}
,	

{
"plague",
{
/* Mag */	23,
/* Dru */	36,
/* She */	36,
/* Cle */	36,
/* Def */	36,
/* Nec */	17,
/* Dea */	36,
/* Kni */	36,
/* War */	36,
/* Wit */	23,
/* Bar */	50,
/* Ran */	36,
/* Ass */	36,
/* Mnk */	36,
/* Brd */	36,
/* Ill */	23,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_plague,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_plague,	SLOT(503),	20,	12,	"sickness",	"Your sores vanish.",	""
}
,	

{
"tracer cloud",
{
/* Mag */	17,
/* Dru */	12,
/* She */	35,
/* Cle */	18,
/* Def */	5,
/* Nec */	12,
/* Dea */	18,
/* Kni */	18,
/* War */	21,
/* Wit */	17,
/* Bar */	50,
/* Ran */	15,
/* Ass */	21,
/* Mnk */	15,
/* Brd */	18,
/* Ill */	17,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_tracer_cloud,	TAR_CHAR_OFFENSIVE,	POS_STANDING,	NULL,	SLOT(804),	80,	12,	"",	"You no longer feel followed.",	""
}
,	

{
"poison",
{
/* Mag */	17,
/* Dru */	12,
/* She */	12,
/* Cle */	18,
/* Def */	5,
/* Nec */	12,
/* Dea */	18,
/* Kni */	18,
/* War */	21,
/* Wit */	17,
/* Bar */	50,
/* Ran */	15,
/* Ass */	21,
/* Mnk */	15,
/* Brd */	18,
/* Ill */	17,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_poison,	TAR_OBJ_CHAR_OFF,	POS_FIGHTING,	&gsn_poison,	SLOT(33),	10,	12,	"poison",	"You feel less sick.",	"The poison on $p dries up."
}
,	

{
"portal",
{
/* Mag */	35,
/* Dru */	30,
/* She */	30,
/* Cle */	30,
/* Def */	30,
/* Nec */	30,
/* Dea */	45,
/* Kni */	45,
/* War */	45,
/* Wit */	35,
/* Bar */	50,
/* Ran */	45,
/* Ass */	45,
/* Mnk */	45,
/* Brd */	45,
/* Ill */	35,
/* Tmp */	35,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_portal,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(519),	80,	12,	"",	"!Portal!",	""
}
,	

{
"protection evil",
{
/* Mag */	12,
/* Dru */	9,
/* She */	9,
/* Cle */	9,
/* Def */	9,
/* Nec */	18,
/* Dea */	9,
/* Kni */	9,
/* War */	17,
/* Wit */	12,
/* Bar */	50,
/* Ran */	17,
/* Ass */	17,
/* Mnk */	17,
/* Brd */	17,
/* Ill */	12,
/* Tmp */	12,
/* Lan */	0,
/* Zea */   17,
/* Geo */   0,
/* Dra */   17 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   2,
/* Geo */   0,
/* Dra */   2 
}
,	spell_protection_evil,	TAR_CHAR_SELF,	POS_STANDING,	NULL,	SLOT(34),	5,	12,	"",	"You feel less protected.",	""
}
,	

{
"protection good",
{
/* Mag */	12,
/* Dru */	17,
/* She */	17,
/* Cle */	17,
/* Def */	17,
/* Nec */	9,
/* Dea */	17,
/* Kni */	17,
/* War */	17,
/* Wit */	12,
/* Bar */	50,
/* Ran */	17,
/* Ass */	17,
/* Mnk */	17,
/* Brd */	17,
/* Ill */	12,
/* Tmp */	12,
/* Lan */	0,
/* Zea */   20,
/* Geo */   0,
/* Dra */   17 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   2,
/* Geo */   0,
/* Dra */   2 
}
,	spell_protection_good,	TAR_CHAR_SELF,	POS_STANDING,	NULL,	SLOT(514),	5,	12,	"",	"You feel less protected.",	""
}
,	

{
"ray of truth",
{
/* Mag */	39,
/* Dru */	35,
/* She */	35,
/* Cle */	35,
/* Def */	35,
/* Nec */	35,
/* Dea */	35,
/* Kni */	35,
/* War */	47,
/* Wit */	39,
/* Bar */	50,
/* Ran */	35,
/* Ass */	47,
/* Mnk */	35,
/* Brd */	35,
/* Ill */	39,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_ray_of_truth,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(518),	20,	12,	"ray of truth",	"!Ray of Truth!",	""
}
,	

{
"smite",
{
/* Mag */	39,
/* Dru */	35,
/* She */	35,
/* Cle */	43,
/* Def */	43,
/* Nec */	35,
/* Dea */	35,
/* Kni */	35,
/* War */	47,
/* Wit */	39,
/* Bar */	50,
/* Ran */	50,
/* Ass */	47,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	39,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_smite,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(700),	50,	12,	"smite",	"!Smite!",	""
}
,	

{
"recharge",
{
/* Mag */	9,
/* Dru */	19,
/* She */	19,
/* Cle */	19,
/* Def */	19,
/* Nec */	13,
/* Dea */	19,
/* Kni */	19,
/* War */	31,
/* Wit */	9,
/* Bar */	50,
/* Ran */	19,
/* Ass */	31,
/* Mnk */	19,
/* Brd */	19,
/* Ill */	9,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_recharge,	TAR_OBJ_INV,	POS_STANDING,	NULL,	SLOT(517),	30,	12,	"",	"!Recharge!",	""
}
,	

{
"refresh",
{
/* Mag */	8,
/* Dru */	5,
/* She */	5,
/* Cle */	5,
/* Def */	5,
/* Nec */	5,
/* Dea */	5,
/* Kni */	5,
/* War */	12,
/* Wit */	8,
/* Bar */	50,
/* Ran */	5,
/* Ass */	5,
/* Mnk */	5,
/* Brd */	5,
/* Ill */	8,
/* Tmp */	8,
/* Lan */	8,
/* Zea */   6,
/* Geo */   18,
/* Dra */   12 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	2,
/* Zea */   0,
/* Geo */   0,
/* Dra */   2 
}
,	spell_refresh,	TAR_CHAR_DEFENSIVE,	POS_STANDING,	NULL,	SLOT(81),	12,	12,	"refresh",	"!Refresh!",	""
}
,	

{
"remove curse",
{
/* Mag */	22,
/* Dru */	18,
/* She */	18,
/* Cle */	18,
/* Def */	18,
/* Nec */	18,
/* Dea */	18,
/* Kni */	18,
/* War */	22,
/* Wit */	22,
/* Bar */	50,
/* Ran */	22,
/* Ass */	22,
/* Mnk */	18,
/* Brd */	18,
/* Ill */	22,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_remove_curse,	TAR_OBJ_CHAR_DEF,	POS_STANDING,	NULL,	SLOT(35),	5,	12,	"",	"!Remove Curse!",	""
}
,	

{
"enhance aura",
{
/* Mag */	20,
/* Dru */	99,
/* She */	45,
/* Cle */	99,
/* Def */	20,
/* Nec */	20,
/* Dea */	20,
/* Kni */	20,
/* War */	0,
/* Wit */	36,
/* Bar */	50,
/* Ran */	20,
/* Ass */	42,
/* Mnk */	20,
/* Brd */	20,
/* Ill */	36,
/* Tmp */	35,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_enhance_aura,	TAR_CHAR_DEFENSIVE,	POS_STANDING,	NULL,	SLOT(1000),	40,	12,	"",	"You no longer have increased attack power.",	""
}
,	

{
"spirit blow",
{
/* Mag */	20,
/* Dru */	20,
/* She */	45,
/* Cle */	20,
/* Def */	20,
/* Nec */	20,
/* Dea */	20,
/* Kni */	20,
/* War */	42,
/* Wit */	36,
/* Bar */	50,
/* Ran */	20,
/* Ass */	42,
/* Mnk */	20,
/* Brd */	20,
/* Ill */	36,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	2,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_spirit_blow,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(1001),	45,	12,	"",	"Your spirit has recovered from the blow.",	""
}
,

{
"sanctuary",
{
/* Mag */	20,
/* Dru */	20,
/* She */	20,
/* Cle */	20,
/* Def */	20,
/* Nec */	20,
/* Dea */	20,
/* Kni */	20,
/* War */	42,
/* Wit */	36,
/* Bar */	50,
/* Ran */	20,
/* Ass */	42,
/* Mnk */	20,
/* Brd */	20,
/* Ill */	36,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,	&gsn_sanctuary,	SLOT(36),	20,	12,	"",	"The white aura around your body fades.",	""
}
,

{
"reflect",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_reflect,	TAR_CHAR_SELF,	POS_STANDING,	&gsn_reflect,	SLOT(313),	20,	12,	"",	"You feel less reflective.",	""
}
,	

{
"shield",
{
/* Mag */	35,
/* Dru */	35,
/* She */	35,
/* Cle */	35,
/* Def */	35,
/* Nec */	35,
/* Dea */	35,
/* Kni */	35,
/* War */	40,
/* Wit */	35,
/* Bar */	50,
/* Ran */	50,
/* Ass */	40,
/* Mnk */	35,
/* Brd */	35,
/* Ill */	35,
/* Tmp */	35,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_shield,	TAR_CHAR_DEFENSIVE,	POS_STANDING,	NULL,	SLOT(67),	12,	12,	"",	"Your force shield shimmers then fades away.",	""
}
,	

{
"noquit",
{0},
{0},
NULL,	TAR_IGNORE,	POS_SLEEPING,	NULL,	SLOT(0),	0,	0,	"",	"You can quit now.",	""
},	
{
"shocking grasp",
{
/* Mag */	10,
/* Dru */	13,
/* She */	13,
/* Cle */	13,
/* Def */	13,
/* Nec */	12,
/* Dea */	13,
/* Kni */	13,
/* War */	14,
/* Wit */	19,
/* Bar */	50,
/* Ran */	13,
/* Ass */	14,
/* Mnk */	13,
/* Brd */	13,
/* Ill */	10,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(54),	15,	12,	"shocking grasp",	"!Shocking Grasp!",	""
}
,	

{
"sleep",
{
/* Mag */	10,
/* Dru */	10,
/* She */	10,
/* Cle */	11,
/* Def */	11,
/* Nec */	11,
/* Dea */	11,
/* Kni */	11,
/* War */	11,
/* Wit */	10,
/* Bar */	50,
/* Ran */	11,
/* Ass */	11,
/* Mnk */	11,
/* Brd */	11,
/* Ill */	9,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_sleep,	TAR_CHAR_OFFENSIVE,	POS_STANDING,	&gsn_sleep,	SLOT(38),	15,	12,	"",	"You feel less tired.",	""
}
,	

{
"slow",
{
/* Mag */	23,
/* Dru */	30,
/* She */	30,
/* Cle */	30,
/* Def */	30,
/* Nec */	23,
/* Dea */	30,
/* Kni */	30,
/* War */	29,
/* Wit */	23,
/* Bar */	50,
/* Ran */	30,
/* Ass */	29,
/* Mnk */	30,
/* Brd */	23,
/* Ill */	23,
/* Tmp */	23,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_slow,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(515),	30,	12,	"",	"You feel yourself speed up.",	""
}
,	

{
"stone skin",
{
/* Mag */	25,
/* Dru */	40,
/* She */	40,
/* Cle */	40,
/* Def */	40,
/* Nec */	40,
/* Dea */	40,
/* Kni */	40,
/* War */	45,
/* Wit */	25,
/* Bar */	50,
/* Ran */	45,
/* Ass */	45,
/* Mnk */	40,
/* Brd */	40,
/* Ill */	25,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_stone_skin,	TAR_CHAR_SELF,	POS_STANDING,	NULL,	SLOT(66),	12,	6,	"",	"Your skin feels soft again.",	""
}
,	

{
"summon",
{
/* Mag */	13,
/* Dru */	13,
/* She */	13,
/* Cle */	13,
/* Def */	13,
/* Nec */	13,
/* Dea */	13,
/* Kni */	13,
/* War */	13,
/* Wit */	13,
/* Bar */	50,
/* Ran */	13,
/* Ass */	13,
/* Mnk */	13,
/* Brd */	99,
/* Ill */	13,
/* Tmp */	13,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	0,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_summon,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(40),	40,	12,	"",	"!Summon!",	""
}
,	

{
"teleport",
{
/* Mag */	13,
/* Dru */	22,
/* She */	22,
/* Cle */	22,
/* Def */	22,
/* Nec */	22,
/* Dea */	22,
/* Kni */	22,
/* War */	25,
/* Wit */	22,
/* Bar */	50,
/* Ran */	22,
/* Ass */	22,
/* Mnk */	22,
/* Brd */	22,
/* Ill */	13,
/* Tmp */	13,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_teleport,	TAR_CHAR_SELF,	POS_FIGHTING,	NULL,	SLOT(2),	50,	12,	"",	"!Teleport!",	""
}
,	

{
"ventriloquate",
{
/* Mag */	2,
/* Dru */	3,
/* She */	3,
/* Cle */	3,
/* Def */	3,
/* Nec */	5,
/* Dea */	3,
/* Kni */	3,
/* War */	5,
/* Wit */	2,
/* Bar */	50,
/* Ran */	3,
/* Ass */	5,
/* Mnk */	3,
/* Brd */	3,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_ventriloquate,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(41),	5,	6,	"",	"!Ventriloquate!",	""
}
,	

{
"weaken",
{
/* Mag */	11,
/* Dru */	14,
/* She */	14,
/* Cle */	14,
/* Def */	14,
/* Nec */	11,
/* Dea */	14,
/* Kni */	14,
/* War */	17,
/* Wit */	11,
/* Bar */	50,
/* Ran */	14,
/* Ass */	17,
/* Mnk */	14,
/* Brd */	14,
/* Ill */	11,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   25,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_weaken,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(68),	10,	6,	"spell",	"You feel stronger.",	""
}
,	

{
"word of recall",
{
/* Mag */	99,
/* Dru */	99,
/* She */	99,
/* Cle */	99,
/* Def */	99,
/* Nec */	99,
/* Dea */	99,
/* Kni */	99,
/* War */	99,
/* Wit */	99,
/* Bar */	99,
/* Ran */	99,
/* Ass */	99,
/* Mnk */	99,
/* Brd */	99,
/* Ill */	99,
/* Tmp */	99,
/* Lan */	99,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	-1,
/* Wlk */	-1,
/* Cle */	-1,
/* Def */	-1,
/* Nec */	-1,
/* Dea */	-1,
/* Kni */	-1,
/* War */	-1,
/* Wit */	-1,
/* Bar */	-1,
/* Ran */	-1,
/* Ass */	-1,
/* Mnk */	-1,
/* Brd */	-1,
/* Ill */	-1,
/* Tmp */	-1,
/* Lan */	-1,
/* Zea */   -1,
/* Geo */   -1,
/* Dra */   -1 
}
,	spell_word_of_recall,	TAR_CHAR_SELF,	POS_RESTING,	NULL,	SLOT(42),	200,	12,	"",	"!Word of Recall!",	""
}
,	

{
"acid breath",
{
/* Mag */	34,
/* Dru */	32,
/* She */	32,
/* Cle */	32,
/* Def */	32,
/* Nec */	32,
/* Dea */	32,
/* Kni */	32,
/* War */	34,
/* Wit */	35,
/* Bar */	50,
/* Ran */	34,
/* Ass */	34,
/* Mnk */	34,
/* Brd */	34,
/* Ill */	35,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(200),	40,	12,	"blast of acid",	"!Acid Breath!",	""
}
,	

{
"fire breath",
{
/* Mag */	40,
/* Dru */	45,
/* She */	45,
/* Cle */	45,
/* Def */	45,
/* Nec */	45,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	43,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	41,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(201),	60,	12,	"blast of flame",	"The smoke leaves your eyes.",	""
}
,	

{
"firestorm",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_firestorm,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(312),	60,	12,	"blast of FIRE",	"Your vision clears.",	""
}
,	

{
"frost breath",
{
/* Mag */	37,
/* Dru */	36,
/* She */	36,
/* Cle */	36,
/* Def */	36,
/* Nec */	36,
/* Dea */	36,
/* Kni */	36,
/* War */	40,
/* Wit */	37,
/* Bar */	50,
/* Ran */	36,
/* Ass */	40,
/* Mnk */	36,
/* Brd */	36,
/* Ill */	37,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(202),	60,	12,	"blast of frost",	"!Frost Breath!",	""
}
,	

{
"gas breath",
{
/* Mag */	30,
/* Dru */	43,
/* She */	43,
/* Cle */	43,
/* Def */	43,
/* Nec */	43,
/* Dea */	50,
/* Kni */	50,
/* War */	37,
/* Wit */	35,
/* Bar */	50,
/* Ran */	43,
/* Ass */	47,
/* Mnk */	43,
/* Brd */	43,
/* Ill */	35,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_gas_breath,	TAR_IGNORE,	POS_FIGHTING,	NULL,	SLOT(203),	50,	12,	"blast of gas",	"!Gas Breath!",	""
}
,	

{
"lightning breath",
{
/* Mag */	40,
/* Dru */	46,
/* She */	46,
/* Cle */	40,
/* Def */	40,
/* Nec */	40,
/* Dea */	46,
/* Kni */	46,
/* War */	46,
/* Wit */	41,
/* Bar */	50,
/* Ran */	50,
/* Ass */	46,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	41,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(204),	60,	12,	"blast of lightning",	"!Lightning Breath!",	""
}
,	

{
"general purpose",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_general_purpose,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(401),	0,	12,	"general purpose ammo",	"!General Purpose Ammo!",	""
}
,	

{
"high explosive",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_high_explosive,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(402),	0,	12,	"high explosive ammo",	"!High Explosive Ammo!",	""
}
,	
{
"age",
{
/* Mag */	19,
/* Dru */	22,
/* She */	22,
/* Cle */	22,
/* Def */	22,
/* Nec */	18,
/* Dea */	22,
/* Kni */	22,
/* War */	26,
/* Wit */	19,
/* Bar */	50,
/* Ran */	22,
/* Ass */	26,
/* Mnk */	22,
/* Brd */	22,
/* Ill */	19,
/* Tmp */	34,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_age,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(403),	20,	12,	"age",	"You feel youthful.",	""
}
,
{
"youth",
{
/* Mag */	19,
/* Dru */	22,
/* She */	22,
/* Cle */	22,
/* Def */	22,
/* Nec */	18,
/* Dea */	22,
/* Kni */	22,
/* War */	26,
/* Wit */	19,
/* Bar */	50,
/* Ran */	22,
/* Ass */	26,
/* Mnk */	22,
/* Brd */	22,
/* Ill */	19,
/* Tmp */	34,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_youth,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(404),	20,	12,	"youth",	"You feel older.",	""
}
,
{
"blink",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	40,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_blink,	TAR_CHAR_SELF,	POS_FIGHTING,	NULL,	SLOT(405),	50,	12,	"blink",	"!Blink!",	""
}
,	
{
"bubble",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	14,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_bubble,	TAR_CHAR_SELF,	POS_STANDING,	NULL,	SLOT(406),	50,	12,	"bubble",	"!Bubble!",	""
}
,
{
"hold",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	46,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_hold,	TAR_CHAR_DEFENSIVE,	POS_STANDING,	NULL, SLOT(407),50,	24,	"",	"You can move again.",	""
}
,

{
"lapse",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	36,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_lapse,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	NULL,	SLOT(408),	60,	12,	"",	"You feel your self slip back into the stream.",	""
}
,

{
"suspend",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	40,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_suspend,	TAR_CHAR_SELF,	POS_STANDING,	NULL,	SLOT(409),	10,	24,	"",	"You are no longer suspended.",	""
}
,

{
"temporal blade",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	30,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_temp_blade,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(410),	100,	10,	"temporal blade",	"!Temporal blade!",	""
}
,
{
"decay",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	50,
/* Lan */	50,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	2,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	1,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_decay,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(11),	250,	12,	"",	"!Decay!",	""
}
,	

{
"timestop",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	45,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_timestop,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(411),	400,	24,	"timestop",	"You can move again.",	""
}
,

{
"rift",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	37,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_rift,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(412),	80,	12,	"rift",	"",	""
}
,
//Geomancer
{
"fire golem",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_summon_fgolem,     TAR_IGNORE, POS_STANDING, NULL, SLOT(600), 250, 32,

        "",     "You gained enough mana to summon more golems now.",""
}
,

{
"lightning golem",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_summon_lgolem,     TAR_IGNORE, POS_STANDING, NULL, SLOT(600), 250, 32,

        "",     "You gained enough mana to summon more golems now.",""
}
,
//Geomancer
{
"ice golem",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_summon_igolem,     TAR_IGNORE, POS_STANDING, NULL, SLOT(600), 250, 32,

        "",     "You gained enough mana to summon more golems now.",""
}
,
//Geomancer
{
"iron golem",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_summon_Igolem,     TAR_IGNORE, POS_STANDING, NULL, SLOT(600), 250, 32,

        "",     "You gained enough mana to summon more golems now.",""
}
,
//Geomancer
{
"acid golem",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_summon_agolem,     TAR_IGNORE, POS_STANDING, NULL, SLOT(600), 250, 32,

        "",     "You gained enough mana to summon more golems now.",""
}
,
// {
// "deflective aura",
// {
// /* Mag */	0,
// /* Dru */	0,
// /* She */	0,
// /* Cle */	0,
// /* Def */	0,
// /* Nec */	0,
// /* Dea */	0,
// /* Kni */	0,
// /* War */	0,
// /* Wit */	0,
// /* Bar */	0,
// /* Ran */	0,
// /* Ass */	0,
// /* Mnk */	0,
// /* Brd */	0,
// /* Ill */	0,
// /* Tmp */	0,
// /* Lan */	0,
// /* Zea */    0,
// /* Geo */    0,
// /* Dra */    0 
// }
// ,
// {
// /* Mag */	0,
// /* Dru */	0,
// /* Wlk */	0,
// /* Cle */	0,
// /* Def */	0,
// /* Nec */	0,
// /* Dea */	0,
// /* Kni */	0,
// /* War */	0,
// /* Wit */	0,
// /* Bar */	0,
// /* Ran */	0,
// /* Ass */	0,
// /* Mnk */	0,
// /* Brd */	0,
// /* Ill */	0,
// /* Tmp */	0,
// /* Lan */	0,
// /* Zea */    0,
// /* Geo */    0,
// /* Dra */    0 
// }
// ,	spell_deflect,	TAR_CHAR_SELF,	POS_STANDING,	&gsn_deflect,	SLOT(610),	80,	12,	"",	"Your deflective aura fades away.",	""
// }
// ,

// {
// "chaotic aura",
// {
// /* Mag */	0,
// /* Dru */	0,
// /* She */	0,
// /* Cle */	0,
// /* Def */	0,
// /* Nec */	0,
// /* Dea */	0,
// /* Kni */	0,
// /* War */	0,
// /* Wit */	0,
// /* Bar */	0,
// /* Ran */	0,
// /* Ass */	0,
// /* Mnk */	0,
// /* Brd */	0,
// /* Ill */	0,
// /* Tmp */	0,
// /* Lan */	0,
// /* Zea */    0,
// /* Geo */    0,
// /* Dra */    0 
// }
// ,
// {
// /* Mag */	0,
// /* Dru */	0,
// /* Wlk */	0,
// /* Cle */	0,
// /* Def */	0,
// /* Nec */	0,
// /* Dea */	0,
// /* Kni */	0,
// /* War */	0,
// /* Wit */	0,
// /* Bar */	0,
// /* Ran */	0,
// /* Ass */	0,
// /* Mnk */	0,
// /* Brd */	0,
// /* Ill */	0,
// /* Tmp */	0,
// /* Lan */	0,
// /* Zea */    0,
// /* Geo */    0,
// /* Dra */    0 
// }
// ,	spell_chaotic,	TAR_CHAR_SELF,	POS_STANDING,	&gsn_chaotic,	SLOT(614),	80,	12,	"",	"You're no longer surrounded in chaos.",	""
// }
// ,


{
"draconian blessing",
{
/* Mag */	-1,
/* Dru */	-1,
/* She */	-1,
/* Cle */	-1,
/* Def */	-1,
/* Nec */	-1,
/* Dea */	-1,
/* Kni */	-1,
/* War */	-1,
/* Wit */	-1,
/* Bar */	-1,
/* Ran */	-1,
/* Ass */	-1,
/* Mnk */	-1,
/* Brd */	-1,
/* Ill */	-1,
/* Tmp */	-1,
/* Lan */	-1,
/* Zea */   -1,
/* Geo */   -1,
/* Dra */   40 
}
,
{
/* Mag */	-1,
/* Dru */	-1,
/* Wlk */	-1,
/* Cle */	-1,
/* Def */	-1,
/* Nec */	-1,
/* Dea */	-1,
/* Kni */	-1,
/* War */	-1,
/* Wit */	-1,
/* Bar */	-1,
/* Ran */	-1,
/* Ass */	-1,
/* Mnk */	-1,
/* Brd */	-1,
/* Ill */	-1,
/* Tmp */	-1,
/* Lan */	-1,
/* Zea */   -1,
/* Geo */   -1,
/* Dra */   2 
}
,	spell_draconian_blessing,	TAR_IGNORE,	POS_FIGHTING,	NULL,	SLOT(611),	45,	12,	"draconian blessing",	"!Draconian Blessing!",	""
}
,
{
"draconian mist",
{
/* Mag */	-1,
/* Dru */	-1,
/* Wlk */	-1,
/* Cle */	-1,
/* Def */	-1,
/* Nec */	-1,
/* Dea */	-1,
/* Kni */	-1,
/* War */	-1,
/* Wit */	-1,
/* Bar */	-1,
/* Ran */	-1,
/* Ass */	-1,
/* Mnk */	-1,
/* Brd */	-1,
/* Ill */	-1,
/* Tmp */	-1,
/* Lan */	-1,
/* Zea */   -1,
/* Geo */   -1,
/* Dra */   45 
}
,
{
/* Mag */	-1,
/* Dru */	-1,
/* Wlk */	-1,
/* Cle */	-1,
/* Def */	-1,
/* Nec */	-1,
/* Dea */	-1,
/* Kni */	-1,
/* War */	-1,
/* Wit */	-1,
/* Bar */	-1,
/* Ran */	-1,
/* Ass */	-1,
/* Mnk */	-1,
/* Brd */	-1,
/* Ill */	-1,
/* Tmp */	-1,
/* Lan */	-1,
/* Zea */   -1,
/* Geo */   -1,
/* Dra */   0 
}
,	spell_draconian_mist,	TAR_IGNORE,	POS_FIGHTING,	NULL,	SLOT(612),	40,	12,	"draconian mist",	"!Draconian Mist!",	""
}
,
{
"draconian skin",
{
/* Mag */	-1,
/* Dru */	-1,
/* Wlk */	-1,
/* Cle */	-1,
/* Def */	-1,
/* Nec */	-1,
/* Dea */	-1,
/* Kni */	-1,
/* War */	-1,
/* Wit */	-1,
/* Bar */	-1,
/* Ran */	-1,
/* Ass */	-1,
/* Mnk */	-1,
/* Brd */	-1,
/* Ill */	-1,
/* Tmp */	-1,
/* Lan */	-1,
/* Zea */   -1,
/* Geo */   -1,
/* Dra */   50 
}
,
{
/* Mag */	-1,
/* Dru */	-1,
/* Wlk */	-1,
/* Cle */	-1,
/* Def */	-1,
/* Nec */	-1,
/* Dea */	-1,
/* Kni */	-1,
/* War */	-1,
/* Wit */	-1,
/* Bar */	-1,
/* Ran */	-1,
/* Ass */	-1,
/* Mnk */	-1,
/* Brd */	-1,
/* Ill */	-1,
/* Tmp */	-1,
/* Lan */	-1,
/* Zea */   -1,
/* Geo */   -1,
/* Dra */   0 
}
,	spell_draconian_skin,	TAR_IGNORE,	POS_FIGHTING,	NULL,	SLOT(613),	35,	12,	"draconian skin",	"!Draconian Skin!",	""
}
,
{
"prismatic spray",		
{
/* Mag */	99,
/* Dru */	-1,
/* Wlk */	-1,
/* Cle */	-1,
/* Def */	-1,
/* Nec */	-1,
/* Dea */	-1,
/* Kni */	-1,
/* War */	-1,
/* Wit */	-1,
/* Bar */	-1,
/* Ran */	-1,
/* Ass */	-1,
/* Mnk */	-1,
/* Brd */	-1,
/* Ill */	-1,
/* Tmp */	-1,
/* Lan */	-1,
/* Zea */   -1,
/* Geo */   -1,
/* Dra */   -1  
}
,     
{
/* Mag */	4,
/* Dru */	-1,
/* Wlk */	-1,
/* Cle */	-1,
/* Def */	-1,
/* Nec */	-1,
/* Dea */	-1,
/* Kni */	-1,
/* War */	-1,
/* Wit */	-1,
/* Bar */	-1,
/* Ran */	-1,
/* Ass */	-1,
/* Mnk */	-1,
/* Brd */	-1,
/* Ill */	-1,
/* Tmp */	-1,
/* Lan */	-1,
/* Zea */   -1,
/* Geo */   -1,
/* Dra */   -1   
}
,
	spell_prismatic_spray,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		SLOT(901),	65,	12,
	"prismatic spray",		"!Prismatic!",	"{Rm{Ga{Bn{My {Cc{Yo{wl{Ro{Gr{Be{Rd{x",
    
}
,
{
"spell edge",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   18,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   2,
/* Geo */   0,
/* Dra */   0 
}
,	spell_zeal_blade,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(410),	100,	10,	"spell edge",	"!Spell Edge!",	""
}
,

{
"summon weapon",
{
/* Mag */	0,
/* Dru */	0,
/* She */	12,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	2,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_warlock_blade,	TAR_IGNORE,	POS_STANDING,	NULL,	SLOT(1002),	90,	10,	"summon weapon",	"!Summon Weapon!",	""
}
,	

{
"axe",
{
/* Mag */	1,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	99,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   1,
/* Geo */   1,
/* Dra */   1 

}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	3,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	3,
/* Kni */	3,
/* War */	2,
/* Wit */	0,
/* Bar */	2,
/* Ran */	3,
/* Ass */	3,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   1,
/* Geo */   1,
/* Dra */   1 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_axe,	SLOT(0),	0,	0,	"",	"!Axe!",	""
}
,	

{
"dagger",
{
/* Mag */	1,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	99,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	1,
/* Zea */   1,
/* Geo */   0,
/* Dra */   1 
}
,
{
/* Mag */	4,
/* Dru */	4,
/* Wlk */	3,
/* Cle */	4,
/* Def */	4,
/* Nec */	4,
/* Dea */	3,
/* Kni */	3,
/* War */	2,
/* Wit */	3,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	4,
/* Tmp */	4,
/* Lan */	4,
/* Zea */   1,
/* Geo */   0,
/* Dra */   1 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_dagger,	SLOT(0),	0,	0,	"",	"!Dagger!",	""
}
,	

{
"flail",
{
/* Mag */	1,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	99,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   1,
/* Geo */   1,
/* Dra */   1 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	3,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	3,
/* Kni */	3,
/* War */	2,
/* Wit */	0,
/* Bar */	2,
/* Ran */	3,
/* Ass */	3,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	0,
/* Tmp */	3,
/* Lan */	0,
/* Zea */   1,
/* Geo */   1,
/* Dra */   1 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_flail,	SLOT(0),	0,	0,	"",	"!Flail!",	""
}
,	

{
"mace",
{
/* Mag */	1,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	99,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   1,
/* Geo */   1,
/* Dra */   1 
}
,
{
/* Mag */	4,
/* Dru */	4,
/* Wlk */	3,
/* Cle */	4,
/* Def */	0,
/* Nec */	4,
/* Dea */	3,
/* Kni */	3,
/* War */	2,
/* Wit */	3,
/* Bar */	2,
/* Ran */	3,
/* Ass */	2,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	4,
/* Tmp */	4,
/* Lan */	0,
/* Zea */   1,
/* Geo */   1,
/* Dra */   1 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_mace,	SLOT(0),	0,	0,	"",	"!Mace!",	""
}
,	

{
"polearm",
{
/* Mag */	1,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	99,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	1,
/* Zea */   1,
/* Geo */   0,
/* Dra */   1 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	3,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	3,
/* Kni */	3,
/* War */	2,
/* Wit */	0,
/* Bar */	2,
/* Ran */	3,
/* Ass */	3,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	3,
/* Zea */   1,
/* Geo */   0,
/* Dra */   1 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_polearm,	SLOT(0),	0,	0,	"",	"!Polearm!",	""
}
,	

{
"shield block",
{
/* Mag */	1,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	99,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	1,
/* Zea */   0,
/* Geo */   5,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	3,
/* Cle */	0,
/* Def */	3,
/* Nec */	0,
/* Dea */	3,
/* Kni */	3,
/* War */	3,
/* Wit */	0,
/* Bar */	4,
/* Ran */	3,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	4,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   0,
/* Geo */   2,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_shield_block,	SLOT(0),	0,	0,	"",	"!Shield!",	""
}
,	

{
"spear",
{
/* Mag */	1,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	99,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	1,
/* Zea */   1,
/* Geo */   0,
/* Dra */   1 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	3,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	3,
/* Kni */	3,
/* War */	2,
/* Wit */	0,
/* Bar */	2,
/* Ran */	3,
/* Ass */	3,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	3,
/* Zea */   1,
/* Geo */   0,
/* Dra */   1 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_spear,	SLOT(0),	0,	0,	"",	"!Spear!",	""
}
,	

{
"sword",
{
/* Mag */	1,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	99,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	1,
/* Zea */   1,
/* Geo */   0,
/* Dra */   1 
}
,
{
/* Mag */	4,
/* Dru */	4,
/* Wlk */	3,
/* Cle */	4,
/* Def */	4,
/* Nec */	4,
/* Dea */	3,
/* Kni */	3,
/* War */	2,
/* Wit */	3,
/* Bar */	2,
/* Ran */	3,
/* Ass */	2,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	4,
/* Tmp */	4,
/* Lan */	3,
/* Zea */   2,
/* Geo */   0,
/* Dra */   2 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_sword,	SLOT(0),	0,	0,	"",	"!sword!",	""
}
,	

{
"whip",
{
/* Mag */	1,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	99,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   1,
/* Geo */   0,
/* Dra */   1 
}
,
{
/* Mag */	4,
/* Dru */	4,
/* Wlk */	3,
/* Cle */	4,
/* Def */	0,
/* Nec */	4,
/* Dea */	3,
/* Kni */	3,
/* War */	2,
/* Wit */	3,
/* Bar */	2,
/* Ran */	3,
/* Ass */	2,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	4,
/* Tmp */	4,
/* Lan */	0,
/* Zea */   1,
/* Geo */   0,
/* Dra */   1 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_whip,	SLOT(0),	0,	0,	"",	"!Whip!",	""
}
,	

{
"backstab",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	1,
/* Mnk */	20,
/* Brd */	20,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	4,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_STANDING,	&gsn_backstab,	SLOT(0),	0,	18,	"backstab",	"!Backstab!",	""
}
,	

{
"circle",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	12,
/* Mnk */	20,
/* Brd */	30,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	4,
/* Mnk */	0,
/* Brd */	4,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_STANDING,	&gsn_circle,	SLOT(0),	0,	12,	"circle",	"!Circle!",	""
}
,	
{
"jump",
{
/* Mag */	0,
/* Dru */   0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   40 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   6 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_STANDING,	&gsn_jump,	SLOT(0),	0,	18,	"jump",	"You land deftly.",	""
}
,
{
"weapon break",
{
/* Mag */	0,
/* Dru */   0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	18,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	2,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_STANDING,	&gsn_weapon_break,	SLOT(0),	0,	12,	"weapon break",	"!Weapon Break!",	""
}
,	
{
"tear",
{
/* Mag */	0,
/* Dru */   0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	26,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	4,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_STANDING,	&gsn_tear,	SLOT(0),	0,	12,	"tear",	"!Tear!",	""
}
,	
{
"skewer",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	12,
/* Mnk */	20,
/* Brd */	30,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	14,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_STANDING,	&gsn_skewer,	SLOT(0),	0,	18,	"skewer",	"!Skewer!",	""
}
,	

{
"throw",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	12,
/* Mnk */	20,
/* Brd */	30,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	17,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	3,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_throw,	SLOT(0),	0,	24,	"throw",	"!Throw!",	""
}
,	


{
"bash",
{
/* Mag */	50,
/* Dru */	50,
/* She */	1,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	50,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	1,
/* Brd */	1,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	1,
/* Zea */   1,
/* Geo */   35,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	4,
/* Kni */	4,
/* War */	4,
/* Wit */	0,
/* Bar */	4,
/* Ran */	4,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   4,
/* Geo */   5,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_bash,
SLOT(0),	0,	16,	"bash",	"!Bash!",	""
}
,

{
"shield bash",
{
/* Mag */	50,
/* Dru */	50,
/* She */	1,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	50,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	1,
/* Brd */	1,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	18,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	3,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_shieldbash,
SLOT(0),	0,	18,	"shieldbash",	"!Shield Bash!",	""
}
,


{
"step",
{
/* Mag */	50,
/* Dru */	50,
/* She */	1,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	50,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	1,
/* Brd */	1,
/* Ill */	50,
/* Tmp */	33,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	6,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_STANDING,	&gsn_step,	SLOT(0),	0,	12,	"step",	"!Step!",	""
}
,

{
"charge",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	40,
/* Kni */	40,
/* War */	99,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	4,
/* Kni */	4,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_charge,	SLOT(0),	0,	12,	"charge",	"!Charge!",	""
}
,	

{
"rush",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	40,
/* Kni */	40,
/* War */	48,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	45,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	3,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_rush,	SLOT(0),	0,	24,	"rush",	"!Rush!",	""
}
,	


{
"berserk",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	1,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	4,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_berserk,	SLOT(0),	0,	24,	"",	"You feel your pulse slow down.",	""
}
,	

{
"dirt kicking",
{
/* Mag */	50,
/* Dru */	50,
/* She */	8,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	8,
/* Kni */	8,
/* War */	3,
/* Wit */	28,
/* Bar */	3,
/* Ran */	4,
/* Ass */	3,
/* Mnk */	3,
/* Brd */	3,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	3,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	4,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	4,
/* Wit */	0,
/* Bar */	6,
/* Ran */	2,
/* Ass */	4,
/* Mnk */	4,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	2,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_dirt,	SLOT(0),	0,	24,	"kicked dirt",	"You rub the dirt out of your eyes.",	""
}
,	

{
"braver",
{
/* Mag */	50,
/* Dru */	50,
/* She */	8,
/* Cle */	50,
/* Def */	10,
/* Nec */	50,
/* Dea */	8,
/* Kni */	8,
/* War */	3,
/* Wit */	28,
/* Bar */	3,
/* Ran */	4,
/* Ass */	3,
/* Mnk */	3,
/* Brd */	3,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	3,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	1,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_braver,	SLOT(0),	0,	12,	"braver",	"!Braver!",	""
}
,

{
"cross slash",
{
/* Mag */	50,
/* Dru */	50,
/* She */	8,
/* Cle */	50,
/* Def */	10,
/* Nec */	50,
/* Dea */	8,
/* Kni */	8,
/* War */	3,
/* Wit */	28,
/* Bar */	3,
/* Ran */	4,
/* Ass */	3,
/* Mnk */	3,
/* Brd */	3,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	3,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	1,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_cross_slash,	SLOT(0),	0,	12,	"cross slash",	"!Braver!",	""
}
,

{
"blade beam",
{
/* Mag */	50,
/* Dru */	50,
/* She */	8,
/* Cle */	50,
/* Def */	16,
/* Nec */	50,
/* Dea */	8,
/* Kni */	8,
/* War */	3,
/* Wit */	28,
/* Bar */	3,
/* Ran */	4,
/* Ass */	3,
/* Mnk */	3,
/* Brd */	3,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	3,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	1,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_blade_beam,	SLOT(0),	0,	12,	"blade beam",	"!Braver!",	""
}
,

{
"climhazzard",
{
/* Mag */	50,
/* Dru */	50,
/* She */	8,
/* Cle */	50,
/* Def */	16,
/* Nec */	50,
/* Dea */	8,
/* Kni */	8,
/* War */	3,
/* Wit */	28,
/* Bar */	3,
/* Ran */	4,
/* Ass */	3,
/* Mnk */	3,
/* Brd */	3,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	3,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	1,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_climhazzard,	SLOT(0),	0,	12,	"climhazzard",	"!Braver!",	""
}
,

{
"meteorain",
{
/* Mag */	50,
/* Dru */	50,
/* She */	8,
/* Cle */	50,
/* Def */	28,
/* Nec */	50,
/* Dea */	8,
/* Kni */	8,
/* War */	3,
/* Wit */	28,
/* Bar */	3,
/* Ran */	4,
/* Ass */	3,
/* Mnk */	3,
/* Brd */	3,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	3,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	1,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_meteorain,	SLOT(0),	0,	12,	"meteorain",	"!Braver!",	""
}
,

{
"finishing touch",
{
/* Mag */	50,
/* Dru */	50,
/* She */	8,
/* Cle */	50,
/* Def */	28,
/* Nec */	50,
/* Dea */	8,
/* Kni */	8,
/* War */	3,
/* Wit */	28,
/* Bar */	3,
/* Ran */	4,
/* Ass */	3,
/* Mnk */	3,
/* Brd */	3,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	3,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	1,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_finishing_touch,	SLOT(0),	0,	12,	"finishing touch",	"!Braver!",	""
}
,

{
"omnislash",
{
/* Mag */	50,
/* Dru */	50,
/* She */	8,
/* Cle */	50,
/* Def */	35,
/* Nec */	50,
/* Dea */	8,
/* Kni */	8,
/* War */	3,
/* Wit */	28,
/* Bar */	3,
/* Ran */	4,
/* Ass */	3,
/* Mnk */	3,
/* Brd */	3,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	3,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	1,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_omnislash,	SLOT(0),	0,	12,	"omnislash",	"!Braver!",	""
}
,

{
"disarm",
{
/* Mag */	50,
/* Dru */	50,
/* She */	15,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	10,
/* Kni */	10,
/* War */	10,
/* Wit */	17,
/* Bar */	9,
/* Ran */	13,
/* Ass */	12,
/* Mnk */	13,
/* Brd */	15,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	10,
/* Zea */   10,
/* Geo */   0,
/* Dra */   10
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	4,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	4,
/* Kni */	4,
/* War */	4,
/* Wit */	0,
/* Bar */	4,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	2,
/* Zea */   4,
/* Geo */   0,
/* Dra */   4 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_disarm,	SLOT(0),	0,	24,	"",	"!Disarm!",	""
}
,	

{
"dodge",
{
/* Mag */	20,
/* Dru */	22,
/* She */	22,
/* Cle */	22,
/* Def */	22,
/* Nec */	22,
/* Dea */	13,
/* Kni */	13,
/* War */	13,
/* Wit */	15,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	1,
/* Brd */	5,
/* Ill */	20,
/* Tmp */	20,
/* Lan */	13,
/* Zea */   11,
/* Geo */   20,
/* Dra */   15 
}
,
{
/* Mag */	6,
/* Dru */	4,
/* Wlk */	2,
/* Cle */	4,
/* Def */	4,
/* Nec */	4,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	2,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	6,
/* Tmp */	6,
/* Lan */	4,
/* Zea */   2,
/* Geo */   2,
/* Dra */   2 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_dodge,	SLOT(0),	0,	0,	"",	"!Dodge!",	""
}
,	

{
"enhanced damage",
{
/* Mag */	50,
/* Dru */	50,
/* She */	15,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	8,
/* Kni */	8,
/* War */	1,
/* Wit */	33,
/* Bar */	1,
/* Ran */	15,
/* Ass */	15,
/* Mnk */	13,
/* Brd */	16,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   5,
/* Dra */   8 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	4,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	4,
/* Kni */	4,
/* War */	4,
/* Wit */	0,
/* Bar */	3,
/* Ran */	4,
/* Ass */	4,
/* Mnk */	3,
/* Brd */	4,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   3,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_enhanced_damage,	SLOT(0),	0,	0,	"",	"!Enhanced Damage!",	""
}
,	

{
"brutal damage",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	20,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	41,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	30,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	21,
/* Zea */   0,
/* Geo */   35,
/* Dra */   35 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	5,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	8,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	8,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   0,
/* Geo */   6,
/* Dra */   6 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_brutal_damage,	SLOT(0),	0,	0,	"",	"!Brutal Damage!",	""
}
,	

{
"blind fighting",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	24,
/* Wit */	50,
/* Bar */	50,
/* Ran */	30,
/* Ass */	50,
/* Mnk */	30,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	30,
/* Zea */   0,
/* Geo */   0,
/* Dra */   30 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	6,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	6,
/* Brd */	2,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   0,
/* Geo */   0,
/* Dra */   4 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_blind_fighting,	SLOT(0),	0,	0,	"",	"!Blind Fighting!",	""
}
,	

{
"envenom",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	12,
/* Ass */	10,
/* Mnk */	12,
/* Brd */	15,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_RESTING,	&gsn_envenom,	SLOT(0),	0,	36,	"",	"!Envenom!",	""
}
,	

{
"hand to hand",
{
/* Mag */	50,
/* Dru */	50,
/* She */	6,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	9,
/* Kni */	9,
/* War */	6,
/* Wit */	30,
/* Bar */	6,
/* Ran */	6,
/* Ass */	50,
/* Mnk */	1,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   20 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	4,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	4,
/* Kni */	4,
/* War */	4,
/* Wit */	0,
/* Bar */	2,
/* Ran */	2,
/* Ass */	0,
/* Mnk */	2,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   2 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_hand_to_hand,	SLOT(0),	0,	0,	"",	"!Hand to Hand!",	""
}
,	

{
"ki power",
{
/* Mag */	6,
/* Dru */	6,
/* She */	6,
/* Cle */	6,
/* Def */	6,
/* Nec */	6,
/* Dea */	12,
/* Kni */	12,
/* War */	6,
/* Wit */	8,
/* Bar */	8,
/* Ran */	8,
/* Ass */	8,
/* Mnk */	5,
/* Brd */	5,
/* Ill */	6,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	2,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_SLEEPING,	&gsn_ki_power,
SLOT(0),	0,	0,	"",	"",	""
}
,

{
"ground",
{
/* Mag */	6,
/* Dru */	6,
/* She */	6,
/* Cle */	6,
/* Def */	6,
/* Nec */	6,
/* Dea */	12,
/* Kni */	12,
/* War */	6,
/* Wit */	8,
/* Bar */	8,
/* Ran */	8,
/* Ass */	8,
/* Mnk */	5,
/* Brd */	5,
/* Ill */	6,
/* Tmp */	0,
/* Lan */	35,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	2,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_STANDING, &gsn_ground, SLOT(0),	0,	24,	"ground",	"!Ground!",	""
}
,


{
"kick",
{
/* Mag */	50,
/* Dru */	50,
/* She */	10,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	8,
/* Kni */	8,
/* War */	8,
/* Wit */	16,
/* Bar */	1,
/* Ran */	8,
/* Ass */	14,
/* Mnk */	5,
/* Brd */	15,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	8,
/* Zea */   15,
/* Geo */   0,
/* Dra */   15 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	2,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	0,
/* Bar */	2,
/* Ran */	2,
/* Ass */	4,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   2,
/* Geo */   0,
/* Dra */   2 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_kick,	SLOT(0),	0,	12,	"kick",	"!Kick!",	""
}
,	

{
"life song",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_SELF,	POS_FIGHTING,	&gsn_life_song,	SLOT(0),	0,	12,	"life song",	"You stop singing.",	""
}
,	

{
"magic song",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	30,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_SELF,	POS_FIGHTING,	&gsn_magic_song,	SLOT(0),	0,	12,	"magic song",	"You stop singing.",	""
}
,	

{
"speed song",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_SELF,	POS_FIGHTING,	&gsn_speed_song,	SLOT(0),	0,	12,	"speed song",	"You stop singing.",	""
}
,	

{
"attack song",
{
/* Mag */	99,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	99,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   99,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_SELF,	POS_FIGHTING,	&gsn_attack_song,	SLOT(0),	0,	12,	"attack song",	"You stop singing.",	""
}
,	

{
"defense song",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_SELF,	POS_FIGHTING,	&gsn_defense_song,	SLOT(0),	0,	12,	"defense song",	"You stop singing.",	""
}
,	

{
"learning song",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	3,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_SELF,	POS_FIGHTING,	&gsn_learning_song,	SLOT(0),	0,	12,	"learning song",	"You stop singing.",	""
}
,	

{
"geomancy",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   4,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   4,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_geomancy,	SLOT(0),	0,	12,	"geomancy",	"The Earth's power leaves you.",	""
}
,	

{
"vine counter",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_vine_counter,	SLOT(0),	0,	12,	"vine counter",	"The trailing vines leave you.",	""
}
,

{
"stone protect",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_stone_protect,	SLOT(0),	0,	12,	"stone protection",	"The stone protection has left you.",	""
}
,

{
"lava shield",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_lava_shield,	SLOT(0),	0,	12,	"lava shield",	"The gout of lava surrounding you splatters out.",	""
}
,

{
"frost shield",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_frost_shield,	SLOT(0),	0,	12,	"frost shield",	"The breath of frost leaves you.",	""
}
,

{
"sung defense",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_sung_defense,	SLOT(0),	0,	12,	"sung defense",	"The protective song around you is tuned out.",	""
}
,


{
"earthlore",
{
/* Mag */	0,
/* Dru */	0,
/* She */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   4,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   4,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_earthlore,	SLOT(0),	0,	12,	"earthlore",	"The Earth's protection leaves you.",	""
}
,	

{
"strike",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	8,
/* Kni */	8,
/* War */	8,
/* Wit */	16,
/* Bar */	1,
/* Ran */	50,
/* Ass */	14,
/* Mnk */	20,
/* Brd */	15,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   30,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	4,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   4,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_strike,	SLOT(0),	0,	12,	"strike",	"!Strike!",	""
}
,	

{
"parry",
{
/* Mag */	50,
/* Dru */	50,
/* She */	1,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	13,
/* Bar */	1,
/* Ran */	1,
/* Ass */	13,
/* Mnk */	52,
/* Brd */	18,
/* Ill */	50,
/* Tmp */	1,
/* Lan */	1,
/* Zea */   13,
/* Geo */   20,
/* Dra */   13 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	4,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	4,
/* Kni */	4,
/* War */	4,
/* Wit */	0,
/* Bar */	4,
/* Ran */	4,
/* Ass */	4,
/* Mnk */	0,
/* Brd */	4,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   4,
/* Geo */   4,
/* Dra */   4 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_parry,	SLOT(0),	0,	0,	"",	"!Parry!",	""
}
,	

{
"fencing",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	45,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	42,
/* Wit */	50,
/* Bar */	50,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   42,
/* Geo */   0,
/* Dra */   50 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	8,
/* Nec */	0,
/* Dea */	6,
/* Kni */	6,
/* War */	6,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   6,
/* Geo */   0,
/* Dra */   8 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_fencing,	SLOT(0),	0,	0,	"",	"!Fencing!",	""
}
,	

{
"rescue",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	1,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	4,
/* Kni */	4,
/* War */	4,
/* Wit */	0,
/* Bar */	4,
/* Ran */	6,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_rescue,	SLOT(0),	0,	12,	"",	"!Rescue!",	""
}
,	

{
"trip",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	3,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	1,
/* Wit */	50,
/* Bar */	1,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	50,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	50,
/* Lan */	50,
/* Zea */   50,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	3,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	2,
/* Wit */	0,
/* Bar */	2,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   2,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,	&gsn_trip,	SLOT(0),	0,	24,	"trip",	"!Trip!",	""
}
,	

{
"second attack",
{
/* Mag */	40,
/* Dru */	24,
/* She */	18,
/* Cle */	24,
/* Def */	4,
/* Nec */	24,
/* Dea */	8,
/* Kni */	8,
/* War */	5,
/* Wit */	22,
/* Bar */	3,
/* Ran */	12,
/* Ass */	12,
/* Mnk */	2,
/* Brd */	14,
/* Ill */	40,
/* Tmp */	40,
/* Lan */	5,
/* Zea */   12,
/* Geo */   10,
/* Dra */   8 
}
,
{
/* Mag */	8,
/* Dru */	8,
/* Wlk */	2,
/* Cle */	8,
/* Def */	4,
/* Nec */	6,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	4,
/* Bar */	2,
/* Ran */	2,
/* Ass */	4,
/* Mnk */	2,
/* Brd */	4,
/* Ill */	6,
/* Tmp */	6,
/* Lan */	4,
/* Zea */   0,
/* Geo */   3,
/* Dra */   4 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_second_attack,	SLOT(0),	0,	0,	"",	"!Second Attack!",	""
}
,	

{
"third attack",
{
/* Mag */	50,
/* Dru */	50,
/* She */	24,
/* Cle */	45,
/* Def */	9,
/* Nec */	50,
/* Dea */	13,
/* Kni */	13,
/* War */	15,
/* Wit */	33,
/* Bar */	8,
/* Ran */	25,
/* Ass */	30,
/* Mnk */	12,
/* Brd */	25,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	15,
/* Zea */   25,
/* Geo */   0,
/* Dra */   15 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	4,
/* Cle */	0,
/* Def */	4,
/* Nec */	0,
/* Dea */	5,
/* Kni */	5,
/* War */	4,
/* Wit */	0,
/* Bar */	4,
/* Ran */	4,
/* Ass */	4,
/* Mnk */	4,
/* Brd */	4,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   0,
/* Geo */   0,
/* Dra */   4 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_third_attack,	SLOT(0),	0,	0,	"",	"!Third Attack!",	""
}
,	

{
"fourth attack",
{
/* Mag */	50,
/* Dru */	50,
/* She */	46,
/* Cle */	50,
/* Def */	30,
/* Nec */	50,
/* Dea */	40,
/* Kni */	40,
/* War */	35,
/* Wit */	53,
/* Bar */	22,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	30,
/* Brd */	45,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	34,
/* Zea */   35,
/* Geo */   0,
/* Dra */   45 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	6,
/* Cle */	0,
/* Def */	5,
/* Nec */	0,
/* Dea */	6,
/* Kni */	6,
/* War */	4,
/* Wit */	0,
/* Bar */	4,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	6,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   0,
/* Geo */   0,
/* Dra */   6 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_fourth_attack,	SLOT(0),	0,	0,	"",	"!Fourth Attack!",	""
}
,	

{
"fifth attack",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	45,
/* Wit */	50,
/* Bar */	40,
/* Ran */	50,
/* Ass */	50,
/* Mnk */	40,
/* Brd */	50,
/* Ill */	50,
/* Tmp */	50,
/* Lan */	50,
/* Zea */   45,
/* Geo */   50,
/* Dra */   50 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	5,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	4,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   4,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_fifth_attack,	SLOT(0),	0,	0,	"",	"!Fifth Attack!",	""
}
,	

{
"dual wield",
{
/* Mag */	50,
/* Dru */	50,
/* She */	13,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	13,
/* Kni */	13,
/* War */	35,
/* Wit */	21,
/* Bar */	8,
/* Ran */	13,
/* Ass */	24,
/* Mnk */	50,
/* Brd */	13,
/* Ill */	50,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   24,
/* Geo */   30,
/* Dra */   30 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	4,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	4,
/* Kni */	4,
/* War */	0,
/* Wit */	0,
/* Bar */	3,
/* Ran */	4,
/* Ass */	4,
/* Mnk */	0,
/* Brd */	4,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   4,
/* Geo */   0,
/* Dra */   3 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_dual_wield,	SLOT(0),	0,	0,	"",	"!Dual Wield!",	""
}
,	

{
"stun",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	45,
/* Ass */	40,
/* Mnk */	50,
/* Brd */	45,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	4,
/* Mnk */	4,
/* Brd */	4,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_stun,	SLOT(0),	0,	24,	"",	"You are no longer stunned!",	""
}
,	

{
"trap",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	50,
/* Ran */	30,
/* Ass */	20,
/* Mnk */	30,
/* Brd */	30,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	4,
/* Ass */	4,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_trap,	SLOT(0),	0,	24,	"",	"You break free from the net.",	""
}
,	

{
"detect trap",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	50,
/* Wit */	50,
/* Bar */	20,
/* Ran */	15,
/* Ass */	15,
/* Mnk */	99,
/* Brd */	20,
/* Ill */	50,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   33,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	4,
/* Ass */	4,
/* Mnk */	0,
/* Brd */	6,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   6,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_detect_trap,	SLOT(0),	0,	24,	"",	"!detect trap!",	""
}
,	

{
"fast healing",
{
/* Mag */	16,
/* Dru */	9,
/* She */	9,
/* Cle */	9,
/* Def */	9,
/* Nec */	9,
/* Dea */	7,
/* Kni */	7,
/* War */	6,
/* Wit */	14,
/* Bar */	2,
/* Ran */	2,
/* Ass */	14,
/* Mnk */	2,
/* Brd */	5,
/* Ill */	16,
/* Tmp */	16,
/* Lan */	6,
/* Zea */   2,
/* Geo */   25,
/* Dra */   5 
}
,
{
/* Mag */	3,
/* Dru */	5,
/* Wlk */	3,
/* Cle */	5,
/* Def */	3,
/* Nec */	3,
/* Dea */	3,
/* Kni */	3,
/* War */	2,
/* Wit */	3,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	3,
/* Tmp */	3,
/* Lan */	3,
/* Zea */   2,
/* Geo */   2,
/* Dra */   2 
}
,	spell_null,	TAR_IGNORE,	POS_SLEEPING,	&gsn_fast_healing,	SLOT(0),	0,	0,	"",	"!Fast Healing!",	""
}
,	
{
"nature walk",
{
/* Mag */	90,
/* Dru */	90,
/* She */	90,
/* Cle */	90,
/* Def */	90,
/* Nec */	90,
/* Dea */	70,
/* Kni */	70,
/* War */	60,
/* Wit */	90,
/* Bar */	90,
/* Ran */	90,
/* Ass */	90,
/* Mnk */	90,
/* Brd */	90,
/* Ill */	90,
/* Tmp */	90,
/* Lan */	90,
/* Zea */   90,
/* Geo */   5,
/* Dra */   90 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   4,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_STANDING,	&gsn_nature_walk,	SLOT(0),	0,	0,	"",	"!Nature Walk!",	""
}
,	

{
"haggle",
{
/* Mag */	1,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	1,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	1,
/* Zea */   1,
/* Geo */   1,
/* Dra */   1 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	1,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_RESTING,	&gsn_haggle,	SLOT(0),	0,	0,	"",	"!Haggle!",	""
}
,	

{
"hunt",
{
/* Mag */	35,
/* Dru */	35,
/* She */	35,
/* Cle */	35,
/* Def */	35,
/* Nec */	35,
/* Dea */	35,
/* Kni */	35,
/* War */	35,
/* Wit */	35,
/* Bar */	14,
/* Ran */	14,
/* Ass */	14,
/* Mnk */	14,
/* Brd */	14,
/* Ill */	35,
/* Tmp */	35,
/* Lan */	34,
/* Zea */   35,
/* Geo */   35,
/* Dra */   35 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	3,
/* Ran */	3,
/* Ass */	3,
/* Mnk */	3,
/* Brd */	4,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   3,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_RESTING,	&gsn_hunt,	SLOT(0),	0,	12,	"",	"!Hunt!",	""
}
,	

{
"hide",
{
/* Mag */	31,
/* Dru */	31,
/* She */	31,
/* Cle */	31,
/* Def */	31,
/* Nec */	31,
/* Dea */	31,
/* Kni */	31,
/* War */	31,
/* Wit */	31,
/* Bar */	31,
/* Ran */	9,
/* Ass */	1,
/* Mnk */	5,
/* Brd */	9,
/* Ill */	31,
/* Tmp */	30,
/* Lan */	30,
/* Zea */   30,
/* Geo */   30,
/* Dra */   30 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	6,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	6,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_RESTING,	&gsn_hide,	SLOT(0),	0,	12,	"",	"!Hide!",	""
}
,	

{
"struggle",
{
/* Mag */	12,
/* Dru */	12,
/* She */	12,
/* Cle */	12,
/* Def */	12,
/* Nec */	12,
/* Dea */	12,
/* Kni */	12,
/* War */	12,
/* Wit */	12,
/* Bar */	12,
/* Ran */	12,
/* Ass */	12,
/* Mnk */	12,
/* Brd */	12,
/* Ill */	12,
/* Tmp */	12,
/* Lan */	12,
/* Zea */   12,
/* Geo */   12,
/* Dra */   12 
}
,
{
/* Mag */	6,
/* Dru */	6,
/* Wlk */	4,
/* Cle */	6,
/* Def */	4,
/* Nec */	6,
/* Dea */	5,
/* Kni */	5,
/* War */	4,
/* Wit */	6,
/* Bar */	4,
/* Ran */	4,
/* Ass */	6,
/* Mnk */	6,
/* Brd */	6,
/* Ill */	6,
/* Tmp */	6,
/* Lan */	6,
/* Zea */   6,
/* Geo */   6,
/* Dra */   6 
}
,	spell_null,	TAR_IGNORE,	POS_STANDING,	&gsn_struggle,	SLOT(0),	0,	36,	"",	"!Struggle!",	""
}
,	

{
"scout",
{
/* Mag */	12,
/* Dru */	12,
/* She */	12,
/* Cle */	12,
/* Def */	12,
/* Nec */	12,
/* Dea */	12,
/* Kni */	12,
/* War */	12,
/* Wit */	12,
/* Bar */	12,
/* Ran */	12,
/* Ass */	12,
/* Mnk */	12,
/* Brd */	12,
/* Ill */	12,
/* Tmp */	0,
/* Lan */	12,
/* Zea */   0,
/* Geo */   0,
/* Dra */   12 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	2,
/* Nec */	0,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	0,
/* Bar */	2,
/* Ran */	2,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	2,
/* Zea */   0,
/* Geo */   0,
/* Dra */   2 
}
,	spell_null,	TAR_IGNORE,	POS_STANDING,	&gsn_scout,	SLOT(0),	0,	1,	"",	"!Scout!",	""
}
,	

{
"lore",
{
/* Mag */	12,
/* Dru */	12,
/* She */	12,
/* Cle */	12,
/* Def */	12,
/* Nec */	12,
/* Dea */	12,
/* Kni */	12,
/* War */	12,
/* Wit */	12,
/* Bar */	12,
/* Ran */	12,
/* Ass */	12,
/* Mnk */	12,
/* Brd */	12,
/* Ill */	12,
/* Tmp */	0,
/* Lan */	12,
/* Zea */   0,
/* Geo */   0,
/* Dra */   12 
}
,
{
/* Mag */	1,
/* Dru */	0,
/* Wlk */	1,
/* Cle */	0,
/* Def */	1,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	1,
/* Bar */	1,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	1,
/* Zea */   0,
/* Geo */   0,
/* Dra */   1 
}
,	spell_null,	TAR_OBJ_INV,	POS_RESTING,	&gsn_lore,	SLOT(0),	0,	1,	"",	"!Lore!",	""
}
,	

{
"focus",
{
/* Mag */	29,
/* Dru */	29,
/* She */	29,
/* Cle */	29,
/* Def */	29,
/* Nec */	29,
/* Dea */	29,
/* Kni */	29,
/* War */	0,
/* Wit */	29,
/* Bar */	29,
/* Ran */	29,
/* Ass */	29,
/* Mnk */	29,
/* Brd */	29,
/* Ill */	29,
/* Tmp */	29,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	1,
/* Dru */	1,
/* Wlk */	0,
/* Cle */	1,
/* Def */	0,
/* Nec */	1,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	1,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_RESTING,	&gsn_focus,	SLOT(0),	0,	1,	"",	"!Focus!",	""
}
,	

{
"disrupt",
{
/* Mag */	32,
/* Dru */	32,
/* She */	32,
/* Cle */	32,
/* Def */	32,
/* Nec */	32,
/* Dea */	32,
/* Kni */	32,
/* War */	32,
/* Wit */	32,
/* Bar */	32,
/* Ran */	32,
/* Ass */	32,
/* Mnk */	32,
/* Brd */	32,
/* Ill */	32,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	3,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_FIGHTING,	&gsn_disrupt,	SLOT(0),	0,	1,	"",	"!Disrupt!",	""
}
,	

{
"bluff",
{
/* Mag */	25,
/* Dru */	25,
/* She */	25,
/* Cle */	25,
/* Def */	25,
/* Nec */	25,
/* Dea */	25,
/* Kni */	25,
/* War */	25,
/* Wit */	25,
/* Bar */	25,
/* Ran */	25,
/* Ass */	25,
/* Mnk */	25,
/* Brd */	25,
/* Ill */	25,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   25,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	4,
/* Mnk */	4,
/* Brd */	7,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   6,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_RESTING,	&gsn_bluff,	SLOT(0),	0,	36,	"",	"!Bluff!",	""
}
,	

{
"chase",
{
/* Mag */	34,
/* Dru */	34,
/* She */	34,
/* Cle */	34,
/* Def */	34,
/* Nec */	34,
/* Dea */	34,
/* Kni */	34,
/* War */	34,
/* Wit */	34,
/* Bar */	34,
/* Ran */	34,
/* Ass */	34,
/* Mnk */	34,
/* Brd */	34,
/* Ill */	34,
/* Tmp */	34,
/* Lan */	33,
/* Zea */   34,
/* Geo */   34,
/* Dra */   34 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	3,
/* Ran */	3,
/* Ass */	3,
/* Mnk */	3,
/* Brd */	4,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   4,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_RESTING,	&gsn_chase,	SLOT(0),	0,	36,	"",	"!Chase!",	""
}
,	

{
"mend",
{
/* Mag */	32,
/* Dru */	32,
/* She */	32,
/* Cle */	32,
/* Def */	32,
/* Nec */	32,
/* Dea */	32,
/* Kni */	32,
/* War */	32,
/* Wit */	32,
/* Bar */	32,
/* Ran */	32,
/* Ass */	32,
/* Mnk */	32,
/* Brd */	32,
/* Ill */	32,
/* Tmp */	0,
/* Lan */	32,
/* Zea */   0,
/* Geo */   25,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	3,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	3,
/* Kni */	3,
/* War */	2,
/* Wit */	0,
/* Bar */	2,
/* Ran */	2,
/* Ass */	3,
/* Mnk */	4,
/* Brd */	4,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   0,
/* Geo */   3,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_RESTING,	&gsn_mend,	SLOT(0),	0,	36,	"",	"!mend!",	""
}
,	

{
"push",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	30,
/* Wit */	50,
/* Bar */	30,
/* Ran */	50,
/* Ass */	42,
/* Mnk */	43,
/* Brd */	49,
/* Ill */	50,
/* Tmp */	50,
/* Lan */	30,
/* Zea */   50,
/* Geo */   50,
/* Dra */   50 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	4,
/* Wit */	0,
/* Bar */	4,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_RESTING,	&gsn_push,	SLOT(0),	0,	36,	"",	"!Push!",	""
}
,	

{
"drag",
{
/* Mag */	50,
/* Dru */	50,
/* She */	50,
/* Cle */	50,
/* Def */	50,
/* Nec */	50,
/* Dea */	50,
/* Kni */	50,
/* War */	30,
/* Wit */	50,
/* Bar */	30,
/* Ran */	50,
/* Ass */	42,
/* Mnk */	43,
/* Brd */	49,
/* Ill */	50,
/* Tmp */	50,
/* Lan */	30,
/* Zea */   50,
/* Geo */   50,
/* Dra */   50 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	4,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	4,
/* Wit */	0,
/* Bar */	4,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	4,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_RESTING,	&gsn_drag,	SLOT(0),	0,	36,	"",	"!Drag!",	""
}
,	
{
"meditation",
{
/* Mag */	6,
/* Dru */	6,
/* She */	6,
/* Cle */	6,
/* Def */	6,
/* Nec */	6,
/* Dea */	12,
/* Kni */	12,
/* War */	6,
/* Wit */	8,
/* Bar */	8,
/* Ran */	8,
/* Ass */	8,
/* Mnk */	1,
/* Brd */	5,
/* Ill */	6,
/* Tmp */	12,
/* Lan */	0,
/* Zea */   0,
/* Geo */   25,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	4,
/* Brd */	0,
/* Ill */	2,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   0,
/* Geo */   2,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_SLEEPING,	&gsn_meditation,	SLOT(0),	0,	0,	"",	"Meditation",	""
}
,	

{
"peek",
{
/* Mag */	1,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	1,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	1,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_STANDING,	&gsn_peek,	SLOT(0),	0,	0,	"",	"!Peek!",	""
}
,	

{
"pick lock",
{
/* Mag */	10,
/* Dru */	10,
/* She */	10,
/* Cle */	10,
/* Def */	10,
/* Nec */	10,
/* Dea */	10,
/* Kni */	10,
/* War */	10,
/* Wit */	10,
/* Bar */	10,
/* Ran */	10,
/* Ass */	7,
/* Mnk */	10,
/* Brd */	10,
/* Ill */	10,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	2,
/* Ass */	1,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_STANDING,	&gsn_pick_lock,	SLOT(0),	0,	12,	"",	"!Pick!",	""
}
,	

{
"sneak",
{
/* Mag */	30,
/* Dru */	30,
/* She */	30,
/* Cle */	30,
/* Def */	30,
/* Nec */	23,
/* Dea */	30,
/* Kni */	30,
/* War */	30,
/* Wit */	15,
/* Bar */	15,
/* Ran */	15,
/* Ass */	4,
/* Mnk */	8,
/* Brd */	9,
/* Ill */	23,
/* Tmp */	30,
/* Lan */	30,
/* Zea */   30,
/* Geo */   30,
/* Dra */   30 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	6,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	6,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_STANDING,	&gsn_sneak,	SLOT(0),	0,	12,	"",	"You no longer feel stealthy.",	""
}
,	

{
"steal",
{
/* Mag */	25,
/* Dru */	25,
/* She */	25,
/* Cle */	25,
/* Def */	25,
/* Nec */	25,
/* Dea */	25,
/* Kni */	25,
/* War */	25,
/* Wit */	25,
/* Bar */	25,
/* Ran */	25,
/* Ass */	25,
/* Mnk */	25,
/* Brd */	25,
/* Ill */	25,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	1,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	4,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_STANDING,	&gsn_steal,	SLOT(0),	0,	24,	"",	"!Steal!",	""
}
,	

{
"scrolls",
{
/* Mag */	1,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	1,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	1,
/* Zea */   1,
/* Geo */   1,
/* Dra */   1 
}
,
{
/* Mag */	2,
/* Dru */	2,
/* Wlk */	2,
/* Cle */	2,
/* Def */	2,
/* Nec */	2,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	2,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	2,
/* Ill */	2,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_STANDING,	&gsn_scrolls,	SLOT(0),	0,	24,	"",	"!Scrolls!",	""
}
,	

{
"staves",
{
/* Mag */	30,
/* Dru */	30,
/* She */	30,
/* Cle */	30,
/* Def */	30,
/* Nec */	30,
/* Dea */	30,
/* Kni */	30,
/* War */	30,
/* Wit */	30,
/* Bar */	30,
/* Ran */	30,
/* Ass */	30,
/* Mnk */	30,
/* Brd */	30,
/* Ill */	30,
/* Tmp */	30,
/* Lan */	30,
/* Zea */   30,
/* Geo */   30,
/* Dra */   30 
}
,
{
/* Mag */	2,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	2,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	2,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_STANDING,	&gsn_staves,	SLOT(0),	0,	12,	"",	"!Staves!",	""
}
,	

{
"wands",
{
/* Mag */	15,
/* Dru */	15,
/* She */	15,
/* Cle */	15,
/* Def */	15,
/* Nec */	15,
/* Dea */	15,
/* Kni */	15,
/* War */	15,
/* Wit */	15,
/* Bar */	15,
/* Ran */	15,
/* Ass */	15,
/* Mnk */	15,
/* Brd */	15,
/* Ill */	15,
/* Tmp */	15,
/* Lan */	15,
/* Zea */   15,
/* Geo */   15,
/* Dra */   15 
}
,
{
/* Mag */	2,
/* Dru */	2,
/* Wlk */	2,
/* Cle */	2,
/* Def */	2,
/* Nec */	2,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	2,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	2,
/* Ill */	2,
/* Tmp */	2,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   0 
}
,	spell_null,	TAR_IGNORE,	POS_STANDING,	&gsn_wands,	SLOT(0),	0,	12,	"",	"!Wands!",	""
}
,	

{
"recall",
{
/* Mag */	1,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	1,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	1,
/* Zea */   1,
/* Geo */   1,
/* Dra */   1 
}
,
{
/* Mag */	2,
/* Dru */	2,
/* Wlk */	2,
/* Cle */	2,
/* Def */	2,
/* Nec */	2,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	2,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	2,
/* Tmp */	2,
/* Lan */	2,
/* Zea */   2,
/* Geo */   2,
/* Dra */   2 
}
,	spell_null,	TAR_IGNORE,	POS_STANDING,	&gsn_recall,	SLOT(0),	0,	12,	"",	"!Recall!",	""
}
,	

{
"riding",
{
/* Mag */	1,
/* Dru */	1,
/* She */	1,
/* Cle */	1,
/* Def */	1,
/* Nec */	1,
/* Dea */	1,
/* Kni */	1,
/* War */	1,
/* Wit */	1,
/* Bar */	1,
/* Ran */	1,
/* Ass */	1,
/* Mnk */	1,
/* Brd */	1,
/* Ill */	1,
/* Tmp */	1,
/* Lan */	1,
/* Zea */   1,
/* Geo */   1,
/* Dra */   1 
}
,
{
/* Mag */	2,
/* Dru */	2,
/* Wlk */	2,
/* Cle */	2,
/* Def */	2,
/* Nec */	2,
/* Dea */	2,
/* Kni */	2,
/* War */	2,
/* Wit */	2,
/* Bar */	2,
/* Ran */	2,
/* Ass */	2,
/* Mnk */	2,
/* Brd */	2,
/* Ill */	2,
/* Tmp */	2,
/* Lan */	2,
/* Zea */   2,
/* Geo */   2,
/* Dra */   2 
}
,	spell_null,	TAR_IGNORE,	POS_STANDING,	&gsn_riding,	SLOT(0),	0,	12,	"",	"!Riding!",	""
}
,

{
"lunge",
{
/* Mag */	99,
/* Dru */	99,
/* She */	99,
/* Cle */	99,
/* Def */	99,
/* Nec */	99,
/* Dea */	99,
/* Kni */	99,
/* War */	99,
/* Wit */	99,
/* Bar */	99,
/* Ran */	99,
/* Ass */	99,
/* Mnk */	99,
/* Brd */	99,
/* Ill */	99,
/* Tmp */	99,
/* Lan */	99,
/* Zea */   99,
/* Geo */   99,
/* Dra */   30 
}
,
{
/* Mag */	0,
/* Dru */	0,
/* Wlk */	0,
/* Cle */	0,
/* Def */	0,
/* Nec */	0,
/* Dea */	0,
/* Kni */	0,
/* War */	0,
/* Wit */	0,
/* Bar */	0,
/* Ran */	0,
/* Ass */	0,
/* Mnk */	0,
/* Brd */	0,
/* Ill */	0,
/* Tmp */	0,
/* Lan */	0,
/* Zea */   0,
/* Geo */   0,
/* Dra */   6 
}
,	spell_null,	TAR_IGNORE,	POS_RESTING,	&gsn_dragonsword, SLOT(0),	0,	24,	"lunge",	"!Dragonsword!",	""
}
,
//merk	


//end skill table
};

const struct group_type group_table[MAX_GROUP] =
{

//begin group table


{
"rom basics",
{
/*Mag*/	0,
/*Dru*/	0,
/*Wlk*/	0,
/*Cle*/	0,
/*Def*/	0,
/*Nec*/	0,
/*Dea*/	0,
/*Kni*/	0,
/*War*/	0,
/*Wit*/	0,
/*Bar*/	0,
/*Ran*/	0,
/*Ass*/	0,
/*Mnk*/	0,
/*Brd*/	-1,
/*Ill*/	0,
/*Tmp*/ 0,
/*Lan*/	0,
/*Zea*/ 0,
/*Geo*/ 0,
/*Dra*/ 0 
}
,
{
"recall",
"riding",
"dodge"
}
}
,


{
"mage basics",
{
/*Mag*/	0,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	0,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"dagger",
"meditation"
}
}
,


{
"cleric basics",
{
/*Mag*/	-1,
/*Dru*/	0,
/*Wlk*/	-1,
/*Cle*/	0,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	0,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"mace",
"meditation",
"healing"
}
}
,


{
"assassin basics",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	0,
/*Mnk*/	-1,
/*Brd*/	0,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"dagger",
"sword",
"parry"
}
}
,

//Added for monks.
{
"monk basics",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	0,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"hand to hand",
"dodge",
"stun"
}
}
,

{
"warrior basics",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	0,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	0,
/*Kni*/	0,
/*War*/	0,
/*Wit*/	-1,
/*Bar*/	0,
/*Ran*/	0,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	0,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"sword",
"parry",
"second attack"
}
}
,


{
"mage default",
{
/*Mag*/	40,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"attack group",
"major attack",
"major transportation",
"enchantment",
"dispel magic",
"illusions",
"transportation",
"conjuration",
"protections",
"meditation",
"major detection",
"wands",
"disrupt",
"struggle"
}
}
,


{
"druid default",
{
/*Mag*/	-1,
/*Dru*/	40,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"benedictions",
"healing",
"maledictions",
"major transportation",
"elemental",
"protections",
"creation",
"major elemental",
"major protection",
"major healing",
"conjuration",
"focus",
"struggle"
}
}
,


{
"warlock default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	30,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"weaponsmaster",
"warlock special",
"maledictions",
"major detection",
"protections",
"transportation",
"third attack",
"enhanced damage",
"dual wield",
"bash",
"attack group",
"fourth attack",
"major transportation",
"major creation",
"struggle"
}
}
,


{
"cleric default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	40,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"benedictions",
"major benedictions",
"detection",
"major healing",
"protections",
"major protection",
"holy power",
"transportation",
"maledictions",
"second attack",
"struggle",
"focus",
"Eagle eye"
}
}
,


{
"soldier default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	35,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"healing",
"faerie fire",
"faerie fog",
"imbue",
"enchant weapon",
"fly",
"shield block",
"braver",
"cross slash",
"blade beam",
"climhazzard",
"meteorain",
"finishing touch",
"omnislash",
"second attack",
"third attack",
"sword",
"parry",
"dodge",
"fast healing",
"scout",
"struggle"
}
}
,


{
"necromancer default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	20,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"detection",
"healing",
"decripify",
"major healing",
"necromancy",
"unholy power",
"protections",
"transportation",
"flail",
"animate dead",
"struggle",
"farsight"
}
}
,


{
"deathknight default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	40,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"weaponsmaster",
"shield block",
"rescue",
"disarm",
"fast healing",
"maledictions",
"protections",
"transportation",
"charge",
"haggle",
"third attack",
"enhanced damage",
"dual wield",
"bash",
"struggle"
}
}
,


{
"knight default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	40,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"weaponsmaster",
"shield block",
"rescue",
"disarm",
"fast healing",
"benedictions",
"protections",
"transportation",
"charge",
"haggle",
"third attack",
"enhanced damage",
"dual wield",
"bash",
"soul coil",
"struggle"
}
}
,


{
"warrior default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	40,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"weaponsmaster",
"shield block",
"forcing",
"disarm",
"enhanced damage",
"third attack",
"struggle",
"rescue",
"mend",
"rush",
"kick",
"transportation",
"bash",
"dual wield",
"blind fighting",
"trip",
"struggle"
}
}
,


{
"witch default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	40,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"maledictions",
"major maledictions",
"detection",
"protections",
"major protection",
"unholy power",
"transportation",
//"benedictions",
"major creation",
"benedictions",
"conjuration",
"elemental",
"struggle",
"focus"
}
}
,


{
"barbarian default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	40,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"weaponsmaster",
"shield block",
"bash",
"tracking",
"disarm",
"fast healing",
"third attack",
"enhanced damage",
"dual wield",
"scout",
"forcing",
"hand to hand",
"mend",
"trip",
"struggle"
}
}
,


{
"ranger default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	30,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"weaponsmaster",
"tracking",
"healing",
"trap",
"kick",
"detect trap",
"dual wield",
"shield block",
"hide",
"struggle",
"fast healing",
"benedictions",
"scout",
"protections",
"elemental",
"forcing",
}
}
,


{
"time default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/     30,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"mage basics",
"meditation",
"temporal",
"temporal attack",
"major temporal",
"detection",
"Enhance Aura",
"transportation",
"focus",
"rom basics",
"struggle"
}
}
,

{
"jump command",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/ -1,
/*Geo*/ -1,
/*Dra*/  40
}
,
{
"weaponsmaster",
"disarm",
"protections",
"second attack",
"third attack",
"brutal damage",
"jump",
"dual wield",
"detect hidden",
"transportation",
"lunge",
"disarm",
"forcing",
"fast healing",
"mend",
"rom basics",
"struggle",
"hand to hand"
}
}
,

{
"temporal",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/      9,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"bubble",
"haste",
"shield",
"slow",
"suspend",
"lapse"
}
}
,
{
"temporal attack",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/      7,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"age",
"chill touch",
"color spray",
"lightning bolt",
"youth"
}
}
,
{
"major temporal",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/     10,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"blink",
"hold",
"temporal blade",
"timestop",
"wall of force",
"rift",
"farsight"
}
}
,

{
"lancer default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/     -1,
/*Lan*/	30,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"brutal damage",
"forcing",
"ground",
"kick",
"rescue",
"rush",
"shield bash",
"shield block",
"skewer",
"second attack",
"third attack",
"polearm",
"spear",
"struggle"
}
}
,

{
"assassin default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	30,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"backstab",
"disarm",
"sneak",
"hide",
"peek",
"pick lock",
"circle",
"detect trap",
"dual wield",
"trap",
"second attack",
"stun",
"dirt kicking",
"tracking",
"struggle"
}
}
,


{
"monk default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	40,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"tracking",
"dirt kicking",
"disarm",
"enhanced damage",
"second attack",
"third attack",
"fourth attack",
"hide",
"hand to hand",
"strike",
"sneak",
"ki power",
"meditation",
"struggle"
// Added Monk Default from Help file (3.5x) 7/2/01
}
}
,


{
"bard default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	40,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"parry",
"circle",
"healing",
"protections",
"transportation",
"hide",
"detect trap",
"stun",
"dual wield",
"dirt kicking",
"tracking",
"disarm",
"bard special",
"struggle",
"bluff"
}
}
,
{
"zealot default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/ 30,
/*Geo*/ -1,
/*Dra*/ -1
}
,
{
"disarm",
"bash",
"strike",
"polearm",
"spear",
"dagger",
"sword",
"fourth attack",
"third attack",
"second attack",
"struggle",
"parry",
"trip",
"fast healing"
}
}
,
{
"geomancer default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/ -1,
/*Geo*/ 30,
/*Dra*/ -1
}
,
{
"geomancy",
"earthlore",
"second attack",
"axe",
"mace",
"attack group",
"parry",
"recall",
"riding",
"enhanced damage",
"struggle",
"shield block",
"creation",
"fast healing"
}
}
,

{
"illusionist default",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	40,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"attack group",
"major illusions",
"transportation",
"major transportation",
"creation",
"major conjuration",
"protections",
"major detection",
"detection",
"illusions",
"hide",
"conjuration",
"enchantment",
"struggle",
"focus"
}
}
,


{
"weaponsmaster",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	9,
/*Kni*/	9,
/*War*/	6,
/*Wit*/	-1,
/*Bar*/	6,
/*Ran*/	6,
/*Ass*/	9,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"axe",
"dagger",
"flail",
"mace",
"polearm",
"spear",
"sword",
"whip"
}
}
,


{
"bard special",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	4,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"charm person",
"sleep",
"blindness",
"life song",
"magic song",
"attack song",
"defense song",
"learning song",
"speed song",
"wall of force",
"soul coil"
}
}
,
{
"zealot special",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  6,
/*Geo*/ -1,
/*Dra*/ -1
}
,
{
"purify",
"nullify",
"spell edge"
}
}
,

{
"warlock special",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	 4,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  -1,
/*Geo*/ -1,
/*Dra*/ -1
}
,
{
"disarm",
"tracer cloud",
"meditation",
"focus",
}
}
,

{
"necromancy",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	6,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  -1,
/*Geo*/ -1,
/*Dra*/ -1
}
,
{
"cure blindness",	
"detect invis",
"decay aura",
"strengthen ground",
"hex",
"life drain",
"mana burn",
"spirit rain"
}
}
,
{
"geomancer special",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  -1,
/*Geo*/ 6,
/*Dra*/ -1
}
,
{
"giant strength",
"nature walk",
"flaming wall",
"weaken",
"control weather",
"strengthen ground",
"call lightning"
}
}
,
{
"benedictions",
{
/*Mag*/	-1,
/*Dru*/	2,
/*Wlk*/	-1,
/*Cle*/	2,
/*Def*/	-1,
/*Nec*/	2,
/*Dea*/	-1,
/*Kni*/	4,
/*War*/	-1,
/*Wit*/	4,
/*Bar*/	-1,
/*Ran*/	4,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"bless",
"cure critical",
"refresh",
"remove curse",
"cure blindness",
"cure disease",
"cure poison",
"cancellation"
}
}
,


{
"major benedictions",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	6,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"prism",
"calm",
"smite",
}
}
,


{
"attack group",
{
/*Mag*/	4,
/*Dru*/	-1,
/*Wlk*/	6,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	6,
/*Ill*/	4,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  4,
/*Dra*/  0
}
,
{
"energy drain",
"lightning bolt",
"chill touch",
"fireball",
"acid blast",
"colour spray"
}
}
,


{
"creation",
{
/*Mag*/	2,
/*Dru*/	2,
/*Wlk*/	2,
/*Cle*/	2,
/*Def*/	-1,
/*Nec*/	2,
/*Dea*/	-1,
/*Kni*/	2,
/*War*/	-1,
/*Wit*/	2,
/*Bar*/	-1,
/*Ran*/	2,
/*Ass*/	-1,
/*Mnk*/	2,
/*Brd*/	2,
/*Ill*/	2,
/*Tmp*/ 2,
/*Lan*/	-1,
/*Zea*/  2,
/*Geo*/  4,
/*Dra*/  4
}
,
{
"continual light",
"create food",
"create rose",
"create spring",
"create water",
"floating disc"
}
}
,


{
"major creation",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"alchemy",
"scribery"
}
}
,


{
"detection",
{
/*Mag*/	1,
/*Dru*/	1,
/*Wlk*/	1,
/*Cle*/	1,
/*Def*/	-1,
/*Nec*/	1,
/*Dea*/	1,
/*Kni*/	1,
/*War*/	1,
/*Wit*/	1,
/*Bar*/	-1,
/*Ran*/	1,
/*Ass*/	1,
/*Mnk*/	1,
/*Brd*/	1,
/*Ill*/	1,
/*Tmp*/ 1,
/*Lan*/	-1,
/*Zea*/  1,
/*Geo*/  2,
/*Dra*/  2
}
,
{
"identify",
"locate object",
"detect alignment",
"infravision",
"know level"
}
}
,
{
"draconian magic",
{
/*Mag*/	0,
/*Dru*/	0,
/*Wlk*/	0,
/*Cle*/	0,
/*Def*/	0,
/*Nec*/	0,
/*Dea*/	0,
/*Kni*/	0,
/*War*/	0,
/*Wit*/	0,
/*Bar*/	0,
/*Ran*/	0,
/*Ass*/	0,
/*Mnk*/	0,
/*Brd*/	0,
/*Ill*/	0,
/*Tmp*/ 0,
/*Lan*/	0,
/*Zea*/ 0,
/*Geo*/ 0,
/*Dra*/ 6
}
,
{
"draconian blessing",
"draconian mist",
"draconian skin",
"soul coil",
}
}
,
{
"conjuration",
{
/*Mag*/	6,
/*Dru*/	6,
/*Wlk*/	4,
/*Cle*/	4,
/*Def*/	-1,
/*Nec*/	4,
/*Dea*/	0,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	3,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	6,
/*Ill*/	3,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"screaming wall",
"servant",
"guardian",
"decoy",
"mount"
}
}
,


{
"major conjuration",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	6,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	6,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"minion",
"guardian",
"wall of force",
"flaming wall"
}
}
,


{
"major detection",
{
/*Mag*/	6,
/*Dru*/	-1,
/*Wlk*/	12,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	4,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  8,
/*Dra*/  0
}
,
{
"detect invisibility",
"detect hidden",
"farsight"
}
}
,


{
"major attack",
{
/*Mag*/	12,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"chain lightning",
"acid breath",
"frost breath",
"gas breath",
"word of recall",
"fire breath",
"prismatic spray",
"lightning breath"
}
}
,


{
"manipulation",
{
/*Mag*/	6,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	6,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"reflect"
}
}
,


{
"enchantment",
{
/*Mag*/	6,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	0,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	4,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"enchant armor",
"enchant weapon",
"fireproof",
"recharge",
"imbue"
}
}
,
{
"minor healing",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	1,
/*Nec*/	-1,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/ -1,
/*Zea*/  4,
/*Geo*/ 2,
/*Dra*/ -1
}
,
{
"cure light",
"refresh"
}
}
,

{
"healing",
{
/*Mag*/	4,
/*Dru*/	2,
/*Wlk*/	4,
/*Cle*/	2,
/*Def*/	-1,
/*Nec*/	2,
/*Dea*/	4,
/*Kni*/	4,
/*War*/	4,
/*Wit*/	2,
/*Bar*/	-1,
/*Ran*/	4,
/*Ass*/	4,
/*Mnk*/	4,
/*Brd*/	4,
/*Ill*/	4,
/*Tmp*/     4,
/*Lan*/     8,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  4
}
,
{
"cure light",
"refresh"
}
}
,


{
"major healing",
{
/*Mag*/	-1,
/*Dru*/	6,
/*Wlk*/	-1,
/*Cle*/	6,
/*Def*/	-1,
/*Nec*/	6,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	8,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"heal",
"mass healing"
}
}
,


{
"illusions",
{
/*Mag*/	4,
/*Dru*/	6,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	6,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	2,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"invisibility",
"ventriloquate",
"change sex",
"slow"
}
}
,


{
"major illusions",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	6,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"web",
"sleep",
"charm person",
"silence",
"mass invisibility",
"blind",
"haste"
}
}
,


{
"maledictions",
{
/*Mag*/	1,
/*Dru*/	6,
/*Wlk*/	6,
/*Cle*/	6,
/*Def*/	-1,
/*Nec*/	4,
/*Dea*/	4,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	4,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"blindness",
"curse",
"energy drain",
"plague",
"poison",
"slow",
"dispel magic",
"death coil",
"soul coil",
"weaken",
"cause light",
"cause serious"
}
}
,


{
"unholy power",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	4,
/*Dea*/	6,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	4,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"death touch",
"demonfire",
"flamestrike",
"nullify",
"unholy word",
}
}
,


{
"major maledictions",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	6,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	6,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"animate dead",
"agony",
"entangle",
"sleep",
"harm",
"cause critical"
}
}
,


{
"protections",
{
/*Mag*/	4,
/*Dru*/	4,
/*Wlk*/	-1,
/*Cle*/	2,
/*Def*/	4,
/*Nec*/	2,
/*Dea*/	4,
/*Kni*/	4,
/*War*/	-1,
/*Wit*/	2,
/*Bar*/	-1,
/*Ran*/	4,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	6,
/*Ill*/	4,
/*Tmp*/ 4,
/*Lan*/	-1,
/*Zea*/  4,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"armor",
"cancellation",
"fireproof",
"protection evil",
"protection good"
}
}
,


{
"major protection",
{
/*Mag*/	-1,
/*Dru*/	8,
/*Wlk*/	-1,
/*Cle*/	6,
/*Def*/	-1,
/*Nec*/	6,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	6,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"stone skin",
"sanctuary",
"shield"
}
}
,


{
"holy power",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	4,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	6,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"frenzy",
"holy word",
"ray of truth",
"cure critical",
"purify"
}
}
,


{
"transportation",
{
/*Mag*/	2,
/*Dru*/	2,
/*Wlk*/	2,
/*Cle*/	2,
/*Def*/	0,
/*Nec*/	2,
/*Dea*/	2,
/*Kni*/	2,
/*War*/	2,
/*Wit*/	2,
/*Bar*/	-1,
/*Ran*/	2,
/*Ass*/	2,
/*Mnk*/	2,
/*Brd*/	2,
/*Ill*/	2,
/*Tmp*/ 2,
/*Lan*/	-1,
/*Zea*/  2,
/*Geo*/  2,
/*Dra*/  0
}
,
{
"fly",
"refresh",
"pass door"
}
}
,


{
"major transportation",
{
/*Mag*/	4,
/*Dru*/	-1,
/*Wlk*/	4,
/*Cle*/	4,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	4,
/*Tmp*/ 4,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"summon",
"gate",
"nexus",
"teleport",
"portal",
}
}
,


{
"elemental",
{
/*Mag*/	-1,
/*Dru*/	4,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	4,
/*Bar*/	-1,
/*Ran*/	6,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"call animal",
"control weather",
"faerie fire",
"faerie fog",
"earthquake",
"haven",
"eagle eye",
"stone skin",
"flamestrike",
"call lightning"
}
}
,


{
"major elemental",
{
/*Mag*/	-1,
/*Dru*/	6,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	-1,
/*Ran*/	-1,
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/	-1,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"entangle",
"chill touch",
"lightning bolt",
"call lightning",
"flamestrike",
"firestorm",
"tremor"
}
}
,


{
"tracking",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	-1,
/*Wit*/	-1,
/*Bar*/	6,
/*Ran*/	4,
/*Ass*/	4,
/*Mnk*/	6,
/*Brd*/	4,
/*Ill*/	-1,
/*Tmp*/     -1,
/*Lan*/      6,
/*Zea*/  6,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"chase",
"hunt"
}
}
,
//krem

{
"forcing",
{
/*Mag*/	-1,
/*Dru*/	-1,
/*Wlk*/	-1,
/*Cle*/	-1,
/*Def*/	-1,
/*Nec*/	0,
/*Dea*/	-1,
/*Kni*/	-1,
/*War*/	4,
/*Wit*/	-1,
/*Bar*/	4,
/*Ran*/	4, //Ranger Forcing 4/26/06 Amos
/*Ass*/	-1,
/*Mnk*/	-1,
/*Brd*/	-1,
/*Ill*/	-1,
/*Tmp*/ -1,
/*Lan*/  4,
/*Zea*/  0,
/*Geo*/  0,
/*Dra*/  0
}
,
{
"push",
"drag"
}
}
,


//end group table

};
/* 39 groups noted */

