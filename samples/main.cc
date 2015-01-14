#include "../krb5.h"

#define USER "pierre"
#define REALM "HADOOP.ADALTAS.COM"
#define SERVER "hadoop.adaltas.com"
#define PASSWD "pierre123"

using namespace std;

int main()
{
  Krb5* k = new Krb5(USER,REALM);
  k->krb5_get_credentials_by_password(PASSWD);
  k->generate_spnego_token(SERVER);
  cout << "error code: " << k->err << endl;
  cout << "token : " << k->spnego_token << endl;
    return 0;
}
