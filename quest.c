/*******************************************************************
This file was... borrowed from ACK!MUD source code and modified
to fit our use.
********************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h" 

/**** Local Functions ****/
CHAR_DATA *get_quest_target 	args( ( int min_level, int max_level ) );
CHAR_DATA *get_quest_giver	args( ( int min_level, int max_level ) );
OBJ_DATA  *load_quest_object    args( ( CHAR_DATA *target ) );
void	   clear_quest		args( ( void ) );
bool area_is_linked args( (int area_vnum) );

/* 17 messages, organised by blocks for each personality 
   indented messages are for when the target mob gets killed  */
struct	qmessage_type
{
	char * const message1;
	char * const message2;
};


 const struct qmessage_type  qmessages[4][17] =

 {
	 /*
   {
      {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""},
	{"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""},
	{"", ""}
   }, */
 {     
	{ "{gOh my! {x%s{x{g has been stolen from me, and I am too young to find it!{x",	 	"" },
	{ "{gWould someone please search for {x%s{x{g? I'm sure that it will be much too hard for me to find.{x", 							"" },
	{ "{gWhat will I do without {x%s{x{g?{x",								"" },
	{ "{gCan't anybody find {x%s{x {gfor me?{x",  							"" },
	{ "{gHelp me! {x%s{x {gis still missing!{x", 							"" },
	{ "{gKeep searching for {x%s{x{g, and i'll ask my mom if she knows who stole it!{x",	"" },
	{ "{gDoes anyone know who stole {x%s{x{g?{x", 							"" },
	{ "{gMy mom says {x%s{x{g stole {x%s{x{g. I know it is too hard for me to get back. Oh my, what will I do?{x",
	  "{gYeay!! It looks like whoever stole {x%s{x {gis now dead!! Thank you very much!{x" 					},
	{ "{gPlease, can you go kill {x%s{x{g? I need {x%s{x{g!! I can't sleep without it!!{x",
	  "{gWell, thank you for killing the nasty thief, but can you please return {x%s{x {gto me?{x" 				},
	{ "{gMy mom says {x%s{x{g stole {x%s{x{g. I know it is too hard for me to get back. Oh my, what will I do?{x",
		"{gYeay!! It looks like whoever stole {x%s{x {gis now dead!! Thank you very much!{x" 					},
	{ "{gIt's time for my nap now, and {x%s{x still has {x%s{x{g!! Can anyone please get it back for me?{x",
		"{gPlease, time is running out! Return {x%s{x {gto me!{x" 				},
	{ "{g{x%s{x{g is a real meanie for stealing {x%s{x{g! Can you pretty pretty pretty please get it back for me?{x",
		"{gPlease, time is running out! Return {x%s{x {gto me!{x" 				},
	{ "{gIt's time for my nap now, and {x%s{x still has {x%s{x{g!! Can anyone please get it back for me?{x",
		"{gPlease, time is running out! Return {x%s{x {gto me!{x" 				},
	{ "{gOh my, I'll never get up to watch cartoons tomorrow now! {x%s{x {gis still holding {x%s{x{g ransom, and I need it for my nap!{x",
		"{gHow can I sleep without {x%s{x{g?{x"					},
	{ "{gI give up! {x%s{x {gcan keep {x%s{x {gfor all I care! I didn't want to take a nap, anyway!{x", 
		"{gI give up! I never want to see {x%s{x {gagain!{x"					},
	{ "{gMommy, can I watch cartoons now, instead of taking a nap?? PLEASE??{x" , 				"" },
	{ "{gOh THANK YOU, {x%s{x{g!! Now that I have {x%s{x {gagain, I can go sleepy sleep!{x",	"" }

    },


   {
	{ "{yHmm, I seem to have lost {x%s{x{y. Oh well, didn't really need it much, anyway.{x", 	"" },
	{ "{yI wonder where I could find another {x%s{x{y? Anyone have any ideas?{x", 							"" },
	{ "{yWhere can {x%s{x {yhave gone?{x", 									"" },
	{ "{yI guess no one really cares, anyway, but I think I might need {x%s{x {ylater.{x",  							"" },
	{ "{yI guess I should try and find {x%s{x{y, but I really don't feel like it.{x", 						"" },
	{ "{yIf anyone has an extra {x%s{x{y, I might be willing to reward them for it.{x", 			"" },
	{ "{yDoesn't anyone out there know where to find {x%s{x{y?{x", 					"" },
	{ "{yHmm, maybe {x%s{x{y knew something I didn't, and thought it was a good idea to steal {x%s{x{y. Maybe he could use it, I know I can't.{x",
		"{yI guess {x%s{x {ydidn't help him much, since he is now dead! I do miss it though.{x" 					},
	{ "{yHmm, maybe it IS worth something.  Could someone go kill {x%s{x{y and get {x%s{x{y back for me?{x",
		"{yI guess {x%s{x {ydidn't help him much, since he is now dead! I do miss it though.{x" 					},
	{ "{yI would pay a lot if someone would kill {x%s{x {yand get {x%s{x{y back. I don't really know where it went.{x",
		"{yEven though it's not worth very much, {x%s{x {yis kind of important to me. Oh, well, guess I will never see it again.{x" 				},
	{ "{yHmm, maybe it IS worth something. Could someone go kill {x%s{x{y and get {x%s{x{y back for me?{x",
		"{yI guess {x%s{x {ydidn't help him much, since he is now dead! I do miss it though.{x" 					},
	{ "{yI would pay a lot if someone would kill {x%s{x {yand get {x%s{x{y back. I don't really know where it went.{x",
		"{yEven though it's not worth very much, {x%s{x {yis kind of important to me. Oh, well, guess I will never see it again.{x" 				},
	{ "{yOh well, since no one will help me, I guess {x%s{x {ycan keep {x%s{x{y.{x",
		"{yIt must be cursed, since everyone who has it is dead. I don't think I want {x%s{x {yafter all!{x" 					},
	{ "{yOh well, since no one will help me, I guess {x%s{x {ycan keep {x%s{x{y.{x",
		"{yIt must be cursed, since everyone who has it is dead. I don't think I want {x%s{x {yafter all!{x" 					},
	{ "{yI give up! {x%s{x {ycan keep {x%s{x {yfor all I care!{x", 
		"{yI give up! I never want to see {x%s{x {yagain!{x"					},
	{ "{yWell, I will stop asking now, but don't ever ask ME for any favors, ok?{x" , 				"" },
	{ "{yWell, looks like {x%s{x {yhas recovered {x%s{x {yfor me. Not sure I want it anymore, but thanks anyway.{x", 						"" }

    },


{
	{ "{cBANZAI! My {x%s{x {chas been stolen from me!  Will someone brave recover it?{x", 	"" },
	{ "{cOh! Has no one found my {x%s{x {cfor me yet?{x", 							"" },
	{ "{cWhere can my {x%s{x {chave gone?{x", 									"" },
	{ "{cCan't anybody find my {x%s{x {cfor me?{x",  							"" },
	{ "{cHelp me! My {x%s{x {chas not yet been recovered!{x", 						"" },
	{ "{cKeep searching for my {x%s{x{c, and i'll find out who stole it!{x", 			"" },
	{ "{cWere there no witnesses to the theft of my {x%s{c?", 					"" },
	{ "{cIt was {x%s{x {cwho stole my {x%s{x {cfrom me!  Someone help me!{x",
		"It looks like whoever stole my {x%s{x {cis now dead!!{x" 					},
	{ "Please, time is running out! Recover my {x%s{x {cfrom {x%s{x {cfor me NOW!{x",
		"Please, time is running out! Return my {x%s{x {cto me!{x" 				},
	{ "Please, time is running out! Recover my {x%s{x {cfrom {x%s{x {cfor me NOW!{x",
		"Please, time is running out! Return my {x%s{x {cto me!{x" 				},
	{ "Please, time is running out! Recover my {x%s{x {cfrom {x%s{x {cfor me NOW!{x",
		"Please, time is running out! Return my {x%s{x {cto me!{x" 				},
	{ "Please, time is running out! Recover my {x%s{x {cfrom {x%s{x {cfor me NOW!{x",
		"Please, time is running out! Return my {x%s{x {cto me!{x" 				},
	{ "I give up! {x%s{x {ccan keep my {x%s{x {cfor all I care!{x",
		"I give up! I never want to see my {x%s{x {cagain!{x" 					},
	{ "I give up! {x%s{x {ccan keep my {x%s{x {cfor all I care!{x",
		"I give up! I never want to see my {x%s{x {cagain!{x"					},
	{ "I give up! {x%s{x {ccan keep my {x%s{x {cfor all I care!{x", 
		"I give up! I never want to see my {x%s{x {cagain!{x"					},
	{ "Shoot! Just forget about recovering ANYTHING for me, ok?{x" , 				"" },
	{ "At Last! {x%s{x {chas recovered {x%s{x {cfor me!{x", 						"" }

    },    

{
	{ "{rMuuaahhhaaahaaaa! Some puny mortal has stolen {x%s{x{r! I shall seek revenge!!{x", 	"" },
	{ "{rI shall send many minions to seek {x%s{x{r! All that steal from me shall die!!{x", 							"" },
	{ "{rSO, you have defeated my servants. I shall still regain {x%s{x{r!!{x", 									"" },
	{ "{rI am prepared to reward well anyone that aids the return of {x%s{x{r. Are any of you puny mortals willing to attempt my challenge?{x",  							"" },
	{ "{rIf you are worthy, I will grant many favors upon anyone that returns {x%s{x{r.{x", 						"" },
	{ "{mMethlok{r, By the dark powers, I command you to seek {x%s{x{r! Now, if any of you worthless mortals wish to attempt to return it, I shall grant you many powers!{x", 			"" },
	{ "{rI sense that {mMethlok{r is nearing the witless thief who stole {x%s{x{r. Now, my vengence shall be sweet!{x", 					"" },
	{ "{rAhhh, my servant has returned, and informs me that {x%s{x {rstole {x%s{x{r. They shall be incinerated by the powers that I command!!!{x",
		"{mMethlok{r has informed me that the weakling that stole {x%s{x {yhas met his maker!!{x" 					},
	{ "{rAre none of you powerful enough to kill {x%s{x {rand regain {x%s{x{r? Bah!! Mortals are useless, except as side dishes!!{x",
		"{rThough my taste for blood has been satiated, {x%s{x {rstill evades my grasp!{x" 				},
	{ "{rAre none of you powerful enough to kill {x%s{x {r and regain {x%s{x {r? Bah!! Mortals are useless, except as side dishes!!{x",
		"{rThough my taste for blood has been satiated, {x%s{x {rstill evades my grasp!{x" 				},
	{ "{rAre none of you powerful enough to kill {x%s{x {r and regain {x%s{x {r? Bah!! Mortals are useless, except as side dishes!!{x",
		"{rThough my taste for blood has been satiated, {x%s{x {rstill evades my grasp!{x" 				},
	{ "{rI should have known that a powerless, puny mortal could never be the servant of my vengence against {x%s{x{r, or regain {x%s{x{r!!{x",
		"{rI shall rain death upon all of you for refusing to return {x%s{x{r!!!{x" 				},
	{ "{rI should have known that a powerless, puny mortal could never be the servant of my vengence against {x%s{x{r, or regain {x%s{x{r!!{x",
		"{rI shall rain death upon all of you for refusing to return {x%s{x{r!!!{x" 				},
	{ "{rI shall slay your brothers and poison your fields for refusing to seek {x%s{x {rand return {x%s{x{r!!!!{x",
		"{rThough my vengeance has been served, I shall drink your souls for your failure to return {x%s{x{r!!!{x"					},
	{ "{rI shall slay your brothers and poison your fields for refusing to seek {x%s{x {rand return {x%s{x{r!!!!{x",
		"{rThough my vengeance has been served, I shall drink your souls for your failure to return {x%s{x{r!!!{x"					},
	{ "{rDeath and great suffering shall be your punishment for failing me!!!?{x" , 				"" },
	{ "{rWell done. It seems that {x%s{x {rat least has a modicum of strength, unlike you worthless idiots who failed to return {x%s{x{r! My curse shall lie upon you for the rest of your short days!{x", "" }

    }




  };


