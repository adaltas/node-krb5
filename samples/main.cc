#include "../krb5_spnego.h"

using namespace std;

// TO COMPILE THIS SAMPLE,
//UNDEFINE NODEJS IN krb5_spnego.h

int main()
{
  string user = "pierre";
  string realm = "HADOOP.ADALTAS.COM";
  string server = "hadoop.adaltas.com";
  string password = "";
  Krb5Spnego* k = new Krb5Spnego(user,realm);
  k->krb5_get_credentials_by_password(password);
  k->generate_spnego_token(server);
  cout << "token : " << k->getSpnegoToken() << endl;
    return 0;
}
