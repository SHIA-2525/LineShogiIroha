#include <cstdio>

#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#include "picojson.h"

#define SOCK_PATH "/var/run/lsi/lsi.sock"

#define BUF_SIZE 65536
static char buf[BUF_SIZE] = { 0 };

int main()
{
	int r;
	int listen_fd = 0;
	struct sockaddr_un local, remote;

	signal(SIGPIPE, SIG_IGN);
	listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, SOCK_PATH);
	unlink(local.sun_path);
	r = bind(listen_fd, (struct sockaddr *)&local, sizeof(local));
	if (r) {
		perror("failed to bind");
	}
	//printf("binded.\n");

	listen(listen_fd, 100);

	for (;;) {
		socklen_t len = sizeof(remote);

		//printf("1.\n");
		int remote_fd = accept(listen_fd, (struct sockaddr *)&remote, &len);
		if (remote_fd < 0) {
			printf("failed to accept.\n");
			perror("failed to accept");
			return 0;
		}
		//printf("2.\n");

		ssize_t size = read(remote_fd, buf, BUF_SIZE);
		if (size > 0) {

			printf("read buf size %d.\n", size);
			printf("buf:\n");
			printf(buf);
			printf("\n");
			
			using pv = picojson::value;
			using po = picojson::object;
			using pa = picojson::array;
			
			char* pBuf = buf;
			pv val;
			std::string err = picojson::parse(val, pBuf, pBuf + strlen(pBuf));
			if (err == "") {
				// Ç±ÇÃêÊÇÕÉâÉCÉìÇ©ÇÁóàÇΩjsonÇêMóäÇµÇƒìÆçÏÇ≥ÇπÇÈ
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

					enum struct EVENT_TYPE : int {
						NONE = 0,
						MESSAGE,
						
						MAX,
					};

					EVENT_TYPE eventType = EVENT_TYPE::NONE;
					if (eventTypeStr == "message") {
						eventType = EVENT_TYPE::MESSAGE;
					}

					switch (eventType)
					{
					case EVENT_TYPE::MESSAGE:
						break;
					case EVENT_TYPE::NONE:
					default:
						break;
					}

					if (type == "message") {
						po mes = evt["message"].get<po>();
						std::string text = mes["text"].get<std::string>();
						std::cout << "test:" << text << std::endl;
					}
				}
			} else {
				std::cout << "json parse err:" << err << std::endl;
			}

		}
		//printf("3.\n");

		//if (errno) {
		//	printf("\n--\n err : %d %s\n--\n", errno, strerror(errno));
		//	perror("fail to send");
		//}

		//printf("close.\n");
		close(remote_fd);
	}
}

//int main() {
//	int fd;
//	struct sockaddr_un addr;
//	int ret;
//	struct sockaddr_un from;
//	int ok = 1;
//	int len;
//	socklen_t fromlen = sizeof(from);
//
//	if ((fd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
//		perror("socket");
//		ok = 0;
//	}
//
//	if (ok) {
//		memset(&addr, 0, sizeof(addr));
//		addr.sun_family = AF_UNIX;
//		strcpy(addr.sun_path, SOCK_PATH);
//		unlink(SOCK_PATH);
//		if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
//			perror("bind");
//			ok = 0;
//		}
//	}
//
//	while ((len = recvfrom(fd, buf, BUF_SIZE, 0, (struct sockaddr *)&from, &fromlen)) > 0) {
//		printf("recvfrom: %s\n", buf);
//		//strcpy(buf, "transmit good!");
//		//ret = sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr *)&from, fromlen);
//		//if (ret < 0) {
//		//	perror("sendto");
//		//	break;
//		//}
//	}
//
//
//	if (fd >= 0) {
//		close(fd);
//	}
//
//	return 0;
//}

//int main()
//{
//    printf("hello from LineShogiIroha!\n");
//	int ret = getchar();
//	printf("%c", ret);
//    return 0;
//}