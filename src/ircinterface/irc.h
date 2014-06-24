#ifndef __LOLIROFLE_IRC_IRC_H_INCLUDED__
#define __LOLIROFLE_IRC_IRC_H_INCLUDED__

#include <lolie/Stringp.h>
#include <lolie/LinkedList.h>

size_t strlen(const char* str);

#define IRC_BUFFER_LENGTH 512
#define IRC_FORMAT_BUFFER_LENGTH 512

typedef enum irc_message_prefix_type{
	IRC_MESSAGE_PREFIX_UNDETERMINED,
	IRC_MESSAGE_PREFIX_NONE,
	IRC_MESSAGE_PREFIX_USER,
	IRC_MESSAGE_PREFIX_SERVER,
}irc_message_prefix_type;

typedef enum irc_message_command_type_type{
	IRC_MESSAGE_COMMAND_TYPE_TYPE_UNDETERMINED,
	IRC_MESSAGE_COMMAND_TYPE_TYPE_ENUMERATOR,
	IRC_MESSAGE_COMMAND_TYPE_TYPE_NUMBER,
	IRC_MESSAGE_COMMAND_TYPE_TYPE_UNKNOWN,
}irc_message_command_type_type;

typedef enum irc_message_command_type{
	IRC_MESSAGE_COMMAND_TYPE_JOIN,
	IRC_MESSAGE_COMMAND_TYPE_PART,
	IRC_MESSAGE_COMMAND_TYPE_NICK,
	IRC_MESSAGE_COMMAND_TYPE_TOPIC,
	IRC_MESSAGE_COMMAND_TYPE_KICK,
	IRC_MESSAGE_COMMAND_TYPE_PRIVMSG,
	IRC_MESSAGE_COMMAND_TYPE_NOTICE,
	IRC_MESSAGE_COMMAND_TYPE_QUIT,
	IRC_MESSAGE_COMMAND_TYPE_SQUIT,
	IRC_MESSAGE_COMMAND_TYPE_MODE,

	IRC_MESSAGE_COMMAND_TYPE_PASS,
	IRC_MESSAGE_COMMAND_TYPE_USER,
	IRC_MESSAGE_COMMAND_TYPE_OPER,
	IRC_MESSAGE_COMMAND_TYPE_SERVICE,
	IRC_MESSAGE_COMMAND_TYPE_NAMES,
	IRC_MESSAGE_COMMAND_TYPE_LIST,
	IRC_MESSAGE_COMMAND_TYPE_INVITE,
	IRC_MESSAGE_COMMAND_TYPE_MOTD,
	IRC_MESSAGE_COMMAND_TYPE_LUSERS,
	IRC_MESSAGE_COMMAND_TYPE_VERSION,
	IRC_MESSAGE_COMMAND_TYPE_STATS,
	IRC_MESSAGE_COMMAND_TYPE_LINKS,
	IRC_MESSAGE_COMMAND_TYPE_TIME,
	IRC_MESSAGE_COMMAND_TYPE_CONNECT,
	IRC_MESSAGE_COMMAND_TYPE_TRACE,
	IRC_MESSAGE_COMMAND_TYPE_ADMIN,
	IRC_MESSAGE_COMMAND_TYPE_INFO,
	IRC_MESSAGE_COMMAND_TYPE_SERVLIST,
	IRC_MESSAGE_COMMAND_TYPE_SQUERY,

	IRC_MESSAGE_COMMAND_TYPE_WHO,
	IRC_MESSAGE_COMMAND_TYPE_WHOIS,
	IRC_MESSAGE_COMMAND_TYPE_WHOWAS,

	IRC_MESSAGE_COMMAND_TYPE_KILL,
	IRC_MESSAGE_COMMAND_TYPE_PING,
	IRC_MESSAGE_COMMAND_TYPE_PONG,
	IRC_MESSAGE_COMMAND_TYPE_ERROR,

	IRC_MESSAGE_COMMAND_TYPE_AWAY,
	IRC_MESSAGE_COMMAND_TYPE_REHASH,
	IRC_MESSAGE_COMMAND_TYPE_RESTART,
	IRC_MESSAGE_COMMAND_TYPE_SUMMON,
	IRC_MESSAGE_COMMAND_TYPE_WALLOPS,
	IRC_MESSAGE_COMMAND_TYPE_USERHOST,
	IRC_MESSAGE_COMMAND_TYPE_ISON,

}irc_message_command_type;

