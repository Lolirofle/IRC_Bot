#ifndef __LOLIROFLE_IRC_IRC_MESSAGENUMBERS_H_INCLUDED__
#define __LOLIROFLE_IRC_IRC_MESSAGENUMBERS_H_INCLUDED__

#define IRC_MESSAGENO_ERR_NOSUCHNICK       401
#define IRC_MESSAGENO_ERR_NOSUCHSERVER     402
#define IRC_MESSAGENO_ERR_NOSUCHCHANNEL    403
#define IRC_MESSAGENO_ERR_CANNOTSENDTOCHAN 404
#define IRC_MESSAGENO_ERR_TOOMANYCHANNELS  405
#define IRC_MESSAGENO_ERR_WASNOSUCHNICK    406
#define IRC_MESSAGENO_ERR_TOOMANYTARGETS   407
#define IRC_MESSAGENO_ERR_NOORIGIN         409
#define IRC_MESSAGENO_ERR_NORECIPIENT      411
#define IRC_MESSAGENO_ERR_NOTEXTTOSEND     412
#define IRC_MESSAGENO_ERR_NOTOPLEVEL       413
#define IRC_MESSAGENO_ERR_WILDTOPLEVEL     414
#define IRC_MESSAGENO_ERR_UNKNOWNCOMMAND   421
#define IRC_MESSAGENO_ERR_NOMOTD           422
#define IRC_MESSAGENO_ERR_NOADMININFO      423
#define IRC_MESSAGENO_ERR_FILEERROR        424
#define IRC_MESSAGENO_ERR_NONICKNAMEGIVEN  431
#define IRC_MESSAGENO_ERR_ERRONEUSNICKNAME 432
#define IRC_MESSAGENO_ERR_NICKNAMEINUSE    433
#define IRC_MESSAGENO_ERR_NICKCOLLISION    436
#define IRC_MESSAGENO_ERR_USERNOTINCHANNEL 441
#define IRC_MESSAGENO_ERR_NOTONCHANNEL     442
#define IRC_MESSAGENO_ERR_USERONCHANNEL    443
#define IRC_MESSAGENO_ERR_NOLOGIN          444
#define IRC_MESSAGENO_ERR_SUMMONDISABLED   445
#define IRC_MESSAGENO_ERR_USERSDISABLED    446
#define IRC_MESSAGENO_ERR_NOTREGISTERED    451
#define IRC_MESSAGENO_ERR_NEEDMOREPARAMS   461
#define IRC_MESSAGENO_ERR_ALREADYREGISTRED 462
#define IRC_MESSAGENO_ERR_NOPERMFORHOST    463
#define IRC_MESSAGENO_ERR_PASSWDMISMATCH   464
#define IRC_MESSAGENO_ERR_YOUREBANNEDCREEP 465
#define IRC_MESSAGENO_ERR_KEYSET           467
#define IRC_MESSAGENO_ERR_CHANNELISFULL    471
#define IRC_MESSAGENO_ERR_UNKNOWNMODE      472
#define IRC_MESSAGENO_ERR_INVITEONLYCHAN   473
#define IRC_MESSAGENO_ERR_BANNEDFROMCHAN   474
#define IRC_MESSAGENO_ERR_BADCHANNELKEY    475
#define IRC_MESSAGENO_ERR_NOPRIVILEGES     481
#define IRC_MESSAGENO_ERR_CHANOPRIVSNEEDED 482
#define IRC_MESSAGENO_ERR_CANTKILLSERVER   483
#define IRC_MESSAGENO_ERR_NOOPERHOST       491
#define IRC_MESSAGENO_ERR_UMODEUNKNOWNFLAG 501
#define IRC_MESSAGENO_ERR_USERSDONTMATCH   502
#define IRC_MESSAGENO_RPL_NONE             300
#define IRC_MESSAGENO_RPL_USERHOST         302
#define IRC_MESSAGENO_RPL_ISON             303
#define IRC_MESSAGENO_RPL_AWAY             301
#define IRC_MESSAGENO_RPL_UNAWAY           305
#define IRC_MESSAGENO_RPL_NOWAWAY          306
#define IRC_MESSAGENO_RPL_WHOISUSER        311
#define IRC_MESSAGENO_RPL_WHOISSERVER      312
#define IRC_MESSAGENO_RPL_WHOISOPERATOR    313
#define IRC_MESSAGENO_RPL_WHOISIDLE        317
#define IRC_MESSAGENO_RPL_ENDOFWHOIS       318
#define IRC_MESSAGENO_RPL_WHOISCHANNELS    319
#define IRC_MESSAGENO_RPL_WHOWASUSER       314
#define IRC_MESSAGENO_RPL_ENDOFWHOWAS      369
#define IRC_MESSAGENO_RPL_LISTSTART        321
#define IRC_MESSAGENO_RPL_LIST             322
#define IRC_MESSAGENO_RPL_LISTEND          323
#define IRC_MESSAGENO_RPL_CHANNELMODEIS    324
#define IRC_MESSAGENO_RPL_NOTOPIC          331
#define IRC_MESSAGENO_RPL_TOPIC            332
#define IRC_MESSAGENO_RPL_INVITING         341
#define IRC_MESSAGENO_RPL_SUMMONING        342
#define IRC_MESSAGENO_RPL_VERSION          351
#define IRC_MESSAGENO_RPL_WHOREPLY         352
#define IRC_MESSAGENO_RPL_ENDOFWHO         315
#define IRC_MESSAGENO_RPL_NAMREPLY         353
#define IRC_MESSAGENO_RPL_ENDOFNAMES       366
#define IRC_MESSAGENO_RPL_LINKS            364
#define IRC_MESSAGENO_RPL_ENDOFLINKS       365
#define IRC_MESSAGENO_RPL_BANLIST          367
#define IRC_MESSAGENO_RPL_ENDOFBANLIST     368
#define IRC_MESSAGENO_RPL_INFO             371
#define IRC_MESSAGENO_RPL_ENDOFINFO        374
#define IRC_MESSAGENO_RPL_MOTDSTART        375
#define IRC_MESSAGENO_RPL_MOTD             372
#define IRC_MESSAGENO_RPL_ENDOFMOTD        376
#define IRC_MESSAGENO_RPL_YOUREOPER        381
#define IRC_MESSAGENO_RPL_REHASHING        382
#define IRC_MESSAGENO_RPL_TIME             391
#define IRC_MESSAGENO_RPL_USERSSTART       392
#define IRC_MESSAGENO_RPL_USERS            393
#define IRC_MESSAGENO_RPL_ENDOFUSERS       394
#define IRC_MESSAGENO_RPL_NOUSERS          395
#define IRC_MESSAGENO_RPL_TRACELINK        200
#define IRC_MESSAGENO_RPL_TRACECONNECTING  201
#define IRC_MESSAGENO_RPL_TRACEHANDSHAKE   202
#define IRC_MESSAGENO_RPL_TRACEUNKNOWN     203
#define IRC_MESSAGENO_RPL_TRACEOPERATOR    204
#define IRC_MESSAGENO_RPL_TRACEUSER        205
#define IRC_MESSAGENO_RPL_TRACESERVER      206
#define IRC_MESSAGENO_RPL_TRACENEWTYPE     208
#define IRC_MESSAGENO_RPL_TRACELOG         261
#define IRC_MESSAGENO_RPL_STATSLINKINFO    211
#define IRC_MESSAGENO_RPL_STATSCOMMANDS    212
#define IRC_MESSAGENO_RPL_STATSCLINE       213
#define IRC_MESSAGENO_RPL_STATSNLINE       214
#define IRC_MESSAGENO_RPL_STATSILINE       215
#define IRC_MESSAGENO_RPL_STATSKLINE       216
#define IRC_MESSAGENO_RPL_STATSYLINE       218
#define IRC_MESSAGENO_RPL_ENDOFSTATS       219
#define IRC_MESSAGENO_RPL_STATSLLINE       241
#define IRC_MESSAGENO_RPL_STATSUPTIME      242
#define IRC_MESSAGENO_RPL_STATSOLINE       243
#define IRC_MESSAGENO_RPL_STATSHLINE       244
#define IRC_MESSAGENO_RPL_UMODEIS          221
#define IRC_MESSAGENO_RPL_LUSERCLIENT      251
#define IRC_MESSAGENO_RPL_LUSEROP          252
#define IRC_MESSAGENO_RPL_LUSERUNKNOWN     253
#define IRC_MESSAGENO_RPL_LUSERCHANNELS    254
#define IRC_MESSAGENO_RPL_LUSERME          255
#define IRC_MESSAGENO_RPL_ADMINME          256
#define IRC_MESSAGENO_RPL_ADMINLOC1        257
#define IRC_MESSAGENO_RPL_ADMINLOC2        258
#define IRC_MESSAGENO_RPL_ADMINEMAIL       259

#endif
