//--------------------------------
// App.h : ÉÅÉCÉìÇ©ÇÁåƒÇŒÇÍÇÈ
// date : 2018/03/04
//--------------------------------
#ifndef __APP_H_INCLUDED__
#define __APP_H_INCLUDED__

#include <sys/un.h>

class App {
public:
	App();
	~App() {}

	int Init();
	int Finish();

	int Main();

private:
	bool		m_bInit;
	bool		m_bExitReservation;

	int			m_listen_fd;
	sockaddr_un m_local, m_remote;

};

#endif /*__APP_H_INCLUDED__*/
/*EOF*/
