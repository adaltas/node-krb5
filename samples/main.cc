#include "../krb5.h"

using namespace std;

int main()
{
  string user = "pierre";
  string realm = "HADOOP.ADALTAS.COM";
  string server = "hadoop.adaltas.com";
  string password = "";
  Krb5* k = new Krb5(user,realm);
  k->krb5_get_credentials_by_password(password);
  k->generate_spnego_token(server);
  cout << "token : " << k->getSpnegoToken() << endl;
    return 0;
}
