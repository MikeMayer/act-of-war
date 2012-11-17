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
#include <string.h>
#include <time.h>
#include <sqlite3.h>
#include "merc.h"
#include "signal.h"
#include "magic.h"

DECLARE_DO_FUN(do_circle);
DECLARE_DO_FUN(do_dragonsword);
DECLARE_DO_FUN(do_backstab);
DECLARE_DO_FUN(do_emote);
DECLARE_DO_FUN(do_berserk);
DECLARE_DO_FUN(do_bash);
DECLARE_DO_FUN(do_trip);
DECLARE_DO_FUN(do_dirt);
DECLARE_DO_FUN(do_flee);
DECLARE_DO_FUN(do_kick);
DECLARE_DO_FUN(do_disarm);
DECLARE_DO_FUN(do_get);
DECLARE_DO_FUN(do_recall);
DECLARE_DO_FUN(do_yell);
DECLARE_DO_FUN(do_sacrifice);
DECLARE_DO_FUN(do_stun);
DECLARE_DO_FUN(do_trap);
DECLARE_DO_FUN(do_jump);

/*
 * Local functions.
 */
int  kingdom_xp         args((CHAR_DATA * ch, int xp ));
void raid               args((CHAR_DATA * ch, CHAR_DATA * victim  ));
void check_assist       args((CHAR_DATA * ch, CHAR_DATA * victim));
bool check_dodge        args((CHAR_DATA * ch, CHAR_DATA * victim));
void check_killer       args((CHAR_DATA * ch, CHAR_DATA * victim));
bool check_parry        args((CHAR_DATA * ch, CHAR_DATA * victim));
bool check_shield_block args((CHAR_DATA * ch, CHAR_DATA * victim));
void dam_message        args((CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, bool immune));
void death_cry          args((CHAR_DATA * ch, CHAR_DATA * killer));
void trophy_gain        args((CHAR_DATA * ch, CHAR_DATA * victim, int isLgg));
void group_gain         args((CHAR_DATA * ch, CHAR_DATA * victim, int isLgg));
int  xp_compute         args((CHAR_DATA * gch, CHAR_DATA * victim, int total_levels, bool grouped));
bool is_safe            args((CHAR_DATA * ch, CHAR_DATA * victim));
void make_corpse        args((CHAR_DATA * ch, CHAR_DATA * killer));
void one_hit            args((CHAR_DATA * ch, CHAR_DATA * victim, int dt, bool secondary));
void mob_hit            args((CHAR_DATA * ch, CHAR_DATA * victim, int dt));
void raw_kill           args((CHAR_DATA * victim, CHAR_DATA * ch, int isLgg));
void set_fighting       args((CHAR_DATA * ch, CHAR_DATA * victim));
void disarm             args((CHAR_DATA * ch, CHAR_DATA * victim));
void stop_hatin         args((CHAR_DATA * ch));
bool is_hating          args((CHAR_DATA * ch, CHAR_DATA * victim));
void calc_warpoints     args((CHAR_DATA * winner, CHAR_DATA * loser));
int calc_isLgg			args((CHAR_DATA * ch, CHAR_DATA * victim));

extern WARSCORE_DATA *main_warscore;
extern CHAR_DATA * quest_target;
extern CHAR_DATA * quest_mob;

//retunrs false if its a clean fight, true if its an lgg.
int calc_isLgg(CHAR_DATA * ch, CHAR_DATA * victim)
{
    DESCRIPTOR_DATA *d;
	CHAR_DATA *subject;
	int people_count = 0;
   	//int killer_count = 0;
	//int killed_count = 0;

	if(IS_NPC(victim))
		return 0;


//should we tally up peoplecount?
    for (d = descriptor_list; d; d = d->next) 
	{

	if (//(d->connected == CON_PLAYING)  
	     ((subject = d->character) != NULL)
	    && (subject->in_room != NULL)
	    && (subject->in_room->area == ch->in_room->area)
		&& (GET_SIDE(subject) == GET_SIDE(ch)) 
		&& (GET_SIDE(ch) != GET_SIDE(victim)) 
	    && (subject->warpoints > 3)
		&& !(ch->in_room->vnum/1000 == 28) //not in cairn
		&& !(ch->in_room->vnum/100  == 202 || ch->in_room->vnum/100  == 203)//!kael
		&& !(ch->in_room->vnum/100  == 158 || ch->in_room->vnum/100  == 159)//!tier
		&& !(ch->in_room->vnum/100  == 204 || ch->in_room->vnum/100  == 300)//!um
//		&& !(ch->in_room->vnum/100  == 2)//!saltmines.. (quest)
		)
		{
			people_count++;
		}
	}

//for testing
	if(people_count > MAX_GROUP_SIZE)
	send_to_char("that was lgg!!!\n\r", ch);

	if(people_count > MAX_GROUP_SIZE)
	    news("LGG!", ch->side);
	if(people_count > MAX_GROUP_SIZE)
	news("LGG!", victim->side);
//--

	if(people_count > MAX_GROUP_SIZE)
		return people_count;
/*
	//should we tally up peoplecount on killer side?
    for (d = descriptor_list; d; d = d->next)
	{

	if (//(d->connected == CON_PLAYING)
	     ((subject = d->character) != NULL)
	    && (subject->in_room != NULL)
	    && (subject->in_room->area == ch->in_room->area)
		&& (GET_SIDE(subject) == GET_SIDE(ch))
		&& (GET_SIDE(ch) != GET_SIDE(victim))
		&& (!is_same_group(d->character, ch))
	    && (subject->warpoints > 3)
		&& !(ch->in_room->vnum/1000 == 28) //Cairn
		&& !(ch->in_room->vnum/100  == 202 || ch->in_room->vnum/100  == 203)//Kael
		&& !(ch->in_room->vnum/100  == 158 || ch->in_room->vnum/100  == 159)//Tier
		&& !(ch->in_room->vnum/100  == 204 || ch->in_room->vnum/100  == 300)//UM
		)
		{
			killer_count++;
		}
	}

   for (d = descriptor_list; d; d = d->next)
       {
         if(is_same_group(d->character, ch))
		killer_count++;
	}
//should we tally up peoplecount on killed side?
    for (d = descriptor_list; d; d = d->next)
	{

	if (//(d->connected == CON_PLAYING)
	     ((subject = d->character) != NULL)
	    && (subject->in_room != NULL)
	    && (subject->in_room->area == ch->in_room->area)
		&& (GET_SIDE(subject) == GET_SIDE(victim))
		&& (GET_SIDE(ch) != GET_SIDE(victim))
		&& (!is_same_group(d->character, victim))
	    && (subject->warpoints > 3)
		&& !(ch->in_room->vnum/1000 == 28) //Cairn
		&& !(ch->in_room->vnum/100  == 202 || ch->in_room->vnum/100  == 203)//Kael
		&& !(ch->in_room->vnum/100  == 158 || ch->in_room->vnum/100  == 159)//Tier
		&& !(ch->in_room->vnum/100  == 204 || ch->in_room->vnum/100  == 300)//UM
		)
		{
			killed_count++;
		}
	}
	for (d = descriptor_list; d; d = d->next)
       {
         if(is_same_group(d->character, victim))
                killed_count++;
	}
	if(killed_count > 2 && (killer_count >= 2* killed_count))
	send_to_char("That was gay!\n\r", ch);

	if(killed_count > 2 && (killer_count >= 2* killed_count))
	     news("GAY!", ch->side);

	if(killer_count > 2 && (killer_count >= 2* killed_count))
	         return (killer_count - killed_count);
*/
	
	return 0;

}


void raid(CHAR_DATA * ch, CHAR_DATA * victim  )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *gch;
    extern WARSCORE_DATA *main_warscore;
    char buf[MAX_STRING_LENGTH];
    int i;
    int raidchs = 0;
    int xptotal = 0; //Adding XP for raids, starts at 100 XP
    
   CHAR_DATA *subject;
		
		    for (d = descriptor_list; d; d = d->next) // first, the victim side
	{

	if ( ((subject = d->character) != NULL)
	    && (subject->in_room != NULL)
	    && (GET_SIDE(subject) == GET_SIDE(victim))
	    && (!IS_NPC(subject)) //Only matters for NPC groupies, killers, and victims 
	    && (!IS_NPC(ch)) ) // Totalling up all playing chars on victim side.
 	
	    xptotal += 0; //50 XP per logged in victim side char.
	    }
	    
	    		    for (d = descriptor_list; d; d = d->next) // first, the killer group
	{

	if ( ((subject = d->character) != NULL)
	    && (subject->in_room != NULL)
	    && (GET_SIDE(subject) == GET_SIDE(ch))
	    && (!IS_NPC(subject))
	    && (!IS_NPC(ch))
		&& (is_same_group(subject,ch)) )
		raidchs += 1;
	}

    if(IS_SET(victim->in_room->affected_by, AFF_RAIDED))
    {
	send_to_char("This town is still recovering from the last raid.\n\r", ch);
	return;
    }

    //group
    for (d = descriptor_list; d; d = d->next)
        if (d->connected == CON_PLAYING) {
	    gch = d->character;
	    if (is_same_group(d->character, ch) && d->character->in_room->area == ch->in_room->area) {
	        send_to_char("You raided!\n\r", gch);
	        gch->raids++;
	        gch->warpoints++;
	        /*sprintf(buf, "You gained %d experience points for raiding!!\n\r",100/raidchs);
	        gain_exp(gch,100/raidchs);
	        send_to_char(buf,gch);
	        if (xptotal > 100){
	        sprintf(buf, "{YBONUS!! {XYou gain %d bonus experience for %d enemy players on!\n\r",
	        (xptotal - 100)/raidchs, //first variable
	        (xptotal - 100)/xptotal); // second variable
	        send_to_char(buf,gch);
	        gain_exp(gch,(xptotal-100)/raidchs);}
	        save_char_obj(gch);*/
	    }
        }       //end group

    for (i = 0; i < MAX_KINGDOM; i++) {
	sprintf(buf, "Cries of agony are heard worldwide!! %s has been raided by %s!!!", 
		kingdom_table[victim->side].name,
		kingdom_table[ch->side].name);
	news(buf, i);
    }
    
    {
        if (ch->side == SIDE_TIER)
	    main_warscore->tier_raids++;
        if (ch->side == SIDE_KAEL)
 	    main_warscore->kael_raids++;
        if (ch->side == SIDE_UM)
	    main_warscore->um_raids++;
        if (ch->side == SIDE_CAIRN)
	    main_warscore->cairn_raids++;
        if (victim->side == SIDE_TIER)
	    main_warscore->tier_been_raided++;
        if (victim->side == SIDE_KAEL)
	    main_warscore->kael_been_raided++;
        if (victim->side == SIDE_UM)
	    main_warscore->um_been_raided++;
        if (victim->side == SIDE_CAIRN)
	    main_warscore->cairn_been_raided++;
        save_warscore();
    }

    {
        AFFECT_DATA af;
        af.where = TO_AFFECTS;
        af.level = 100;
        af.duration = 120;
        af.bitvector = AFF_RAIDED;
        affect_to_room(victim->in_room, &af);
    }

    return;

}   //end raid


//kingdom xp modifier
int kingdom_xp(CHAR_DATA * ch, int xp )
{
//  null kingdom,       	tier, 	kael, 	um, 	cairn,    3 extras    ...  100 is standard
    const int base[] = { 0 , 	200 , 	200 , 	200 , 	200,    0,0,0}; //manual xp balance percentwise
    int penalty_at = 1500;     //if the kingdom gets above the average by this much...
    int penalty_amount = 33;   //mob xp gets reduced to this percent
    int x1, x2, x3, x4, temp;

    //char buf[MAX_STRING_LENGTH];

    //sprintf(buf, "%d.\n\r", xp);
    //send_to_char(buf, ch);

//    return xp;

    xp = (base[ch->side] * xp) / 100;
    //sprintf(buf, "%d.\n\r", xp);
    //send_to_char(buf, ch);


//calculate warscore, we seem to have no globals
    x1 = (main_warscore->tier_killed          - main_warscore->tier_lost) +
	 (main_warscore->tier_legends_killed  - main_warscore->tier_legends_lost) +
	 (main_warscore->tier_raids           - main_warscore->tier_been_raided) * 1;
    x2 = (main_warscore->kael_killed          - main_warscore->kael_lost) +
	 (main_warscore->kael_legends_killed  - main_warscore->kael_legends_lost) +
	 (main_warscore->kael_raids           - main_warscore->kael_been_raided) * 1;
    x3 = (main_warscore->um_killed            - main_warscore->um_lost) +
	 (main_warscore->um_legends_killed    - main_warscore->um_legends_lost) +
	 (main_warscore->um_raids             - main_warscore->um_been_raided) * 1;
    x4 = (main_warscore->cairn_killed         - main_warscore->cairn_lost) +
	 (main_warscore->cairn_legends_killed - main_warscore->cairn_legends_lost) +
	 (main_warscore->cairn_raids          - main_warscore->cairn_been_raided) * 1;

    switch(ch->side) {
    case SIDE_TIER:  temp = x1; break;
    case SIDE_KAEL:  temp = x2; break;
    case SIDE_UM:    temp = x3; break;
    case SIDE_CAIRN: temp = x4; break;
    default:         return 0;  break;
    }

//    if( temp + penalty_at > (x1 + x2 + x3 + x4) / 4)

    if (temp > penalty_at) {			
	xp = (xp * penalty_amount) / 100;
    }

    //sprintf(buf, "%d.\n\r", xp);
    //send_to_char(buf, ch);

    return xp;
}

