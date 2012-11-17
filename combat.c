#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "signal.h"
#include "magic.h"

/* command procedures needed */
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
DECLARE_DO_FUN(do_geomancy);
DECLARE_DO_FUN(do_disarm);
DECLARE_DO_FUN(do_get);
DECLARE_DO_FUN(do_recall);
DECLARE_DO_FUN(do_yell);
DECLARE_DO_FUN(do_sacrifice);
DECLARE_DO_FUN(do_stun);
DECLARE_DO_FUN(do_trap);
DECLARE_DO_FUN(do_drag);
DECLARE_DO_FUN(do_step);
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_jump);
DECLARE_DO_FUN(do_weaponbreak);
DECLARE_DO_FUN(do_tear);

void do_berserk(CHAR_DATA * ch, char *argument)
{
    int chance, hp_percent;
    int purify_num;

    purify_num = skill_lookup("purify");

    if ((chance = get_skill(ch, gsn_berserk)) == 0
	|| (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_BERSERK))
	|| (!IS_NPC(ch)
	    && ch->level < skill_table[gsn_berserk].skill_level[ch->class])) {
	send_to_char("You turn red in the face, but nothing happens.\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_BERSERK) || is_affected(ch, gsn_berserk)
	|| is_affected(ch, skill_lookup("frenzy"))) {
	send_to_char("You get a little madder.\n\r", ch);
	return;
    }

    /* modifiers */

    /* fighting */
    if (ch->position == POS_FIGHTING)
	chance += 10;

    /* damage -- below 50% of hp helps, above hurts */
    hp_percent = 100 * ch->hit / ch->max_hit;
    chance += 25 - hp_percent / 2;

    if (number_percent() < chance) {
	AFFECT_DATA af;

if (!IS_IMMORTAL(ch))
{	WAIT_STATE(ch, PULSE_VIOLENCE);
	ch->move = (ch->move * 3) / 4;
}

	/* heal a little damage */
	ch->hit += ch->level * 4;
	ch->hit = UMIN(ch->hit, ch->max_hit);

	send_to_char("Your pulse races as you are consumed by rage!\n\r", ch);
	act("$n gets a wild look in $s eyes.", ch, NULL, NULL, TO_ROOM);
	check_improve(ch, gsn_berserk, TRUE, 2);

	spell_purify(purify_num, ch->level, ch, ch, TARGET_CHAR);
	spell_purify(purify_num, ch->level, ch, ch, TARGET_CHAR);


	af.where = TO_AFFECTS;
	af.type = gsn_berserk;
	af.level = ch->level;
	af.duration = number_fuzzy(ch->level / 15);
	af.modifier = UMAX(1, (ch->level * 3) / 4);
	af.bitvector = AFF_BERSERK;

	af.location = APPLY_HITROLL;
	affect_to_char(ch, &af);

	af.location = APPLY_DAMROLL;
	affect_to_char(ch, &af);

//	af.modifier = UMAX(20, 5 * (ch->level / 5));
//	af.location = APPLY_AC;
//	affect_to_char(ch, &af);
    } else {
	WAIT_STATE(ch, PULSE_VIOLENCE);

	send_to_char("Your pulse speeds up, but nothing happens.\n\r", ch);
	check_improve(ch, gsn_berserk, FALSE, 2);
    }
}

void do_magic_song(CHAR_DATA * ch, char *argument)
{
	int chance = 0;
    if ((get_skill(ch, gsn_magic_song)) == 0
	|| (!IS_NPC(ch) && ch->level < skill_table[gsn_magic_song].skill_level[ch->class])) {
	send_to_char("You have a voice like a squashed frog.\n\r", ch);
	return;
    }
    
    if (is_affected(ch,gsn_magic_song) 
    || is_affected(ch, gsn_life_song)
    || is_affected(ch, gsn_attack_song)
    || is_affected(ch, gsn_defense_song)
    || is_affected(ch, gsn_learning_song)
    || is_affected(ch, gsn_speed_song))
{
	send_to_char("You can't sing two songs at once!\n\r",ch);
	return;
}
	
	if (ch->mana == 0)	//Fix for crashbug -Amos
		ch->mana = 2;

	chance = (get_skill(ch,gsn_magic_song) / 2) * (ch->max_mana / ch->mana+1);
    if (ch->position == POS_FIGHTING)
	chance -= 10;
	
	if (ch->position == POS_STANDING)
	chance += 10;

    if (number_percent() < chance) {
	AFFECT_DATA af;

if (!IS_IMMORTAL(ch))
{	WAIT_STATE(ch, PULSE_VIOLENCE);
}


	send_to_char("You begin to sing a song of magic!\n\r", ch);
	act("$n clears $s throat, and begins to sing.", ch, NULL, NULL, TO_ROOM);
	check_improve(ch, gsn_magic_song, TRUE, 2);

	af.where = TO_AFFECTS;
	af.type = gsn_magic_song;
	af.level = ch->level;
	af.duration = number_range(0,1);
	af.modifier = 2;
	af.bitvector = 0;
	af.location = 0;
	affect_to_char(ch, &af);
    } 
    else {
	WAIT_STATE(ch, PULSE_VIOLENCE);

	send_to_char("You have stage fright!\n\r", ch);
	check_improve(ch, gsn_magic_song, FALSE, 2);
    }
}

void do_life_song(CHAR_DATA * ch, char *argument)
{
	int chance = 0;
    if ((get_skill(ch, gsn_life_song)) == 0
	|| (!IS_NPC(ch) && ch->level < skill_table[gsn_life_song].skill_level[ch->class])) {
	send_to_char("You have a voice like a squashed frog.\n\r", ch);
	return;
    }
    
      if (is_affected(ch,gsn_magic_song) 
    || is_affected(ch, gsn_life_song)
    || is_affected(ch, gsn_attack_song)
    || is_affected(ch, gsn_defense_song)
    || is_affected(ch, gsn_learning_song)
    || is_affected(ch, gsn_speed_song))
{
	send_to_char("You can't sing two songs at once!\n\r",ch);
	return;
}
	
	chance = (get_skill(ch,gsn_life_song) / 2) * (ch->max_hit / ch->hit);
    if (ch->position == POS_FIGHTING)
	chance -= 10;
	
	if (ch->position == POS_STANDING)
	chance += 10;

    if (number_percent() < chance) {
	AFFECT_DATA af;

if (!IS_IMMORTAL(ch))
{	WAIT_STATE(ch, PULSE_VIOLENCE);
}


	send_to_char("You begin to sing a song of life!\n\r", ch);
	act("$n clears $s throat, and begins to sing.", ch, NULL, NULL, TO_ROOM);
	check_improve(ch, gsn_life_song, TRUE, 2);

	af.where = TO_AFFECTS;
	af.type = gsn_life_song;
	af.level = ch->level;
	af.duration = number_range(0,1);
	af.modifier = 2;
	af.bitvector = 0;
	af.location = 0;
	affect_to_char(ch, &af);
    } 
    else {
	WAIT_STATE(ch, PULSE_VIOLENCE);

	send_to_char("You have stage fright!\n\r", ch);
	check_improve(ch, gsn_life_song, FALSE, 2);
    }
}

void do_attack_song(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *vch;
    CHAR_DATA *vch_next;   
    AFFECT_DATA af;
   
	int chance = 0;
    if ((get_skill(ch, gsn_attack_song)) == 0
	|| (!IS_NPC(ch) && ch->level < skill_table[gsn_attack_song].skill_level[ch->class])) {
	send_to_char("You have a voice like a squashed frog.\n\r", ch);
	return;
    }
    
      if (is_affected(ch,gsn_magic_song) 
    || is_affected(ch, gsn_life_song)
    || is_affected(ch, gsn_attack_song)
    || is_affected(ch, gsn_defense_song)
    || is_affected(ch, gsn_learning_song)
    || is_affected(ch, gsn_speed_song))
{
	send_to_char("You can't sing two songs at once!\n\r",ch);
	return;
}
	
	chance = (get_skill(ch,gsn_attack_song)*3 / 2) * (ch->max_move / ch->move);
    if (ch->position == POS_FIGHTING)
	chance -= 10;
	
	if (ch->position == POS_STANDING)
	chance += 10;

    if (number_percent() < chance) {

	if (!IS_IMMORTAL(ch))
		WAIT_STATE(ch, PULSE_VIOLENCE);
		
		send_to_char("You begin to sing a song of attack!\n\r", ch);
	act("$n clears $s throat, and begins to sing.", ch, NULL, NULL, TO_ROOM);
	check_improve(ch, gsn_attack_song, TRUE, 2);

    	for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
		vch_next = vch->next_in_room;

		if (IS_NPC(vch))
		continue;
		
		if (vch->side != ch->side)
		continue;
	    
	    af.where = TO_AFFECTS;
		af.type = gsn_attack_song;
		af.level = ch->level;
		af.duration = 0;
		af.location = 0;
		af.modifier = -1;
		af.bitvector = 0;
		affect_to_char(vch, &af);
    }
}
    else {
	WAIT_STATE(ch, PULSE_VIOLENCE);

	send_to_char("You have stage fright!\n\r", ch);
	check_improve(ch, gsn_attack_song, FALSE, 2);
    }
}

void do_speed_song(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *vch;
    CHAR_DATA *vch_next;   
    AFFECT_DATA af;
   
	int chance = 0;
    if ((get_skill(ch, gsn_speed_song)) == 0
	|| (!IS_NPC(ch) && ch->level < skill_table[gsn_speed_song].skill_level[ch->class])) {
	send_to_char("You have a voice like a squashed frog.\n\r", ch);
	return;
    }
    
      if (is_affected(ch,gsn_magic_song) 
    || is_affected(ch, gsn_life_song)
    || is_affected(ch, gsn_attack_song)
    || is_affected(ch, gsn_defense_song)
    || is_affected(ch, gsn_learning_song)
    || is_affected(ch, gsn_speed_song))
{
	send_to_char("You can't sing two songs at once!\n\r",ch);
	return;
}
	
	chance = (get_skill(ch,gsn_speed_song)*3 / 5) * (ch->max_move / ch->move);
    if (ch->position == POS_FIGHTING)
	chance -= 10;
	
	if (ch->position == POS_STANDING)
	chance += 10;

    if (number_percent() < chance) {
	    
	    send_to_char("You begin to sing a song of speed!\n\r", ch);
	act("$n clears $s throat, and begins to sing.", ch, NULL, NULL, TO_ROOM);
	check_improve(ch, gsn_speed_song, TRUE, 2);

		if (!IS_IMMORTAL(ch))
			WAIT_STATE(ch, PULSE_VIOLENCE);
		

    	for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
		vch_next = vch->next_in_room;

		if (IS_NPC(vch))
		continue;
		
		if (vch->side != ch->side)
		continue;
	    
	    af.where = TO_AFFECTS;
		af.type = gsn_speed_song;
		af.level = ch->level;
		af.duration = 0;
		af.location = 0;
		af.modifier = -1;
		af.bitvector = 0;
		affect_to_char(vch, &af);
    }
}
    else {
	WAIT_STATE(ch, PULSE_VIOLENCE);

	send_to_char("You have stage fright!\n\r", ch);
	check_improve(ch, gsn_speed_song, FALSE, 2);
    }
}

void do_learning_song(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *vch;
    CHAR_DATA *vch_next;   
    AFFECT_DATA af;
   
	int chance = 0;
    if ((get_skill(ch, gsn_learning_song)) == 0
	|| (!IS_NPC(ch) && ch->level < skill_table[gsn_learning_song].skill_level[ch->class])) {
	send_to_char("You have a voice like a squashed frog.\n\r", ch);
	return;
    }
    
      if (is_affected(ch,gsn_magic_song) 
    || is_affected(ch, gsn_life_song)
    || is_affected(ch, gsn_attack_song)
    || is_affected(ch, gsn_defense_song)
    || is_affected(ch, gsn_learning_song)
    || is_affected(ch, gsn_speed_song))
{
	send_to_char("You can't sing two songs at once!\n\r",ch);
	return;
}
	
	chance = (get_skill(ch,gsn_learning_song)*7 / 8);
    if (ch->position == POS_FIGHTING)
	chance -= 10;
	
	if (ch->position == POS_STANDING)
	chance += 10;

    if (number_percent() < chance) {
	    
	    send_to_char("You begin to sing a song of learning!\n\r", ch);
	act("$n clears $s throat, and begins to sing.", ch, NULL, NULL, TO_ROOM);
	check_improve(ch, gsn_learning_song, TRUE, 2);

		if (!IS_IMMORTAL(ch))
			WAIT_STATE(ch, PULSE_VIOLENCE);
		

    	for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
		vch_next = vch->next_in_room;

		if (IS_NPC(vch))
		continue;
		
		if (vch->side != ch->side)
		continue;
	    
	    af.where = TO_AFFECTS;
		af.type = gsn_learning_song;
		af.level = ch->level;
		af.duration = 0;
		af.location = 0;
		af.modifier = -1;
		af.bitvector = 0;
		affect_to_char(vch, &af);
    }
}
    else {
	WAIT_STATE(ch, PULSE_VIOLENCE);

	send_to_char("You have stage fright!\n\r", ch);
	check_improve(ch, gsn_learning_song, FALSE, 2);
    }
}

void do_defense_song(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *vch;
    CHAR_DATA *vch_next;   
    AFFECT_DATA af;
   
	int chance = 0;
    if ((get_skill(ch, gsn_defense_song)) == 0
	|| (!IS_NPC(ch) && ch->level < skill_table[gsn_defense_song].skill_level[ch->class])) {
	send_to_char("You have a voice like a squashed frog.\n\r", ch);
	return;
    }
    
      if (is_affected(ch,gsn_magic_song) 
    || is_affected(ch, gsn_life_song)
    || is_affected(ch, gsn_attack_song)
    || is_affected(ch, gsn_defense_song)
    || is_affected(ch, gsn_learning_song)
    || is_affected(ch, gsn_sung_defense)
    || is_affected(ch, gsn_speed_song))
{
	send_to_char("You can't sing two songs at once!\n\r",ch);
	return;
}
	
	chance = (get_skill(ch,gsn_defense_song)*7 / 8);
    if (ch->position == POS_FIGHTING)
	chance -= 10;
	
	if (ch->position == POS_STANDING)
	chance += 10;

    if (number_percent() < chance) {
	    
	    send_to_char("You begin to sing a song of defense!\n\r", ch);
	act("$n clears $s throat, and begins to sing.", ch, NULL, NULL, TO_ROOM);
	check_improve(ch, gsn_defense_song, TRUE, 2);

		if (!IS_IMMORTAL(ch))
			WAIT_STATE(ch, PULSE_VIOLENCE);
		

    	for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
		vch_next = vch->next_in_room;
		
		if (IS_NPC(vch))
		continue;
		
		if (vch->side != ch->side)
		continue;
	    
	    af.where = TO_AFFECTS;
		af.type = gsn_defense_song;
		af.level = ch->level;
		af.duration = 0;
		af.location = 0;
		af.modifier = -1;
		af.bitvector = 0;
		affect_to_char(vch, &af);
    }
}
    else {
	WAIT_STATE(ch, PULSE_VIOLENCE);

	send_to_char("You have stage fright!\n\r", ch);
	check_improve(ch, gsn_defense_song, FALSE, 2);
    }
}


