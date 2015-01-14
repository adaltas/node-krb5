{
  'targets': [
    {
      'target_name': 'krb5',
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'defines' : ['NODEGYP'],
      'libraries': ['-lkrb5', '-lgssapi_krb5'],
      'sources': ['krb5.cc','base64.cc'],
    }
  ]
}
