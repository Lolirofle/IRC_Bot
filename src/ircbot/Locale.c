#include "Locale.h"

enum Languages language = LANG_SWEDISH;

const struct Locale locale[2]={
	[LANG_SWEDISH]={
		.lang_name={"swedish",7},
		.boolean={
			{"falskt",6},
			{"sant",4}
		},
		.language={
			.set={"Språk är satt till svenska",26+2},
			.unknown={"Språket finns inte att välja bland",34+2}
		},
		.magic8ball={
			.ask={"Ställ en fråga först",20+3},
			.yes={
				{"Med säkerhet"          ,12+1},
				{"Det är bestämt så"     ,17+3},
				{"Utan tvekan"           ,11},
				{"Ja, definitivt"        ,14},
				{"Du kan lita på det"    ,18+1},
				{"Så som jag ser det, ja",22+1},
				{"Troligtvis"            ,10},
				{"Verkar positivt"       ,15},
				{"Ja"                    , 2},
				{"Det går mot ett ja"    ,18+1}
			},
			.maybe={
				{"Svaret är suddigt, prova igen"               ,29+1},
				{"Ställ frågan igen senare"                    ,24+2},
				{"Bättre att inte berätta nu"                  ,26+2},
				{"Det går inte att förutse för tillfället"     ,39+4},
				{"Koncentrera dig på frågan och ställ den igen",44+2}
			},
			.no={
				{"Räkna inte med det"          ,18+1},
				{"Mitt svar är nej"            ,16+1},
				{"Enligt mina källor: Nej"     ,23+1},
				{"Verkar negativt"             ,15},
				{"Mycket tveksamt"             ,15}
			},
			.failure={"Prova igen",10}
		},

		.prefix={
			.get={"Kommandoprefix är för tillfället",32+3},
			.set={"Kommandoprefix är satt till",27+1}
		},

		.missing_argument={"Argument fattas",15},
		.unknown_command={"Okänt kommando: ",14+1},
		.command_error={"Fel under körning av kommando: ",31+1}
	},
	[LANG_ENGLISH]={
		.lang_name={"english",7},
		.boolean={
			{"false",5},
			{"true",4}
		},
		.language={
			.set={"Language is set to english",26},
			.unknown={"Unknown language",16}
		},
		.magic8ball={
			.ask={"Ask a question first",20},
			.yes={
				{"It is certain"            ,13},
				{"It is decidedly so"       ,18},
				{"Without a doubt"          ,15},
				{"Yes definitely"           ,14},
				{"You may rely on it"       ,18},
				{"As I see it yes"          ,15},
				{"Most likely"              ,11},
				{"Outlook good"             ,12},
				{"Yes"                      , 3},
				{"Signs point to yes"       ,18}
			},
			.maybe={
				{"Reply hazy try again"     ,20},
				{"Ask again later"          ,15},
				{"Better not tell you now"  ,23},
				{"Cannot predict now"       ,18},
				{"Concentrate and ask again",25}
			},
			.no={
				{"Don't count on it"        ,17},
				{"My reply is no"           ,14},
				{"My sources say no"        ,17},
				{"Outlook not so good"      ,19},
				{"Very doubtful"            ,13}
			},
			.failure={"Try again",9}
		},

		.prefix={
			.get={"Command prefix is currently",27},
			.set={"Command prefix set to",21}
		},

		.missing_argument={"Missing arguments",17},
		.unknown_command={"Unknown command: ",17},
		.command_error={"Command Error: ",15}
	}
};