void do_charge(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *mount;
    OBJ_DATA *vobj;
    OBJ_DATA *obj;
	//OBJ_DATA *vobj;
    //CHAR_DATA *mount;
    int chance;

    one_argument(argument, arg);

    if ((chance = get_skill(ch, gsn_charge)) == 0
	|| (!IS_NPC(ch)
	    && ch->level < skill_table[gsn_charge].skill_level[ch->class])) {
	send_to_char("Charge? What's that?\n\r", ch);
	return;
    }
    if (!MOUNTED(ch)) {
	send_to_char("You must be riding something to charge!\n\r", ch);
	return;
    }
    if (arg[0] == '\0') {
	send_to_char("Charge whom?\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (victim->mount != NULL)
	if (victim->mount == ch) {
	    send_to_char("Charge your own mount?\n\r", ch);
	    return;
	}
    if (ch->fighting != NULL) {
	send_to_char("You are to busy fighting to gain momentum.\n\r", ch);
	return;
    }
    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {
	send_to_char("You need to wield a weapon to make charge effective.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("How can you charge on yourself?\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	act("But $N is your friend!", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (victim->fighting != NULL) {
	send_to_char("Your target will not hold still!\n\r", ch);
	return;
    }

    vobj = get_eq_char(victim, WEAR_WIELD);
    if (get_skill(victim, gsn_ground) > 0
    && number_percent() < get_skill(victim, gsn_ground)
    && vobj->value[0] == WEAPON_POLEARM )
      {
       check_improve(victim, gsn_ground, TRUE, 1);
       DAZE_STATE(ch, 3 * PULSE_VIOLENCE);
       WAIT_STATE(victim, skill_table[gsn_ground].beats);
       mount = ch->mount;
       ch->mount = NULL;
       ch->position = POS_RESTING;
       ch->riding = FALSE;
       mount->riding = FALSE;
       act("$n's grounded weapon knocks your from your mount and guts you!", victim, NULL, ch, TO_VICT);
       act("You ground your weapon!", victim, NULL, ch, TO_CHAR);
       act("$n catches $N with a grounded polearm!", victim, NULL, ch, TO_NOTVICT);
       one_hit(victim, ch, gsn_ground, FALSE);
       check_killer(ch, victim);
       return;
      }

    if (number_percent() < chance) {
	check_improve(ch, gsn_charge, TRUE, 1);
	DAZE_STATE(victim, 3 * PULSE_VIOLENCE);
	if (!IS_IMMORTAL(ch))
	{	WAIT_STATE(ch, skill_table[gsn_charge].beats); }
	victim->position = POS_RESTING;
	one_hit(ch, victim, gsn_charge, FALSE);
    } else {
	damage(ch, victim, 0, gsn_charge, DAM_BASH, FALSE);
	check_improve(ch, gsn_charge, FALSE, 1);
	if (!IS_IMMORTAL(ch))
	{
	WAIT_STATE(ch, skill_table[gsn_charge].beats * 3 / 2); }
    }
    check_killer(ch, victim);
}

void do_bash(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument, arg);

    if ((chance = get_skill(ch, gsn_bash)) == 0
	|| (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_BASH))
	|| (!IS_NPC(ch)
	    && ch->level < skill_table[gsn_bash].skill_level[ch->class])) {
	send_to_char("Bashing? What's that?\n\r", ch);
	return;
    }
    if (arg[0] == '\0') {
	victim = ch->fighting;
	if (victim == NULL) {
	    send_to_char("But you aren't fighting anyone!\n\r", ch);
	    return;
	}
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (MOUNTED(ch)) {
	send_to_char("You can't bash while riding!\n\r", ch);
	return;
    }
    if (MOUNTED(victim)) {
	send_to_char("They are riding something you can't bash them.\n\r", ch);
	return;
    }
    if (victim->position < POS_FIGHTING) {
	act("You'll have to let $M get back up first.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (victim == ch) {
	send_to_char("You try to bash your brains out, but fail.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	act("But $N is your friend!", ch, NULL, victim, TO_CHAR);
	return;
    }
    /* modifiers */

    /* size  and weight */
    chance += ch->carry_weight / 250;
    chance -= victim->carry_weight / 200;

    if (ch->size < victim->size)
	chance += (ch->size - victim->size) * 15;
    else
	chance += (ch->size - victim->size) * 10;

    /* stats */
    chance += get_curr_stat(ch, STAT_STR);
    chance -= (get_curr_stat(victim, STAT_DEX) * 4) / 3;
    chance -= GET_AC(victim, AC_BASH) / 10;
    /* speed */
    if (IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags, OFF_FAST) || IS_AFFECTED(victim, AFF_HASTE))
	chance -= 30;

    /* level */
    chance += (ch->level - victim->level);

    if (!IS_NPC(victim)
	&& chance < get_skill(victim, gsn_dodge)) {	/*
	   act("$n tries to bash you, but you dodge it.",ch,NULL,victim,TO_VICT);
	   act("$N dodges your bash, you fall flat on your face.",ch,NULL,victim,TO_CHAR);
							   WAIT_STATE(ch,skill_table[gsn_bash].beats);
							   return; */
	chance -= 3 * (get_skill(victim, gsn_dodge) - chance);
    }
    /* now the attack */
    if (number_percent() < chance) {

	act("$n sends you sprawling with a powerful bash!",
	    ch, NULL, victim, TO_VICT);
	act("You slam into $N, and send $M flying!", ch, NULL, victim, TO_CHAR);
	act("$n sends $N sprawling with a powerful bash.",
	    ch, NULL, victim, TO_NOTVICT);
	check_improve(ch, gsn_bash, TRUE, 1);
	
	DAZE_STATE(victim, 1.5 * PULSE_VIOLENCE);
	if (!IS_IMMORTAL(ch))
	{
      
	WAIT_STATE(ch, skill_table[gsn_bash].beats);
	}
/* Stupid idea. --Tasydra
	ch->position = POS_RESTING;
*/
	damage(ch, victim, number_range(2, 2 + 2 * ch->size + chance / 20), gsn_bash,
	       DAM_BASH, FALSE);

	if (RIDDEN(victim)) {
	    mount_success(RIDDEN(victim), victim, FALSE);
	}
    } else {
	damage(ch, victim, 0, gsn_bash, DAM_BASH, FALSE);
	act("You fall flat on your face!",
	    ch, NULL, victim, TO_CHAR);
	act("$n falls flat on $s face.",
	    ch, NULL, victim, TO_NOTVICT);
	act("You evade $n's bash, causing $m to fall flat on $s face.",
	    ch, NULL, victim, TO_VICT);
	check_improve(ch, gsn_bash, FALSE, 1);
/* Stupid idea. --Tasydra
	ch->position = POS_RESTING;
*/
if (!IS_IMMORTAL(ch))
{
	WAIT_STATE(ch, skill_table[gsn_bash].beats * 3 / 2);
}
    }
    check_killer(ch, victim);
}

void do_dirt(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument, arg);

    if ( !IS_NPC(ch) && (get_skill(ch, gsn_dirt) == 0 || ch->level < skill_table[gsn_dirt].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    if ((chance = get_skill(ch, gsn_dirt)) == 0
	|| (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_KICK_DIRT))
	|| (!IS_NPC(ch)
	    && ch->level < skill_table[gsn_dirt].skill_level[ch->class])) {
	send_to_char("You get your feet dirty.\n\r", ch);
	return;
    }

    if (arg[0] == '\0') {
	victim = ch->fighting;
	if (victim == NULL) {
	    send_to_char("But you aren't in combat!\n\r", ch);
	    return;
	}
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }

    if (victim == ch) {
	send_to_char("You can't do that to yourself!\n\r", ch);
	return;
    }

    if (MOUNTED(ch)) {
	send_to_char("You can't dirt while riding!\n\r", ch);
	return;
    }

    if (IS_AFFECTED(victim, AFF_BLIND)) {
	act("$E's already been blinded.", ch, NULL, victim, TO_CHAR);
	return;
    }

    if (victim == ch) {
	send_to_char("Very funny.\n\r", ch);
	return;
    }

    if (is_safe(ch, victim))
	return;

    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	act("But $N is such a good friend!", ch, NULL, victim, TO_CHAR);
	return;
    }
    /* modifiers */

    /* dexterity */
    chance += get_curr_stat(ch, STAT_DEX);
    chance -= 2 * get_curr_stat(victim, STAT_DEX);

    /* speed  */
    if (IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags, OFF_FAST) || IS_AFFECTED(victim, AFF_HASTE))
	chance -= 25;

    /* level */
    chance += (ch->level - victim->level) * 2;

    /* sloppy hack to prevent false zeroes */
    if (chance % 5 == 0)
	chance += 1;
	
	// Izlude - Lowering dirt effectiveness.
	
    chance -= 33;

    /* terrain */

    switch (ch->in_room->sector_type) {
    case (SECT_INSIDE):
	chance = 0;
	break;
    case (SECT_CITY):
	chance = 0;
	break;
    case (SECT_FIELD):
	chance += 5;
	break;
    case (SECT_FOREST):
	break;
    case (SECT_HILLS):
	break;
    case (SECT_MOUNTAIN):
	chance -= 10;
	break;
    case (SECT_WATER_SWIM):
	chance = 0;
	break;
    case (SECT_WATER_NOSWIM):
	chance = 0;
	break;
    case (SECT_AIR):
	chance = 0;
	break;
    case (SECT_DESERT):
	chance += 10;
	break;
    default:
	chance += 0;
	break;
    }

    if (chance == 0) {
	send_to_char("There isn't any dirt to kick.\n\r", ch);
	return;
    }
    
    /* now the attack */
    if (number_percent() < chance) {
	AFFECT_DATA af;
	act("$n is blinded by the dirt in $s eyes!", victim, NULL, NULL, TO_ROOM);
	act("$n kicks dirt in your eyes!", ch, NULL, victim, TO_VICT);
	damage(ch, victim, number_range(2, 10), gsn_dirt, DAM_NONE, FALSE);
	send_to_char("You can't see a thing!\n\r", victim);
	check_improve(ch, gsn_dirt, TRUE, 2);
	if (!IS_IMMORTAL(ch))
		{	WAIT_STATE(ch, skill_table[gsn_dirt].beats); }

	af.where = TO_AFFECTS;
	af.type = gsn_dirt;
	af.level = ch->level;
	af.duration = 0;
	af.location = APPLY_HITROLL;
	af.modifier = -4;
	af.bitvector = AFF_BLIND;

	affect_to_char(victim, &af);
    }
	else
	{
		damage(ch, victim, 0, gsn_dirt, DAM_NONE, TRUE);
		check_improve(ch, gsn_dirt, FALSE, 2);
		if (!IS_IMMORTAL(ch))
		{ WAIT_STATE(ch, skill_table[gsn_dirt].beats); }
    }
    check_killer(ch, victim);
}


void do_focus(CHAR_DATA * ch, char *argument)
{
	int chance = 0;

    if ((chance = get_skill(ch, gsn_focus)) == 0 || ch->level < skill_table[gsn_focus].skill_level[ch->class])
	{
		send_to_char("Huh?\n\r", ch);
		return;
    }
if (!IS_IMMORTAL(ch))
{ WAIT_STATE(ch, PULSE_VIOLENCE); }


    if (number_range(1, 100) < chance)
	{
		send_to_char("You focus.\n\r", ch);
		ch->mana += 35;
	    check_improve(ch, gsn_focus, TRUE, 2);

		switch(number_range(1,3))
		{
		case 1:
			act("$n hums quietly.", ch, NULL, NULL, TO_ROOM);
			break;
		case 2:
			act("$n rocks back and forth.", ch, NULL, NULL, TO_ROOM);
			break;
		case 3:
			act("$n chants.", ch, NULL, NULL, TO_ROOM);
			break;
		}
    }
	else
	{
		send_to_char("You fail.\n\r", ch);
	    check_improve(ch, gsn_focus, FALSE, 2);
	}

    return;


}
void do_disrupt(CHAR_DATA * ch, char *argument)
{
    AFFECT_DATA *paf;
    int chance = 0;

    if ((chance = get_skill(ch, gsn_disrupt)) == 0 || ch->level < skill_table[gsn_disrupt].skill_level[ch->class])
	{
		send_to_char("Huh?\n\r", ch);
		return;
    }
	paf = ch->in_room->affected;

    if (paf == NULL)
	{
		send_to_char("The room is not silenced!\n\r", ch);
		return;
    }
    while (paf->bitvector != AFF_SILENT_ROOM && paf->next != NULL)
	paf = paf->next;

    if (paf == NULL)
	{
		send_to_char("The room is not silenced!\n\r", ch);
		return;
    }

if (!IS_IMMORTAL(ch))
{ WAIT_STATE(ch, PULSE_VIOLENCE); }

    check_improve(ch, gsn_disrupt, FALSE, 2);

//chance??        skill / 2    min 5

    if (number_range(1, 100) < chance)
	{
		/*
		affect_remove(ch, paf);
		*/
		send_to_char("You break the silence!\n\r", ch);
		act("$n breaks the air with his voice!", ch, NULL, NULL, TO_ROOM);
		affect_remove_room(ch->in_room, paf);

    }

    return;

}

void do_lore(CHAR_DATA * ch, char *argument)
{

    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    int roll;

    if ((get_skill(ch, gsn_lore) == 0
	|| ch->level < skill_table[gsn_lore].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    if (argument[0] == '\0')
	{
		send_to_char("Lore what?\n\r", ch);
		return;
    }
    argument = one_argument(argument, arg);
    obj = get_obj_carry(ch, arg, ch);

    if (obj == NULL)
	{
		send_to_char("You don't seem to have this item.\n\r", ch);
		return;
    }

	check_improve(ch, gsn_lore, TRUE, 4);

	roll = number_percent();

    if (roll > get_skill(ch, gsn_lore)) {
	send_to_char("You fail.\n\r", ch);
	return;
    }



	sprintf(buf, "Object '%s' is type %s, extra flags %s.\n\rWeight is %d, value is %d, level is %d.\n\r",

	    obj->name,
	    item_name(obj->item_type),
	    extra_bit_name(obj->extra_flags),
	    obj->weight / 10,
	    obj->cost,
	    obj->level
	);
    send_to_char(buf, ch);

    switch (obj->item_type) {
    case ITEM_SCROLL:
    case ITEM_POTION:
    case ITEM_PILL:
	sprintf(buf, "Level %d spells of:", obj->value[0]);
	send_to_char(buf, ch);

	if (obj->value[1] >= 0 && obj->value[1] < MAX_SKILL) {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[1]].name, ch);
	    send_to_char("'", ch);
	}
	if (obj->value[2] >= 0 && obj->value[2] < MAX_SKILL) {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[2]].name, ch);
	    send_to_char("'", ch);
	}
	if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL) {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[3]].name, ch);
	    send_to_char("'", ch);
	}
	if (obj->value[4] >= 0 && obj->value[4] < MAX_SKILL) {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[4]].name, ch);
	    send_to_char("'", ch);
	}
	send_to_char(".\n\r", ch);
	break;

    case ITEM_WAND:
    case ITEM_STAFF:
	sprintf(buf, "Has %d charges of level %d",
		obj->value[2], obj->value[0]);
	send_to_char(buf, ch);

	if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL) {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[3]].name, ch);
	    send_to_char("'", ch);
	}
	send_to_char(".\n\r", ch);
	break;

    case ITEM_DRINK_CON:
	sprintf(buf, "It holds %s-colored %s.\n\r",
		liq_table[obj->value[2]].liq_color,
		liq_table[obj->value[2]].liq_name);
	send_to_char(buf, ch);
	break;

    case ITEM_CONTAINER:
	sprintf(buf, "Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
		obj->value[0], obj->value[3], cont_bit_name(obj->value[1]));
	send_to_char(buf, ch);
	if (obj->value[4] != 100) {
	    sprintf(buf, "Weight multiplier: %d%%\n\r",
		    obj->value[4]);
	    send_to_char(buf, ch);
	}
	break;

	case ITEM_SHIELD:
	
	send_to_char("Is a shield.\n\r",ch);
	
	break;
	
    case ITEM_WEAPON:
	send_to_char("Weapon type is ", ch);
	switch (obj->value[0]) {
	case (WEAPON_EXOTIC):
	    send_to_char("exotic.\n\r", ch);
	    break;
	case (WEAPON_SWORD):
	    send_to_char("sword.\n\r", ch);
	    break;
	case (WEAPON_DAGGER):
	    send_to_char("dagger.\n\r", ch);
	    break;
	case (WEAPON_SPEAR):
	    send_to_char("spear/staff.\n\r", ch);
	    break;
	case (WEAPON_MACE):
	    send_to_char("mace/club.\n\r", ch);
	    break;
	case (WEAPON_AXE):
	    send_to_char("axe.\n\r", ch);
	    break;
	case (WEAPON_FLAIL):
	    send_to_char("flail.\n\r", ch);
	    break;
	case (WEAPON_WHIP):
	    send_to_char("whip.\n\r", ch);
	    break;
	case (WEAPON_POLEARM):
	    send_to_char("polearm.\n\r", ch);
	    break;
	default:
	    send_to_char("unknown.\n\r", ch);
	    break;
	}
	if (obj->pIndexData->new_format)
	{
	    sprintf(buf, "Damage is %dd%d (average %d).\n\r",
		    obj->value[1], obj->value[2],
		    (1 + obj->value[2]) * obj->value[1] / 2);}
	else{
	    sprintf(buf, "Damage is %d to %d (average %d).\n\r",
		    obj->value[1], obj->value[2],
		    (obj->value[1] + obj->value[2]) / 2);}
	
		    send_to_char(buf, ch);
		if (obj->value[4]) {	/* weapon flags */
	    sprintf(buf, "Weapons flags: %s\n\r", weapon_bit_name(obj->value[4]));
	    send_to_char(buf, ch);
	}
	break;

case ITEM_ARMOR:
	sprintf(buf,
		"Armor class is %d pierce, %d bash, %d slash, and %d vs. magic.\n\r",
		obj->value[0], obj->value[1], obj->value[2], obj->value[3]);
	send_to_char(buf, ch);
	break;
case ITEM_INGREDIENT:
case ITEM_PARCHMENT:
	sprintf(buf,
		"Level of %d with %d uses left.\n\r",
		obj->value[0], obj->value[1]);
	send_to_char(buf, ch);
	break;
    }

    if (!obj->enchanted)
	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) {
	    if (paf->location != APPLY_NONE && paf->modifier != 0) {
		sprintf(buf, "Affects %s by %d.\n\r",
			affect_loc_name(paf->location), paf->modifier);
		send_to_char(buf, ch);
		if (paf->bitvector) {
		    switch (paf->where) {
		    case TO_AFFECTS:
			sprintf(buf, "Adds %s affect.\n",
				affect_bit_name(paf->bitvector));
			break;
		    case TO_OBJECT:
			sprintf(buf, "Adds %s object flag.\n",
				extra_bit_name(paf->bitvector));
			break;
		    case TO_IMMUNE:
			sprintf(buf, "Adds immunity to %s.\n",
				imm_bit_name(paf->bitvector));
			break;
		    case TO_RESIST:
			sprintf(buf, "Adds resistance to %s.\n\r",
				imm_bit_name(paf->bitvector));
			break;
		    case TO_VULN:
			sprintf(buf, "Adds vulnerability to %s.\n\r",
				imm_bit_name(paf->bitvector));
			break;
		    default:
			sprintf(buf, "Unknown bit %d: %d\n\r",
				paf->where, paf->bitvector);
			break;
		    }
		    send_to_char(buf, ch);
		}
	    }
	}
    for (paf = obj->affected; paf != NULL; paf = paf->next) {
	if (paf->location != APPLY_NONE && paf->modifier != 0) {
	    sprintf(buf, "Affects %s by %d",
		    affect_loc_name(paf->location), paf->modifier);
	    send_to_char(buf, ch);
	    if (paf->duration > -1)
		sprintf(buf, ", %d hours.\n\r", paf->duration);
	    else
		sprintf(buf, ".\n\r");
	    send_to_char(buf, ch);
	    if (paf->bitvector) {
		switch (paf->where) {
		case TO_AFFECTS:
		    sprintf(buf, "Adds %s affect.\n",
			    affect_bit_name(paf->bitvector));
		    break;
		case TO_OBJECT:
		    sprintf(buf, "Adds %s object flag.\n",
			    extra_bit_name(paf->bitvector));
		    break;
		case TO_WEAPON:
		    sprintf(buf, "Adds %s weapon flags.\n",
			    weapon_bit_name(paf->bitvector));
		    break;
		case TO_IMMUNE:
		    sprintf(buf, "Adds immunity to %s.\n",
			    imm_bit_name(paf->bitvector));
		    break;
		case TO_RESIST:
		    sprintf(buf, "Adds resistance to %s.\n\r",
			    imm_bit_name(paf->bitvector));
		    break;
		case TO_VULN:
		    sprintf(buf, "Adds vulnerability to %s.\n\r",
			    imm_bit_name(paf->bitvector));
		    break;
		default:
		    sprintf(buf, "Unknown bit %d: %d\n\r",
			    paf->where, paf->bitvector);
		    break;
		}
		send_to_char(buf, ch);
	    }
	}
    }




    return;
}
void do_struggle(CHAR_DATA * ch, char *argument)
{
    AFFECT_DATA *paf;
    int chance = 0;
    int roller = 0;
    paf = ch->affected;

    if (paf == NULL) {
	send_to_char("You are not stuck!\n\r", ch);
	return;
    }
    while (paf->bitvector != AFF_WEB && paf->next != NULL)
	paf = paf->next;

    if (paf->bitvector != AFF_WEB) {
	send_to_char("You are not stuck!\n\r", ch);
	return;
    }
    check_improve(ch, gsn_struggle, FALSE, 2);

    chance += get_skill(ch, gsn_struggle);
    chance += get_curr_stat(ch, STAT_DEX) / 5;
    chance += get_curr_stat(ch, STAT_STR) / 5;
    chance += 10;

//Determine Outcome

     roller = number_range(1,100);

     if(get_curr_stat(ch, STAT_STR) > get_curr_stat(ch, STAT_INT))
     {
     if (roller < (chance - 85))
        {
	affect_remove(ch, paf);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 2 * PULSE_VIOLENCE); }
	send_to_char("You break free!\n\r", ch);
	act("$n struggles and breaks free!", ch, NULL, NULL, TO_ROOM);
        }
     else if(roller < (chance - 50))
        {
        if (paf->duration > 0)
           {
	   paf->duration--;
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 1 * PULSE_VIOLENCE); }
	   send_to_char("You struggle!\n\r", ch);
	   act("$n struggles!", ch, NULL, NULL, TO_ROOM);
	   }
	else
	   {
	   affect_remove(ch, paf);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 1 * PULSE_VIOLENCE); }
	   send_to_char("You break free!\n\r", ch);
	   act("$n struggles and breaks free!", ch, NULL, NULL, TO_ROOM);
	   }
	}
     else if(roller < chance)
        {
        if (paf->duration > 0)
           {
     	   paf->duration--;
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 2 * PULSE_VIOLENCE); }
     	   send_to_char("You struggle!\n\r", ch);
     	   act("$n struggles!", ch, NULL, NULL, TO_ROOM);
     	   }
     	else
     	   {
     	   affect_remove(ch, paf);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 2 * PULSE_VIOLENCE); }
     	   send_to_char("You break free!\n\r", ch);
     	   act("$n struggles and breaks free!", ch, NULL, NULL, TO_ROOM);
     	   }
	}
     else if(roller >= (chance))
        {
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 1 * PULSE_VIOLENCE); }
	   send_to_char("You struggle in vain!\n\r", ch);
	   act("$n struggles!", ch, NULL, NULL, TO_ROOM);
	}
    }
    else
    {
    if (roller < (chance - 95))
        {
        affect_remove(ch, paf);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 2 * PULSE_VIOLENCE); }
    	send_to_char("You break free!\n\r", ch);
    	act("$n struggles and breaks free!", ch, NULL, NULL, TO_ROOM);
        }
    else if(roller < (chance - 60))
        {
        if (paf->duration > 0)
           {
    	   paf->duration--;
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 1 * PULSE_VIOLENCE); }
    	   send_to_char("You struggle!\n\r", ch);
    	   act("$n struggles!", ch, NULL, NULL, TO_ROOM);
    	   }
    	else
    	   {
    	   affect_remove(ch, paf);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 1 * PULSE_VIOLENCE); }
    	   send_to_char("You break free!\n\r", ch);
    	   act("$n struggles and breaks free!", ch, NULL, NULL, TO_ROOM);
    	   }
    	}
         else if(roller < chance)
            {
            if (paf->duration > 0)
               {
         	   paf->duration--;
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 2 * PULSE_VIOLENCE); }
         	   send_to_char("You struggle!\n\r", ch);
         	   act("$n struggles!", ch, NULL, NULL, TO_ROOM);
         	   }
         	else
         	   {
         	   affect_remove(ch, paf);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 2 * PULSE_VIOLENCE); }
         	   send_to_char("You break free!\n\r", ch);
         	   act("$n struggles and breaks free!", ch, NULL, NULL, TO_ROOM);
         	   }
    	}
         else if(roller >= (chance))
            {
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 1 * PULSE_VIOLENCE); }
    	   send_to_char("You struggle in vain!\n\r", ch);
    	   act("$n struggles!", ch, NULL, NULL, TO_ROOM);
    	}
    }
    return;
}