/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update(void)
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;
    int jumpchance=0;
    int jumptwo=0;
    int song=0;

    for (ch = char_list; ch != NULL; ch = ch->next) {
	ch_next = ch->next;
	
	// crappy ass bard songs.
	
	if (is_affected(ch, gsn_life_song) 
	&& !IS_SET(ch->in_room->affected_by, AFF_SILENT_ROOM)
	&& !IS_AFFECTED(ch, AFF_SLEEP) )
	{
		ch->hit += number_range(ch->level/2,ch->level*2);
		if (ch->hit > ch->max_hit)
		ch->hit = ch->max_hit;
		ch->move += number_range(ch->level/3, ch->level/2);
		song = number_range(0,5);
		
		switch (song) {
			case 0:
			send_to_char("You sing, 'Come gather around to me as I sing the song that will lift your spirits!'\n\r",ch);
			act("$n sings, 'Come gather around to me as I sing the song that will lift your spirits!'", ch, NULL, NULL, TO_ROOM);
			break;
			case 1:
			send_to_char("You sing, 'Closer to me as I lift your heavy heart and soften your spirit!'\n\r",ch);
			act("$n sings, 'Closer to me as I lift your heavy heart and soften your spirit!'", ch, NULL, NULL, TO_ROOM);
			break;
			case 2:
			send_to_char("You sing, 'Harken to my song I lift your burden and make you unweary!'\n\r",ch);
			act("$n sings, 'Harken to my song I lift your burden and make you unweary!'", ch, NULL, NULL, TO_ROOM);
			break;
			case 3:
			send_to_char("You sing, 'Raise your voice with mine and let them entertwine and heal your body!'\n\r",ch);
			act("$n sings, 'Raise your voice with mine and let them entertwine and heal your body!'", ch, NULL, NULL, TO_ROOM);
			break;
			case 4:
			send_to_char("You sing, 'Listen to my voice It will soothe your mood!'\n\r",ch);
			act("$n sings, 'Listen to my voice It will soothe your mood!'", ch, NULL, NULL, TO_ROOM);
			break;
			default:
			send_to_char("You sing, 'Join with me and hear the hymn of healing!'\n\r",ch);
			act("$n sings, 'Join with me and hear the hymn of healing!'", ch, NULL, NULL, TO_ROOM);
			break;
		}
	}
		
	if (is_affected(ch, gsn_magic_song) 
	&& !IS_SET(ch->in_room->affected_by, AFF_SILENT_ROOM)
	&& !IS_AFFECTED(ch, AFF_SLEEP) )
	{
		ch->mana += number_range(ch->level/4,ch->level/3);
		song = number_range(0,5);
		
		switch (song) {
			case 0:
			send_to_char("You sing, 'Listen to this song you will dance into its throng!'\n\r",ch);
			act("$n sings, 'Listen to this song you will dance into its throng!'", ch, NULL, NULL, TO_ROOM);
			break;
			case 1:
			send_to_char("You sing, 'Rejoice for I have come, listen to my drum!'\n\r",ch);
			act("$n sings, 'Rejoice for I have come, listen to my drum!'", ch, NULL, NULL, TO_ROOM);
			break;
			case 2:
			send_to_char("You sing, 'The water gathers round and the elements twirl around you feel your mana rise!'\n\r",ch);
			act("$n sings, 'The water gathers round and the elements twirl around you feel your mana rise!'", ch, NULL, NULL, TO_ROOM);
			break;
			case 3:
			send_to_char("You sing, 'The song fills you with pride and your burden becomes light!'\n\r",ch);
			act("$n sings, 'The song fills you with pride and your burden becomes light!'", ch, NULL, NULL, TO_ROOM);
			break;
			default:
			send_to_char("You sing, 'Now listen to this hymn and get ready to go and fight!'\n\r",ch);
			act("$n sings, 'Now listen to this hymn and get ready to go and fight!'", ch, NULL, NULL, TO_ROOM);
			break;
			}
		}
		
		if (is_affected(ch, gsn_speed_song)
		&& !IS_SET(ch->in_room->affected_by, AFF_SILENT_ROOM)
	&& !IS_AFFECTED(ch, AFF_SLEEP) )
	{
		if (get_skill(ch,gsn_speed_song)>0){
		ch->mana -= number_range(3,6);
		ch->move -= number_range(3,6);
		song = number_range(0,5);
		
		switch (song) {
			case 0:
			send_to_char("You sing, 'Quicken your steps my children, the battle must be joined..'\n\r",ch);
			act("$n sings, 'Quicken your steps my children, the battle must be joined..'", ch, NULL, NULL, TO_ROOM);
			break;
			case 1:
			send_to_char("You sing, 'Lighten your footsteps, the fight is upon us...'\n\r",ch);
			act("$n sings, 'Lighten your footsteps, the fight is upon us...'", ch, NULL, NULL, TO_ROOM);
			break;
			case 2:
			send_to_char("You sing, 'Upon the breath of the wind we shall tread...'\n\r",ch);
			act("$n sings, 'Upon the breath of the wind we shall tread...'", ch, NULL, NULL, TO_ROOM);
			break;
			case 3:
			send_to_char("You sing, 'Feel the power of the Hurricane...'\n\r",ch);
			act("$n sings, 'Feel the power of the Hurricane...'", ch, NULL, NULL, TO_ROOM);
			break;
			case 4:
			send_to_char("You sing, 'Harness the spin of the Tornado...'\n\r",ch);
			act("$n sings, 'Harness the spin of the Tornado...'", ch, NULL, NULL, TO_ROOM);
			break;
			default:
			send_to_char("You sing, 'Dance the whirl of blades, Dance the dance of Death...'\n\r",ch);
			act("$n sings, 'Dance the whirl of blades, Dance the dance of Death...'", ch, NULL, NULL, TO_ROOM);
			break;
		}
	}
}
	
		if (is_affected(ch, gsn_attack_song) 
		&& !IS_SET(ch->in_room->affected_by, AFF_SILENT_ROOM)
	&& !IS_AFFECTED(ch, AFF_SLEEP) )
	{
		if (get_skill(ch,gsn_attack_song)>0){
		ch->mana -= number_range(10,20);
		ch->move -= number_range(5,8);
		song = number_range(0,5);
		
		switch (song) {
			case 0:
			send_to_char("You sing, 'The march begins, you shall be punished for your sins...'\n\r",ch);
			act("$n sings, 'The march begins, you shall be punished for your sins...'", ch, NULL, NULL, TO_ROOM);
			break;
			case 1:
			send_to_char("You sing, 'We come with might, we do not fear the night...'\n\r",ch);
			act("$n sings, 'We come with might, we do not fear the night...'", ch, NULL, NULL, TO_ROOM);
			break;
			case 2:
			send_to_char("You sing, 'our swords are drawn, our fear is gone...'\n\r",ch);
			act("$n sings, 'Our swords are drawn, our fear is gone...'", ch, NULL, NULL, TO_ROOM);
			break;
			case 3:
			send_to_char("You sing, 'The rage hath come, your reign is undone...'\n\r",ch);
			act("$n sings, 'The rage hath come, your reign is undone...'", ch, NULL, NULL, TO_ROOM);
			break;
			case 4:
			send_to_char("You sing, 'Our blows will rend, now is the time of your end...'\n\r",ch);
			act("$n sings, 'Our blows will rend, now is the time of your end...'", ch, NULL, NULL, TO_ROOM);
			break;
			default:
			send_to_char("You sing, 'Your blood shall fly, it is now time for you to DIE!'\n\r",ch);
			act("$n sings, 'Your blood shall fly, it is now time for you to DIE!'", ch, NULL, NULL, TO_ROOM);
			break;
		}
	}
}
		if (is_affected(ch, gsn_learning_song) 
		&& !IS_SET(ch->in_room->affected_by, AFF_SILENT_ROOM)
	&& !IS_AFFECTED(ch, AFF_SLEEP) )
	{
		if (get_skill(ch,gsn_learning_song)>0){
		ch->mana -= number_range(3,6);
		ch->move -= number_range(3,6);
		song = number_range(0,5);
		
		switch (song) {
			case 0:
			send_to_char("You sing, 'Here I sing the song of learning..'\n\r",ch);
			act("$n sings, 'Here I sing the song of learning..'", ch, NULL, NULL, TO_ROOM);
			break;
			case 1:
			send_to_char("You sing, 'So that all might quench the burning..'\n\r",ch);
			act("$n sings, 'So that all might quench the burning..'", ch, NULL, NULL, TO_ROOM);
			break;
			case 2:
			send_to_char("You sing, 'The burning of their thoughtful mind..'\n\r",ch);
			act("$n sings, 'The burning of their thoughtful mind..'", ch, NULL, NULL, TO_ROOM);
			break;
			case 3:
			send_to_char("You sing, 'Let my soul find the skill!'\n\r",ch);
			act("$n sings, 'Let my soul find the skill!'", ch, NULL, NULL, TO_ROOM);
			break;
			case 4:
			send_to_char("You sing, 'And my body have the will!'\n\r",ch);
			act("$n sings, 'And my body have the will!'", ch, NULL, NULL, TO_ROOM);
			break;
			default:
			send_to_char("You sing, 'So that I might conquer all the enemies that I find!'\n\r",ch);
			act("$n sings, 'So that I might conquer all the enemies that I find!'", ch, NULL, NULL, TO_ROOM);
			break;
		}
	}
}
		if (is_affected(ch, gsn_defense_song)
		&& !IS_SET(ch->in_room->affected_by, AFF_SILENT_ROOM)
	&& !IS_AFFECTED(ch, AFF_SLEEP) )
	{
			    AFFECT_DATA af;
	 		af.where = TO_AFFECTS;
	 		af.type = gsn_sung_defense;
	 		af.level = ch->level;
	 		af.duration = 1;
	 		af.bitvector = 0;
	 		af.location = APPLY_SAVING_SPELL;
	 		af.modifier =number_range(-2,-4);
			affect_to_char(ch, &af);
			af.location = APPLY_AC;
	 		af.modifier = -5;
			affect_to_char(ch, &af);
		
		if (get_skill(ch,gsn_defense_song)>0 && is_affected(ch,gsn_defense_song)){
		ch->mana -= number_range(6,9);
		ch->move -= number_range(4,7);
		song = number_range(0,5);
		
		switch (song) {
			case 0:
			send_to_char("You sing, 'Night or day, We will hold the way...'\n\r",ch);
			act("$n sings, 'Night or day, We will hold the way...'", ch, NULL, NULL, TO_ROOM);
			break;
			case 1:
			send_to_char("You sing, 'The ground shakes, but we shall not quake...'\n\r",ch);
			act("$n sings, 'The ground shakes, but we shall not quake...'", ch, NULL, NULL, TO_ROOM);
			break;
			case 2:
			send_to_char("You sing, 'Our enemy is near, but never shall we fear...'\n\r",ch);
			act("$n sings, 'Our enemy is near, but never shall we fear...'", ch, NULL, NULL, TO_ROOM);
			break;
			case 3:
			send_to_char("You sing, 'We must hold the way, we must win the day...'\n\r",ch);
			act("$n sings, 'We must hold the way, we must win the day...'", ch, NULL, NULL, TO_ROOM);
			break;
			case 4:
			send_to_char("You sing, 'By the mountains's strength, noone shall ever passs...'\n\r",ch);
			act("$n sings, 'By the mountains's strength, noone shall ever passs...'", ch, NULL, NULL, TO_ROOM);
			break;
			default:
			send_to_char("You sing, 'We are the wall and we shall NEVER fall!'\n\r",ch);
			act("$n sings, 'We are the wall and we shall NEVER fall!'", ch, NULL, NULL, TO_ROOM);
			break;
		}
	}
}
	
	//ch5 necro spells, hopefully
			if ( is_affected(ch, skill_lookup("mana burn")) && ch->mana >= 20)
{
	if (IS_AFFECTED(ch, AFF_SLEEP))
	affect_strip(ch, gsn_sleep);

	send_to_char("You feel a deep pain in your chest.\n\r",ch);
	ch->mana -= number_fuzzy(13);
	if (ch->mana <= 0)
	ch->mana = 1;
}
	if ( is_affected(ch, skill_lookup("life drain")) && ch->hit >= 100)
{
	if (IS_AFFECTED(ch, AFF_SLEEP))
	affect_strip(ch, gsn_sleep);

	send_to_char("You feel as if you are being drained.\n\r",ch);
	ch->hit -= number_fuzzy(35);
	ch->move -= number_fuzzy(6);
	if (ch->hit <= 0)
	ch->hit = 1;
	if (ch->move <= 0)
	ch->move = 1;
}
	

	if (IS_NPC(ch) && ch->fighting == NULL && IS_AWAKE(ch) && ch->hunting != NULL) {
	    hunt_victim(ch);
	    continue;
	}
	if (IS_NPC(ch) && ch->fighting == NULL && IS_AWAKE(ch) && ch->hunting == NULL && ch->dir_stack != NULL) {
	    hunt_victim(ch);
	    continue;
	}
	if ((victim = ch->fighting) == NULL || ch->in_room == NULL)
	    continue;

	if (IS_AWAKE(ch) && ch->in_room == victim->in_room)
	    multi_hit(ch, victim, TYPE_UNDEFINED);
	else
	    stop_fighting(ch, FALSE);
	    
	if ((victim = ch->fighting) == NULL)
	    continue;
	    
	    // Jump ? Maybe
	    
	if (!IS_NPC(ch) && is_affected(ch,gsn_jump)) {
		jumpchance = number_percent();
		jumptwo = number_percent();
			if (jumpchance >= 50 && ((victim = ch->fighting) != NULL))
			{
				act("$n comes flashing down from the sky, striking you HARD!", ch, NULL, victim, TO_VICT);
				act("You fall back to the ground with vast speeds, targetting $N!", ch, NULL, victim, TO_CHAR);
				act("$n falls from the sky at a huge speed, striking $N!", ch, NULL, victim, TO_NOTVICT);
				one_hit(ch, victim, gsn_jump, FALSE);
				affect_strip(ch, gsn_jump);
			}
			else if ((jumpchance < 50) && 
			((jumptwo < 25) || ((victim = ch->fighting) == NULL)))
			{
				act("You fall back to the ground with vast speeds, and don't hit squat!!", ch, NULL, NULL, TO_CHAR);
				act("$n falls from the sky at a huge speed, striking.... the ground.", ch, NULL, NULL, TO_NOTVICT);
				ch->hit -= 150;
				affect_strip(ch, gsn_jump);
			}
		}

	/*
	 * Fun for the whole family!
	 */
	check_assist(ch, victim);

	if (IS_NPC(ch)) {
	    if (HAS_TRIGGER(ch, TRIG_FIGHT))
		mp_percent_trigger(ch, victim, NULL, NULL, TRIG_FIGHT);
	    if (HAS_TRIGGER(ch, TRIG_HPCNT))
		mp_hprct_trigger(ch, victim);
	}
    }

    return;
}

/* for auto assisting */

void check_assist(CHAR_DATA * ch, CHAR_DATA * victim)
{
    CHAR_DATA *rch, *rch_next;

    for (rch = ch->in_room->people; rch != NULL; rch = rch_next) {
	rch_next = rch->next_in_room;

	if (IS_AWAKE(rch) && rch->fighting == NULL) {
	    /* quick check for ASSIST_PLAYER */
	    if (!IS_NPC(ch) && IS_NPC(rch) && IS_SET(rch->off_flags, ASSIST_PLAYERS) && rch->level + 6 > victim->level) {
		do_emote(rch, "screams and attacks!");
		multi_hit(rch, victim, TYPE_UNDEFINED);
		continue;
	    }
	    /* PCs next */
	    if (!IS_NPC(ch) || IS_AFFECTED(ch, AFF_CHARM)) {
		if (((!IS_NPC(rch) && IS_SET(rch->act, PLR_AUTOASSIST)) || IS_AFFECTED(rch, AFF_CHARM))
		    && is_same_group(ch, rch) && !is_safe(rch, victim) && rch->side != victim->side)
		    multi_hit(rch, victim, TYPE_UNDEFINED);

		continue;
	    }
	    /* now check the NPC cases */

	    if (IS_NPC(ch) && !IS_AFFECTED(ch, AFF_CHARM)) {
		if ((IS_NPC(rch) && IS_SET(rch->off_flags, ASSIST_ALL))
		    || (IS_NPC(rch) && rch->group && rch->group == ch->group)
		    || (IS_NPC(rch) && rch->race == ch->race && IS_SET(rch->off_flags, ASSIST_RACE))
		    || (IS_NPC(rch) && IS_SET(rch->off_flags, ASSIST_ALIGN)
		    && ((IS_GOOD(rch) && IS_GOOD(ch))
		    || (IS_EVIL(rch) && IS_EVIL(ch))
		    || (IS_NEUTRAL(rch) && IS_NEUTRAL(ch))))
		    || (rch->pIndexData == ch->pIndexData && IS_SET(rch->off_flags, ASSIST_VNUM))) {
		    CHAR_DATA *vch;
		    CHAR_DATA *target;
		    int number;

		    if (number_bits(1) == 0)
			continue;

		    target = NULL;
		    number = 0;
		    for (vch = ch->in_room->people; vch; vch = vch->next) {
			if (can_see(rch, vch)
			    && is_same_group(vch, victim)
			    && number_range(0, number) == 0) {
			    target = vch;
			    number++;
			}
		    }

		    if (target != NULL) {
			do_emote(rch, "screams and attacks!");
			multi_hit(rch, target, TYPE_UNDEFINED);
		    }
		}
	    }
	}
    }
}

/*
 * Do one group of attacks.
 */

void multi_hit(CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
    int chance;

    if (ch->fighting == NULL)
	stop_fighting(ch, FALSE);
    if (IS_NPC(victim) && IS_SET(victim->act, ACT_NOFIGHT))
	return;

    if (IS_NPC(ch) && IS_SET(ch->act, ACT_NOFIGHT))
	return;

    /* decrement the wait */
    if (ch->desc == NULL)
	ch->wait = UMAX(0, ch->wait - PULSE_VIOLENCE);

    if (ch->desc == NULL)
	ch->daze = UMAX(0, ch->daze - PULSE_VIOLENCE);

    /* no attacks for stunnies -- just a check */
    if (ch->position < POS_RESTING)
	return;

    /* if character is standing, set pos to pos_fighting */
    if (ch->position == POS_STANDING)
	ch->position = POS_FIGHTING;

    if (IS_NPC(ch)) {
	mob_hit(ch, victim, dt);
	return;
    }
    one_hit(ch, victim, dt, FALSE);

    chance = get_skill(ch, gsn_dual_wield);

    if (get_eq_char(ch, WEAR_SECONDARY) && (number_percent() < chance) && (dt != gsn_backstab || dt != gsn_circle || gsn_dragonsword || gsn_jump)) {
	one_hit(ch, victim, dt, TRUE);
	check_improve(ch, gsn_dual_wield, TRUE, 5);
	if (ch->fighting != victim)
	    return;
    }
    if (ch->fighting != victim)
	return;

    if (IS_AFFECTED(ch, AFF_HASTE)) {
	if (dt != gsn_circle)
	    one_hit(ch, victim, dt, FALSE);

	if (get_eq_char(ch, WEAR_SECONDARY) && number_percent() < chance && (dt != gsn_backstab || dt != gsn_circle || dt != gsn_jump)) {
	    one_hit(ch, victim, dt, TRUE);
	    if (ch->fighting != victim)
		return;
	}
    }
    if (ch->fighting != victim || dt == gsn_backstab || dt == gsn_circle || dt == gsn_dragonsword || dt == gsn_jump)
	return;

    chance = get_skill(ch, gsn_second_attack) / 2;
    chance += get_skill(ch, gsn_fencing) / 4;
    check_improve(ch, gsn_fencing, TRUE, 5);

    if (IS_AFFECTED(ch, AFF_SLOW))
	chance /= 2;

    if (number_percent() < chance) {
	
one_hit(ch, victim, dt, FALSE);
	check_improve(ch, gsn_second_attack, TRUE, 5);
	if (ch->fighting != victim)
	    return;
    }
    chance = get_skill(ch, gsn_third_attack) / 4;
    chance += get_skill(ch, gsn_fencing) / 4;

    if (IS_AFFECTED(ch, AFF_SLOW))
	chance /= 2;

    if (number_percent() < chance) {
	one_hit(ch, victim, dt, FALSE);
	check_improve(ch, gsn_third_attack, TRUE, 6);
	if (ch->fighting != victim)
	    return;
    }
    chance = get_skill(ch, gsn_fourth_attack) / 4;
    chance += get_skill(ch, gsn_fencing) / 4;

    if (is_affected(ch, skill_lookup("decripify")))
	chance = 10;
    
    if (IS_AFFECTED(ch, AFF_SLOW))
	chance = 0;

    if (number_percent() < chance) {
	one_hit(ch, victim, dt, FALSE);
	check_improve(ch, gsn_fourth_attack, TRUE, 6);
	if (ch->fighting != victim)
	    return;
    }
    chance = get_skill(ch, gsn_fifth_attack) / 4;
    chance += get_skill(ch, gsn_fencing) / 4;

    if (is_affected(ch, skill_lookup("decripify")))
	chance = 10;
    
    if (IS_AFFECTED(ch, AFF_SLOW))
	chance = 0;

    if (number_percent() < chance) {
	one_hit(ch, victim, dt, FALSE);
	check_improve(ch, gsn_fifth_attack, TRUE, 6);
	if (ch->fighting != victim)
	    return;
    }
    return;
}

/* procedure for all mobile attacks */

