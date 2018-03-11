#include <add_include.h>
//--------------------------------
// Sender.h : ラインにメッセージをreply,pushする
// クラス作成後、reply,push選択、送信先選択
// 送信先に送るメッセージをAddしていく
// sendする、returnがhttpの結果
// date : 2018/03/05
//--------------------------------
#include "CommandManager.h"

CommandManager* CommandManager::s_instance = NULL;

CommandManager* CommandManager::GetInstance()
{
	if (!s_instance) {
		s_instance = new CommandManager();
	}
	return s_instance;
}

CommandManager::CommandManager()
{

}

CommandManager::~CommandManager()
{

}

int CommandManager::SetCommand(std::string cmd)
{
	

	return 0;
}


/*EOF*/