void do_quest( CHAR_DATA *ch, char *argument )
{
   extern bool quest;
   extern bool auto_quest;
   extern CHAR_DATA *quest_mob;
   extern CHAR_DATA *quest_target;
   extern OBJ_DATA *quest_object;
   extern int quest_timer;
   extern int quest_wait;
  
   char buf[MAX_STRING_LENGTH];
   char new_long_desc[MAX_STRING_LENGTH];
   
   if ( argument[0] == '\0' )	/* Display status */
   {
      if ( !quest )
      {
         send_to_char( "There is no quest currently running.\n\r", ch );
         if ( auto_quest )
            send_to_char( "Quests are currently running automatically.\n\r", ch );
         if ( quest_wait > 0 )
         {
            sprintf( buf, "The next quest may occur in %d minutes.\n\r", (quest_wait * 2) );
            send_to_char( buf, ch );
         }
         return;
      }
      else
         send_to_char( "There is currently a quest running ", ch );
      
      if ( auto_quest )
         send_to_char( "(Automatically)", ch );
      
      send_to_char( "\n\rQuest Details:\n\r\n\r", ch );
      if ( quest_mob )
      {

        sprintf( buf, "The questing mobile is: {x%s{x [In Room %d]\n\r", 
           quest_mob->short_descr, quest_mob->in_room->vnum );
        send_to_char( buf, ch );
      }
      else
      {
        send_to_char( "The questing mobile is dead!\n\r", ch );
      }
      if ( quest_target )
      {
        sprintf( buf, "Target Mobile is: {x%s{x [In Room %d]\n\r",
           quest_target->short_descr, quest_target->in_room->vnum );
        send_to_char( buf, ch );
      }
      else
        send_to_char( "The target mobile is dead!\n\r", ch );

      sprintf( buf, "Target Object is: {x%s{x.\n\r", quest_object->short_descr );
      send_to_char( buf, ch );

      sprintf( buf, "Quest Object is worth: %d QP, %d XP, %d raise points and %d GP\n\r",
         quest_object->value[0], (ch->level < LVL_MOBXPCAP) ? quest_object->value[1] : 0,
         quest_object->value[4], quest_object->value[2]);
      send_to_char( buf, ch );

      
      sprintf( buf, "The Quest has been running for %d/30 minutes.\n\r", (quest_timer * 2) );
      send_to_char( buf, ch );
      
      return;
   }
   if ( !strcmp( argument, "stop" ) )
   {
     if ( quest )
     {
		 /*
       sprintf( buf, "@@NThe quest has been stopped by an {mImmortal@@N. Please speak up if you have already gotten the item.\n\r" );
       do_gecho( ch, buf );
	     */
       clear_quest();
     }
     return;
   }

   if ( !strcmp( argument, "start" ) )
   {
      DESCRIPTOR_DATA *d;
      int a = 80;
      int b = 0;
      sh_int player_count = 0, average_level= 0, total_levels = 0;
      
      /* generate a new quest! */
      if ( quest )
      {
         send_to_char( "There is already a quest running...\n\r", ch );
         return;
      }
      
      if ( auto_quest )
      {
         auto_quest = FALSE;
         send_to_char( "Automatic Quests now OFF.\n\r", ch );
      }
      
      /* Work out levels of currently playing folks */
      for ( d = descriptor_list; d; d = d->next )
      {
         if (  ( d->connected != CON_PLAYING )
            || ( IS_IMMORTAL( d->character )  ) )
            continue;
         player_count += 1;
         total_levels += d->character->level;
      }
      average_level = ( ( ( total_levels == 0 ) ? 30 : total_levels ) /
                        ( ( player_count == 0 ) ? 1 : player_count ) );
      a = average_level - 20;
      b = average_level + 20;

	  sprintf(buf, "Mob lLevel: %d hLevel: %d\n\r", a, b);
	  stc(buf,ch);
      quest_mob    = get_quest_giver(a, b);
      if ( quest_mob == NULL )
      {
         send_to_char( "Failed to find a quest mob\n\r", ch );
         return;
      }
      b = UMIN( b, 110 );
      a = UMAX( 20, a );

      quest_target = get_quest_target( a, b );
      if (  ( quest_target == NULL )
         || ( quest_target == quest_mob )  )
      {
         send_to_char( "Failed to find a quest target\n\r", ch );
         return;
      }
      quest_object = load_quest_object( quest_target );
      if ( quest_object == NULL )
      {
         send_to_char( "An invalid quest object was encountered.  Check log files.\n\r", ch );
         quest = FALSE;
         return;
      }
      /* Set values on quest item for Qp, Pracs, Exp, Gold */
      quest_object->value[0] = UMAX( 1, ( quest_target->level / 3) );
      quest_object->value[1] = number_range(1, 60);
      quest_object->value[2] = ( quest_target->level * 100 );
      quest_object->value[4] = 2;

      if ( number_percent() < 10 )
      {
         quest_object->value[0] += 2;
         quest_object->value[1] += 3;
         quest_object->value[2] *= 2;
         quest_object->value[4] *= 3;
      }
      
      quest_timer = 0;
      quest = TRUE;
      new_long_desc[0] = '\0';
      if ( quest_mob->pIndexData->long_descr_orig != NULL )
        free_string( quest_mob->pIndexData->long_descr_orig );
      quest_mob->pIndexData->long_descr_orig = str_dup(quest_mob->long_descr );
      sprintf( new_long_desc, "%s {xsays have you found %s?\n\r",
         quest_mob->pIndexData->short_descr, quest_object->short_descr );
      if ( quest_mob->long_descr != NULL )      
        free_string( quest_mob->long_descr );    
      quest_mob->long_descr = str_dup( new_long_desc );
      SET_BIT( quest_mob->act, PLR_NOSUMMON );
      REMOVE_BIT(quest_mob->act, ACT_AGGRESSIVE);
     /* T!!! */
      new_long_desc[0] = '\0';
      if ( quest_target->pIndexData->long_descr_orig != NULL )
        free_string( quest_target->pIndexData->long_descr_orig );
      quest_target->pIndexData->long_descr_orig = str_dup( quest_target->long_descr );
      sprintf( new_long_desc, "%s {xsays I stole the {x%s{x!\n\r",
        quest_target->pIndexData->short_descr, quest_object->short_descr );
      if ( quest_target->long_descr != NULL )     
        free_string( quest_target->long_descr );   
      quest_target->long_descr = str_dup( new_long_desc );

      SET_BIT( quest_target->act, PLR_NOSUMMON );

      send_to_char( "QUEST STARTED!\n\r\n\r", ch );
            
      sprintf( buf, "The questing mobile is: {x%s{x [In Room %d]\n\r", 
         quest_mob->pIndexData->short_descr, quest_mob->in_room->vnum );
      send_to_char( buf, ch );
      
      sprintf( buf, "Target Mobile is: {x%s{x [In Room %d]\n\r",
         quest_target->pIndexData->short_descr, quest_target->in_room->vnum );
      send_to_char( buf, ch );
      
      sprintf( buf, "Target Object is: {x%s{x.\n\r", quest_object->short_descr );
      send_to_char( buf, ch );
      
      sprintf( buf, "Quest Object is worth: %d QP, %d XP, %d raise points and %d GP\n\r",
         quest_object->value[0], quest_object->value[1],
         quest_object->value[4], quest_object->value[2] );
      send_to_char( buf, ch );
      
      return;
   }
   if ( !str_cmp( argument, "auto" ) )
   {
      send_to_char( "AutoQuest now initiated!\n\r", ch );
      auto_quest = TRUE;
	  quest_wait = 360;
      return;
   }
   
   
   return;
}      
      
            

