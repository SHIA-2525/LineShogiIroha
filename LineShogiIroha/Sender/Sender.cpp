#include <add_include.h>
//--------------------------------
// Sender.h : ラインにメッセージをreply,pushする
// クラス作成後、reply,push選択、送信先選択
// 送信先に送るメッセージをAddしていく
// sendする、returnがhttpの結果
// date : 2018/03/05
//--------------------------------
#include "Sender.h"
#include "picojson.h"

// This file is not versioned.
// Copy from ChannelAccessToken.h.template
#include "ChannelAccessToken.h"

namespace
{
	using pv = picojson::value;
	using po = picojson::object;
	using pa = picojson::array;
}

Sender::Sender()
	: m_sendType(SEND_TYPE::NONE)
	, m_sendTarget("")
	, m_messageNr(0)
	, m_contents()
{
}

Sender::~Sender()
{
}

int Sender::SetSendType(SEND_TYPE sendType, std::string sendTarget)
{
	m_sendType = sendType;
	m_sendTarget = sendTarget;
	return 0;
}

int Sender::GetCurrentMessageNr()
{
	return m_messageNr;
}

int Sender::AddText(std::string text)
{
	if (m_messageNr >= MESSAGE_NR_MAX) { return -1; }
	
	int i = m_messageNr;

	m_contents[i].insert({ "type", pv("text") });
	m_contents[i].insert({ "text", pv(text) });

	m_messageNr++;
	return m_messageNr;
}

int Sender::AddImage(std::string origUrl, std::string previewUrl)
{
	if (m_messageNr >= MESSAGE_NR_MAX) { return -1; }

	int i = m_messageNr;

	m_contents[i].insert({ "type", pv("image") });
	m_contents[i].insert({ "originalContentUrl", pv(origUrl) });
	m_contents[i].insert({ "previewImageUrl", pv(previewUrl) });

	m_messageNr++;
	return m_messageNr;
}

int Sender::Send()
{
	std::string typeStr = "";
	switch (m_sendType)
	{
	case Sender::SEND_TYPE::REPLY:	typeStr = "replyToken";	break;
	case Sender::SEND_TYPE::PUSH:	typeStr = "to";			break;
	case Sender::SEND_TYPE::NONE:
	default:
		break;
	}
	if (typeStr == "") { return -1; }

	po obj;
	obj.insert({ typeStr, pv(m_sendTarget) });

	pa ary;
	for (int i = 0; i < m_messageNr; i++) {
		ary.push_back(pv(m_contents[i]));
	}
	obj.insert({ "messages", pv(ary) });

	return 0;
}

/*EOF*/
