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
 * Ever wonder just what the stats on things were in the game? Statting    *
 * one object or mob at a time can be tedious and frequently you have to   *
 * stop and write things down, like hitpoints, armor classes, etc, if you  *
 * are trying to build an area and you want to preserve some continuity.   *
 * Granted, there should probably be tables and such availabe for builders'*
 * use (as there are on Broken Shadows), but you have to base those tables *
 * off something.                                                          *
 *                                                                         *
 * Well... this function is a cross between stat and dump. It loads each   *
 * mob and object briefly and writes its stats (or at least the vital ones)*
 * to a file. I removed a lot of the things from the stat part, mostly     *
 * empty lines where PC data was stored and also a lot of the things that  *
 * returned 0, such as carry weight.                                       *
 *                                                                         *
 * The files are place in the parent directory of the area directory by    *
 * default and are rather large (about 800k each for Shadows). With a      *
 * little work (I wrote a little C++ program to do this), they can be      *
 * converted into a character-delimeted file, so you can import it into    *
 * Access, Excel, or many other popular programs. I could have modified    *
 * This file to write it out in that format, but I was too lazy.           *
 *                                                                         *
 * One other thing work noting: Since it does load all the objects and     *
 * mobs in quick succession, CPU and memory usage climbs for about 10-15   *
 * seconds. This might cause a bit of lag for the players. I dunno. I      *
 * haven't used it when players were on.                                   *
 *                                                                         *
 * If you choose to use this code, please retain my name in this file and  *
 * send me an email (dwa1844@rit.edu) saying you are using it. Suggestions *
 * for improvement are welcome                                             *
 ***************************************************************************/

/*
 *    This is where I am going to put all of the commands that dump stats 
 *    to a file  --  Coelcoeth
 */

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "tables.h"
#include "magic.h"

int power ( int x , int y )
{
    int result = 1;

    while ( y > 0 ) {
        result *= x;
        y--;
    }

    return result;
}

/*
 * help_dump by coelcoeth
 */