/*
 * get_quest_target : This attempts to pick a random mobile to hold the quest
 * item for the player (questor).  Various checks are made to ensure that the
 * questor has a chance of killing the mobile, etc.
 * Returns NULL if it didn't get a mobile this time.
 */

CHAR_DATA *get_quest_target( int min_level, int max_level  )
{
   CHAR_DATA *target;
   extern CHAR_DATA *quest_mob;
   int min_index = 0;  /* the minimum number of times to go through the list */



/*   int min_distance = 50; unused */
/*   char *dirs = NULL; unused */


    if ( max_level > 140 )
      max_level = 140;
   min_index = number_range(1, 1000);

   for ( target = char_list; target != NULL; target = target->next )
   {
      if ( !IS_NPC( target ) )
         continue;
      min_index -= 1;

      if ( min_index > 0 )
         continue;

         
      /*
       * Check if mobile is suitable for the quest -
       * Ignore mobs that are likely to be in well known places, such
       * as train/prac mobs, healers, etc
       */
      
     if (    ( target->level < min_level )
          || ( target->level > max_level )
          || ( IS_SET( target->act, ACT_SENTINEL ) )  
          || ( IS_SET( target->act, ACT_PET ) )
          || ( target->in_room->area == quest_mob->in_room->area ) 
		  || ( target->in_room->area->vnum == 0 )
		  || ( target->in_room->area->vnum == 1 )
		  || ( target->in_room->area->vnum == 2 )
		  || ( target->in_room->area->vnum == 3 )
		  || ( target->in_room->area->vnum == 4 )
		  || ( target->in_room->area->vnum == 6 )
		  || ( target->in_room->area->vnum == 12 )
		  || ( target->in_room->area->vnum == 13 )
		  || ( target->in_room->area->vnum == 15 )
		  || ( target->in_room->area->vnum == 18 )
		  || ( target->in_room->area->vnum == 19 )
		  || ( target->in_room->area->vnum == 24 )
		  || ( target->in_room->area->vnum == 25 )
		  || ( target->in_room->area->vnum == 33 )
		  || ( target->in_room->area->vnum == 35 )
		  || ( target->in_room->area->vnum == 41 )
		  || ( target->in_room->area->vnum == 80 )
		  || ( target->in_room->area->vnum == 81 )
		  || ( target->in_room->area->vnum == 86 )
		  || ( target->in_room->area->vnum == 89 )
		  || ( target->in_room->area->vnum == 90 )
		  || ( target->in_room->area->vnum == 93 )
		  || ( target->in_room->area->vnum == 94 )
		  || ( target->in_room->area->vnum == 95 )
		  || ( target->in_room->area->vnum == 96 )
		  || ( target->in_room->area->vnum == 97 )
		  || ( target->in_room->area->vnum == 98 )
		  || ( target->in_room->area->vnum == 99 )
		  || ( target->in_room->area->vnum == 100 )
		  || ( target->in_room->area->vnum == 101 )
		  || ( target->in_room->area->vnum == 102 )
		  || ( target->in_room->area->vnum == 103 )
		  || ( target->in_room->area->vnum == 104 )
		  || ( target->in_room->area->vnum == 105 )
		  || ( target->in_room->area->vnum == 106 )
		  || ( target->in_room->area->vnum == 107 )
		  || ( target->in_room->area->vnum == 108 )
		  || ( target->in_room->area->vnum == 109 )
		  || ( target->in_room->area->vnum == 110 )
                  || ( target->in_room->area->vnum == 91 )
                  || ( target->in_room->area->vnum == 38 ) ) 

		  /*
		  || ( !strcmp(spec_name(target->spec_fun), "spec_guard_kael") )
          || ( !strcmp(spec_name(target->spec_fun), "spec_guard_evil") )
          || ( !strcmp(spec_name(target->spec_fun), "spec_guard_tier") )
          || ( !strcmp(spec_name(target->spec_fun), "spec_guard_tier2") )
          || ( !strcmp(spec_name(target->spec_fun), "spec_guard_cairn") ) )
          */
		  continue;

     /* Lastly, some random choice */
     if ( number_percent() < 2 )
        break;
   } 
   
   return target;
}       

