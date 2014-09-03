#include "Commands.h"

#include <lolien/seq/DynamicArray.h>
#include <lolien/seq/Array.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "api/Command.h"

/* ///////////////////////////////////////////////////
 * Type of commands list:
 *   DynamicArray<LinkedList<struct Command*>*>
 */

bool initCommands(DynamicArray* commands){
	*commands=DynamicArray_init;

	//Use length for checking if capacity changes when using registerCommand()
	commands->length=commands->capacity;
	DynamicArray_FOREACH(*commands,list,DynamicArray){//Set all LinkedLists to the initial value
		*list=DynamicArray_init;
	}

	return commands->data!=NULL;
}

void freeCommands(DynamicArray* commands){
	DynamicArray_free(commands);
	//TODO: Free all the other resources
}

const struct Command* getCommand(const DynamicArray* commands,StringCP name){
	if(name.length>commands->capacity || DynamicArray_isEmpty(DynamicArray_GET(*commands,name.length,DynamicArray).constSeq))
		return NULL;

	DynamicArray_FOREACH(DynamicArray_GET(*commands,name.length,DynamicArray),elem,struct Command*){
//printf("Search command: %s\n",((struct Command*)node->ptr)->name.ptr);
		if(memcmp((*elem)->name.ptr,name.ptr,name.length)==0)
			return *elem;
	}
	return NULL;
}

bool registerCommand(DynamicArray* commands,const struct Command* command){
	if(!command || command->name.length==0)
		return false;

	if(command->name.length+1>commands->capacity){//If the array needs to be resized for the new command
		DynamicArray_resize(commands,command->name.length+1,sizeof(DynamicArray));

		Array_FOREACH(((ArrayP){.length=commands->capacity-commands->length,.data=commands->data + commands->length}),list,DynamicArray){
			//Set all new LinkedLists to the initial value
			*list = DynamicArray_init;
		}
		commands->length = commands->capacity;
	}
	DynamicArray_add(&DynamicArray_GET(*commands,command->name.length,DynamicArray),command,sizeof(struct Command*));
//printf("Size: %u, {%p,%p} -> %p\n",LinkedList_size(DynamicArray__get(*commands,command->name.length)),((LinkedList*)DynamicArray__get(*commands,command->name.length))->ptr,DynamicArray__get(*commands,command->name.length),((LinkedList*)DynamicArray__get(*commands,command->name.length))->next);

	return true;
}

bool registerCommandsFromArray(DynamicArray* commands,const struct Command* cmds,size_t count){
	if(!commands)
		return false;

	while(count-->0){
		if(cmds){
			if(cmds->name.length==0)
				return false;

			if(cmds->name.length+1>commands->capacity){//If the array needs to be resized for the new commands
				DynamicArray_resize(commands,cmds->name.length+1,sizeof(DynamicArray));

				Array_FOREACH(((ArrayP){.length=commands->capacity-commands->length,.data=commands->data + commands->length}),list,DynamicArray){
					//Set all new LinkedLists to the initial value
					*list = DynamicArray_init;
				}
				commands->length=commands->capacity;
			}
			DynamicArray_add(&DynamicArray_GET(*commands,cmds->name.length,DynamicArray),cmds,sizeof(struct Command*));
		}
		++cmds;
	}

	return true;
}

bool unregisterCommand(DynamicArray* commands,const struct Command* command){
	if(!command || command->name.length==0)
		return false;

	//For every command length list
	for(size_t i=0;i<commands->capacity;++i){
		printf("%lu Size: %lu\n",i,DynamicArray_length(DynamicArray_GET(*commands,i,DynamicArray).constSeq));
		if(DynamicArray_isEmpty(DynamicArray_GET(*commands,i,DynamicArray).constSeq))
			continue;

		DynamicArray_FOREACH(DynamicArray_GET(*commands,i,DynamicArray),node,struct Command*){
			printf("%p == %p\n",*node,command);
		}

		//If command is found and removed
		if(LinkedList_remove((LinkedList**)&DynamicArray_GET(*commands,i,DynamicArray),command))
			return true;
	}

	return false;
}

bool unregisterCommandByName(DynamicArray* commands,StringCP commandName){
	if(commandName.length>commands->capacity)
		return false;

	return LinkedList_removeFirst((LinkedList**)&DynamicArray__get(*commands,commandName.length),
		(bool(*)(void *))function(bool,(struct Command* command){
			return memcmp(command->name.ptr,commandName.ptr,commandName.length)==0;
		})
	);
}

void unregisterCommands(DynamicArray* commands){
	DynamicArray_FOREACH(*commands,list,DynamicArray){
		DynamicArray_removeAll(list);
	}
}