void do_help_dump(CHAR_DATA * ch, char *argument)
{
    FILE *fp;
    int x;
    int count;
    int i;
    char *cl;
    int sn;
    int gn;

    /* open file */
    fclose(fpReserve);

    /* start printing out help data */
    fp = fopen("../races.hlp", "w");

    fprintf(fp, "#HELPS\n\n");

    for( x = 1 ; x < MAX_PC_RACE ; x++) {
        if (pc_race_table[x].name == NULL)
	    break;

        fprintf(fp, "-1 '%s'~\n.\n", pc_race_table[x].name);
	fprintf(fp, "Race Name: %s\n", pc_race_table[x].name);
        fprintf(fp, "Race Description: %s\n", pc_race_table[x].short_description);

        if ( !pc_race_table[x].isRemort )
        {
            fprintf(fp, "Starting Stats: ");
            fprintf(fp, "STR: %d INT: %d WIS: %d DEX: %d CON: %d\n",
                    pc_race_table[x].stats[STAT_STR],
                    pc_race_table[x].stats[STAT_INT],
                    pc_race_table[x].stats[STAT_WIS],
                    pc_race_table[x].stats[STAT_DEX],
                    pc_race_table[x].stats[STAT_CON] );
        }

        fprintf(fp, "Max Stats: ");
        fprintf(fp, "STR: %d INT: %d WIS: %d DEX: %d CON: %d\n",
                pc_race_table[x].max_stats[STAT_STR],
        	pc_race_table[x].max_stats[STAT_INT],
        	pc_race_table[x].max_stats[STAT_WIS],
        	pc_race_table[x].max_stats[STAT_DEX],
        	pc_race_table[x].max_stats[STAT_CON] );
        
        fprintf(fp, "Available to: ");
            
        count = 0;
        for ( i = 1 ; i < power( 2, MAX_KINGDOM - 1 ) + 1 ; i *= 2 ) {
            count++;
            if ( IS_SET( pc_race_table[x].side , i ) )
                fprintf(fp, "%s ", kingdom_table[count].name);
        }

        fprintf(fp, "\nRacial Skills: ");
        for ( i = 0 ; i < 5 ; i++ )
            if (pc_race_table[x].skills[i] != NULL)
                fprintf(fp, "%s ", pc_race_table[x].skills[i]);
    
        fprintf(fp, "\nImmune to: ");
        for ( i = 0 ; imm_flags[i].name != NULL ; i++ )
            if ( IS_SET ( imm_flags[i].bit , race_table[x].imm ) )
                fprintf(fp, "%s ", imm_flags[i].name);

        fprintf(fp, "\nResistant to: ");
        for ( i = 0 ; res_flags[i].name != NULL ; i++ )
            if ( IS_SET ( res_flags[i].bit , race_table[x].res ) )
                fprintf(fp, "%s ", res_flags[i].name);

        fprintf(fp, "\nVulnerable to: ");
        for ( i = 0 ; vuln_flags[i].name != NULL ; i++ )
            if ( IS_SET ( vuln_flags[i].bit , race_table[x].vuln ) )
                fprintf(fp, "%s ", vuln_flags[i].name);

        fprintf(fp, "\n\nFor more help on races see {chttp://www.actofwar.com/cgi-bin/raceclass.cgi{x\n");  

	fprintf(fp, "\n~\n");
    }	

    fprintf(fp, "0 $~\n\n#$");
    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");

    /* open file */
    fclose(fpReserve);

    /* start printing out help data */
    fp = fopen("../classes.hlp", "w");

    fprintf(fp, "#HELPS\n\n");

    for( x = 0 ; x < MAX_CLASS ; x++) {
        if (class_table[x].name == NULL)
	    break;

        fprintf(fp, "-1 '%s'~\n.\n", class_table[x].name);
	fprintf(fp, "Class Name: %s\n", class_table[x].name);
        fprintf(fp, "Class Description: %s\n", class_table[x].short_description);
         
        fprintf(fp, "Prime Attribute: ");
        switch( class_table[x].attr_prime ) {
            case STAT_STR : cl = "Strength"     ; break ;
            case STAT_INT : cl = "Intelligence" ; break ;
            case STAT_WIS : cl = "Wisdom"       ; break ;
            case STAT_DEX : cl = "Dexterity"    ; break ;
            case STAT_CON : cl = "Constitution" ; break ;
	    default       : cl = "None"         ; break ;
        }
        fprintf(fp, "%s\n", cl);

        fprintf(fp, "Available to: ");  
        
        count = 0;
        for ( i = 1 ; i < power( 2, MAX_KINGDOM - 1 ) + 1 ; i *= 2 ) {
            count++;
            if ( IS_SET( class_table[x].side , i ) )
                fprintf(fp, "%s ", kingdom_table[count].name);
        }
                
        fprintf(fp, "\n\nFor more help on classes see {chttp://www.actofwar.com/cgi-bin/raceclass.cgi{x\n");  
   
	fprintf(fp, "\n~\n");
    }	

    fprintf(fp, "0 $~\n\n#$");
    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");

    /* open file */
    fclose(fpReserve);
 
    /* start printing out help data */
    fp = fopen("../skills_spells.hlp", "w");
        
    fprintf(fp, "#HELPS\n\n");

    for (x = 1; x < MAX_SKILL; x++) {
        if (skill_table[x].name == NULL)
            continue;

        fprintf(fp, "-1 '%s'~\n.\n", skill_table[x].name);
	fprintf(fp, "   {w{4   Name:     {x  %s\n", skill_table[x].name);

        if (skill_table[x].spell_fun == spell_null) {
	    fprintf(fp,"   {w{4   Type:     {x  Skill\n");
  	} else {
	    fprintf(fp,"   {w{4   Type:     {x  Spell\n");
	    fprintf(fp,"   {w{4   Mana:     {x  %d\n", skill_table[x].min_mana);
	}

        fprintf(fp, "   {w{4   Damage:   {x  \n"); 
        fprintf(fp, "   {w{4   Duration: {x  \n"); 
        fprintf(fp, "   {w{4   Delay:    {x  %d\n", skill_table[x].beats);

	fprintf(fp, "\n~\n");
    }	

    fprintf(fp, "0 $~\n\n#$");
    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");

    /* open file */
    fclose(fpReserve);
 
    /* start printing out help data */
    fp = fopen("../group_add.hlp", "w");
        
    fprintf(fp, "#HELPS\n\n");

    for (gn = 0; gn < MAX_GROUP; gn++) { 
        if (group_table[gn].name == NULL)
            break;

        fprintf(fp, "-1 '%s'~\n.\n", group_table[gn].name);
        fprintf(fp, "%s\n\n", group_table[gn].name);
     
        for (sn = 0; sn < MAX_IN_GROUP; sn++) {
            if (group_table[gn].spells[sn] == NULL)
                break;   
            fprintf(fp, "%s\n", group_table[gn].spells[sn]);
        }       

	fprintf(fp, "\n~\n");
    }

    fprintf(fp, "0 $~\n\n#$");
    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");

//    do_command_help_dump();
}

