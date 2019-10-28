{
    "targets": [{
        "target_name": "krb5",
        "sources": [
            "./src/module.cc",
            "./src/krb5_bind.cc",
            "./src/gss_bind.cc",
            "./src/base64.cc"
        ],
        'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
        'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
        'cflags!': ['-fno-exceptions'],
        'cflags_cc!': ['-fno-exceptions'],
        "conditions": [
            [
                "OS=='win'",
                {
                    "variables": {
                        "KRB_PATH": "/Program Files/MIT/Kerberos"
                    },
                    "include_dirs": ["<(KRB_PATH)/include", "<(KRB_PATH)/include/gssapi", "src"],
                    "conditions": [
                        [
                            "target_arch=='x64'",
                            {
                                "msvs_settings": {
                                    "VCCLCompilerTool": {
                                        "AdditionalOptions": ["/MP /EHsc"]
                                    },
                                    "VCLinkerTool": {
                                        "AdditionalLibraryDirectories": ["<(KRB_PATH)/lib/amd64/"]

                                    }
                                },
                                "libraries": ["-lkrb5_64.lib", "-lgssapi64.lib"]
                            }
                        ],
                        [
                            "target_arch=='ia32'",
                            {
                                "msvs_settings": {
                                    "VCCLCompilerTool": {
                                        "AdditionalOptions": ["/MP /EHsc"]
                                    },
                                    "VCLinkerTool": {
                                        "AdditionalLibraryDirectories": ["<(KRB_PATH)/lib/amd64/"]

                                    }
                                },
                                "libraries": ["-lkrb5_32.lib", "-lgssapi32.lib"]
                            }
                        ]
                    ]
                }
            ],
            [
                'OS=="mac"', {
                    'cflags+': ['-fvisibility=hidden'],
                    'xcode_settings': {
                        'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                        'CLANG_CXX_LIBRARY': 'libc++',
                        'MACOSX_DEPLOYMENT_TARGET': '10.7',
                    }
                }
            ],
            [
                "OS!='win'",
                {
                    "libraries": ["-lkrb5", "-lgssapi_krb5"]
                }
            ],
            [
                "OS=='zos'",
                {
                    'cflags': ['-qASCII', '-qNOOPT'],
                    'cflags_cc': ['-qASCII', '-qNOOPT'],
                    'cxxflags_cc': ['-qASCII', '-qNOOPT'],
                    'include_dirs': [
                        '$(KRB5_HOME)/include/',
                        '$(KRB5_HOME)/include/gssapi/'
                    ],
                    'libraries': [
                        '$(KRB5_HOME)/lib/libgssrpc.a',
                        '$(KRB5_HOME)/lib/libgssapi_krb5.a',
                        '$(KRB5_HOME)/lib/libkrb5.a',
                        '$(KRB5_HOME)/lib/libk5crypto.a',
                        '$(KRB5_HOME)/lib/libcom_err.a',
                        '$(KRB5_HOME)/lib/libkrb5support.a'
                    ],
                    "libraries!": ["-lkrb5", "-lgssapi_krb5"]
                }
            ],
        ]
    }]
}
