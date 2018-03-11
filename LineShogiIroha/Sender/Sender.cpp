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

#include <curl/curl.h>

#define LINE_REPLY_URL "https://api.line.me/v2/bot/message/reply"
#define LINE_PUSH_URL  "https://api.line.me/v2/bot/message/push"

// This file is not versioned.
// Copy from ChannelAccessToken.h.template
#include "ChannelAccessToken.h"

namespace
{
	using pv = picojson::value;
	using po = picojson::object;
	using pa = picojson::array;

	size_t curl_callback(char *ptr, size_t size, size_t nmemb, void *stream) {
	}
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
	std::string sendJsonStr = pv(obj).serialize();
	
	CURL *curl;
	CURLcode curlRet;
	int wr_error = 0;

	curl = curl_easy_init();
	if (curl == NULL) {
		DEBUG_PRINTF("curl init failed.\n");
		return -2;
	}

	curl_easy_setopt(curl, CURLOPT_URL, (m_sendType == SEND_TYPE::REPLY) ? LINE_REPLY_URL : LINE_PUSH_URL);
	//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

	struct curl_slist* headers = NULL;
	headers = curl_slist_append(headers, "Content-Type:application/json; charser=UTF-8");
	headers = curl_slist_append(headers, "Authorization: Bearer {" CHANNEL_ACCESS_TOKEN "}");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sendJsonStr.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(sendJsonStr.c_str()));

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);

	curlRet = curl_easy_perform(curl);
	
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	curl_easy_cleanup(curl);
	
	if (http_code == 200 && curlRet != CURLE_ABORTED_BY_CALLBACK) {
		return http_code;
	} else {
		return -curlRet;
	}

	return 0;
}

/*EOF*/
