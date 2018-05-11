{
    "targets": [{
        "target_name": "krb5",
	  "sources": [
            "./src/module.cc",
            "./src/krb5_bind.cc",
        ],
	  'cflags!': [ '-fno-exceptions' ],
	  'cflags_cc!': [ '-fno-exceptions' ],
        'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
        'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
        'cflags!': [ '-fno-exceptions' ],
        'cflags_cc!': [ '-fno-exceptions' ]
    }]
}
