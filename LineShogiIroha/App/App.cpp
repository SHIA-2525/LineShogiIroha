#include <add_include.h>
//--------------------------------
// App.cpp : メインから呼ばれる
// ラインからのjsonをunixドメインソケット経由で受信
// パースして、それぞれの挙動ごとにクラスへ渡す
// jsonのパース等はこの中で全部処理して、必要なデータを構造体で渡す
//
// date : 2018/03/04
//--------------------------------
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#include "App.h"
#include "picojson.h"
#include "../Sender/Sender.h"

namespace
{
	using pv = picojson::value;
	using po = picojson::object;
	using pa = picojson::array;

	#define SOCK_PATH "/var/run/lsi/lsi.sock"
	#define BUF_SIZE (8192+1)
	static char s_sockReadBuf[BUF_SIZE] = { 0 };

	enum struct EVENT_TYPE : int {
		NONE = 0,
		MESSAGE,	// テキスト、画像、スタンプ等
		FOLLOW,		// 友達追加、ブロック解除
		UNFOLLOW,	// ブロック
		JOIN,		// グループorトークルームへの参加
		LEAVE,		// グループからBOTが削除(トークルームからの削除は発行されない,自身で抜けた場合は発行されない)
		POSTBACK,	// ユーザーが、ポストバックを開始するアクションを実行
		BEACON,		// LINE Beaconデバイスの受信圏をユーザーが出入り
		MAX,
	};

	enum struct MESSAGE_TYPE : int {
		NONE = 0,
		TEXT,
		IMAGE,
		VIDEO,
		AUDIO,
		FILE,
		LOCATION,
		STICKER,
	};
}


App::App()
	: m_bInit(false)
	, m_bExitReservation(false)
	, m_listen_fd(0)
	, m_local()
	, m_remote()
{
}

int App::Init()
{
	int ret = 0;
	m_listen_fd = 0;

	signal(SIGPIPE, SIG_IGN);
	m_listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	m_local.sun_family = AF_UNIX;
	strcpy(m_local.sun_path, SOCK_PATH);
	
	unlink(m_local.sun_path);
	ret = bind(m_listen_fd, (struct sockaddr *)&m_local, sizeof(m_local));
	if (ret) {
		perror("failed to bind");
		return ret;
	}
	DEBUG_PRINTF("binded.\n");

	listen(m_listen_fd, 100);

	m_bInit = true;

	return ret;
}

int App::Finish()
{
	if (m_bInit) {
		unlink(m_local.sun_path);
		m_bInit = false;
	}
	return 0;
}