void do_mend(CHAR_DATA * ch, char *argument)
{

    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
    int roll;

if ( !IS_NPC(ch) && (get_skill(ch, gsn_mend) == 0
	|| ch->level < skill_table[gsn_mend].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    if (argument[0] == '\0') {
	send_to_char("Mend what?\n\r", ch);
	return;
    }
    argument = one_argument(argument, arg);
    obj = get_obj_carry(ch, arg, ch);

    if (obj == NULL) {
	send_to_char("You don't seem to have this item.\n\r", ch);
	return;
    }
    if ((obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_WEAPON)) {
	send_to_char("You can't mend that.\n\r", ch);
	return;
    }
    check_improve(ch, gsn_mend, TRUE, 4);

	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, PULSE_VIOLENCE/2); }
    roll = number_percent();

    if (roll > get_skill(ch, gsn_mend)) {
	send_to_char("You fail.\n\r", ch);
	return;
    }
    obj->condition += number_range(roll / 14,roll/3);

    if (obj->condition > 100)
	obj->condition = 100;

    send_to_char("You work on it.\n\r", ch);
    act("You mend $p.", NULL, obj, NULL, TO_CHAR);
    act("$n mends $p.", ch, obj, NULL, TO_ROOM);

    return;
}

void do_bluff(CHAR_DATA * ch, char *argument)
{

    CHAR_DATA *mob;
    int chance;
/*
    if (!strcmp(class_table[ch->class].name, "monk"))
    ch->pcdata->learned[gsn_bluff] = 0;
*/

if ( !IS_NPC(ch) && (get_skill(ch, gsn_bluff) == 0
	|| ch->level < skill_table[gsn_bluff].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }


    if ((mob = ch->fighting) == NULL) {
	send_to_char("But you're not fighting!\n\r", ch);
	return;
    }
    chance = get_skill(ch, gsn_bluff);
    chance -= 9 * get_curr_stat(mob, STAT_DEX);
    chance += 9 * get_curr_stat(ch, STAT_DEX);

    chance += (ch->level - mob->level) * 12;

    if (chance > number_percent()) {
	check_improve(ch, gsn_bluff, TRUE, 1);
	act("You fake an attack and step away!", ch, NULL, mob, TO_CHAR);
	act("$n steps back!", ch, NULL, mob, TO_NOTVICT);
	stop_fighting(ch, FALSE);
//      stop_fighting( mob, FALSE );
    } else {
	act("You fail!", ch, NULL, mob, TO_CHAR);
	act("$n attempts to step back!", ch, NULL, mob, TO_VICT);
	check_improve(ch, gsn_bluff, FALSE, 1);
    }

    return;
}

void do_stun(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument, arg);

    if ((chance = get_skill(ch, gsn_stun)) == 0
	|| (!IS_NPC(ch)
	    && ch->level < skill_table[gsn_stun].skill_level[ch->class])) {
	send_to_char("You couldn't stun a fly.\n\r", ch);
	return;
    }
    if (arg[0] == '\0') {
	send_to_char("Stun who?.\n\r", ch);
	return;
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (IS_IMMORTAL(victim)) {
	send_to_char("Sorry, no stunning immortals.\n\r", ch);
	return;
    }
    if (MOUNTED(ch)) {
	send_to_char("You can't stun opponents while riding!\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("Very funny.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	act("But $N is such a good friend!", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (ch->fighting != NULL) {
	send_to_char("You can't stun while fighting.\n\r", ch);
	return;
    }
    if (IS_AFFECTED(victim, AFF_SLEEP)) {
	send_to_char("They're already out cold.\n\r", ch);
	return;
    }

    /* modifiers */

    /* dexterity */
    chance += get_curr_stat(ch, STAT_DEX) / 2;
    chance -= 2 * get_curr_stat(victim, STAT_DEX);

    /* level */
    chance += (ch->level - victim->level) * 2;

    /* sloppy hack to prevent false zeroes */
    if (chance % 5 == 0)
	chance += 1;

    /* now the attack */
    if (number_percent() < chance) {
	AFFECT_DATA af;
	act("$n is knocked out cold!", victim, NULL, NULL, TO_ROOM);
	act("$n knocks you out!", ch, NULL, victim, TO_VICT);
	send_to_char("You fall to the ground, stunned!\n\r", victim);
	check_improve(ch, gsn_stun, TRUE, 2);
	if (!IS_IMMORTAL(ch))
	{
	WAIT_STATE(ch, PULSE_VIOLENCE*3/2);
	}

	af.where = TO_AFFECTS;
	af.type = gsn_stun;
	af.level = ch->level;
	af.duration = 1;
	af.location = APPLY_NONE;
	af.modifier = 2;
	af.bitvector = AFF_SLEEP;

	affect_to_char(victim, &af);

	af.where = TO_AFFECTS;
	af.type = skill_lookup("noquit");
	af.level = ch->level;
	af.duration = 4;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_NOQUIT;
	affect_to_char(victim, &af);

	if (IS_AWAKE(victim))
	    victim->position = POS_SLEEPING;

    } else {
	damage(ch, victim, 0, gsn_stun, DAM_NONE, TRUE);
	check_improve(ch, gsn_stun, FALSE, 2);
	if (!IS_IMMORTAL(ch))
		{ WAIT_STATE(ch, PULSE_VIOLENCE); }
    }
    check_killer(ch, victim);
}

void do_trap(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    int chance;

    one_argument(argument, arg);

    if ((chance = get_skill(ch, gsn_trap)) == 0
	|| (!IS_NPC(ch)
	    && ch->level < skill_table[gsn_trap].skill_level[ch->class])) {
	send_to_char("You couldn't trap a fly with honey.\n\r", ch);
	return;
    }
    if (arg[0] == '\0') {
	send_to_char("What type of trap do you want to set?  Fire, net, stun, blind, or trip?\n\r", ch);
	return;
    }
    if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)) {
	send_to_char("You cannot set traps in this room.\n\r", ch);
	return;
}
	
	if (IS_SET(ch->in_room->affected_by, AFF_CANT_TRAP) ){
	send_to_char("The ground here is too hard to trap!\n\r",ch);
	return;
}
    if((ch->in_room->sector_type == SECT_CITY) ||
        (ch->in_room->sector_type == SECT_WATER_SWIM) ||
        (ch->in_room->sector_type == SECT_WATER_NOSWIM) ||
        (ch->in_room->sector_type == SECT_AIR) ||
        (ch->in_room->sector_type == SECT_LAVA))
    {
	send_to_char("You cannot set traps in this room.\n\r", ch);
	return;
    }
    if (ch->move < 100) {
	send_to_char("You lack the energy to set a trap up. You need 100 moves minimum.\n\r", ch);
	return;
    }
    if (MOUNTED(ch)) {
	send_to_char("You can't set traps while riding!\n\r", ch);
	return;
    }
    if (ch->fighting != NULL) {
	send_to_char("You can't set traps while fighting.\n\r", ch);
	return;
    }
    chance = chance * 9 / 10;

    send_to_char("You attempt to set a trap.\n\r", ch);

    if (number_percent() < chance) {
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 2 * PULSE_VIOLENCE); }
	check_improve(ch, gsn_trap, TRUE, 2);
	ch->in_room->trap->level = ch->level;
	ch->in_room->trap->kingdom = ch->side;
	ch->in_room->trap->is_set = TRUE;
	// trap decay - Akuji
	ch->in_room->trap->tick = 25;

	if (arg[0] == 'f')
	    if (ch->level >= 35)
		ch->in_room->trap->type = 0;
	    else
		send_to_char("You aren't high enough level yet.\n\r", ch);
	else if (arg[0] == 'n')
	    if (ch->level >= 30) {
		// Attempting to add lag to nets from assassins. -Amos
		//if(strcmp(class_table[ch->class].name, "assassin"))
		//{ WAIT_STATE(ch, 3 * PULSE_VIOLENCE + 6); }
		ch->in_room->trap->type = 1; }
	    else
		send_to_char("You aren't high enough level yet.\n\r", ch);
	else if (arg[0] == 's')
	    if (ch->level >= 35)
		ch->in_room->trap->type = 2;
	    else
		send_to_char("You aren't high enough level yet.\n\r", ch);
	else if (arg[0] == 'b')
	    if (ch->level >= 25)
		ch->in_room->trap->type = 3;
	    else
		send_to_char("You aren't high enough level yet.\n\r", ch);
	else if (arg[0] == 't')
	    ch->in_room->trap->type = 4;
	else
	    send_to_char("What type of trap? Fire, net, stun, blind, or trip?\n\r", ch);
    }
    send_to_char("You work hard setting up your trap.\n\r", ch);
    send_to_char("You feel pretty tired.\n\r", ch);
    ch->move -= 100;
    if (ch->move < 5)
	ch->move = 5;

}

void do_trip(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument, arg);

    if ((chance = get_skill(ch, gsn_trip)) == 0
	|| (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_TRIP))
	|| (!IS_NPC(ch)
	    && ch->level < skill_table[gsn_trip].skill_level[ch->class])) {
	send_to_char("Tripping?  What's that?\n\r", ch);
	return;
    }

    if (arg[0] == '\0') {
	victim = ch->fighting;
	if (victim == NULL) {
	    send_to_char("But you aren't fighting anyone!\n\r", ch);
	    return;
	}
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (MOUNTED(ch)) {
	send_to_char("You can't trip while riding!\n\r", ch);
	return;
    }
    if (MOUNTED(victim)) {
	send_to_char("You can't trip someone who is riding.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if (IS_AFFECTED(victim, AFF_FLYING)) {
	act("$S feet aren't on the ground.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (victim->position < POS_FIGHTING) {
	act("$N is already down.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (victim == ch) {
	send_to_char("You fall flat on your face!\n\r", ch);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, PULSE_VIOLENCE); }
	act("$n trips over $s own feet!", ch, NULL, NULL, TO_ROOM);
	victim->position = POS_RESTING;
	return;
    }
    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	act("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
	return;
    }
    /* modifiers */

    /* size */
    if (ch->size < victim->size)
	chance += (ch->size - victim->size) * 10;	/* bigger = harder to trip */

    /* dex */
    chance += get_curr_stat(ch, STAT_DEX);
    chance -= get_curr_stat(victim, STAT_DEX) * 3 / 2;

    /* speed */
    if (IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags, OFF_FAST) || IS_AFFECTED(victim, AFF_HASTE))
	chance -= 20;

    /* level */
    chance += (ch->level - victim->level) * 2;

    /* now the attack */
    if (number_percent() < chance) {
	act("$n trips you and you go down!", ch, NULL, victim, TO_VICT);
	act("You trip $N and $N goes down!", ch, NULL, victim, TO_CHAR);
	act("$n trips $N, sending $M to the ground.", ch, NULL, victim, TO_NOTVICT);
	check_improve(ch, gsn_trip, TRUE, 1);
	
	if (is_affected(victim, gsn_magic_song))
	{
		act("$n is tripped, and stops singing!", victim, NULL, NULL, TO_ROOM);
		send_to_char("You can't sing anymore, you're tripped\n\r",ch);
		affect_strip(ch, gsn_magic_song);
	}
	
		if (is_affected(victim, gsn_life_song))
	{
		act("$n is tripped, and stops singing!", victim, NULL, NULL, TO_ROOM);
		send_to_char("You can't sing anymore, you're tripped\n\r",ch);
		affect_strip(ch, gsn_life_song);
	}
			if (is_affected(victim, gsn_attack_song))
	{
		act("$n is tripped, and stops singing!", victim, NULL, NULL, TO_ROOM);
		send_to_char("You can't sing anymore, you're tripped\n\r",ch);
		affect_strip(ch, gsn_attack_song);
	}
			if (is_affected(victim, gsn_defense_song))
	{
		act("$n is tripped, and stops singing!", victim, NULL, NULL, TO_ROOM);
		send_to_char("You can't sing anymore, you're tripped\n\r",ch);
		affect_strip(ch, gsn_defense_song);
	}
			if (is_affected(victim, gsn_speed_song))
	{
		act("$n is tripped, and stops singing!", victim, NULL, NULL, TO_ROOM);
		send_to_char("You can't sing anymore, you're tripped\n\r",ch);
		affect_strip(ch, gsn_speed_song);
	}
			if (is_affected(victim, gsn_learning_song))
	{
		act("$n is tripped, and stops singing!", victim, NULL, NULL, TO_ROOM);
		send_to_char("You can't sing anymore, you're tripped\n\r",ch);
		affect_strip(ch, gsn_learning_song);
	}

	/* DAZE_STATE(victim, 2 * PULSE_VIOLENCE); */
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, PULSE_VIOLENCE); }
	victim->position = POS_RESTING;
	damage(ch, victim, number_range(2, 2 + 2 * victim->size), gsn_trip,
	       DAM_BASH, TRUE);
    } else {
	damage(ch, victim, 0, gsn_trip, DAM_BASH, TRUE);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, PULSE_VIOLENCE); }
	check_improve(ch, gsn_trip, FALSE, 1);
    }
    check_killer(ch, victim);
}

void do_kill(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Kill whom?\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("You hit yourself.  Ouch!\n\r", ch);
//      multi_hit( ch, ch, TYPE_UNDEFINED );
	return;
    }
    if (is_safe(ch, victim))
	return;

    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	act("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (ch->position == POS_FIGHTING) {
	send_to_char("You do the best you can!\n\r", ch);
	return;
    }
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 1 * PULSE_VIOLENCE); }
//    multi_hit(ch, victim, TYPE_UNDEFINED);
    one_hit(ch, victim, TYPE_UNDEFINED, FALSE);

    return;
}

void do_murde(CHAR_DATA * ch, char *argument)
{
    send_to_char("If you want to MURDER, spell it out.\n\r", ch);
    return;
}

void do_murder(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Murder whom?\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_CHARM) || (IS_NPC(ch) && IS_SET(ch->act, ACT_PET)))
	return;

    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("Suicide is a mortal sin.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;
/*
   if (IS_NPC(victim) &&
   victim->fighting != NULL &&
   !is_same_group(ch,victim->fighting))
   {
   send_to_char("Kill stealing is not permitted.\n\r",ch);
   return;
   }
 */
    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	act("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (ch->position == POS_FIGHTING) {
	send_to_char("You do the best you can!\n\r", ch);
	return;
    }
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 1 * PULSE_VIOLENCE); }
    if (IS_NPC(ch))
	sprintf(buf, "Help! I am being attacked by %s!", ch->short_descr);
    else
	sprintf(buf, "Help!  I am being attacked by %s!", ch->name);
    do_yell(victim, buf);
    check_killer(ch, victim);
    multi_hit(ch, victim, TYPE_UNDEFINED);
    return;
}

