//--------------------------------
// SQLManager.h : @～を解釈してよしなにする
// date : 2018/03/11
//--------------------------------
#ifndef __SQL_MANAGER_H_INCLUDED__
#define __SQL_MANAGER_H_INCLUDED__
#include <string>

class SQLManager
{
public:
	SQLManager();
	~SQLManager();

	int SetCommand(std::string cmd);

private:

};

#endif /*__SQL_MANAGER_H_INCLUDED__*/
/*EOF*/
