import os
import http.server
import base64
import socket
import gssapi

FQDN = socket.getfqdn()
print('FQDN: %s' % FQDN)

princ_name = "HTTP/%s@%s" % (FQDN, 'KRB.LOCAL')
server_name = gssapi.Name(
    'HTTP@' + FQDN, name_type=gssapi.NameType.hostbased_service)
server_creds = gssapi.Credentials(usage='accept', name=server_name)
print("Using creds with name '%s'" % str(server_name))


class ReqHandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        print("Processing request")
        auth_header = self.headers.get('Authorization', None)
        if auth_header is None or not auth_header.startswith('Negotiate '):
            print("Missing auth: '%s'" % auth_header)
            self.send_response(401)
            self.send_header('WWW-Authenticate', 'Negotiate ')
        else:
            tok = base64.b64decode(auth_header[len('Negotiate '):])
            ctx = gssapi.SecurityContext(creds=server_creds, usage='accept')
            server_tok = ctx.step(tok)
            b64tok = base64.b64encode(server_tok)
            print('Done: %s' % ctx.complete)
            if ctx.complete:
                self.send_response(200)
            else:
                self.send_response(401)

            self.send_header('WWW-Authenticate',
                             'Negotiate ' + b64tok.decode('ascii'))

        self.end_headers()


server = http.server.HTTPServer(('0.0.0.0', 8080), ReqHandler)
server.serve_forever()