/*
 * skill_dump by coelcoeth
 */
void do_skill_dump(CHAR_DATA * ch, char *argument)
{
    extern int has_saved;
    int skillnum;
    int avg_dam;
    int x = 1;
    FILE *fp;
    char buf[MAX_STRING_LENGTH];
    char char_x_buf[MAX_INPUT_LENGTH];
    char char_y_buf[MAX_INPUT_LENGTH];
    CHAR_DATA *char_x;
    CHAR_DATA *char_y;

    sprintf(char_x_buf, "xxxxxx");
    sprintf(char_y_buf, "yyyyyy");

    /* need to load the test chars Xxxxxx and Yyyyyy */
    do_pload(ch, char_x_buf);
    do_pload(ch, char_y_buf);

    /* assign pointers to them so we can refrence them */
    char_x = get_char_world(ch, char_x_buf);
    char_y = get_char_world(ch, char_y_buf);

    /* massage the chars so that they can withstand the tests. */
    char_x->max_hit = 3000;
    char_x->max_mana = 3000;
    char_x->max_move = 3000;
    char_y->max_hit = 3000;
    char_y->max_mana = 3000;
    char_y->max_move = 3000;

    do_restore(ch, char_x_buf);
    do_restore(ch, char_y_buf);

    /* open file */
    fclose(fpReserve);

    /* start printing out skill data */
    fp = fopen("../skills.txt", "w");

    fprintf(fp, "\n                                  Skill Analysis\n");
    fprintf(fp, "------------------------------------------------------------------------------------\n");
    fprintf(fp, "|     Skill name      | Slot # | Min Mana | Beats | Avg Damage | Dam/Mana | Saves%% |\n");
    fprintf(fp, "------------------------------------------------------------------------------------\n");

    for ( skillnum = 0 ; skillnum < MAX_SKILL ; skillnum++ )
    {
	avg_dam = 0;
	has_saved = 0;

	if (skill_table[skillnum].name == NULL)
	    continue;

	if (skill_table[skillnum].spell_fun != NULL && skill_table[skillnum].target == TAR_CHAR_OFFENSIVE)
	    for (x = 1; x < 1000; x++) {
		(*skill_table[skillnum].spell_fun) (skillnum, char_x->level, char_x, char_y, TARGET_CHAR);
		avg_dam += char_y->max_hit - char_y->hit;
		WAIT_STATE(char_x, 0);
		WAIT_STATE(char_y, 0);
		DAZE_STATE(char_x, 0);
		DAZE_STATE(char_y, 0);
		do_restore(ch, char_x_buf);
		do_restore(ch, char_y_buf);
	    }

	avg_dam /= x;

	sprintf(buf, "|%20s | %6d | %8d | %5d | %10d | %8d | %6d |\n",
		skill_table[skillnum].name,
		skill_table[skillnum].slot,
		skill_table[skillnum].min_mana,
		skill_table[skillnum].beats,
		avg_dam,
		skill_table[skillnum].min_mana ? avg_dam / skill_table[skillnum].min_mana : 0,
		has_saved * 100 / x);
	fprintf(fp, buf);
	fprintf(fp, "------------------------------------------------------------------------------------\n");
    }

    do_peace(ch, "");
    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");

}

