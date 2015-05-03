{
    'variables': {
        
    },
    'includes': [
      '../gyp_build/common.gypi',
    ],
    'targets': [
      {
        'target_name': 'libsnet',
        'type': '<(library)',
        'include_dirs': [
            '../include',
            './',
            './include/'
        ],
        'sources': [
			'../include/assertions.h',
            '../include/common.h',
			'../include/eventloop.h',
			'../include/ipaddress.h',
			'../include/sigslot.h',
			'../include/socket.h',
			'../include/socketaddress.h',
			'include/mutex.h',
			'include/thread.h',
			'assertions.cpp',
			'ipaddress.cpp',
			'mutex.cpp',
			'queue.h',
			'request.h',
			'socket-inl.h',
			'socket-inl.cpp',
			'socket.cpp',
			'socketaddress.cpp',
			'thread.cpp',
			'stringutils.h',
			'eventloop.cpp'
        ],
        'conditions': [
            ['OS=="win"', {
				'include_dirs' : [
					'win/'
				], 
				'sources' : [
					'win/error-inl.cpp',
					'win/error-inl.h',
					'win/eventloopadapter.cpp',
					'win/eventloopadapter.h',
					'win/iocp-poll.cpp',
					'win/iocp-poll.h',
					'win/request-win.h',
					'win/socket-tcp.cpp',
					'win/socket-tcp.h',
					'win/socket-udp.cpp',
					'win/socket-udp.h',
					'win/win32helper.cpp',
					'win/win32helper.h',
					'win/windowheaders.h',
				],
				'link_settings': {
					'libraries': [
						'-ladvapi32.lib',
						'-liphlpapi.lib',
						'-lpsapi.lib',
						'-lws2_32.lib',
					],
				}
			}], # end win32
			['OS=="linux"', {
				'include_dirs' : [
					'linux/'
				], 
				'sources' : [
					'linux/eventloopadapter.cpp',
					'linux/eventloopadapter.h',
					'linux/io-poll.h',
					'linux/io-poll.cpp',
					'linux/linux-epoll.h',
					'linux/linux-epoll.cpp',
					'linux/request-linux.h',
					'linux/socket-udp.cpp',
					'linux/socket-udp.h',
					'linux/socket-tcp.cpp',
					'linux/socket-tcp.h'
				],
				'link_settings' : {
					'libraries': [
						'-lpthread'
					]
				}
			}]
        ]
      },
    ],
}