void mob_hit(CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
    int chance, number;
    CHAR_DATA *vch, *vch_next;

    one_hit(ch, victim, dt, FALSE);

    if (ch->fighting != victim)
	return;

    /* Area attack -- BALLS nasty! */

    if (IS_SET(ch->off_flags, OFF_AREA_ATTACK)) {
	for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
	    vch_next = vch->next;
	    if ((vch != victim && vch->fighting == ch))
		one_hit(ch, vch, dt, FALSE);
	}
    }
    if (IS_AFFECTED(ch, AFF_HASTE) || (IS_SET(ch->off_flags, OFF_FAST) && !IS_AFFECTED(ch, AFF_SLOW)))
	one_hit(ch, victim, dt, FALSE);

    if (ch->fighting != victim || dt == gsn_backstab)
	return;

	if (ch->fighting != victim || dt == gsn_jump)
	return;
	
    chance = get_skill(ch, gsn_second_attack) / 2;
    chance += get_skill(ch, gsn_fencing) / 4;

    if (is_affected(ch, skill_lookup("decripify")))
	chance = 10;
    
    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_SET(ch->off_flags, OFF_FAST))
	chance /= 2;

    if (number_percent() < chance) {
	one_hit(ch, victim, dt, FALSE);
	if (ch->fighting != victim)
	    return;
    }
    chance = get_skill(ch, gsn_third_attack) / 4;
    chance += get_skill(ch, gsn_fencing) / 4;

    if (is_affected(ch, skill_lookup("decripify")))
	chance = 10;
    
    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_SET(ch->off_flags, OFF_FAST))
	chance = 0;

    if (number_percent() < chance) {
	one_hit(ch, victim, dt, FALSE);
	if (ch->fighting != victim)
	    return;
    }
    chance = get_skill(ch, gsn_fourth_attack) / 4;
    chance += get_skill(ch, gsn_fencing) / 4;

    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_SET(ch->off_flags, OFF_FAST))
	chance = 0;

    if (number_percent() < chance) {
	one_hit(ch, victim, dt, FALSE);
	if (ch->fighting != victim)
	    return;
    }
    /* oh boy!  Fun stuff! */

    if (ch->wait > 0)
	return;

    /* now for the skills */

    number = number_range(0, 8);

    switch (number) {
    case (0):
	if (IS_SET(ch->off_flags, OFF_BASH))
	    do_bash(ch, "");
	break;

    case (1):
	if (IS_SET(ch->off_flags, OFF_BERSERK) && !IS_AFFECTED(ch, AFF_BERSERK))
	    do_berserk(ch, "");
	break;

    case (2):
	if (IS_SET(ch->off_flags, OFF_DISARM)
	    || (get_weapon_sn(ch, FALSE) != gsn_hand_to_hand
		&& (IS_SET(ch->act, ACT_WARRIOR)
		    || IS_SET(ch->act, ACT_THIEF))))
	    do_disarm(ch, "");
	break;

    case (3):
	if (IS_SET(ch->off_flags, OFF_KICK))
	    do_kick(ch, "");
	break;

    case (4):
	if (IS_SET(ch->off_flags, OFF_KICK_DIRT))
	    do_dirt(ch, "");
	break;

    case (5):
	if (IS_SET(ch->off_flags, OFF_TAIL)) {
	    /* do_tail(ch,"") */ ;
	}
	break;

    case (6):
	if (IS_SET(ch->off_flags, OFF_TRIP))
	    do_trip(ch, "");
	break;

    case (7):
	if (IS_SET(ch->off_flags, OFF_CRUSH)) {
	    /* do_crush(ch,"") */ ;
	}
	break;

    case (8):
	if (IS_SET(ch->off_flags, OFF_BACKSTAB)) {
	    do_backstab(ch, "");
	}
    }
}

/*
 * Hit one guy once.
 */

void one_hit(CHAR_DATA * ch, CHAR_DATA * victim, int dt, bool secondary)
{
	int damroll = 0;
	int hitroll = 0;
    OBJ_DATA *wield;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int dam;
    int dam2 = 0;
    int diceroll;
    int sn, skill;
    int dam_type;
    bool result;

    sn = -1;

    /* just in case */
    if (victim == ch || ch == NULL || victim == NULL)
	return;


    /*
     * Guard against weird room-leavings.
     */
    if (ch->in_room != victim->in_room)
	return;

    /*
     * Figure out the type of damage message.
     */

    if(IS_AFFECTED(victim, AFF_HOLD))
      REMOVE_BIT(victim->affected_by, AFF_HOLD);

    if(IS_AFFECTED(ch, AFF_HOLD))
      REMOVE_BIT(ch->affected_by, AFF_HOLD);

    if (!secondary)
	wield = get_eq_char(ch, WEAR_WIELD);
    else
	wield = get_eq_char(ch, WEAR_SECONDARY);

    if (dt == TYPE_UNDEFINED) {
	dt = TYPE_HIT;
	if (wield != NULL && wield->item_type == ITEM_WEAPON)
	    dt += wield->value[3];
	else
	    dt += ch->dam_type;
    }
    if (dt < TYPE_HIT)
	if (wield != NULL)
	    dam_type = attack_table[wield->value[3]].damage;
	else
	    dam_type = attack_table[ch->dam_type].damage;
    else
	dam_type = attack_table[dt - TYPE_HIT].damage;

    if (dam_type == -1)
	dam_type = DAM_BASH;

    /* get the weapon skill */
    sn = get_weapon_sn(ch, secondary);
    skill = 20 + get_weapon_skill(ch, sn);

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if (IS_NPC(ch)) {
	thac0_00 = 20;
	thac0_32 = -4;		/* as good as a thief */
	if (IS_SET(ch->act, ACT_WARRIOR))
	    thac0_32 = -10;
	else if (IS_SET(ch->act, ACT_THIEF))
	    thac0_32 = -4;
	else if (IS_SET(ch->act, ACT_CLERIC))
	    thac0_32 = 2;
	else if (IS_SET(ch->act, ACT_MAGE))
	    thac0_32 = 6;
    } else {
	thac0_00 = class_table[ch->class].thac0_00;
	thac0_32 = class_table[ch->class].thac0_32;
    }
    thac0 = interpolate(ch->level, thac0_00, thac0_32);

    if (thac0 < 0)
	thac0 = thac0 / 2;

    if (thac0 < -5)
	thac0 = -5 + (thac0 + 5) / 2;



//slow down hr past 160
	hitroll = GET_HITROLL(ch);
	hitroll = hitroll <= 160 ? hitroll : 160 + ((hitroll - 160) / 5);


    if (!IS_NPC(ch)) {
	if (ch->pcdata->strategy <= 1) {
	    thac0 -= (hitroll - (ch->level / 3)) * skill / 100;
	} else if (ch->pcdata->strategy >= 3) {
	    thac0 -= (hitroll + (ch->level / 3)) * skill / 100;
	} else
	    thac0 -= hitroll * skill / 100;
    } else
	thac0 -= hitroll * skill / 100;

    thac0 += 5 * (100 - skill) / 100;

    if (dt == gsn_backstab)
	thac0 -= 10 * (100 - get_skill(ch, gsn_backstab));
	
	if (dt == gsn_jump)
	thac0 -= 10 * (100 - get_skill(ch, gsn_jump));

    if (dt == gsn_circle)
	thac0 -= 10 * (100 - get_skill(ch, gsn_circle));
    if (dt == gsn_dragonsword)
	thac0 -= 10 * (100 - get_skill(ch, gsn_dragonsword));

    if (dt == gsn_charge)
	thac0 -= 10 * (100 - get_skill(ch, gsn_charge));

    switch (dam_type) {
    case (DAM_PIERCE):
	victim_ac = GET_AC(victim, AC_PIERCE) / 10;
	break;
    case (DAM_BASH):
	victim_ac = GET_AC(victim, AC_BASH) / 10;
	break;
    case (DAM_SLASH):
	victim_ac = GET_AC(victim, AC_SLASH) / 10;
	break;
    default:
	victim_ac = GET_AC(victim, AC_EXOTIC) / 10;
	break;
    };

    if (victim_ac < -15)
	victim_ac = (victim_ac + 15) / 5 - 15;

    if (!can_see(ch, victim)) {
	if (get_skill(ch, gsn_blind_fighting) > 0) {
	    if (number_percent() <= get_skill(ch, gsn_blind_fighting))
		check_improve(ch, gsn_blind_fighting, TRUE, 6);
	    else
		victim_ac -= 4;
	} else
	    victim_ac -= 4;
    }
    if (victim->position < POS_FIGHTING)
	victim_ac += 4;

    if (victim->position <= POS_RESTING)
	victim_ac += 6;

    /*
     * The moment of excitement!
     */
    while ((diceroll = number_bits(5)) >= 20);

    // knock off a bit of the condition for the wielded weapon.
    if (wield != NULL) {
	if (number_range(1, 10) == 7) {
	    if (wield->condition > 0)
		wield->condition += -1;
	}
    }
    if (diceroll == 0 || (diceroll != 19 && diceroll < thac0 - victim_ac)) {
	/* Miss. */
	damage(ch, victim, 0, dt, dam_type, TRUE);
	tail_chain();
	return;
    }
    /*
     * Hit.
     * Calc damage.
     */
    if (IS_NPC(ch) && (!ch->pIndexData->new_format || wield == NULL)) 
	if (!ch->pIndexData->new_format) {
	    dam = number_range(ch->level / 2, ch->level * 3 / 2);
	    if (wield != NULL)
		dam += dam / 2;
        } else
	    dam = dice(ch->damage[DICE_NUMBER], ch->damage[DICE_TYPE]);
    else {
	if (sn != -1)
	    check_improve(ch, sn, TRUE, 5);
	if (wield != NULL) {
	    if (wield->pIndexData->new_format)
		dam = dice(wield->value[1], wield->value[2]) * skill / 100;
	    else
		dam = number_range(wield->value[1] * skill / 100, wield->value[2] * skill / 100);

           //Vashna 07-14-01:Reducing damage to 75 hp max without bonuses.
            if(!IS_NPC(ch) && dam > 75){
                dam = 75;
            }

	    if (get_eq_char(ch, WEAR_SHIELD) == NULL)	/* no shield = more */
		dam = dam * 11 / 10;
// This code is damn near illegible, Cleaning it up.
		/* twohanded! */ //Failure, Exotic weapon + Monk somehow worked :\ will attempt again in the future. -Zyloch
/*
		if (IS_WEAPON_STAT(wield, WEAPON_TWO_HANDS) && get_eq_char(ch, WEAR_SECONDARY) == NULL && get_eq_char(ch, WEAR_SHIELD) == NULL) {
			
		   if (!strcmp(class_table[ch->class].name, "monk")) {
	    	     dam *= 1.0;
		   }
		   else if (wield->value[0] == WEAPON_DAGGER) 
		   {
       		    dam *= 1.5;
      	    	   }
		   else if (!strcmp(class_table[ch->class].name, "Barbarian")) {
			damroll = GET_DAMROLL(ch);
			dam += damroll + 40;
			dam *= 1.65;
		   }
		   else {
		     damroll = GET_DAMROLL(ch);
		     dam += damroll;
		     dam *= 1.5;
	  	   }
		}
*/		
	    /* sharpness! */
	    if (IS_WEAPON_STAT(wield, WEAPON_SHARP))  {
	        int percent;
	        if ((percent = number_percent()) <= (skill / 4))
	            dam2 = 2 * dam  * percent / 100;
                if (dam2 > dam)
                dam = dam2;
	    }
        } else {   //bare handed
          if(!str_cmp(class_table[ch->class].name, "monk") && get_skill(ch, gsn_hand_to_hand) > 0)
            {
            dam = number_range(1 + 5 * skill / 30, 2 * ch->level / 2 * skill / 75);
            } else {
             dam = number_range(1 + 4 * skill / 100, 2 * ch->level / 3 * skill / 100);
             }
           }
            //Vashna 07-14-01:Repeating the 75 hp damage check.
            //Tasydra 07-22-01:Monks can do more than 75 bare handed
            if(!IS_NPC(ch) && dam > 75 && str_cmp(class_table[ch->class].name, "monk")){
              dam = 75;
            }
    }

    /*
     * Bonuses.
     */
    if (get_skill(ch, gsn_enhanced_damage) > 0) {
	diceroll = number_percent();
	if (diceroll <= get_skill(ch, gsn_enhanced_damage)) {
	    check_improve(ch, gsn_enhanced_damage, TRUE, 6);
	    dam += 7 * (dam * get_skill(ch, gsn_enhanced_damage) / 600);
	}
    }
    if (get_skill(ch, gsn_brutal_damage) > 0) 
    if ((dt != gsn_charge
    || dt != gsn_ground))
  //no bonus to charge or ground
    {
	diceroll = number_percent();
	if (diceroll <= get_skill(ch, gsn_brutal_damage)) {
	    check_improve(ch, gsn_brutal_damage, TRUE, 6);
	    if(!str_cmp(class_table[ch->class].name, "lancer") ) // Lancers get better brutal, :D Zyloch changed it to barb to 6. 11/12/2005
	    dam += 5 * (dam * get_skill(ch, gsn_brutal_damage) / 300);
	    else
	    dam += 4 * (dam * get_skill(ch, gsn_brutal_damage) / 300);
	}
    }
    if (!IS_AWAKE(victim))
	dam *= 2;
    else if (victim->position < POS_FIGHTING)
	dam *= 3/2;

    if (dt == gsn_ground)
       dam *= 6;

    if (dt == gsn_throw)
       dam *= 6;

    if (dt == gsn_skewer)
       dam *= 3;

    if (dt == gsn_rush) 
   {
     if (wield->value[0] == WEAPON_POLEARM)
      {
       dam *= 4;
      }
     else
     {
	     dam *=2;
     }
   }
   
   if (dt == gsn_braver)
   dam *= number_range(15,35)/10;
   
   if (dt == gsn_cross_slash)
   dam *= number_range(3,7)/4;

    if ((dt == gsn_circle) && wield != NULL) {
	if (wield->value[0] == WEAPON_DAGGER)
       {
        if (IS_NPC(victim))
         {
	    dam *= 4;
         } else {
           if(!strcmp(class_table[ch->class].name,"Bard"))
             dam *= 2.6;
           else dam *= 2.8;
         }
        }
	else
      {
	    dam *= 1.5;
      }
   }
    if ((dt == gsn_dragonsword) && wield != NULL)
	{
	if(wield->value[0] == WEAPON_SWORD)
	    dam *= number_range(3,4.5);
	else
	    dam *= 3;
	}
    if ((dt == gsn_backstab) && wield != NULL) {
	if (wield->value[0] == WEAPON_DAGGER)
          {
      if (IS_AFFECTED(victim, AFF_NOQUIT) && IS_AWAKE(victim))
          {
	    dam *= 4;
          } else if (!IS_AWAKE(victim))
                  {
                 dam *= 7.5;
                  } else {
                 dam *= 6.5;
                         }
        } else {
	    dam *= 2;
        }
    }
    if ((dt == gsn_jump) && wield != NULL) {
	if (wield->value[0] == WEAPON_POLEARM && WEAPON_SPEAR)
          {
      if (IS_AFFECTED(victim, AFF_NOQUIT) && IS_AWAKE(victim))
          {
	    dam *= 7.8;
          } 
          else if (!IS_AWAKE(victim))
                  {
                 dam *= 7.8;
                  } else {
                 dam *= 7.8;
                         }
        } else {
	    dam *= 3;
        }
    }
    if (dt == gsn_charge && wield != NULL) 
    {
	dam *= 9;
    }
    
#if 0 // here's the problem with damroll... Archan.	
    /* secondary weapon means less damage */
    if (secondary)
//	dam *= 2/3;  
#endif 
//slow down damroll
	damroll = GET_DAMROLL(ch);
	damroll = damroll <= 100 ? damroll : 100 + ((damroll - 100) / 5);

	dam += damroll * UMIN(100, skill) / 100;

    if (!IS_NPC(ch)) {
	if (ch->pcdata->strategy <= 1)
	    dam -= (ch->level / 2);
	if (ch->pcdata->strategy >= 3)
	    dam += (ch->level / 5);
    }
    if (dam <= 0)
	dam = 1;

    if (wield != NULL) {
	if (wield->condition <= 0)
	    dam = 2;
    }

//slow down combat period.
	dam = (dam * 3) / 4;

    result = damage(ch, victim, dam, dt, dam_type, TRUE);

    /* but do we have a funky weapon? */
    if (result && wield != NULL) {
	int dam=0;
	int spellchance = 0;
	int thunderchance;
	int firechance;
	int drainchance;
	int holychance;
	spellchance = number_percent();

	if (ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_POISON)) {
	    int level;
	    AFFECT_DATA *poison, af;

	    if ((poison = affect_find(wield->affected, gsn_poison)) == NULL)
		level = wield->level;
	    else
		level = poison->level;

	    if (!saves_spell(level / 2, victim, DAM_POISON,0)) {
		send_to_char("You feel poison coursing through your veins.",
			     victim);
		act("$n is poisoned by the venom on $p.",
		    victim, wield, NULL, TO_ROOM);

		af.where = TO_AFFECTS;
		af.type = gsn_poison;
		af.level = level * 3 / 4;
		af.duration = level / 2;
		af.location = APPLY_STR;
		af.modifier = -1;
		af.bitvector = AFF_POISON;
		affect_join(victim, &af);
	    }
	    /* weaken the poison if it's temporary */
	    if (poison != NULL) {
		poison->level = UMAX(0, poison->level - 2);
		poison->duration = UMAX(0, poison->duration - 1);

		if (poison->level == 0 || poison->duration == 0)
		    act("The poison on $p has worn off.", ch, wield, NULL, TO_CHAR);
	    }
	}
//Unique Weapon #1, Created by Zyloch   (Glowing) (Humming) <Thunderfury, Blessed Blade of the Windseeker>
	if (ch->fighting == victim 
	&& (is_name("Zyloch Thunderfury Blessed Blade of the Windseeker Sword",wield->name)) 
	&& (ch->in_room == victim->in_room)
	&& !is_name("restring",wield->name) )  {
		thunderchance = number_range(1,100);
			if (thunderchance <= 4) {
			spell_chain_lightning(skill_lookup("chain lightning"), ch->level-10, ch, victim, TARGET_CHAR);
		}
	}

//Unique Weapon, Created by Zyloch   (Glowing) (Humming) <Heal Staff>
	if (ch->fighting == victim 
	&& (is_name("Zyloch Heal Staff",wield->name)) 
	&& (ch->in_room == victim->in_room)
	&& !is_name("restring",wield->name) )  {
			spell_heal(skill_lookup("heal"), ch->level/5, ch, victim, TARGET_CHAR);
		}
		
//Unique Weapon #2, (Glowing) (Humming) Sulfuras, Hand of Ragnaros
	if (ch->fighting == victim 
	&& (is_name("Zyloch Sulfuras Hand of Ragnaros Mace",wield->name)) 
	&& (ch->in_room == victim->in_room)
	&& !is_name("restring",wield->name) )  {
	firechance = number_range(1,100);
	if (firechance <= 4) {
		spell_fireball(skill_lookup("fireball"), ch->level+25, ch, victim, TARGET_CHAR);
	}
}

//Unique Weapon #3, (Glowing) (Humming) Decado, Harvester of the Souls
	if (ch->fighting == victim 
	&& (is_name("Zyloch Decado Harvester of the Souls Axe",wield->name)) 
	&& (ch->in_room == victim->in_room)
	&& !is_name("restring",wield->name) )  {
		drainchance = number_range(1,100);
		if (drainchance <= 10) {
			spell_soul_coil(skill_lookup("soul coil"), ch->level, ch, victim, TARGET_CHAR);
			spell_death_coil(skill_lookup("death coil"), ch->level, ch, victim, TARGET_CHAR);
		}
	}

//Unique Weapon #4,
	if (ch->fighting == victim 
	&& (is_name("Zyloch Yesdinl, the Pure Sword",wield->name)) 
	&& (ch->in_room == victim->in_room)
	&& !is_name("restring",wield->name) )  {
		holychance = number_range(1,1000);
		if (holychance == 1) {
			spell_prism(skill_lookup("prism"), ch->level, ch, ch, TARGET_CHAR);
		}
		if (holychance <= 100) {
			spell_purify(skill_lookup("purify"), ch->level*2, ch, ch, TARGET_CHAR);
		}
	}

						
	if (ch->fighting == victim 
	&& (is_name("baselard",wield->name)) 
	&& (ch->in_room == victim->in_room)
	&& !is_name("restring",wield->name) )  {
		switch (spellchance/4) {
			case 0:
			case 1:
			case 2:
			case 3:
			spell_fireball(skill_lookup("fireball"), ch->level*3/5, ch, victim, TARGET_CHAR);
			break;
			case 4:
			case 5:
			case 6:
			spell_chill_touch(skill_lookup("chill touch"), ch->level*3/5, ch, victim, TARGET_CHAR);
			break;
			case 7:
			case 8:
			case 9:
			spell_colour_spray(skill_lookup("colour spray"), ch->level*3/5, ch, victim, TARGET_CHAR);
			break;
			case 10:
			case 11:
			case 12:
			spell_lightning_bolt(skill_lookup("lightning bolt"), ch->level*3/5, ch, victim, TARGET_CHAR);
			break;
			default:
			break;		
		} if (ch->in_room == victim->in_room){
		dam = number_range(10, ch->level*2/3);
	    act("$p steals the life from $n.", victim, wield, NULL, TO_ROOM);
	    act("You feel $p stealing your life away.",
		victim, wield, NULL, TO_CHAR);
	    damage_old(ch, victim, dam, 0, DAM_NEGATIVE, FALSE);
	    ch->hit += dam;}
	}
	
	spellchance = number_range(0,100);
	if (ch->fighting == victim && (is_name("ethereal",wield->name)) && !is_name("restring",wield->name) ) {
		if (spellchance <= 2 && ch->in_room == victim->in_room)
		spell_energy_drain(skill_lookup("energy drain"), ch->level, ch, victim, TARGET_CHAR);
		else if (spellchance <= 20 && ch->in_room == victim->in_room)
		spell_soul_coil(skill_lookup("soul coil"), ch->level, ch, victim, TARGET_CHAR);
		else if (spellchance <= 36 && ch->in_room == victim->in_room)
		spell_death_coil(skill_lookup("death coil"), ch->level, ch, victim, TARGET_CHAR);
		else if (spellchance > 36) {
		dam = number_range(5, ch->level*1/3);
	    act("$p steals the magic from $n!", victim, wield, NULL, TO_ROOM);
	    act("You feel $p stealing your magic away!", victim, wield, NULL, TO_CHAR);
	    victim->mana -= dam;
	    ch->mana += dam;
		}
	}
	
	if (ch->fighting == victim && (IS_WEAPON_STAT(wield, WEAPON_VAMPIRIC) || IS_AFFECTED2(ch, AFF_UNDEAD))) {
	    dam = number_range(1, ch->level / 12 + 1);
	    act("$p draws life from $n.", victim, wield, NULL, TO_ROOM);
	    act("You feel $p drawing your life away.",
		victim, wield, NULL, TO_CHAR);
	    damage_old(ch, victim, dam, 0, DAM_NEGATIVE, FALSE);
	    ch->alignment = UMAX(-1000, ch->alignment - 1);
	    ch->hit += dam;
	}
	if (ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_FLAMING)) {
	    dam = number_range(1, wield->level / 4 + 1);
	    act("$n is burned by $p.", victim, wield, NULL, TO_ROOM);
	    act("$p sears your flesh.", victim, wield, NULL, TO_CHAR);
	    fire_effect((void *) victim, wield->level / 2, dam, TARGET_CHAR);
	    damage(ch, victim, dam, 0, DAM_FIRE, FALSE);
	}
	if (ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_FROST)) {
	    dam = number_range(1, wield->level / 6 + 2);
	    act("$p freezes $n.", victim, wield, NULL, TO_ROOM);
	    act("The cold touch of $p surrounds you with ice.",
		victim, wield, NULL, TO_CHAR);
	    cold_effect(victim, wield->level / 2, dam, TARGET_CHAR);
	    damage(ch, victim, dam, 0, DAM_COLD, FALSE);
	}
	if (ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_SHOCKING)) {
	    dam = number_range(1, wield->level / 5 + 2);
	    act("$n is struck by lightning from $p.", victim, wield, NULL, TO_ROOM);
	    act("You are shocked by $p.", victim, wield, NULL, TO_CHAR);
	    shock_effect(victim, wield->level / 2, dam, TARGET_CHAR);
	    damage(ch, victim, dam, 0, DAM_LIGHTNING, FALSE);
	}
	if (ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_VORPAL)) {
		dam = number_range(2, ch->level/10);
	    act("$p leechs $n's mana!", victim, wield, NULL, TO_ROOM);
	    act("You feel $p leeching your mana!", victim, wield, NULL, TO_CHAR);
	    victim->mana -= dam;
	    ch->mana += dam;
	}
    }
    tail_chain();
    return;
}