void do_backstab(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if ((get_skill(ch, gsn_backstab) == 0 || ch->level < skill_table[gsn_backstab].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }
    if (!IS_NPC(ch)
	&& ch->level < skill_table[gsn_backstab].skill_level[ch->class]) {
	send_to_char("You better leave the backstabbing to the assassins.\n\r", ch);
	return;
    }
    if (arg[0] == '\0') {
	send_to_char("Backstab whom?\n\r", ch);
	return;
    }
    if (MOUNTED(ch)) {
	send_to_char("You can't backstab while riding!\n\r", ch);
	return;
    }
    if (ch->fighting != NULL) {
	send_to_char("You're facing the wrong end.\n\r", ch);
	return;
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("You can't do that to yourself!\n\r", ch);
	return;
    }
    if (victim->fighting != NULL) {
	send_to_char("You could not get behind them.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("How can you sneak up on yourself?\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {
	send_to_char("You need to wield a weapon to backstab.\n\r", ch);
	return;
    }
if (!IS_IMMORTAL(ch))
{ WAIT_STATE(ch, skill_table[gsn_backstab].beats); }
    if (number_percent() < get_skill(ch, gsn_backstab)
	|| (get_skill(ch, gsn_backstab) >= 2 && !IS_AWAKE(victim))) {
	check_improve(ch, gsn_backstab, TRUE, 1);

	one_hit(ch, victim, gsn_backstab, FALSE);
    } else {
	check_improve(ch, gsn_backstab, FALSE, 1);
	damage(ch, victim, 0, gsn_backstab, DAM_NONE, TRUE);
    }

    check_killer(ch, victim);
    return;
}

void do_circle(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if ((!IS_NPC(ch) && ch->level < skill_table[gsn_circle].skill_level[ch->class])
       || get_skill(ch, gsn_circle) <= 0) {
	send_to_char(
			"You draw a few circles in the dirt.\n\r", ch);
	return;
    }

if (arg[0] == '\0') {
        victim = ch->fighting;
        if (victim == NULL) {
            send_to_char("But you aren't fighting anyone!\n\r", ch);
            return;
        }  
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }

    if (MOUNTED(ch)) {
	send_to_char("You can't circle while riding!\n\r", ch);
	return;
    }
    if (ch->fighting == NULL) {
	send_to_char("But you aren't fighting anyone!.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("You are starting to get dizzy.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {
	send_to_char("You need to wield a weapon to circle.\n\r", ch);
	return;
    }
    check_killer(ch, victim);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, skill_table[gsn_circle].beats); }
    if (number_percent() < get_skill(ch, gsn_circle)) {
	check_improve(ch, gsn_circle, TRUE, 1);

	one_hit(ch, victim, gsn_circle, FALSE);
/*
      if(IS_AFFECTED(ch, AFF_HASTE))
       one_hit(ch, victim, gsn_circle, FALSE);
*/

    } else {
	check_improve(ch, gsn_circle, FALSE, 1);
	damage(ch, victim, 0, gsn_circle, DAM_NONE, TRUE);
    }

    return;
}
void do_lunge(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;
    int dam=0;

    one_argument(argument, arg);

    /*if (!IS_NPC(ch) && get_skill(ch, gsn_dragonsword) == 0) {
      if(get_skill(ch, gsn_drag) > 0)
       {
        do_drag(ch, argument);
        return;
       } else {
	send_to_char("What is it exactly that you're trying to do?\n\r", ch);
	return;
       }
    }*/
    if (arg[0] == '\0') {
	victim = ch->fighting;
	if (victim == NULL) {
	    send_to_char("But you aren't fighting anyone!\n\r", ch);
	    return;
	}
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }

    if (ch->fighting == NULL) {
	send_to_char("But you aren't fighting anyone!\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("That would be pointless.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

	chance = number_range(0,20);
    check_killer(ch, victim);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, skill_table[gsn_dragonsword].beats); }
    if (number_percent() < get_skill(ch, gsn_dragonsword)) {
	check_improve(ch, gsn_dragonsword, TRUE, 1);
		
	switch (chance) {
		default:
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		one_hit(ch, victim, gsn_dragonsword, FALSE);
		break;
		
		case 13:
		case 14:
		one_hit(ch, victim, gsn_dragonsword, FALSE);
		shock_effect(victim, 3, dam, TARGET_CHAR);
		break;
		
		case 15:
		case 16:
		one_hit(ch, victim, gsn_dragonsword, FALSE);
		cold_effect(victim, 5, dam, TARGET_CHAR);
		break;
		
		case 17:
		case 18:
		one_hit(ch, victim, gsn_dragonsword, FALSE);
		fire_effect((void *) victim, 3, dam, TARGET_CHAR);
		break;
		
		case 19:
		one_hit(ch, victim, gsn_dragonsword, FALSE);
		dam = number_range(1, ch->level / 12 + 1);
		damage_old(ch, victim, dam, 0, DAM_NEGATIVE, FALSE);
		damage_old(ch, victim, dam, 0, DAM_NEGATIVE, FALSE);
		ch->hit += 3*dam;
		break;
		
		case 20:	
		one_hit(ch, victim, gsn_dragonsword, FALSE);
		dam = number_range(1, ch->level / 12 + 1);
		damage_old(ch, victim, dam, 0, DAM_NEGATIVE, FALSE);
		damage_old(ch, victim, dam, 0, DAM_NEGATIVE, FALSE);
		ch->mana += 3*dam;
		break;
	}
    } else {
	check_improve(ch, gsn_dragonsword, FALSE, 1);
	damage(ch, victim, 0, gsn_dragonsword, DAM_NONE, TRUE);
    }

    return;
}

void do_flee(CHAR_DATA * ch, char *argument){
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    CHAR_DATA *rch;
    int attempt;
    if ((victim = ch->fighting) == NULL) {
		if (ch->position == POS_FIGHTING)
		    ch->position = POS_STANDING;
		send_to_char("You aren't fighting anyone.\n\r", ch);
		return;
    }
    if (IS_SET(ch->affected_by, AFF_WEB)) {
		send_to_char("You can't flee your feet are stuck!\n\r", ch);
		return;
    }
    was_in = ch->in_room;
    for (attempt = 0; attempt < 6; attempt++) {
		EXIT_DATA *pexit;
		int door;
		door = number_door();
		if ((pexit = was_in->exit[door]) == 0
	    || pexit->u1.to_room == NULL
	    || IS_SET(pexit->exit_info, EX_CLOSED)
	    || number_range(0, ch->daze) != 0
	    || (IS_NPC(ch)
		&& IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB)))
		    continue;
		for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room) {
		   if (IS_NPC(rch) && rch->spec_fun == spec_lookup("spec_wall_force")) {
		       if ((door == 0 && !strcmp(rch->long_descr ,"{YA wall of force blocks the north exit.{N\n\r"))
		       || (door == 1 && !strcmp(rch->long_descr ,"{YA wall of force blocks the east exit.{N\n\r"))
		       || (door == 2 && !strcmp(rch->long_descr ,"{YA wall of force blocks the south exit.{N\n\r"))
		       || (door == 3 && !strcmp(rch->long_descr ,"{YA wall of force blocks the west exit.{N\n\r"))
		       || (door == 4 && !strcmp(rch->long_descr ,"{YA wall of force blocks the up exit.{N\n\r"))
		       || (door == 5 && !strcmp(rch->long_descr ,"{YA wall of force blocks the down exit.{N\n\r"))){
			   		send_to_char("You run into a wall trying to flee!\n\r", ch);
			   		continue;
			   }
		   }
        }
		move_char(ch, door, FALSE);
		if ((now_in = ch->in_room) == was_in)
	    	continue;

		ch->in_room = was_in;
		act("$n has fled!", ch, NULL, NULL, TO_ROOM);
		ch->in_room = now_in;

		if (!IS_NPC(ch)) {
		    send_to_char("You flee from combat!\n\r", ch);
		    if ((ch->class == 2)
			&& (number_percent() < 3 * (ch->level / 2)))
				send_to_char("You snuck away safely.\n\r", ch);
		    else {
				if (ch->level < 50) {
				    send_to_char("You lost 10 exp.\n\r", ch);
				    gain_exp(ch, -10);
				}
		    }
		}
		stop_fighting(ch, TRUE);
		if (IS_SET(victim->act, PLR_TMPKILLER))
		    REMOVE_BIT(victim->act, PLR_TMPKILLER);
		if (IS_SET(ch->act, PLR_TMPKILLER))
		    REMOVE_BIT(ch->act, PLR_TMPKILLER);
		return;
    }
    send_to_char("{YPANIC! You couldn't escape!\n\r{x", ch);
    return;
}

void do_rescue(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;

    if ( !IS_NPC(ch) && (get_skill(ch, gsn_rescue) == 0
	|| ch->level < skill_table[gsn_rescue].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    one_argument(argument, arg);
    if (arg[0] == '\0') {
	send_to_char("Rescue whom?\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("What about fleeing instead?\n\r", ch);
	return;
    }
    if (!IS_NPC(ch) && IS_NPC(victim)) {
	send_to_char("Doesn't need your help!\n\r", ch);
	return;
    }
    if (ch->fighting == victim || victim->fighting == ch) {
	send_to_char("What about fleeing instead?.\n\r", ch);
	return;
    }
    if ((fch = victim->fighting) == NULL) {
	send_to_char("That person is not fighting right now.\n\r", ch);
	return;
    }
/*
   if ( IS_NPC(fch) && !is_same_group(ch,victim))
   {
   send_to_char("Kill stealing is not permitted.\n\r",ch);
   return;
   }
 */
 if (!IS_IMMORTAL(ch))
 { WAIT_STATE(ch, skill_table[gsn_rescue].beats); }
    if ( !IS_NPC(ch) && ( number_percent() > get_skill(ch, gsn_rescue) ) ) {
	send_to_char("You fail the rescue.\n\r", ch);
	check_improve(ch, gsn_rescue, FALSE, 1);
	return;
    }
    act("You rescue $N!", ch, NULL, victim, TO_CHAR);
    act("$n rescues you!", ch, NULL, victim, TO_VICT);
    act("$n rescues $N!", ch, NULL, victim, TO_NOTVICT);
    check_improve(ch, gsn_rescue, TRUE, 1);

    stop_fighting(fch, FALSE);
    stop_fighting(victim, FALSE);

    check_killer(ch, fch);
    set_fighting(ch, fch);
    set_fighting(fch, ch);
    return;
}

void do_earthlore(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    AFFECT_DATA af;
	int afft; //for determining what geomancy does on that tile
	int chance; // multiple uses
	
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
	// mass purify chance based per hit
	afft = 0;
	break;
		
	case (SECT_FIELD):
	case (SECT_PLAINS):
	case (SECT_FARMLAND):
	case (SECT_SCRUBLAND):
	case (SECT_WHEATFIELD):
	//"tree branch" counter
	afft = 1;
	break;

	case (SECT_JUNGLE):
	case (SECT_FOREST):
	case (SECT_DENSE_FOREST):
	case (SECT_DARKWOOD):
	//stone skinish
	afft = 2;
	break;

	case (SECT_HILLS):
	case (SECT_MOUNTAIN):
	case (SECT_DIRTPATH):
	// stone protection, reduce damage 5%
	afft = 3;
	break;

	case (SECT_WATER_SWIM):	
	case (SECT_OCEAN):
	case (SECT_WATER_NOSWIM):
	case (SECT_FROZEN_SEA):
	case (SECT_WHIRLPOOL):
	// area fly, area refresh
	afft = 4;
	break;

	case (SECT_DESERT):
	case (SECT_BEACH):
	case (SECT_QUICKSAND):
	// faerie fire on enemies
	afft = 5;
	break;
	
	case (SECT_WASTELAND):
	case (SECT_SWAMP):
	case (SECT_BOG):
	case (SECT_DARK_SWAMP):
	// protection evil
	// protection good
	afft = 6;
	break;

	case (SECT_TUNDRA):
	case (SECT_ICE):
	case (SECT_ICE_MOUNTAIN):
	case (SECT_FROZEN_FOREST):
	// "counter" ice shard
	afft = 7;
	break;
	
	case (SECT_LAVA):
	case (SECT_LAVACHUTE):
	case (SECT_FIRE_ROOM):
	// "counter" lava shield
	afft = 8;
	break;
	
	case (SECT_PYRAMID):
	case (SECT_UNDERGROUND): 
	case (SECT_CAVE):
	case (SECT_CAVERN):
	case (SECT_EARTH):
	case (SECT_CAVE_FLOOR):
	// triple decoy echos
	afft = 9;
	break;
	
	case (SECT_BLOOD_RIVER):
	case (SECT_REDOCEAN):
	// remove curse, cure blind, cure poison, cure disease, 2x purify
	afft = 10;
	break;

	case (SECT_AIR):
	case (SECT_WIND):
	case (SECT_TORNADO):
	// silence room
	afft = 11;
	break;
	
    default:
    afft = 12;
    // reflect, prism user
	break;
    }
    
    victim = ch->fighting;
    if (victim == NULL)
    victim = ch;
    
    vch = ch->fighting;
    if (vch == NULL)
    vch = ch;
	
    if ( !IS_NPC(ch) && (get_skill(ch, gsn_earthlore) == 0
	|| ch->level < skill_table[gsn_geomancy].skill_level[ch->class])) {
	send_to_char("You can't control the elements!\n\r", ch);
	return;
    }

one_argument(argument, arg);
    if (!IS_NPC(ch)	&& ch->level < skill_table[gsn_earthlore].skill_level[ch->class])
     {
	 	send_to_char("You can't control the elements!.\n\r", ch);
		return;
   	 }
    
    if (is_safe(ch, victim))
	return;
	
	if (ch->mana <= 0){
		send_to_char("Not enough mana.\n\r",ch);
	return;}
	
	if (ch->hit <= 50){
		send_to_char("Not enough health.\n\r",ch);
	return;}
	
	if (ch->move <= 50){
		send_to_char("Not enough movement points.\n\r",ch);
	return;}
	
// Whee, we're finally ready
WAIT_STATE(ch, PULSE_VIOLENCE);

if (( number_percent() ) > (get_skill(ch, gsn_earthlore)-5))
{
	send_to_char("You failed to call the elements.\n\r",ch);
	check_improve(ch, gsn_earthlore, FALSE, 1);
	return;
}


check_improve(ch, gsn_earthlore, TRUE, 1);


	switch (afft) {
		case 0:
		send_to_char("The earth's power restores your status.\n\r",ch);
		spell_purify(skill_lookup("purify"), ch->level+3, ch, ch, TARGET_CHAR);
		ch->mana -= ch->level/2;
		
			for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
			vch_next = vch->next_in_room;
			
			chance = number_percent();
			if (chance > 75) {
		
			if (vch->side == ch->side && vch != ch) {
				ch->mana -= ch->level/4;
				spell_purify(skill_lookup("purify"), ch->level+3, vch, vch, TARGET_CHAR);
			
			send_to_char("You feel the earth's power restoring you!\n\r",vch);
			}
			else if (vch->side != ch->side)
			send_to_char("You feel the earth's power pass over you.\n\r",vch);
			}
	}
		break;
		
			case 1:
		send_to_char("The plant life shields you from harm.\n\r",ch);
		ch->mana -= ch->level/2;
		
			af.where = TO_AFFECTS;
    		af.type = gsn_vine_counter;
    		af.level = 1;
    		af.duration = 0;
    		af.location = APPLY_DEX;
    		af.modifier = 3;
    		af.bitvector = 0;
    		affect_to_char(ch, &af);
    		send_to_char("Vines trail you at your feet!\n\r", ch);
		
		break;
		
		case 2:
		send_to_char("The trees encircle you protectively.\n\r",ch);
		ch->mana -= ch->level/5;
		
			af.where = TO_AFFECTS;
    		af.type = gsn_earthlore;
    		af.level = 1;
    		af.duration = 5;
    		af.location = APPLY_AC;
    		af.modifier = -70;
    		af.bitvector = 0;
    		affect_to_char(ch, &af);
		
		break;
		
		case 3:
		send_to_char("The stones surround you and protect you from harm.\n\r",ch);
		ch->mana -= ch->level;
		
			af.where = TO_AFFECTS;
    		af.type = gsn_earthlore;
    		af.level = 1;
    		af.duration = 3;
    		af.location = APPLY_CON;
    		af.modifier = 2;
    		af.bitvector = 0;
    		affect_to_char(ch, &af);
		
		break;
		
		case 4:
		send_to_char("The water rises to your call.\n\r",ch);
		ch->move -= ch->level/3;
		ch->mana -= 10;
				spell_fly(skill_lookup("fly"), ch->level+3, vch, vch, TARGET_CHAR);
				spell_refresh(skill_lookup("refresh"), ch->level, vch, vch, TARGET_CHAR);
		
			for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
			vch_next = vch->next_in_room;
			
			chance = number_percent();
			if (chance > 75) {
		
			if (vch->side == ch->side && vch != ch) {
				ch->mana -= ch->level/5;
				spell_fly(skill_lookup("fly"), ch->level+3, vch, vch, TARGET_CHAR);
				spell_refresh(skill_lookup("refresh"), ch->level, vch, vch, TARGET_CHAR);
			
			send_to_char("The water restores you!\n\r",vch);
					}
			
				}
			}
		break;
		
		case 5:
		send_to_char("A sandstorm swirls and disrupts your enemies.\n\r",ch);
		ch->move -= 5;
		ch->mana -= 5;
				
		
			for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
			vch_next = vch->next_in_room;
			
			chance = number_percent();
			if (chance > 75) {
		
			if (vch->side != ch->side) {
				ch->mana -= ch->level/5;
				spell_faerie_fire(skill_lookup("faerie_fire"), ch->level*3/2, vch, vch, TARGET_CHAR);
			
			send_to_char("The sand ruins your armor!\n\r",vch);
					}
			
				}
			}
			
		break;
		
		case 6:
		send_to_char("The swamp waters shield you.\n\r",ch);
		ch->mana -= ch->level;
		
		if (!is_affected(ch,gsn_earthlore)){
			af.where = TO_AFFECTS;
    		af.type = gsn_earthlore;
    		af.level = ch->level;
    		af.duration = 9;
    		af.location = APPLY_SAVING_SPELL;
    		af.modifier = -20;
    		af.bitvector = AFF_PROTECT_EVIL;
    		affect_to_char(ch, &af);
    		send_to_char("You feel holy and pure.\n\r", ch);
    		
    		af.where = TO_AFFECTS;
    		af.type = gsn_earthlore;
    		af.level = ch->level;
    		af.duration = 9;
    		af.location = APPLY_SAVING_SPELL;
    		af.modifier = -20;
    		af.bitvector = AFF_PROTECT_GOOD;
    		affect_to_char(ch, &af);
    		send_to_char("You feel aligned with darkness.\n\r", ch);
	}
		break;
		
		case 7:
		send_to_char("A frosty wind surrounds you.\n\r",ch);
		ch->move -= ch->level/3;
		
			af.where = TO_AFFECTS;
    		af.type = gsn_frost_shield;
    		af.level = 1;
    		af.duration = 3;
    		af.location = APPLY_STR;
    		af.modifier = 2;
    		af.bitvector = 0;
    		affect_to_char(ch, &af);
		
		break;
		
		case 8:
		send_to_char("A gout of lava surrounds you.\n\r",ch);
		ch->move -= ch->level/3;
		
			af.where = TO_AFFECTS;
    		af.type = gsn_lava_shield;
    		af.level = 1;
    		af.duration = 3;
    		af.location = APPLY_DEX;
    		af.modifier = 2;
    		af.bitvector = 0;
    		affect_to_char(ch, &af);
		
		break;
		case 9:
		send_to_char("You call out, and the caverns echo endlessly...\n\r",ch);
		ch->move -= 20;
		ch->hit  -= 20;
		ch->mana -= 20;
		spell_decoy(skill_lookup("decoy"), ch->level+3,ch,ch,TARGET_CHAR);
		spell_decoy(skill_lookup("decoy"), ch->level+3, ch, ch, TARGET_CHAR);
		spell_decoy(skill_lookup("decoy"), ch->level+3, ch, ch, TARGET_CHAR);
		break;
		
		case 10:
		send_to_char("An intense flash of light purifies your wounds!\n\r",ch);
		spell_cure_poison(skill_lookup("cure poison"), ch->level+3, ch, ch, TARGET_CHAR);
		spell_remove_curse(skill_lookup("remove curse"), ch->level+3, ch, ch, TARGET_CHAR);
		spell_cure_disease(skill_lookup("cure disease"), ch->level+3, ch, ch, TARGET_CHAR);
		spell_cure_blindness(skill_lookup("cure blindness"), ch->level+3, ch, ch, TARGET_CHAR);
		spell_purify(skill_lookup("purify"), ch->level+3, ch, ch, TARGET_CHAR);
		spell_purify(skill_lookup("purify"), ch->level+3, ch, ch, TARGET_CHAR);
		break;
		
		case 11:
		
		act("A vacuum sucks all the air out of the room!", NULL, NULL, NULL, TO_ROOM);

   		    af.where = TO_AFFECTS;
    		af.type = skill_lookup("silence");
    		af.level = ch->level/2;
    		af.duration = ch->level / 20;
    		af.bitvector = AFF_SILENT_ROOM;
    		affect_to_room(ch->in_room, &af);
    		break;
    		
    	case 12:
    	default:
		send_to_char("You feel empowered by the earth.\n\r",ch);
		ch->hit -= ch->level;
		ch->mana -= ch->level;
		
		af.where = TO_AFFECTS;
   		af.type = skill_lookup("reflect");
   	 	af.level = ch->level/2;
   		af.duration = 1;
   	 	af.location = APPLY_CON;
    	af.modifier = 2;
    	af.bitvector = AFF_REFLECT;
    	affect_to_char(ch, &af);
    	
    	spell_heal(skill_lookup("heal"), ch->level, ch, ch, TARGET_CHAR);
		
		break;
}
return;
}

