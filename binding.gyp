{
  'targets': [
    {
      'target_name': 'krb_spnego',      
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'include_dirs': ['/usr/include/krb5','/usr/include/gssapi' ],
      "libraries": ['-lkrb5', '-lgssapi_krb5'],
      'sources': ['krb_spnego.cc','base64.cc'],
    }
  ]
}