/*
 * Inflict damage from a hit.
 */
bool damage(CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, int dam_type, bool show)
{

	int isLgg = 0;
    char buf[MAX_STRING_LENGTH];
    char pers_buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    bool immune;
    bool mdebug = 0;
    CHAR_DATA *mount;
    int geochance = 0;
    int decaychance = 0;
    //int rdam = 0;

    //Tasydra Aug 21, 2001: Fixed Dupe bug here.

   if(victim->level == 1 && ch->level == 1
   && !IS_NPC(ch) && !IS_NPC(victim) )
   {
    send_to_char("The gods protect him!\n\r", ch);
    send_to_char("The gods protect you from harm!\n\r", victim);
    return FALSE;
   }

if(mdebug) {sprintf(buf, "orig %i - ", dam);   send_to_char(buf, ch); }


    if (victim != ch) 
	{
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
		if (is_safe(ch, victim))
		    return FALSE;
		check_killer(ch, victim);

		if (victim->position > POS_STUNNED) 
		{
		    if (victim->fighting == NULL) 
			{
				set_fighting(victim, ch);
				if (IS_NPC(victim) && HAS_TRIGGER(victim, TRIG_KILL))
				    mp_percent_trigger(victim, ch, NULL, NULL, TRIG_KILL);
		    }
		}
		if (victim->position > POS_STUNNED) 
		{
		    if (ch->fighting == NULL)
			set_fighting(ch, victim);
		}
		if (is_same_group(victim, ch) 
		&& (GET_SIDE(victim) != GET_SIDE(ch)) ) // Izlude, preparing for new group system
		    stop_follower(victim);
    }

    /*
     * Inviso attacks ... not.
     * No Held attacks either, :D
     */

    if (IS_AFFECTED(ch, AFF_INVISIBLE)) 
	{
		affect_strip(ch, gsn_invis);
		affect_strip(ch, gsn_mass_invis);
		REMOVE_BIT(ch->affected_by, AFF_INVISIBLE);
		act("$n fades into existence.", ch, NULL, NULL, TO_ROOM);
    } 

    {
		OBJ_DATA *pWornObj;
		int iWear;
		bool changed;
		changed = FALSE;

		for (iWear = 0; iWear < MAX_WEAR; iWear++) 
		{
		    if ((pWornObj = get_eq_char(ch, iWear)) == NULL)
				continue;

		    if (pWornObj->item_type == ITEM_ARMOR) 
			{
				if (number_range(1, 20) == 1) 
				{
				    if (pWornObj->condition > 0)
						pWornObj->condition += -1;
				}
				if (pWornObj->condition <= 30)
					changed = TRUE;
		    }
		}

		if (changed)
		    calculate_ac(ch);
    }

    if (victim->hate) 
		victim->hate->who = ch;
	else
		start_hating(victim, ch);

    if (IS_SET(victim->act, ACT_HUNT))
		victim->hunting = ch;

    /*
     * Damage modifiers.
     */

// Izlude - Warlock spell
     if (dam > 1 && is_affected(ch, skill_lookup("enhance aura")))
     dam = dam * 108 / 100; 
     
    if (dam > 1 && !IS_NPC(victim) && victim->pcdata->condition[COND_DRUNK] > 10)
		dam = (dam * 9) / 10;

    if (dam > 1 && IS_AFFECTED(victim, AFF_SANCTUARY))
		dam /= 1.5;

    if (dam > 1 && IS_AFFECTED(victim, AFF_REFLECT))
		dam = (dam * 3) / 4;  //ints->   3/4 = 0
	
	if (dam > 1 && is_affected(victim, gsn_stone_protect))
		dam = dam *9/10;
	
	if (dam > 1 && is_affected(ch, gsn_attack_song))
	    dam = dam *11/10;

    if (dam > 1 && ((IS_AFFECTED(victim, AFF_PROTECT_EVIL) && IS_EVIL(ch))
		    || (IS_AFFECTED(victim, AFF_PROTECT_GOOD) && IS_GOOD(ch))))
	dam -= (dam / 4);
	
	if (is_affected(victim,gsn_jump))
	dam = 0;

    immune = FALSE;

    /*
     * Check for parry, and dodge.
     */
    if (dt >= TYPE_HIT && ch != victim) 
	{
		if (get_eq_char(ch, WEAR_SECONDARY))
		    if (check_parry(ch, victim))
				return FALSE;

		if (check_parry(ch, victim))
		    return FALSE;
		if (check_dodge(ch, victim))
		    return FALSE;
		if (check_shield_block(ch, victim))
		    return FALSE;
		if (is_affected(victim,gsn_lava_shield))
		{
			geochance = number_range(0,20);
			send_to_char("A gout of lava splatters up!\n\r",victim);
			send_to_char("A gout of lava splatters up and hits you!\n\r",ch);
			if ( 8 <= geochance <= 18)
			spell_fireball(skill_lookup("fireball"), victim->level, victim, ch, TARGET_CHAR);
			else if (geochance >= 19)
			spell_fire_breath(skill_lookup("fire breath"), victim->level, victim, ch, TARGET_CHAR);
		}
		
		if (is_affected(victim,gsn_frost_shield))
		{
			geochance = number_range(0,20);
			send_to_char("Frost bunches up by you and hits your attacker!\n\r",victim);
			send_to_char("You feel chilled!\n\r",ch);
			
			if (geochance >= 19)
			spell_frost_breath(skill_lookup("frost breath"), victim->level, victim, ch, TARGET_CHAR);
			
			if (geochance > 0)
			cold_effect(victim, 5, 1, TARGET_CHAR);
			if (geochance > 10)
			cold_effect(victim, 5, 1, TARGET_CHAR);
			if (geochance > 15)
			cold_effect(victim, 5, 1, TARGET_CHAR);
		}
		
		if (is_affected(victim,gsn_vine_counter))
		{
			geochance = number_range(0,20);
			if (geochance >= 13){
			send_to_char("The vines around you attack your enemy!\n\r",victim);
			send_to_char("You are attacked by a bunch of vines!\n\r",ch);
			damage(victim, ch, number_range(ch->level/2, ch->level*2), gsn_earthlore, DAM_BASH, TRUE);
			}
		}
		
		
		
    }

    switch (check_immune(victim, dam_type)) 
	{
	    case (IS_IMMUNE):
			immune = TRUE;
			dam = 0;
			break;
		case (IS_RESISTANT):
			dam -= dam / 3;
			break;
	    case (IS_VULNERABLE):
			dam += dam / 2;
			break;
    }

    if (show)
		dam_message(ch, victim, dam, dt, immune);

    if (dam == 0)
		return FALSE;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */

    if (dam > 1 && IS_AFFECTED(victim, AFF_REFLECT)) 
    {
		ch->hit -= dam / 2;
		if (ch->hit < 0)
		    ch->hit = 1;
		 sprintf(buf, "{y    Your attack is reflected back at you!{x [{R%d{x]\n\r", (dam * 3) / 4);
		 send_to_char(buf, ch);
		//act(buf, ch, NULL, NULL, TO_CHAR);
		//send_to_char("{y    Your attack is reflected back at you!{x\n\r", ch, );
    }

        if (dam > 5 && is_affected(victim, skill_lookup("decay aura")))
    {
		decaychance = number_range(0,100);
		if ( decaychance <= 4)
		spell_blindness(skill_lookup("blindness"), victim->level-15, victim, ch, TARGET_CHAR);
		else if ( decaychance <= 24)
		spell_curse(skill_lookup("curse"), victim->level-15, victim, ch, TARGET_CHAR);
		else if ( decaychance <= 50)
		spell_poison(skill_lookup("poison"), victim->level-15, victim, ch, TARGET_CHAR);
		else if ( decaychance <= 65)
		spell_plague(skill_lookup("plague"), victim->level-15, victim, ch, TARGET_CHAR);
		else if ( decaychance <= 85)
		spell_weaken(skill_lookup("weaken"), victim->level-15, victim, ch, TARGET_CHAR);
		else if ( decaychance <= 100)
		spell_soul_coil(skill_lookup("soul coil"), victim->level, victim, ch, TARGET_CHAR);
    }


    victim->hit -= dam;

    if (!IS_NPC(victim) && victim->level >= LEVEL_IMMORTAL && victim->hit < 1)
		victim->hit = 1;

    if(victim->hit < 1 && IS_NPC(victim) && ch == victim && victim->position > POS_STUNNED)
		victim->hit = 1;
    else if(victim->hit < -6 && ch == victim && victim->position > POS_STUNNED)
		victim->hit = -6;

    update_pos(victim);

    switch (victim->position) 
	{
	    case POS_MORTAL:
			act("$n is mortally wounded, and will die soon, if not aided.",
		    victim, NULL, NULL, TO_ROOM);
			send_to_char("You are mortally wounded, and will die soon, if not aided.\n\r", victim);
			break;

		case POS_INCAP:
			act("$n is incapacitated and will slowly die, if not aided.",
		    victim, NULL, NULL, TO_ROOM);
			send_to_char("You are incapacitated and will slowly die, if not aided.\n\r", victim);
			break;

		case POS_STUNNED:
			act("$n is stunned, but will probably recover.",
		    victim, NULL, NULL, TO_ROOM);
			send_to_char("You are stunned, but will probably recover.\n\r", victim);
			break;

		case POS_DEAD:
			act("$n is DEAD!!", victim, 0, 0, TO_ROOM);
			send_to_char("{YYou have been KILLED!!{x\n\r\n\r", victim);

		if (IS_NPC(ch))
		    if (is_hating(ch, victim))
				stop_hating(ch);
               if (armageddon){
               if (!IS_NPC(victim)){
               REMOVE_BIT(victim->affected_by, AFF_NOQUIT);
               PERS(victim, ch, pers_buf);
               sprintf(buf, "%s is OUT!\n\r", victim->name);
               if (victim->side != ch->side)
               news(buf, victim->side);
               send_to_char("Quiting since you died in Armageddon!\n\r", victim);
               sprintf(buf, "%s is OUT!\n\r", pers_buf);
               news(buf, ch->side);
               stop_fighting(victim, TRUE);
               stop_fighting(ch, TRUE);
               victim->fighting = NULL;
               ch->fighting = NULL;
             /* If this doesn't fucking stop fighting, nothing will */
               victim->hit = 1;
               update_pos(victim);
               update_pos(ch);
               do_quit(victim, "");
               return TRUE;
                                   }
                               }

//boobytrapped shadow dragon
//if(IS_NPC(victim))
//    if(victim->pIndexData->vnum == 14011)
//    {
//	sprintf(log_buf, "BOOBY: %s killed shadowdragon", (IS_NPC(ch) ? ch->short_descr : ch->name));
//	log_string(log_buf);

  //  }

//if its a king then it may be a raid
        if(IS_NPC(victim))
            if(!IS_NPC(ch))
                if((victim->pIndexData->vnum == MOB_VNUM_TIER_KING) ||
		           (victim->pIndexData->vnum == MOB_VNUM_KAEL_KING) ||
		           (victim->pIndexData->vnum == MOB_VNUM_UM_KING)   ||
			       (victim->pIndexData->vnum == MOB_VNUM_CAIRN_KING))
		           if(victim->side != ch->side) 
		                raid(ch, victim);//end king

	//if its a wall of force, kill the twin
		if (victim->spec_fun != NULL && !strcmp(spec_name(victim->spec_fun), "spec_wall_force"))
		{
		    int exit;
		    char *oppExit = "";
		    CHAR_DATA *twin ;

		    if (is_name("north", victim->long_descr)) 
			{
				exit = 0;
				oppExit = "south";
		    } 
			else if (is_name("east", victim->long_descr)) 
			{
				exit = 1;
				oppExit = "west";
		    } 
			else if (is_name("south", victim->long_descr)) 
			{
				exit = 2;
				oppExit = "north";
		    } 
			else if (is_name("west", victim->long_descr)) 
			{
				exit = 3;
				oppExit = "east";
		    } 
			else if (is_name("up", victim->long_descr)) 
			{
				exit = 4;
				oppExit = "down";
		    } 
			else if (is_name("down", victim->long_descr)) 
			{
				exit = 5;
				oppExit = "up";
		    } 
			else
				exit = -1;

		    if (exit != -1)
	            if(victim->in_room->exit[exit] != NULL)
					for (twin = victim->in_room->exit[exit]->u1.to_room->people; twin != NULL; twin = twin->next_in_room) 
					{
				        if(IS_NPC(twin))
					        if( twin->pIndexData->vnum == MOB_WALL_OF_FORCE && is_name( oppExit, twin->long_descr))
							    raw_kill(twin, victim, 0);
					}
		}	//end is wall

		if (MOUNTED(victim)) 
		{
		    mount = MOUNTED(victim);
		    victim->riding = FALSE;
		    mount->riding = FALSE;
		    act("$n is no longer being ridden!", mount, 0, 0, TO_ROOM);
		}
		if (RIDDEN(victim)) 
		{
		    mount = RIDDEN(victim);
		    victim->riding = FALSE;
		    mount->riding = FALSE;
		    send_to_char("Your mount is DEAD!!\n\r", mount);
		}

		break;

    default:
	if (dam > victim->max_hit / 4)
	    send_to_char("{RThat really did HURT!{X\n\r", victim);
	if (!IS_NPC(victim) && victim->hit < victim->max_hit / 4) {
	    OBJ_DATA *puddle;
	    send_to_char("{RYou sure are BLEEDING!!!{X\n\r", victim);
	    act("{RBlood flies everywhere!!!{X", ch, NULL, victim, TO_ROOM);
	    puddle = create_object(get_obj_index(OBJ_VNUM_BLOOD_PUDDLE), 0);
	    puddle->timer = number_range(2,4);
	    obj_to_room(puddle, victim->in_room);
	}
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if (!IS_AWAKE(victim) && dam_type != DAM_POISON && IS_AFFECTED(victim, AFF_SLEEP))
	stop_fighting(victim, FALSE);

    /*
     * Payoff for killing things.
     */
    if (victim->position == POS_DEAD) 
	{
		
		
		//Izlude quick mount fix
	CHAR_DATA *mount;

    if (MOUNTED(ch)) {
	mount = MOUNTED(ch);
	ch->mount = NULL;
	ch->riding = FALSE;
	mount->riding = FALSE;
	}
		

	isLgg = calc_isLgg(ch, victim);		

		if (IS_SET(ch->act, PLR_TMPKILLER) && !IS_NPC(victim) && (GET_SIDE(ch) == GET_SIDE(victim))) 
		{
		    send_to_char("{Y*** The gods brand you a TRAITOR!! ***{X\n\r", ch);
		    REMOVE_BIT(ch->act, PLR_TMPKILLER);
		    SET_BIT(ch->act, PLR_KILLER);
		    sprintf(buf, "$N is attempting to murder %s", victim->name);
		    wiznet(buf, ch, NULL, WIZ_FLAGS, 0, 0);

		    sprintf(buf, "%s has just murdered %s! TRAITOR!\n\r",
			    ch->name, victim->name);
		    news(buf, ch->side);

		    if (ch->level > 1)
				save_char_obj(ch);
		}
		if(!isLgg)
		{
			trophy_gain(ch, victim, isLgg);
			group_gain(ch, victim, isLgg);
		}
	if (!IS_NPC(victim)) 
	{
	    sprintf(log_buf, "%s killed by %s at %d", victim->name, (IS_NPC(ch) ? ch->short_descr : ch->name), ch->in_room->vnum);
	    log_string(log_buf);

	    /*
	     * Dying penalty:
	     * 2/3 way back to previous level.
	     */

        // readded lgg check for dieing
		if (victim->exp > exp_per_level(victim, victim->pcdata->points)* victim->level && (!isLgg)) 
		{
			if (IS_NPC(ch) && (victim->level < 50))			
			    gain_exp(victim, (2 * (exp_per_level(victim, victim->pcdata->points)
					   * victim->level - victim->exp) / 3) + 50);
			else 
				gain_exp(victim, -250);
		}
	  }
	
	sprintf(log_buf, "%s got toasted by %s at %s [room %d]",
		(IS_NPC(victim) ? victim->short_descr : victim->name),
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		ch->in_room->name, ch->in_room->vnum);

	if (IS_NPC(victim))
	    wiznet(log_buf, NULL, NULL, WIZ_MOBDEATHS, 0, 0);
	else
	    wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);


	if(isLgg)
	{
		sprintf(log_buf, "Gay kill on %s", victim->name);
	    log_string(log_buf);
	}


	if (!IS_NPC(victim)) 
	{
	    if (!IS_NPC(ch)) 
		{
			if(!isLgg)
				if (victim->level > 10 && ch != victim && ch->side != victim->side && victim->side != SIDE_NONE) 
				{
					
				    send_to_char("FINAL BLOW bonus awarded to you!\n\r", ch);
				    ch->final_blows++;
				    ch->warpoints++;
				}

		/* add a warpoint for the kill */
		/* or get one for being in the same group */
		if (victim->level >= (ch->level - 10) && ch->side != victim->side) 
		{
		    if(!isLgg)
				calc_warpoints(ch, victim);
		}
		PERS(victim, ch, pers_buf);
		sprintf(buf, "Victory, %s has just killed %s at %s.\n\r", ch->name, pers_buf, ch->in_room->name);
		if (victim->side != ch->side)
		    news(buf, ch->side);
		{
		    PERS(ch, victim, pers_buf);
		    sprintf(buf, "Loss, %s was butchered by %s at %s.\n\r", victim->name, pers_buf, ch->in_room->name);

		}
		if (victim->side != ch->side)
		    news(buf, victim->side);
	    } else {
		sprintf(buf, "R.I.P. %s was butchered by %s.\n\r",
			victim->name, ch->short_descr);
		news(buf, victim->side);
	    }
	}
	/*
	 * Death trigger
	 */
	if (IS_NPC(victim) && HAS_TRIGGER(victim, TRIG_DEATH)) 
	{
	    victim->position = POS_STANDING;
	    mp_percent_trigger(victim, ch, NULL, NULL, TRIG_DEATH);
	}
	raw_kill(victim, ch, isLgg);
	/* dump the flags */
	if (ch != victim && !IS_NPC(ch) && !is_same_clan(ch, victim)) 
	{
	    if (IS_SET(victim->act, PLR_TMPKILLER))
			REMOVE_BIT(victim->act, PLR_TMPKILLER);

	    if (IS_SET(victim->act, PLR_KILLER))
			REMOVE_BIT(victim->act, PLR_KILLER);
	    else if (IS_SET(victim->act, PLR_THIEF))
			REMOVE_BIT(victim->act, PLR_THIEF);
	}
	/* RT new auto commands */

	if (!IS_NPC(ch)
	    && (corpse = get_obj_list(ch, "corpse", ch->in_room->contents)) != NULL
	    && corpse->item_type == ITEM_CORPSE_NPC && can_see_obj(ch, corpse)) 
	{
	    OBJ_DATA *coins;

	    corpse = get_obj_list(ch, "corpse", ch->in_room->contents);

	    if (IS_SET(ch->act, PLR_AUTOLOOT) &&
		corpse && corpse->contains)	/* exists and not empty */
		do_get(ch, "all corpse");

	    if (IS_SET(ch->act, PLR_AUTOGOLD) &&
		corpse && corpse->contains &&	/* exists and not empty */
		!IS_SET(ch->act, PLR_AUTOLOOT))
		if ((coins = get_obj_list(ch, "gcash", corpse->contains))
		    != NULL)
		    do_get(ch, "all.gcash corpse");

	    if (IS_SET(ch->act, PLR_AUTOSAC)){
		if (IS_SET(ch->act, PLR_AUTOLOOT) && corpse && corpse->contains)
		    return TRUE;	/* leave if corpse has treasure */
		else
		    do_sacrifice(ch, "corpse");
            }
	}
	return TRUE;
}
    if (victim == ch)
	return TRUE;

    /*
     * Take care of link dead people.
     */
    if (!IS_NPC(victim) && victim->desc == NULL) 
	{
	if (number_range(0, victim->wait) == 0) 
	{
	    do_recall(victim, "");
	    return TRUE;
	}
    }
    /*
     * Wimp out?
     */
    if (IS_NPC(victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2) {
	if ((IS_SET(victim->act, ACT_WIMPY) && number_bits(2) == 0
	     && victim->hit < victim->max_hit / 5)
	    || (IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
		&& victim->master->in_room != victim->in_room))
	    do_flee(victim, "");
    }
    if (!IS_NPC(victim) && victim->hit > 0 && victim->wait < PULSE_VIOLENCE / 2) {
	if (victim->pcdata->strategy <= 0)
	    do_flee(victim, "");
    }
    if (!IS_NPC(victim)
	&& victim->hit > 0
	&& victim->hit <= victim->wimpy
	&& victim->wait < PULSE_VIOLENCE / 2)
	do_flee(victim, "");
    tail_chain();
    return TRUE;
}

bool is_safe(CHAR_DATA * ch, CHAR_DATA * victim)
{
    if (IS_NPC(victim) && IS_SET(victim->act, ACT_NOFIGHT))
	return TRUE;

    if (IS_NPC(ch) && IS_SET(ch->act, ACT_NOFIGHT))
	return TRUE;

    if (victim->in_room == NULL || ch->in_room == NULL)
	return TRUE;

    if (victim->fighting == ch || victim == ch)
	return FALSE;

    if (IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL)
	return FALSE;


    /* killing mobiles */
    if (IS_NPC(victim)) {

	/* safe room? */
	if (IS_SET(victim->in_room->room_flags, ROOM_SAFE)) {
	    send_to_char("Not in this room.\n\r", ch);
	    return TRUE;
	}
	if (victim->pIndexData->pShop != NULL) {
	    send_to_char("The shopkeeper wouldn't like that.\n\r", ch);
	    return TRUE;
	}
	/* no killing healers, trainers, etc */
	if (IS_SET(victim->act, ACT_NOFIGHT)
	    || IS_SET(victim->act, ACT_IS_HEALER)
	    || IS_SET(victim->act, ACT_IS_CHANGER)) {
	    send_to_char("I don't think AoW would approve.\n\r", ch);
	    return TRUE;
	}
    }
    /* killing players */
    else {
	/* NPC doing the killing */
	if (IS_NPC(ch)) {
	    /* safe room check */
	    if (IS_SET(victim->in_room->room_flags, ROOM_SAFE)) {
		send_to_char("Not in this room.\n\r", ch);
		return TRUE;
	    }
	    /* charmed mobs and pets cannot attack players while owned */
/*	    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL
		&& ch->master->fighting != victim) {
		send_to_char("Players are your friends!\n\r", ch);
		return TRUE;
	    }
*/	}
	/* player doing the killing */
	else {
	    if (IS_SET(victim->act, PLR_KILLER) || IS_SET(victim->act, PLR_THIEF))
		return FALSE;
	    if ((victim->level <= 6) &&
		(GET_SIDE(ch) == GET_SIDE(victim))) {
		send_to_char("They are still learning leave them alone.\n\r", ch);
		return TRUE;
	    }
	}
    }
    return FALSE;
}

bool is_safe_spell(CHAR_DATA * ch, CHAR_DATA * victim, bool area)
{
    if (victim->in_room == NULL || ch->in_room == NULL)
	return TRUE;

    if (victim == ch && area)
	return TRUE;

    if (victim->fighting == ch || victim == ch)
	return FALSE;

    if(victim->side == ch->side && area)
	return TRUE;


    if (IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL && !area)
	return FALSE;

    /* killing mobiles */
    if (IS_NPC(victim)) {
	/* safe room? */
	if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
	    return TRUE;

	/* shouldn't be able to area guards who are fighting enemies */
	if (victim->spec_fun != NULL && victim->fighting != NULL)
	    if (strncmp("spec_guard", spec_name(victim->spec_fun), strlen("spec_guard")) == 0)
		if (ch->side != victim->fighting->side)
		    return TRUE;

	if (victim->pIndexData->pShop != NULL)
	    return TRUE;

	/* no killing healers, trainers, etc */
	if (IS_SET(victim->act, ACT_NOFIGHT)
	    || IS_SET(victim->act, ACT_IS_HEALER)
	    || IS_SET(victim->act, ACT_IS_CHANGER))
	    return TRUE;

	if (!IS_NPC(ch)) {	/* pc vs mob */
	    /* no charmed creatures unless different sides */
	    if (IS_AFFECTED(victim, AFF_CHARM) && (area && ch->side == victim->side))
		return TRUE;
	} else {		/* mob vs mob */
	    /* mobs with the exception of charmies should be immune to areas */
	    if (!IS_AFFECTED(victim, AFF_CHARM) && area)
		return TRUE;
	}
    }
    /* killing players */
    else {
	if (area && IS_IMMORTAL(victim) && victim->level > LEVEL_IMMORTAL)
	    return TRUE;

	/* NPC doing the killing */
	if (IS_NPC(ch)) {
	    /* safe room? */
	    if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
		return TRUE;
	}
	/* player doing the killing */
	else {
	    if (IS_SET(victim->act, PLR_KILLER) || IS_SET(victim->act, PLR_THIEF))
		return FALSE;
	    if (ch->side == victim->side)
		return TRUE;
	}
    }
    return FALSE;
}

/*
 * See if an attack justifies a KILLER flag.
 */

void check_killer(CHAR_DATA * ch, CHAR_DATA * victim)
{
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA af;
    AFFECT_DATA *paf;

    /* ok full on combat has happened between two players */
    if (!IS_NPC(ch) && !IS_NPC(victim)) {
	paf = affect_find(ch->affected, skill_lookup("noquit"));
	if (paf != NULL)
	    affect_remove(ch, paf);

	paf = affect_find(victim->affected, skill_lookup("noquit"));
	if (paf != NULL)
	    affect_remove(victim, paf);

	af.where = TO_AFFECTS;
	af.type = skill_lookup("noquit");
	af.level = 1;
	af.duration = 3;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_NOQUIT;
	affect_to_char(ch, &af);
	affect_to_char(victim, &af);
    }
    /*
     * Follow charm thread to responsible character.
     * Attacking someone's charmed char is hostile!
     */
    while (IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL)
	victim = victim->master;

    /*
     * NPC's are fair game.
     * So are killers and thieves.
     */
    if (IS_NPC(victim) || IS_SET(victim->act, PLR_KILLER) || IS_SET(victim->act, PLR_THIEF))
	return;

    /*
     * Charm-o-rama.
     */
    if (IS_SET(ch->affected_by, AFF_CHARM)) {
	if (ch->master == NULL) {
	    char buf[MAX_STRING_LENGTH];

	    sprintf(buf, "Check_killer: %s bad AFF_CHARM",
		    IS_NPC(ch) ? ch->short_descr : ch->name);
	    bug(buf, 0);
	    affect_strip(ch, gsn_charm_person);
	    REMOVE_BIT(ch->affected_by, AFF_CHARM);
	    return;
	}
	if (ch->master != NULL)
	    if (ch->master->side != SIDE_UM)
		if (victim->side == ch->master->side && !IS_NPC(victim)) {
		    sprintf(buf, "{Y*** WARNING:If you kill %s, you will be a TRAITOR!! ***{x\n\r", victim->name);
		    send_to_char(buf, ch->master);
		    SET_BIT(ch->master->act, PLR_TMPKILLER);
		}
	return;
    }
    /*
     * NPC's are cool of course (as long as not charmed).
     * Hitting yourself is cool too (bleeding).
     * So is being immortal (Alander's idea).
     * And current killers stay as they are.
     */
    if (IS_NPC(ch) || ch == victim || ch->level >= LEVEL_IMMORTAL || IS_SET(ch->act, PLR_KILLER)
	|| ch->fighting == victim || (GET_SIDE(ch) != GET_SIDE(victim)))
	return;

    if (ch->side != SIDE_UM) {
	sprintf(buf, "{Y*** WARNING:If you kill %s, you will be a TRAITOR!! ***{x\n\r", victim->name);
	send_to_char(buf, ch);
	SET_BIT(ch->act, PLR_TMPKILLER);
    }
    return;
}

bool is_hating(CHAR_DATA * ch, CHAR_DATA * victim)
{
    if (!ch->hate || ch->hate->who != victim)
	return FALSE;
    return TRUE;
}

void stop_hating(CHAR_DATA * ch)
{
    if (ch->hate) {
	free_mem(ch->hate, sizeof(HATE_DATA));
	ch->hate = NULL;
    }
    return;
}

void start_hating(CHAR_DATA * ch, CHAR_DATA * victim)
{

    if(!IS_NPC(ch))
	return;

    if (ch->hate)
	stop_hating(ch);

    ch->hate = alloc_mem(sizeof(HATE_DATA));
    ch->hate->who = victim;
    return;
}

int get_parry(CHAR_DATA * ch)
{
    int chance;

    chance = get_skill(ch, gsn_parry) / 3;

    if (get_eq_char(ch, WEAR_WIELD) == NULL) 
		chance /= 4;
    
    chance += (get_skill(ch, gsn_fencing) / 5);

    if (IS_SET(ch->act, PLR_SUNWEAK))
		chance += 0;

    if (!IS_NPC(ch)) 
	{
		if (!strcmp(class_table[ch->class].name, "warrior") ||
				!strcmp(class_table[ch->class].name, "barbarian") ||
				!strcmp(class_table[ch->class].name, "knight") ||
				!strcmp(class_table[ch->class].name, "ranger") ||
				!strcmp(class_table[ch->class].name, "geomancer") ||
				!strcmp(class_table[ch->class].name, "warlock") ||
				!strcmp(class_table[ch->class].name, "lancer"))
				
			chance += ch->level / 4;
	
		if (!strcmp(class_table[ch->class].name, "Assassin") || // Izlude, Assassin parry nerf 9/2/04
//Warlocks Old Spot, Changed December 19, 2005			    
			    !strcmp(class_table[ch->class].name, "Bard"))
		    chance += ch->level / 5;
		    
		if (!strcmp(class_table[ch->class].name, "Warrior")) //Was Zealot, Zyloch Changed to Warrior 11/13/2005
		chance += 5;

		if (ch->pcdata->strategy <= 1) 
		{

			if (!strcmp(class_table[ch->class].name, "warrior") ||
					!strcmp(class_table[ch->class].name, "barbarian") ||
					!strcmp(class_table[ch->class].name, "knight") ||
					!strcmp(class_table[ch->class].name, "ranger") ||
					!strcmp(class_table[ch->class].name, "warlock") ||
					!strcmp(class_table[ch->class].name, "lancer"))
				chance += 20;
		    else if (!strcmp(class_table[ch->class].name, "assassin") ||
//Warlocks Old Spot, Changed December 19, 2005			    
					 !strcmp(class_table[ch->class].name, "bard"))
				chance += 15;
			else
				chance += 5;
		}
	
		if (ch->pcdata->strategy >= 3) 
	    chance += -30;
	
	}

    if (chance >= 85)
	chance = 85;
    if (chance < 0)
	chance = 0;

    return chance;
}

/*
 * Check for parry.
 */
bool check_parry(CHAR_DATA * ch, CHAR_DATA * victim)
{
    int chance;
    CHAR_DATA *gch;
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char pers_buf[MAX_STRING_LENGTH];

    if (!IS_AWAKE(victim))
	return FALSE;

    chance = get_skill(victim, gsn_parry) / 3;

    if (get_eq_char(victim, WEAR_WIELD) == NULL) 
	{
		if (IS_NPC(victim))
		    chance /= 4;
		else if (!IS_NPC(victim) && get_skill(victim,gsn_ki_power) > 0)
			chance = chance * 3/5;
		else
		    return FALSE;
    }
    if (!can_see(ch, victim)) 
	{
		if (get_skill(ch, gsn_blind_fighting) > 0) 
		{
		    if (number_percent() <= get_skill(ch, gsn_blind_fighting))
				check_improve(ch, gsn_blind_fighting, TRUE, 6);
			else
				chance /= 3;
		} 
		else
			chance /= 3;
    }
    chance += (get_skill(victim, gsn_fencing) / 5);

    if (IS_SET(victim->act, PLR_SUNWEAK))
	chance += 0;

    if (!IS_NPC(victim)) 
	{

		if (!strcmp(class_table[victim->class].name, "warrior") ||
			    !strcmp(class_table[victim->class].name, "barbarian") ||
			    !strcmp(class_table[victim->class].name, "knight") ||
			    !strcmp(class_table[ch->class].name, "warlock") ||
			    !strcmp(class_table[victim->class].name, "ranger"))
		    chance += victim->level / 4;

		if (!strcmp(class_table[victim->class].name, "assassin") ||
//Warlocks Old Spot, Changed December 19, 2005			
			    !strcmp(class_table[victim->class].name, "bard"))
		    chance += ch->level / 5;


		if (victim->pcdata->strategy <= 1) 
		{
			if (!strcmp(class_table[victim->class].name, "warrior") ||
					!strcmp(class_table[victim->class].name, "barbarian") ||
					!strcmp(class_table[victim->class].name, "knight") ||
					!strcmp(class_table[ch->class].name, "warlock") ||
					!strcmp(class_table[victim->class].name, "ranger"))
				chance += 20;
		    else if (!strcmp(class_table[victim->class].name, "assassin") ||
//Warlocks Old Spot, Changed December 19, 2005						
				     !strcmp(class_table[victim->class].name, "bard"))
				chance += 15;
		    else
				chance += 8;
		}

		if (victim->pcdata->strategy >= 3) 
			chance += -30;
	
    }
    if (chance >= 55)
		chance = 55;
		
	if (is_affected(victim,gsn_speed_song))
	chance += 8;

    if (chance < 0)
		chance = 0;

    if (number_percent() >= chance + victim->level - ch->level)
		return FALSE;

    if (!IS_SET(victim->act, PLR_NOSPAM))
		act("You parry $n's attack.", ch, NULL, victim, TO_VICT);

    if (!IS_SET(ch->act, PLR_NOSPAM))
		act("$N parries your attack.", ch, NULL, victim, TO_CHAR);

    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {

	if (!IS_SET(gch->act, PLR_NOSPAM)) {
	    if (IS_AWAKE(gch)) {
		PERS(ch, gch, pers_buf);
		sprintf(buf1, "%s", pers_buf);
		PERS(victim, gch, pers_buf);
		sprintf(buf, "%s parries %s's attack.\n\r", pers_buf, buf1);
		buf[0] = UPPER(buf[0]);

		if (gch != ch && gch != victim)
		    send_to_char(buf, gch);
	    }
	}
    }

    /* act( "$N parries $n's attack.", ch, NULL, victim, TO_NOTVICT    );
     */
    check_improve(victim, gsn_parry, TRUE, 6);
    return TRUE;
}

/*
 * Check for shield block.
 */
bool check_shield_block(CHAR_DATA * ch, CHAR_DATA * victim)
{
    int chance;

    if (!IS_AWAKE(victim))
	return FALSE;

    chance = get_skill(victim, gsn_shield_block) / 5;

    if (get_eq_char(victim, WEAR_SHIELD) == NULL)
	return FALSE;

    if (number_percent() >= chance + victim->level - ch->level)
	return FALSE;

    if (!IS_SET(ch->act, PLR_NOSPAM)) {
        act("You block $n's attack with your shield.", ch, NULL, victim, TO_VICT);
        act("$N blocks your attack with a shield.", ch, NULL, victim, TO_CHAR);
        act("$N blocks $n's attack with a shield.", ch, NULL, victim, TO_NOTVICT);
    }

    check_improve(victim, gsn_shield_block, TRUE, 6);
    return TRUE;
}

int get_dodge(CHAR_DATA * ch)
{
    int chance;

    chance = get_skill(ch, gsn_dodge) / 4;

    if (!IS_NPC(ch)) {

	if (!strcmp(class_table[ch->class].name, "warrior") ||
	    !strcmp(class_table[ch->class].name, "barbarian") ||
	    !strcmp(class_table[ch->class].name, "ranger") ||
            !strcmp(class_table[ch->class].name, "warlock")
	    )
	    chance += ch->level / 4;

	if (!strcmp(class_table[ch->class].name, "bard") ||
	    !strcmp(class_table[ch->class].name, "knight") ||
	    !strcmp(class_table[ch->class].name, "monk") ||
	    !strcmp(class_table[ch->class].name, "deathknight")
	    )
	    chance += ch->level / 5;

	if (ch->pcdata->strategy <= 1) {

	    if (!strcmp(class_table[ch->class].name, "warrior") ||
		!strcmp(class_table[ch->class].name, "barbarian") ||
		!strcmp(class_table[ch->class].name, "ranger") ||
		!strcmp(class_table[ch->class].name, "assassin") ||
                !strcmp(class_table[ch->class].name, "warlock")
		)
		chance += 30;
	    else if (!strcmp(class_table[ch->class].name, "monk") ||
		     !strcmp(class_table[ch->class].name, "knight") ||
		     !strcmp(class_table[ch->class].name, "bard") ||
		     !strcmp(class_table[ch->class].name, "deathknight")
		)
		chance += 20;
		else if (!strcmp(class_table[ch->class].name, "mage") ||
				 !strcmp(class_table[ch->class].name, "soldier"))
				 chance -= 30;
	    else
		chance += 10;
	}
	if (ch->pcdata->strategy >= 3) {
	    chance += -40;
	}
    }
//    if (IS_SET(ch->act, PLR_SUNWEAK))
//	chance += -20;

    if (chance >= 85)
	chance = 85;
    if (chance < 0)
	chance = 0;

    return chance;
}

/*
 * Check for dodge.
 */
bool check_dodge(CHAR_DATA * ch, CHAR_DATA * victim)
{
    int chance;
    CHAR_DATA *gch;
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char pers_buf[MAX_STRING_LENGTH];

    if (!IS_AWAKE(victim))
	return FALSE;

    if (MOUNTED(victim))
	return FALSE;

    chance = get_skill(victim, gsn_dodge) / 4;

    if (!can_see(ch, victim)) {
	if (get_skill(ch, gsn_blind_fighting) > 0) {
	    if (number_percent() <= get_skill(ch, gsn_blind_fighting))
		check_improve(ch, gsn_blind_fighting, TRUE, 6);
	    else
		chance /= 3;
	} else
	    chance /= 3;
    }
    if (IS_SET(victim->act, PLR_SUNWEAK))
	chance += -20;

    if (!IS_NPC(victim)) {
    if( !strcmp(class_table[victim->class].name, "warrior" ) ||
	      !strcmp(class_table[victim->class].name, "barbarian")||
	      !strcmp(class_table[victim->class].name, "ranger")||
	      !strcmp(class_table[victim->class].name, "assassin") ||
              !strcmp(class_table[victim->class].name, "warlock") 
	     )
	chance += victim->level / 4;

	  if( !strcmp(class_table[victim->class].name, "monk" ) ||
	      !strcmp(class_table[victim->class].name, "knight"  ) ||
	      !strcmp(class_table[victim->class].name, "deathknight"  ) 
	    )
	    chance += ch->level/5; 

	if (victim->pcdata->strategy <= 1) {

	    if (!strcmp(class_table[victim->class].name, "warrior") ||
		!strcmp(class_table[victim->class].name, "barbarian") ||
		!strcmp(class_table[victim->class].name, "ranger") ||
		!strcmp(class_table[victim->class].name, "assassin") ||
                !strcmp(class_table[victim->class].name, "warlock")
		)
		chance += 40;
	    else if (!strcmp(class_table[victim->class].name, "bard") ||
		     !strcmp(class_table[victim->class].name, "knight") ||
		     !strcmp(class_table[victim->class].name, "deathknight")
		)
		chance += 35;
	    else
		chance += 35;
	}
	if (victim->pcdata->strategy >= 3) {
	    chance += -40;
	}
    }
    if (chance >= 85)
	chance = 85;
    if (chance < 0)
	chance = 0;
	
		if (is_affected(victim,gsn_speed_song))
	chance += 10;

    if (number_percent() >= chance + victim->level - ch->level)
	return FALSE;

    if (!IS_SET(victim->act, PLR_NOSPAM))
	act("You dodge $n's attack.", ch, NULL, victim, TO_VICT);

    if (!IS_SET(ch->act, PLR_NOSPAM))
	act("$N dodges your attack.", ch, NULL, victim, TO_CHAR);

    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {

	if (!IS_SET(gch->act, PLR_NOSPAM)) {
	    if (IS_AWAKE(gch)) {
		PERS(ch, gch, pers_buf);
		sprintf(buf1, "%s", pers_buf);
		PERS(victim, gch, pers_buf);
		sprintf(buf, "%s dodges %s's attack.\n\r",
			pers_buf, buf1);
		buf[0] = UPPER(buf[0]);

		if (gch != ch && gch != victim)
		    send_to_char(buf, gch);
	    }
	}
    }

    /*act( "$N dodges $n's attack.", ch, NULL, victim, TO_NOTVICT    );
     */
    check_improve(victim, gsn_dodge, TRUE, 6);
    return TRUE;
}

/*
 * Set position of a victim.
 */
 void update_pkpos(CHAR_DATA * victim)
{
	    if(victim->fighting != NULL)
		victim->position = POS_FIGHTING;
}

void update_pos(CHAR_DATA * victim)
{
    if (victim->hit > 0) {
	if (victim->position <= POS_STUNNED)
	    victim->position = POS_RESTING;
	return;
    }
    if (IS_NPC(victim) && victim->hit < 1) {
	victim->position = POS_DEAD;
	return;
    }
    if (victim->hit <= -11) {
	victim->position = POS_DEAD;
	return;
    }
    if (victim->hit <= -6)
	victim->position = POS_MORTAL;
    else if (victim->hit <= -3)
	victim->position = POS_INCAP;
    else
	victim->position = POS_STUNNED;

    return;
}

/*
 * Start fights.
 */
void set_fighting(CHAR_DATA * ch, CHAR_DATA * victim)
{
    if (ch->fighting != NULL) {
	bug("Set_fighting: already fighting", 0);
	return;
    }
    if (IS_AFFECTED(ch, AFF_SLEEP))
	affect_strip(ch, gsn_sleep);

    ch->fighting = victim;

    if (ch->position >= POS_FIGHTING)
	ch->position = POS_FIGHTING;

    return;
}

/*
 * Stop fights.
 */
void stop_fighting(CHAR_DATA * ch, bool fBoth)
{
    CHAR_DATA *fch;

    for (fch = char_list; fch != NULL; fch = fch->next) {
	if (fch == ch || (fBoth && fch->fighting == ch)) {
	    fch->fighting = NULL;
	    fch->position = IS_NPC(fch) ? fch->default_pos : POS_STANDING;
	    update_pos(fch);
	}
    }

    return;
}

/*
 * Make a corpse out of a character.
 */
void make_corpse(CHAR_DATA * ch, CHAR_DATA * killer)
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char chancebuf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;
    int itemchance = 0;
      
    buf2[0] = '\0';

    if (IS_NPC(ch)) {
	name = ch->short_descr;
	corpse = create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
	corpse->timer = number_range(3, 6);
	if (ch->gold > 0 || ch->silver > 0) {
	    obj_to_obj(create_money(ch->gold, ch->silver), corpse);
	    ch->gold = 0;
	    ch->silver = 0;
	}
	corpse->cost = 0;
    } else {

	if (!IS_NPC(killer) && !IS_NPC(ch)) {
	    if ((ch->level < LVL_LEGEND) && !pc_race_table[ch->race].isRemort) {
		if (killer->level > (ch->level + 10))
		    return;
	    }
	}
	if ((!IS_NPC(killer) && !IS_NPC(ch)) &&
	    (GET_SIDE(ch) != GET_SIDE(killer))
	    ) {

	    if (ch->level > 100)
			sprintf(buf2, "Immortal %s", ch->name);
		else if (ch->level >= LVL_LEGEND)
			sprintf(buf2, "Legendary %s", ch->name);
		else if (ch->level >= LVL_REGENT)
			sprintf(buf2, "Regent %s", ch->name);
	    else if (ch->level >= LVL_LORD){
			if (ch->sex == SEX_MALE)
			    sprintf(buf2, "Lord %s", ch->name);
			else
				sprintf(buf2, "Lady %s", ch->name);}
	    else if (ch->level >= LVL_MASTER){
			if (ch->sex == SEX_MALE)
				sprintf(buf2, "Master %s", ch->name);
			else
				sprintf(buf2, "Mistress %s", ch->name);}
	    else if (ch->level >= LVL_DUKE){
			if (ch->sex == SEX_MALE)
			    sprintf(buf2, "Duke %s", ch->name);
			else
			    sprintf(buf2, "Duchess %s", ch->name);}
	    else if (ch->level >= LVL_COUNT){
			if (ch->sex == SEX_MALE)
			    sprintf(buf2, "Count %s", ch->name);
			else
			    sprintf(buf2, "Countess %s", ch->name);}
		else if (ch->level >= LVL_BARON){
			if (ch->sex == SEX_MALE)
			    sprintf(buf2, "Baron %s", ch->name);
			else
			    sprintf(buf2, "Baroness %s", ch->name);}
//		else if (ch->level >= LVL_LEGEND)
//			sprintf(buf2, "Legendary %s", ch->name);
/*
		else if (ch->pcdata->judge == 3 && ch->level >=50)
		  	    sprintf(buf2, "Mercenary %s", ch->name);
		     if (ch->pcdata->judge == 3 && ch->level >=54)
			    sprintf(buf2, "Ser %s the Hedge Knight",ch->name);

*/				
/* 		set_prename(ch);
		if (ch->level >= LVL_LEGEND)
			sprintf(buf2, "%s %s",ch->pcdata->prename, ch->name);
*/ 		else 
			sprintf(buf2, "a %s", race_table[ch->race].name);
	    name = buf2;
	} else
	    name = ch->name;
	    
	    	/*	else if (ch->pcdata->judge == 3 && ch->level >=50)
		  	    sprintf(buf2, "Mercenary %s", ch->name);
		     if (ch->pcdata->judge == 3 && ch->level >=54)
			    sprintf(buf2, "Ser %s the Hedge Knight",ch->name);*/

				

	corpse = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
	corpse->timer = number_range(25, 40);
	REMOVE_BIT(ch->act, PLR_CANLOOT);
	if (!is_clan(ch))
	    corpse->owner = str_dup(ch->name);
	else {
	    corpse->owner = NULL;
	    if (ch->gold > 1 || ch->silver > 1) {
		obj_to_obj(create_money(ch->gold / 2, ch->silver / 2), corpse);
		ch->gold -= ch->gold / 2;
		ch->silver -= ch->silver / 2;
	    }
	}

	corpse->cost = 0;
    }

    corpse->level = ch->level;

    sprintf(buf, corpse->short_descr, name);
    free_string(corpse->short_descr);
    corpse->short_descr = str_dup(buf);

    sprintf(buf, corpse->description, name);
    free_string(corpse->description);
    corpse->description = str_dup(buf);

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
	bool floating = FALSE;
	obj_next = obj->next_content;
	itemchance = 20; // for mobs
	if (!IS_NPC(ch))  // to make sure only players can have 30+ itemchance (nodrop)
	itemchance = number_range(1,100);

	if ( (is_name("restring",obj->name)) 
	|| (is_name("unique",obj->name))
	|| (is_name("basket",obj->name))
	|| (is_name("potion",obj->name))
	|| (is_name("scroll",obj->name)) )  // Restrings always dropped, baskets, scrolls, pots. UNLESS difference of more than 2
	itemchance = 20;
	
	if (!IS_SET(ch->act, PLR_NOSPAM) && !IS_NPC(ch)){
	sprintf(chancebuf, "Chance on {Y%s{x is {R%d percent{X.\n\r", obj->short_descr, itemchance);
					send_to_char(chancebuf, ch);
				}
	
	if (itemchance >= 30)  // chance that item is dropped
	continue;
	
	else if (itemchance < 30) {
	if (obj->wear_loc == WEAR_FLOAT)
	    floating = TRUE;
	obj_from_char(obj);
	if (obj->item_type == ITEM_POTION)
	    obj->timer = number_range(500, 1000);
	if (obj->item_type == ITEM_SCROLL)
	    obj->timer = number_range(1000, 2500);
	if (IS_SET(obj->extra_flags, ITEM_ROT_DEATH) && !floating) {
	    obj->timer = number_range(5, 10);
	    REMOVE_BIT(obj->extra_flags, ITEM_ROT_DEATH);
	}
	REMOVE_BIT(obj->extra_flags, ITEM_VIS_DEATH);

	if (IS_SET(obj->extra_flags, ITEM_INVENTORY))
	    extract_obj(obj);
	else if (floating) {
	    if (IS_OBJ_STAT(obj, ITEM_ROT_DEATH)) {	/* get rid of it! */
		if (obj->contains != NULL) {
		    OBJ_DATA *in, *in_next;

		    act("$p evaporates,scattering its contents.",
			ch, obj, NULL, TO_ROOM);
		    for (in = obj->contains; in != NULL; in = in_next) {
			in_next = in->next_content;
			obj_from_obj(in);
			obj_to_room(in, ch->in_room);
		    }
		} else
		    act("$p evaporates.",
			ch, obj, NULL, TO_ROOM);
		extract_obj(obj);
	    } else {
		act("$p falls to the floor.", ch, obj, NULL, TO_ROOM);
		obj_to_room(obj, ch->in_room);
	    }
	} else
	    obj_to_obj(obj, corpse);
    }
	name = '\0';
	    //random eq
    int multi = getMulti(ch);
    if (multi > 0){
	addRandomItemsToCorpse(corpse, ch, multi);
    }
    }

    obj_to_room(corpse, ch->in_room);
    return;
}