/* 
 * new_dump written by Rahl (Daniel Anderson) of Broken Shadows
 */
void do_new_dump(CHAR_DATA * ch, char *argument)
{
    MOB_INDEX_DATA *pMobIndex;
    OBJ_INDEX_DATA *pObjIndex;
    FILE *fp;
    int vnum, nMatch = 0;
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    AFFECT_DATA *paf;
    CHAR_DATA *mob;

    /* open file */
    fclose(fpReserve);

    /* start printing out mobile data */
    fp = fopen("../mob.txt", "w");

    fprintf(fp, "\nMobile Analysis\n");
    fprintf(fp, "---------------\n");
    nMatch = 0;
    for (vnum = 0; nMatch < top_mob_index; vnum++)
	if ((pMobIndex = get_mob_index(vnum)) != NULL) {
	    nMatch++;
	    mob = create_mobile(pMobIndex);
	    sprintf(buf, "Name: %s : ", mob->name);
	    fprintf(fp, buf);

	    sprintf(buf, "Vnum: %d : Race: %s : Sex: %s : Room: %d : Count: %d : ",
		    IS_NPC(mob) ? mob->pIndexData->vnum : 0,
		    race_table[mob->race].name,
		    mob->sex == SEX_MALE ? "male" :
		    mob->sex == SEX_FEMALE ? "female" : "neutral",
		    mob->in_room == NULL ? 0 : mob->in_room->vnum,
		    mob->pIndexData->count);
	    fprintf(fp, buf);

	    sprintf(buf, "Str: %d(%d) : Int: %d(%d) : Wis: %d(%d) : Dex: %d(%d) : Con: %d(%d) : ",
		    mob->perm_stat[STAT_STR],
		    get_curr_stat(mob, STAT_STR),
		    mob->perm_stat[STAT_INT],
		    get_curr_stat(mob, STAT_INT),
		    mob->perm_stat[STAT_WIS],
		    get_curr_stat(mob, STAT_WIS),
		    mob->perm_stat[STAT_DEX],
		    get_curr_stat(mob, STAT_DEX),
		    mob->perm_stat[STAT_CON],
		    get_curr_stat(mob, STAT_CON));
	    fprintf(fp, buf);

	    sprintf(buf, "Hp: %d : Mana: %d : Move: %d : Hit: %d : Dam: %d : ",
		    mob->max_hit,
		    mob->max_mana,
		    mob->max_move,
		    GET_HITROLL(mob), GET_DAMROLL(mob));
	    fprintf(fp, buf);

	    sprintf(buf, "Lv: %d : Align: %d : Gold: %ld : Damage: %dd%d : Message: %s : ",
		    mob->level,
		    mob->alignment,
		    mob->gold,
		    mob->damage[DICE_NUMBER], mob->damage[DICE_TYPE],
		    attack_table[mob->dam_type].name);
	    fprintf(fp, buf);

	    sprintf(buf, "Armor: pierce: %d : bash: %d : slash: %d : magic: %d : ",
		    GET_AC(mob, AC_PIERCE), GET_AC(mob, AC_BASH),
		    GET_AC(mob, AC_SLASH), GET_AC(mob, AC_EXOTIC));
	    fprintf(fp, buf);

	    sprintf(buf, "Act: %s : ", act_bit_name(mob->act));
	    fprintf(fp, buf);

	    if (IS_NPC(mob) && mob->off_flags) {
		sprintf(buf, "Offense: %s : ", off_bit_name(mob->off_flags));
		fprintf(fp, buf);
	    } else {
		sprintf(buf, "Offense: (none) : ");
		fprintf(fp, buf);
	    }

	    if (mob->imm_flags) {
		sprintf(buf, "Immune: %s : ", imm_bit_name(mob->imm_flags));
		fprintf(fp, buf);
	    } else {
		sprintf(buf, "Immune: (none) : ");
		fprintf(fp, buf);
	    }

	    if (mob->res_flags) {
		sprintf(buf, "Resist: %s : ", imm_bit_name(mob->res_flags));
		fprintf(fp, buf);
	    } else {
		sprintf(buf, "Resist: (none) : ");
		fprintf(fp, buf);
	    }

	    if (mob->vuln_flags) {
		sprintf(buf, "Vulnerable: %s : ", imm_bit_name(mob->vuln_flags));
		fprintf(fp, buf);
	    } else {
		sprintf(buf, "Vulnerable: (none) : ");
		fprintf(fp, buf);
	    }

	    sprintf(buf, "Form: %s : Parts: %s : ",
		    form_bit_name(mob->form), part_bit_name(mob->parts));
	    fprintf(fp, buf);

	    if (mob->affected_by) {
		sprintf(buf, "Affected by: %s : ",
			affect_bit_name(mob->affected_by));
		fprintf(fp, buf);
	    } else {
		sprintf(buf, "Affected by: (none) : ");
		fprintf(fp, buf);
	    }

	    sprintf(buf, "Short description: %s : ",
		    mob->short_descr);
	    fprintf(fp, buf);

	    if (IS_NPC(mob) && mob->spec_fun != 0) {
		sprintf(buf, "Mobile has special procedure. - %s : ", spec_name(mob->spec_fun));
		fprintf(fp, buf);
	    } else {
		sprintf(buf, "Mobile has special procedure. - (none) : ");
		fprintf(fp, buf);
	    }

	    for (paf = mob->affected; paf != NULL; paf = paf->next) {
		sprintf(buf,
			"Spell: '%s' modifies %s by %d for %d hours with bits %s, level %d. : ",
			skill_table[(int) paf->type].name,
			affect_loc_name(paf->location),
			paf->modifier,
			paf->duration,
			affect_bit_name(paf->bitvector),
			paf->level
		    );
		fprintf(fp, buf);
	    }

	    fprintf(fp, "\n");

	    extract_char(mob, FALSE);
	}
    fclose(fp);

    /* start printing out object data */
    fp = fopen("../obj.txt", "w");

    fprintf(fp, "\nObject Analysis\n");
    fprintf(fp, "---------------\n");
    nMatch = 0;
    for (vnum = 0; nMatch < top_obj_index; vnum++)
	if ((pObjIndex = get_obj_index(vnum)) != NULL) {
	    nMatch++;

	    obj = create_object(pObjIndex, 0);

	    sprintf(buf, "Name(s): %s : ",
		    obj->name);
	    fprintf(fp, buf);

	    sprintf(buf, "Vnum: %d : Format: %s : Type: %s : Number: %d/%d : Weight: %d/%d : ",
		    obj->pIndexData->vnum, obj->pIndexData->new_format ? "new" : "old",
		    item_name(obj->item_type), 1, get_obj_number(obj),
		    obj->weight, get_obj_weight(obj));
	    fprintf(fp, buf);

	    sprintf(buf, "Short description: %s : Long description: %s : ",
		    obj->short_descr, obj->description);
	    fprintf(fp, buf);

	    sprintf(buf, "Wear bits: %s : Extra bits: %s : ",
		    wear_bit_name(obj->wear_flags), extra_bit_name(obj->extra_flags));
	    fprintf(fp, buf);

	    sprintf(buf, "Level: %d : Cost: %d : Condition: %d : Timer: %d : ",
		    obj->level, obj->cost, obj->condition, obj->timer);
	    fprintf(fp, buf);

	    sprintf(buf,
		    "In room: %d : In object: %s : Carried by: %s : Wear_loc: %d : ",
		    obj->in_room == NULL ? 0 : obj->in_room->vnum,
		    obj->in_obj == NULL ? "(none)" : obj->in_obj->short_descr,
		    obj->carried_by == NULL ? "(none)" : obj->carried_by->name,
		    obj->wear_loc);
	    fprintf(fp, buf);

	    sprintf(buf, "Values: %d : %d : %d : %d : %d : ",
		    obj->value[0], obj->value[1], obj->value[2], obj->value[3],
		    obj->value[4]);
	    fprintf(fp, buf);

	    /* now give out vital statistics as per identify */
	    /* now for the tough bit, all items need to generate the same number of */
	    /* colon delimited fields so that it will be acceptable to excel */

	    switch (obj->item_type) {
	    case ITEM_SCROLL:
	    case ITEM_POTION:
	    case ITEM_PILL:
		if (obj->item_type == ITEM_SCROLL)
		    sprintf(buf, "ITEM_SCROLL : ");
		if (obj->item_type == ITEM_POTION)
		    sprintf(buf, "ITEM_POTION : ");
		if (obj->item_type == ITEM_PILL)
		    sprintf(buf, "ITEM_PILL : ");

		sprintf(buf, "Level %d spells of : ", obj->value[0]);
		fprintf(fp, buf);

		if (obj->value[1] >= 0 && obj->value[1] < MAX_SKILL) {
		    fprintf(fp, " '");
		    fprintf(fp, skill_table[obj->value[1]].name);
		    fprintf(fp, "' : ");
		}
		if (obj->value[2] >= 0 && obj->value[2] < MAX_SKILL) {
		    fprintf(fp, " '");
		    fprintf(fp, skill_table[obj->value[2]].name);
		    fprintf(fp, "' : ");
		}
		if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL) {
		    fprintf(fp, " '");
		    fprintf(fp, skill_table[obj->value[3]].name);
		    fprintf(fp, "' : ");
		}
		break;

	    case ITEM_WAND:
	    case ITEM_STAFF:
		if (obj->item_type == ITEM_WAND)
		    sprintf(buf, "ITEM_WAND : ");
		if (obj->item_type == ITEM_STAFF)
		    sprintf(buf, "ITEM_STAFF : ");

		sprintf(buf, "Has %d(%d) charges of level %d : ",
			obj->value[1], obj->value[2], obj->value[0]);
		fprintf(fp, buf);

		if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL) {
		    fprintf(fp, " '");
		    fprintf(fp, skill_table[obj->value[3]].name);
		    fprintf(fp, "' : ");
		}
		fprintf(fp, " x : x : ");  /* for 5 fields */
		break;

	    case ITEM_WEAPON:
		if (obj->item_type == ITEM_WEAPON)
		    sprintf(buf, "ITEM_WEAPON : ");

		fprintf(fp, "Weapon type is ");
		switch (obj->value[0]) {
		case (WEAPON_EXOTIC):
		    fprintf(fp, "exotic : ");
		    break;
		case (WEAPON_SWORD):
		    fprintf(fp, "sword : ");
		    break;
		case (WEAPON_DAGGER):
		    fprintf(fp, "dagger : ");
		    break;
		case (WEAPON_SPEAR):
		    fprintf(fp, "spear/staff : ");
		    break;
		case (WEAPON_MACE):
		    fprintf(fp, "mace/club : ");
		    break;
		case (WEAPON_AXE):
		    fprintf(fp, "axe : ");
		    break;
		case (WEAPON_FLAIL):
		    fprintf(fp, "flail : ");
		    break;
		case (WEAPON_WHIP):
		    fprintf(fp, "whip : ");
		    break;
		case (WEAPON_POLEARM):
		    fprintf(fp, "polearm : ");
		    break;
		default:
		    fprintf(fp, "unknown : ");
		    break;
		}
		if (obj->pIndexData->new_format)
		    sprintf(buf, "Damage is %dd%d (average %d) : ",
			    obj->value[1], obj->value[2],
			    (1 + obj->value[2]) * obj->value[1] / 2);
		else
		    sprintf(buf, "Damage is %d to %d (average %d) : ",
			    obj->value[1], obj->value[2],
			    (obj->value[1] + obj->value[2]) / 2);
		fprintf(fp, buf);

		if (obj->value[4]) {	/* weapon flags */
		    sprintf(buf, "Weapons flags: %s : ", weapon_bit_name(obj->value[4]));
		    fprintf(fp, buf);
		}
		break;

	    case ITEM_ARMOR:
		if (obj->item_type == ITEM_ARMOR)
		    sprintf(buf, "ITEM_ARMOR : ");
		sprintf(buf,
			"Armor class is %d pierce : %d bash : %d slash : and %d vs. magic : ",
			obj->value[0], obj->value[1], obj->value[2], obj->value[3]);
		fprintf(fp, buf);
		break;
	    }			/* switch */

	    for (paf = obj->affected; paf != NULL; paf = paf->next) {
		sprintf(buf, "Affects %s by %d, level %d : ",
			affect_loc_name(paf->location), paf->modifier, paf->level);
		fprintf(fp, buf);
		/* added by Rahl */
		if (paf->duration > -1)
		    sprintf(buf, ", %d hours. : ", paf->duration);
		else
		    sprintf(buf, " : ");
		fprintf(fp, buf);
		if (paf->bitvector) {
		    switch (paf->where) {
		    case TO_AFFECTS:
			sprintf(buf, "Adds %s affect. : ",
				affect_bit_name(paf->bitvector));
			break;
		    case TO_WEAPON:
			sprintf(buf, "Adds %s weapon flags. : ",
				weapon_bit_name(paf->bitvector));
			break;
		    case TO_OBJECT:
			sprintf(buf, "Adds %s object flag. : ",
				extra_bit_name(paf->bitvector));
			break;
		    case TO_IMMUNE:
			sprintf(buf, "Adds immunity to %s. : ",
				imm_bit_name(paf->bitvector));
			break;
		    case TO_RESIST:
			sprintf(buf, "Adds resistance to %s. : ",
				imm_bit_name(paf->bitvector));
			break;
		    case TO_VULN:
			sprintf(buf, "Adds vulnerability to %s. : ",
				imm_bit_name(paf->bitvector));
			break;
		    default:
			sprintf(buf, "Unknown bit %d %d : ",
				paf->where, paf->bitvector);
			break;
		    }
		    fprintf(fp, buf);
		}		/* if */
	    }			/* for */

	    if (!obj->enchanted)
		for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) {
		    sprintf(buf, "Affects %s by %d, level %d. : ",
			    affect_loc_name(paf->location), paf->modifier, paf->level);
		    fprintf(fp, buf);
		    if (paf->bitvector) {
			switch (paf->where) {
			case TO_AFFECTS:
			    sprintf(buf, "Adds %s affect. : ",
				    affect_bit_name(paf->bitvector));
			    break;
			case TO_WEAPON:
			    sprintf(buf, "Adds %s weapon flags. : ",
				    weapon_bit_name(paf->bitvector));
			    break;
			case TO_OBJECT:
			    sprintf(buf, "Adds %s object flag. : ",
				    extra_bit_name(paf->bitvector));
			    break;
			case TO_IMMUNE:
			    sprintf(buf, "Adds immunity to %s. : ",
				    imm_bit_name(paf->bitvector));
			    break;
			case TO_RESIST:
			    sprintf(buf, "Adds resistance to %s. : ",
				    imm_bit_name(paf->bitvector));
			    break;
			case TO_VULN:
			    sprintf(buf, "Adds vulnerability to %s. : ",
				    imm_bit_name(paf->bitvector));
			    break;
			default:
			    sprintf(buf, "Unknown bit %d %d : ",
				    paf->where, paf->bitvector);
			    break;
			}	/* switch */
			fprintf(fp, buf);
		    }		/* if */
		}		/* for */
	    fprintf(fp, "\n");
	    extract_obj(obj);

	}			/* if */
    /* close file */
    fclose(fp);

    fpReserve = fopen(NULL_FILE, "r");

    send_to_char("Done writing files...\n\r", ch);
}