/*
 * load_quest_object : This takes a pointer to OBJ_INDEX_DATA and places the
 * object onto the target.
 */

OBJ_DATA *load_quest_object( CHAR_DATA *target )
{
   OBJ_INDEX_DATA *pObj;
   OBJ_DATA *object;
   int foo;
   
   foo = number_range( OBJ_VNUM_QUEST_MIN, OBJ_VNUM_QUEST_MAX );
   
   pObj = get_obj_index( foo );
   
   if ( pObj == NULL )
   {
      bug( "load_quest_object : Invalid object vnum %d.", foo );
      return NULL;
   }
      
   object = create_object( pObj, 1 );
   obj_to_char( object, target );
   
   return object;
}
   
CHAR_DATA *get_quest_giver( int min_level, int max_level )
{
   CHAR_DATA *target;
   int    min_index = 0;



/*   int max_distance = 20; unused */
/*   char *dirs = NULL; unused */
   min_index = number_range(0, 1000);

   for ( target = char_list; target != NULL; target = target->next )
   {
      if ( !IS_NPC( target ) )
         continue;
      min_index -= 1;

      if ( min_index > 0 )
        continue;

		//try and prevent quests to unlinked areas!
	  if (area_is_linked(target->in_room->area->vnum) == FALSE)
	  {
		  continue;
	  }

         
      /*
       * Check if mobile is suitable for the quest -
       * Ignore mobs that are likely to be in well known places, such
       * as train/prac mobs, healers, etc
       */
      
     if (    ( target->level < min_level )
          || ( target->level > max_level )
		  || ( target->in_room->area->vnum == number_range(5, 58)))
             continue;
     {
       if ( number_percent() < 2 )
        break;
     }
      
   }  

   return target;
}         
/*		  || ( target->in_room->area->vnum == 0 )
		  || ( target->in_room->area->vnum == 1 )
		  || ( target->in_room->area->vnum == 2 )
		  || ( target->in_room->area->vnum == 3 )
		  || ( target->in_room->area->vnum == 4 )
		  || ( target->in_room->area->vnum == 6 )
		  || ( target->in_room->area->vnum == 12 )
		  || ( target->in_room->area->vnum == 13 )
		  || ( target->in_room->area->vnum == 15 )
		  || ( target->in_room->area->vnum == 18 )
		  || ( target->in_room->area->vnum == 19 )
		  || ( target->in_room->area->vnum == 24 )
		  || ( target->in_room->area->vnum == 25 )
		  || ( target->in_room->area->vnum == 33 )
		  || ( target->in_room->area->vnum == 35 )
		  || ( target->in_room->area->vnum == 41 )
		  || ( target->in_room->area->vnum == 80 )
		  || ( target->in_room->area->vnum == 81 )
		  || ( target->in_room->area->vnum == 86 )
		  || ( target->in_room->area->vnum == 89 )
		  || ( target->in_room->area->vnum == 90 )
		  || ( target->in_room->area->vnum == 93 )
		  || ( target->in_room->area->vnum == 94 )
		  || ( target->in_room->area->vnum == 95 )
		  || ( target->in_room->area->vnum == 96 )
		  || ( target->in_room->area->vnum == 97 )
		  || ( target->in_room->area->vnum == 98 )
		  || ( target->in_room->area->vnum == 99 )
		  || ( target->in_room->area->vnum == 100 )
		  || ( target->in_room->area->vnum == 101 )
		  || ( target->in_room->area->vnum == 102 )
		  || ( target->in_room->area->vnum == 103 )
		  || ( target->in_room->area->vnum == 104 )
		  || ( target->in_room->area->vnum == 105 )
		  || ( target->in_room->area->vnum == 106 )
		  || ( target->in_room->area->vnum == 107 )
		  || ( target->in_room->area->vnum == 108 )
		  || ( target->in_room->area->vnum == 109 )
		  || ( target->in_room->area->vnum == 110 ) */