void do_geomancy(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    AFFECT_DATA af;
	int argt; //for determining if arg is needed
	int afft; //for determining what geomancy does on that tile
	int chance; // multiple uses
	
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
	argt = 0;
	afft = 0;
	break;
		
	case (SECT_FIELD):
	case (SECT_PLAINS):
	case (SECT_FARMLAND):
	case (SECT_SCRUBLAND):
	case (SECT_WHEATFIELD):
	//damage victim with bash, chance to slow victim
	argt = 1;
	afft = 1;
	break;

	case (SECT_JUNGLE):
	case (SECT_FOREST):
	case (SECT_DENSE_FOREST):
	case (SECT_DARKWOOD):
	//(small) chance to entangle victim, damage with wood
	argt = 1;
	afft = 2;
	break;

	case (SECT_HILLS):
	case (SECT_MOUNTAIN):
	case (SECT_DIRTPATH):
	// good damage, bash, daze state
	argt = 1;
	afft = 3;
	break;

	case (SECT_WATER_SWIM):	
	case (SECT_OCEAN):
	case (SECT_WATER_NOSWIM):
	case (SECT_FROZEN_SEA):
	case (SECT_WHIRLPOOL):
	// area drowning damage, daze state
	argt = 1;
	afft = 4;
	break;

	case (SECT_DESERT):
	case (SECT_BEACH):
	case (SECT_QUICKSAND):
	// pierce damage to victim, chance to blind 0 tic
	argt = 1;
	afft = 5;
	break;
	
	case (SECT_WASTELAND):
	case (SECT_SWAMP):
	case (SECT_BOG):
	case (SECT_DARK_SWAMP):
	// disease damage to victim, chance to poison and plague
	argt = 1;
	afft = 6;
	break;

	case (SECT_TUNDRA):
	case (SECT_ICE):
	case (SECT_ICE_MOUNTAIN):
	case (SECT_FROZEN_FOREST):
	// high cold damage to victim, nothing else
	argt = 1;
	afft = 7;
	break;
	
	case (SECT_LAVA):
	case (SECT_LAVACHUTE):
	case (SECT_FIRE_ROOM):
	// high fire damage to victim, mirror of afft 7
	argt = 1;
	afft = 8;
	break;
	
	case (SECT_PYRAMID):
	case (SECT_UNDERGROUND): 
	case (SECT_CAVE):
	case (SECT_CAVERN):
	case (SECT_EARTH):
	case (SECT_CAVE_FLOOR):
	// area bash spell, chance to knock group to feet
	argt = 1;
	afft = 9;
	break;
	
	case (SECT_BLOOD_RIVER):
	case (SECT_REDOCEAN):
	// light damage to victim, chance to curse
	argt = 1;
	afft = 10;
	break;

	case (SECT_AIR):
	case (SECT_WIND):
	case (SECT_TORNADO):
	// area pierce damage to victim, chance to blind
	argt = 1;
	afft = 11;
	break;
	
    default:
    argt = 0;
    afft = 12;
    // haste, sanc user, only 2 tiles use this.
	break;
    }
	
    if ( !IS_NPC(ch) && (get_skill(ch, gsn_geomancy) == 0
	|| ch->level < skill_table[gsn_geomancy].skill_level[ch->class])) {
	send_to_char("You can't control the elements!\n\r", ch);
	return;
    }

one_argument(argument, arg);
    if (!IS_NPC(ch)	&& ch->level < skill_table[gsn_geomancy].skill_level[ch->class])
     {
	 	send_to_char("You can't control the elements!.\n\r", ch);
		return;
   	 }

if (arg[0] == '\0')
	{
	victim = ch->fighting;
	if (victim == NULL && argt!=0) 
		{
	    send_to_char("But you aren't in combat!\n\r", ch);
	    return;
		}
	else if (victim == NULL && argt==0)
	victim = ch;
	
    } 
else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    
if (victim == ch && argt != 0) {
	send_to_char("You decide to do better things with your time.\n\r", ch);
	return;
    }

    if (is_safe(ch, victim))
	return;
	
	if (ch->mana <= 0){
		send_to_char("Not enough mana.\n\r",ch);
	return;}
	
	if (ch->hit <= 50){
		send_to_char("Not enough health.\n\r",ch);
	return;}
	
	if (ch->move <= 50){
		send_to_char("Not enough movement points.\n\r",ch);
	return;}
	
// Whee, we're finally ready
WAIT_STATE(ch, PULSE_VIOLENCE);

if (( number_percent() ) > (get_skill(ch, gsn_geomancy)-5))
{
	send_to_char("You failed to call the elements.\n\r",ch);
	check_improve(ch, gsn_geomancy, FALSE, 1);
	return;
}


check_improve(ch, gsn_geomancy, TRUE, 1);
chance = number_percent();
if (chance <= 3){
	send_to_char("The elements bestow you with enchantment!\n\r",ch);
spell_purify(skill_lookup("purify"), ch->level+3, ch, ch, TARGET_CHAR);
}

	switch (afft) {
		case 0:
		send_to_char("The earth's power heals you of your wounds.\n\r",ch);
		ch->hit += number_range(ch->level*2,ch->level*8);
		ch->move -= ch->level/2;
		if (ch->hit > ch->max_hit)
		ch->hit = ch->max_hit;
		
			for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
			vch_next = vch->next_in_room;
			
			chance = number_percent();
			if (chance > 85) {
		
			if (vch->side == ch->side) {
				ch->mana -= ch->level/4;
				vch->hit += number_range((ch->level + vch->level) * 2, (ch->level + vch->level) * 4);
				
			if (vch->hit > vch->max_hit)
			vch->hit = vch->max_hit;
			
			send_to_char("You feel the earth's power healing you!\n\r",vch);
			}
			else if (vch->side != ch->side)
			send_to_char("You feel the earth's power pass over you.\n\r",vch);
			}
	}
		break;
				
		case 1:
		send_to_char("The plant life rises to your call, attacking your opponent.\n\r",ch);
		damage(ch, victim, number_range(ch->level*3, 210), gsn_geomancy, DAM_BASH, TRUE);
		chance = number_percent();
		ch->mana -= ch->level/2;
		if (chance > 70)
		{
			af.where = TO_AFFECTS;
    		af.type = gsn_geomancy;
    		af.level = 1;
    		af.duration = 0;
    		af.location = APPLY_DEX;
    		af.modifier = -3;
    		af.bitvector = AFF_SLOW;
    		affect_to_char(victim, &af);
    		send_to_char("Vines grip your feet, slowing you down!\n\r", victim);
    		act("$n is held to the ground by many plants!", victim, NULL, NULL, TO_ROOM);
		}
		break;
		
		case 2:
		ch->mana -= ch->level/6;
		ch->move -= ch->level/5;
		send_to_char("You summon the trees to help you in battle.\n\r",ch);
		damage(ch, victim, number_range(ch->level*3,ch->level*4), gsn_geomancy, DAM_MENTAL, TRUE);
		chance = number_percent();
		if (chance > 92)
		{
			ch->mana -= 45;
			af.where = TO_AFFECTS;
    		af.type = gsn_geomancy;
    		af.level = 1;
    		af.duration = 0;
    		af.location = APPLY_DEX;
    		af.modifier = -1;
    		af.bitvector = AFF_WEB;
    		affect_to_char(victim, &af);
    		send_to_char("You are tangled in some branches, unable to move!\n\r", victim);
    		act("$n is surrounded by tree branches.", victim, NULL, NULL, TO_ROOM);
		}
		break;
		
		case 3:
		act("The very earth throws itself at $n.", victim,NULL,NULL, TO_CHAR);
		damage(ch, victim, number_range(ch->level*3,ch->level*5), gsn_geomancy, DAM_BASH, TRUE);
		ch->mana -= ch->level*6/10;
		DAZE_STATE(victim, PULSE_VIOLENCE);
		send_to_char("Stones fly through the air, pummeling you to the ground.\n\r", victim);
    	act("All of the stones on the ground fling themselves at $n.", victim, NULL, NULL, TO_ROOM);
    	break;
		
		case 4:
		send_to_char("You call up a giant tidal wave.\n\r",ch);
		for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
			vch_next = vch->next_in_room;
		
			if (vch->side != ch->side) {
				ch->mana -= ch->level*4/15;
				ch->hit -= ch->level/10;
			damage(ch, vch, number_range(ch->level*4,ch->level*6), gsn_geomancy, DAM_DROWNING, TRUE);
			DAZE_STATE(vch, PULSE_VIOLENCE/2);
			send_to_char("A giant tidal wave smashes you to the ground!\n\r",vch);
			}
			else if (vch->side == ch->side)
			send_to_char("You watch as a giant tidal wave strikes all enemies.\n\r",vch);
			}
			
		break;
		
		case 5:
		send_to_char("A swirling sandstorm appears.\n\r",ch);
		damage(ch, victim, number_range(ch->level*3, ch->level*5), gsn_geomancy, DAM_PIERCE, TRUE);
				chance = number_percent();
		ch->move -= ch->level/5;
		ch->mana -= ch->level*2/5;
		if (chance > 80)
		{
			af.where = TO_AFFECTS;
    		af.type = gsn_geomancy;
    		af.level = 1;
    		af.duration = 0;
    		af.location = APPLY_CON;
    		af.modifier = -2;
    		af.bitvector = AFF_BLIND;
    		affect_to_char(victim, &af);
    		send_to_char("Sand flies into your eyes!\n\r", victim);
    		act("$n is blinded by sand!", victim, NULL, NULL, TO_ROOM);
		}
		break;
		
		case 6:
		send_to_char("The swamps emit a giant ball of sludge.\n\r",ch);
		damage(ch, victim, number_fuzzy(ch->level*7/2), gsn_geomancy, DAM_DISEASE, TRUE);
		chance = number_percent();
		ch->mana -= ch->level*6/10;
		if (chance > 80)
		{
			af.where = TO_AFFECTS;
    		af.type = gsn_geomancy;
    		af.level = 1;
    		af.duration = 0;
    		af.location = APPLY_STR;
    		af.modifier = -1;
    		af.bitvector = AFF_POISON;
    		affect_to_char(victim, &af);
    		send_to_char("A gross ball of sludge hits you!\n\r", victim);
    		act("$n is hit by a ball of sludge!", victim, NULL, NULL, TO_ROOM);
    		ch->move -= 5;
		}
		
		chance = number_percent();
		if (chance > 80)
		{
			af.where = TO_AFFECTS;
    		af.type = gsn_geomancy;
    		af.level = 1;
    		af.duration = 0;
    		af.location = APPLY_CON;
    		af.modifier = -1;
    		af.bitvector = AFF_PLAGUE;
    		affect_to_char(victim, &af);
    		send_to_char("A gross ball of sludge hits you!\n\r", victim);
    		act("$n is hit by a ball of sludge!", victim, NULL, NULL, TO_ROOM);
    		ch->move -= 5;
		}
		break;
		
		case 7:
		send_to_char("Chunks of ice form in your hands.\n\r",ch);
		damage(ch, victim, number_range(ch->level*5, ch->level*7), gsn_geomancy, DAM_COLD, TRUE);
		ch->hit -= ch->level-10;
		break;
		
		case 8:
		send_to_char("Gouts of lava spew forth from the ground.\n\r",ch);
		damage(ch, victim, number_range(ch->level*5, ch->level*7), gsn_geomancy, DAM_FIRE, TRUE);
		ch->hit -= ch->level-10;
		break;
		
		case 9:
		send_to_char("The cavern walls shake and rumble.\n\r",ch);
				for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
			vch_next = vch->next_in_room;
		
			if (vch->side != ch->side) {
			damage(ch, vch, number_range(170,240), gsn_geomancy, DAM_BASH, TRUE);
			ch->hit -= ch->level*3/10;
			ch->mana -=ch->level*3/10;
			chance = number_percent();
			
			if (chance > 80)
			{
			DAZE_STATE(vch, PULSE_VIOLENCE/2);
			vch->position = POS_RESTING;
			}
			
			send_to_char("An earthquake rocks the ground!\n\r",vch);
			}
			else if (vch->side == ch->side)
			send_to_char("An earthquake rocks the ground, but leaves you unharmed.\n\r",vch);
			}
		break;
		
		case 10:
		send_to_char("An intense flash of light spews forth.\n\r",ch);
		damage(ch, victim, number_range(ch->level*4, 280), gsn_geomancy, DAM_LIGHT, TRUE);
		ch->mana -= ch->level*4/5;
		ch->hit -= ch->level*2/5;
		chance = number_percent();
		if (chance > 75)
		{
			af.where = TO_AFFECTS;
    		af.type = gsn_geomancy;
    		af.level = 1;
    		af.duration = 0;
    		af.location = APPLY_INT;
    		af.modifier = -3;
    		af.bitvector = AFF_CURSE;
    		affect_to_char(victim, &af);
    		send_to_char("A blinding flash of light strikes you!\n\r", victim);
    		act("$n is paralyzed by blinding light!", victim, NULL, NULL, TO_ROOM);
		}
		break;
		
		case 11:
		send_to_char("The winds rage at your command.\n\r",ch);
						for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
			vch_next = vch->next_in_room;
		
			if (vch->side != ch->side) {
			damage(ch, vch, number_range(190,290), gsn_geomancy, DAM_PIERCE, TRUE);
			chance = number_percent();
			ch->move -= ch->level/10;
			ch->mana -= ch->level/12;
			
			if (chance > 80){
		
				af.where = TO_AFFECTS;
    			af.type = gsn_geomancy;
    			af.level = 1;
    			af.duration = 0;
    			af.location = APPLY_WIS;
    			af.modifier = -2;
    			af.bitvector = AFF_BLIND;
    			affect_to_char(vch, &af);
    			send_to_char("Debris flies into your eyes!\n\r", vch);
    			act("$n is blinded by debris!", vch, NULL, NULL, TO_ROOM);
			}
			
			send_to_char("A whirlwind rages in the area!\n\r",vch);
			}
			else if (vch->side == ch->side)
			send_to_char("A whirlwind is channeled into your enemies.\n\r",vch);
			}
			break;
			
			case 12:
			send_to_char("A subtle light washes over you.\n\r",ch);
				af.where = TO_AFFECTS;
    			af.type = gsn_geomancy;
    			af.level = 1;
    			af.duration = 1;
    			af.location = APPLY_WIS;
    			af.modifier = 2;
    			af.bitvector = AFF_HASTE;
    			affect_to_char(ch, &af);
    			
    			send_to_char("You feel protected by the earth.\n\r",ch);
    			
    			af.where = TO_AFFECTS;
    			af.type = gsn_geomancy;
    			af.level = 1;
    			af.duration = 1;
    			af.location = APPLY_INT;
    			af.modifier = 2;
    			af.bitvector = AFF_SANCTUARY;
    			affect_to_char(ch, &af);
    			
    			ch->mana -= ch->level;
    			break;
		
	return;
}
}

void do_kick(CHAR_DATA * ch, char *argument)
{
char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;


if ( !IS_NPC(ch) && (get_skill(ch, gsn_kick) == 0
	|| ch->level < skill_table[gsn_kick].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }

one_argument(argument, arg);
    if (!IS_NPC(ch)
	&& ch->level < skill_table[gsn_kick].skill_level[ch->class]) {
	send_to_char(
			"You better leave the martial arts to fighters.\n\r", ch);
	return;
    }
    if (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_KICK))
	return;

if (arg[0] == '\0') {
	victim = ch->fighting;
	if (victim == NULL) {
	    send_to_char("But you aren't in combat!\n\r", ch);
	    return;
	}
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
if (victim == ch) {
	send_to_char("You kick yourself in anger!\n\r", ch);
	return;
    }

    if (is_safe(ch, victim))
	return;

if (!IS_IMMORTAL(ch))
{WAIT_STATE(ch, PULSE_VIOLENCE / 2); }
if (!strcmp(class_table[ch->class].name, "dragoon")){
	
	if (get_skill(ch, gsn_kick) > number_percent()) {
	damage(ch, victim, number_range(ch->level, ch->level * 3), gsn_kick, DAM_BASH, TRUE);
	check_improve(ch, gsn_kick, TRUE, 1);
    } else {
	damage(ch, victim, 0, gsn_kick, DAM_BASH, TRUE);
	check_improve(ch, gsn_kick, FALSE, 1);
    }
    check_killer(ch, victim);
    return;
} else {
    if (get_skill(ch, gsn_kick) > number_percent()) {
	damage(ch, victim, number_range(ch->level, ch->level * 2), gsn_kick, DAM_BASH, TRUE);
	check_improve(ch, gsn_kick, TRUE, 1);
    } else {
	damage(ch, victim, 0, gsn_kick, DAM_BASH, TRUE);
	check_improve(ch, gsn_kick, FALSE, 1);
    }
    check_killer(ch, victim);
    return;
    }
}

void do_disarm(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int chance, hth, ch_weapon, vict_weapon, ch_vict_weapon;
	hth = 0;

	chance = get_skill(ch, gsn_disarm);

    if (	!IS_NPC(ch)	 &&		chance == 0)
	{
	send_to_char("You don't know how to disarm opponents.\n\r", ch);
	return;
    }
    if (get_eq_char(ch, WEAR_WIELD) == NULL
	&& ((hth = get_skill(ch, gsn_hand_to_hand)) == 0
	    || (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_DISARM)))) {
	send_to_char("You must wield a weapon to disarm.\n\r", ch);
	return;
    }
    if ((victim = ch->fighting) == NULL) {
	send_to_char("You aren't fighting anyone.\n\r", ch);
	return;
    }
    if ((obj = get_eq_char(victim, WEAR_WIELD)) == NULL) {
	send_to_char("Your opponent is not wielding a weapon.\n\r", ch);
	return;
    }
    /* find weapon skills */
    ch_weapon = get_weapon_skill(ch, get_weapon_sn(ch, FALSE));
    vict_weapon = get_weapon_skill(victim, get_weapon_sn(victim, FALSE));
    ch_vict_weapon = get_weapon_skill(ch, get_weapon_sn(victim, FALSE));

    /* modifiers */

    /* skill */
    if (get_eq_char(ch, WEAR_WIELD) == NULL)
	chance = chance * hth / 150;
    else
	chance = chance * ch_weapon / 100;

    chance += (ch_vict_weapon / 2 - vict_weapon) / 2;

    /* dex vs. strength */
    chance += get_curr_stat(ch, STAT_DEX);
    chance -= 2 * get_curr_stat(victim, STAT_STR);

    /* level */
    chance += (ch->level - victim->level) * 2;

    /* and now the attack */
    if (number_percent() < chance) {
		if (!IS_IMMORTAL(ch))
		{ WAIT_STATE(ch, skill_table[gsn_disarm].beats); }
	disarm(ch, victim);
	check_improve(ch, gsn_disarm, TRUE, 1);
    } else {
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, PULSE_VIOLENCE); }
	act("You fail to disarm $N.", ch, NULL, victim, TO_CHAR);
	act("$n tries to disarm you, but fails.", ch, NULL, victim, TO_VICT);
	act("$n tries to disarm $N, but fails.", ch, NULL, victim, TO_NOTVICT);
	check_improve(ch, gsn_disarm, FALSE, 1);
    }
    check_killer(ch, victim);
    return;
}

void do_surrender(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *mob;

    if ((mob = ch->fighting) == NULL) {
	send_to_char("But you're not fighting!\n\r", ch);
	return;
    }
    if (!IS_NPC(ch) && !IS_NPC(mob)) {
	if (ch->side != mob->side) {
	    send_to_char("You can't surrender to enemies!\n\r", ch);
	    return;
	}
    }
    if (!IS_NPC(ch) && IS_NPC(mob)
	&& (!HAS_TRIGGER(mob, TRIG_SURR)
	    || !mp_percent_trigger(mob, ch, NULL, NULL, TRIG_SURR))) {
	act("$N seems to ignore your cowardly act!", ch, NULL, mob, TO_CHAR);
	multi_hit(mob, ch, TYPE_UNDEFINED);
	return;
    }
    act("You surrender to $N!", ch, NULL, mob, TO_CHAR);
    act("$n surrenders to you!", ch, NULL, mob, TO_VICT);
    act("$n tries to surrender to $N!", ch, NULL, mob, TO_NOTVICT);
    stop_fighting(ch, TRUE);
    return;
}

void do_sla(CHAR_DATA * ch, char *argument)
{
    send_to_char("If you want to SLAY, spell it out.\n\r", ch);
    return;
}

