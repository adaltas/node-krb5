#include "../krb5.h"

#define USER "user"
#define REALM "KERBEROS_REALM.MYDOMAIN.COM"
#define SERVER "server.mydomain.com"
#define PASSWD "password"

using namespace std;

int main()
{
  Krb5* k = new Krb5();
  k->init(USER,REALM);
  k->get_credentials_by_password(PASSWD);
  k->generate_spnego_token(SERVER);
  cout << "error code: " << k->err << endl;
  cout << "token : " << k->spnego_token << endl;
    return 0;
}
