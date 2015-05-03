{
    'variables': {
		'library' : 'static_library',
    }, # end variables
	
    'target_defaults': {
        'conditions': [
            ['OS=="ios"', { # iOS platform
                'defines': ['__PLATFORM_IOS', '__iOS'],
                'xcode_settings': {
                    #'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
                    #'GCC_VERSION': 'com.apple.compilers.llvmgcc42',
                    'SDKROOT': 'iphoneos',
                    'ARCHS': '$(ARCHS_STANDARD_32_64_BIT)',
                    # For iPhone, use 1
                    # For iPad, use 2
                    # For both, use 1,2. Default is 1, iphone
                    'TARGETED_DEVICE_FAMILY': '1,2',
                    'IPHONEOS_DEPLOYMENT_TARGET': '7.0',
                    'CODE_SIGN_IDENTITY': 'iPhone Developer',
                    
                    #'CLANG_CXX_LANGUAGE_STANDARD': 'gnu++0x',
                    #'CLANG_CXX_LIBRARY': 'libc++',
                    #'GCC_C_LANGUAGE_STANDARD': 'gnu99',
                    #'ONLY_ACTIVE_ARCH': 'YES',
                },
            }],
            ['OS=="android"', { # android platform
                'defines': ['__PLATFORM_ANDROID', '__android',
                    # not sure the usage
                    #'os_posix=1',
                    #'target_arch=arm',
                    #'clang=0',
                    #'component=static_library',
                    #'use_system_stlport=0',
                ],
            }],
            ['OS=="linux"', {
                'defines': ['PLATFORM_OS_LINUX', 'PLATFORM_USE_PTHREADS', 'PLATFORM_USE_POSIX'],
                'cflags': ['-Wall']
            }],
            ['OS=="win"', {
                'defines': ['PLATFORM_OS_WINDOWS', '__win', 'WIN32'],
                'msvs_configuration_attributes': {
                    # 1 for unicode, 2 for multi-byte.
                    'CharacterSet': '1',
					'OutputDirectory': '$(SolutionDir)$(ConfigurationName)',
					'IntermediateDirectory': '$(OutDir)/obj/$(ProjectName)',
                },
                'msvs_settings': {
                    'VCCLCompilerTool': {
                        'WarningLevel': '3',
                        'Detect64BitPortabilityProblems': 'true'
                    }
                },
				'msvs_disabled_warnings': [
					4201,
				]
            }],
            ['OS=="mac"', {
                'defines': ['__PLATFORM_MAC', '__mac'],
                'cflags': ['-Wall']
            }]
        ], # end conditions OS

        'default_configuration': 'Debug',
        'configurations': {
            'Debug': {
                'defines': ['_DEBUG'],

                'conditions': [
                    ['OS=="linux" or OS=="freebsd" or OS=="openbsd"', {
                        'cflags': ['-g']
                    }],
                    ['OS=="win"', {
                        'msvs_settings': {
                            'VCCLCompilerTool': {
                                'Optimization': '0',
                                'MinimalRebuild': 'true',
                                'BasicRuntimeChecks': '3',
                                'DebugInformationFormat': '4',

                                'conditions': [
                                    ['library=="shared_library"', {
                                        'RuntimeLibrary': '3'  # /MDd
                                    }, {
                                        'RuntimeLibrary': '1'  # /MTd
                                    }]
                                ]
                            },
                            'VCLinkerTool': {
                                'GenerateDebugInformation': 'true',
                                'LinkIncremental': '2'
                            }
                        } # end msvs_settings
                    }],
                    ['OS=="mac" or OS=="ios"', {
                        'xcode_settings': {
                            'GCC_GENERATE_DEBUGGING_SYMBOLS': 'YES'
                        }
                    }]
                ]
            }, # end Debug

            'Release': {
                'conditions': [
                    ['OS=="linux" or OS=="freebsd" or OS=="openbsd"', {
                        'cflags': ['-O3']
                    }],
                    ['OS=="win"', {
                        'msvs_settings': {
                            'VCCLCompilerTool': {
                                'Optimization': '2',

                                'conditions': [
                                    ['library=="shared_library"', {
                                        'RuntimeLibrary': '2'  # /MD
                                    }, {
                                        'RuntimeLibrary': '0'  # /MT
                                    }]
                                ]
                            }
                        } # end msvs_settings
                    }],
                    ['OS=="mac" or OS=="ios"', {
                        'xcode_settings': {
                            'GCC_GENERATE_DEBUGGING_SYMBOLS': 'NO',
                            'GCC_OPTIMIZATION_LEVEL': '3',

                            # -fstrict-aliasing. Mainline gcc enables
                            # this at -O2 and above, but Apple gcc does
                            # not unless it is specified explicitly.
                            'GCC_STRICT_ALIASING': 'YES'
                        }
                    }]
                ]
            } # end Release
        } # end configurations
    }, # end target_defaults
}