/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry(CHAR_DATA * ch, CHAR_DATA * killer)
{
    ROOM_INDEX_DATA *was_in_room;
    char buf9[MAX_STRING_LENGTH];
    char *msg;
    int door;
    int vnum;

    vnum = 0;
    msg = "You hear $n's death cry.";

    buf9[0] = '\0';

    switch (number_bits(4)) {
    case 0:
	msg = "$n hits the ground ... DEAD.";
	break;
    case 1:
	if (ch->material == 0) {
	    msg = "$n splatters blood on your armor.";
	    break;
	}
    case 2:
	if (IS_SET(ch->parts, PART_GUTS)) {
	    msg = "$n spills $s guts all over the floor.";
	    vnum = OBJ_VNUM_GUTS;
	}
	break;
    case 3:
	if (IS_SET(ch->parts, PART_HEAD)) {
	    msg = "$n's severed head plops on the ground.";
	    vnum = OBJ_VNUM_SEVERED_HEAD;
	}
	break;
    case 4:
	if (IS_SET(ch->parts, PART_HEART)) {
	    msg = "$n's heart is torn from $s chest.";
	    vnum = OBJ_VNUM_TORN_HEART;
	}
	break;
    case 5:
	if (IS_SET(ch->parts, PART_ARMS)) {
	    msg = "$n's arm is sliced from $s dead body.";
	    vnum = OBJ_VNUM_SLICED_ARM;
	}
	break;
    case 6:
	if (IS_SET(ch->parts, PART_LEGS)) {
	    msg = "$n's leg is sliced from $s dead body.";
	    vnum = OBJ_VNUM_SLICED_LEG;
	}
	break;
    case 7:
	if (IS_SET(ch->parts, PART_BRAINS)) {
	    msg = "$n's head is shattered, and $s brains splash all over you.";
	    vnum = OBJ_VNUM_BRAINS;
	}
    }

    act(msg, ch, NULL, NULL, TO_ROOM);

    if (vnum != 0) {
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	char *name;

	if (!IS_NPC(ch) && !IS_NPC(killer) &&
	    (GET_SIDE(ch) != GET_SIDE(killer))
	    ) {
	    
		if (ch->level > 100)
			sprintf(buf9, "the Immortal %s", IS_NPC(ch) ? ch->short_descr : ch->name);
	    else if (ch->level >= LVL_LORD)
			if (ch->sex == SEX_MALE)
				sprintf(buf9, "the Lord %s", IS_NPC(ch) ? ch->short_descr : ch->name);
			else
			    sprintf(buf9, "the Lady %s", IS_NPC(ch) ? ch->short_descr : ch->name);
	    else if (ch->level >= LVL_MASTER)
			if (ch->sex == SEX_MALE)
				sprintf(buf9, "Master %s", IS_NPC(ch) ? ch->short_descr : ch->name);
			else
			    sprintf(buf9, "Mistress %s", IS_NPC(ch) ? ch->short_descr : ch->name);
	    else if (ch->level >= LVL_DUKE)
			if (ch->sex == SEX_MALE)
				sprintf(buf9, "the Duke %s", IS_NPC(ch) ? ch->short_descr : ch->name);
			else
				sprintf(buf9, "the Duchess %s", IS_NPC(ch) ? ch->short_descr : ch->name);
	    else if (ch->level >= LVL_COUNT)
			if (ch->sex == SEX_MALE)
			    sprintf(buf9, "the Count %s", IS_NPC(ch) ? ch->short_descr : ch->name);
			else
				sprintf(buf9, "the Countess %s", IS_NPC(ch) ? ch->short_descr : ch->name);
	    else
	    	if (ch->level >= LVL_BARON)
			if (ch->sex == SEX_MALE)
			    sprintf(buf9, "the Baron %s", IS_NPC(ch) ? ch->short_descr : ch->name);
			else
				sprintf(buf9, "the Baroness %s", IS_NPC(ch) ? ch->short_descr : ch->name);
	    else if (ch->level >= LVL_LEGEND)
			sprintf(buf9, "the Legendary %s", IS_NPC(ch) ? ch->short_descr : ch->name);
		else if (ch->level >= LVL_REGENT)
			sprintf(buf9, "the Regent %s", IS_NPC(ch) ? ch->short_descr : ch->name);
/* 		set_prename(ch);
		if(ch->level >= LVL_LEGEND)
			sprintf(buf9, "the %s %s",ch->pcdata->prename, IS_NPC(ch) ? ch->short_descr : ch->name);
*/ 		else
			sprintf(buf9, "a %s", race_table[ch->race].name);
	    name = buf9;
	} else
	    name = IS_NPC(ch) ? ch->short_descr : ch->name;

	obj = create_object(get_obj_index(vnum), 0);
	obj->timer = number_range(4, 7);

	sprintf(buf, obj->short_descr, name);
	free_string(obj->short_descr);
	obj->short_descr = str_dup(buf);

	sprintf(buf, obj->description, name);
	free_string(obj->description);
	obj->description = str_dup(buf);

	if (obj->item_type == ITEM_FOOD) {
	    if (IS_SET(ch->form, FORM_POISON))
		obj->value[3] = 1;
	    else if (!IS_SET(ch->form, FORM_EDIBLE))
		obj->item_type = ITEM_TRASH;
	}
	obj_to_room(obj, ch->in_room);
    }
    if (IS_NPC(ch))
	msg = "You hear something's death cry.";
    else
	msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    for (door = 0; door <= 5; door++) {
	EXIT_DATA *pexit;

	if ((pexit = was_in_room->exit[door]) != NULL
	    && pexit->u1.to_room != NULL
	    && pexit->u1.to_room != was_in_room) {
	    ch->in_room = pexit->u1.to_room;
	    act(msg, ch, NULL, NULL, TO_ROOM);
	}
    }
    ch->in_room = was_in_room;

    return;
}

