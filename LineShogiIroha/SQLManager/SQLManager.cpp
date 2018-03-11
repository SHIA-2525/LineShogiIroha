#include <add_include.h>
//--------------------------------
// Sender.h : ラインにメッセージをreply,pushする
// クラス作成後、reply,push選択、送信先選択
// 送信先に送るメッセージをAddしていく
// sendする、returnがhttpの結果
// date : 2018/03/05
//--------------------------------
#include "SQLManager.h"

#include <postgres_ext.h>
#include <libpq-fe.h>

SQLManager::SQLManager()
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	char      conninfo[256];
	PGconn    *conn = NULL;
	PGresult  *res;
	int       nFields;
	int       i;
	int       j;

	sprintf(conninfo, "host=%s port=%s dbname=%s user=%s password=%s"
		, "192.168.0.1"       //ホスト名
		, "5432"              //ポート 
		, "testdb"            //データベース名
		, "pgsql"             //接続ユーザ名
		, "pwd");             //パスワード

							  /* データベースとの接続を確立する */
	conn = PQconnectdb(conninfo);
}

SQLManager::~SQLManager()
{

}

int SQLManager::SetCommand(std::string cmd)
{


	return 0;
}


/*EOF*/
