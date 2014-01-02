#include "Plugin.h"

#include <stdlib.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include "IRCBot.h"

bool Plugin_loadAll(struct IRCBot* bot,const char* directoryPath){
	DIR* directory;
	struct dirent* dir;

	//Open directory and initialize `directory`. If specified path cannot be opened as a directory: return false
	if(!(directory=opendir(directoryPath)))
		return false;

	//Length of `directoryPath` string
	size_t directoryPathLen=strlen(directoryPath);

	//File path string buffer
	char filePath[directoryPathLen+sizeof(((struct dirent*)0)->d_name)/sizeof(char)+2];

	//Copy `directoryPath` to `filePath` string buffer
	strcpy(filePath,directoryPath);

	//Append ´/´ path separator if `directoryPath` is specified and there isn't already a file separator in `directoryPath`
	if(directoryPathLen>0 && filePath[directoryPathLen-1]!='/')
		filePath[directoryPathLen++]='/';

	//Set pointer to the filename in the file path
	char* filename=filePath+directoryPathLen;

	//For each file in the directory
	while((dir=readdir(directory))){
		//If file doesn't start with "mod_", don't use this file
		if(memcmp("mod_",dir->d_name,4)!=0)
			continue;

		//Copy filename to `filePath` string buffer
		strcpy(filename,dir->d_name);

		//Load plugin by file path
		Plugin_load(bot,filePath);
	}

	//Free resources
	closedir(directory);

	return true;
}

struct Plugin* Plugin_load(struct IRCBot* bot,const char* filename){
	printf("Modules: Loading `%s`\n",filename);
	
	//Allocate memory to structure
	struct Plugin* plugin = malloc(sizeof(struct Plugin));

	//Open file, initialising plugin
	if(!(plugin->lib = dlopen(filename,RTLD_LAZY))){
		fprintf(stderr,"Modules: Error: %s\n",dlerror());
		goto Error;
	}

	//Load the required fields from the plugin
	if(!(plugin->constants.version = dlsym(plugin->lib,"plugin_version"))){
		fprintf(stderr,"Modules: Error: %s\n",dlerror());
		goto Error;
	}

	if(!(plugin->constants.author = dlsym(plugin->lib,"plugin_author"))){
		fprintf(stderr,"Modules: Error: %s\n",dlerror());
		goto Error;
	}

	//Load the optional fields from the plugin
	plugin->functions.onLoad   = dlsym(plugin->lib,"plugin_onLoad");
	plugin->functions.onUnload = dlsym(plugin->lib,"plugin_onUnload");

	//Push the plugin into the plugin list
	LinkedList_push(&bot->plugins,plugin);

	//Call onLoad function from plugin if any
	if(plugin->functions.onLoad)
		if(!plugin->functions.onLoad(bot)){
			fputs("Modules: Error: onLoad function failed\n",stderr);
			goto Error;
		}

	return plugin;

	//In case of error, label for gotos
	Error:
	free(plugin);
	return NULL;
}

bool Plugin_unload(struct IRCBot* bot,struct Plugin* plugin){
	if(!plugin->functions.onUnload(bot))
		fputs("Modules: Warning: onUnload function failed\n",stderr);

	//Remove from plugin list
	LinkedList_remove(&bot->plugins,plugin);

	//Close dynamic library
	dlclose(plugin->lib);

	//Free structure
	free(plugin);
	
	return true;
}