void raw_kill(CHAR_DATA * victim, CHAR_DATA * ch, int isLgg)
{
    int i;
    char buf[MAX_STRING_LENGTH];

    stop_fighting(victim, TRUE);
    //Memleak fix - Akuji 3/23/03
    stop_hating(ch);
    stop_hating(victim);
    death_cry(victim, ch);
// Saves char on kills
    save_char_obj(ch);
    save_char_obj(victim);

    if ( victim == quest_target )
     quest_target = NULL;
    if ( victim == quest_mob )
    {
      sprintf( buf, "Oh well, I guess the quest is over, since I am about to {RDIE!!!!!{x" );
      do_crusade( victim, buf, 9 );
      
      quest_mob = NULL;
    }

    if(!isLgg)
		make_corpse(victim, ch);
		
		// LEGEND + KBPS SEEM TO NOT BE WORKING! WE'LL PUT IT HERE. HAR.
		if (!IS_NPC(victim) && !IS_NPC(ch) && victim->level >= 50)
		victim->playerkilled += 1;
		
    if (IS_NPC(victim)) 
	{
		victim->pIndexData->killed++;
		kill_table[URANGE(0, victim->level, MAX_LEVEL - 1)].killed++;
		extract_char(victim, TRUE);
		return;
    }
    if (IS_NPC(ch))
		extract_char(victim, FALSE);
    else //if is pc
	{
		if(isLgg)
			extract_char_keepeq(victim, FALSE);

		else if (victim->level < LVL_LEGEND && !pc_race_table[victim->race].isRemort) 
		{
		    if ((ch->level > (victim->level + 10))) 
			{
				extract_char_keepeq(victim, FALSE);

				while (victim->affected)
					affect_remove(victim, victim->affected);

				victim->affected_by = race_table[victim->race].aff;
				victim->position = POS_RESTING;
				victim->hit = UMAX(1, victim->hit);
				victim->mana = UMAX(1, victim->mana);
				victim->move = UMAX(1, victim->move);

				return;
			}
			else
				extract_char(victim, FALSE);
		} 
		else
		    extract_char(victim, FALSE);
	}
    while (victim->affected)
		affect_remove(victim, victim->affected);
	victim->affected_by = race_table[victim->race].aff;


    for (i = 0; i < 4; i++)
		victim->armor[i] = 100;

    victim->position = POS_RESTING;
    victim->hit = UMAX(1, victim->hit);
    victim->mana = UMAX(1, victim->mana);
    victim->move = UMAX(1, victim->move);
    save_char_obj(victim);
//armageddon code
	if(armageddon && !IS_NPC(victim)) do_quit(victim, "");
	    return;
}

