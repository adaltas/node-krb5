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
            "libraries": ["-lkrb5.lib", "-lgssapi_krb5.lib"]
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