void do_slay(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument(argument, arg);
    if (arg[0] == '\0') {
	send_to_char("Slay whom?\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (ch == victim) {
	send_to_char("Suicide is a mortal sin.\n\r", ch);
	return;
    }
    if (!IS_NPC(victim) && victim->level >= get_trust(ch)) {
	send_to_char("You failed.\n\r", ch);
	return;
    }
    act("You slay $M in cold blood!", ch, NULL, victim, TO_CHAR);
    act("$n slays you in cold blood!", ch, NULL, victim, TO_VICT);
    act("$n slays $N in cold blood!", ch, NULL, victim, TO_NOTVICT);
    raw_kill(victim, ch, 0);
    return;
}

void do_push_drag(CHAR_DATA * ch, char *argument, char *verb)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    CHAR_DATA *victim;
    EXIT_DATA *pexit;
    int chance = 0;
    int door;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0') {
	sprintf(buf, "%s <target> <direction>\n\r", capitalize(verb));
	send_to_char(buf, ch);
	return;
    }

    if ((victim = get_char_room(ch, arg1)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }

    if (     !str_cmp(arg2, "n") || !str_cmp(arg2, "north"))	door = 0;
    else if (!str_cmp(arg2, "e") || !str_cmp(arg2, "east"))	door = 1;
    else if (!str_cmp(arg2, "s") || !str_cmp(arg2, "south"))	door = 2;
    else if (!str_cmp(arg2, "w") || !str_cmp(arg2, "west"))	door = 3;
    else if (!str_cmp(arg2, "u") || !str_cmp(arg2, "up"))	door = 4;
    else if (!str_cmp(arg2, "d") || !str_cmp(arg2, "down"))	door = 5;
    else {
	sprintf(buf, "Alas, you cannot %s in that direction.\n\r", verb);
	send_to_char(buf, ch);
	return;
    }

    if (ch == victim) {
        act("You $t yourself about the room and look very silly.", ch, verb, NULL, TO_CHAR);
        act("$n decides to be silly and $t $mself about the room.", ch, verb, NULL, TO_ROOM);
        return;
    }

    in_room = victim->in_room;
    if ((pexit = in_room->exit[door]) == NULL
         || (to_room = pexit->u1.to_room) == NULL
	 || !can_see_room(victim, pexit->u1.to_room)
	 || (IS_SET(to_room->room_flags,ROOM_NO_PUSHTO) 
		&& ch->side != victim->side) ) {
        sprintf(buf, "Alas, you cannot %s them that way.\n\r", verb);
	send_to_char(buf, ch);
	return;
    }

	// This code remmed out for my forcing fix, much easier to add
	// classes this way. -Amos
    
   int check = 0;

    if ((ch->side != victim->side &&
    	 get_skill(ch, gsn_tear) > 0))
    	 check = 1;
    	 
    if ((ch->side != victim->side &&
    	 get_skill(ch, gsn_berserk) > 0))
    	 check = 1;
    	 
    	 //Izlude - Warlock fix for push drag, they can use it on enemies now 
    if ((ch->side != victim->side &&
    	get_skill(ch, skill_lookup("tracer cloud")) > 0))
    	check = 1;

	// Fix for dragoons. -Amos
	if ((ch->side != victim->side &&
    	get_skill(ch, skill_lookup("lunge")) > 0))
    	check = 1;

	//Fix for lancers. -Amos
	if ((ch->side != victim->side &&
    	get_skill(ch, skill_lookup("skewer")) > 0))
    	check = 1;

    if (check == 0 && ch->side != victim->side)
    	{
	    	send_to_char("You can't force people not of your kingdom",ch);
	    	return;
    	}

	// Hackish, because I don't know how to check race. -Amos.
	// Statement checks class, if they aren't one of the listed
	// Classes and they are attempting to force an enemy, returns
	// an failed message to them and exits the function.

	// Blah commented out again because I need to fix the way were checking classes.
	// Back to Izlude's way for now -Amos
	//if (
	//	((!strcmp(class_table[ch->class].name, "barbarian")) 
	//	|| (!strcmp(class_table[ch->class].name, "warlock"))
	//   	|| (!strcmp(class_table[ch->class].name, "dragoon"))
	//	|| (!strcmp(class_table[ch->class].name, "lancer")))
	//	&& ch->side != victim->side)
	//	{
	//	   	send_to_char("You can't force people not of your kingdom",ch);
	//    		return;
	//	}


    if (IS_SET(pexit->exit_info, EX_CLOSED)
        && (!IS_AFFECTED(victim, AFF_PASS_DOOR) || IS_SET(pexit->exit_info,EX_NOPASS))) {
	act("You try to $t them through the $d.", ch, verb, pexit->keyword, TO_CHAR);
	act("$n decides to $t you around!", ch, verb, victim, TO_VICT);
	act("$n decides to $t $N around!", ch, verb, victim, TO_NOTVICT);
	return;
    }

    act("You attempt to $t $N out of the room.", ch, verb, victim, TO_CHAR);
    act("$n is attempting to $t you out of the room!", ch, verb, victim, TO_VICT);
    act("$n is attempting to $t $N out of the room.", ch, verb, victim, TO_NOTVICT);

    if (IS_AFFECTED(victim, AFF_SLEEP)) {
        affect_strip(victim, gsn_sleep);
	affect_strip(victim, gsn_stun);
    }
    if(IS_AFFECTED(victim, AFF_HOLD))
      REMOVE_BIT(victim->affected_by, AFF_HOLD);

    if (!IS_AWAKE(victim))
	do_stand(victim, "");

    if (IS_NPC(victim))
	if (IS_SET(victim->act, ACT_IS_HEALER)
	    || IS_SET(victim->act, ACT_NO_MOVE)
	    || IS_SET(victim->act, ACT_NO_PUSH)
	    || victim->pIndexData->pShop
		|| ( IS_SET(ch->affected_by, AFF_WEB) && !str_cmp(verb, "drag") ) ) {
	    send_to_char("They won't budge.\n\r", ch);
	    return;
    }

    if( !str_cmp(verb, "push"))
	chance = get_skill(ch, gsn_push);
    else if( !str_cmp(verb, "drag"))
	chance = get_skill(ch, gsn_drag);
    else
	send_to_char("{RBug!{N push drag verb\n\r",ch);

//    chance -= (25 - get_curr_stat(victim, STAT_STR)) * 10;

    chance -= 25 ; //75% base aebe
    if (ch->level > victim->level)
	chance += (ch->level - victim->level) * 7;

//	if (!strcmp(class_table[ch->class].name, "assassin"))
//		chance -= 25;

	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, PULSE_VIOLENCE); }

    if (chance < number_percent()) {
        send_to_char("You failed.\n\r", ch);
    if (!str_cmp(verb, "push"))
        check_improve(ch, gsn_push, FALSE, 1);
    else
        check_improve(ch, gsn_drag, FALSE, 1);
        return;
    }

    if (!str_cmp(verb, "push"))
	check_improve(ch, gsn_push, TRUE, 1);
    else
	check_improve(ch, gsn_drag, TRUE, 1);


    if (ch->move > 10) {
	ch->move -= 10;
	send_to_char("You succeed!\n\r", ch);
	act("$n succeeds!", ch, NULL, NULL, TO_ROOM);
	if (!str_cmp(verb, "drag"))
	{
	    if(victim->following == ch)
	    {	
  		// Double move hack-fix -- Archan	
		victim->following = NULL;
	        move_char(ch, door, FALSE);
		move_char(victim, door, FALSE);
		victim->following = ch;
	    }
	    else
	    {
              move_char(ch,door,FALSE);
              move_char(victim,door,FALSE);
            }
	}	
	else move_char(victim, door, FALSE);
	if (IS_NPC(victim) && victim->spec_fun != NULL)
	    if (strncmp("spec_guard", spec_name(victim->spec_fun), strlen("spec_guard")) == 0)
		push_dir(victim, reverse_dir(door));
    } else {
	sprintf(buf, "You are too tired to %s anybody around!\n\r", verb);
	send_to_char(buf, ch);
    }

    return;
}