void calc_warpoints(CHAR_DATA * winner, CHAR_DATA * loser)
{
    if (winner->side == loser->side)
	return;

//if (winner->side != SIDE_CAIRN)
//{

    if (winner->side == SIDE_TIER)
	main_warscore->tier_killed++;
    if (winner->side == SIDE_KAEL)
	main_warscore->kael_killed++;
    if (winner->side == SIDE_UM)
	main_warscore->um_killed++;
  if (winner->side == SIDE_CAIRN)
	main_warscore->cairn_killed++;
    if (loser->side == SIDE_TIER)
	main_warscore->tier_lost++;
    if (loser->side == SIDE_KAEL)
	main_warscore->kael_lost++;
    if (loser->side == SIDE_UM)
	main_warscore->um_lost++;
    if (loser->side == SIDE_CAIRN)
	main_warscore->cairn_lost++;

//}

    if (loser->level >= LEGEND_LEVEL) {
//		if (winner->side != SIDE_CAIRN)
//		{
	if (winner->side == SIDE_TIER)
	    main_warscore->tier_legends_killed++;
	if (winner->side == SIDE_KAEL)
	    main_warscore->kael_legends_killed++;
	if (winner->side == SIDE_UM)
	    main_warscore->um_legends_killed++;
//		}
 if (winner->side == SIDE_CAIRN)
    main_warscore->cairn_legends_killed++;

	if (loser->side == SIDE_TIER)
	    main_warscore->tier_legends_lost++;
	if (loser->side == SIDE_KAEL)
	    main_warscore->kael_legends_lost++;
	if (loser->side == SIDE_UM)
	    main_warscore->um_legends_lost++;
	if (loser->side == SIDE_CAIRN)
	    main_warscore->cairn_legends_lost++;
//              winner->warpoints++;
	//              winner->leg_killed++;
    }
//        winner->warpoints++;
    //        winner->enemies_killed++;


if (loser->level < 50)
{
 loser->playerkilled++;
}
    save_char_obj(loser);
    save_char_obj(winner);
    save_warscore();
}

// Note to self... we prolly need to cache this database.
void update_toplist(CHAR_DATA *ch)
{
  sqlite3 * db;
  int rc = sqlite3_open(TOPPLAYER_DB,&db);
  char * error = NULL;
  char buf[MAX_STRING_LENGTH];
  if(rc)
  {
    bug("Couldn't open TOPPLAYERS DB. Reason:",0);
    bug(sqlite3_errmsg(db),0);
    sqlite3_close(db);
    return;
  }
  
  sprintf(buf,"INSERT OR REPLACE into topplayer (name, race, kingdom,level,warpoints,class,playerkilled) VALUES(\"%s\",%d,%d,%d,%d,%d,%d)",
               ch->name,ch->race,ch->side,ch->level,ch->warpoints,ch->class,ch->playerkilled);

		
  rc = sqlite3_exec(db,buf,NULL,NULL,&error);
  if(rc != SQLITE_OK)
  {
    bug("Couldn't exec string. Reason:",0);
    bug(sqlite3_errmsg(db),0);
  } 
  sqlite3_close(db);
}

void trophy_gain(CHAR_DATA * ch, CHAR_DATA * victim, int isLgg)
{
    extern TOPPLAYER_DATA *new_warlist(void);
//    TOPPLAYER_DATA *temp;
    char buf[MAX_STRING_LENGTH];
    char buf10[MAX_STRING_LENGTH];
    char pers_buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    int pos;
    bool is_inserted;
    bool is_modified;
    DESCRIPTOR_DATA *d;
//    TOPPLAYER_DATA *warlist;

    /* NPC's do not get trophy's */
    if (IS_NPC(ch))
		return;

/* no trophies for mobs */
    if (IS_NPC(victim))
		return;

//look at everyone
    for (d = descriptor_list; d; d = d->next) 
	{
		is_inserted = FALSE;
		is_modified = FALSE;
		gch = d->character;

		if(gch == NULL) 
			continue;

		if (IS_NPC(gch))
		    continue;

// if theyre grouped and in the zone
		if (is_same_group(d->character, ch) && d->character->in_room->area == ch->in_room->area) {
		    if ((!IS_NPC(victim) && !IS_NPC(gch)) &&
				(GET_SIDE(victim) != GET_SIDE(gch))) 
			{
				{
					sprintf(log_buf, "%s gave wp to %s", (IS_NPC(victim) ? victim->short_descr : victim->name), (IS_NPC(ch) ? ch->short_descr : ch->name));
				    log_string(log_buf);
				}
				gch->enemies_killed++;
				gch->warpoints++;
				if (victim->level >= LEGEND_LEVEL)
				    gch->leg_killed++;
					
							// no imms
							if (!IS_IMMORTAL(ch))
							{
                                update_toplist(gch);
							}
/**** This is some ugly code. And now it's coming out. -- Archan
				for (temp = main_warlist; temp != NULL; temp = temp->next) 
				{

					if (!strcmp(gch->name, temp->name)) 
					{
						if (is_modified == FALSE) 
						{
						    temp->warpoints = gch->warpoints; 
						    temp->race = gch->race;
						    temp->kingdom = gch->side;
						    temp->level = gch->level;
						    temp->playerkilled = gch->playerkilled;
						    temp->class = gch->class;
						    is_modified = TRUE;
						} 
						else 
						{
						    temp->prev->next = temp->next;
						    if (temp->next != NULL)
								temp->next->prev = temp->prev;

						    free_mem(temp, sizeof(TOPPLAYER_DATA));
						    break;
						}
				    }
// kill kbps					if (((gch->warpoints - gch->playerkilled) > (temp->warpoints - temp->playerkilled)) &&
					if (((gch->warpoints) > (temp->warpoints)) &&
						!is_inserted && !is_modified) 
					{
						warlist = new_warlist();
						warlist->next = temp;
						warlist->prev = temp->prev;

						if (temp->prev != NULL)
						    temp->prev->next = warlist;
						else
						    main_warlist = warlist;

						temp->prev = warlist;
						warlist->name = strdup(gch->name);
						warlist->warpoints = gch->warpoints;
						warlist->kingdom = gch->side;
						warlist->race = gch->race;
						warlist->playerkilled = gch->playerkilled;
						warlist->level = gch->level;
						warlist->class = gch->class;
						is_inserted = TRUE;
						is_modified = TRUE;
				    }
				    if (is_inserted && temp->next == NULL) 
					{
						if (temp->prev != NULL)
						    temp->prev->next = NULL;
						if (temp != NULL) 
						{
						    free_mem(temp, sizeof(TOPPLAYER_DATA));
						}
				    }
				} */
		/* save the list */
//				save_topplayers();
		    }
	    /* search trophy of player */
		    for (pos = 0; pos < MAX_TROPHY; pos++) 
			{
				if (IS_NPC(victim) && !IS_NPC(gch)) 
				{

				} 
			else if (!IS_NPC(victim) && !IS_NPC(gch)) 
			{
			    PERS(victim, ch, pers_buf);
			    sprintf(buf10, "%s", pers_buf);

			    if (gch->pcdata->trophy[pos] == NULL) 
				{
					if (!IS_NPC(gch) && (GET_SIDE(gch) != GET_SIDE(victim))) 
					{
					    gch->pcdata->trophy[pos] = str_dup(buf10);
					} 
					else 
					{
					    break;
					}

					gch->pcdata->trophy_sub[pos] = 1;
					sprintf(buf, "{YAdding %s to your trophy.{x \n\r",
					gch->pcdata->trophy[pos]);
					send_to_char(buf, gch);
					break;
			    } 
				else if (!strcmp(victim->name, gch->pcdata->trophy[pos]) ||
			       !strcmp(buf10, gch->pcdata->trophy[pos])) 
				{
					if (!IS_NPC(gch) && (GET_SIDE(gch) != GET_SIDE(victim))) 
					{
					    gch->pcdata->trophy_sub[pos] += 1;
					    sprintf(buf, "You have killed %s %d times. \n\r",
					    gch->pcdata->trophy[pos],
					    gch->pcdata->trophy_sub[pos]);
					    send_to_char(buf, gch);
					}
					break;
			    }
			}
		}
	}
	}
}

