
# Running tests locally

## MacOS installation

Install Kerberos with Homebrew, the default installation of Kerberos doesn't work.

Add the following information to your shell init file, eg `~/.zshrc`:

```
# If you need to have krb5 first in your PATH
export PATH="/opt/homebrew/opt/krb5/bin:$PATH"
export PATH="/opt/homebrew/opt/krb5/sbin:$PATH"
# For compilers to find krb5
export LDFLAGS="-L/opt/homebrew/opt/krb5/lib"
export CPPFLAGS="-I/opt/homebrew/opt/krb5/include"
#For pkg-config to find krb5 you may need to set:
export PKG_CONFIG_PATH="/opt/homebrew/opt/krb5/lib/pkgconfig"
```

## Kerberos client configuration

At the moment, test only check the file ticket cache. Note, on macOS, it default to `KCM:501`.

```ini
[logging]
 default = FILE:/var/log/kerberos/krb5libs.log
 kdc = FILE:/var/log/kerberos/krb5kdc.log
 admin_server = FILE:/var/log/kerberos/kadmind.log
[libdefaults]
 default_ccache_name = FILE:/tmp/krb5cc_%{uid}
 default_realm = KRB.LOCAL
 dns_lookup_realm = false
 dns_lookup_kdc = false
 ticket_lifetime = 24h
 renew_lifetime = 7d
 forwardable = true
 rdns = false
[realms]
 KRB.LOCAL = {
  kdc = localhost:8088
  admin_server = localhost:8749
 }
[domain_realm]
 .krb.local = KRB.LOCAL
 krb.local = KRB.LOCAL
```

## Host configuration

The spnego server written in python and running inside the `rest` container is configured with a keytab for the `rest/rest.krb.local@KRB.LOCAL` principal. The `/etc/hosts` file must be modified accordingly by adding `127.0.0.1 		rest.krb.local`.

# Docker command

Use Docker Compose start the Kerberos and REST container:

```bash
docker compose up -D kerberos
docker compose up -D rest
```

Here is the equivalent of the Docker Compose instruction for the Kerberos server container.

```bash
docker build -t node_krb5_kdc ./kdc
docker run -it --rm \
  -v /dev/urandom:/dev/random \
  -v /tmp/krb5_test:/tmp/krb5_test \
  -p 8088:88 -p 8749:749  \
  -h kdc.krb.local \
  -e BOOTSTRAP=1 \
  --name test_kdc \
  node_krb5_kdc
```

# Docker Compose command

```bash
docker compose run kerberos  
```