typedef struct irc_message{
	Stringcp raw_message;

	irc_message_prefix_type prefix_type;
	union{
		struct{
			Stringcp nickname;
			Stringcp username;
			Stringcp host;
		}user;

		struct{
			Stringcp name;
		}server;
	}prefix;

	irc_message_command_type_type command_type_type;
	union{
		irc_message_command_type enumerator;
		unsigned short number;
		Stringcp unknown;
	}command_type;
	union{
		struct{
			Stringcp target;
			Stringcp text;
		}privmsg;

		struct{
			LinkedList/*<Stringcp>*/* channels;
			LinkedList/*<Stringcp>*/* keys;
		}join;

		struct{
			LinkedList/*<Stringcp>*/* channels;
			Stringcp message;
		}part;

		struct{
			Stringcp channel;
			Stringcp text;
		}topic;

		struct{
			Stringcp target;
			Stringcp text;
		}notice;

		struct{
			LinkedList/*<Stringcp>*/* channels;
			LinkedList/*<Stringcp>*/* users;
			Stringcp comment;
		}kick;

		struct{
			Stringcp name;
		}nick;

		struct{
			Stringcp message;
		}quit;

		struct{
			Stringcp server;
			Stringcp comment;
		}squit;
		
		struct{
			Stringcp nickname;
			bool operation;
			char mode;
		}mode;

		struct{
			Stringcp nickname;
			Stringcp comment;
		}kill;

		struct{
			Stringcp text;
		}away;

		struct{
			Stringcp message;
		}error;

		struct{
			Stringcp from;
			Stringcp to;
		}ping;

		struct{
			Stringcp from;
			Stringcp to;
		}pong;

		struct{
			Stringcp params;
		}unknown;
	}command;
}irc_message;

/**
 * IRC Connection structure
 */
typedef struct irc_connection{
	int id;//Id of the connection, got from socket()
	char* read_buffer;//Read buffer, used in irc_read_message

	char* initial_channel;
	enum irc_connection_verbosity{
		IRCINTERFACE_VERBOSITY_SILENT,
		IRCINTERFACE_VERBOSITY_NORMAL,
		IRCINTERFACE_VERBOSITY_VERBOSE,
	}verbosity;
}irc_connection;

/**
 * Sends a raw message to the irc server
 *
 * @param id  Id of the connection
 * @param str String to be sent
 * @param len Length of Stringp to be sent
 */
void irc_send_raw(const irc_connection* connection,const char* str,size_t len);

/**
 * Sends a raw message to the irc server using a null terminated string
 *
 * @param id  Id of the connection
 * @param str String to be sent
 */
inline void irc_send_rawnt(const irc_connection* connection,const char* str){
	irc_send_raw(connection,str,strlen(str));
}
/**
 * Sends a formatted raw message to the irc server
 *
 * @param id     Id of the connection
 * @param format Equivalent to the printf formatting, see printf documentation
 * @param ...    Data 
 */
void irc_send_rawf(const irc_connection* connection,const char* format,...);

/**
 * Connects to a irc server by host and port
 *
 * @param host Hostname (address/ip) of the irc server
 * @param port Port for the hostname of the irc server
 */
bool irc_connect(const char* host,unsigned short port,irc_connection* out_connection);

/**
 * Sets nickname for irc connection
 *
 * @param id   Id of the connection
 * @param name Specified nickname
 */
inline void irc_set_nickname(const irc_connection* connection,const char* name){
	irc_send_rawf(connection,"NICK %s\r\n",name);
}

/**
 * Sets username for irc connection
 *
 * @param id   Id of the connection
 * @param name Specified username
 */
inline void irc_set_username(const irc_connection* connection,const char* username,const char* realname){
	irc_send_rawf(connection,"USER %s 0 0 :%s\r\n",username,realname?:username);
}

/**
 * Joins a channel in an irc connection
 *
 * @param id      Id of the connection
 * @param channel Channel name
 */
inline void irc_join_channel(const irc_connection* connection,const char* channel){
	irc_send_rawf(connection,"JOIN %s\r\n",channel);
}

/**
 * Part from a channel in an irc connection
 *
 * @param id      Id of the connection
 * @param channel Channel name
 */
inline void irc_part_channel(const irc_connection* connection,const char* channel){
	irc_send_rawf(connection,"PART %s\r\n",channel);
}

Stringcp irc_parse_message(const irc_connection* connection,Stringcp raw_message,irc_message* out);

void irc_send_message(const irc_connection* connection,Stringcp target,Stringcp message);

bool irc_read_message(const irc_connection* connection,void* user_data,void(*onMessageFunc)(const irc_connection* connection,const irc_message* message,void* user_data));

/**
 * Read data from the IRC connection and copy it to `out` string.
 * Behaves like the POSIX function `ssize_t read(int,void*,size_t)`
 */
ssize_t irc_read(const irc_connection* connection,Stringp out);

bool irc_disconnect(const irc_connection* connection);

#endif
