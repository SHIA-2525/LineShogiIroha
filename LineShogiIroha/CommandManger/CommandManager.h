//--------------------------------
// CommandManager.h : @～を解釈してよしなにする
// date : 2018/03/11
//--------------------------------
#ifndef __COMMAND_MANAGER_H_INCLUDED__
#define __COMMAND_MANAGER_H_INCLUDED__
#include <string>

class CommandManager
{
public:
	static CommandManager* GetInstance();
	
	int SetCommand(std::string cmd);

private:

	CommandManager();
	~CommandManager();

	enum struct CMD_TYPE : int {
		NONE = 0,
		NEW_THREAD,
	};

	static CommandManager* s_instance;
};

#endif
/*EOF*/
