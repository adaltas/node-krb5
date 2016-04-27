{
  "targets": [
    {
      "target_name": "krb5",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "defines" : ["NODEGYP"],
      "sources": ["src/base64.cc","src/krb5.cc","src/bind.cc","src/worker.cc","src/addon.cc"],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "conditions": [
        [
          "OS=='win'",
          {
            "variables": {
              "KRB_PATH": "/Program Files/MIT/Kerberos"
            },
            "include_dirs": ["<(KRB_PATH)/include","<(KRB_PATH)/include/gssapi","src"],
            "conditions": [
              [
                "target_arch=='x64'",
                {
                  "msvs_settings": {
                    "VCCLCompilerTool": {
                      "AdditionalOptions": [ "/MP /EHsc" ]
                    },
                    "VCLinkerTool": {
                      "AdditionalLibraryDirectories": ["<(KRB_PATH)/lib/amd64/"]

                    }
                  },
                  "libraries": ["-lkrb5_64.lib","-lgssapi64.lib"]
                }
              ],
              [
                "target_arch=='ia32'",
                {
                  "msvs_settings": {
                    "VCCLCompilerTool": {
                      "AdditionalOptions": [ "/MP /EHsc" ]
                    },
                    "VCLinkerTool": {
                      "AdditionalLibraryDirectories": ["<(KRB_PATH)/lib/amd64/"]

                    }
                  },
                  "libraries": ["-lkrb5_32.lib","-lgssapi32.lib"]
                }
              ] 
            ]
          }
        ],
        [
          "OS!='win'",
          {
            "libraries": ["-lkrb5", "-lgssapi_krb5"]
          }
        ]
      ]
    }
  ]
}