int App::Main()
{
	if (!m_bInit) { return 1; }

	int ret = 0;
	while (!m_bExitReservation) {
		socklen_t len = sizeof(m_remote);

		DEBUG_PRINTF("before accept.\n");
		int remote_fd = accept(m_listen_fd, (struct sockaddr *)&m_remote, &len);
		if (remote_fd < 0) {
			printf("failed to accept.\n");
			perror("failed to accept");
			return 0;
		}
		DEBUG_PRINTF("accepted.\n");

		memset(s_sockReadBuf, 0, BUF_SIZE);
		ssize_t size = read(remote_fd, s_sockReadBuf, BUF_SIZE);
		if (size > 0) {

			//DEBUG_PRINTF("read buf size %d.\n", size);
			DEBUG_PRINTF("buf:\n");
			DEBUG_PRINTF(s_sockReadBuf);
			DEBUG_PRINTF("\n\n");

			// PC内から終了を命令
			if (strcmp(s_sockReadBuf, "stop\n") == 0) {
				DEBUG_PRINTF("recv cmd:stop");
				m_bExitReservation = true;
				continue;
			}

			char* pBuf = s_sockReadBuf;
			pv val;
			std::string err = picojson::parse(val, pBuf, pBuf + strlen(pBuf));
			if (err == "") {
				// この先はラインから来たjsonを信頼して動作させる
				// ソケットから来る前に署名確認済み
				po root = val.get<po>();
				pa events = root["events"].get<pa>();
				for (int i = 0; i < events.size(); i++) {
					po evt = events[i].get<po>();

					std::string replyToken = evt["replyToken"].get<std::string>();
					std::string eventTypeStr = evt["type"].get<std::string>();
					double timestamp = evt["timestamp"].get<double>();

					po src = evt["source"].get<po>();
					std::string sourceType = src["type"].get<std::string>();
					std::string userId = "";
					std::string groupId = "";
					std::string roomId = "";
					if (src["userId"].is<std::string>()) {
						userId = src["userId"].get<std::string>();
					}
					if (src["groupId"].is<std::string>()) {
						groupId = src["groupId"].get<std::string>();
					}
					if (src["roomId"].is<std::string>()) {
						roomId = src["roomId"].get<std::string>();
					}


					EVENT_TYPE eventType = EVENT_TYPE::NONE;
					if (eventTypeStr == "message") { eventType = EVENT_TYPE::MESSAGE; } else if (eventTypeStr == "follow") { eventType = EVENT_TYPE::FOLLOW; } else if (eventTypeStr == "unfollow") { eventType = EVENT_TYPE::UNFOLLOW; } else if (eventTypeStr == "join") { eventType = EVENT_TYPE::JOIN; } else if (eventTypeStr == "leave") { eventType = EVENT_TYPE::LEAVE; } else if (eventTypeStr == "postback") { eventType = EVENT_TYPE::POSTBACK; } else if (eventTypeStr == "follow") { eventType = EVENT_TYPE::BEACON; }

					switch (eventType)
					{
					case EVENT_TYPE::MESSAGE:
					{
						po mes = evt["message"].get<po>();
						std::string id = mes["id"].get<std::string>();
						std::string mesTypeStr = mes["type"].get<std::string>();

						MESSAGE_TYPE mesType = MESSAGE_TYPE::NONE;
						if (mesTypeStr == "text") { mesType = MESSAGE_TYPE::TEXT; } else if (mesTypeStr == "image") { mesType = MESSAGE_TYPE::IMAGE; } else if (mesTypeStr == "video") { mesType = MESSAGE_TYPE::VIDEO; } else if (mesTypeStr == "audio") { mesType = MESSAGE_TYPE::AUDIO; } else if (mesTypeStr == "file") { mesType = MESSAGE_TYPE::FILE; } else if (mesTypeStr == "location") { mesType = MESSAGE_TYPE::LOCATION; } else if (mesTypeStr == "sticker") { mesType = MESSAGE_TYPE::STICKER; }

						std::cout << "message type:" << mesTypeStr << std::endl;

						switch (mesType)
						{
						case MESSAGE_TYPE::TEXT:
						{
							std::string text = mes["text"].get<std::string>();
							std::cout << "test:" << text << std::endl;

							Sender sender;
							sender.SetSendType(Sender::SEND_TYPE::REPLY, replyToken);
							sender.AddText((text + "ぞい"));
							int httpRet = sender.Send();

							Sender sender2;
							sender2.SetSendType(Sender::SEND_TYPE::PUSH, userId);
							sender2.AddText("これもテストですぞ");
							httpRet = sender2.Send();
						}
						break;

						// テキスト以外は今んとこ何もしない
						case MESSAGE_TYPE::NONE:
						case MESSAGE_TYPE::IMAGE:
						case MESSAGE_TYPE::VIDEO:
						case MESSAGE_TYPE::AUDIO:
						case MESSAGE_TYPE::FILE:
						case MESSAGE_TYPE::LOCATION:
						case MESSAGE_TYPE::STICKER:
						default:
							break;
						}
					}
					break;

					case EVENT_TYPE::NONE:
					case EVENT_TYPE::FOLLOW:
					case EVENT_TYPE::UNFOLLOW:
					case EVENT_TYPE::JOIN:
					case EVENT_TYPE::LEAVE:
					case EVENT_TYPE::POSTBACK:
					case EVENT_TYPE::BEACON:
					default:
						break;
					}
				}
			} else {
				std::cout << "json parse err:" << err << std::endl;
			}

		}

		DEBUG_PRINTF("end read.\n");
		//if (errno) { // 相手側からクローズされるとエラーはくんで無視
		//	printf("\n--\n err : %d %s\n--\n", errno, strerror(errno));
		//	perror("fail to send");
		//}

		DEBUG_PRINTF("close.\n");
		close(remote_fd);
	}
	return ret;
}

/*EOF*/
