//--------------------------------
// Sender.h : ラインにメッセージをreply,pushする
// date : 2018/03/05
//--------------------------------
#ifndef __SENDER_H_INCLUDED__
#define __SENDER_H_INCLUDED__
#include <string>
#include "picojson.h"

class Sender
{
public:
	enum struct SEND_TYPE : int {
		NONE = 0,
		REPLY,
		PUSH,
	};
	
	Sender();
	~Sender();

	int SetSendType(SEND_TYPE sendType, std::string sendTarget);

	static const int MESSAGE_NR_MAX = 5;
	int GetCurrentMessageNr();

	int AddText(std::string text);
	int AddImage(std::string origUrl, std::string previewUrl);

	int Send();

private:
	SEND_TYPE	m_sendType;
	std::string m_sendTarget;

	int				 m_messageNr;
	picojson::object m_contents[MESSAGE_NR_MAX];
};

#endif // !__SENDER_H_INCLUDED__
/*EOF*/