/*
 * quest_inform : Makes the questing mobile give out information to the
 * players on the mud.  Starts off real simple, and gets more helpful as
 * time runs out :P
 */

void quest_inform( void )
{
   char buf[MAX_STRING_LENGTH];
   extern CHAR_DATA *quest_mob;
   extern CHAR_DATA *quest_target;
   extern OBJ_DATA *quest_object;
   extern int quest_timer;
   extern sh_int quest_personality;
   extern const struct qmessage_type  qmessages[4][17]; 



   /* Work out what the mob should tell the players.... */
   /* Add random element to each case so quests look different each time? */
   if ( quest_timer < 7 )
   {
    sprintf( buf, qmessages[quest_personality][quest_timer].message1, quest_object->short_descr );
   }
   else
   { 
   if (quest_target)
	sprintf ( buf, qmessages[quest_personality][quest_timer].message1,
			 quest_target->short_descr,quest_object->short_descr );
   else
      sprintf( buf, qmessages[quest_personality][quest_timer].message2,
		 quest_object->short_descr );
   }



   quest_timer++;
   
   if ( quest_mob && quest_timer < 16 )
        do_crusade( quest_mob, buf, 9 );
	
   if ( quest_timer == 1 )
   {
     sprintf( buf, " {x%s{x is crusading for {x%s{x ", quest_mob->pIndexData->short_descr, quest_object->short_descr);
    /*
     info( buf, 5 );
     */
  }
   if ( !quest_mob )
   {
     clear_quest();
   }    
   return;
}