void group_gain(CHAR_DATA * ch, CHAR_DATA * victim, int isLgg)
{
    char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    int xp;
    int members;
    int group_levels;
   // int killchs = 0;
//	int victchs = 0;
//	int difference = 0; // Always with respect to the killers side
		

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if (victim == ch)
	return;
    if (IS_SET(ch->act, PLR_QUESTOR) && IS_NPC(victim)) {
	if (ch->questmob == victim->pIndexData->vnum) {
	    send_to_char("You have almost completed your QUEST!\n\r", ch);
	    send_to_char("Return to the questmaster before your time runs out.\n\r", ch);
	    ch->questmob = -1;
	}
    }
    members = 0;
    group_levels = 0;


//    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {
//	if (is_same_group(gch, ch)) {
//	    members++;
//	    group_levels += IS_NPC(gch) ? gch->level / 2 : gch->level;
//	}
//    }


	//added for area xp calc - Akuji
    for (d = descriptor_list; d; d = d->next) 
	{
		gch = d->character;

		if (gch == NULL)
			continue;
		
		if ((gch->in_room == NULL) || (ch->in_room == NULL)
			|| (gch->in_room->area == NULL) || (ch->in_room->area == NULL))
			continue;


		if ( gch->in_room->area->vnum == ch->in_room->area->vnum )
		{
			if (is_same_group(gch, ch)) 
			{
				members++;
				group_levels += IS_NPC(gch) ? gch->level / 2 : gch->level;
			}
		}
	}

    if (members == 0) {
	bug("Group_gain: members.", members);
	members = 1;
	group_levels = ch->level;
    }
    lch = (ch->leader != NULL) ? ch->leader : ch;
 
//    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {

	//added for area xp calc - Akuji
    for (d = descriptor_list; d; d = d->next) 
	{
	gch = d->character;
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

		if (gch == NULL)
			continue;
		
		if ((gch->in_room == NULL) || (ch->in_room == NULL)
			|| (gch->in_room->area == NULL) || (ch->in_room->area == NULL))
			continue;
		
		// I think this was missing for area xp calc.... Akuji
		if ( gch->in_room->area->vnum != ch->in_room->area->vnum )
			continue;

		if (!is_same_group(gch, ch))
			continue;

/*      Taken out, add it back if you want it */
	if (gch->level - lch->level >= 20) {
	    send_to_char("You are too high level for this group.\n\r", gch);
	    continue;
	}
	if (gch->level - lch->level <= -20) {
	    send_to_char("You are too low for this group.\n\r", gch);
	    continue;
	}
    if (members == 1) xp = xp_compute(gch, victim, group_levels, FALSE);
    else xp = xp_compute(gch, victim, group_levels, TRUE);

	if (gch->level - lch->level <= -10) {
	    xp /= (lch->level - gch->level);
	} else if (gch->level - lch->level >= 10) {
	    xp /= (gch->level - lch->level);
	}
	
	//Izlude, 10-17-05, readding the XP bonus
	if (IS_NPC(victim) 
	&& (victim->level > ch->level-2)
	&& ch->level < 56) //Not at 49, to prevent instant-51ing
	xp *= number_range(3,4); //2 to 4x bonus for mob kills higher levels
	
	if (!IS_NPC(victim) && (victim->level > ch->level))
	xp *= number_range(5,7)/4; //1.25 to 1.75 bonus for PK higher levels
	
// 22MAR03 - increased exp from npc's to 500 for characters under
// level 50, at lvl 50, the max achieved is 300 through pk.
// Love, Hobbie
	if (xp > 4000 && IS_NPC(victim))
	    xp = 4000;
//
//	if (xp > 350 && IS_NPC(victim))
//	   xp = 350;

	if (xp > 300 && !IS_NPC(victim))
	    xp = 300;
// for beta testing, I added the following
	//      xp *= 5;

	if (!IS_NPC(victim) && !IS_NPC(gch)) {
	    if (GET_SIDE(victim) == GET_SIDE(gch))
		xp = 0;

	    if (ch->level > (victim->level + 20))
		xp = 0;
	}
	if( IS_NPC(victim) && IS_SET(victim->act, ACT_NOXP))
		xp = 0;

	if(isLgg)
		xp = 0;

	sprintf(buf, "You receive %d experience points.\n\r", xp);
	send_to_char(buf, gch);
	
	gain_exp(gch, xp);
    save_char_obj(gch);
	for (obj = ch->carrying; obj != NULL; obj = obj_next) {
	    obj_next = obj->next_content;
	    if (obj->wear_loc == WEAR_NONE)
		continue;

	    /*
	       if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
	       ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
	       ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) ) )
	       {
	       act( "You are zapped by $p.", ch, obj, NULL, TO_CHAR );
	       act( "$n is zapped by $p.",   ch, obj, NULL, TO_ROOM );
	       obj_from_char( obj );
	       obj_to_room( obj, ch->in_room );
	       }
	     */
	}
    }

    return;
}

/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute(CHAR_DATA * gch, CHAR_DATA * victim, int total_levels, bool grouped)
{
    int xp, base_exp;
    int align, level_range;
    int change;
    int time_per_level;

    level_range = victim->level - gch->level;

    /* compute the base exp */
    switch (level_range) {
    default:
	base_exp = 0;
	break;
    case -9:
	base_exp = 1;
	break;
    case -8:
	base_exp = 2;
	break;
    case -7:
	base_exp = 5;
	break;
    case -6:
	base_exp = 9;
	break;
    case -5:
	base_exp = 11;
	break;
    case -4:
	base_exp = 22;
	break;
    case -3:
	base_exp = 33;
	break;
    case -2:
	base_exp = 50;
	break;
    case -1:
	base_exp = 66;
	break;
    case 0:
	base_exp = 83;
	break;
    case 1:
	base_exp = 99;
	break;
    case 2:
	base_exp = 121;
	break;
    case 3:
	base_exp = 143;
	break;
    case 4:
	base_exp = 165;
	break;
    }

    if (level_range > 4)
	base_exp = 160 + 20 * (level_range - 4);

    /* do alignment computations */

    align = victim->alignment - gch->alignment;

    if (IS_SET(victim->act, ACT_NOALIGN)) {
	/* no change */
    } else if (align > 500) {	/* monster is more good than slayer */
	change = (align - 500) * base_exp / 500 * gch->level / total_levels;
	change = UMAX(1, change);
	gch->alignment = UMAX(-1000, gch->alignment - change);
    } else if (align < -500) {	/* monster is more evil than slayer */
	change = (-1 * align - 500) * base_exp / 500 * gch->level / total_levels;
	change = UMAX(1, change);
	gch->alignment = UMIN(1000, gch->alignment + change);
    } else {			/* improve this someday */
	change = gch->alignment * base_exp / 500 * gch->level / total_levels;
	gch->alignment -= change;
    }

    /* calculate exp multiplier */
    if (IS_SET(victim->act, ACT_NOALIGN))
	xp = base_exp;

    else if (gch->alignment > 500) {	/* for goodie two shoes */
	/*
	   if (victim->alignment < -750)
	   xp = (base_exp *4)/3;

	   else if (victim->alignment < -500)
	   xp = (base_exp * 5)/4;

	   else if (victim->alignment > 750)
	   xp = base_exp / 4;

	   else if (victim->alignment > 500)
	   xp = base_exp / 2;

	   else if (victim->alignment > 250)
	   xp = (base_exp * 3)/4; 

	   else
	 */
	xp = base_exp;

    } else if (gch->alignment < -500) {		/* for baddies */
	/*
	   if (victim->alignment > 750)
	   xp = (base_exp * 5)/4;

	   else if (victim->alignment > 500)
	   xp = (base_exp * 11)/10; 

	   else if (victim->alignment < -750)
	   xp = base_exp/2;

	   else if (victim->alignment < -500)
	   xp = (base_exp * 3)/4;

	   else if (victim->alignment < -250)
	   xp = (base_exp * 9)/10;

	   else
	 */
	xp = base_exp;

    } else if (gch->alignment > 200) {	/* a little good */
	/*
	   if (victim->alignment < -500)
	   xp = (base_exp * 6)/5;

	   else if (victim->alignment > 750)
	   xp = base_exp/2;

	   else if (victim->alignment > 0)
	   xp = (base_exp * 3)/4; 

	   else
	 */
	xp = base_exp;
    } else if (gch->alignment < -200) {		/* a little bad */
	/*
	   if (victim->alignment > 500)
	   xp = (base_exp * 6)/5;

	   else if (victim->alignment < -750)
	   xp = base_exp/2;

	   else if (victim->alignment < 0)
	   xp = (base_exp * 3)/4;

	   else
	 */
	xp = base_exp;
    } else {			/* neutral */
	/*
	   if (victim->alignment > 500 || victim->alignment < -500)
	   xp = (base_exp * 4)/3;

	   else if (victim->alignment < 200 && victim->alignment > -200)
	   xp = base_exp/2;

	   else
	 */
	xp = base_exp;
    }
    /* lowbie killing as 50+ gives reduced exp */
    if(!IS_NPC(victim) && victim->level < 50 && gch->level >= 50)
	xp = xp/10;
    /* more exp at the low levels */
    if (gch->level < 6)
	xp = 10 * xp / (gch->level + 4);

    /* less at high */
    if (gch->level > LVL_LEGEND)
	xp = 15 * xp / (gch->level - 25);

    /* reduce for playing time */

    {

	/* compute quarter-hours per level */
	time_per_level = 4 *
	    (gch->played + (int) (current_time - gch->logon)) / 3600
	    / gch->level;

	time_per_level = URANGE(2, time_per_level, 12);
	if (gch->level < 15)	/* make it a curve */
	    time_per_level = UMAX(time_per_level, (15 - gch->level));
	/* xp = xp * time_per_level / 12; */

    }

    /* randomize the rewards */
    xp = number_range(xp * 3 / 4, xp * 5 / 4);

    /* adjust for grouping */

//Fix for solo xp bug, now solo xp gets 1.3 more then 2 group xp of the same level
//Marcus
    if (grouped)
	xp = xp * gch->level / (UMAX(1, total_levels / 4));
    else
	xp = xp * gch->level / (UMAX(1, gch->level / 2)) * 1.3;
    
//to slow down pk leveling some after 52
//Rizen

    if(gch->level >= 52)
	xp = xp/1.5;

    if ((gch->level >= LVL_MOBXPCAP) && IS_NPC(victim))
	return 0;

    if ((gch->level >= LVL_PKXPCAP))
	return 0;

    return xp;
}

void dam_message(CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, bool immune)
{
    char buf1[256], buf2[256], buf3[256];
    const char *vs;
    const char *vp;
    const char *attack;
    int spell;
    char punct;
    spell = 0;

    if (ch == NULL || victim == NULL)
	return;

    if (dam == 0) {
	vs = "miss";
	vp = "misses";
    } else if (dam <= 4) {
	vs = "{gscratch{x";
	vp = "{gscratches{x";
    } else if (dam <= 8) {
	vs = "{ggraze{x";
	vp = "{ggrazes{x";
    } else if (dam <= 12) {
	vs = "{ghit{x";
	vp = "{ghits{x";
    } else if (dam <= 18) {
	vs = "{ginjure{x";
	vp = "{ginjures{x";
    } else if (dam <= 24) {
	vs = "{ywound{x";
	vp = "{ywounds{x";
    } else if (dam <= 30) {
	vs = "{ymaul{x";
	vp = "{ymauls{x";
    } else if (dam <= 38) {
	vs = "{ydecimate{x";
	vp = "{ydecimates{x";
    } else if (dam <= 44) {
	vs = "{ydevastate{x";
	vp = "{ydevastates{x";
    } else if (dam <= 50) {
	vs = "{ymaim{x";
	vp = "{ymaims{x";
    } else if (dam <= 75) {
	vs = "{rMUTILATE{x";
	vp = "{rMUTILATES{x";
    } else if (dam <= 100) {
	vs = "{rDISEMBOWEL{x";
	vp = "{rDISEMBOWELS{x";
    } else if (dam <= 125) {
	vs = "{rDISMEMBER{x";
	vp = "{rDISMEMBERS{x";
    } else if (dam <= 150) {
	vs = "{rMASSACRE{x";
	vp = "{rMASSACRES{x";
    } else if (dam <= 175) {
	vs = "{rMANGLE{x";
	vp = "{rMANGLES{x";
    } else if (dam <= 200) {
	vs = "{r*** DEMOLISH ***{x";
	vp = "{r*** DEMOLISHES ***{x";
    } else if (dam <= 225) {
	vs = "{r*** DEVASTATE ***{x";
	vp = "{r*** DEVASTATES ***{x";
    } else if (dam <= 250) {
	vs = "{R=== OBLITERATE ==={x";
	vp = "{R=== OBLITERATES ==={x";
    } else if (dam <= 300) {
	vs = "{R>>> ANNIHILATE <<<{x";
	vp = "{R>>> ANNIHILATES <<<{x";
    } else if (dam <= 350) {
	vs = "{R<<< ERADICATE >>>{x";
	vp = "{R<<< ERADICATES >>>{x";
    } else {
	vs = "{Rdo UNSPEAKABLE things to{x";
	vp = "{Rdoes UNSPEAKABLE things to{x";
    }

    punct = (dam <= 100) ? '.' : '!';

    if (dt == TYPE_HIT) {
	if (ch == victim) {
	    sprintf(buf1, "$n %s{x $melf%c", vp, punct);
	    sprintf(buf2, "You %s{x yourself%c", vs, punct);
	} else {
          if( dam == 0 )
            {
	    sprintf(buf1, "$n %s{x $N%c", vp, punct);
	    sprintf(buf2, "You %s{x $N%c", vs, punct);
	    sprintf(buf3, "$n %s{x you%c", vp, punct);
            } else {
	    sprintf(buf1, "$n %s{x $N%c [{R%d{x]", vp, punct, dam);
	    sprintf(buf2, "You %s{x $N%c [{R%d{x]", vs, punct, dam);
	    sprintf(buf3, "$n %s{x you%c [{R%d{x]", vp, punct, dam);
            }
	}
    } else {
	if (dt >= 0 && dt < MAX_SKILL) {
	    attack = skill_table[dt].noun_damage;
	    spell = 1;
	} else if (dt >= TYPE_HIT &&
		   dt < TYPE_HIT + MAX_DAMAGE_MESSAGE) {
	    attack = attack_table[dt - TYPE_HIT].noun;
	} else {
//	    bug("Dam_message: bad dt %d.", dt);
	    dt = TYPE_HIT;
	    attack = attack_table[0].name;
	}

	if (immune) {
	    if (ch == victim) {
		if (spell == 1) {
		    sprintf(buf1, "$n is unaffected by $s own {W%s{x.", attack);
		    sprintf(buf2, "Luckily, you are immune to that.");
		} else {
		    sprintf(buf1, "$n is unaffected by $s own %s{x.", attack);
		    sprintf(buf2, "Luckily, you are immune to that.");
		}
	    } else {
		if (spell == 1) {
		    sprintf(buf1, "$N is unaffected by $n's {W%s{x!", attack);
		    sprintf(buf2, "$N is unaffected by your {W%s{x!", attack);
		    sprintf(buf3, "$n's {W%s{x is powerless against you.", attack);
		} else {
		    sprintf(buf1, "$N is unaffected by $n's %s{x!", attack);
		    sprintf(buf2, "$N is unaffected by your %s{x!", attack);
		    sprintf(buf3, "$n's %s{x is powerless against you.", attack);
		}
	    }
	} else {
          if( dam == 0 )
               {
	    if (ch == victim)
             {
		if (spell == 1) {
		    sprintf(buf1, "$n's {W%s{x %s $m%c", attack, vp, punct);
		    sprintf(buf2, "Your {W%s{x %s you%c", attack, vp, punct);
		                } else {
		    sprintf(buf1, "$n's %s %s{x $m%c", attack, vp, punct);
		    sprintf(buf2, "Your %s %s{x you%c", attack, vp, punct);
		                       }
	       } else {
		if (spell == 1) {
		    sprintf(buf1, "$n's {W%s{x %s $N%c", attack, vp, punct);
		    sprintf(buf2, "Your {W%s{x %s $N%c", attack, vp, punct);
		    sprintf(buf3, "$n's {W%s{x %s you%c", attack, vp, punct);
		                } else {
		    sprintf(buf1, "$n's %s %s{x $N%c", attack, vp, punct);
		    sprintf(buf2, "Your %s %s{x $N%c", attack, vp, punct);
		    sprintf(buf3, "$n's %s %s{x you%c", attack, vp, punct);
		                       }
	              }
               } else {
           if (ch == victim)
                   {
		if (spell == 1) {
		    sprintf(buf1, "$n's {W%s{x %s $m%c [{R%d{x]", attack, vp, punct, dam);
		    sprintf(buf2, "Your {W%s{x %s you%c [{R%d{x]", attack, vp, punct, dam);
		                } else {
		    sprintf(buf1, "$n's %s %s{x $m%c [{R%d{x]", attack, vp, punct, dam);
		    sprintf(buf2, "Your %s %s{x you%c [{R%d{x]", attack, vp, punct, dam);
		                       }
	       } else {
		if (spell == 1) {
		    sprintf(buf1, "$n's {W%s{x %s $N%c [{R%d{x]", attack, vp, punct, dam);
		    sprintf(buf2, "Your {W%s{x %s $N%c [{R%d{x]", attack, vp, punct, dam);
		    sprintf(buf3, "$n's {W%s{x %s you%c [{R%d{x]", attack, vp, punct, dam);
		                } else {
		    sprintf(buf1, "$n's %s %s{x $N%c [{R%d{x]", attack, vp, punct, dam);
		    sprintf(buf2, "Your %s %s{x $N%c [{R%d{x]", attack, vp, punct, dam);
		    sprintf(buf3, "$n's %s %s{x you%c [{R%d{x]", attack, vp, punct, dam);
		                       }
	              }
                 }
	}
    }

    if (ch == victim) {
	act(buf1, ch, NULL, NULL, TO_ROOM);
	act(buf2, ch, NULL, NULL, TO_CHAR);
    } else {
	act(buf1, ch, NULL, victim, TO_NOTVICT);
	act(buf2, ch, NULL, victim, TO_CHAR);
	act(buf3, ch, NULL, victim, TO_VICT);
    }

    return;
}

/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm(CHAR_DATA * ch, CHAR_DATA * victim)
{
    OBJ_DATA *obj;

    if ((obj = get_eq_char(victim, WEAR_WIELD)) == NULL)
	return;

    if (IS_OBJ_STAT(obj, ITEM_NOREMOVE)) {
	act("$S weapon won't budge!", ch, NULL, victim, TO_CHAR);
	act("$n tries to disarm you, but your weapon won't budge!",
	    ch, NULL, victim, TO_VICT);
	act("$n tries to disarm $N, but fails.", ch, NULL, victim, TO_NOTVICT);
	return;
    }
    act("{Y$n DISARMS you and sends your weapon flying!{x",
	ch, NULL, victim, TO_VICT);
    act("{YYou disarm $N!{x", ch, NULL, victim, TO_CHAR);
    act("{Y$n disarms $N!{x", ch, NULL, victim, TO_NOTVICT);

    obj_from_char(obj);
    if (IS_OBJ_STAT(obj, ITEM_NODROP) || IS_OBJ_STAT(obj, ITEM_INVENTORY))
	obj_to_char(obj, victim);
    else {
	obj_to_room(obj, victim->in_room);
	if (IS_NPC(victim) && victim->wait == 0 && can_see_obj(victim, obj))
	    get_obj(victim, obj, NULL);
    }

    return;
}



