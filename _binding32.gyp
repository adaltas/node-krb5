{
  "targets": [
    {
      "target_name": "krb5",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "defines" : ["NODEGYP"],
      "sources": ["src/krb5.cc","src/base64.cc"],
      "conditions": [
        [
          "OS=='mac'",
          {
            "libraries": ["-lkrb5", "-lgssapi_krb5"]
          }
        ],
        [
          "OS=='win'",
          {
            "variables": {
              "KRB_PATH": "/Program Files/MIT/Kerberos"
            },
            "include_dirs": ["<(KRB_PATH)/include","<(KRB_PATH)/include/gssapi","src"],
            "msvs_settings": {
              "VCCLCompilerTool": {
                "AdditionalOptions": [ "/MP /EHsc" ]
              },
              'VCLinkerTool': {
                'AdditionalLibraryDirectories': ['/Program Files/Microsoft SDKs/Windows/v7.1/Lib', '<(KRB_PATH)/lib/i386/']
              }
            },
            "libraries": ["-lkrb5_32.lib","-lgssapi32.lib"],
          }
        ],
        [
          "OS=='linux'",
          {
            "libraries": ["-lkrb5", "-lgssapi_krb5"]
          }
        ]
      ]
    }
  ]
}