void quest_complete( CHAR_DATA *ch )
{
   extern CHAR_DATA *quest_mob;
   extern bool auto_quest;
   extern OBJ_DATA *quest_object;
   extern sh_int quest_personality;
   extern const struct qmessage_type  qmessages[4][17]; 


   char buf[MAX_STRING_LENGTH];

   sprintf( buf, qmessages[quest_personality][16].message1, ch->name, quest_object->short_descr );

   do_crusade( quest_mob, buf, 9 );

  clear_quest();
  auto_quest = TRUE;
   return;
}   
   

void quest_cancel()
{
   extern CHAR_DATA *quest_mob;
   extern bool auto_quest;
    if ( quest_mob )
       do_crusade( quest_mob, "{RShoot! Just forget about recovering ANYTHING for me, ok?{x", 9 );


    clear_quest();
    auto_quest = TRUE;
    return;   
}   

void clear_quest()
{
   extern bool quest;
   extern CHAR_DATA *quest_mob;
   extern CHAR_DATA *quest_target;
   extern OBJ_DATA *quest_object;
   extern int quest_timer;
   extern int quest_wait;
   extern sh_int quest_personality;

   /* Clear ALL values, ready for next quest */
   
   quest = FALSE;
   extract_obj ( quest_object );
   if ( quest_mob )
   {
     free_string( quest_mob->long_descr );
     quest_mob->long_descr =  str_dup( quest_mob->pIndexData->long_descr_orig );
     free_string( quest_mob->pIndexData->long_descr_orig );
     quest_mob->pIndexData->long_descr_orig = NULL;
   }
   if ( quest_target )
   {
     free_string( quest_target->long_descr );
     quest_target->long_descr =  str_dup( quest_target->pIndexData->long_descr_orig );
     free_string( quest_target->pIndexData->long_descr_orig );
     quest_target->pIndexData->long_descr_orig = NULL;
   };


   quest_mob = NULL;
   quest_target = NULL;
   quest_object = NULL;
   quest_timer = 0;
   quest_wait = 180 + number_range( 1, 180);
   quest_personality = 1;
   
   return;
}
   
   
void generate_auto_quest()
{

  extern bool quest;
   
  extern CHAR_DATA *quest_mob;
  extern CHAR_DATA *quest_target;
  extern OBJ_DATA *quest_object;
  extern int quest_timer;
  extern int quest_wait;
  extern sh_int quest_personality;
 // int  hunt_flags = 0;
  char new_long_desc[MAX_STRING_LENGTH];
  sh_int       loop_counter = 0; 

  int a = 140;
  int b = 0;
  sh_int  average_level= 0;
      
  /* generate a new quest! */
  if ( quest )
  {
    return;
  }
      
#if 0
      /* Work out levels of currently playing folks */
  if ( descriptor_list && descriptor_list->connected == CON_PLAYING )
  {
  for ( d = descriptor_list; d; d = d->next )
  {
    if (    d->connected != CON_PLAYING )
      continue;
	if (IS_IMMORTAL(d->character))
		continue;
      player_count += 1;
      total_levels += d->character->level;
  }
  player_count = UMAX( 1, player_count );
  average_level = ( total_levels / player_count );
#endif

  quest_mob = NULL;
  quest_target = NULL;

  average_level = number_range( 0, 65 );
  if ( average_level < 20 )
  {
    a = number_range( 5, 25 );
    b = number_range( 30, 45 );
//    hunt_flags = HUNT_WORLD | HUNT_OPENDOOR ;
    quest_personality = 1;
  }
  else   if ( average_level < 40 )
  {
    a = number_range( 30, 40 );
    b = number_range( 40, 55 );
//    hunt_flags = HUNT_WORLD | HUNT_OPENDOOR | HUNT_PICKDOOR;
    quest_personality = 2;
  }
  else
  {
    a = number_range( 50, 55 );
    b = number_range( 56, 99 );
//    hunt_flags = HUNT_WORLD | HUNT_OPENDOOR | HUNT_PICKDOOR | HUNT_UNLOCKDOOR;
    quest_personality = 3;
  }
  if (quest_personality == 0 )
	  quest_personality = 1;

  while (  ( quest_mob == NULL )
        && ( loop_counter < 500 )  )
  {
    loop_counter++;
    quest_mob    = get_quest_giver(a, b);
  }

  if ( quest_mob == NULL )
  {
    quest = FALSE;
    quest_wait = number_range( 1, 3 );
    return;
  }
  loop_counter = 0;
  while (  ( quest_target == NULL )
        && ( loop_counter < 500 )  )
  {
    loop_counter++;
    quest_target = get_quest_target( a, b );
	
    if (  ( quest_target == NULL )
       || ( quest_target == quest_mob )  ) 
      quest_target = NULL;
	  
  }



  if ( quest_target == NULL )
  {
    quest = FALSE;
    quest_wait = number_range( 1, 3 );
    return;
  }
  quest_object = load_quest_object( quest_target );
  if ( quest_object == NULL )
  {
    quest = FALSE;
    quest_wait = number_range( 1, 3 );
    return;
  }
      /* Set values on quest item for Qp, Pracs, Exp, Gold */
  quest_object->value[0] = UMAX( 1, ( quest_target->level / 3 ) );
  quest_object->value[1] = number_range(1, 60);
  quest_object->value[2] = ( quest_target->level * 100 );
  quest_object->value[3] = average_level;
  quest_object->value[4] = 2;

  if ( number_percent() < 10 )
  {
    quest_object->value[0] += 2;
    quest_object->value[1] += 3;
    quest_object->value[2] *= 2;
    quest_object->value[4] *= 3;
  }
      
  quest_timer = 0;
  quest = TRUE;
  new_long_desc[0] = '\0';
  if ( quest_mob->pIndexData->long_descr_orig != NULL )
    free_string( quest_mob->pIndexData->long_descr_orig );
  quest_mob->pIndexData->long_descr_orig = str_dup(quest_mob->long_descr );
  sprintf( new_long_desc, "{x%s{x says have you found {x%s{x?\n\r",
         quest_mob->short_descr, quest_object->short_descr );
  if ( quest_mob->long_descr != NULL )      
    free_string( quest_mob->long_descr );    
  quest_mob->long_descr = str_dup( new_long_desc );
  SET_BIT( quest_mob->act, PLR_NOSUMMON );
  REMOVE_BIT(quest_mob->act, ACT_AGGRESSIVE);

  new_long_desc[0] = '\0';
  if ( quest_target->pIndexData->long_descr_orig != NULL )
    free_string( quest_target->pIndexData->long_descr_orig );
  quest_target->pIndexData->long_descr_orig = str_dup( quest_target->long_descr );
  sprintf( new_long_desc, "{x%s{x says I stole {x%s{x!\n\r",
        quest_target->pIndexData->short_descr, quest_object->short_descr );
  if ( quest_target->long_descr != NULL )     
        free_string( quest_target->long_descr );   
  quest_target->long_descr = str_dup( new_long_desc );

  SET_BIT( quest_target->act, PLR_NOSUMMON );  
  return;

}