void do_push(CHAR_DATA * ch, char *argument)
{
   if ((get_skill(ch, gsn_push) == 0 || ch->level < skill_table[gsn_push].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    do_push_drag(ch, argument, "push");

    return;
}

void do_drag(CHAR_DATA * ch, char *argument)
{
    if ((get_skill(ch, gsn_drag) == 0 || ch->level < skill_table[gsn_drag].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    do_push_drag(ch, argument, "drag");

    return;
}

void do_strike(CHAR_DATA * ch, char *argument)
{
CHAR_DATA *victim;
int dam = 0;
int roll;
char arg[MAX_INPUT_LENGTH];
AFFECT_DATA af;


one_argument(argument, arg);
if ((get_skill(ch, gsn_strike) == 0
	|| ch->level < skill_table[gsn_strike].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    if (!IS_NPC(ch)
	&& ch->level < skill_table[gsn_strike].skill_level[ch->class]) {
	send_to_char("Huh?\n\r", ch);
	return;
    }

if (arg[0] == '\0') {
	victim = ch->fighting;
	if (victim == NULL) {
	    send_to_char("But you aren't in combat!\n\r", ch);
	    return;
	}
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

if (victim == ch) {
	send_to_char("You can't do that to yourself!\n\r", ch);
	return;
    }

	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, PULSE_VIOLENCE); }
    roll = number_percent();
    if (get_skill(ch, gsn_strike) > roll)
    {
	roll += ch->level - victim->level;
	roll += get_skill(ch, gsn_brutal_damage)/4;
	if (roll > 124)
	{
	act("$n reaches into your chest and pulls out your heart!", ch, NULL, victim, TO_VICT);
	act("You reach into $N's chest and pull out their heart!{x", ch, NULL, victim, TO_CHAR);
	act("$n reaches into $N's chest and pulls out their heart!{x", ch, NULL, victim, TO_NOTVICT);
	dam = ch->level * 8;
	}
	else if (roll > 122)
	{
	act("$n jumps in the air screaming 'DAAEIEIAAUGH!!!' and punts your head across the room!", ch, NULL, victim, TO_VICT);
	act("You jump in the air screaming a battle cry punting $N's head across the room!{x", ch, NULL, victim, TO_CHAR);
	act("$n jumps in the air screaming 'DAAEIEIAAUGH!!!' and punts $N's head across the room!{x", ch, NULL, victim, TO_NOTVICT);
	dam = ch->level * 6.5;
	}
	else if (roll > 119)
	{
	//act("$n yells 'SIIEEE YUUKEN!! and sticks their thumbs in your eyes!", ch, NULL, victim, TO_VICT);
	//act("You yell 'SIIEEE YUUKEN!! and stick your thumbs in $N's eyes!{x", ch, NULL, victim, TO_CHAR);
	//act("$n yells 'SIIEEE YUUKEN!!and stick their thumbs in $N's eyes!{x", ch, NULL, victim, TO_NOTVICT);
	act("$n strikes quickly, hitting you near the eyes!", ch, NULL, victim, TO_VICT);
	act("You attack at $N's eyes!{x", ch, NULL, victim, TO_CHAR);
	act("$n quickly hits $N in the face!{x", ch, NULL, victim, TO_NOTVICT);
	    af.where = TO_AFFECTS;
	    af.type = skill_lookup("blind");
   af.location  = APPLY_NONE;
	    af.level = ch->level;
	    af.modifier = 0;
	    af.duration = 1;
	    af.bitvector = AFF_BLIND;
	    affect_to_char(victim, &af);

	dam = ch->level * 4;
	}
	else if (roll > 92)
	{
	//act("$n screams 'You die now!' He makes sushi out of your face!", ch, NULL, victim, TO_VICT);
	//act("You scream 'You die now!' You make sushi out of $N's face!{x", ch, NULL, victim, TO_CHAR);
	//act("$n screams 'You die now!' He makes sushi out of $N's face!{x", ch, NULL, victim, TO_NOTVICT);
	act("$n releases a powerful strike at you!", ch, NULL, victim, TO_VICT);
	act("You rush at $N with all your power!{x", ch, NULL, victim, TO_CHAR);
	act("$n strikes $N with a powerful blow!{x", ch, NULL, victim, TO_NOTVICT);
	dam = ch->level * 4;
	}
	else if (roll > 87)
	{
	//act("$n clobbers you with his forehead!", ch, NULL, victim, TO_VICT);
	//act("You clobber $N with your forehead!{x", ch, NULL, victim, TO_CHAR);
	//act("$n clobbers $N with their forehead!{x", ch, NULL, victim, TO_NOTVICT);
	act("$n strikes you in the leg, slowing you down!", ch, NULL, victim, TO_VICT);
	act("You strike $N hard above the kneecap!{x", ch, NULL, victim, TO_CHAR);
	act("$n hits $N's legs with a powerful blow!{x", ch, NULL, victim, TO_NOTVICT);
	    af.where = TO_AFFECTS;
	    af.type = skill_lookup("slow");
	    af.level = ch->level;
   af.location  = APPLY_NONE;
	    af.modifier = 0;
	    af.duration = 1;
	    af.bitvector = AFF_SLOW;
	    affect_to_char(victim, &af);
	dam = ch->level * 3;
	}
	else if (roll > 80)
	{
	//act("$n takes your weapon and clobbers you with it!", ch, NULL, victim, TO_VICT);
	//act("You take $N's weapon and clobber them with it!{x", ch, NULL, victim, TO_CHAR);
	//act("$n takes $N's weapon and clobbers them with it!{x", ch, NULL, victim, TO_NOTVICT);
	act("$n disarms you and hits you with your own weapon!", ch, NULL, victim, TO_VICT);
	act("You deftly steal $N's weapon and attack them with it!{x", ch, NULL, victim, TO_CHAR);
	act("$n steals $N's weapon and hits them with it!{x", ch, NULL, victim, TO_NOTVICT);
	disarm(ch, victim);
	dam = ch->level * 3;
	}
	else if (roll > 65)
	{
	//act("$n kicks you 36 times in the face!", ch, NULL, victim, TO_VICT);
	//act("You kick $N 36 times in the face!{x", ch, NULL, victim, TO_CHAR);
	//act("$n kicks $N 36 times in the face!{x", ch, NULL, victim, TO_NOTVICT);
	act("$n unleashes a flurry of blows at you!", ch, NULL, victim, TO_VICT);
	act("You attack $N as fast as you can!{x", ch, NULL, victim, TO_CHAR);
	act("$n moves like lightning, attacking $N!{x", ch, NULL, victim, TO_NOTVICT);
	dam = ch->level * 3;
	//haste
	    af.where = TO_AFFECTS;
	    af.type = skill_lookup("haste");
	    af.level = ch->level;
	    af.modifier = 0;
	    af.location  = APPLY_NONE;
	    af.duration = 0;
	    af.bitvector = AFF_HASTE;
	    affect_to_char(ch, &af);
	}
	else if (roll > 45)
	{
	//act("$n does sixteen flips and kicks you in the nuts!", ch, NULL, victim, TO_VICT);
	//act("You do sixteen flips and kick $N in the nuts!{x", ch, NULL, victim, TO_CHAR);
	//act("$n does sixteen flips and kicks $N in the nuts!{x", ch, NULL, victim, TO_NOTVICT);
	act("$n strikes you hard, growing stronger!", ch, NULL, victim, TO_VICT);
	act("You feel empowered by your attack against $N!{x", ch, NULL, victim, TO_CHAR);
	act("$n seems to grow a littler stronger after striking $N!{x", ch, NULL, victim, TO_NOTVICT);

	    af.where = TO_AFFECTS;
	    af.type = skill_lookup("ki power");
	    af.level = ch->level;
	    af.modifier = 8;
	    af.location  = APPLY_DAMROLL;
	    af.duration = 10;
	    af.bitvector = AFF_HASTE;
	    affect_to_char(ch, &af);
	dam = ch->level * 3;
	}
	else if (roll > 25)
	{
	//act("$n teaches you new respect for chopsticks!", ch, NULL, victim, TO_VICT);
	//act("You teach $N new respect for chopsticks!{x", ch, NULL, victim, TO_CHAR);
	//act("$n teaches $N new respect for chopsticks!{x", ch, NULL, victim, TO_NOTVICT);
	dam = ch->level * 2;
	}
	else if (roll > 10)
	{
	//act("$n yells at you 'You have bad manna's!'", ch, NULL, victim, TO_VICT);
	//act("You yell at $N 'You have bad manna's!'{x", ch, NULL, victim, TO_CHAR);
	//act("$n yells at $N 'You have bad manna's!'{x", ch, NULL, victim, TO_NOTVICT);
	dam = ch->level * 2;
	}
	else if (roll > -20)
	{
	act("$n screams at you 'You kill my brodda! Now you pay!'", ch, NULL, victim, TO_VICT);
	act("You scream at $N 'You kill my brodda! Now you pay!'{x", ch, NULL, victim, TO_CHAR);
	act("$n screams at $N 'You kill my brodda! Now you pay!'{x", ch, NULL, victim, TO_NOTVICT);
	dam = ch->level;
	}
	else if (roll > -40)
	{
	act("$n screams at you 'You rape my gran fadda! Now you pay!'", ch, NULL, victim, TO_VICT);
	act("You scream at $N 'You rape my gran fadda! Now you pay!!{x", ch, NULL, victim, TO_CHAR);
	act("$n screams at $N 'You rape my gran fadda! Now you pay!!{x", ch, NULL, victim, TO_NOTVICT);
	dam = ch->level;
	}
	else
	{
	act("$n leaps high in the air and strikes you in the knee cap!", ch, NULL, victim, TO_VICT);
	act("You leap high in the air and strike $N in the knee cap!{x", ch, NULL, victim, TO_CHAR);
	act("$n leaps high in the air and strikes $N's in the knee cap!{x", ch, NULL, victim, TO_NOTVICT);
	dam = ch->level / 2;
	}

	damage(ch, victim,dam, gsn_strike, DAM_BASH, TRUE);
	check_improve(ch, gsn_strike, TRUE, 1);

    }
    else
    {
	damage(ch, victim, 0, gsn_strike, DAM_BASH, TRUE);
	check_improve(ch, gsn_strike, FALSE, 1);
    }
    check_killer(ch, victim);
    return;
}
/* Getting the door for a single argument char */
int get_that_door(char arg[0])
{
	if (arg[0] == 'n')
		return DIR_NORTH;
	if (arg[0] == 's')
		return DIR_SOUTH;
	if (arg[0] == 'e')
		return DIR_EAST;
	if (arg[0] == 'w')
		return DIR_WEST;
	if (arg[0] == 'd')
		return DIR_DOWN;
	if (arg[0] == 'u')
		return DIR_UP;
	return -1;
}
void do_step(CHAR_DATA * ch, char *argument)
{

ROOM_INDEX_DATA *in_room;
ROOM_INDEX_DATA *to_room;
EXIT_DATA *pexit;
CHAR_DATA *rch;
char arg[MAX_INPUT_LENGTH];
int do1 = -1;
int do2 = -1;
int do3 = -1;
int do4 = -1;
int do5 = -1;
char door1[0];
char door2[0];
char door3[0];
char door4[0];
char door5[0];

argument = one_argument(argument, arg);




    if (IS_AFFECTED(ch, AFF_BLIND)) {
	send_to_char("You can't see where you're stepping!\n\r", ch);
	return;
    }

if ((get_skill(ch, gsn_step) == 0
	|| ch->level < skill_table[gsn_step].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    if (!IS_NPC(ch)
	&& ch->level < skill_table[gsn_step].skill_level[ch->class]) {
	send_to_char("Huh?\n\r", ch);
	return;
    }


if (arg[0] == '\0') {
    send_to_char("Step in which way how many where?\n\r", ch);
    return;
    }

/*******************************************************
*  MESSY CODE ALERT                                    *
*  Until I fix0r this, it will be messy, live with it. *
*                                                      *
********************************************************/
/* Step 1 */

door1[0] = arg[0];
do1 = get_that_door(door1);

if(do1 == -1)
{
send_to_char("Move that way? Ha, try again.\n\r", ch);
return;
}

      in_room = ch->in_room;
/* Just in case... */

      if ((pexit = in_room->exit[do1]) == NULL
	|| (to_room = pexit->u1.to_room) == NULL) {
	send_to_char("Step where there isn't an exit? Sure.\n\r", ch);
	return;
    }

/* Checking for guards! Can't step past THOSE */

	for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room) {

	    if (IS_NPC(rch) && rch->spec_fun != NULL) {

		if (GET_SIDE(ch) != SIDE_TIER
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_tier")) {
		    act("$N prevents you from stepping away!!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_TIER
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_tier2")) {
		    act("$N prevents you from steoping away!!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_KAEL
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_kael")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_UM
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_evil")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_CAIRN
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_cairn")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;
		}
       }
}

  if(ch->move > 50)
   {
  ch->move -= 50;
   }
  else {
  send_to_char("You need at least 50 moves to step!\n\r", ch);
  return;
  }

/* I think that's all the checks I need to make for the first step! */

act("$n steps away in a blur!", ch, NULL, NULL, TO_ROOM);
send_to_char("You step away in a blur!\n\r", ch);
char_from_room(ch);
char_to_room(ch, to_room);

door2[0] = arg[1];
do2 = get_that_door(door2);

if(do2 == -1)
{
act("$n appears in a blur!", ch, NULL, NULL, TO_ROOM);
do_look(ch, "auto");
return;
}

/* Step # 2 */

      in_room = ch->in_room;

/* Just in case... */

      if ((pexit = in_room->exit[do2]) == NULL
	|| (to_room = pexit->u1.to_room) == NULL) {
	send_to_char("You slam face first into a wall, expecting an exit.\n\r", ch);
	act("$n slams face first into a wall expecting an exit!", ch, NULL, NULL, TO_ROOM);
	do_look(ch, "auto");
	return;
    }

/* Checking for guards! Can't step past THOSE */

	for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room) {

	    if (IS_NPC(rch) && rch->spec_fun != NULL) {

		if (GET_SIDE(ch) != SIDE_TIER
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_tier")) {
		    act("$N prevents you from stepping away!!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_TIER
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_tier2")) {
		    act("$N prevents you from stepping away!!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_KAEL
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_kael")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_UM
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_evil")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_CAIRN
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_cairn")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;
		}
       }
}

  if(ch->move > 50)
   {
  ch->move -= 50;
   }
  else {
  send_to_char("You drop out of your step!\n\r", ch);
  act("$n phases in front of you, looking rather tired!", ch, NULL, NULL, TO_ROOM);
  do_look(ch, "auto");
  return;
  }

act("$n blurs right past you!", ch, NULL, NULL, TO_ROOM);
char_from_room(ch);
char_to_room(ch, to_room);
door3[0] = arg[2];
do3 = get_that_door(door3);

if (do3 == -1)
{
act("$n appears in a blur!", ch, NULL, NULL, TO_ROOM);
do_look(ch, "auto");
return;
}

/* Step # 3 */

      in_room = ch->in_room;

/* Just in case... */

      if ((pexit = in_room->exit[do3]) == NULL
	|| (to_room = pexit->u1.to_room) == NULL) {
	send_to_char("You slam face first into a wall, expecting an exit.\n\r", ch);
	act("$n slams face first into a wall expecting an exit!", ch, NULL, NULL, TO_ROOM);
	do_look(ch, "auto");
	return;
    }

/* Checking for guards! Can't step past THOSE */

	for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room) {

	    if (IS_NPC(rch) && rch->spec_fun != NULL) {

		if (GET_SIDE(ch) != SIDE_TIER
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_tier")) {
		    act("$N prevents you from stepping away!!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_TIER
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_tier2")) {
		    act("$N prevents you from stepping away!!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_KAEL
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_kael")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_UM
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_evil")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_CAIRN
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_cairn")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;
		}
       }
}

  if(ch->move > 50)
   {
  ch->move -= 50;
   }
  else {
  send_to_char("You drop out of your step!\n\r", ch);
  act("$n phases in front of you, looking rather tired!", ch, NULL, NULL, TO_ROOM);
  do_look(ch, "auto");
  return;
  }

act("$n blurs right past you!", ch, NULL, NULL, TO_ROOM);
char_from_room(ch);
char_to_room(ch, to_room);

door4[0] = arg[3];
do4 = get_that_door(door4);

if (do4 == -1)
{
act("$n appears in a blur!", ch, NULL, NULL, TO_ROOM);
do_look(ch, "auto");
return;
}

/* Step # 4 */

      in_room = ch->in_room;

/* Just in case... */

      if ((pexit = in_room->exit[do4]) == NULL
	|| (to_room = pexit->u1.to_room) == NULL) {
	send_to_char("You slam face first into a wall, expecting an exit.\n\r", ch);
	act("$n slams face first into a wall expecting an exit!", ch, NULL, NULL, TO_ROOM);
	do_look(ch, "auto");
	return;
    }

/* Checking for guards! Can't step past THOSE */

	for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room) {

	    if (IS_NPC(rch) && rch->spec_fun != NULL) {

		if (GET_SIDE(ch) != SIDE_TIER
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_tier")) {
		    act("$N prevents you from stepping away!!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_TIER
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_tier2")) {
		    act("$N prevents you from stepping away!!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_KAEL
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_kael")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_UM
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_evil")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_CAIRN
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_cairn")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;
		}
       }
}

  if(ch->move > 50)
   {
  ch->move -= 50;
   }
  else {
  send_to_char("You drop out of your step!\n\r", ch);
  act("$n phases in front of you, looking rather tired!", ch, NULL, NULL, TO_ROOM);
  do_look(ch, "auto");
  return;
  }

act("$n blurs right past you!", ch, NULL, NULL, TO_ROOM);
char_from_room(ch);
char_to_room(ch, to_room);

door5[0] = arg[4];
do5 = get_that_door(door5);

if (do5 == -1)
{
act("$n appears in a blur!", ch, NULL, NULL, TO_ROOM);
do_look(ch, "auto");
return;
}

/* Step # 5! */

      in_room = ch->in_room;

/* Just in case... */

      if ((pexit = in_room->exit[do5]) == NULL
	|| (to_room = pexit->u1.to_room) == NULL) {
	send_to_char("You slam face first into a wall, expecting an exit.\n\r", ch);
	act("$n slams face first into a wall expecting an exit!", ch, NULL, NULL, TO_ROOM);
	do_look(ch, "auto");
	return;
    }

/* Checking for guards! Can't step past THOSE */

	for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room) {

	    if (IS_NPC(rch) && rch->spec_fun != NULL) {

		if (GET_SIDE(ch) != SIDE_TIER
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_tier")) {
		    act("$N prevents you from stepping away!!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_TIER
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_tier2")) {
		    act("$N prevents you from stepping away!!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_KAEL
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_kael")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_UM
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_evil")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;

		}

		if (GET_SIDE(ch) != SIDE_CAIRN
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_cairn")) {
		    act("$N blocks you from stepping away!", ch, NULL, 
rch, TO_CHAR);
		    return;
		}
       }
}

  if(ch->move > 75)
   {
  ch->move -= 75;
   }
  else {
  send_to_char("You drop out of your step!\n\r", ch);
  act("$n phases in front of you, looking rather tired!", ch, NULL, NULL, TO_ROOM);
  do_look(ch, "auto");
  return;
  }

act("$n blurs right past you!", ch, NULL, NULL, TO_ROOM);
char_from_room(ch);
char_to_room(ch, to_room);
act("$n appears in a blur!", ch, NULL, NULL, TO_ROOM);
do_look(ch, "auto");


/* The End */
return;
}

void do_rush(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int chance = 0;

    one_argument(argument, arg);

    if ((chance = get_skill(ch, gsn_rush)) == 0
	|| (!IS_NPC(ch)
	    && ch->level < skill_table[gsn_rush].skill_level[ch->class])) {
	send_to_char("Rush? What's that?\n\r", ch);
	return;
    }
    if (MOUNTED(ch)) {
	send_to_char("Wouldn't a rush while mounted be a charge?\n\r", ch);
	return;
    }
    if (arg[0] == '\0') {
	send_to_char("Rush whom?\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }

    if (ch->fighting != NULL) {
	send_to_char("You are too busy fighting to gain momentum.\n\r", 
ch);
	return;
    }
    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {
	send_to_char("You need to wield a weapon to make rush effective.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("How can you rush yourself?\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	act("But $N is your friend!", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (victim->fighting != NULL) {
	send_to_char("Your target will not hold still!\n\r", ch);
	return;
    }
    if (ch->move < 75)
     {
      send_to_char("You need 100 moves to rush!\n\r", ch);
      return;
     }

     act("$n lowers his weapon and rushes straight at you!", ch, NULL, victim, TO_VICT);
     act("You lower your weapon and rush headlong into battle!", ch, NULL, victim, TO_CHAR);
     act("$n catches $N in a rush!", ch, NULL, victim, TO_NOTVICT);
    chance += get_skill(ch, gsn_rush) * 3/4;
    chance += get_curr_stat(ch, STAT_STR) /5;
    chance -= get_curr_stat(victim, STAT_DEX) /2; 
    if (number_percent() < chance) {
	check_improve(ch, gsn_rush, TRUE, 1);
	DAZE_STATE(victim, 2 * PULSE_VIOLENCE);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 3/2 * PULSE_VIOLENCE); }
	victim->position = POS_RESTING;
	one_hit(ch, victim, gsn_rush, FALSE);
      ch->move -= 100;
    } else {
	damage(ch, victim, 0, gsn_rush, DAM_BASH, FALSE);
	check_improve(ch, gsn_rush, FALSE, 1);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, 2 * PULSE_VIOLENCE); }
      ch->move -= 100;
    }

    check_killer(ch, victim);
}

void do_shieldbash(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int chance;

    one_argument(argument, arg);

    if ((chance = get_skill(ch, gsn_shieldbash)) == 0
	|| (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_BASH))
	|| (!IS_NPC(ch)
	    && ch->level < skill_table[gsn_shieldbash].skill_level[ch->class])) {
	send_to_char("What? Shield bash? You? HA!\n\r", ch);
	return;
    }
    if ((obj = get_eq_char(ch, WEAR_SHIELD)) == NULL)
    {
     send_to_char("A shield bash without a shield, would be a bash!\n\r", ch);
     return;
    }
    if (arg[0] == '\0') {
	victim = ch->fighting;
	if (victim == NULL) {
	    send_to_char("But you aren't fighting anyone!\n\r", ch);
	    return;
	}
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (MOUNTED(ch)) {
	send_to_char("You can't shield bash while riding!\n\r", ch);
	return;
    }
    if (MOUNTED(victim)) {
	send_to_char("They are riding something, so you can't shield bash them.\n\r", ch);
	return;
    }
    if (victim->position < POS_FIGHTING) {
	act("You'll have to let $M get back up first.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (victim == ch) {
	send_to_char("You smack your self with your shield.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	act("But $N is your friend!", ch, NULL, victim, TO_CHAR);
	return;
    }
    /* modifiers */

    /* size  and weight */
    chance += ch->carry_weight / 250;
    chance -= victim->carry_weight / 200;

    if (ch->size < victim->size)
	chance += (ch->size - victim->size) * 15;
    else
	chance += (ch->size - victim->size) * 10;

    /* stats */
    chance += get_curr_stat(ch, STAT_STR);
    chance -= (get_curr_stat(victim, STAT_DEX) * 4) / 3;
    chance -= GET_AC(victim, AC_BASH) / 25;
    /* speed */
    if (IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags, OFF_FAST) || IS_AFFECTED(victim, AFF_HASTE))
	chance -= 30;

    /* level */
    chance += (ch->level - victim->level);

    if (!IS_NPC(victim)
	&& chance < get_skill(victim, gsn_dodge)) {	/*
	   act("$n tries slam his shield into you, but you dodge it.",ch,NULL,victim,TO_VICT);
	   act("$N dodges your bash, you fall flat on your face.",ch,NULL,victim,TO_CHAR);
							   WAIT_STATE(ch,skill_table[gsn_bash].beats);
							   return; */
	chance -= 3 * (get_skill(victim, gsn_dodge) - chance);
    }
    /* now the attack */
    if (number_percent() < chance) {

	act("$n smashes into you with his shield!",
	    ch, NULL, victim, TO_VICT);
	act("You smack $N with your shield!", ch, NULL, victim, TO_CHAR);
	act("$n slams into $N wish his shield!",
	    ch, NULL, victim, TO_NOTVICT);
	check_improve(ch, gsn_shieldbash, TRUE, 1);

	DAZE_STATE(victim, 3 * PULSE_VIOLENCE);
	if (!IS_IMMORTAL(ch))
	{
	WAIT_STATE(ch, skill_table[gsn_shieldbash].beats);
	}
/* Stupid idea. --Tasydra
	ch->position = POS_RESTING;
*/
	//damage(ch, victim, ((4 * get_obj_weight(obj)) / 5)*5, gsn_shieldbash, DAM_BASH, TRUE);
	damage(ch, victim, number_range(180,240),gsn_shieldbash, DAM_BASH, TRUE);

	if (RIDDEN(victim)) {
	    mount_success(RIDDEN(victim), victim, FALSE);
	}
    } else {
	damage(ch, victim, 0, gsn_bash, DAM_BASH, FALSE);
	act("You fall flat on your face!",
	    ch, NULL, victim, TO_CHAR);
	act("$n falls flat on $s face.",
	    ch, NULL, victim, TO_NOTVICT);
	act("You evade $n's shield bash, causing $m to fall flat on $s face.",
	    ch, NULL, victim, TO_VICT);
	check_improve(ch, gsn_shieldbash, FALSE, 1);
/* Stupid idea. --Tasydra
	ch->position = POS_RESTING;
*/
if (!IS_IMMORTAL(ch))
{ WAIT_STATE(ch, skill_table[gsn_shieldbash].beats * 3 / 2); }
    }
    check_killer(ch, victim);
}

void do_skewer(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if ((get_skill(ch, gsn_skewer) == 0 || ch->level < skill_table[gsn_skewer].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }
    if (!IS_NPC(ch)
	&& ch->level < skill_table[gsn_skewer].skill_level[ch->class]) {
	send_to_char("Skewer? Hmm, nope, sorry.\n\r", ch);
	return;
    }

    if (arg[0] == '\0') {
	victim = ch->fighting;
	if (victim == NULL) {
	    send_to_char("Skewer whom?\n\r", ch);
	    return;
	}
     } else if ((victim = get_char_room(ch, arg)) == NULL) {
                 send_to_char("They aren't here.\n\r", ch);
                 return;
    }

    if (MOUNTED(ch)) {
	send_to_char("You can't Skewer while riding!\n\r", ch);
	return;
    }

    if (victim == ch) {
	send_to_char("How can you skewer yourself? (Please don't answer)\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL || obj->value[0] != WEAPON_POLEARM ) {
	send_to_char("You need to wield POLEARM to skewer.\n\r", ch);
	return;
    }
if (!IS_IMMORTAL(ch))
{ WAIT_STATE(ch, skill_table[gsn_skewer].beats); }

     act("$n lunges at you!", ch, NULL, victim, TO_VICT);
     act("You lunge at $N!", ch, NULL, victim, TO_CHAR);
     act("$n lunges toward $N!", ch, NULL, victim, TO_NOTVICT);

    if (number_percent() < (get_skill(ch, gsn_skewer)))
    {
	check_improve(ch, gsn_skewer, TRUE, 1);
	one_hit(ch, victim, gsn_skewer, FALSE);
    } else {
	check_improve(ch, gsn_skewer, FALSE, 1);
	damage(ch, victim, 0, gsn_skewer, DAM_NONE, TRUE);
    }
    check_killer(ch, victim);
}

void do_throw(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if ((get_skill(ch, gsn_throw) == 0 || ch->level < skill_table[gsn_throw].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }
    if (!IS_NPC(ch)
	&& ch->level < skill_table[gsn_throw].skill_level[ch->class]) {
	send_to_char("Throw? With your weak arm?\n\r", ch);
	return;
    }
    if (arg[0] == '\0') {
	victim = ch->fighting;
	if (victim == NULL) {
	    send_to_char("Throw at whom?!\n\r", ch);
	    return;
	}
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }

    if (victim == ch) {
	send_to_char("Throw a spear at your self. Sure. Why not?\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL || obj->value[0] != WEAPON_SPEAR ) {
	send_to_char("You can only throw a SPEAR that is EQUIPED.\n\r", ch);
	return;
    }

if (!IS_IMMORTAL(ch))
{ WAIT_STATE(ch, skill_table[gsn_throw].beats); }
     /* He shoots! */

	act("Something is suddenly flying at you...", ch, NULL, victim, TO_VICT);
	act("You launch your spear!", ch, NULL, victim, TO_CHAR);
	act("$n aims at $N and lets fly!", ch, NULL, victim, TO_NOTVICT);


/* Doh, target caught it */
    if ( ( get_skill(ch, gsn_throw) ) -5 < ( get_skill(victim, gsn_dodge) -(number_range(1,25)) ) )
     {
	act("You catch a spear in mid air!", ch, NULL, victim, TO_VICT);
	act("$N catches your spear in mid air!", ch, NULL, victim, TO_CHAR);
	act("$N catches $n's spear mid air!", ch, NULL, victim, TO_NOTVICT);
	  obj_from_char(obj);
      obj_to_char(obj, victim);
      check_killer(ch, victim);
      return;
     }
   /* He scores! */
   if (number_percent() < (get_skill(ch, gsn_throw)* 2/3))
    {
	check_improve(ch, gsn_throw, TRUE, 1);
	one_hit(ch, victim, gsn_throw, FALSE);
	obj_from_char(obj);
    obj_to_room(obj, ch->in_room);
    } else { /* Or does he? */
	act("$n's spear goes wide and misses you!", ch, NULL, victim, TO_VICT);
	act("Your spear goes wide and misses $N!", ch, NULL, victim, TO_CHAR);
	act("$n's spear goes wide and misses $N!", ch, NULL, victim, TO_NOTVICT);
    obj_from_char(obj);
    obj_to_room(obj, ch->in_room);
    }
    check_killer(ch, victim);
}

void do_jump(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    AFFECT_DATA af;

    if (!IS_OUTSIDE(ch)) {
	send_to_char("You must be out of doors.\n\r", ch);
	return;
    }
    
    one_argument(argument, arg);

    if ((get_skill(ch, gsn_jump) == 0 || ch->level < skill_table[gsn_jump].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }
    if (!IS_NPC(ch)
	&& ch->level < skill_table[gsn_jump].skill_level[ch->class]) {
	send_to_char("You jump a couple of feet in the air.\n\r", ch);
	return;
    }
    if (arg[0] == '\0') {
	send_to_char("Jump whom?\n\r", ch);
	return;
    }
    if (MOUNTED(ch)) {
	send_to_char("What! You intend on jumping off your mount?\n\r", ch);
	return;
    }
    if (ch->fighting != NULL) {
	send_to_char("You'd have an awful time jumping now!\n\r", ch);
	return;
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("You want to jump on yourself?\n\r", ch);
	return;
    }
    /*if (victim->fighting != NULL) {
	send_to_char("You could not get high enough.\n\r", ch);
	return;
    }*/
    if (victim == ch) {
	send_to_char("How are you going to jump yourself?\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {
	send_to_char("You wouldn't hurt your foe without a weapon.\n\r", ch);
	return;
    }
if (!IS_IMMORTAL(ch))
{ WAIT_STATE(ch, skill_table[gsn_jump].beats); }
    if (number_percent() < get_skill(ch, gsn_jump)
	&& (get_skill(ch, gsn_jump) >= 2)) {
	check_improve(ch, gsn_jump, TRUE, 1);
	send_to_char("You leap high into the air, where attacks cannot harm you.\n\r",ch);
				af.where = TO_AFFECTS;
    			af.type = gsn_jump;
    			af.level = 1;
    			af.duration = 0;
    			af.location = APPLY_WIS;
    			af.modifier = 2;
    			af.bitvector = 0;
    			affect_to_char(ch, &af);
    			ch->fighting = victim;

	//one_hit(ch, victim, gsn_jump, FALSE);
    } else {
	    send_to_char("You can't even jump off right..\n\r",ch);
	check_improve(ch, gsn_jump, FALSE, 1);
	damage(ch, victim, 0, gsn_jump, DAM_NONE, TRUE);
    }

    check_killer(ch, victim);
    return;
}

void do_weapon_break(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *wield;
    
    one_argument(argument, arg);

   if ((ch->level < skill_table[gsn_weapon_break].skill_level[ch->class])
       || get_skill(ch, gsn_weapon_break) <= 0) 
       {
			send_to_char("You don't possess that ability.\n\r", ch);
			return;
  	   }
    
    if (arg[0] == '\0') {
	victim = ch->fighting;
	if (victim == NULL) {
	    send_to_char("But you aren't fighting anyone!\n\r", ch);
	    return;
	}
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }

    if (ch->fighting == NULL) {
	send_to_char("But you aren't fighting anyone!\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("That would be stupid.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;
	
	if (ch->move < 30) {
	send_to_char("You need at least 30 moves to use this skill!\n\r",ch);
	return;
}

if (get_eq_char(victim, WEAR_WIELD) == NULL) 
	{ 
		send_to_char("You can't break weapons that don't exist.\n\r",ch);
 		ch->move -= 30;
 		return;
	}
	
	wield = get_eq_char(victim, WEAR_WIELD);
    check_killer(ch, victim);
    WAIT_STATE(ch, skill_table[gsn_weapon_break].beats);
    int chanceroll = 0; //New Weapon Break    
    int chanceskill = 0; //for purposes
    
    chanceroll = number_percent();
    chanceskill = get_skill(ch, gsn_weapon_break) - chanceroll;
    
    
    
    if (chanceskill >= 92) 
    {
		check_improve(ch, gsn_weapon_break, TRUE, 1);
	    wield->condition -= 90;
	    act("{RSparks fly from $n's weapon! ***{X", victim, NULL, NULL, TO_NOTVICT);
        act("{R$n targets your weapon, attempting to break it!{X", ch, NULL, victim, TO_VICT);
    }
    else if (chanceskill >= 80)
    {
	    check_improve(ch, gsn_weapon_break, TRUE, 1);
	    wield->condition -= 50;
	    act("{YSparks fly from $n's weapon! **{X", victim, NULL, NULL, TO_NOTVICT);
        act("{Y$n targets your weapon, attempting to break it!{X", ch, NULL, victim, TO_VICT);
    }
    else if (chanceskill >= 30)
    {
	    check_improve(ch, gsn_weapon_break, TRUE, 1);
	    wield->condition -= 35;
	    act("{GSparks fly from $n's weapon!{X *", victim, NULL, NULL, TO_NOTVICT);
        act("{G$n targets your weapon, attempting to break it!{X", ch, NULL, victim, TO_VICT);
    }
    else if (chanceskill >= 10)
    {
	    check_improve(ch, gsn_weapon_break, TRUE, 1);
	    wield->condition -= 20;
	    act("Sparks fly from $n's weapon!", victim, NULL, NULL, TO_NOTVICT);
        act("$n targets your weapon, attempting to break it!", ch, NULL, victim, TO_VICT);
    }
    
    else 
    {
		send_to_char("You missed!\n\r",ch);
		check_improve(ch, gsn_weapon_break, FALSE, 1);
		damage(ch, victim, 0, gsn_weapon_break, DAM_NONE, TRUE);
	}
	
	if (wield->condition < 0)
	wield->condition = 0;
	
	ch->move -= 30;
    return;
}

void do_tear( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA *paf;
    CHAR_DATA *victim;
    int chance = 0;
    char arg[MAX_INPUT_LENGTH];
            
    one_argument(argument, arg);
 
        if (get_skill(ch, gsn_tear) < 1)
    {
	    send_to_char("You don't know how to do that.\n\r",ch);
	    return;
    }
    
   
      if (arg[0] == '\0') {
	send_to_char("Tear the entanglement of whom?\n\r", ch);
	return;
    }
    
    
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    
    if (victim == ch){
	    send_to_char("Why not struggle instead?\n\r",ch);
	    return;
    }
    
    paf = victim->affected;

    if (paf == NULL) {
	send_to_char("They are not stuck!\n\r", ch);
	return;
    }
    
    while (paf->bitvector != AFF_WEB && paf->next != NULL)
	paf = paf->next;

    if (paf->bitvector != AFF_WEB) {
	send_to_char("They are not stuck!\n\r", ch);
	return;
    }
    
    check_improve(ch, gsn_tear, FALSE, 2);

    chance += number_range(0, 70);
    chance += get_curr_stat(ch, STAT_DEX) / 8;
    chance += get_curr_stat(ch, STAT_STR) / 3;
    chance += get_skill(ch,gsn_tear)/5;

    if (chance >= 95)
    {
	affect_remove(ch, paf);
	WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
	act("$You tear the entanglement of $N!", ch, ch, ch, TO_VICT);
	act("$n tears the net around you to shreds!", victim, victim, victim, TO_CHAR);
        }

    else if (chance < 95 && chance >= 80)
    	{
        if (paf->duration > 1)
           {
	   paf->duration--;
	   paf->duration--;
	   WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
	act("$n loosens the net holding you!", ch, victim, victim, TO_VICT);
	act("You loosen the entanglement of $N!", ch, ch, victim, TO_CHAR);
   			}
	   else
	    	{
		   affect_remove(ch, paf);
	WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
	act("$n loosens the net holding you!", ch, ch, ch, TO_VICT);
	act("You loosen the entanglement of $N!", victim, victim, victim, TO_CHAR);
			}
	   
		}
		
	else if (chance < 80 && chance >= 45)
		{
        if (paf->duration > 0)
           {
	   paf->duration--;
	   WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
	 act("$n loosens the net holding you!", ch, ch, ch, TO_VICT);
	act("You loosen the entanglement of $N!", victim, victim, victim, TO_CHAR);
   			}
	   else
	    	{
		   affect_remove(ch, paf);
	WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
	act("$n loosens the net holding you!", ch, ch, ch, TO_VICT);
	act("You loosen the entanglement of $N!", victim, victim, victim, TO_CHAR);
			}
	   
		}
		
	else if (chance < 20)
	{
		send_to_char("You failed to loosen the net.\n\r",ch);
		return;
	}
}

void do_braver(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if ((!IS_NPC(ch) && ch->level < skill_table[gsn_braver].skill_level[ch->class])
       || get_skill(ch, gsn_braver) <= 0) {
	send_to_char(
			"You aren't very brave.\n\r", ch);
	return;
    }

if (arg[0] == '\0') {
        victim = ch->fighting;
        if (victim == NULL) {
            send_to_char("But you aren't fighting anyone!\n\r", ch);
            return;
        }  
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }

    if (MOUNTED(ch)) {
	send_to_char("You can't braver while riding!\n\r", ch);
	return;
    }
    if (ch->fighting == NULL) {
	send_to_char("But you aren't fighting anyone!.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("You are starting to get dizzy.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {
	send_to_char("You need to wield a weapon to use braver.\n\r", ch);
	return;
    }
    check_killer(ch, victim);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, skill_table[gsn_braver].beats); }
    if (number_percent() < get_skill(ch, gsn_braver)) {
	check_improve(ch, gsn_braver, TRUE, 1);

	one_hit(ch, victim, gsn_braver, FALSE);

    } else {
	check_improve(ch, gsn_braver, FALSE, 1);
	damage(ch, victim, 0, gsn_braver, DAM_NONE, TRUE);
    }

    return;
}

void do_cross_slash(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if ((!IS_NPC(ch) && ch->level < skill_table[gsn_cross_slash].skill_level[ch->class])
       || get_skill(ch, gsn_cross_slash) <= 0) {
	send_to_char(
			"You aren't very brave.\n\r", ch);
	return;
    }

if (arg[0] == '\0') {
        victim = ch->fighting;
        if (victim == NULL) {
            send_to_char("But you aren't fighting anyone!\n\r", ch);
            return;
        }  
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }

    if (MOUNTED(ch)) {
	send_to_char("You can't cross_slash while riding!\n\r", ch);
	return;
    }
    if (ch->fighting == NULL) {
	send_to_char("But you aren't fighting anyone!.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("You are starting to get dizzy.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {
	send_to_char("You need to wield a weapon to use cross_slash.\n\r", ch);
	return;
    }
    check_killer(ch, victim);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, skill_table[gsn_cross_slash].beats); }
    if (number_percent() < get_skill(ch, gsn_cross_slash)) {
	check_improve(ch, gsn_cross_slash, TRUE, 1);

	one_hit(ch, victim, gsn_cross_slash, FALSE);
	if (victim->in_room == ch->in_room)
	one_hit(ch, victim, gsn_cross_slash, FALSE);
	if (victim->in_room == ch->in_room)
	one_hit(ch, victim, gsn_cross_slash, FALSE);

    } else {
	check_improve(ch, gsn_cross_slash, FALSE, 1);
	damage(ch, victim, 0, gsn_cross_slash, DAM_NONE, TRUE);
    }

    return;
}

void do_blade_beam(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    CHAR_DATA *vch, *vch_next;

    one_argument(argument, arg);

    if ((!IS_NPC(ch) && ch->level < skill_table[gsn_blade_beam].skill_level[ch->class])
       || get_skill(ch, gsn_blade_beam) <= 0) {
	send_to_char(
			"You aren't very brave.\n\r", ch);
	return;
    }

if (arg[0] == '\0') {
        victim = ch->fighting;
        if (victim == NULL) {
            send_to_char("But you aren't fighting anyone!\n\r", ch);
            return;
        }  
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }

    if (MOUNTED(ch)) {
	send_to_char("You can't blade_beam while riding!\n\r", ch);
	return;
    }
    if (ch->fighting == NULL) {
	send_to_char("But you aren't fighting anyone!.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("You are starting to get dizzy.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {
	send_to_char("You need to wield a weapon to use blade_beam.\n\r", ch);
	return;
    }
    check_killer(ch, victim);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, skill_table[gsn_blade_beam].beats); }
    if (number_percent() < get_skill(ch, gsn_blade_beam)) {
	check_improve(ch, gsn_blade_beam, TRUE, 1);

		
	for (vch = victim->in_room->people; vch != NULL; vch = vch_next) {
			vch_next = vch->next_in_room;
			if (vch->side != ch->side){
	
		if (vch == victim)
			damage(ch, vch, number_fuzzy(victim->hit/12), gsn_blade_beam, DAM_SLASH, TRUE);
		else
			damage(ch, vch, number_fuzzy(victim->hit/24), gsn_blade_beam, DAM_SLASH, TRUE);
		}
			

    }} else {
	check_improve(ch, gsn_blade_beam, FALSE, 1);
	damage(ch, victim, 0, gsn_blade_beam, DAM_NONE, TRUE);
    }

    return;
}

void do_climhazzard(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if ((!IS_NPC(ch) && ch->level < skill_table[gsn_climhazzard].skill_level[ch->class])
       || get_skill(ch, gsn_climhazzard) <= 0) {
	send_to_char(
			"You aren't very brave.\n\r", ch);
	return;
    }

if (arg[0] == '\0') {
        victim = ch->fighting;
        if (victim == NULL) {
            send_to_char("But you aren't fighting anyone!\n\r", ch);
            return;
        }  
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }

    if (MOUNTED(ch)) {
	send_to_char("You can't climhazzard while riding!\n\r", ch);
	return;
    }
    if (ch->fighting == NULL) {
	send_to_char("But you aren't fighting anyone!.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("You are starting to get dizzy.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {
	send_to_char("You need to wield a weapon to use climhazzard.\n\r", ch);
	return;
    }
    check_killer(ch, victim);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, skill_table[gsn_climhazzard].beats); }
    if (number_percent() < get_skill(ch, gsn_climhazzard)) {
	check_improve(ch, gsn_climhazzard, TRUE, 1);

	if (ch->hit >= 500)
	damage(ch, victim, number_fuzzy(100000/victim->hit), gsn_climhazzard, DAM_SLASH, TRUE);
	else
	damage(ch, victim, number_fuzzy(5000/victim->hit), gsn_climhazzard, DAM_SLASH, TRUE);

    } else {
	check_improve(ch, gsn_climhazzard, FALSE, 1);
	damage(ch, victim, 0, gsn_climhazzard, DAM_NONE, TRUE);
    }

    return;
}

void do_meteorain(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    CHAR_DATA *vch, *vch_next;

    one_argument(argument, arg);

    if ((!IS_NPC(ch) && ch->level < skill_table[gsn_meteorain].skill_level[ch->class])
       || get_skill(ch, gsn_meteorain) <= 0) {
	send_to_char(
			"You aren't very brave.\n\r", ch);
	return;
    }

if (arg[0] == '\0') {
        victim = ch->fighting;
        if (victim == NULL) {
            send_to_char("But you aren't fighting anyone!\n\r", ch);
            return;
        }  
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }

    if (MOUNTED(ch)) {
	send_to_char("You can't meteorain while riding!\n\r", ch);
	return;
    }
    if (ch->fighting == NULL) {
	send_to_char("But you aren't fighting anyone!.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("You are starting to get dizzy.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {
	send_to_char("You need to wield a weapon to use meteorain.\n\r", ch);
	return;
    }
    check_killer(ch, victim);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, skill_table[gsn_meteorain].beats); }
    if (number_percent() < get_skill(ch, gsn_meteorain)-10) {
	check_improve(ch, gsn_meteorain, TRUE, 1);

		
	for (vch = victim->in_room->people; vch != NULL; vch = vch_next) {
			vch_next = vch->next_in_room;
			
			if (IS_NPC(vch))
			continue;

			if (victim->in_room == ch->in_room)
			one_hit(ch, victim, gsn_meteorain, FALSE);
			

    }} else {
	check_improve(ch, gsn_meteorain, FALSE, 1);
	damage(ch, victim, 0, gsn_meteorain, DAM_NONE, TRUE);
    }

    return;
}

void do_finishing_touch(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if ((!IS_NPC(ch) && ch->level < skill_table[gsn_finishing_touch].skill_level[ch->class])
       || get_skill(ch, gsn_finishing_touch) <= 0) {
	send_to_char(
			"You aren't very brave.\n\r", ch);
	return;
    }

if (arg[0] == '\0') {
        victim = ch->fighting;
        if (victim == NULL) {
            send_to_char("But you aren't fighting anyone!\n\r", ch);
            return;
        }  
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }

    if (MOUNTED(ch)) {
	send_to_char("You can't finishing_touch while riding!\n\r", ch);
	return;
    }
    if (ch->fighting != NULL) {
	send_to_char("You cannot use this skill between rounds.\n\r", ch);
	return;
	}
    if (victim == ch) {
	send_to_char("You are starting to get dizzy.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {
	send_to_char("You need to wield a weapon to use finishing touch.\n\r", ch);
	return;
    }
    check_killer(ch, victim);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, skill_table[gsn_finishing_touch].beats); }
    if (number_percent() < get_skill(ch, gsn_finishing_touch)) {
	check_improve(ch, gsn_finishing_touch, TRUE, 1);

	damage(ch, victim, number_fuzzy(200), gsn_finishing_touch, DAM_SLASH, TRUE);
	ch->fighting = NULL;
	if (victim->in_room == ch->in_room)
	spell_web(skill_lookup("web"), ch->level, ch, victim, TARGET_CHAR);
	if (victim->in_room == ch->in_room)
	spell_weaken(skill_lookup("weaken"), ch->level, ch, victim, TARGET_CHAR);
	if (victim->in_room == ch->in_room)
	spell_blindness(skill_lookup("blind"), ch->level, ch, victim, TARGET_CHAR);
	ch->fighting = victim;

    } else {
	check_improve(ch, gsn_finishing_touch, FALSE, 1);
	damage(ch, victim, 0, gsn_finishing_touch, DAM_NONE, TRUE);
    }

    return;
}

void do_omnislash(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if ((!IS_NPC(ch) && ch->level < skill_table[gsn_omnislash].skill_level[ch->class])
       || get_skill(ch, gsn_omnislash) <= 0) {
	send_to_char(
			"You aren't very brave.\n\r", ch);
	return;
    }

if (arg[0] == '\0') {
        victim = ch->fighting;
        if (victim == NULL) {
            send_to_char("But you aren't fighting anyone!\n\r", ch);
            return;
        }  
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }

    if (MOUNTED(ch)) {
	send_to_char("You can't omnislash while riding!\n\r", ch);
	return;
    }
    if (ch->fighting == NULL) {
	send_to_char("But you aren't fighting anyone!.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("You are starting to get dizzy.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {
	send_to_char("You need to wield a weapon to use omnislash.\n\r", ch);
	return;
    }
    check_killer(ch, victim);
	if (!IS_IMMORTAL(ch))
	{ WAIT_STATE(ch, skill_table[gsn_omnislash].beats); }
    if (number_percent() < get_skill(ch, gsn_omnislash)) {
	check_improve(ch, gsn_omnislash, TRUE, 1);

	one_hit(ch, victim, gsn_omnislash, FALSE);
	if ((ch->hit*100/ch->max_hit <= 95) && (victim->in_room == ch->in_room))
	{
		ch->move -= 1;
		one_hit(ch, victim, gsn_omnislash, FALSE);
	}
	if ((ch->hit*100/ch->max_hit <= 50) && (victim->in_room == ch->in_room))
	{
		ch->move -= 2;
		one_hit(ch, victim, gsn_omnislash, FALSE);
	}
	if ((ch->hit*100/ch->max_hit <= 40) && (victim->in_room == ch->in_room))
	{
		ch->move -= 4;
		one_hit(ch, victim, gsn_omnislash, FALSE);
	}
	if ((ch->hit*100/ch->max_hit <= 30) && (victim->in_room == ch->in_room))
	{
		ch->move -= 6;
		one_hit(ch, victim, gsn_omnislash, FALSE);
	}
	if ((ch->hit*100/ch->max_hit <= 25) && (victim->in_room == ch->in_room))
	{
		ch->move -= 8;
		one_hit(ch, victim, gsn_omnislash, FALSE);
	}
	if ((ch->hit*100/ch->max_hit <= 20) && (victim->in_room == ch->in_room))
	{
		ch->move -= 10;
		one_hit(ch, victim, gsn_omnislash, FALSE);
	}
	if ((ch->hit*100/ch->max_hit <= 19) && (victim->in_room == ch->in_room))
	{
		ch->move -= 12;
		one_hit(ch, victim, gsn_omnislash, FALSE);
	}
	if ((ch->hit*100/ch->max_hit <= 18) && (victim->in_room == ch->in_room))
	{
		ch->move -= 14;
		one_hit(ch, victim, gsn_omnislash, FALSE);
	}
	if ((ch->hit*100/ch->max_hit <= 17) && (victim->in_room == ch->in_room))
	{
		ch->move -= 16;
		one_hit(ch, victim, gsn_omnislash, FALSE);
	}
	if ((ch->hit*100/ch->max_hit <= 16) && (victim->in_room == ch->in_room))
	{
		ch->move -= 18;
		one_hit(ch, victim, gsn_omnislash, FALSE);
	}
	if ((ch->hit*100/ch->max_hit <= 15) && (victim->in_room == ch->in_room))
	{
		ch->move -= 20;
		one_hit(ch, victim, gsn_omnislash, FALSE);
	}
	if ((ch->hit*100/ch->max_hit <= 5) && (victim->in_room == ch->in_room))
	{
		ch->move -= 22;
		one_hit(ch, victim, gsn_omnislash, FALSE);
	}

    } else {
	check_improve(ch, gsn_omnislash, FALSE, 1);
	damage(ch, victim, 0, gsn_omnislash, DAM_NONE, TRUE);
    }

    return;
}
