{
    'variables': {
        
    },
    'includes': [
      '../gyp_build/common.gypi',
    ],
    'targets': [
      {
        'target_name': 'libsnet_test',
        'type': 'executable',
        'include_dirs': [
            '<(DEPTH)/include/',
        ],
        'sources': [
			'fast_udp_test.cpp'
        ],
		'dependencies': [
            '<(DEPTH)/src/libsnet.gyp:libsnet'
        ]
      },
    ],
}